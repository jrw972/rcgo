#include "runtime.hpp"
#include "util.hpp"
#include "debug.hpp"
#include "instance.hpp"
#include "type.hpp"
#include <string.h>
#include <pthread.h>
#include "field.hpp"
#include "ast.hpp"
#include "action.hpp"
#include "stack_frame.hpp"
#include "symbol.hpp"
#include "trigger.hpp"
#include "method.hpp"
#include "semantic.hpp"
#include "heap.hpp"
#include <error.h>
#include "function.hpp"

struct instance_record_t {
  // Scheduling lock.
  pthread_rwlock_t lock;
  instance_t* instance;
  heap_t* heap;
  // Next instance on the schedule.
  // 0 means this instance is not on the schedule.
  // 1 means this instance is the end of the schedule.
  // ? means this instance is on the schedule.
  instance_record_t* next;
};

static instance_record_t* instance_record_make (void* ptr, instance_t* instance)
{
  instance_record_t* record = (instance_record_t*)malloc (sizeof (instance_record_t));
  memset (record, 0, sizeof (instance_record_t));
  pthread_rwlock_init (&record->lock, NULL);
  record->instance = instance;
  record->heap = heap_make ((char*)ptr, (char*)ptr + instance->type ()->size ());

  // Link the instance to its scheduling information.
  *((instance_record_t**)ptr) = record;

  instance->record = record;

  return record;
}

static void* instance_record_get_ptr (const instance_record_t* record)
{
  return heap_instance (record->heap);
}

static void instance_record_lock (const instance_set_t& set)
{
  for (instance_set_t::const_iterator pos = set.begin (), limit = set.end ();
       pos != limit;
       ++pos)
    {
      instance_record_t* record = pos->first->record;
      switch (pos->second)
        {
        case TRIGGER_READ:
          pthread_rwlock_rdlock (&record->lock);
          break;
        case TRIGGER_WRITE:
          pthread_rwlock_wrlock (&record->lock);
          break;
        }
    }
}

static void instance_record_unlock (const instance_set_t& set)
{
  for (instance_set_t::const_iterator pos = set.begin (), limit = set.end ();
       pos != limit;
       ++pos)
    {
      instance_record_t* record = pos->first->record;
      switch (pos->second)
        {
        case TRIGGER_READ:
        case TRIGGER_WRITE:
          pthread_rwlock_unlock (&record->lock);
          break;
        }
    }
}

static void instance_record_collect_garbage (instance_record_t* record)
{
  pthread_rwlock_wrlock (&record->lock);
  heap_collect_garbage (record->heap);
  pthread_rwlock_unlock (&record->lock);
}

struct runtime_t {
  instance_table_t* instance_table;
  instance_record_t* head;
  instance_record_t** tail;
  size_t pending;
  pthread_mutex_t list_mutex;
  pthread_cond_t list_cond;
  pthread_mutex_t stdout_mutex;
};

struct thread_runtime_t {
  pthread_t thread;
  runtime_t* runtime;
  stack_frame_t* stack;
  char* mutable_phase_base_pointer;
  instance_record_t* current_instance;
};

runtime_t* runtime_make (instance_table_t* instance_table)
{
  runtime_t* r = (runtime_t*)malloc (sizeof (runtime_t));
  memset (r, 0, sizeof (runtime_t));
  r->instance_table = instance_table;
  r->tail = &(r->head);
  pthread_mutex_init (&r->list_mutex, NULL);
  pthread_cond_init (&r->list_cond, NULL);
  pthread_mutex_init (&r->stdout_mutex, NULL);
  return r;
}

static void push (runtime_t* runtime, instance_record_t* record)
{
  pthread_mutex_lock (&runtime->list_mutex);
  if (record->next == 0)
    {
      // Not on the schedule.
      *(runtime->tail) = record;
      runtime->tail = &(record->next);
      record->next = (instance_record_t*)1;
    }
  pthread_cond_signal (&runtime->list_cond);
  pthread_mutex_unlock (&runtime->list_mutex);
}

void runtime_allocate_instances (runtime_t* runtime)
{
  for (instance_table_t::InstancesType::const_iterator pos = runtime->instance_table->instances.begin (),
         limit = runtime->instance_table->instances.end ();
       pos != limit;
       ++pos)
    {
      instance_t* instance = pos->second;
      void * ptr;
      if (instance->is_top_level ())
        {
          const type_t* type = instance->type ();
          size_t size = type->size ();
          ptr = malloc (size);
          memset (ptr, 0, size);
        }
      else
        {
          ptr = (char*)instance_record_get_ptr (instance->parent ()->record) + instance->offset ();
        }

      // Set up the scheduling data structure.
      instance_record_t* record = instance_record_make (ptr, instance);
      // Add the instance to the schedule.
      push (runtime, record);
    }
}

struct port_t {
  void* instance;
  const reaction_t* reaction;
  int64_t parameter;
  port_t* next;

  port_t (void* i, const reaction_t* r, int64_t p) : instance (i), reaction (r), parameter (p), next (NULL) { }
};

void runtime_create_bindings (runtime_t* runtime)
{
  for (instance_table_t::PortsType::const_iterator output_pos = runtime->instance_table->ports.begin (),
         output_limit = runtime->instance_table->ports.end ();
       output_pos != output_limit;
       ++output_pos)
    {
      instance_t* output_instance = output_pos->second.output_instance;
      size_t output_port = output_pos->first - output_instance->address ();

      for (instance_table_t::InputsType::const_iterator input_pos = output_pos->second.inputs.begin (),
             input_limit = output_pos->second.inputs.end ();
           input_pos != input_limit;
           ++input_pos)
        {
          port_t* port = new port_t (instance_record_get_ptr (input_pos->instance->record), input_pos->reaction, input_pos->parameter);

          port_t** p = (port_t**)((char*)instance_record_get_ptr (output_instance->record) + output_port);
          port->next = *p;
          *p = port;
        }
    }
}

