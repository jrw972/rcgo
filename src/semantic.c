#include "semantic.h"
#include "debug.h"
#include "symtab.h"
#include "ast.h"
#include <error.h>
#include "action.h"
#include "type.h"
#include "parameter.h"
#include "instance.h"
#include "trigger.h"
#include "field.h"
#include "binding.h"
#include "symbol.h"
#include "memory_model.h"
#include "method.h"

// TODO:  Replace interacting with type_t* with typed_value_t.

void
construct_symbol_table (ast_t * node, symtab_t * symtab)
{
  if (symtab == NULL)
    {
      symtab = symtab_make (NULL);
    }

  switch (ast_kind (node))
    {
    case AstAction:
    case AstBind:
    case AstMethod:
    case AstReaction:
    case AstTopLevelList:
      {
	/* Create a new symbol table. */
	symtab_t *new_symtab = symtab_make (symtab);
	symtab = new_symtab;
      }
      break;

    case AstStatement:
      switch (ast_statement_kind (node))
	{
	case AstIfStmt: /* If statements are in their own scope. */
	case AstStmtList:
	  {
	    /* Create a new symbol table. */
	    symtab_t *new_symtab = symtab_make (symtab);
	    symtab = new_symtab;
	  }
	  break;

	case AstAssignmentStmt:
        case AstAddAssignStmt:
        case AstChangeStmt:
	case AstExpressionStmt:
	case AstReturnStmt:
	case AstTriggerStmt:
	case AstVarStmt:
        case AstPrintlnStmt:
          break;
	}
      break;

    case AstBindStatement:
      switch (ast_bind_statement_kind (node))
	{
	case AstBindStmtList:
	  {
	    /* Create a new symbol table. */
	    symtab_t *new_symtab = symtab_make (symtab);
	    symtab = new_symtab;
	  }
	  break;
	case AstBindStmt:
	  break;
	}
      break;

    case AstExpression:
    case AstIdentifier:
    case AstIdentifierList:
    case AstInstance:
    case AstReceiverDefinition:
    case AstTypeDefinition:
    case AstTypeSpecification:
      break;
    }

  ast_set_symtab (node, symtab);
  AST_FOREACH (child, node)
    {
      construct_symbol_table (child, symtab);
    }
}

static void
enter_undefined_symbol (ast_t * node,
			ast_t * identifier_node, SymbolKind kind)
{
  string_t identifier = ast_get_identifier (identifier_node);
  symtab_t *symtab = ast_get_symtab (node);
  symbol_t *symbol = symtab_find_current (symtab, identifier);
  if (symbol == NULL)
    {
      symbol = symbol_make_undefined (identifier, kind, node);
      symtab_enter (symtab, symbol);
      ast_set_symbol (node, symbol);
    }
  else
    {
      error_at_line (-1, 0, ast_file (identifier_node),
		     ast_line (identifier_node),
		     "%s is already defined in this scope", get (identifier));
    }
}

static void
enter_symbol (ast_t * node, symbol_t * symbol)
{
  // Check if the symbol is defined locally.
  symtab_t *symtab = ast_get_symtab (node);
  string_t identifier = symbol_identifier (symbol);
  symbol_t *s = symtab_find_current (symtab, identifier);
  if (s == NULL)
    {
      symtab_enter (symtab, symbol);
      ast_set_symbol (node, symbol);
    }
  else
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
		     "%s is already defined in this scope", get (identifier));
    }
}

static void
enter_signature (ast_t * node, const type_t * type)
{
  parameter_t **pos;
  parameter_t **limit;
  for (pos = type_signature_begin (type), limit = type_signature_end (type);
       pos != limit; pos = type_signature_next (pos))
    {
      parameter_t* parameter = *pos;
      // Check if the symbol is defined locally.
      symtab_t *symtab = ast_get_symtab (node);
      string_t identifier = parameter_name (parameter);
      type_t *type = parameter_type (parameter);
      symbol_t *s = symtab_find_current (symtab, identifier);
      if (s == NULL)
	{
          if (parameter_is_receiver (parameter))
            {
              s = symbol_make_receiver (identifier, type, node);
            }
          else
            {
              s = symbol_make_parameter (identifier, type, node);
            }
	  symtab_enter (symtab, s);
	}
      else
	{
	  error_at_line (-1, 0, ast_file (node), ast_line (node),
			 "%s is already defined in this scope",
			 get (identifier));
	}
    }
}

static void
enter_symbols_helper (ast_t * node)
{
  switch (ast_kind (node))
    {
    case AstAction:
    case AstBind:
    case AstBindStatement:
    case AstMethod:
    case AstExpression:
    case AstIdentifier:
    case AstIdentifierList:
    case AstReceiverDefinition:
    case AstReaction:
    case AstStatement:
    case AstTopLevelList:
    case AstTypeSpecification:
      break;

    case AstInstance:
      enter_undefined_symbol (node, ast_get_child (node, INSTANCE_IDENTIFIER),
			      SymbolInstance);
      break;

    case AstTypeDefinition:
      enter_undefined_symbol (node, ast_get_child (node, TYPE_IDENTIFIER),
			      SymbolType);
      break;
    }

  AST_FOREACH (child, node)
    {
      enter_symbols_helper (child);
    }
}

static type_t* bool_type = NULL;
static type_t* uint_type = NULL;
static type_t* string_type = NULL;

void
enter_symbols (ast_t * node)
{
  /* Construct the top-level table. */
  symtab_t *symtab = symtab_get_root (ast_get_symtab (node));

  /* Insert types. */
  {
    if (bool_type == NULL)
      {
        bool_type = type_make_bool ();
      }
    symtab_enter (symtab,
		  symbol_make_type (enter ("bool"), bool_type, node));

    if (uint_type == NULL)
      {
        uint_type = type_make_uint ();
      }
    symtab_enter (symtab,
		  symbol_make_type (enter ("uint"), uint_type, node));

    if (string_type == NULL)
      {
        string_type = type_make_string ();
      }
    symtab_enter (symtab,
		  symbol_make_type (enter ("string"), string_type, node));
  }

  /* Insert zero constant. */
  symtab_enter (symtab,
                symbol_make_typed_constant (enter ("nil"),
                                            typed_value_make_nil (),
                                            node));

  /* Insert untyped boolean constants. */
  symtab_enter (symtab,
		symbol_make_typed_constant (enter ("true"),
                                            typed_value_make_bool (type_make_untyped_bool (), true),
                                            node));
  symtab_enter (symtab,
		symbol_make_typed_constant (enter ("false"),
                                            typed_value_make_bool (type_make_untyped_bool (), false),
                                            node));

  enter_symbols_helper (node);
}

// Look up a symbol.  If it is not defined, process its definition.
static symbol_t *
lookup_force (ast_t * node, string_t identifier)
{
  symtab_t *symtab = ast_get_symtab (node);
  symbol_t *symbol = symtab_find (symtab, identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "%s was not declared in this scope", get (identifier));
    }
  if (symbol_get_in_progress (symbol))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "%s is defined recursively", get (identifier));
    }
  if (!symbol_defined (symbol))
    {
      // Process the definition.
      unimplemented;
    }

  return symbol;
}

static symbol_t *
lookup_no_force (ast_t * node, string_t identifier)
{
  symtab_t *symtab = ast_get_symtab (node);
  symbol_t *symbol = symtab_find (symtab, identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "%s was not declared in this scope", get (identifier));
    }
  return symbol;
}

