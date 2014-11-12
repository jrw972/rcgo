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
	case AstAssignmentStmt:
	  break;
	case AstExpressionStmt:
	  unimplemented;
	case AstStmtList:
	  {
	    /* Create a new symbol table. */
	    symtab_t *new_symtab = symtab_make (symtab);
	    symtab = new_symtab;
	  }
	  break;
	case AstReturnStmt:
	  break;
	case AstTriggerStmt:
	  break;
	case AstVarStmt:
	  unimplemented;
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
      // Check if the symbol is defined locally.
      symtab_t *symtab = ast_get_symtab (node);
      string_t identifier = parameter_name (*pos);
      type_t *type = parameter_type (*pos);
      symbol_t *s = symtab_find_current (symtab, identifier);
      if (s == NULL)
	{
	  s =
	    symbol_make_parameter (identifier, type, node);
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
    case AstExpression:
    case AstIdentifier:
    case AstIdentifierList:
      break;
    case AstInstance:
      enter_undefined_symbol (node, ast_get_child (node, INSTANCE_IDENTIFIER),
			      SymbolInstance);
      break;
    case AstPointerReceiverDefinition:
    case AstReaction:
    case AstStatement:
    case AstTopLevelList:
      break;
    case AstTypeDefinition:
      enter_undefined_symbol (node, ast_get_child (node, TYPE_IDENTIFIER),
			      SymbolType);
      break;
    case AstTypeSpecification:
      break;
    }

  AST_FOREACH (child, node)
  {
    enter_symbols_helper (child);
  }
}

static type_t* bool_type = NULL;

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
  }

  /* Insert untyped boolean constants. */
  symtab_enter (symtab,
		symbol_make_untyped_constant (enter ("true"),
					      untyped_value_make_bool (true),
					      node));
  symtab_enter (symtab,
		symbol_make_untyped_constant (enter ("false"),
					      untyped_value_make_bool (false),
					      node));

  enter_symbols_helper (node);
}

static symbol_t *
lookup (ast_t * node, string_t identifier)
{
  symtab_t *symtab = ast_get_symtab (node);
  symbol_t *symbol = symtab_find (symtab, identifier);
  if (!symbol_defined (symbol))
    {
      unimplemented;
    }
  return symbol;
}

static type_t *
process_type_spec (ast_t * node)
{
  assert (ast_kind (node) == AstTypeSpecification);
  switch (ast_type_specification_kind (node))
    {
    case AstComponent:
      return
	type_make_component (process_type_spec
			     (ast_get_child (node, COMPONENT_FIELD_LIST)));

    case AstFieldList:
      {
	type_t *field_list = type_make_field_list ();
	AST_FOREACH (child, node)
	{
	  ast_t *identifier_list = ast_get_child (child, IDENTIFIER_LIST);
	  ast_t *type_spec = ast_get_child (child, TYPE_SPEC);
	  type_t *type = process_type_spec (type_spec);
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
	symbol_t *symbol = lookup (child, identifier);
	if (symbol_kind (symbol) != SymbolType)
	  {
	    error_at_line (-1, 0, ast_file (child), ast_line (child),
			   "%s does not refer to a type", get (identifier));
	  }
	return symbol_get_type_type (symbol);
      }

    case AstPort:
      return
	type_make_port (process_type_spec
			(ast_get_child (node, PORT_SIGNATURE)));

    case AstSignature:
      {
	type_t *signature = type_make_signature ();
	AST_FOREACH (child, node)
	{
	  ast_t *identifier_list = ast_get_child (child, IDENTIFIER_LIST);
	  ast_t *type_spec = ast_get_child (child, TYPE_SPEC);
	  type_t *type = process_type_spec (type_spec);
	  AST_FOREACH (id, identifier_list)
	  {
	    string_t identifier = ast_get_identifier (id);
	    const parameter_t *parameter =
	      type_signature_find (signature, identifier);
	    if (parameter == NULL)
	      {
		type_signature_append (signature, identifier, type);
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
	symbol_t *symbol = lookup (type_node, type_identifier);
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
	symbol_t *symbol = lookup (type_node, type_identifier);
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
	  lookup (type_identifier_node,
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
	symbol_t *symbol = lookup (type_node, type_identifier);
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
	type_t *signature = process_type_spec (signature_node);

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
	type_t *new_type = process_type_spec (type_spec_node);
	type_move (type, new_type);
	symbol_set_in_progress (symbol, false);
      }
      break;
    case AstTypeSpecification:
      unimplemented;
    }
}

static void check_rvalue (ast_t ** ptr);

static void
check_lvalue (ast_t ** ptr)
{
  ast_t *node = *ptr;
  assert (ast_kind (node) == AstExpression);

  switch (ast_expression_kind (node))
    {
    case AstCallExpr:
      unimplemented;
    case AstDereferenceExpr:
      {
	ast_t **child = ast_get_child_ptr (node, UNARY_CHILD);
	check_rvalue (child);
	/* Can't dereference literals. */
	if (ast_is_literal (*child))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot dereference literal");
	  }
	const type_t *type = ast_get_type (*child);
	if (!type_is_pointer (type))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot dereference non-pointer");
	  }
	ast_set_type (node, type_pointer_base_type (type),
		      type_pointer_kind (type) == PointerToImmutable,
		      ast_get_derived_from_receiver (*child));
      }
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
    case AstSelectExpr:
      {
	ast_t **left = ast_get_child_ptr (node, BINARY_LEFT_CHILD);
	ast_t *right = ast_get_child (node, BINARY_RIGHT_CHILD);
	string_t identifier = ast_get_identifier (right);
	check_lvalue (left);
	if (ast_is_literal (*left))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot select from literal");
	  }
	type_t *type = ast_get_type (*left);
	type_t *selected_type = type_select (type, identifier);
	if (selected_type == NULL)
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot select %s from expression of type %s",
			   get (identifier), type_to_string (type));
	  }
	ast_set_type (node, selected_type, ast_get_immutable (*left),
		      ast_get_derived_from_receiver (*left));
      }
      break;
    case AstTypedLiteral:
      unimplemented;
    case AstPortCallExpr:
      unimplemented;
    case AstUntypedLiteral:
      unimplemented;
    }
}