static void
evaluate_rvalue (thread_runtime_t* runtime,
                 ast_t* expr);

static void evaluate_lvalue_dereference_expr (thread_runtime_t* runtime, const ast_t* expr)
{
  evaluate_rvalue (runtime, expr->at (UNARY_CHILD));
}

static void evaluate_lvalue_identifier_expr (thread_runtime_t* runtime, const ast_identifier_expr_t* expr)
{
  // Get the address of the identifier.
  symbol_t* symbol = expr->symbol.symbol ();
  assert (symbol != NULL);
  ptrdiff_t offset = symbol_get_offset (symbol);
  stack_frame_push_address (runtime->stack, offset);
}

static void
evaluate_lvalue (thread_runtime_t* runtime,
                 ast_t* expr);

static void evaluate_lvalue_select_expr (thread_runtime_t* runtime, const ast_select_expr_t* expr)
{
  ast_t *left = expr->base ();
  ast_t *right = expr->identifier ();
  string_t identifier = ast_get_identifier (right);

  typed_value_t selected_type = ast_get_typed_value (expr);
  typed_value_t type = ast_get_typed_value (left);

  if (dynamic_cast<const function_type_t*> (selected_type.type))
    {
      method_t* method = dynamic_cast<const named_type_t*> (type.type)->get_method (identifier);
      stack_frame_push_pointer (runtime->stack, method->node);
      return;
    }

  evaluate_lvalue (runtime, left);
  field_t* field = type_select_field (type.type, identifier);
  assert (field != NULL);
  void* ptr = stack_frame_pop_pointer (runtime->stack);
  stack_frame_push_pointer (runtime->stack, (char*)ptr + field_offset (field));
}

static void
evaluate_lvalue (thread_runtime_t* runtime,
                 ast_t* expr)
{
  struct evaluate_lvalue_visitor_t : public ast_const_visitor_t
  {
    thread_runtime_t* runtime;

    evaluate_lvalue_visitor_t (thread_runtime_t* r) : runtime (r) { }

    void default_action (const ast_t& node)
    {
      not_reached;
    }

    void visit (const ast_index_expr_t& node)
    {
      struct visitor : public const_type_visitor_t
      {
        thread_runtime_t* runtime;
        const ast_index_expr_t& node;

        visitor (thread_runtime_t* r, const ast_index_expr_t& n) : runtime (r), node (n) { }

        void default_action (const type_t& type)
        {
          not_reached;
        }

        void visit (const array_type_t& type)
        {
          evaluate_lvalue (runtime, node.base ());
          void* ptr = stack_frame_pop_pointer (runtime->stack);
          evaluate_rvalue (runtime, node.index ());
          int64_t idx = stack_frame_pop_int (runtime->stack);
          if (idx < 0 || static_cast<size_t> (idx) >= type.dimension ())
            {
              error_at_line (-1, 0, node.file, node.line,
                             "array index is out of bounds");
            }
          stack_frame_push_pointer (runtime->stack,
                                    static_cast<char*> (ptr) + idx * type.element_size ());
        }
      };
      visitor v (runtime, node);
      ast_get_typed_value (node.base ()).type->accept (v);
    }

    void visit (const ast_dereference_expr_t& node) { evaluate_lvalue_dereference_expr (runtime, &node); }
    void visit (const ast_identifier_expr_t& node) { evaluate_lvalue_identifier_expr (runtime, &node); }
    void visit (const ast_select_expr_t& node) { evaluate_lvalue_select_expr (runtime, &node); }
  };

  evaluate_lvalue_visitor_t evaluate_lvalue_visitor (runtime);
  expr->accept (evaluate_lvalue_visitor);

  typed_value_t tv = ast_get_typed_value (expr);
  if (tv.has_value)
    {
      unimplemented;
    }
}

static void
execute (thread_runtime_t* runtime,
         const reaction_t* reaction,
         instance_record_t* instance);

typedef enum {
  RETURN,
  CONTINUE,
} ControlAction;

static ControlAction
evaluate_statement (thread_runtime_t* runtime,
                    ast_t* node);

static void
call (thread_runtime_t* runtime)
{
  const ast_t* node = static_cast<const ast_t*> (stack_frame_pop_pointer (runtime->stack));

  struct visitor : public ast_const_visitor_t
  {
    thread_runtime_t* runtime;

    visitor (thread_runtime_t* r) : runtime (r) { }

    void default_action (const ast_method_t& node)
    {
      not_reached;
    }

    void visit (const ast_function_t& node)
    {
      function_t* function = get_current_function (&node);
      stack_frame_push_base_pointer (runtime->stack, function->locals_size);
      evaluate_statement (runtime, node.at (FUNCTION_BODY));
      stack_frame_pop_base_pointer (runtime->stack);
    }

    void visit (const ast_method_t& node)
    {
      method_t* method = get_current_method (&node);
      stack_frame_push_base_pointer (runtime->stack, method->locals_size);
      evaluate_statement (runtime, node.at (METHOD_BODY));
      stack_frame_pop_base_pointer (runtime->stack);
    }
  };

  visitor v (runtime);
  node->accept (v);
}

typedef struct {
  heap_t* heap;
  pthread_mutex_t mutex;
  size_t change_count;
} heap_link_t;

static heap_link_t* make_heap_link (heap_t* heap,
                                    heap_t* allocator)
{
  // Allocate a new heap link in the parent.
  heap_link_t* hl = (heap_link_t*)heap_allocate (allocator, sizeof (heap_link_t));
  // Set up the link.
  hl->heap = heap;
  pthread_mutex_init (&hl->mutex, NULL);
  return hl;
}