static type_t *
process_type_spec (ast_t * node, bool force_identifiers)
{
  assert (ast_kind (node) == AstTypeSpecification);
  switch (ast_type_specification_kind (node))
    {
    case AstComponent:
      return
	type_make_component (process_type_spec
			     (ast_get_child (node, COMPONENT_FIELD_LIST), true));

    case AstEmptyTypeSpec:
      return type_make_void ();

    case AstFieldList:
      {
	type_t *field_list = type_make_field_list ();
	AST_FOREACH (child, node)
          {
            ast_t *identifier_list = ast_get_child (child, IDENTIFIER_LIST);
            ast_t *type_spec = ast_get_child (child, TYPE_SPEC);
            type_t *type = process_type_spec (type_spec, true);
            AST_FOREACH (id, identifier_list)
              {
                string_t identifier = ast_get_identifier (id);
                const type_t *field = type_select (field_list, identifier);
                if (field == NULL)
                  {
                    type_field_list_append (field_list, identifier, type);
                  }
                else
                  {
                    error_at_line (-1, 0, ast_file (id), ast_line (id),
                                   "duplicate field name %s", get (identifier));
                  }
              }
          }
	return field_list;
      }

    case AstForeign:
      return
	type_make_foreign (process_type_spec
                           (ast_get_child (node, FOREIGN_BASE_TYPE), false));

    case AstHeap:
      return
	type_make_heap (process_type_spec
                        (ast_get_child (node, HEAP_BASE_TYPE), false));

    case AstIdentifierListTypeSpec:
      not_reached;

    case AstIdentifierTypeSpec:
      {
	ast_t *child = ast_get_child (node, IDENTIFIER_TYPE_SPEC_CHILD);
	string_t identifier = ast_get_identifier (child);
	symbol_t *symbol;
        if (force_identifiers)
          {
            symbol = lookup_force (child, identifier);
          }
        else
          {
            symbol = lookup_no_force (child, identifier);
          }

	if (symbol_kind (symbol) != SymbolType)
	  {
	    error_at_line (-1, 0, ast_file (child), ast_line (child),
			   "%s does not refer to a type", get (identifier));
	  }
	return symbol_get_type_type (symbol);
      }

    case AstPointer:
      return
	type_make_pointer (process_type_spec
                           (ast_get_child (node, POINTER_BASE_TYPE), false));

    case AstPointerToForeign:
      return
	type_make_pointer_to_foreign (process_type_spec
                                      (ast_get_child (node, POINTER_BASE_TYPE), false));

    case AstPointerToImmutable:
      return
	type_make_pointer_to_immutable (process_type_spec
                                        (ast_get_child (node, POINTER_BASE_TYPE), false));

    case AstPort:
      {
        type_t* signature = process_type_spec (ast_get_child (node, PORT_SIGNATURE), true);
        if (type_leaks_non_foreign_pointers (signature))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "signature leaks pointers");
          }
        return type_make_port (signature);
      }

    case AstSignature:
      {
	type_t *signature = type_make_signature ();
	AST_FOREACH (child, node)
          {
            ast_t *identifier_list = ast_get_child (child, IDENTIFIER_LIST);
            ast_t *type_spec = ast_get_child (child, TYPE_SPEC);
            type_t *type = process_type_spec (type_spec, true);
            AST_FOREACH (id, identifier_list)
              {
                string_t identifier = ast_get_identifier (id);
                const parameter_t *parameter =
                  type_signature_find (signature, identifier);
                if (parameter == NULL)
                  {
                    type_signature_append (signature, identifier, type, false);
                  }
                else
                  {
                    error_at_line (-1, 0, ast_file (id), ast_line (id),
                                   "duplicate parameter name %s",
                                   get (identifier));
                  }
              }
          }
	return signature;
      }

    case AstStruct:
      return
	type_make_struct (process_type_spec
                          (ast_get_child (node, STRUCT_FIELD_LIST), true));
    }

  not_reached;
}

