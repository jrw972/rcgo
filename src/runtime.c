#include "runtime.h"
#include "util.h"
#include "debug.h"
#include "instance.h"
#include "type.h"
#include <string.h>
#include <pthread.h>
#include "field.h"
#include "ast.h"
#include "action.h"
#include "stack_frame.h"
#include "symbol.h"
#include "trigger.h"
#include "method.h"
#include "semantic.h"
#include "heap.h"
#include "instance_set.h"

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
  instance_record_t* record = xmalloc (sizeof (instance_record_t));
  pthread_rwlock_init (&record->lock, NULL);
  record->instance = instance;
  record->heap = heap_make (ptr, ptr + type_size (instance_type (instance)));

  // Link the instance to its scheduling information.
  *((instance_record_t**)ptr) = record;

  instance_set_record (instance, record);

  return record;
}

static void* instance_record_get_ptr (const instance_record_t* record)
{
  return heap_instance (record->heap);
}

static void instance_record_lock (instance_record_t* record,
                                  action_t* action)
{
  const instance_set_t* set = instance_get_set (record->instance, action);
  instance_set_element_t* pos;
  instance_set_element_t* limit;
  for (pos = instance_set_begin (set), limit = instance_set_end (set);
       pos != limit;
       pos = instance_set_next (pos))
    {
      instance_record_t* record = instance_get_record (pos->instance);
      switch (pos->action)
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

static void instance_record_unlock (instance_record_t* record,
                                  action_t* action)
{
  const instance_set_t* set = instance_get_set (record->instance, action);
  instance_set_element_t* pos;
  instance_set_element_t* limit;
  for (pos = instance_set_begin (set), limit = instance_set_end (set);
       pos != limit;
       pos = instance_set_next (pos))
    {
      instance_record_t* record = instance_get_record (pos->instance);
      switch (pos->action)
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

typedef struct thread_runtime_t thread_runtime_t;
struct thread_runtime_t {
  pthread_t thread;
  runtime_t* runtime;
  stack_frame_t* stack;
  char* mutable_phase_base_pointer;
  instance_record_t* current_instance;
};

runtime_t* runtime_make (instance_table_t* instance_table)
{
  runtime_t* r = xmalloc (sizeof (runtime_t));
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

static void initialize_instance (runtime_t* runtime, void* ptr, instance_t* instance, instance_table_t* instance_table)
{
  // Set up the scheduling data structure.
  instance_record_t* record = instance_record_make (ptr, instance);

  // Add the instance to the schedule.
  push (runtime, record);

  // Recur on all contained instances.
  const type_t* field_list = type_component_field_list (instance_type (instance));
  field_t** pos;
  field_t** limit;
  for (pos = type_field_list_begin (field_list), limit = type_field_list_end (field_list);
       pos != limit;
       pos = type_field_list_next (pos))
    {
      field_t* field = *pos;
      const type_t* t = field_type (field);
      if (type_is_component (t))
        {
          initialize_instance (runtime, ptr + field_offset (field), instance_table_get_subinstance (instance_table, instance, field), instance_table);
        }
    }
}

void runtime_allocate_instances (runtime_t* runtime)
{
  instance_t** pos;
  instance_t** limit;
  for (pos = instance_table_begin (runtime->instance_table), limit = instance_table_end (runtime->instance_table);
       pos != limit;
       pos = instance_table_next (pos))
    {
      instance_t* instance = *pos;
      if (instance_is_top_level (instance))
        {
          const type_t* type = instance_type (instance);
          size_t size = type_size (type);
          void* ptr = xmalloc (size);
          initialize_instance (runtime, ptr, instance, runtime->instance_table);
        }
    }
}

typedef struct port_t port_t;
struct port_t {
  void* instance;
  const action_t* reaction;
  port_t* next;
};

static port_t*
port_make (void* instance,
           const action_t* reaction)
{
  port_t* p = xmalloc (sizeof (port_t));
  p->instance = instance;
  p->reaction = reaction;
  return p;
}

void runtime_create_bindings (runtime_t* runtime)
{
  concrete_binding_t* pos;
  concrete_binding_t* limit;
  for (pos = instance_table_binding_begin (runtime->instance_table), limit = instance_table_binding_end (runtime->instance_table);
       pos != limit;
       pos = instance_table_binding_next (pos))
    {
      instance_t* output_instance = concrete_binding_output_instance (pos);
      const field_t* output_port = concrete_binding_output_port (pos);
      instance_t* input_instance = concrete_binding_input_instance (pos);
      const action_t* input_reaction = concrete_binding_input_reaction (pos);

      port_t* port = port_make (instance_record_get_ptr (instance_get_record (input_instance)), input_reaction);

      port_t** p = instance_record_get_ptr (instance_get_record (output_instance)) + field_offset (output_port);
      port->next = *p;
      *p = port;
    }
}

static void
evaluate_rvalue (thread_runtime_t* runtime,
                 ast_t* expr);

static void evaluate_lvalue_dereference_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  evaluate_rvalue (runtime, ast_get_child (expr, UNARY_CHILD));
}

static void evaluate_lvalue_identifier_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  // Get the address of the identifier.
  symbol_t* symbol = ast_get_symbol (expr);
  assert (symbol != NULL);
  ptrdiff_t offset = symbol_get_offset (symbol);
  stack_frame_push_address (runtime->stack, offset);
}

static void
evaluate_lvalue (thread_runtime_t* runtime,
                 ast_t* expr);

static void evaluate_lvalue_select_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  ast_t *left = ast_get_child (expr, BINARY_LEFT_CHILD);
  ast_t *right = ast_get_child (expr, BINARY_RIGHT_CHILD);
  string_t identifier = ast_get_identifier (right);

  typed_value_t selected_type = ast_get_typed_value (expr);
  typed_value_t type = ast_get_typed_value (left);

  if (type_kind (selected_type.type) == TypeFunc)
    {
      method_t* method = type_get_method (type.type, identifier);
      stack_frame_push_pointer (runtime->stack, method_node (method));
      return;
    }

  evaluate_lvalue (runtime, ast_get_child (expr, BINARY_LEFT_CHILD));
  field_t* field = type_select_field (type.type, identifier);
  assert (field != NULL);
  void* ptr = stack_frame_pop_pointer (runtime->stack);
  stack_frame_push_pointer (runtime->stack, ptr + field_offset (field));
}

static ast_const_visitor_t evaluate_lvalue_visitor = {
 visit_dereference_expr: evaluate_lvalue_dereference_expr,
 visit_identifier_expr: evaluate_lvalue_identifier_expr,
 visit_select_expr: evaluate_lvalue_select_expr,
};

static void
evaluate_lvalue (thread_runtime_t* runtime,
                 ast_t* expr)
{
  ast_const_accept (expr, &evaluate_lvalue_visitor, runtime);

  typed_value_t tv = ast_get_typed_value (expr);
  if (tv.has_value)
    {
      ast_print (expr, 0);
      unimplemented;
    }
}

static void
execute (thread_runtime_t* runtime,
         const action_t* action,
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
  ast_t* node = stack_frame_pop_pointer (runtime->stack);

  switch (ast_kind (node))
    {
    case AstAction:
      unimplemented;
    case AstBind:
      unimplemented;
    case AstBindStatement:
      unimplemented;
    case AstExpression:
      unimplemented;
    case AstMethod:
      {
        method_t* method = get_current_method (node);
        stack_frame_push_base_pointer (runtime->stack, method_get_locals_size (method));
        evaluate_statement (runtime, ast_get_child (node, METHOD_BODY));
        stack_frame_pop_base_pointer (runtime->stack);
      }
      break;
    case AstIdentifier:
      unimplemented;
    case AstIdentifierList:
      unimplemented;
    case AstInstance:
      unimplemented;
    case AstReceiverDefinition:
      unimplemented;
    case AstReaction:
      unimplemented;
    case AstStatement:
      unimplemented;
    case AstTopLevelList:
      unimplemented;
    case AstTypeDefinition:
      unimplemented;
    case AstTypeSpecification:
      unimplemented;
    }
}

static void evaluate_rvalue_expr_list (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  AST_FOREACH (child, expr)
    {
      evaluate_rvalue (runtime, child);
    }
}

static void evaluate_rvalue_address_of_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  evaluate_lvalue (runtime, ast_get_child (expr, UNARY_CHILD));
}

static void evaluate_rvalue_call_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  // Create space for the return.
  typed_value_t return_tv = ast_get_typed_value (expr);
  stack_frame_reserve (runtime->stack, type_size (return_tv.type));

  // Sample the top of the stack.
  char* top_before = stack_frame_top (runtime->stack);

  // Push the arguments.
  evaluate_rvalue (runtime, ast_get_child (expr, CALL_ARGS));

  // Push a fake instruction pointer.
  stack_frame_push_pointer (runtime->stack, NULL);

  // Sample the top.
  char* top_after = stack_frame_top (runtime->stack);

  // Push the thing to call.
  evaluate_rvalue (runtime, ast_get_child (expr, CALL_EXPR));

  // Perform the call.
  call (runtime);

  // Pop the arguments.
  stack_frame_pop (runtime->stack, top_after - top_before);
}