static void
evaluate_rvalue (thread_runtime_t* runtime,
                 ast_t* expr)
{
  typed_value_t tv = ast_get_typed_value (expr);
  if (tv.has_value)
    {
      struct visitor : public const_type_visitor_t
      {
        thread_runtime_t* runtime;
        const typed_value_t& tv;

        visitor (thread_runtime_t* r,
                 const typed_value_t& t) : runtime (r), tv (t) { }

        void visit (const named_type_t& type)
        {
          type.subtype ()->accept (*this);
        }

        void visit (const bool_type_t&)
        {
          stack_frame_push_bool (runtime->stack, tv.bool_value);
        }

        void visit (const int_type_t&)
        {
          stack_frame_push_int (runtime->stack, tv.int_value);
        }

        void visit (const uint_type_t&)
        {
          stack_frame_push_uint (runtime->stack, tv.uint_value);
        }

        void visit (const string_type_t&)
        {
          stack_frame_push_string (runtime->stack, tv.string_value);
        }

        void visit (const pointer_type_t&)
        {
          stack_frame_push_pointer (runtime->stack, NULL);
        }

        void visit (const pointer_to_foreign_type_t&)
        {
          stack_frame_push_pointer (runtime->stack, NULL);
        }

        void visit (const pointer_to_immutable_type_t&)
        {
          stack_frame_push_pointer (runtime->stack, NULL);
        }

        void visit (const nil_type_t&)
        {
          stack_frame_push_pointer (runtime->stack, NULL);
        }

        void visit (const function_type_t&)
        {
          stack_frame_push_pointer (runtime->stack, tv.function_value->node);
        }

        void visit (const method_type_t&)
        {
          stack_frame_push_pointer (runtime->stack, tv.method_value->node);
        }

        void default_action (const type_t& type)
        {
          not_reached;
        }
      };

      visitor v (runtime, tv);
      tv.type->accept (v);

      return;
    }

  struct evaluate_rvalue_visitor_t : public ast_const_visitor_t
  {
    thread_runtime_t* runtime;

    evaluate_rvalue_visitor_t (thread_runtime_t* r) : runtime (r) { }

    void default_action (const ast_t& node)
    {
      not_reached;
    }

    void visit (const ast_address_of_expr_t& node)
    {
      evaluate_lvalue (runtime, node.child ());
    }

    void visit (const ast_add_expr_t& node)
    {
      struct visitor : public const_type_visitor_t
      {
        thread_runtime_t* runtime;
        const ast_add_expr_t& node;

        visitor (thread_runtime_t* r, const ast_add_expr_t& n) : runtime (r), node (n) { }

        void default_action (const type_t& type)
        {
          not_reached;
        }

        void visit (const named_type_t& type)
        {
          type.subtype ()->accept (*this);
        }

        void visit (const uint_type_t& type)
        {
          evaluate_rvalue (runtime, node.left ());
          evaluate_rvalue (runtime, node.right ());
          uint64_t v = stack_frame_pop_uint (runtime->stack);
          v += stack_frame_pop_uint (runtime->stack);
          stack_frame_push_uint (runtime->stack, v);
        }

        void visit (const int_type_t& type)
        {
          evaluate_rvalue (runtime, node.left ());
          evaluate_rvalue (runtime, node.right ());
          uint64_t v = stack_frame_pop_int (runtime->stack);
          v += stack_frame_pop_int (runtime->stack);
          stack_frame_push_int (runtime->stack, v);
        }
      };
      visitor v (runtime, node);
      node.get_type ().type->accept (v);
    }

    void visit (const ast_call_expr_t& node)
    {
      // Create space for the return.
      typed_value_t return_tv = node.get_type ();
      stack_frame_reserve (runtime->stack, return_tv.type->size ());

      // Sample the top of the stack.
      char* top_before = stack_frame_top (runtime->stack);

      // Push the arguments.
      evaluate_rvalue (runtime, node.args ());

      // Push a fake instruction pointer.
      stack_frame_push_pointer (runtime->stack, NULL);

      // Sample the top.
      char* top_after = stack_frame_top (runtime->stack);

      // Push the thing to call.
      evaluate_rvalue (runtime, node.expr ());

      // Perform the call.
      call (runtime);

      // Pop the arguments.
      stack_frame_pop (runtime->stack, top_after - top_before);
    }

    void visit (const ast_dereference_expr_t& node)
    {
      typed_value_t tv = node.get_type ();
      evaluate_rvalue (runtime, node.child ());
      void* ptr = stack_frame_pop_pointer (runtime->stack);
      stack_frame_load (runtime->stack, ptr, tv.type->size ());
    }

    void visit (const ast_equal_expr_t& node)
    {
      ast_t* left = node.left ();
      ast_t* right = node.right ();
      // Evaluate the left.
      evaluate_rvalue (runtime, left);
      // Evaluate the right.
      evaluate_rvalue (runtime, right);
      typed_value_t tv = ast_get_typed_value (left);
      stack_frame_equal (runtime->stack, tv.type->size ());
    }

    void visit (const ast_not_equal_expr_t& node)
    {
      ast_t* left = node.left ();
      ast_t* right = node.right ();
      // Evaluate the left.
      evaluate_rvalue (runtime, left);
      // Evaluate the right.
      evaluate_rvalue (runtime, right);
      typed_value_t tv = ast_get_typed_value (left);
      stack_frame_not_equal (runtime->stack, tv.type->size ());
    }

    void visit (const ast_identifier_expr_t& node)
    {
      symbol_t* symbol = node.symbol.symbol ();
      typed_value_t tv = node.get_type ();
      stack_frame_push (runtime->stack, symbol_get_offset (symbol), tv.type->size ());
    }

    void visit (const ast_index_expr_t& node)
    {
      struct visitor : public const_type_visitor_t
      {
        thread_runtime_t* runtime;
        const ast_index_expr_t& node;

        visitor (thread_runtime_t* r, const ast_index_expr_t& n) : runtime (r), node (n) { }

        void default_action (const type_t& type)
        {
          not_reached;
        }

        void visit (const array_type_t& type)
        {
          evaluate_lvalue (runtime, node.base ());
          void* ptr = stack_frame_pop_pointer (runtime->stack);
          evaluate_rvalue (runtime, node.index ());
          int64_t idx = stack_frame_pop_int (runtime->stack);
          if (idx < 0 || static_cast<size_t> (idx) >= type.dimension ())
            {
              error_at_line (-1, 0, node.file, node.line,
                             "array index is out of bounds");
            }
          stack_frame_load (runtime->stack,
                            static_cast<char*> (ptr) + idx * type.element_size (),
                            node.get_type ().type->size ());
        }
      };
      visitor v (runtime, node);
      ast_get_typed_value (node.base ()).type->accept (v);
    }

    void visit (const ast_logic_not_expr_t& node)
    {
      evaluate_rvalue (runtime, node.child ());
      bool b = stack_frame_pop_bool (runtime->stack);
      stack_frame_push_bool (runtime->stack, !b);
    }

    void visit (const ast_logic_and_expr_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          evaluate_rvalue (runtime, *pos);
          if (!stack_frame_pop_bool (runtime->stack))
            {
              stack_frame_push_bool (runtime->stack, false);
              return;
            }
        }
      stack_frame_push_bool (runtime->stack, true);
    }

    void visit (const ast_logic_or_expr_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          evaluate_rvalue (runtime, *pos);
          if (stack_frame_pop_bool (runtime->stack))
            {
              stack_frame_push_bool (runtime->stack, true);
              return;
            }
        }
      stack_frame_push_bool (runtime->stack, false);
    }

    void visit (const ast_merge_expr_t& node)
    {
      ast_t* child = node.child ();
      evaluate_rvalue (runtime, child);
      heap_link_t* hl = (heap_link_t*)stack_frame_pop_pointer (runtime->stack);
      if (hl != NULL)
        {
          pthread_mutex_lock (&hl->mutex);
          if (hl->heap != NULL && hl->change_count == 0)
            {
              // Break the link.
              heap_t* h = hl->heap;
              hl->heap = NULL;
              pthread_mutex_unlock (&hl->mutex);

              // Get the heap root.
              void* root = heap_instance (h);

              // Remove from parent.
              heap_remove_from_parent (h);

              // Merge into the new parent.
              heap_merge (runtime->current_instance->heap, h);

              // Return the root.
              stack_frame_push_pointer (runtime->stack, root);
            }
          else
            {
              pthread_mutex_unlock (&hl->mutex);
              stack_frame_push_pointer (runtime->stack, NULL);
            }
        }
      else
        {
          stack_frame_push_pointer (runtime->stack, NULL);
        }
    }

    void visit (const ast_move_expr_t& node)
    {
      ast_t* child = node.child ();
      evaluate_rvalue (runtime, child);
      heap_link_t* hl = (heap_link_t*)stack_frame_pop_pointer (runtime->stack);
      if (hl != NULL)
        {
          pthread_mutex_lock (&hl->mutex);
          if (hl->heap != NULL && hl->change_count == 0)
            {
              // Break the link.
              heap_t* h = hl->heap;
              hl->heap = NULL;
              pthread_mutex_unlock (&hl->mutex);

              // Remove from parent.
              heap_remove_from_parent (h);
              // Insert into the new parent.
              heap_insert_child (runtime->current_instance->heap, h);

              // Allocate a new heap link in the parent.
              heap_link_t* new_hl = make_heap_link (h, runtime->current_instance->heap);

              // Return the heap link.
              stack_frame_push_pointer (runtime->stack, new_hl);
            }
          else
            {
              pthread_mutex_unlock (&hl->mutex);
              stack_frame_push_pointer (runtime->stack, NULL);
            }
        }
      else
        {
          stack_frame_push_pointer (runtime->stack, NULL);
        }
    }

    void visit (const ast_new_expr_t& node)
    {
      // Allocate a new instance of the type.
      typed_value_t tv = node.get_type ();
      const type_t* type = dynamic_cast<const pointer_type_t*> (tv.type)->base_type ();
      if (!type_cast<heap_type_t> (type))
        {
          void* ptr = heap_allocate (runtime->current_instance->heap, type->size ());
          // Return the instance.
          stack_frame_push_pointer (runtime->stack, ptr);
        }
      else
        {
          const type_t* t = dynamic_cast<const heap_type_t*> (type)->base_type ();
          // Allocate a new heap and root object.
          heap_t* h = heap_make_size (t->size ());
          // Insert it into its parent.
          heap_insert_child (runtime->current_instance->heap, h);
          // Allocate a new heap link in the parent.
          heap_link_t* hl = make_heap_link (h, runtime->current_instance->heap);
          // Return the heap link.
          stack_frame_push_pointer (runtime->stack, hl);
        }
    }

    void port_call (const ast_t& node, ast_t* args, const field_t* field, size_t offset = 0)
    {
      // Push all of the arguments first and measure their size.
      char* top_before = stack_frame_top (runtime->stack);
      evaluate_rvalue (runtime, args);
      char* top_after = stack_frame_top (runtime->stack);
      ptrdiff_t arguments_size = top_after - top_before; // Assumes stack grows up.

      // Find the port to trigger.
      symbol_t* this_ = symtab_get_this (node.symtab);
      stack_frame_push (runtime->stack, symbol_get_offset (this_), symbol_parameter_type (this_)->size ());
      port_t* port = *((port_t**)((char*)stack_frame_pop_pointer (runtime->stack) + field_offset (field) + offset));

      char* base_pointer = stack_frame_base_pointer (runtime->stack);
      instance_record_t* instance = runtime->current_instance;

      // Trigger all the reactions bound to the port.
      while (port != NULL)
        {
          // Set up a frame.
          // Push the instance.
          stack_frame_push_pointer (runtime->stack, port->instance);
          // Push the parameter.
          if (port->reaction->has_dimension ())
            {
              stack_frame_push_int (runtime->stack, port->parameter);
            }
          // Push the arguments.
          stack_frame_load (runtime->stack, top_before, arguments_size);
          // Push an instruction pointer.
          stack_frame_push_pointer (runtime->stack, NULL);
          execute (runtime, port->reaction, *(instance_record_t**)port->instance);
          // Reset the base pointer because the callee won't do it.
          stack_frame_set_base_pointer (runtime->stack, base_pointer);
          // Also reset the current instance.
          runtime->current_instance = instance;
          port = port->next;
        }
    }

    void visit (const ast_port_call_expr_t& node)
    {
      port_call (node, node.args (), node.field);
    }

    void visit (const ast_indexed_port_call_expr_t& node)
    {
      // Determine the trigger index.
      evaluate_rvalue (runtime, node.index ());
      int64_t idx = stack_frame_pop_int (runtime->stack);
      if (idx < 0 || static_cast<size_t> (idx) >= node.array_type->dimension ())
        {
          error_at_line (-1, 0, node.file, node.line,
                         "array index is out of bounds");
        }

      port_call (node, node.args (), node.field, idx * node.array_type->element_size ());
    }

    void visit (const ast_select_expr_t& node)
    {
      evaluate_lvalue (runtime, node.base ());
      void* ptr = stack_frame_pop_pointer (runtime->stack);
      stack_frame_load (runtime->stack, (char*)ptr + field_offset (node.field), node.get_type ().type->size ());
    }

    void visit (const ast_list_expr_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          evaluate_rvalue (runtime, *pos);
        }
    }
  };

  evaluate_rvalue_visitor_t evaluate_rvalue_visitor (runtime);
  expr->accept (evaluate_rvalue_visitor);
}