void
process_declarations (ast_t * node)
{
  switch (ast_kind (node))
    {
    case AstAction:
      {
	ast_t *receiver = ast_get_child (node, ACTION_RECEIVER);
	ast_t *type_node = ast_get_child (receiver, RECEIVER_TYPE_IDENTIFIER);
	string_t type_identifier = ast_get_identifier (type_node);
	symbol_t *symbol = lookup_force (type_node, type_identifier);
	if (symbol_kind (symbol) != SymbolType)
	  {
	    error_at_line (-1, 0, ast_file (type_node), ast_line (type_node),
			   "%s does not refer to a type",
			   get (type_identifier));
	  }
	type_t *type = symbol_get_type_type (symbol);
	if (!type_is_component (type))
	  {
	    error_at_line (-1, 0, ast_file (type_node), ast_line (type_node),
			   "%s does not refer to a component",
			   get (type_identifier));
	  }
	action_t *action = type_component_add_action (type, node);
	symtab_set_current_action (ast_get_symtab (node), action);
	symtab_set_current_receiver_type (ast_get_symtab (node), type);
      }
      break;
    case AstBind:
      {
	ast_t *receiver = ast_get_child (node, BIND_RECEIVER);
	ast_t *type_node = ast_get_child (receiver, RECEIVER_TYPE_IDENTIFIER);
	string_t type_identifier = ast_get_identifier (type_node);
	symbol_t *symbol = lookup_force (type_node, type_identifier);
	if (symbol_kind (symbol) != SymbolType)
	  {
	    error_at_line (-1, 0, ast_file (type_node), ast_line (type_node),
			   "%s does not refer to a type",
			   get (type_identifier));
	  }
	type_t *type = symbol_get_type_type (symbol);
	if (!type_is_component (type))
	  {
	    error_at_line (-1, 0, ast_file (type_node), ast_line (type_node),
			   "%s does not refer to a component",
			   get (type_identifier));
	  }
	symtab_set_current_receiver_type (ast_get_symtab (node), type);
      }
      break;
    case AstBindStatement:
      unimplemented;
    case AstExpression:
      unimplemented;
    case AstMethod:
      {
	ast_t *receiver = ast_get_child (node, METHOD_RECEIVER);
	ast_t *type_node = ast_get_child (receiver, RECEIVER_TYPE_IDENTIFIER);
	ast_t *signature_node = ast_get_child (node, METHOD_SIGNATURE);
	ast_t *return_type_node = ast_get_child (node, METHOD_RETURN_TYPE);
	ast_t *identifier_node = ast_get_child (node, METHOD_IDENTIFIER);
	string_t identifier = ast_get_identifier (identifier_node);
	string_t type_identifier = ast_get_identifier (type_node);
	symbol_t *symbol = lookup_force (type_node, type_identifier);
	if (symbol_kind (symbol) != SymbolType)
	  {
	    error_at_line (-1, 0, ast_file (type_node), ast_line (type_node),
			   "%s does not refer to a type",
			   get (type_identifier));
	  }
	type_t *type = symbol_get_type_type (symbol);
	const type_t *t = type_select (type, identifier);
	if (t != NULL)
	  {
	    error_at_line (-1, 0, ast_file (identifier_node),
			   ast_line (identifier_node),
			   "component already contains a member named %s",
			   get (identifier));
	  }

        /* Determine the type of this. */
        type_t* this_type;
        switch (ast_receiver_kind (receiver))
          {
          case AstPointerReceiver:
            this_type = type_make_pointer (type);
            break;
          case AstPointerToImmutableReceiver:
            this_type = type_make_pointer_to_immutable (type);
            break;
          }

	/* Process the signature. */
	type_t *signature = process_type_spec (signature_node, true);
        /* Prepend the signature with the receiver. */
        type_signature_prepend (signature,
                                ast_get_identifier (ast_get_child (receiver, RECEIVER_THIS_IDENTIFIER)),
                                this_type, true);

        /* Process the return type. */
        type_t *return_type = process_type_spec (return_type_node, true);

        method_t* method = type_add_method (type, node, identifier, signature, return_type);
	symtab_set_current_method (ast_get_symtab (node), method);
	symtab_set_current_receiver_type (ast_get_symtab (node), type);
      }
      break;
    case AstIdentifier:
      unimplemented;
    case AstIdentifierList:
      unimplemented;
    case AstInstance:
      {
	ast_t *type_identifier_node =
	  ast_get_child (node, INSTANCE_TYPE_IDENTIFIER);
	string_t type_identifier = ast_get_identifier (type_identifier_node);
	symbol_t *symbol =
	  lookup_force (type_identifier_node,
                        ast_get_identifier (type_identifier_node));
	if (symbol_kind (symbol) != SymbolType)
	  {
	    error_at_line (-1, 0, ast_file (type_identifier_node),
			   ast_line (type_identifier_node),
			   "%s does not refer to a type",
			   get (type_identifier));
	  }
	type_t *type = symbol_get_type_type (symbol);
	if (!type_is_component (type))
	  {
	    error_at_line (-1, 0, ast_file (type_identifier_node),
			   ast_line (type_identifier_node),
			   "%s does not refer to a component",
			   get (type_identifier));
	  }
	symbol = ast_get_symbol (node);
	symbol_set_instance_type (symbol, type);
      }
      break;
    case AstReceiverDefinition:
      unimplemented;
    case AstReaction:
      {
	ast_t *receiver = ast_get_child (node, REACTION_RECEIVER);
	ast_t *type_node = ast_get_child (receiver, RECEIVER_TYPE_IDENTIFIER);
	ast_t *signature_node = ast_get_child (node, REACTION_SIGNATURE);
	ast_t *identifier_node = ast_get_child (node, REACTION_IDENTIFIER);
	string_t identifier = ast_get_identifier (identifier_node);
	string_t type_identifier = ast_get_identifier (type_node);
	symbol_t *symbol = lookup_force (type_node, type_identifier);
	if (symbol_kind (symbol) != SymbolType)
	  {
	    error_at_line (-1, 0, ast_file (type_node), ast_line (type_node),
			   "%s does not refer to a type",
			   get (type_identifier));
	  }
	type_t *type = symbol_get_type_type (symbol);
	if (!type_is_component (type))
	  {
	    error_at_line (-1, 0, ast_file (type_node), ast_line (type_node),
			   "%s does not refer to a component",
			   get (type_identifier));
	  }
	const type_t *t = type_select (type, identifier);
	if (t != NULL)
	  {
	    error_at_line (-1, 0, ast_file (identifier_node),
			   ast_line (identifier_node),
			   "component already contains a member named %s",
			   get (identifier));
	  }

	/* Process the signature. */
	type_t *signature = process_type_spec (signature_node, true);

	action_t *action =
	  type_component_add_reaction (type, node, identifier, signature);
	symtab_set_current_action (ast_get_symtab (node), action);
	symtab_set_current_receiver_type (ast_get_symtab (node), type);
      }
      break;
    case AstStatement:
      unimplemented;
    case AstTopLevelList:
      {
	AST_FOREACH (child, node)
          {
            process_declarations (child);
          }
      }
      break;
    case AstTypeDefinition:
      {
	symbol_t *symbol = ast_get_symbol (node);
	string_t identifier = symbol_identifier (symbol);
	type_t *type = symbol_get_type_type (symbol);

	if (symbol_defined (symbol))
	  break;

	if (symbol_get_in_progress (symbol))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "%s is defined recursively", get (identifier));
	  }

	symbol_set_in_progress (symbol, true);
	ast_t *type_spec_node = ast_get_child (node, TYPE_TYPE_SPEC);
	type_t *new_type = process_type_spec (type_spec_node, true);
	type_move_guts_to (type, new_type);
	symbol_set_in_progress (symbol, false);
      }
      break;
    case AstTypeSpecification:
      unimplemented;
    }
}

static void check_rvalue (ast_t ** ptr, binding_t* binding, bool output);

static void check_identifier_expr (ast_t** ptr)
{
  ast_t *node = *ptr;
  ast_t *identifier_node = ast_get_child (node, UNARY_CHILD);
  string_t identifier = ast_get_identifier (identifier_node);
  symtab_t *symtab = ast_get_symtab (node);
  symbol_t *symbol = symtab_find (symtab, identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, ast_file (identifier_node),
                     ast_line (identifier_node), "%s is not defined",
                     get (identifier));
    }

  switch (symbol_kind (symbol))
    {
    case SymbolInstance:
      unimplemented;

    case SymbolParameter:
      {
        type_t* t = symbol_parameter_type (symbol);
        ast_set_type (*ptr, typed_value_make (t),
                      symbol_parameter_kind (symbol) ==
                      ParameterReceiver
                      || symbol_parameter_kind (symbol) ==
                      ParameterReceiverDuplicate);
      }
      break;

    case SymbolType:
      unimplemented;

    case SymbolTypedConstant:
      ast_set_type (*ptr, symbol_typed_constant_value (symbol), false);
      break;

    case SymbolVariable:
      ast_set_type (*ptr, typed_value_make (symbol_variable_type (symbol)), false);
      break;
    }

  ast_set_symbol (*ptr, symbol);
}

static void
set_dereference_type (ast_t* node, typed_value_t tv)
{
  tv = typed_value_dereference (tv);
  ast_set_type (node, tv,
                ast_get_derived_from_receiver (ast_get_child (node, UNARY_CHILD)));
}

typedef union {
  field_t * field;
  action_t * action;
} FieldOrAction;

static FieldOrAction make_field (field_t * field)
{
  FieldOrAction foa = { field: field };
  return foa;
}

static FieldOrAction make_action (action_t * action)
{
  FieldOrAction foa = { action: action };
  return foa;
}

typedef struct {
  ast_t** ptr;
  binding_t* binding;
  bool output;
  FieldOrAction retval;
} check_lvalue_data_t;

static void
check_lvalue_dereference_expr (void* data, ast_t* node)
{
  check_lvalue_data_t* d = data;
  ast_t **child = ast_get_child_ptr (node, UNARY_CHILD);
  check_rvalue (child, d->binding, d->output);
  typed_value_t tv = ast_get_typed_value (*child);
  if (!typed_value_can_dereference (tv))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "cannot dereference non-pointer");
    }
  set_dereference_type (node, tv);
  d->retval = make_field (NULL);
}

static void
check_lvalue_identifier_expr (void* data, ast_t* node)
{
  check_lvalue_data_t* d = data;
  check_identifier_expr (d->ptr);
  d->retval = make_field (NULL);
}

static FieldOrAction
check_lvalue (ast_t ** ptr, binding_t* binding, bool output);