static void
convert (type_t * target_type, ast_t ** source, const ast_t * error_node)
{
  if (ast_is_untyped_literal (*source))
    {
      untyped_value_t value = ast_get_untyped_value (*source);
      if (!type_can_represent (target_type, value))
	{
	  error_at_line (-1, 0, ast_file (error_node), ast_line (error_node),
			 "cannot represent literal");
	}

      typed_value_t typed_value =
	typed_value_from_untyped (value, target_type);

      *source = ast_make_typed_literal (typed_value);
    }
  else if (ast_is_typed_literal (*source))
    {
      unimplemented;
    }
  else
    {
      const type_t *source_type = ast_get_type (*source);
      if (!type_equivalent (target_type, source_type))
	{
	  error_at_line (-1, 0, ast_file (error_node), ast_line (error_node),
			 "types not equivalent in assignment");
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
      ast_t **arg = ast_get_child_ptr (args, idx);
      type_t *parameter_type = type_parameter_type (expr_type, idx);
      convert (parameter_type, arg, *arg);
    }

  // Set the return type.
  // TODO:  Determine if the return type is derived from the receiver.
  // For safety, assume that it is.
  ast_set_type (node, type_return_type (expr_type), true, true);
}

static void
check_rvalue (ast_t ** ptr)
{
  ast_t *node = *ptr;
  assert (ast_kind (node) == AstExpression);

  switch (ast_expression_kind (node))
    {
    case AstCallExpr:
      {
	ast_t **expr = ast_get_child_ptr (node, CALL_EXPR);
	check_lvalue (expr);
	type_t *expr_type = ast_get_type (*expr);
	if (!type_callable (expr_type))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot call");
	  }

	ast_t *args = ast_get_child (node, CALL_ARGS);
	check_rvalue_list (args);

	check_call (node, args, expr_type);
      }
      break;
    case AstDereferenceExpr:
      unimplemented;
    case AstExprList:
      unimplemented;
    case AstIdentifierExpr:
      {
	ast_t *identifier_node = ast_get_child (node, UNARY_CHILD);
	string_t identifier = ast_get_identifier (identifier_node);
	symtab_t *symtab = ast_get_symtab (node);
	symbol_t *symbol = symtab_find (symtab, identifier);
	if (symbol != NULL)
	  {
	    switch (symbol_kind (symbol))
	      {
	      case SymbolInstance:
		unimplemented;
	      case SymbolParameter:
		ast_set_type (*ptr, symbol_parameter_type (symbol), true,
			      symbol_parameter_kind (symbol) ==
			      ParameterReceiver
			      || symbol_parameter_kind (symbol) ==
			      ParameterReceiverDuplicate);
		break;
	      case SymbolType:
		unimplemented;
	      case SymbolTypedConstant:
		unimplemented;
	      case SymbolUntypedConstant:
		/* Replace ourselves with a literal. */
		*ptr =
		  ast_make_untyped_literal (symbol_untyped_constant_value
					    (symbol));
		break;
	      case SymbolVariable:
		unimplemented;
	      }
	  }
	else
	  {
	    error_at_line (-1, 0, ast_file (identifier_node),
			   ast_line (identifier_node), "%s is not defined",
			   get (identifier));
	  }
      }
      break;
    case AstLogicAndExpr:
      unimplemented;
    case AstLogicNotExpr:
      {
	ast_t **child = ast_get_child_ptr (node, UNARY_CHILD);
	check_rvalue (child);
	if (!ast_is_boolean (*child))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "cannot apply ! to expression");
	  }
	if (ast_is_untyped_literal (*child))
	  {
	    unimplemented;
	  }
	else if (ast_is_typed_literal (*child))
	  {
	    unimplemented;
	  }
	else
	  {
	    ast_set_type (node, ast_get_type (*child),
			  ast_get_immutable (*child),
			  ast_get_derived_from_receiver (*child));
	  }
      }
      break;
    case AstLogicOrExpr:
      unimplemented;
    case AstSelectExpr:
      check_lvalue (ptr);
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
      unimplemented;
    case AstUntypedLiteral:
      unimplemented;
    }
}