static ControlAction
evaluate_statement (thread_runtime_t* runtime,
                    ast_t* node)
{
  struct visitor : public ast_const_visitor_t
  {
    ControlAction retval;
    thread_runtime_t* runtime;

    visitor (thread_runtime_t* r) : retval (CONTINUE), runtime (r) { }

    void default_action (const ast_t& node)
    {
      not_reached;
    }

    void visit (const ast_assign_statement_t& node)
    {
      ast_t* left = node.left ();
      ast_t* right = node.right ();
      // Determine the size of the value being assigned.
      size_t size = ast_get_typed_value (right).type->size ();
      // Evaluate the address.
      evaluate_lvalue (runtime, left);
      void* ptr = stack_frame_pop_pointer (runtime->stack);
      // Evaluate the value.
      evaluate_rvalue (runtime, right);
      // Store.
      stack_frame_store_heap (runtime->stack, ptr, size);
    }

    void visit (const ast_change_statement_t& node)
    {
      ast_t* expr = node.expr ();
      ast_t* body = node.body ();
      // Evaluate the pointer to the heap link.
      evaluate_rvalue (runtime, expr);
      heap_link_t* hl = (heap_link_t*)stack_frame_pop_pointer (runtime->stack);
      if (hl == NULL)
        {
          // Heap link is null.
          unimplemented;
        }
      pthread_mutex_lock (&hl->mutex);
      ++hl->change_count;
      pthread_mutex_unlock (&hl->mutex);

      // Save the old heap.
      heap_t* old_heap = runtime->current_instance->heap;
      // Set the the new heap.
      runtime->current_instance->heap = hl->heap;

      {
        // Evaluate the address of the heap root.
        symbol_t* symbol = node.root_symbol.symbol ();
        assert (symbol != NULL);
        ptrdiff_t offset = symbol_get_offset (symbol);
        stack_frame_push_address (runtime->stack, offset);
      }

      char** root_value = (char**)stack_frame_pop_pointer (runtime->stack);
      // Push a pointer to the root object.
      *root_value = (char*)heap_instance (runtime->current_instance->heap);

      evaluate_statement (runtime, body);

      // Restore the old heap.
      runtime->current_instance->heap = old_heap;

      pthread_mutex_lock (&hl->mutex);
      --hl->change_count;
      pthread_mutex_unlock (&hl->mutex);
    }

    void visit (const ast_expression_statement_t& node)
    {
      ast_t* child = node.at (UNARY_CHILD);
      // Determine the size of the value being generated.
      size_t size = ast_get_typed_value (child).type->size ();
      // Evaluate.
      evaluate_rvalue (runtime, child);
      // Remove value.
      stack_frame_pop (runtime->stack, size);
    }

    void visit (const ast_if_statement_t& node)
    {
      ast_t* condition = node.at (IF_CONDITION);
      evaluate_rvalue (runtime, condition);
      bool c = stack_frame_pop_bool (runtime->stack);
      if (c)
        {
          ast_t* true_branch = node.at (IF_TRUE_BRANCH);
          if (evaluate_statement (runtime, true_branch) == RETURN)
            {
              retval = RETURN;
              return;
            }
        }
    }

    void visit (const ast_add_assign_statement_t& node)
    {
      // Determine the size of the value being assigned.
      const type_t* type = ast_get_typed_value (node.right ()).type;
      // Evaluate the address.
      evaluate_lvalue (runtime, node.left ());
      void* ptr = stack_frame_pop_pointer (runtime->stack);
      // Evaluate the value.
      evaluate_rvalue (runtime, node.right ());

      struct visitor : public const_type_visitor_t
      {
        thread_runtime_t* runtime;
        void* ptr;
        visitor (thread_runtime_t* r, void* p) : runtime (r), ptr (p) { }

        void visit (const named_type_t& type)
        {
          type.subtype ()->accept (*this);
        }

        void visit (const uint_type_t& type)
        {
          *((uint64_t*)ptr) += stack_frame_pop_uint (runtime->stack);
        }

        void default_action (const type_t& type)
        {
          unimplemented;
        }
      };
      visitor v (runtime, ptr);
      type->accept (v);
    }

    void visit (const ast_subtract_assign_statement_t& node)
    {
      // Determine the size of the value being assigned.
      const type_t* type = ast_get_typed_value (node.right ()).type;
      // Evaluate the address.
      evaluate_lvalue (runtime, node.left ());
      void* ptr = stack_frame_pop_pointer (runtime->stack);
      // Evaluate the value.
      evaluate_rvalue (runtime, node.right ());

      struct visitor : public const_type_visitor_t
      {
        thread_runtime_t* runtime;
        void* ptr;
        visitor (thread_runtime_t* r, void* p) : runtime (r), ptr (p) { }

        void visit (const named_type_t& type)
        {
          type.subtype ()->accept (*this);
        }

        void visit (const uint_type_t& type)
        {
          *((uint64_t*)ptr) -= stack_frame_pop_uint (runtime->stack);
        }

        void default_action (const type_t& type)
        {
          unimplemented;
        }
      };
      visitor v (runtime, ptr);
      type->accept (v);
    }

    void visit (const ast_list_statement_t& node)
    {
      AST_FOREACH (child, &node)
	{
	  if (evaluate_statement (runtime, child) == RETURN)
            {
              retval = RETURN;
              return;
            }
	}
    }

    void visit (const ast_return_statement_t& node)
    {
      // Evaluate the expression.
      evaluate_rvalue (runtime, node.at (UNARY_CHILD));
      // Get the return parameter.
      symbol_t* symbol = symtab_get_first_return_parameter (node.symtab);
      assert (symbol != NULL);
      stack_frame_store_stack (runtime->stack, symbol_get_offset (symbol), symbol_parameter_type (symbol)->size ());
      retval = RETURN;
      return;
    }

    void visit (const ast_increment_statement_t& node)
    {
      evaluate_lvalue (runtime, node.child ());
      void* ptr = stack_frame_pop_pointer (runtime->stack);

      struct visitor : public const_type_visitor_t
      {
        void* ptr;

        visitor (void* p) : ptr (p) { }

        void default_action (const type_t& type)
        {
          not_reached;
        }

        void visit (const named_type_t& type)
        {
          type.subtype ()->accept (*this);
        }

        void visit (const uint_type_t& type)
        {
          ++(*static_cast<uint64_t*> (ptr));
        }
      };
      visitor v (ptr);
      ast_get_typed_value (node.child ()).type->accept (v);
    }

    void visit (const ast_trigger_statement_t& node)
    {
      // Need to keep track of the largest base pointer so we can process the mutable section.
      char* base_pointer = stack_frame_base_pointer (runtime->stack);
      if (base_pointer > runtime->mutable_phase_base_pointer)
        {
          runtime->mutable_phase_base_pointer = base_pointer;
        }

      // The caller pushed an instruction pointer which is just
      // before the base pointer.  Overwrite it with the body.
      ast_t* body = node.body ();
      memcpy (stack_frame_ip (runtime->stack), &body, sizeof (void*));
      // Execute the expression list.
      evaluate_rvalue (runtime, node.expr_list ());

      // Stop execution.
      retval = RETURN;
      return;
    }

    void visit (const ast_var_statement_t& node)
    {
      // Do nothing.  Variable is already on the stack and zeroed out.
    }

    void visit (const ast_println_statement_t& node)
    {
      pthread_mutex_lock (&runtime->runtime->stdout_mutex);
      ast_t* expr_list = node.at (UNARY_CHILD);
      AST_FOREACH (child, expr_list)
        {
          evaluate_rvalue (runtime, child);
          struct visitor : public const_type_visitor_t
          {
            thread_runtime_t* runtime;
            visitor (thread_runtime_t* rt) : runtime (rt) { }

            void default_action (const type_t& type)
            {
              not_reached;
            }

            void visit (const named_type_t& type)
            {
              type.subtype ()->accept (*this);
            }

            void visit (const bool_type_t& type)
            {
              bool b = stack_frame_pop_bool (runtime->stack);
              if (b)
                {
                  printf ("true");
                }
              else
                {
                  printf ("false");
                }
            }

            void visit (const pointer_type_t& type)
            {
              void* ptr = stack_frame_pop_pointer (runtime->stack);
              printf ("%p", ptr);
            }

            void visit (const pointer_to_foreign_type_t& type)
            {
              void* ptr = stack_frame_pop_pointer (runtime->stack);
              printf ("%p", ptr);
            }

            void visit (const pointer_to_immutable_type_t& type)
            {
              void* ptr = stack_frame_pop_pointer (runtime->stack);
              printf ("%p", ptr);
            }

            void visit (const string_type_t& type)
            {
              rtstring_t s = stack_frame_pop_string (runtime->stack);
              fwrite (s.bytes, 1, s.size, stdout);
            }

            void visit (const uint_type_t& type)
            {
              uint64_t u = stack_frame_pop_uint (runtime->stack);
              printf ("%lu", u);
            }

            void visit (const int_type_t& type)
            {
              int64_t u = stack_frame_pop_int (runtime->stack);
              printf ("%lu", u);
            }

            void visit (const iota_type_t& type)
            {
              int64_t u = stack_frame_pop_int (runtime->stack);
              printf ("%lu", u);
            }
          };
          visitor v (runtime);
          ast_get_typed_value (child).type->accept (v);
        }
      printf ("\n");
      pthread_mutex_unlock (&runtime->runtime->stdout_mutex);
    }
  };
  visitor v (runtime);
  node->accept (v);

  return v.retval;
}