static void
check_lvalue_select_expr (void* data, ast_t* node)
{
  check_lvalue_data_t* d = data;
  ast_t **left = ast_get_child_ptr (node, BINARY_LEFT_CHILD);
  ast_t *right = ast_get_child (node, BINARY_RIGHT_CHILD);
  string_t identifier = ast_get_identifier (right);
  check_lvalue (left, d->binding, d->output);
  typed_value_t tv = ast_get_typed_value (*left);

  type_t* t = tv.type;
  if (type_is_immutable (t))
    {
      t = type_immutable_base_type (t);
    }
  else if (type_is_foreign (t))
    {
      t = type_foreign_base_type (t);
    }

  if (type_is_any_pointer (t))
    {
      // Selecting from a pointer.  Insert a dereference.
      ast_t* deref = ast_make_dereference (ast_line (node), *left);
      set_dereference_type (deref, tv);
      *left = deref;
      tv = ast_get_typed_value (*left);
    }

  if (!typed_value_can_select (tv, identifier))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "cannot select %s from expression of type %s",
                     get (identifier), type_to_string (tv.type));
    }

  typed_value_t selected_tv = typed_value_select (tv, identifier);

  ast_set_type (node, selected_tv,
                ast_get_derived_from_receiver (*left));

  if (d->binding != NULL)
    {
      if (type_is_component (selected_tv.type) || type_is_port (selected_tv.type))
        {
          field_t *field = type_select_field (tv.type, identifier);
          assert (field != NULL);
          if (d->output)
            {
              binding_add_output (d->binding, field);
            }
          else
            {
              binding_add_input (d->binding, field);
            }
          d->retval = make_field (field);
          return;
        }
      else if (type_is_reaction (selected_tv.type))
        {
          action_t * reaction = type_component_get_reaction (tv.type, identifier);
          assert (reaction != NULL);
          binding_set_reaction (d->binding, reaction);
          d->retval = make_action (reaction);
          return;
        }
    }

  d->retval = make_field (NULL);
}

static ast_visitor_t check_lvalue_visitor = {
 visit_dereference_expr: check_lvalue_dereference_expr,
 visit_identifier_expr: check_lvalue_identifier_expr,
 visit_select_expr: check_lvalue_select_expr,
};

static FieldOrAction
check_lvalue (ast_t ** ptr, binding_t* binding, bool output)
{
  check_lvalue_data_t data = { ptr: ptr, binding: binding, output: output };
  ast_accept (*ptr, &check_lvalue_visitor, &data);
  return data.retval;
}

static void
convert_if_untyped (typed_value_t target_tv, ast_t * source)
{
  typed_value_t source_tv = ast_get_typed_value (source);
  if (typed_value_is_untyped (source_tv))
    {
      if (typed_value_can_convert (source_tv, target_tv.type))
        {
          source_tv = typed_value_convert (source_tv, target_tv.type);
          ast_set_typed_value (source, source_tv);
        }
      else
        {
          error_at_line (-1, 0, ast_file (source), ast_line (source),
                         "cannot convert");
        }
    }
}

static trigger_t *
get_current_trigger (const ast_t * node)
{
  return symtab_get_current_trigger (ast_get_symtab (node));
}

static void check_rvalue_list (ast_t * node);

static void
check_call (ast_t * node, ast_t * args, const type_t * expr_type)
{
  size_t idx;
  size_t argument_count = ast_children_count (args);
  size_t parameter_count = type_parameter_count (expr_type);
  if (argument_count != parameter_count)
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
		     "call expects %zd arguments but given %zd",
		     parameter_count, argument_count);
    }

  bool derived_from_receiver = false;

  for (idx = 0; idx != argument_count; ++idx)
    {
      ast_t *arg = ast_get_child (args, idx);
      typed_value_t argument_tv  = ast_get_typed_value (arg);
      type_t *parameter_type = type_parameter_type (expr_type, idx);
      typed_value_t parameter_tv = typed_value_make (parameter_type);
      if (!typed_value_convertible (parameter_tv, argument_tv))
        {
          error_at_line (-1, 0, ast_file (arg),
                         ast_line (arg),
                         "cannot convert %s to %s", type_to_string (argument_tv.type), type_to_string (parameter_tv.type));
        }
      if (!typed_value_copyable (argument_tv))
        {
          error_at_line (-1, 0, ast_file (arg),
                         ast_line (arg),
                         "cannot copy values of type %s", type_to_string (argument_tv.type));
        }

      // Convert since we must compute.
      convert_if_untyped (parameter_tv, arg);

      if (ast_get_derived_from_receiver (arg))
        {
          derived_from_receiver = true;
          trigger_t *trigger = get_current_trigger (node);
          if (trigger != NULL)
            {
              trigger_set_mutates_receiver (trigger, true);
            }
        }
    }

  // Set the return type.
  // TODO:  Don't know that we can assume the return is derived from immutable.
  ast_set_type (node, typed_value_make (type_return_type (expr_type)), derived_from_receiver);
}

static action_t *
get_current_action (const ast_t * node)
{
  return symtab_get_current_action (ast_get_symtab (node));
}

method_t *
get_current_method (const ast_t * node)
{
  return symtab_get_current_method (ast_get_symtab (node));
}

static type_t *
get_current_return_type (const ast_t * node)
{
  method_t* g = get_current_method (node);
  if (g != NULL)
    {
      return method_return_type (g);
    }

  return NULL;
}

static type_t *
get_current_receiver_type (const ast_t * node)
{
  return symtab_get_current_receiver_type (ast_get_symtab (node));
}

static bool
in_trigger_statement (const ast_t * node)
{
  symtab_t *symtab = ast_get_symtab (node);
  return symtab_get_current_trigger (symtab) != NULL;
}

static bool
in_mutable_section (const ast_t* node)
{
  return get_current_action (node) != NULL &&
    get_current_trigger (node) != NULL;
}

static void
check_comparison (ast_t* node)
{
  ast_t **left = ast_get_child_ptr (node, BINARY_LEFT_CHILD);
  check_rvalue (left, NULL, false);
  typed_value_t left_tv = ast_get_typed_value (*left);
  ast_t **right = ast_get_child_ptr (node, BINARY_RIGHT_CHILD);
  check_rvalue (right, NULL, false);
  typed_value_t right_tv = ast_get_typed_value (*right);
  if (!(typed_value_convertible (left_tv, right_tv) ||
        typed_value_convertible (right_tv, left_tv)))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "cannot convert %s to %s in comparison", type_to_string (right_tv.type), type_to_string (left_tv.type));

    }

  convert_if_untyped (left_tv, *right);
  convert_if_untyped (right_tv, *left);

  ast_set_type (node, typed_value_make (type_make_untyped_bool ()), false);
}

typedef struct {
  method_t* retval;
} extract_method_data_t;

static void
extract_method_select_expr (void* data, const ast_t* node)
{
  extract_method_data_t* d = data;
  ast_t *left = ast_get_child (node, BINARY_LEFT_CHILD);
  ast_t *right = ast_get_child (node, BINARY_RIGHT_CHILD);
  string_t identifier = ast_get_identifier (right);

  type_t* t = ast_get_typed_value (left).type;

  if (type_is_pointer (t))
    {
      // Selecting from a pointer.  Insert a dereference.
      t = type_pointer_base_type (t);
    }

  d->retval = type_get_method (t, identifier);
}

static ast_const_visitor_t extract_method_visitor = {
 visit_select_expr: extract_method_select_expr
};

static method_t*
extract_method (const ast_t* node)
{
  extract_method_data_t data;
  ast_const_accept (node, &extract_method_visitor, &data);
  return data.retval;
}

typedef struct {
  ast_t** ptr;
  binding_t* binding;
  bool output;
} check_rvalue_data_t;

static void
check_rvalue_typed_literal (void* data, ast_t* node)
{
  // Do nothing.
}

static void
check_rvalue_address_of_expr (void* data, ast_t* node)
{
  check_rvalue_data_t* d = data;
  ast_t **expr = ast_get_child_ptr (node, UNARY_CHILD);
  check_lvalue (expr, d->binding, d->output);
  typed_value_t expr_tv = ast_get_typed_value (*expr);
  type_t* type;
  // TODO:  Move to typed_value.
  if (type_is_foreign (expr_tv.type))
    {
      type = type_make_pointer_to_foreign (type_foreign_base_type (expr_tv.type));
    }
  else if (type_is_immutable (expr_tv.type))
    {
      type = type_make_pointer_to_immutable (type_immutable_base_type (expr_tv.type));
    }
  else
    {
      type = type_make_pointer (expr_tv.type);
    }

  ast_set_type (node, typed_value_make (type), ast_get_derived_from_receiver (*expr));
}