static void
check_rvalue_list (ast_t * node)
{
  assert (ast_kind (node) == AstExpression);
  assert (ast_expression_kind (node) == AstExprList);

  AST_FOREACH_PTR (child, node)
  {
    check_rvalue (child);
  }
}

static action_t *
get_current_action (const ast_t * node)
{
  return symtab_get_current_action (ast_get_symtab (node));
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

static field_t *
extract_port_field (const ast_t * node)
{
  assert (ast_expression_kind (node) == AstPortCallExpr);

  string_t port_identifier =
    ast_get_identifier (ast_get_child (node, CALL_EXPR));
  const type_t *this_type =
    type_pointer_base_type (symtab_get_this_type (ast_get_symtab (node)));
  const type_t *field_list = type_component_field_list (this_type);
  return type_field_list_find (field_list, port_identifier);
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
	check_lvalue (left);
	if (ast_get_immutable (*left))
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
	ast_t **right = ast_get_child_ptr (node, BINARY_RIGHT_CHILD);
	check_rvalue (right);
	type_t *left_type = ast_get_type (*left);
	convert (left_type, right, node);
      }
      break;
    case AstExpressionStmt:
      unimplemented;
    case AstStmtList:
      {
	AST_FOREACH (child, node)
	{
	  check_statement (child);
	}
      }
      break;
    case AstReturnStmt:
      unimplemented;
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
	symtab_set_current_trigger (ast_get_symtab (node), trigger);

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
      unimplemented;
    }
}

static field_t *
check_output (ast_t ** node, binding_t * binding)
{
  switch (ast_expression_kind (*node))
    {
    case AstCallExpr:
      unimplemented;
    case AstDereferenceExpr:
      check_lvalue (node);
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
	ast_t **left = ast_get_child_ptr (*node, BINARY_LEFT_CHILD);
	ast_t *right = ast_get_child (*node, BINARY_RIGHT_CHILD);
	string_t identifier = ast_get_identifier (right);
	check_output (left, binding);

	const type_t *component_type = ast_get_type (*left);

	if (!type_is_component (component_type))
	  {
	    error_at_line (-1, 0, ast_file (*node), ast_line (*node),
			   "bind source path contains something other than a component or port");
	  }

	const type_t *field_list = type_component_field_list (component_type);
	field_t *field = type_field_list_find (field_list, identifier);

	if (field == NULL)
	  {
	    error_at_line (-1, 0, ast_file (*node), ast_line (*node),
			   "component does not have a data member named %s",
			   get (identifier));
	  }

	type_t *field_typ = field_type (field);
	if (!(type_is_component (field_typ) || type_is_port (field_typ)))
	  {
	    error_at_line (-1, 0, ast_file (*node), ast_line (*node),
			   "bind source path contains something other than a component or port");
	  }

	ast_set_type (*node, field_typ, ast_get_immutable (*left),
		      ast_get_derived_from_receiver (*left));

	binding_add_output (binding, field);

	return field;
      }
      break;
    case AstTypedLiteral:
      unimplemented;
    case AstUntypedLiteral:
      unimplemented;
    }

  return NULL;
}