static bool
enabled (thread_runtime_t* runtime,
         instance_record_t* record,
         const action_t* action,
         size_t iota)
{
  assert (stack_frame_empty (runtime->stack));
  ast_t* precondition;
  if (dynamic_cast<ast_action_t*> (action->node ()))
    {
      precondition = dynamic_cast<ast_action_t*> (action->node ())->precondition ();
    }
  else
    {
      precondition = dynamic_cast<ast_dimensioned_action_t*> (action->node ())->precondition ();
    }

  // Push this.
  stack_frame_push_pointer (runtime->stack, heap_instance (record->heap));
  // Push iota.
  if (action->has_dimension ())
    {
      stack_frame_push_uint (runtime->stack, iota);
    }
  // Set the current instance.
  runtime->current_instance = record;
  // Push an instruction pointer.
  stack_frame_push_pointer (runtime->stack, NULL);
  stack_frame_push_base_pointer (runtime->stack, 0 /* No locals. */);
  evaluate_rvalue (runtime, precondition);
  bool retval = stack_frame_pop_bool (runtime->stack);
  stack_frame_pop_base_pointer (runtime->stack);
  // Pop the instruction pointer.
  stack_frame_pop_pointer (runtime->stack);
  // Pop iota.
  if (action->has_dimension ())
    {
      stack_frame_pop_uint (runtime->stack);
    }
  // Pop this.
  stack_frame_pop_pointer (runtime->stack);
  assert (stack_frame_empty (runtime->stack));
  return retval;
}

