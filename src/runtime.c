#include "runtime.h"
#include "util.h"
#include "debug.h"
#include "instance.h"
#include "type.h"
#include <string.h>
#include "field.h"
#include "ast.h"
#include "action.h"
#include "stack_frame.h"
#include "symbol.h"
#include "trigger.h"
#include "getter.h"
#include "semantic.h"
#include "heap.h"

typedef struct instance_record_t instance_record_t;
struct instance_record_t {
  // The type.
  const type_t* type;
  heap_t* heap;
  // Next instance on the schedule.
  // 0 means this instance is not on the schedule.
  // 1 means this instance is the end of the schedule.
  // ? means this instance is on the schedule.
  instance_record_t* next;
};

struct runtime_t {
  instance_table_t* instance_table;
  instance_record_t* head;
  instance_record_t** tail;
  stack_frame_t* stack;
  char* mutable_phase_base_pointer;
  instance_record_t* current_instance;
};

runtime_t* runtime_make (instance_table_t* instance_table,
                         memory_model_t* memory_model, size_t stack_size)
{
  runtime_t* r = xmalloc (sizeof (runtime_t));
  r->instance_table = instance_table;
  r->tail = &(r->head);
  r->stack = stack_frame_make (memory_model, stack_size);
  return r;
}

static void push (runtime_t* runtime, instance_record_t* record)
{
  if (record->next == 0)
    {
      // Not on the schedule.
      *(runtime->tail) = record;
      runtime->tail = &(record->next);
      record->next = (instance_record_t*)1;
    }
}

static void initialize_instance (runtime_t* runtime, void* ptr, const type_t* type, instance_t* instance, instance_table_t* instance_table)
{
  // Set up the scheduling data structure.
  instance_record_t* record = xmalloc (sizeof (instance_record_t));
  record->type = type;
  record->heap = heap_make (ptr, ptr + type_size (type));

  // Link the instance to its scheduling information.
  *((instance_record_t**)ptr) = record;

  // Add the instance to the schedule.
  push (runtime, record);

  instance_set_ptr (instance, ptr);

  // Recur on all contained instances.
  const type_t* field_list = type_component_field_list (type);
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
          initialize_instance (runtime, ptr + field_offset (field), t, instance_table_get_subinstance (instance_table, instance, field), instance_table);
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
          initialize_instance (runtime, ptr, type, instance, runtime->instance_table);
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

      port_t* port = port_make (instance_get_ptr (input_instance), input_reaction);

      port_t** p = instance_get_ptr (output_instance) + field_offset (output_port);
      port->next = *p;
      *p = port;
    }
}

static void
evaluate_rvalue (runtime_t* runtime,
                 ast_t* expr);

static void
evaluate_lvalue (runtime_t* runtime,
                 ast_t* expr)
{
  typed_value_t tv = ast_get_typed_value (expr);
  if (tv.has_value)
    {
      ast_print (expr, 0);
      unimplemented;
    }

  switch (ast_expression_kind (expr))
    {
    case AstAddressOfExpr:
      unimplemented;
    case AstCallExpr:
      unimplemented;
    case AstDereferenceExpr:
      evaluate_rvalue (runtime, ast_get_child (expr, UNARY_CHILD));
      break;
    case AstEqualExpr:
      unimplemented;
    case AstExprList:
      unimplemented;

    case AstIdentifierExpr:
      {
        // Get the address of the identifier.
        symbol_t* symbol = ast_get_symbol (expr);
        assert (symbol != NULL);
        ptrdiff_t offset = symbol_get_offset (symbol);
        stack_frame_push_address (runtime->stack, offset);
      }
      break;

    case AstLogicAndExpr:
      unimplemented;
    case AstLogicNotExpr:
      unimplemented;
    case AstLogicOrExpr:
      unimplemented;
    case AstNewExpr:
      unimplemented;
    case AstNotEqualExpr:
      unimplemented;
    case AstPortCallExpr:
      unimplemented;
    case AstSelectExpr:
      {
        ast_t *left = ast_get_child (expr, BINARY_LEFT_CHILD);
	ast_t *right = ast_get_child (expr, BINARY_RIGHT_CHILD);
	string_t identifier = ast_get_identifier (right);

        typed_value_t selected_type = ast_get_typed_value (expr);
	typed_value_t type = ast_get_typed_value (left);

        if (type_kind (selected_type.type) == TypeGetter)
          {
            getter_t* getter = type_component_get_getter (type.type, identifier);
            stack_frame_push_pointer (runtime->stack, getter_node (getter));
            break;
          }

        evaluate_lvalue (runtime, ast_get_child (expr, BINARY_LEFT_CHILD));
	field_t* field = type_select_field (type.type, identifier);
        assert (field != NULL);
        void* ptr = stack_frame_pop_pointer (runtime->stack);
        stack_frame_push_pointer (runtime->stack, ptr + field_offset (field));
      }
      break;
    case AstTypedLiteral:
      unimplemented;
    }
}

static void
execute (runtime_t* runtime,
         const action_t* action,
         instance_record_t* instance);

typedef enum {
  RETURN,
  CONTINUE,
} ControlAction;

static ControlAction
evaluate_statement (runtime_t* runtime,
                    ast_t* node);

static void
call (runtime_t* runtime)
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
    case AstGetter:
      {
        getter_t* getter = get_current_getter (node);
        stack_frame_push_base_pointer (runtime->stack, getter_get_locals_size (getter));
        evaluate_statement (runtime, ast_get_child (node, GETTER_BODY));
        stack_frame_pop_base_pointer (runtime->stack);
      }
      break;
    case AstIdentifier:
      unimplemented;
    case AstIdentifierList:
      unimplemented;
    case AstInstance:
      unimplemented;
    case AstPointerReceiverDefinition:
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