static action_t *
check_input (ast_t ** node, binding_t * binding)
{
  switch (ast_expression_kind (*node))
    {
    case AstCallExpr:
      unimplemented;
    case AstDereferenceExpr:
      check_lvalue (node);
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
	ast_t **left = ast_get_child_ptr (*node, BINARY_LEFT_CHILD);
	ast_t *right = ast_get_child (*node, BINARY_RIGHT_CHILD);
	string_t identifier = ast_get_identifier (right);
	check_input (left, binding);

	const type_t *component_type = ast_get_type (*left);

	if (!type_is_component (component_type))
	  {
	    error_at_line (-1, 0, ast_file (*node), ast_line (*node),
			   "bind sink path contains something other than a component or reaction");
	  }

	type_t *selected_type = type_select (component_type, identifier);
	if (selected_type == NULL)
	  {
	    error_at_line (-1, 0, ast_file (*node), ast_line (*node),
			   "cannot select %s from expression of type %s",
			   get (identifier), type_to_string (component_type));
	  }

	if (type_is_component (selected_type))
	  {
	    const type_t *field_list =
	      type_component_field_list (component_type);
	    field_t *field = type_field_list_find (field_list, identifier);
	    assert (field != NULL);
	    binding_add_input (binding, field);
	    ast_set_type (*node, selected_type, ast_get_immutable (*left),
			  ast_get_derived_from_receiver (*left));
	  }
	else if (type_is_reaction (selected_type))
	  {
	    action_t *reaction =
	      type_component_get_reaction (component_type, identifier);
	    assert (reaction != NULL);
	    binding_set_reaction (binding, reaction);
	    ast_set_type (*node, reaction_type (reaction),
			  ast_get_immutable (*left),
			  ast_get_derived_from_receiver (*left));
	    return reaction;
	  }
	else
	  {
	    error_at_line (-1, 0, ast_file (*node), ast_line (*node),
			   "bind sink path contains something other than a component or reaction");
	  }
      }
      break;
    case AstTypedLiteral:
      unimplemented;
    case AstUntypedLiteral:
      unimplemented;
    }

  return NULL;
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
	field_t *port_field = check_output (port_node, binding);
	const type_t *port_type = field_type (port_field);

	if (!type_is_port (port_type))
	  {
	    error_at_line (-1, 0, ast_file (node), ast_line (node),
			   "source of bind is not a port");
	  }

	ast_t **reaction_node = ast_get_child_ptr (node, BINARY_RIGHT_CHILD);
	action_t *reaction = check_input (reaction_node, binding);
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
                                            (PointerToImmutable,
                                             get_current_receiver_type
                                             (node)), this_node));

	/* Check the precondition. */
	check_rvalue (precondition_node);

        if (ast_expression_kind (*precondition_node) == AstUntypedLiteral)
          {
            convert (bool_type, precondition_node, *precondition_node);
          }

	if (!ast_is_boolean (*precondition_node))
	  {
	    error_at_line (-1, 0, ast_file (*precondition_node),
			   ast_line (*precondition_node),
			   "precondition is not a boolean expression");
	  }

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
                                            (PointerToImmutable,
                                             get_current_receiver_type
                                             (node)), this_node));
	/* Check the body. */
	check_bind_statement (body_node);
      }
      break;
    case AstBindStatement:
      unimplemented;
    case AstExpression:
      unimplemented;
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
                                            (PointerToImmutable,
                                             action_type (reaction)),
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
        type_t* type = symbol_parameter_type (symbol);
        memory_model_arguments_push (memory_model, type_size (type));
        symbol_set_offset (symbol, memory_model_arguments_offset (memory_model));
      }
      break;
    case SymbolType:
      unimplemented;
      break;
    case SymbolTypedConstant:
      unimplemented;
      break;
    case SymbolUntypedConstant:
      unimplemented;
      break;
    case SymbolVariable:
      unimplemented;
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
      unimplemented;
      break;
    case AstExpressionStmt:
      unimplemented;
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
      unimplemented;
      break;
    case AstTriggerStmt:
      break;
    case AstVarStmt:
      unimplemented;
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

static void
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
}

void
allocate_stack_variables (ast_t* node, memory_model_t* memory_model)
{
  switch (ast_kind (node))
    {
    case AstAction:
      allocate_stack_variables_helper (node, ast_get_child (node, ACTION_BODY), memory_model);
      break;
    case AstBind:
      break;
    case AstBindStatement:
      unimplemented;
    case AstExpression:
      unimplemented;
    case AstIdentifier:
      unimplemented;
    case AstIdentifierList:
      unimplemented;
    case AstInstance:
      break;
    case AstPointerReceiverDefinition:
      unimplemented;
    case AstReaction:
      allocate_stack_variables_helper (node, ast_get_child (node, REACTION_BODY), memory_model);
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
