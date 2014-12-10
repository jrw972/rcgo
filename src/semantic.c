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
#include "getter.h"

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
    case AstGetter:
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
    case AstPointerReceiverDefinition:
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
    case AstGetter:
    case AstExpression:
    case AstIdentifier:
    case AstIdentifierList:
    case AstPointerReceiverDefinition:
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

    case AstPort:
      return
	type_make_port (process_type_spec
			(ast_get_child (node, PORT_SIGNATURE), true));

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
    case AstGetter:
      {
	ast_t *receiver = ast_get_child (node, GETTER_RECEIVER);
	ast_t *type_node = ast_get_child (receiver, RECEIVER_TYPE_IDENTIFIER);
	ast_t *signature_node = ast_get_child (node, GETTER_SIGNATURE);
	ast_t *return_type_node = ast_get_child (node, GETTER_RETURN_TYPE);
	ast_t *identifier_node = ast_get_child (node, GETTER_IDENTIFIER);
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
        /* Prepend the signature with the receiver. */
        type_signature_prepend (signature,
                                ast_get_identifier (ast_get_child (receiver, RECEIVER_THIS_IDENTIFIER)),
                                type_make_pointer (type_make_immutable (type)), true);

        /* Process the return type. */
        type_t *return_type = process_type_spec (return_type_node, true);

        getter_t* getter = type_component_add_getter (type, node, identifier, signature, return_type);
	symtab_set_current_getter (ast_get_symtab (node), getter);
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
    case AstPointerReceiverDefinition:
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
	type_move (type, new_type);
	symbol_set_in_progress (symbol, false);
      }
      break;
    case AstTypeSpecification:
      unimplemented;
    }
}

static void check_rvalue (ast_t ** ptr, binding_t* binding, bool output);

static void check_identifier_expr (ast_t** ptr,
                                   bool immutable)
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
      ast_set_type (*ptr, typed_value_make (symbol_parameter_type (symbol)), immutable,
                    symbol_parameter_kind (symbol) ==
                    ParameterReceiver
                    || symbol_parameter_kind (symbol) ==
                    ParameterReceiverDuplicate);
      break;

    case SymbolType:
      unimplemented;

    case SymbolTypedConstant:
      ast_set_type (*ptr, symbol_typed_constant_value (symbol), true, false);
      break;

    case SymbolVariable:
      ast_set_type (*ptr, typed_value_make (symbol_variable_type (symbol)), false, false);
      break;
    }

  ast_set_symbol (*ptr, symbol);
}