static void
execute (thread_runtime_t* runtime,
         const action_t* action,
         instance_record_t* instance)
{
  // Set the current instance.
  runtime->current_instance = instance;

  ast_t* body;
  if (dynamic_cast<ast_action_t*> (action->node ()))
    {
      body = dynamic_cast<ast_action_t*> (action->node ())->body ();
    }
  else
    {
      body = dynamic_cast<ast_dimensioned_action_t*> (action->node ())->body ();
    }

  // Reset the mutable phase base pointer.
  runtime->mutable_phase_base_pointer = 0;

  stack_frame_push_base_pointer (runtime->stack, action->locals_size);

  evaluate_statement (runtime, body);

  if (runtime->mutable_phase_base_pointer == 0)
    {
      // No triggers.  Pop the base pointer and finish.
      stack_frame_pop_base_pointer (runtime->stack);
      return;
    }

  // Process all of the deferred trigger bodies.
  // First, go to the last frame.
  stack_frame_set_base_pointer (runtime->stack, runtime->mutable_phase_base_pointer);

  while (stack_frame_base_pointer (runtime->stack) != NULL)
    {
      // Get the deferred body.
      ast_t* b = *(ast_t**)stack_frame_ip (runtime->stack);

      // Get the trigger.
      trigger_t *trigger = symtab_get_current_trigger (b->symtab);
      assert (trigger != NULL);

      // Set the current record.
      symbol_t* this_ = symtab_get_this (b->symtab);
      stack_frame_push (runtime->stack, symbol_get_offset (this_), symbol_parameter_type (this_)->size ());
      runtime->current_instance = *(instance_record_t**)stack_frame_pop_pointer (runtime->stack);

      // Execute it.
      evaluate_statement (runtime, b);

      // Add to the schedule.
      if (trigger->action == TRIGGER_WRITE)
        {
          push (runtime->runtime, runtime->current_instance);
        }

      // Pop until the base pointer.
      stack_frame_set_top (runtime->stack, stack_frame_base_pointer (runtime->stack) + sizeof (void*));
      // Pop the base pointer.
      stack_frame_pop_base_pointer (runtime->stack);
    }
}