static void evaluate_rvalue_dereference_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  typed_value_t tv = ast_get_typed_value (expr);
  evaluate_rvalue (runtime, ast_get_child (expr, UNARY_CHILD));
  void* ptr = stack_frame_pop_pointer (runtime->stack);
  stack_frame_load (runtime->stack, ptr, type_size (tv.type));
}

static void evaluate_rvalue_equal_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  ast_t* left = ast_get_child (expr, BINARY_LEFT_CHILD);
  ast_t* right = ast_get_child (expr, BINARY_RIGHT_CHILD);
  // Evaluate the left.
  evaluate_rvalue (runtime, left);
  // Evaluate the right.
  evaluate_rvalue (runtime, right);
  typed_value_t tv = ast_get_typed_value (left);
  stack_frame_equal (runtime->stack, type_size (tv.type));
}

static void evaluate_rvalue_identifier_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  ast_t* identifier_node = ast_get_child (expr, UNARY_CHILD);
  string_t name = ast_get_identifier (identifier_node);
  symtab_t* symtab = ast_get_symtab (expr);
  symbol_t* symbol = symtab_find (symtab, name);
  typed_value_t tv = ast_get_typed_value (expr);
  stack_frame_push (runtime->stack, symbol_get_offset (symbol), type_size (tv.type));
}