static void
check_rvalue_call_expr (void* data, ast_t* node)
{
  check_rvalue_data_t* d = data;
  ast_t **expr = ast_get_child_ptr (node, CALL_EXPR);
  ast_t *args = ast_get_child (node, CALL_ARGS);

  // Analyze the callee.
  check_rvalue (expr, d->binding, d->output);

  typed_value_t expr_tv = ast_get_typed_value (*expr);
  if (!type_callable (expr_tv.type))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "cannot call");
    }

  method_t* method = extract_method (*expr);
  if (method != NULL)
    {
      // Calling a method.

      // The receiver is either a copy or a pointer.
      bool receiver_is_pointer = type_is_any_pointer (type_parameter_type (expr_tv.type, 0));

      // Transfer the computation for the receiver to the argument list.
      ast_t* receiver_select_expr = ast_get_child (*expr, BINARY_LEFT_CHILD);
      if (receiver_is_pointer)
        {
          ast_t* e = ast_make_address_of (ast_line (node), receiver_select_expr);
          ast_prepend_child (args, e);
        }
      else
        {
          ast_prepend_child (args, receiver_select_expr);
        }

      if (in_mutable_section (node))
        {
          type_t* t = method_named_type (method);
          if (type_is_component (t))
            {
              // Invoking a method on a component in a mutable section.
              // Ensure the receiver is this.
              unimplemented;
            }
        }

      // Replace the callee with a literal or expression.
      *expr = ast_make_typed_literal (ast_line (node), typed_value_make_method (method));
    }
  else
    {
      unimplemented;
    }

  check_rvalue_list (args);
  check_call (node, args, expr_tv.type);
}

static void
check_rvalue_dereference_expr (void* data, ast_t* node)
{
  check_rvalue_data_t* d = data;
  ast_t** child = ast_get_child_ptr (node, UNARY_CHILD);
  check_rvalue (child, d->binding, d->output);
  typed_value_t tv = ast_get_typed_value (*child);
  if (!typed_value_can_dereference (tv))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "cannot apply @ to expression of type %s", type_to_string (tv.type));
    }

  ast_set_type (node, typed_value_dereference (tv), ast_get_derived_from_receiver (*child));
}

static void
check_rvalue_equal_expr (void* data, ast_t* node)
{
  check_comparison (node);
}

static void
check_rvalue_identifier_expr (void* data, ast_t* node)
{
  check_rvalue_data_t* d = data;
  check_identifier_expr (d->ptr);
}

static void
check_rvalue_logic_not_expr (void* data, ast_t* node)
{
  check_rvalue_data_t* d = data;
  ast_t **child = ast_get_child_ptr (node, UNARY_CHILD);
  check_rvalue (child, d->binding, d->output);
  typed_value_t tv = ast_get_typed_value (*child);
  if (!typed_value_is_boolean (tv))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "cannot apply ! to expression of type %s", type_to_string (tv.type));
    }
  tv = typed_value_logic_not (tv);
  ast_set_type (node, tv,
                ast_get_derived_from_receiver (*child));
}

static void
check_rvalue_merge_expr (void* data, ast_t* node)
{
  check_rvalue_data_t* d = data;
  ast_t** child = ast_get_child_ptr (node, UNARY_CHILD);
  check_rvalue (child, d->binding, d->output);
  typed_value_t tv = ast_get_typed_value (*child);
  if (!typed_value_is_mergeable (tv))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "cannot merge expression of type %s", type_to_string (tv.type));
    }
  tv = typed_value_merge (tv);
  ast_set_type (node, tv,
                ast_get_derived_from_receiver (*child));
}

static void
check_rvalue_move_expr (void* data, ast_t* node)
{
  check_rvalue_data_t* d = data;
  ast_t** child = ast_get_child_ptr (node, UNARY_CHILD);
  check_rvalue (child, d->binding, d->output);
  typed_value_t tv = ast_get_typed_value (*child);
  if (!typed_value_is_moveable (tv))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "cannot move expression of type %s", type_to_string (tv.type));
    }
  tv = typed_value_move (tv);
  ast_set_type (node, tv,
                ast_get_derived_from_receiver (*child));
}

static void
check_rvalue_new_expr (void* data, ast_t* node)
{
  ast_t* type_spec = ast_get_child (node, UNARY_CHILD);
  type_t* t = process_type_spec (type_spec, false);
  type_t* type = type_make_pointer (t);
  ast_set_type (node, typed_value_make (type), false);
}

static void
check_rvalue_port_call_expr (void* data, ast_t* node)
{
  ast_t *expr = ast_get_child (node, CALL_EXPR);
  string_t port_identifier = ast_get_identifier (expr);
  const type_t *this_type =
    type_pointer_base_type (symtab_get_this_type
                            (ast_get_symtab (node)));
  const type_t *port_type = type_select (this_type, port_identifier);

  if (port_type == NULL)
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "no port named %s", get (port_identifier));
    }

  if (!type_is_port (port_type))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "%s is not a port", get (port_identifier));
    }

  ast_t *args = ast_get_child (node, CALL_ARGS);
  check_rvalue_list (args);

  check_call (node, args, port_type);
}

static void
check_rvalue_select_expr (void* data, ast_t* node)
{
  check_rvalue_data_t* d = data;
  check_lvalue (d->ptr, d->binding, d->output);
}

static ast_visitor_t check_rvalue_visitor = {
 visit_typed_literal: check_rvalue_typed_literal,
 visit_address_of_expr: check_rvalue_address_of_expr,
 visit_call_expr: check_rvalue_call_expr,
 visit_dereference_expr: check_rvalue_dereference_expr,
 visit_equal_expr: check_rvalue_equal_expr,
 visit_identifier_expr: check_rvalue_identifier_expr,
 visit_logic_not_expr: check_rvalue_logic_not_expr,
 visit_merge_expr: check_rvalue_merge_expr,
 visit_move_expr: check_rvalue_move_expr,
 visit_new_expr: check_rvalue_new_expr,
 visit_not_equal_expr: check_rvalue_equal_expr,
 visit_port_call_expr: check_rvalue_port_call_expr,
 visit_select_expr: check_rvalue_select_expr,
};

static void
check_rvalue (ast_t ** ptr, binding_t* binding, bool output)
{
  check_rvalue_data_t data = { ptr: ptr, binding: binding, output: output };
  ast_accept (*ptr, &check_rvalue_visitor, &data);
}

static void
check_rvalue_list_expr_list (void* data, ast_t* node)
{
  AST_FOREACH_PTR (child, node)
  {
    check_rvalue (child, NULL, false);
  }
}

static ast_visitor_t check_rvalue_list_visitor = {
 visit_expr_list: check_rvalue_list_expr_list
};

static void
check_rvalue_list (ast_t * node)
{
  ast_accept (node, &check_rvalue_list_visitor, NULL);
}