static void
execute (thread_runtime_t* runtime,
         const reaction_t* reaction,
         instance_record_t* instance)
{
  // Set the current instance.
  runtime->current_instance = instance;

  ast_t* body;
  if (dynamic_cast<ast_reaction_t*> (reaction->node ()))
    {
      body = dynamic_cast<ast_reaction_t*> (reaction->node ())->body ();
    }
  else
    {
      body = dynamic_cast<ast_dimensioned_reaction_t*> (reaction->node ())->body ();
    }

  stack_frame_push_base_pointer (runtime->stack, reaction->locals_size);

  evaluate_statement (runtime, body);
}

void
dump_schedule (const runtime_t* runtime)
{
  if (runtime->head != NULL)
    {
      instance_record_t* record = runtime->head;
      while (record != (instance_record_t*)1)
        {
          printf ("%p instance=%p type=%p\n", record, heap_instance (record->heap), record->instance->type ());
          record = record->next;
        }
    }
}

void
dump_instances (const runtime_t* runtime)
{
  for (instance_table_t::InstancesType::const_iterator pos = runtime->instance_table->instances.begin (),
         limit = runtime->instance_table->instances.end ();
       pos != limit;
       ++pos)
    {
      instance_t* instance = pos->second;
      if (instance->is_top_level ())
        {
          unimplemented;
          //void* ptr = instance_record_get_ptr (instance_get_record (instance));
          //type_print_value (instance_type (instance), ptr);
          printf ("\n");
        }
    }
}