static void
evaluate_rvalue (runtime_t* runtime,
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
        case TypePointer:
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
        case TypeGetter:
          stack_frame_push_pointer (runtime->stack, getter_node (tv.getter_value));
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

  switch (ast_expression_kind (expr))
    {
    case AstAddressOfExpr:
      {
        evaluate_lvalue (runtime, ast_get_child (expr, UNARY_CHILD));
      }
      break;
    case AstCallExpr:
      {
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
      break;
    case AstDereferenceExpr:
      unimplemented;

    case AstEqualExpr:
      {
        ast_t* left = ast_get_child (expr, BINARY_LEFT_CHILD);
        ast_t* right = ast_get_child (expr, BINARY_RIGHT_CHILD);
        // Evaluate the left.
        evaluate_rvalue (runtime, left);
        // Evaluate the right.
        evaluate_rvalue (runtime, right);
        typed_value_t tv = ast_get_typed_value (left);
        stack_frame_equal (runtime->stack, type_size (tv.type));
      }
      break;

    case AstExprList:
      {
	AST_FOREACH (child, expr)
	{
	  evaluate_rvalue (runtime, child);
	}
      }
      break;
    case AstIdentifierExpr:
      {
        ast_t* identifier_node = ast_get_child (expr, UNARY_CHILD);
        string_t name = ast_get_identifier (identifier_node);
        symtab_t* symtab = ast_get_symtab (expr);
        symbol_t* symbol = symtab_find (symtab, name);
        typed_value_t tv = ast_get_typed_value (expr);
        stack_frame_push (runtime->stack, symbol_get_offset (symbol), type_size (tv.type));
      }
      break;
    case AstLogicAndExpr:
      unimplemented;
    case AstLogicNotExpr:
      {
        evaluate_rvalue (runtime, ast_get_child (expr, UNARY_CHILD));
        bool b = stack_frame_pop_bool (runtime->stack);
        stack_frame_push_bool (runtime->stack, !b);
      }
      break;
    case AstLogicOrExpr:
      unimplemented;

    case AstNewExpr:
      {
        // Allocate a new instance of the type.
        typed_value_t tv = ast_get_typed_value (expr);
        type_t* type = type_pointer_base_type (tv.type);
        void* ptr = heap_allocate (runtime->current_instance->heap, type);
        // Return the instance.
        stack_frame_push_pointer (runtime->stack, ptr);
      }
      break;

    case AstNotEqualExpr:
      {
        ast_t* left = ast_get_child (expr, BINARY_LEFT_CHILD);
        ast_t* right = ast_get_child (expr, BINARY_RIGHT_CHILD);
        // Evaluate the left.
        evaluate_rvalue (runtime, left);
        // Evaluate the right.
        evaluate_rvalue (runtime, right);
        typed_value_t tv = ast_get_typed_value (left);
        stack_frame_not_equal (runtime->stack, type_size (tv.type));
      }
      break;

    case AstPortCallExpr:
      {
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
      break;
    case AstSelectExpr:
      {
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
      break;
    case AstTypedLiteral:
      not_reached;
      break;
    }
}

static ControlAction
evaluate_statement (runtime_t* runtime,
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

    case AstExpressionStmt:
      unimplemented;

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
          case TypePointer:
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
          case TypeGetter:
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

            case TypePointer:
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
            case TypeGetter:
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
    }

  return CONTINUE;
}

static bool
enabled (runtime_t* runtime,
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
execute (runtime_t* runtime,
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
          if (trigger_get_mutates_receiver (trigger))
            {
              push (runtime, runtime->current_instance);
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
          printf ("%p instance=%p type=%p\n", record, heap_instance (record->heap), record->type);
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
          void* ptr = instance_get_ptr (instance);
          type_print_value (instance_type (instance), ptr);
          printf ("\n");
        }
    }
}

void runtime_run (runtime_t* runtime)
{
  while (runtime->head != NULL)
    {
      /* printf ("BEGIN schedule before pop\n"); */
      /* dump_schedule (runtime); */
      /* printf ("END schedule before pop\n"); */

      // Get an instance to execute.
      instance_record_t* record = runtime->head;
      runtime->head = record->next;
      if (runtime->head == (instance_record_t*)1)
        {
          runtime->head = NULL;
          runtime->tail = &(runtime->head);
        }
      record->next = NULL;

      /* if (rand () % 100 < 50) */
      /*   { */
      /*     push (runtime, record); */
      /*     continue; */
      /*   } */

      /* printf ("BEGIN schedule after pop\n"); */
      /* dump_schedule (runtime); */
      /* printf ("END schedule after pop\n"); */

      // Try all the actions.
      action_t** pos;
      action_t** limit;
      for (pos = type_actions_begin (record->type), limit = type_actions_end (record->type);
           pos != limit;
           pos = type_actions_next (pos))
        {
          /* printf ("BEGIN instances before enabled\n"); */
          /* dump_instances (runtime); */
          /* printf ("END instances before enabled\n"); */

          action_t* action = *pos;
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

          /* printf ("BEGIN instances after enabled\n"); */
          /* dump_instances (runtime); */
          /* printf ("END instances after enabled\n"); */
        }

      // Collect garbage.
      heap_collect_garbage (record->heap);
    }
}