static void
convert_rvalue_to_builtin_types (ast_t ** ptr)
{
  ast_t *node = *ptr;
  assert (ast_kind (node) == AstExpression);

  typed_value_t tv = ast_get_typed_value (node);
  switch (type_kind (tv.type))
    {
    case TypeBool:
    case TypeComponent:
    case TypeFieldList:
    case TypeForeign:
    case TypeFunc:
    case TypeHeap:
    case TypeImmutable:
    case TypePointer:
    case TypePointerToForeign:
    case TypePointerToImmutable:
    case TypePort:
    case TypeReaction:
    case TypeSignature:
    case TypeString:
    case TypeStruct:
    case TypeUint:
    case TypeVoid:
    case TypeUndefined:
      // Do nothing.
      break;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;
    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      ast_set_typed_value (node, typed_value_convert (tv, string_type));
      break;
    }
}

static void
convert_rvalue_list_to_builtin_types_list (void* data, ast_t* node)
{
  AST_FOREACH_PTR (child, node)
  {
    convert_rvalue_to_builtin_types (child);
  }
}

static ast_visitor_t convert_rvalue_list_to_builtin_types_visitor = {
 visit_expr_list: convert_rvalue_list_to_builtin_types_list
};

typedef struct {
  field_t* retval;
} extract_port_field_data_t;

static void
convert_rvalue_list_to_builtin_types (ast_t * node)
{
  ast_accept (node, &convert_rvalue_list_to_builtin_types_visitor, NULL);
}

static void
extract_port_field_port_call_expr (void* data, const ast_t* node)
{
  extract_port_field_data_t* d = data;
  string_t port_identifier =
    ast_get_identifier (ast_get_child (node, CALL_EXPR));
  const type_t *this_type =
    type_pointer_base_type (symtab_get_this_type (ast_get_symtab (node)));
  const type_t *field_list = type_component_field_list (this_type);
  d->retval = type_field_list_find (field_list, port_identifier);
}

static ast_const_visitor_t extract_port_field_visitor = {
 visit_port_call_expr: extract_port_field_port_call_expr
};

static field_t *
extract_port_field (const ast_t * node)
{
  extract_port_field_data_t data;
  ast_const_accept (node, &extract_port_field_visitor, &data);
  return data.retval;
}

static void
check_assignment_target (ast_t** left, ast_t* node)
{
  check_lvalue (left, NULL, false);
  typed_value_t tv = ast_get_typed_value (*left);
  if (!typed_value_can_assign (tv))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "cannot assign to read-only location of type %s", type_to_string (tv.type));
    }
  if (ast_get_derived_from_receiver (*left))
    {
      trigger_t *trigger = get_current_trigger (node);
      if (trigger != NULL)
        {
          trigger_set_mutates_receiver (trigger, true);
        }
    }
}

static void
check_statement (ast_t * node)
{
  assert (ast_kind (node) == AstStatement);
  switch (ast_statement_kind (node))
    {
    case AstAssignmentStmt:
      {
	ast_t **left = ast_get_child_ptr (node, BINARY_LEFT_CHILD);
        check_assignment_target (left, node);
	ast_t **right = ast_get_child_ptr (node, BINARY_RIGHT_CHILD);
	check_rvalue (right, NULL, false);
	typed_value_t left_tv = ast_get_typed_value (*left);
        typed_value_t right_tv = ast_get_typed_value (*right);

        if (!typed_value_copyable (right_tv))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "values of type %s cannot be assigned", type_to_string (left_tv.type));
          }

        if (type_is_immutable (right_tv.type) &&
            type_leaks_mutable_pointers (right_tv.type))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "values of type %s cannot be assigned because it leaks pointers", type_to_string (right_tv.type));
          }

        if (!typed_value_convertible (left_tv, right_tv))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot convert %s to %s in assignment", type_to_string (right_tv.type), type_to_string (left_tv.type));
          }

        if (type_contains_foreign_pointers (right_tv.type))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "assignment leaks foreign pointer");
          }

        // Convert the rvalue since we must compute it.
        convert_if_untyped (left_tv, *right);
      }
      break;

    case AstChangeStmt:
      {
        ast_t** expr = ast_get_child_ptr (node, CHANGE_EXPR);
        string_t identifier = ast_get_identifier (ast_get_child (node, CHANGE_IDENTIFIER));
        ast_t* type = ast_get_child (node, CHANGE_TYPE);
        ast_t* body = ast_get_child (node, CHANGE_BODY);

        // Process the expression.
        check_rvalue (expr, NULL, false);
        typed_value_t tv = ast_get_typed_value (*expr);
        if (!typed_value_is_changeable (tv))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot change expression of type %s", type_to_string (tv.type));
          }

        // Process the root variable.
        type_t* t = process_type_spec (type, false);
        if (!type_is_valid_root (t, tv.type))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "expression of type %s is not a valid root", type_to_string (t));
          }

        // Enter the new heap root.
        symbol_t* symbol = symbol_make_variable (identifier, t, node);
        enter_symbol (body, symbol);

        // Enter all parameters and variables in scope that are pointers as pointers to foreign.
        symtab_change (ast_get_symtab (body));

        // Check the body.
        check_statement (body);
      }
      break;

    case AstExpressionStmt:
      {
        ast_t **child = ast_get_child_ptr (node, UNARY_CHILD);
        check_rvalue (child, NULL, false);
      }
      break;

    case AstIfStmt:
      {
        ast_t** condition_node = ast_get_child_ptr (node, IF_CONDITION);
        ast_t** true_branch = ast_get_child_ptr (node, IF_TRUE_BRANCH);

        // Check the condition.
        check_rvalue (condition_node, NULL, false);

        typed_value_t tv = ast_get_typed_value (*condition_node);
        if (!typed_value_is_boolean (tv))
          {
	    error_at_line (-1, 0, ast_file (*condition_node),
			   ast_line (*condition_node),
			   "condition is not a boolean expression");
          }

        /* Must be typed since it will be evaluated. */
        convert_if_untyped (typed_value_make (bool_type), *condition_node);

        // Check the branch.
        check_statement (*true_branch);
      }
      break;

    case AstAddAssignStmt:
      {
	ast_t **left = ast_get_child_ptr (node, BINARY_LEFT_CHILD);
        check_assignment_target (left, node);
	ast_t **right = ast_get_child_ptr (node, BINARY_RIGHT_CHILD);
	check_rvalue (right, NULL, false);
	typed_value_t left_type = ast_get_typed_value (*left);
        typed_value_t right_type = ast_get_typed_value (*right);
        if (!typed_value_is_arithmetic (left_type))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot add to non-arithmetic type");
          }
        if (!typed_value_convertible (left_type, right_type))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot convert %s to %s in assignment", type_to_string (right_type.type), type_to_string (left_type.type));

          }
        // Convert the rvalue since we must compute it.
        convert_if_untyped (left_type, *right);
      }
      break;
    case AstStmtList:
      {
	AST_FOREACH (child, node)
          {
            check_statement (child);
          }
      }
      break;
    case AstReturnStmt:
      {
        // Check the expression.
        ast_t** expr = ast_get_child_ptr (node, UNARY_CHILD);
        check_rvalue (expr, NULL, false);
        typed_value_t expr_tv = ast_get_typed_value (*expr);

        // Check that it matches with the return type.
        type_t* return_type = get_current_return_type (node);
        assert (return_type != NULL);
        typed_value_t return_tv = typed_value_make (return_type);
        if (!typed_value_convertible (return_tv, expr_tv))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot convert %s to %s in return", type_to_string (expr_tv.type), type_to_string (return_tv.type));

          }
        // Convert the rvalue since we must compute it.
        convert_if_untyped (return_tv, *expr);
      }
      break;
    case AstTriggerStmt:
      {
	symtab_t *symtab = ast_get_symtab (node);
	action_t *action = get_current_action (node);
	assert (action != NULL);
	ast_t *expression_list_node =
	  ast_get_child (node, TRIGGER_EXPRESSION_LIST);
	ast_t *body_node = ast_get_child (node, TRIGGER_BODY);

	/* Trigger statements can only appear in actions and reactions. */
	if (get_current_action (node) == NULL)
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "trigger statements can only appear in actions and reactions");
	  }

	/* Trigger statements cannot be recursive. */
	if (in_trigger_statement (node))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "recursive trigger statement");
	  }

	trigger_t *trigger = trigger_make ();
	action_add_trigger (action, trigger);
	symtab_set_current_trigger (ast_get_symtab (body_node), trigger);

	/* Check the triggers. */
	check_rvalue_list (expression_list_node);

	AST_FOREACH (child, expression_list_node)
          {
            field_t *field = extract_port_field (child);
            assert (field != NULL);
            assert (type_is_port (field_type (field)));
            trigger_add_field (trigger, field);
          }

	/* Re-insert this as a pointer to mutable. */
	symbol_t *this_symbol = symtab_get_this (symtab);
	symbol_t *new_this_symbol =
	  symbol_make_receiver_duplicate (this_symbol);
	enter_symbol (body_node, new_this_symbol);

	/* Check the body. */
	check_statement (body_node);
      }
      break;

    case AstVarStmt:
      {
        ast_t** identifier_list = ast_get_child_ptr (node, VAR_IDENTIFIER_LIST);
        ast_t** type_spec = ast_get_child_ptr (node, VAR_TYPE_SPEC);

        // Process the type spec.
        type_t* type = process_type_spec (*type_spec, true);

        // Enter each symbol.
        AST_FOREACH (child, *identifier_list)
          {
            string_t name = ast_get_identifier (child);
            symbol_t* symbol = symbol_make_variable (name, type, child);
            enter_symbol (child, symbol);
          }
      }
      break;

    case AstPrintlnStmt:
      {
	ast_t **child = ast_get_child_ptr (node, UNARY_CHILD);
        check_rvalue_list (*child);
        // Convert to type so we can evaluate it.
        convert_rvalue_list_to_builtin_types (*child);
      }
      break;
    }
}