static void evaluate_rvalue_logic_not_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  evaluate_rvalue (runtime, ast_get_child (expr, UNARY_CHILD));
  bool b = stack_frame_pop_bool (runtime->stack);
  stack_frame_push_bool (runtime->stack, !b);
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
  heap_link_t* hl = heap_allocate (allocator, sizeof (heap_link_t));
  // Set up the link.
  hl->heap = heap;
  pthread_mutex_init (&hl->mutex, NULL);
  return hl;
}

static void evaluate_rvalue_new_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  // Allocate a new instance of the type.
  typed_value_t tv = ast_get_typed_value (expr);
  type_t* type = type_pointer_base_type (tv.type);
  if (!type_is_heap (type))
    {
      void* ptr = heap_allocate (runtime->current_instance->heap, type_size (type));
      // Return the instance.
      stack_frame_push_pointer (runtime->stack, ptr);
    }
  else
    {
      const type_t* t = type_heap_base_type (type);
      // Allocate a new heap and root object.
      heap_t* h = heap_make_size (type_size (t));
      // Insert it into its parent.
      heap_insert_child (runtime->current_instance->heap, h);
      // Allocate a new heap link in the parent.
      heap_link_t* hl = make_heap_link (h, runtime->current_instance->heap);
      // Return the heap link.
      stack_frame_push_pointer (runtime->stack, hl);
    }
}

