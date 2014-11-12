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

typedef struct instance_record_t instance_record_t;
struct instance_record_t {
  // Back pointer to the data.
  void* instance;
  // The type.
  const type_t* type;
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
  record->instance = ptr;
  record->type = type;

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
  switch (ast_expression_kind (expr))
    {
    case AstCallExpr:
      unimplemented;
    case AstDereferenceExpr:
      evaluate_rvalue (runtime, ast_get_child (expr, UNARY_CHILD));
      break;
    case AstExprList:
      unimplemented;
    case AstIdentifierExpr:
      unimplemented;
    case AstLogicAndExpr:
      unimplemented;
    case AstLogicNotExpr:
      unimplemented;
    case AstLogicOrExpr:
      unimplemented;
    case AstPortCallExpr:
      unimplemented;
    case AstSelectExpr:
      {
        evaluate_lvalue (runtime, ast_get_child (expr, BINARY_LEFT_CHILD));
        ast_t *left = ast_get_child (expr, BINARY_LEFT_CHILD);
	ast_t *right = ast_get_child (expr, BINARY_RIGHT_CHILD);
	string_t identifier = ast_get_identifier (right);
	type_t *type = ast_get_type (left);
	field_t* field = type_select_field (type, identifier);
        assert (field != NULL);
        void* ptr = stack_frame_pop_pointer (runtime->stack);
        stack_frame_push_pointer (runtime->stack, ptr + field_offset (field));
      }
      break;
    case AstTypedLiteral:
      unimplemented;
    case AstUntypedLiteral:
      unimplemented;
    }
}

static void
execute (runtime_t* runtime,
         const action_t* action);

static void
evaluate_rvalue (runtime_t* runtime,
                 ast_t* expr)
{
  switch (ast_expression_kind (expr))
    {
    case AstCallExpr:
      unimplemented;
    case AstDereferenceExpr:
      unimplemented;
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
        type_t* type = ast_get_type (expr);
        stack_frame_push (runtime->stack, symbol_get_offset (symbol), type_size (type));
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
            execute (runtime, port->reaction);
            // Reset the base pointer because the callee won't do it.
            stack_frame_set_base_pointer (runtime->stack, base_pointer);
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
	type_t *type = ast_get_type (left);
	field_t* field = type_select_field (type, identifier);
        assert (field != NULL);
        void* ptr = stack_frame_pop_pointer (runtime->stack);
        stack_frame_load (runtime->stack, ptr + field_offset (field), type_size (ast_get_type (expr)));
      }
      break;
    case AstTypedLiteral:
      {
        typed_value_t value = ast_get_typed_value (expr);
        switch (type_kind (value.type))
          {
          case TypeUndefined:
            unimplemented;
          case TypeVoid:
            unimplemented;
          case TypeBool:
            stack_frame_push_bool (runtime->stack, value.bool_value);
            break;
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
          }
      }
      break;
    case AstUntypedLiteral:
      bug ("AstUntyepdLiteral evaluated at runtime");
    }
}

typedef enum {
  RETURN,
  CONTINUE,
} ControlAction;

static ControlAction
evaluate_statement (runtime_t* runtime,
                    ast_t* node)
{
  switch (ast_statement_kind (node))
    {
    case AstAssignmentStmt:
      {
        // Determine the size of the value being assigned.
        size_t size = type_size (ast_get_type2 (ast_get_child (node, BINARY_RIGHT_CHILD)));
        // Evaluate the address.
        evaluate_lvalue (runtime, ast_get_child (node, BINARY_LEFT_CHILD));
        void* ptr = stack_frame_pop_pointer (runtime->stack);
        // Evaluate the value.
        evaluate_rvalue (runtime, ast_get_child (node, BINARY_RIGHT_CHILD));
        // Store.
        stack_frame_store (runtime->stack, ptr, size);
      }
      break;
    case AstExpressionStmt:
      unimplemented;
    case AstStmtList:
      {
	AST_FOREACH (child, node)
	{
	  ControlAction ca = evaluate_statement (runtime, child);
          if (ca == RETURN)
            {
              return RETURN;
            }
	}
      }
      break;
    case AstReturnStmt:
      unimplemented;
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
      unimplemented;
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
  stack_frame_push_pointer (runtime->stack, record->instance);
  // Push an instruction pointer.
  stack_frame_push_pointer (runtime->stack, NULL);
  stack_frame_push_base_pointer (runtime->stack);
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
         const action_t* action)
{
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

  stack_frame_push_base_pointer (runtime->stack);
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
          // Execute it.
          evaluate_statement (runtime, b);

          // Add to the schedule.
          trigger_t *trigger = symtab_get_current_trigger (ast_get_symtab (b));
          assert (trigger != NULL);
          if (trigger_get_mutates_receiver (trigger))
            {
              symbol_t* this = symtab_get_this (ast_get_symtab (b));
              stack_frame_push (runtime->stack, symbol_get_offset (this), type_size (symbol_parameter_type (this)));
              instance_record_t** ptr = stack_frame_pop_pointer (runtime->stack);
              push (runtime, *ptr);
            }

          // Pop until the base pointer.
          stack_frame_set_top (runtime->stack, stack_frame_base_pointer (runtime->stack) + sizeof (void*));
          // Pop the base pointer.
          stack_frame_pop_base_pointer (runtime->stack);
        }
    }
}

static void
dump_schedule (const runtime_t* runtime)
{
  if (runtime->head != NULL)
    {
      instance_record_t* record = runtime->head;
      while (record != (instance_record_t*)1)
        {
          printf ("%p instance=%p type=%p\n", record, record->instance, record->type);
          record = record->next;
        }
    }
}

static void
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
              stack_frame_push_pointer (runtime->stack, record->instance);
              // Push the instruction pointer.
              stack_frame_push_pointer (runtime->stack, NULL);
              // Execute.
              execute (runtime, action);
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
    }
}