static void
check_bind_statement (ast_t * node)
{
  assert (ast_kind (node) == AstBindStatement);
  switch (ast_bind_statement_kind (node))
    {
    case AstBindStmtList:
      {
	AST_FOREACH (child, node)
          {
            check_bind_statement (child);
          }
      }
      break;
    case AstBindStmt:
      {
	binding_t *binding = binding_make ();

	ast_t **port_node = ast_get_child_ptr (node, BINARY_LEFT_CHILD);
	field_t *port_field = check_lvalue (port_node, binding, true).field;
	const type_t *port_type = field_type (port_field);

	if (!type_is_port (port_type))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "source of bind is not a port");
	  }

	ast_t **reaction_node = ast_get_child_ptr (node, BINARY_RIGHT_CHILD);
	action_t *reaction = check_lvalue (reaction_node, binding, false).action;
	const type_t *reaction_typ = reaction_type (reaction);

	if (!type_compatible_port_reaction (port_type, reaction_typ))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "incompatible signatures in bind");
	  }

	type_component_add_binding (get_current_receiver_type (node),
				    binding);
      }
      break;
    }
}

/* Check the semantics of all executable code. */
void
process_definitions (ast_t * node)
{
  switch (ast_kind (node))
    {
    case AstAction:
      {
	ast_t *receiver_node = ast_get_child (node, ACTION_RECEIVER);
	ast_t **precondition_node =
	  ast_get_child_ptr (node, ACTION_PRECONDITION);
	ast_t *body_node = ast_get_child (node, ACTION_BODY);
	/* Insert "this" into the symbol table. */
	ast_t *this_node =
	  ast_get_child (receiver_node, RECEIVER_THIS_IDENTIFIER);
	string_t this_identifier = ast_get_identifier (this_node);
	enter_symbol (receiver_node,
		      symbol_make_receiver (this_identifier,
                                            type_make_pointer_to_immutable (get_current_receiver_type
                                                                            (node)), this_node));

	/* Check the precondition. */
	check_rvalue (precondition_node, NULL, false);

        typed_value_t tv = ast_get_typed_value (*precondition_node);
        if (!typed_value_is_boolean (tv))
          {
	    error_at_line (-1, 0, ast_file (*precondition_node),
			   ast_line (*precondition_node),
			   "precondition is not a boolean expression");
          }

        /* Must be typed since it will be evaluated. */
        convert_if_untyped (typed_value_make (bool_type), *precondition_node);

	/* Check the body. */
	check_statement (body_node);
      }
      break;
    case AstBind:
      {
	ast_t *receiver_node = ast_get_child (node, BIND_RECEIVER);
	ast_t *body_node = ast_get_child (node, BIND_BODY);
	/* Insert "this" into the symbol table. */
	ast_t *this_node =
	  ast_get_child (receiver_node, RECEIVER_THIS_IDENTIFIER);
	string_t this_identifier = ast_get_identifier (this_node);
	enter_symbol (receiver_node,
		      symbol_make_receiver (this_identifier,
                                            type_make_pointer
                                            (get_current_receiver_type
                                             (node)), this_node));
	/* Check the body. */
	check_bind_statement (body_node);
      }
      break;
    case AstBindStatement:
      unimplemented;
    case AstExpression:
      unimplemented;
    case AstMethod:
      {
	ast_t *signature_node = ast_get_child (node, METHOD_SIGNATURE);
	ast_t *body_node = ast_get_child (node, METHOD_BODY);
        ast_t *return_type_node = ast_get_child (node, METHOD_RETURN_TYPE);
	method_t *method = get_current_method (node);

        /* Enter the return type into the symbol table. */
        enter_symbol (return_type_node, symbol_make_return_parameter (enter ("0return"), method_return_type (method), return_type_node));

	/* Enter the signature into the symbol table. */
	enter_signature (signature_node, method_signature (method));

	/* Check the body. */
	check_statement (body_node);
      }
      break;
    case AstIdentifier:
      unimplemented;
    case AstIdentifierList:
      unimplemented;
    case AstInstance:
      break;
    case AstReceiverDefinition:
      unimplemented;
    case AstReaction:
      {
	ast_t *receiver_node = ast_get_child (node, REACTION_RECEIVER);
	ast_t *signature_node = ast_get_child (node, REACTION_SIGNATURE);
	ast_t *body_node = ast_get_child (node, REACTION_BODY);
	action_t *reaction = get_current_action (node);
	/* Insert "this" into the symbol table. */
	ast_t *this_node =
	  ast_get_child (receiver_node, RECEIVER_THIS_IDENTIFIER);
	string_t this_identifier = ast_get_identifier (this_node);
	enter_symbol (receiver_node,
		      symbol_make_receiver (this_identifier,
                                            type_make_pointer_to_immutable (action_component_type (reaction)),
                                            this_node));
	/* Enter the signature into the symbol table. */
	enter_signature (signature_node, reaction_signature (reaction));

	/* Check the body. */
	check_statement (body_node);
      }
      break;
    case AstStatement:
      unimplemented;
    case AstTopLevelList:
      {
	AST_FOREACH (child, node)
          {
            process_definitions (child);
          }
      }
      break;
    case AstTypeDefinition:
      break;
    case AstTypeSpecification:
      unimplemented;
    }
}