static void
set_dereference_type (ast_t* node, typed_value_t tv)
{
  tv = typed_value_dereference (tv);
  ast_set_type (node, tv,
                type_is_immutable (tv.type),
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

static FieldOrAction
check_lvalue (ast_t ** ptr, binding_t* binding, bool output)
{
  ast_t *node = *ptr;
  assert (ast_kind (node) == AstExpression);

  switch (ast_expression_kind (node))
    {
    case AstAddressOfExpr:
      unimplemented;
    case AstCallExpr:
      unimplemented;
    case AstDereferenceExpr:
      {
	ast_t **child = ast_get_child_ptr (node, UNARY_CHILD);
	check_rvalue (child, binding, output);
        typed_value_t tv = ast_get_typed_value (*child);
        if (!typed_value_can_dereference (tv))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot dereference non-pointer");
          }
        set_dereference_type (node, tv);
      }
      break;
    case AstEqualExpr:
      unimplemented;
    case AstExprList:
      unimplemented;
    case AstIdentifierExpr:
      check_identifier_expr (ptr, false);
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
    case AstSelectExpr:
      {
	ast_t **left = ast_get_child_ptr (node, BINARY_LEFT_CHILD);
	ast_t *right = ast_get_child (node, BINARY_RIGHT_CHILD);
	string_t identifier = ast_get_identifier (right);
	check_lvalue (left, binding, output);
        typed_value_t tv = ast_get_typed_value (*left);

        if (type_is_pointer (tv.type))
          {
            // Selecting from a pointer.  Insert a dereference.
            ast_t* deref = ast_make_dereference (*left);
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

	ast_set_type (node, selected_tv, ast_get_derived_from_immutable (*left),
		      ast_get_derived_from_receiver (*left));

        if (binding != NULL)
          {
            if (type_is_component (selected_tv.type) || type_is_port (selected_tv.type))
              {
                field_t *field = type_select_field (tv.type, identifier);
                assert (field != NULL);
                if (output)
                  {
                    binding_add_output (binding, field);
                  }
                else
                  {
                    binding_add_input (binding, field);
                  }
                return make_field (field);
              }
            else if (type_is_reaction (selected_tv.type))
              {
                action_t * reaction = type_component_get_reaction (tv.type, identifier);
                assert (reaction != NULL);
                binding_set_reaction (binding, reaction);
                return make_action (reaction);
              }
          }
      }
      break;
    case AstTypedLiteral:
      unimplemented;
    case AstPortCallExpr:
      unimplemented;
    }

  return make_field (NULL);
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

      // Convert since we must compute.
      convert_if_untyped (parameter_tv, arg);
    }

  // Set the return type.
  // TODO:  Determine if the return type is derived from the receiver.
  // For safety, assume that it is.
  // TODO:  Is this analysis the same for getters?
  ast_set_type (node, typed_value_make (type_return_type (expr_type)), true, true);
}

static action_t *
get_current_action (const ast_t * node)
{
  return symtab_get_current_action (ast_get_symtab (node));
}

getter_t *
get_current_getter (const ast_t * node)
{
  return symtab_get_current_getter (ast_get_symtab (node));
}

static type_t *
get_current_return_type (const ast_t * node)
{
  getter_t* g = get_current_getter (node);
  if (g != NULL)
    {
      return getter_return_type (g);
    }

  return NULL;
}

static type_t *
get_current_receiver_type (const ast_t * node)
{
  return symtab_get_current_receiver_type (ast_get_symtab (node));
}

static trigger_t *
get_current_trigger (const ast_t * node)
{
  return symtab_get_current_trigger (ast_get_symtab (node));
}

static bool
in_trigger_statement (const ast_t * node)
{
  symtab_t *symtab = ast_get_symtab (node);
  return symtab_get_current_trigger (symtab) != NULL;
}

static bool
in_immutable_section (const ast_t* node)
{
  if (get_current_getter (node) != NULL)
    {
      // Calling a getter in a getter is okay.
      return true;
    }

  if (get_current_action (node) != NULL &&
      get_current_trigger (node) == NULL)
    {
      // In an action or reaction but not in a trigger body.
      return true;
    }

  return false;
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

  ast_set_type (node, typed_value_make (type_make_untyped_bool ()), true, false);
}

static void
check_rvalue (ast_t ** ptr, binding_t* binding, bool output)
{
  ast_t *node = *ptr;
  assert (ast_kind (node) == AstExpression);

  switch (ast_expression_kind (node))
    {
    case AstAddressOfExpr:
      {
        ast_t **expr = ast_get_child_ptr (node, UNARY_CHILD);
        check_lvalue (expr, binding, output);
        typed_value_t expr_tv = ast_get_typed_value (*expr);
        type_t* type;
        if (ast_get_derived_from_immutable (*expr))
          {
            type = type_make_pointer (type_make_immutable (expr_tv.type));
          }
        else
          {
            type = type_make_pointer (expr_tv.type);
          }

        ast_set_type (node, typed_value_make (type), true, ast_get_derived_from_receiver (*expr));
      }
      break;

    case AstCallExpr:
      {
	ast_t **expr = ast_get_child_ptr (node, CALL_EXPR);
	ast_t *args = ast_get_child (node, CALL_ARGS);

        // Analyze the callee.
        check_rvalue (expr, binding, output);

        typed_value_t expr_tv = ast_get_typed_value (*expr);
	if (!type_callable (expr_tv.type))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot call");
	  }

        if (type_called_with_receiver (expr_tv.type))
          {
            // The callee requires a receiver.

            // The receiver is either a copy or a pointer.
            bool receiver_is_pointer = type_is_pointer (type_parameter_type (expr_tv.type, 0));

            // Transfer the computation for the receiver to the argument list.
            assert (ast_expression_kind (*expr) == AstSelectExpr);
            ast_t* receiver_select_expr = ast_get_child (*expr, BINARY_LEFT_CHILD);
            if (receiver_is_pointer)
              {
                ast_t* e = ast_make_address_of (receiver_select_expr);
                ast_prepend_child (args, e);
              }
            else
              {
                ast_prepend_child (args, receiver_select_expr);
              }

            // Replace the callee with a literal or expression.
            typed_value_t select_tv = ast_get_typed_value (receiver_select_expr);
            string_t id = ast_get_identifier (ast_get_child (*expr, BINARY_RIGHT_CHILD));

            if (type_kind (expr_tv.type) == TypeGetter)
              {
                // Must be in immutable section.
                if (!in_immutable_section (node))
                  {
                    error_at_line (-1, 0, ast_file (node), ast_line (node),
                                   "getter called outside of immutable section");
                  }
                getter_t* getter = type_component_get_getter (select_tv.type, id);
                *expr = ast_make_typed_literal (typed_value_make_getter (getter));
              }
            else
              {
                unimplemented;
              }
          }

	check_rvalue_list (args);
	check_call (node, args, expr_tv.type);
      }
      break;

    case AstDereferenceExpr:
      unimplemented;

    case AstEqualExpr:
      check_comparison (node);
      break;

    case AstExprList:
      check_rvalue_list (*ptr);
      break;
    case AstIdentifierExpr:
      check_identifier_expr (ptr, true);
      break;
    case AstLogicAndExpr:
      unimplemented;
    case AstLogicNotExpr:
      {
	ast_t **child = ast_get_child_ptr (node, UNARY_CHILD);
	check_rvalue (child, binding, output);
        typed_value_t tv = ast_get_typed_value (*child);
        if (!typed_value_is_boolean (tv))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot apply ! to expression");
	  }
        tv = typed_value_logic_not (tv);
        ast_set_type (node, tv,
                      ast_get_derived_from_immutable (*child),
                      ast_get_derived_from_receiver (*child));
      }
      break;

    case AstLogicOrExpr:
      unimplemented;

    case AstNewExpr:
      {
        string_t name = ast_get_identifier (ast_get_child (node, UNARY_CHILD));
        symbol_t* symbol = lookup_no_force (node, name);
        if (symbol_kind (symbol) != SymbolType)
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "%s is not a type", get (name));
          }
        type_t* type = type_make_pointer (symbol_get_type_type (symbol));
        ast_set_type (node, typed_value_make (type), false, false);
      }
      break;

    case AstNotEqualExpr:
      check_comparison (node);
      break;

    case AstSelectExpr:
      check_lvalue (ptr, binding, output);
      break;
    case AstPortCallExpr:
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
      break;

    case AstTypedLiteral:
      // Do nothing.
      break;
    }

  if (ast_expression_kind (node) != AstExprList)
    {
      if (ast_get_derived_from_immutable (node))
        {
          typed_value_t tv = ast_get_typed_value (node);
          if (type_is_pointer (tv.type))
            {
              type_t* base_type = type_pointer_base_type (tv.type);
              if (!type_is_immutable (base_type))
                {
                  // Make the pointer immutable to avoid an immutability leak.
                  tv.type = type_make_pointer (type_make_immutable (base_type));
                  ast_set_typed_value (node, tv);
                }
            }
        }
    }
}