static void evaluate_rvalue_not_equal_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  ast_t* left = ast_get_child (expr, BINARY_LEFT_CHILD);
  ast_t* right = ast_get_child (expr, BINARY_RIGHT_CHILD);
  // Evaluate the left.
  evaluate_rvalue (runtime, left);
  // Evaluate the right.
  evaluate_rvalue (runtime, right);
  typed_value_t tv = ast_get_typed_value (left);
  stack_frame_not_equal (runtime->stack, type_size (tv.type));
}

static void evaluate_rvalue_port_call_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  // Push all of the arguments first and measure their size.
  char* top_before = stack_frame_top (runtime->stack);
  evaluate_rvalue (runtime, ast_get_child (expr, CALL_ARGS));
  char* top_after = stack_frame_top (runtime->stack);
  ptrdiff_t arguments_size = top_after - top_before; // Assumes stack grows up.

  // Find the port to trigger.
  ast_t *e = ast_get_child (expr, CALL_EXPR);
  string_t port_identifier = ast_get_identifier (e);
  const type_t *this_type =
    type_pointer_base_type (symtab_get_this_type
                            (ast_get_symtab (e)));
  field_t* field = type_select_field (this_type, port_identifier);
  symbol_t* this = symtab_get_this (ast_get_symtab (e));
  stack_frame_push (runtime->stack, symbol_get_offset (this), type_size (symbol_parameter_type (this)));
  port_t* port = *((port_t**)(stack_frame_pop_pointer (runtime->stack) + field_offset (field)));

  char* base_pointer = stack_frame_base_pointer (runtime->stack);
  instance_record_t* instance = runtime->current_instance;

  // Trigger all the reactions bound to the port.
  while (port != NULL)
    {
      // Set up a frame.
      // Push the instance.
      stack_frame_push_pointer (runtime->stack, port->instance);
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

static void evaluate_rvalue_select_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  evaluate_lvalue (runtime, ast_get_child (expr, BINARY_LEFT_CHILD));
  ast_t *left = ast_get_child (expr, BINARY_LEFT_CHILD);
  ast_t *right = ast_get_child (expr, BINARY_RIGHT_CHILD);
  string_t identifier = ast_get_identifier (right);
  typed_value_t tv = ast_get_typed_value (left);
  field_t* field = type_select_field (tv.type, identifier);
  assert (field != NULL);
  void* ptr = stack_frame_pop_pointer (runtime->stack);
  stack_frame_load (runtime->stack, ptr + field_offset (field), type_size (ast_get_typed_value (expr).type));
}

static void evaluate_rvalue_move_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  ast_t* child = ast_get_child (expr, UNARY_CHILD);
  evaluate_rvalue (runtime, child);
  heap_link_t* hl = stack_frame_pop_pointer (runtime->stack);
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