static instance_t *
instantiate (const type_t * type, instance_table_t * instance_table, bool is_top_level)
{
  // Get the instance.
  instance_t *instance = instance_table_insert (instance_table, type, is_top_level);

  const type_t *field_list = type_component_field_list (type);

  field_t **field;
  field_t **limit;
  for (field = type_field_list_begin (field_list), limit =
         type_field_list_end (field_list); field != limit;
       field = type_field_list_next (field))
    {
      const type_t *fieldtype = field_type (*field);
      if (type_is_component (fieldtype))
	{
	  instance_t *subinstance = instantiate (fieldtype, instance_table, false);
	  instance_table_insert_subinstance (instance_table, instance, *field,
					     subinstance);
	}
    }

  return instance;
}


/*
  The goal of this function is to generate the following look-up
  tables:
  1.  (instance) -> (type)
  2.  (instance, field) -> instance
*/
void
enumerate_instances (ast_t * node, instance_table_t * instance_table)
{
  switch (ast_kind (node))
    {
    case AstAction:
      break;
    case AstBind:
      break;
    case AstBindStatement:
      unimplemented;
    case AstExpression:
      unimplemented;
    case AstMethod:
      break;
    case AstIdentifier:
      unimplemented;
    case AstIdentifierList:
      unimplemented;
    case AstInstance:
      {
	symbol_t *symbol = ast_get_symbol (node);
	const type_t *type = symbol_get_instance_type (symbol);
	instantiate (type, instance_table, true);
      }
      break;
    case AstReceiverDefinition:
      unimplemented;
    case AstReaction:
      break;
    case AstStatement:
      unimplemented;
    case AstTopLevelList:
      {
	AST_FOREACH (child, node)
          {
            enumerate_instances (child, instance_table);
          }
      }
      break;
    case AstTypeDefinition:
      break;
    case AstTypeSpecification:
      unimplemented;
    }
}

static void
allocate_symbol (memory_model_t* memory_model,
                 symbol_t* symbol)
{
  switch (symbol_kind (symbol))
    {
    case SymbolInstance:
      unimplemented;
      break;

    case SymbolParameter:
      {
        switch (symbol_parameter_kind (symbol))
          {
          case ParameterOrdinary:
          case ParameterReceiver:
          case ParameterReturn:
            {
              type_t* type = symbol_parameter_type (symbol);
              memory_model_arguments_push (memory_model, type_size (type));
              symbol_set_offset (symbol, memory_model_arguments_offset (memory_model));
            }
            break;
          case ParameterReceiverDuplicate:
          case ParameterDuplicate:
            break;
          }
      }
      break;

    case SymbolType:
      unimplemented;
      break;

    case SymbolTypedConstant:
      unimplemented;
      break;

    case SymbolVariable:
      {
        type_t* type = symbol_variable_type (symbol);
        symbol_set_offset (symbol, memory_model_locals_offset (memory_model));
        memory_model_locals_push (memory_model, type_size (type));
      }
      break;
    }
}

static void
allocate_symtab (ast_t* node, memory_model_t* memory_model)
{
  // Allocate the parameters.
  symtab_t* symtab = ast_get_symtab (node);
  symbol_t** pos;
  symbol_t** limit;
  for (pos = symtab_begin (symtab), limit = symtab_end (symtab);
       pos != limit;
       pos = symtab_next (pos))
    {
      allocate_symbol (memory_model, *pos);
    }
}

static void
allocate_statement_stack_variables (ast_t* node, memory_model_t* memory_model)
{
  switch (ast_statement_kind (node))
    {
    case AstAssignmentStmt:
      // Do nothing.
      break;

    case AstChangeStmt:
      {
        ast_t* body = ast_get_child (node, CHANGE_BODY);
        allocate_statement_stack_variables (body, memory_model);
      }
      break;

    case AstExpressionStmt:
      // Do nothing.
      break;

    case AstIfStmt:
      {
        ast_t* true_branch = ast_get_child (node, IF_TRUE_BRANCH);
        allocate_statement_stack_variables (true_branch, memory_model);
      }
      break;

    case AstAddAssignStmt:
      // Do nothing.
      break;

    case AstStmtList:
      {
        ptrdiff_t offset_before = memory_model_locals_offset (memory_model);
        allocate_symtab (node, memory_model);
        ptrdiff_t offset_after = memory_model_locals_offset (memory_model);
	AST_FOREACH (child, node)
          {
            allocate_statement_stack_variables (child, memory_model);
          }
        memory_model_locals_pop (memory_model, offset_after - offset_before);
        assert (memory_model_locals_offset (memory_model) == offset_before);
      }
      break;

    case AstReturnStmt:
      // Do nothing.
      break;

    case AstTriggerStmt:
      allocate_statement_stack_variables (ast_get_child (node, TRIGGER_BODY), memory_model);
      break;

    case AstVarStmt:
      // Do nothing.  The variables are allocated in the StmtList containing this.
      break;

    case AstPrintlnStmt:
      // Do nothing.
      break;
    }
}

static void
allocate_parameter (memory_model_t* memory_model,
                    symbol_t** pos,
                    symbol_t** limit)
{
  if (pos != limit)
    {
      allocate_parameter (memory_model, symtab_next (pos), limit);
      allocate_symbol (memory_model, *pos);
    }
}

// Return the size of the locals.
static size_t
allocate_stack_variables_helper (ast_t* node,
                                 ast_t* child,
                                 memory_model_t* memory_model)
{
  assert (memory_model_arguments_empty (memory_model));
  assert (memory_model_locals_empty (memory_model));
  // Allocate the parameters.
  symtab_t* symtab = ast_get_symtab (node);
  symbol_t** pos = symtab_begin (symtab);
  symbol_t** limit = symtab_end (symtab);
  ptrdiff_t offset_before = memory_model_arguments_offset (memory_model);
  allocate_parameter (memory_model, pos, limit);
  ptrdiff_t offset_after = memory_model_arguments_offset (memory_model);
  // Allocate the locals.
  allocate_statement_stack_variables (child, memory_model);
  // Deallocate the parameters.
  memory_model_arguments_pop (memory_model, offset_before - offset_after);
  assert (memory_model_arguments_empty (memory_model));
  assert (memory_model_locals_empty (memory_model));
  size_t locals_size = memory_model_locals_size (memory_model);
  memory_model_reset_locals_size (memory_model);
  return locals_size;
}

void
allocate_stack_variables (ast_t* node, memory_model_t* memory_model)
{
  switch (ast_kind (node))
    {
    case AstAction:
      {
        action_t* action = get_current_action (node);
        size_t locals_size = allocate_stack_variables_helper (node, ast_get_child (node, ACTION_BODY), memory_model);
        action_set_locals_size (action, locals_size);
      }
      break;

    case AstBind:
      break;

    case AstBindStatement:
      unimplemented;

    case AstExpression:
      unimplemented;

    case AstMethod:
      {
        method_t* method = get_current_method (node);
        size_t locals_size = allocate_stack_variables_helper (node, ast_get_child (node, METHOD_BODY), memory_model);
        method_set_locals_size (method, locals_size);
      }
      break;

    case AstIdentifier:
      unimplemented;

    case AstIdentifierList:
      unimplemented;

    case AstInstance:
      break;

    case AstReceiverDefinition:
      unimplemented;

    case AstReaction:
      {
        action_t* action = get_current_action (node);
        size_t locals_size = allocate_stack_variables_helper (node, ast_get_child (node, REACTION_BODY), memory_model);
        action_set_locals_size (action, locals_size);
      }
      break;

    case AstStatement:
      unimplemented;

    case AstTopLevelList:
      {
	AST_FOREACH (child, node)
          {
            allocate_stack_variables (child, memory_model);
          }
      }
      break;

    case AstTypeDefinition:
      break;

    case AstTypeSpecification:
      unimplemented;
    }
}