static void
check_rvalue_list (ast_t * node)
{
  assert (ast_kind (node) == AstExpression);
  assert (ast_expression_kind (node) == AstExprList);

  AST_FOREACH_PTR (child, node)
  {
    check_rvalue (child, NULL, false);
  }
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
    case TypeGetter:
    case TypeImmutable:
    case TypePointer:
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
convert_rvalue_list_to_builtin_types (ast_t * node)
{
  assert (ast_kind (node) == AstExpression);
  assert (ast_expression_kind (node) == AstExprList);

  AST_FOREACH_PTR (child, node)
  {
    convert_rvalue_to_builtin_types (child);
  }
}

static field_t *
extract_port_field (const ast_t * node)
{
  assert (ast_expression_kind (node) == AstPortCallExpr);

  string_t port_identifier =
    ast_get_identifier (ast_get_child (node, CALL_EXPR));
  const type_t *this_type =
    type_immutable_base_type (type_pointer_base_type (symtab_get_this_type (ast_get_symtab (node))));
  const type_t *field_list = type_component_field_list (this_type);
  return type_field_list_find (field_list, port_identifier);
}

static void
check_assignment_target (ast_t** left, ast_t* node)
{
  check_lvalue (left, NULL, false);
  if (ast_get_derived_from_immutable (*left))
    {
      error_at_line (-1, 0, ast_file (node), ast_line (node),
                     "cannot assign to read-only location");
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
	typed_value_t left_type = ast_get_typed_value (*left);
        typed_value_t right_type = ast_get_typed_value (*right);

        if (!typed_value_convertible (left_type, right_type))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot convert %s to %s in assignment", type_to_string (right_type.type), type_to_string (left_type.type));
          }

        if (ast_get_derived_from_immutable (*right) &&
            type_leaks_mutable_pointers (left_type.type))
          {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "assignment leaks mutable pointer");
          }

        // Convert the rvalue since we must compute it.
        convert_if_untyped (left_type, *right);
      }
      break;

    case AstExpressionStmt:
      unimplemented;

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


      ast_print (node, 0);
      unimplemented;

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
                                            type_make_pointer
                                            (type_make_immutable (get_current_receiver_type
                                                                  (node))), this_node));

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
    case AstGetter:
      {
	ast_t *signature_node = ast_get_child (node, GETTER_SIGNATURE);
	ast_t *body_node = ast_get_child (node, GETTER_BODY);
        ast_t *return_type_node = ast_get_child (node, GETTER_RETURN_TYPE);
	getter_t *getter = get_current_getter (node);

        /* Enter the return type into the symbol table. */
        enter_symbol (return_type_node, symbol_make_return_parameter (enter ("0return"), getter_return_type (getter), return_type_node));

	/* Enter the signature into the symbol table. */
	enter_signature (signature_node, getter_signature (getter));

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
    case AstPointerReceiverDefinition:
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
                                            type_make_pointer
                                            (type_make_immutable (action_component_type (reaction))),
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
    case AstGetter:
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
    case AstPointerReceiverDefinition:
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

    case AstExpressionStmt:
      unimplemented;
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

    case AstGetter:
      {
        getter_t* getter = get_current_getter (node);
        size_t locals_size = allocate_stack_variables_helper (node, ast_get_child (node, GETTER_BODY), memory_model);
        getter_set_locals_size (getter, locals_size);
      }
      break;

    case AstIdentifier:
      unimplemented;

    case AstIdentifierList:
      unimplemented;

    case AstInstance:
      break;

    case AstPointerReceiverDefinition:
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