static void evaluate_rvalue_merge_expr (void* data, const ast_t* expr)
{
  thread_runtime_t* runtime = data;
  ast_t* child = ast_get_child (expr, UNARY_CHILD);
  evaluate_rvalue (runtime, child);
  heap_link_t* hl = stack_frame_pop_pointer (runtime->stack);
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

static ast_const_visitor_t evaluate_rvalue_visitor = {
 visit_address_of_expr: evaluate_rvalue_address_of_expr,
 visit_call_expr: evaluate_rvalue_call_expr,
 visit_dereference_expr: evaluate_rvalue_dereference_expr,
 visit_equal_expr: evaluate_rvalue_equal_expr,
 visit_identifier_expr: evaluate_rvalue_identifier_expr,
 visit_logic_not_expr: evaluate_rvalue_logic_not_expr,
 visit_merge_expr: evaluate_rvalue_merge_expr,
 visit_move_expr: evaluate_rvalue_move_expr,
 visit_new_expr: evaluate_rvalue_new_expr,
 visit_not_equal_expr: evaluate_rvalue_not_equal_expr,
 visit_port_call_expr: evaluate_rvalue_port_call_expr,
 visit_select_expr: evaluate_rvalue_select_expr,

 visit_expr_list: evaluate_rvalue_expr_list,
};

static void
evaluate_rvalue (thread_runtime_t* runtime,
                 ast_t* expr)
{
  typed_value_t tv = ast_get_typed_value (expr);
  if (tv.has_value)
    {
      switch (type_kind (tv.type))
        {
        case TypeUndefined:
          unimplemented;
        case TypeVoid:
          unimplemented;
        case TypeBool:
          stack_frame_push_bool (runtime->stack, tv.bool_value);
          break;
        case TypeComponent:
          unimplemented;
        case TypeForeign:
          unimplemented;
        case TypeHeap:
          unimplemented;
        case TypeImmutable:
          unimplemented;
        case TypePointer:
        case TypePointerToForeign:
        case TypePointerToImmutable:
          stack_frame_push_pointer (runtime->stack, tv.pointer_value);
          break;
        case TypePort:
          unimplemented;
        case TypeReaction:
          unimplemented;
        case TypeFieldList:
          unimplemented;
        case TypeSignature:
          unimplemented;
        case TypeString:
          stack_frame_push_string (runtime->stack, tv.string_value);
          break;
        case TypeFunc:
          stack_frame_push_pointer (runtime->stack, method_node (tv.method_value));
          break;
        case TypeUint:
          stack_frame_push_uint (runtime->stack, tv.uint_value);
          break;
        case TypeStruct:
          unimplemented;

        case UntypedUndefined:
          unimplemented;
        case UntypedNil:
          unimplemented;
        case UntypedBool:
          unimplemented;
        case UntypedInteger:
          unimplemented;
        case UntypedString:
          unimplemented;
        }

      return;
    }

  ast_const_accept (expr, &evaluate_rvalue_visitor, runtime);
}

static ControlAction
evaluate_statement (thread_runtime_t* runtime,
                    ast_t* node)
{
  switch (ast_statement_kind (node))
    {
    case AstAssignmentStmt:
      {
        ast_t* left = ast_get_child (node, BINARY_LEFT_CHILD);
        ast_t* right = ast_get_child (node, BINARY_RIGHT_CHILD);
        // Determine the size of the value being assigned.
        size_t size = type_size (ast_get_typed_value (right).type);
        // Evaluate the address.
        evaluate_lvalue (runtime, left);
        void* ptr = stack_frame_pop_pointer (runtime->stack);
        // Evaluate the value.
        evaluate_rvalue (runtime, right);
        // Store.
        stack_frame_store_heap (runtime->stack, ptr, size);
      }
      break;

    case AstChangeStmt:
      {
        ast_t* expr = ast_get_child (node, CHANGE_EXPR);
        ast_t* body = ast_get_child (node, CHANGE_BODY);
        // Evaluate the pointer to the heap link.
        evaluate_rvalue (runtime, expr);
        heap_link_t* hl = stack_frame_pop_pointer (runtime->stack);
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

        // Evaluate the address of the heap root.
        evaluate_lvalue_identifier_expr (runtime, body);
        char** root_value = stack_frame_pop_pointer (runtime->stack);
        // Push a pointer to the root object.
        *root_value = heap_instance (runtime->current_instance->heap);

        evaluate_statement (runtime, body);

        // Restore the old heap.
        runtime->current_instance->heap = old_heap;

        pthread_mutex_lock (&hl->mutex);
        --hl->change_count;
        pthread_mutex_unlock (&hl->mutex);
      }
      break;

    case AstExpressionStmt:
      {
        ast_t* child = ast_get_child (node, UNARY_CHILD);
        // Determine the size of the value being generated.
        size_t size = type_size (ast_get_typed_value (child).type);
        // Evaluate.
        evaluate_rvalue (runtime, child);
        // Remove value.
        stack_frame_pop (runtime->stack, size);
      }
      break;

    case AstIfStmt:
      {
        ast_t* condition = ast_get_child (node, IF_CONDITION);
        evaluate_rvalue (runtime, condition);
        bool c = stack_frame_pop_bool (runtime->stack);
        if (c)
          {
            ast_t* true_branch = ast_get_child (node, IF_TRUE_BRANCH);
            if (evaluate_statement (runtime, true_branch) == RETURN)
              {
                return RETURN;
              }
          }
      }
      break;

    case AstAddAssignStmt:
      {
        // Determine the size of the value being assigned.
        type_t* type = ast_get_typed_value (ast_get_child (node, BINARY_RIGHT_CHILD)).type;
        // Evaluate the address.
        evaluate_lvalue (runtime, ast_get_child (node, BINARY_LEFT_CHILD));
        void* ptr = stack_frame_pop_pointer (runtime->stack);
        // Evaluate the value.
        evaluate_rvalue (runtime, ast_get_child (node, BINARY_RIGHT_CHILD));
        switch (type_kind (type))
          {
          case TypeUndefined:
            unimplemented;
          case TypeVoid:
            unimplemented;
          case TypeBool:
            unimplemented;
          case TypeComponent:
            unimplemented;
          case TypeForeign:
            unimplemented;
          case TypeHeap:
            unimplemented;
          case TypeImmutable:
            unimplemented;
          case TypePointer:
            unimplemented;
          case TypePointerToForeign:
            unimplemented;
          case TypePointerToImmutable:
            unimplemented;
          case TypePort:
            unimplemented;
          case TypeReaction:
            unimplemented;
          case TypeFieldList:
            unimplemented;
          case TypeSignature:
            unimplemented;
          case TypeString:
            unimplemented;
          case TypeFunc:
            unimplemented;
          case TypeUint:
            *((uint64_t*)ptr) += stack_frame_pop_uint (runtime->stack);
            break;
          case TypeStruct:
            unimplemented;

          case UntypedUndefined:
            unimplemented;
          case UntypedNil:
            unimplemented;
          case UntypedBool:
            unimplemented;
          case UntypedInteger:
            unimplemented;
          case UntypedString:
            unimplemented;

          }
      }
      break;

    case AstSubtractAssignStmt:
      {
        // Determine the size of the value being assigned.
        type_t* type = ast_get_typed_value (ast_get_child (node, BINARY_RIGHT_CHILD)).type;
        // Evaluate the address.
        evaluate_lvalue (runtime, ast_get_child (node, BINARY_LEFT_CHILD));
        void* ptr = stack_frame_pop_pointer (runtime->stack);
        // Evaluate the value.
        evaluate_rvalue (runtime, ast_get_child (node, BINARY_RIGHT_CHILD));
        switch (type_kind (type))
          {
          case TypeUndefined:
            unimplemented;
          case TypeVoid:
            unimplemented;
          case TypeBool:
            unimplemented;
          case TypeComponent:
            unimplemented;
          case TypeForeign:
            unimplemented;
          case TypeHeap:
            unimplemented;
          case TypeImmutable:
            unimplemented;
          case TypePointer:
            unimplemented;
          case TypePointerToForeign:
            unimplemented;
          case TypePointerToImmutable:
            unimplemented;
          case TypePort:
            unimplemented;
          case TypeReaction:
            unimplemented;
          case TypeFieldList:
            unimplemented;
          case TypeSignature:
            unimplemented;
          case TypeString:
            unimplemented;
          case TypeFunc:
            unimplemented;
          case TypeUint:
            *((uint64_t*)ptr) -= stack_frame_pop_uint (runtime->stack);
            break;
          case TypeStruct:
            unimplemented;

          case UntypedUndefined:
            unimplemented;
          case UntypedNil:
            unimplemented;
          case UntypedBool:
            unimplemented;
          case UntypedInteger:
            unimplemented;
          case UntypedString:
            unimplemented;

          }
      }
      break;

    case AstStmtList:
      {
	AST_FOREACH (child, node)
	{
	  if (evaluate_statement (runtime, child) == RETURN)
            {
              return RETURN;
            }
	}
      }
      break;
    case AstReturnStmt:
      {
        // Evaluate the expression.
        evaluate_rvalue (runtime, ast_get_child (node, UNARY_CHILD));
        // Get the return parameter.
        symbol_t* symbol = symtab_get_first_return_parameter (ast_get_symtab (node));
        assert (symbol != NULL);
        stack_frame_store_stack (runtime->stack, symbol_get_offset (symbol), type_size (symbol_parameter_type (symbol)));
        return RETURN;
      }
      break;
    case AstTriggerStmt:
      {
        // Need to keep track of the largest base pointer so we can process the mutable section.
        char* base_pointer = stack_frame_base_pointer (runtime->stack);
        if (base_pointer > runtime->mutable_phase_base_pointer)
          {
            runtime->mutable_phase_base_pointer = base_pointer;
          }

        // The caller pushed an instruction pointer which is just
        // before the base pointer.  Overwrite it with the body.
        ast_t* body = ast_get_child (node, TRIGGER_BODY);
        memcpy (stack_frame_ip (runtime->stack), &body, sizeof (void*));
        // Execute the expression list.
        evaluate_rvalue (runtime, ast_get_child (node, TRIGGER_EXPRESSION_LIST));

        // Stop execution.
        return RETURN;
      }
      break;

    case AstVarStmt:
      // Do nothing.  Variable is already on the stack and zeroed out.
      break;

    case AstPrintlnStmt:
      {
        pthread_mutex_lock (&runtime->runtime->stdout_mutex);
        ast_t* expr_list = ast_get_child (node, UNARY_CHILD);
        AST_FOREACH (child, expr_list)
        {
          evaluate_rvalue (runtime, child);
          switch (type_kind (ast_get_typed_value (child).type))
            {
            case TypeUndefined:
              unimplemented;
            case TypeVoid:
              unimplemented;
            case TypeBool:
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
              break;
            case TypeComponent:
              unimplemented;

            case TypeForeign:
              unimplemented;

            case TypeHeap:
              unimplemented;

            case TypeImmutable:
              unimplemented;

            case TypePointer:
            case TypePointerToForeign:
            case TypePointerToImmutable:
              {
                void* ptr = stack_frame_pop_pointer (runtime->stack);
                printf ("%p", ptr);
              }
              break;

            case TypePort:
              unimplemented;
            case TypeReaction:
              unimplemented;
            case TypeFieldList:
              unimplemented;
            case TypeSignature:
              unimplemented;
            case TypeString:
              {
                rtstring_t s = stack_frame_pop_string (runtime->stack);
                fwrite (s.bytes, 1, s.size, stdout);
              }
              break;
            case TypeFunc:
              unimplemented;
            case TypeUint:
              {
                uint64_t u = stack_frame_pop_uint (runtime->stack);
                printf ("%lu", u);
              }
              break;
          case TypeStruct:
            unimplemented;

          case UntypedUndefined:
            unimplemented;
            case UntypedNil:
              unimplemented;

          case UntypedBool:
            unimplemented;
          case UntypedInteger:
            unimplemented;
          case UntypedString:
            unimplemented;

            }
        }
      }
      printf ("\n");
      pthread_mutex_unlock (&runtime->runtime->stdout_mutex);
    }

  return CONTINUE;
}

static bool
enabled (thread_runtime_t* runtime,
         instance_record_t* record,
         action_t* action)
{
  assert (stack_frame_empty (runtime->stack));
  ast_t* precondition = ast_get_child (action_node (action), ACTION_PRECONDITION);
  stack_frame_push_pointer (runtime->stack, heap_instance (record->heap));
  // Set the current instance.
  runtime->current_instance = record;
  // Push an instruction pointer.
  stack_frame_push_pointer (runtime->stack, NULL);
  stack_frame_push_base_pointer (runtime->stack, 0 /* No locals. */);
  evaluate_rvalue (runtime, precondition);
  bool retval = stack_frame_pop_bool (runtime->stack);
  stack_frame_pop_base_pointer (runtime->stack);
  stack_frame_pop_pointer (runtime->stack);
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
  if (action_is_reaction (action))
    {
      body = ast_get_child (action_node (action), REACTION_BODY);
    }
  else
    {
      body = ast_get_child (action_node (action), ACTION_BODY);
      // Reset the mutable phase base pointer.
      runtime->mutable_phase_base_pointer = 0;
    }

  stack_frame_push_base_pointer (runtime->stack, action_get_locals_size (action));
  evaluate_statement (runtime, body);

  if (action_is_action (action))
    {
      // Process all of the deferred trigger bodies.
      // First, go to the last frame.
      stack_frame_set_base_pointer (runtime->stack, runtime->mutable_phase_base_pointer);

      while (stack_frame_base_pointer (runtime->stack) != NULL)
        {
          // Get the deferred body.
          ast_t* b = *(ast_t**)stack_frame_ip (runtime->stack);

          // Get the trigger.
          trigger_t *trigger = symtab_get_current_trigger (ast_get_symtab (b));
          assert (trigger != NULL);

          // Set the current record.
          symbol_t* this = symtab_get_this (ast_get_symtab (b));
          stack_frame_push (runtime->stack, symbol_get_offset (this), type_size (symbol_parameter_type (this)));
          runtime->current_instance = *(instance_record_t**)stack_frame_pop_pointer (runtime->stack);

          // Execute it.
          evaluate_statement (runtime, b);

          // Add to the schedule.
          if (trigger_get_action (trigger) == TRIGGER_WRITE)
            {
              push (runtime->runtime, runtime->current_instance);
            }

          // Pop until the base pointer.
          stack_frame_set_top (runtime->stack, stack_frame_base_pointer (runtime->stack) + sizeof (void*));
          // Pop the base pointer.
          stack_frame_pop_base_pointer (runtime->stack);
        }
    }
}

void
dump_schedule (const runtime_t* runtime)
{
  if (runtime->head != NULL)
    {
      instance_record_t* record = runtime->head;
      while (record != (instance_record_t*)1)
        {
          printf ("%p instance=%p type=%p\n", record, heap_instance (record->heap), instance_type (record->instance));
          record = record->next;
        }
    }
}

void
dump_instances (const runtime_t* runtime)
{
  instance_t** pos;
  instance_t** limit;
  for (pos = instance_table_begin (runtime->instance_table), limit = instance_table_end (runtime->instance_table);
       pos != limit;
       pos = instance_table_next (pos))
    {
      instance_t* instance = *pos;
      if (instance_is_top_level (instance))
        {
          void* ptr = instance_record_get_ptr (instance_get_record (instance));
          type_print_value (instance_type (instance), ptr);
          printf ("\n");
        }
    }
}

static void*
run (void* arg)
{
  thread_runtime_t* runtime = arg;

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
      action_t** pos;
      action_t** limit;
      const type_t* type = instance_type (record->instance);
      for (pos = type_actions_begin (type), limit = type_actions_end (type);
           pos != limit;
           pos = type_actions_next (pos))
        {
          /* printf ("BEGIN instances before enabled\n"); */
          /* dump_instances (runtime); */
          /* printf ("END instances before enabled\n"); */

          action_t* action = *pos;
          instance_record_lock (record, action);
          if (enabled (runtime, record, action))
            {
              /* printf ("BEGIN instances before execute\n"); */
              /* dump_instances (runtime); */
              /* printf ("END instances before execute\n"); */

              // Push the instance.
              stack_frame_push_pointer (runtime->stack, heap_instance (record->heap));
              // Push the instruction pointer.
              stack_frame_push_pointer (runtime->stack, NULL);
              // Execute.
              execute (runtime, action, record);
              // Pop the instruction pointer.
              stack_frame_pop_pointer (runtime->stack);
              // Pop the instance.
              stack_frame_pop_pointer (runtime->stack);

              /* type_print_value (record->type, record->instance); */
              /* printf ("\n"); */

              /* printf ("BEGIN instances after execute\n"); */
              /* dump_instances (runtime); */
              /* printf ("END instances after execute\n"); */
            }
          instance_record_unlock (record, action);

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
  thread_runtime_t* runtimes = xmalloc (NUM_THREADS * sizeof (thread_runtime_t));
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