static void*
run (void* arg)
{
  thread_runtime_t* runtime = (thread_runtime_t*)arg;

  for (;;)
    {
      pthread_mutex_lock (&runtime->runtime->list_mutex);
      while (!(runtime->runtime->head != NULL ||
               (runtime->runtime->head == NULL && runtime->runtime->pending == 0)))
        {
          pthread_cond_wait (&runtime->runtime->list_cond, &runtime->runtime->list_mutex);
        }

      if (runtime->runtime->head == NULL)
        {
          // Implies that runtime->pending == 0.
          // Signal so other threads can exit.
          pthread_cond_signal (&runtime->runtime->list_cond);
          pthread_mutex_unlock (&runtime->runtime->list_mutex);
          pthread_exit (NULL);
        }

      /* printf ("BEGIN schedule before pop\n"); */
      /* dump_schedule (runtime); */
      /* printf ("END schedule before pop\n"); */

      // Get an instance to execute.
      instance_record_t* record = runtime->runtime->head;
      runtime->runtime->head = record->next;
      if (runtime->runtime->head == (instance_record_t*)1)
        {
          runtime->runtime->head = NULL;
          runtime->runtime->tail = &(runtime->runtime->head);
        }
      record->next = NULL;

      ++runtime->runtime->pending;
      pthread_mutex_unlock (&runtime->runtime->list_mutex);

      /* // TODO:  Comment this out. */
      /* if (rand () % 100 < 50) */
      /*   { */
      /*     push (runtime->runtime, record); */

      /*     pthread_mutex_lock (&runtime->runtime->list_mutex); */
      /*     --runtime->runtime->pending; */
      /*     pthread_cond_signal (&runtime->runtime->list_cond); */
      /*     pthread_mutex_unlock (&runtime->runtime->list_mutex); */
      /*     continue; */
      /*   } */

      /* printf ("BEGIN schedule after pop\n"); */
      /* dump_schedule (runtime); */
      /* printf ("END schedule after pop\n"); */

      // Try all the actions.
      for (instance_t::InstanceSetsType::const_iterator pos = record->instance->instance_sets.begin (),
             limit = record->instance->instance_sets.end ();
           pos != limit;
           ++pos)
        {
          /* printf ("BEGIN instances before enabled\n"); */
          /* dump_instances (runtime); */
          /* printf ("END instances before enabled\n"); */

          const action_t* action = pos->action;

          instance_record_lock (pos->set);
          if (enabled (runtime, record, action, pos->iota))
            {
              assert (stack_frame_empty (runtime->stack));
              /* printf ("BEGIN instances before execute\n"); */
              /* dump_instances (runtime); */
              /* printf ("END instances before execute\n"); */

              // Push the instance.
              stack_frame_push_pointer (runtime->stack, heap_instance (record->heap));
              // Push iota.
              if (action->has_dimension ())
                {
                  stack_frame_push_uint (runtime->stack, pos->iota);
                }
              // Push the instruction pointer.
              stack_frame_push_pointer (runtime->stack, NULL);
              // Execute.
              execute (runtime, action, record);
              // Pop the instruction pointer.
              stack_frame_pop_pointer (runtime->stack);
              // Pop iota.
              if (action->has_dimension ())
                {
                  stack_frame_pop_uint (runtime->stack);
                }
              // Pop the instance.
              stack_frame_pop_pointer (runtime->stack);

              /* type_print_value (record->type, record->instance); */
              /* printf ("\n"); */

              /* printf ("BEGIN instances after execute\n"); */
              /* dump_instances (runtime); */
              /* printf ("END instances after execute\n"); */
              assert (stack_frame_empty (runtime->stack));
            }
          instance_record_unlock (pos->set);

          /* // TODO: Comment this out. */
          /* { */
          /*   instance_t** pos; */
          /*   instance_t** limit; */
          /*   for (pos = instance_table_begin (runtime->runtime->instance_table), limit = instance_table_end (runtime->runtime->instance_table); */
          /*        pos != limit; */
          /*        pos = instance_table_next (pos)) */
          /*     { */
          /*       instance_t* instance = *pos; */
          /*       printf ("BEFORE FIRST\n"); */
          /*       heap_dump (instance_get_record (instance)->heap); */
          /*       printf ("BEFORE LAST\n"); */
          /*       instance_record_collect_garbage (instance_get_record (instance)); */
          /*       printf ("AFTER FIRST\n"); */
          /*       heap_dump (instance_get_record (instance)->heap); */
          /*       printf ("AFTER LAST\n"); */
          /*     } */
          /* } */

          /* printf ("BEGIN instances after enabled\n"); */
          /* dump_instances (runtime); */
          /* printf ("END instances after enabled\n"); */
        }

      // Collect garbage.
      instance_record_collect_garbage (record);
      //heap_dump (record->heap);

      pthread_mutex_lock (&runtime->runtime->list_mutex);
      --runtime->runtime->pending;
      pthread_cond_signal (&runtime->runtime->list_cond);
      pthread_mutex_unlock (&runtime->runtime->list_mutex);
    }

  return NULL;
}

#define NUM_THREADS 2

void runtime_run (runtime_t* r,
                  memory_model_t* memory_model,
                  size_t stack_size)
{
  thread_runtime_t runtime;
  runtime.runtime = r;
  runtime.stack = stack_frame_make (memory_model, stack_size);

  for (instance_table_t::InstancesType::const_iterator pos = r->instance_table->instances.begin (),
         limit = r->instance_table->instances.end ();
       pos != limit;
       ++pos)
    {
      instance_t* instance = pos->second;
      if (instance->is_top_level ())
        {
          char* top_before = stack_frame_top (runtime.stack);
          // Push this.
          stack_frame_push_pointer (runtime.stack, heap_instance (instance->record->heap));
          // Push a fake instruction pointer.
          stack_frame_push_pointer (runtime.stack, NULL);
          char* top_after = stack_frame_top (runtime.stack);
          stack_frame_push_pointer (runtime.stack, instance->method ()->node);
          call (&runtime);
          stack_frame_pop (runtime.stack, top_after - top_before);
        }
    }

  thread_runtime_t* runtimes = (thread_runtime_t*)malloc (NUM_THREADS * sizeof (thread_runtime_t));
  memset (runtimes, 0, NUM_THREADS * sizeof (thread_runtime_t));
  size_t idx;
  for (idx = 0; idx != NUM_THREADS; ++idx)
    {
      thread_runtime_t* runtime = runtimes + idx;
      runtime->runtime = r;
      runtime->stack = stack_frame_make (memory_model, stack_size);
      pthread_create (&runtime->thread, NULL, run, runtime);
    }

  for (idx = 0; idx != NUM_THREADS; ++idx)
    {
      thread_runtime_t* runtime = runtimes + idx;
      pthread_join (runtime->thread, NULL);
    }
}
