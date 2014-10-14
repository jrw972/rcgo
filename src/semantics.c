#include "semantics.h"
#include "symtab.h"
#include "debug.h"
#include <error.h>
#include <string.h>
#include "util.h"
#include "instance_set.h"

#ifdef blah
static const type_t *
check_type_spec (node_t * node, symtab_t * symtab);

static signature_t*
check_signature (node_t * node, symtab_t * symtab)
{
  signature_t* signature = signature_make ();

  NODE_FOREACH (parameter, node) {
    node_t* identifier_list = node_child (parameter, PARAMETER_IDENTIFIER_LIST);
    node_t* type_spec = node_child (parameter, PARAMETER_TYPE_SPEC);
    const type_t* type = check_type_spec (type_spec, symtab);
    NODE_FOREACH (name, identifier_list) {
      string_t identifier = node_identifier (name);
      if (signature_add_parameter (signature, identifier, type)) {
        // Add to scope.
        symbol_t* symbol = symbol_make_variable (identifier, type);
        symtab_enter (symtab, symbol);
      }
      else {
        error_at_line (-1, 0, node_file (name), node_line (name), "%s is a duplicate parameter", get (identifier));
      }
    }
  }

  node_set_signature (node, signature);

  return signature;
}

static const type_t *
check_type_spec (node_t * node, symtab_t * symtab)
{
  switch (node_type_spec_type (node))
    {
    case FieldListTypeSpec:
      {
	type_t *type = NULL;

	switch (node_get_field_list_type (node))
	  {
	  case Component:
	    type = type_make_component ();
	    break;
	  }

	NODE_FOREACH (field, node)
	  {
	    node_t *identifier_list = node_child (field, FIELD_IDENTIFIER_LIST);
	    node_t *type_spec = node_child (field, FIELD_TYPE_SPEC);
	    const type_t *field_type = check_type_spec (type_spec, symtab);

            NODE_FOREACH (identifier, identifier_list)
	      {
		string_t id = node_identifier (identifier);
		if (!type_append_field (type, id, field_type))
		  {
		    /* TODO:  Error reporting. */
		    printf ("duplicate field name %s\n", id);
		  }
	      }
	  }

	node_set_type (node, type);
      }
      break;

    case IdentifierTypeSpec:
      {
	/* Look up the identifier in the symbol table. */
	string_t identifier = node_identifier_type_spec_identifier (node);
	symbol_t *symbol = symtab_find (symtab, identifier);
	if (symbol == NULL)
	  {
            error_at_line (-1, 0, node_file (node), node_line (node), "%s is not defined in this scope", get (identifier));
	    break;
	  }

	/* Ensure that the symbol refers to a type. */
	if (!symbol_is_type (symbol))
	  {
	    /* TODO:  Error reporting. */
	    printf ("%s is not a type\n", get (identifier));
	    node_set_type (node, type_make_undefined ());
	    break;
	  }

	node_set_type (node, symbol_type_type (symbol));
      }
      break;

    case PortTypeSpec:
      {
        node_t * signature = node_child (node, PORT_TYPE_SPEC_SIGNATURE);
        signature_t * sig = check_signature (signature, symtab);
        node_set_type (node, type_make_port (sig));
      }
      break;
    }

  return node_get_type (node);
}

static semval_t check_expr (node_t * node, symtab_t * symtab);

static semval_t
check_unary (node_t * node,
	     symtab_t * symtab,
	     semval_t (*func) (semval_t), const char *message)
{
  /* Process the child. */
  node_t *child = node_child (node, UNARY_CHILD);
  semval_t child_sv = check_expr (child, symtab);

  semval_t this_sv;
  if (semval_is_undefined (child_sv))
    {
      this_sv = semval_undefined ();
    }
  else
    {
      this_sv = func (child_sv);
      if (semval_is_undefined (this_sv))
	{
	  /* TODO:  Error reporting. */
	  printf ("%s\n", message);
	  node_print (node, 0);
	}
    }
  return node_set_semval (node, this_sv);
}

static semval_t
check_binary (node_t * node,
	      symtab_t * symtab,
	      semval_t (*func) (semval_t, semval_t), const char *message)
{
  /* Process the children. */
  node_t *left = node_child (node, BINARY_LEFT_CHILD);
  node_t *right = node_child (node, BINARY_RIGHT_CHILD);
  semval_t left_sv = check_expr (left, symtab);
  semval_t right_sv = check_expr (right, symtab);

  semval_t this_sv;
  if (semval_is_undefined (left_sv) || semval_is_undefined (right_sv))
    {
      this_sv = semval_undefined ();
    }
  else
    {
      this_sv = func (left_sv, right_sv);
      if (semval_is_undefined (this_sv))
	{
	  /* TODO:  Error reporting. */
	  printf ("%s\n", message);
	}
    }

  return node_set_semval (node, this_sv);
}

static semval_list_t*
check_expr_list (node_t* node, symtab_t* symtab);

static semval_t
check_expr (node_t * node, symtab_t * symtab)
{
  switch (node_expr_type (node))
    {
    case CallExpr:
      {
        node_t* expr = node_child (node, CALL_EXPR);
        node_t* args = node_child (node, CALL_ARGS);

        semval_t expr_semval = check_expr (expr, symtab);
        semval_list_t* args_list = check_expr_list (args, symtab);

        semval_t this_sv;
        if (semval_is_undefined (expr_semval)
            || semval_list_contains_undefined (args_list))
          {
            this_sv = semval_undefined ();
          }
        else
          {
            this_sv = semval_call (expr_semval, args_list);
            if (semval_is_undefined (this_sv))
              {
                error_at_line (-1, 0, node_file (node), node_line (node), "result of call is not defined");
              }
          }
        node_set_semval (node, this_sv);
      }
      break;

    case ExplicitDereferenceExpr:
      {
	/* Process the child. */
	node_t *child = node_child (node, UNARY_CHILD);
	semval_t child_sv = check_expr (child, symtab);

	semval_t this_sv;
	if (semval_is_undefined (child_sv))
	  {
	    this_sv = semval_undefined ();
	  }
	else
	  {
	    this_sv = semval_explicit_dereference (child_sv);
	    if (semval_is_undefined (this_sv))
	      {
		/* TODO:  Error reporting. */
		printf ("could not deference\n");
	      }
	  }
	node_set_semval (node, this_sv);
      }
      break;

    case IdentifierExpr:
      {
	/* Look up the identifier in the symbol table. */
	string_t identifier = node_identifier_expr_identifier (node);
	symbol_t *symbol = symtab_find (symtab, identifier);
	if (symbol == NULL)
	  {
            error_at_line (-1, 0, node_file (node), node_line (node), "%s is not defined in this scope", get (identifier));
	    break;
	  }

        node_set_symbol (node, symbol);

	if (symbol_is_variable (symbol))
	  {
	    node_set_semval (node,
			     semval_make_reference (reference_make
						    (abstract_value_make_typed
						     (symbol_variable_type
						      (symbol), select_result_make_undefined ()))));
	  }
	else if (symbol_is_constant (symbol))
	  {
	    node_set_semval (node,
			     semval_make_reference (reference_make
						    (symbol_constant_value
						     (symbol))));
	  }
	else
	  {
	    /* TODO:  Error reporting. */
	    printf ("%s is not a variable or constant\n", get (identifier));
	    node_set_semval (node, semval_undefined ());
	    break;
	  }
      }
      break;

    case ImplicitDereferenceExpr:
      {
	/* Process the child. */
	node_t *child = node_child (node, UNARY_CHILD);
	semval_t child_sv = check_expr (child, symtab);

	semval_t this_sv;
	if (semval_is_undefined (child_sv))
	  {
	    this_sv = semval_undefined ();
	  }
	else
	  {
	    this_sv = semval_implicit_dereference (child_sv);
	    if (semval_is_undefined (this_sv))
	      {
		/* TODO:  Error reporting. */
		printf ("could not deference\n");
	      }
	  }
	node_set_semval (node, this_sv);
      }
      break;

    case LogicAndExpr:
      check_binary (node, symtab, semval_logic_and, "could not do logic and");
      break;

    case LogicNotExpr:
      check_unary (node, symtab, semval_logic_not, "could not do logic not");
      break;

    case LogicOrExpr:
      check_binary (node, symtab, semval_logic_or, "could not do logic or");
      break;

    case SelectExpr:
      {
	node_t *expr = node_child (node, UNARY_CHILD);
	string_t identifier = node_get_select_identifier (node);
	semval_t expr_semval = check_expr (expr, symtab);

	if (semval_is_undefined (expr_semval))
	  {
	    node_set_semval (node, semval_undefined ());
	    break;
	  }

	semval_t semval = semval_select (expr_semval, identifier);
	if (semval_is_undefined (semval))
	  {
            error_at_line (-1, 0, node_file (node), node_line (node), "could not select %s from expression of type %s", get (identifier), semval_to_string (expr_semval));
	  }
	node_set_semval (node, semval);
      }
      break;
    }

  return node_get_semval (node);
}

static semval_list_t*
check_expr_list (node_t* node, symtab_t* symtab)
{
  assert (node_def_type (node) == ListDef);
  semval_list_t* list = semval_list_make ();

  NODE_FOREACH (child, node)
    {
      semval_list_append (list, check_expr (child, symtab));
    }

  return list;
}

static void
check_stmt (node_t * node, symtab_t * symtab)
{
  switch (node_stmt_type (node))
    {
    case AssignmentStmt:
      {
	node_t *left = node_child (node, BINARY_LEFT_CHILD);
	semval_t left_sv = check_expr (left, symtab);
	node_t *right = node_child (node, BINARY_RIGHT_CHILD);
	semval_t right_sv = check_expr (right, symtab);

	if (!semval_assignable (left_sv, right_sv))
	  {
            unimplemented;
	    /* /\* TODO:  Error reporting. *\/ */
	    /* printf ("could not assign\n"); */
	    /* semval_print (left_sv); */
	    /* printf (" = "); */
	    /* semval_print (right_sv); */
	    /* printf ("\n"); */
	  }
      }
      break;

    case BindListStmt:
      unimplemented;

    case BindStmt:
      unimplemented;

    case ExprStmt:
      {
	/* Check the expression. */
	check_expr (node_child (node, UNARY_CHILD), symtab);
      }
      break;

    case ListStmt:
      {
	/* Create a new scope. */
	symtab_t* scope = symtab_make (symtab);
        node_set_symtab (node, scope);

	/* Check each statment in the list. */
	NODE_FOREACH (child, node)
	  {
	    check_stmt (child, scope);
	  }
      }
      break;

    case PrintStmt:
      check_expr (node_child (node, UNARY_CHILD), symtab);
      break;

    case ReturnStmt:
      check_expr (node_child (node, UNARY_CHILD), symtab);
      break;

    case TriggerStmt:
      {
        node_t *expr_list = node_child (node, TRIGGER_EXPR_LIST);
	node_t *body = node_child (node, TRIGGER_BODY);

        check_expr_list (expr_list, symtab);

	/* Create a new scope and enter this as a pointer to mutable. */
	symtab_t* scope = symtab_make (symtab);
        node_set_symtab (node, scope);

	symbol_t *this_symbol = symtab_get_this (scope);
	const type_t *type = type_make_pointer (type_pointer_base_type
                                                (symbol_variable_type (this_symbol)));
        symbol_t *new_this = symbol_make_variable (symbol_identifier (this_symbol),
                                                   type);
        symbol_set_as_this (new_this);
	symtab_enter (scope, new_this);

	check_stmt (body, scope);
      }
      break;

    case VarStmt:
      {
	/* Extract the children. */
	node_t *identifier_list = node_child (node, VAR_IDENTIFIER_LIST);
	node_t *type_spec = node_child (node, VAR_TYPE_SPEC);

	/* Process the type spec. */
	const type_t *type = check_type_spec (type_spec, symtab);

	/* Enter the identifiers into the symbol table. */
	NODE_FOREACH (id, identifier_list)
	  {
	    string_t id_str = node_identifier (id);
	    if (symtab_find_current (symtab, id_str) == NULL)
	      {
		symtab_enter (symtab, symbol_make_variable (id_str, type));
	      }
	    else
	      {
		/* TODO:  Error reporting. */
		printf ("%s is already defined in this scope\n",
			get (id_str));
	      }
	  }
      }
      break;
    }
}

static const type_t*
check_pointer_receiver (node_t* node, symtab_t* symtab)
{
  node_t *this_identifier = node_child (node, POINTER_RECEIVER_THIS_IDENTIFIER);
  string_t this_str = node_identifier (this_identifier);
  node_t *type_identifier = node_child (node, POINTER_RECEIVER_TYPE_IDENTIFIER);
  string_t type_str = node_identifier (type_identifier);

  /* Look up the type. */
  symbol_t *symbol = symtab_find (symtab, type_str);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, node_file (node), node_line (node), "%s is not defined in this scope", get (type_str));
      return NULL;
    }

  if (!symbol_is_type (symbol))
    {
      /* TODO:  Error reporting. */
      printf ("%s does not refer to a type\n", get (type_str));
      return NULL;
    }

  const type_t *type = symbol_type_type (symbol);

  /* Check that the identifier is not defined in the local scope. */
  symbol = symtab_find_current (symtab, this_str);
  if (symbol != NULL)
    {
      /* TODO:  Error reporting. */
      printf ("%s is already defined in this scope\n", get (this_str));
      return NULL;
    }

  /* Define this_str to the appropriate type. */
  const type_t *pointer_type = type_make_pointer (type);

  symbol_t *s = symbol_make_variable (this_str, pointer_type);
  symbol_set_as_this (s);
  symtab_enter (symtab, s);

  node_receiver_set_type (node, type);
  return type;
}

static const type_t *
check_receiver (node_t * node, symtab_t * symtab)
{
  switch (node_decl_type (node)) {
  case Field:
  case Identifier:
  case IdentifierList:
  case Parameter:
    bug ("Expected a receiver");
  case PointerReceiver:
    return check_pointer_receiver (node, symtab);
  }

  bug ("unhandled case");
}

static void
check_bind_stmt (node_t* node,
                 symtab_t* scope)
{
  switch (node_stmt_type (node)) {
  case BindStmt:
    {
      node_t* output = node_child (node, BINARY_LEFT_CHILD);
      node_t* input = node_child (node, BINARY_RIGHT_CHILD);

      semval_t output_semval = check_expr (output, scope);
      semval_t input_semval = check_expr (input, scope);

      if (!semval_bindable (output_semval, input_semval)) {
        error_at_line (-1, 0, node_file (node), node_line (node), "cannot bind port of type %s to reaction of type %s", semval_to_string (output_semval), semval_to_string (input_semval));
      }
    }
    break;
  case AssignmentStmt:
  case BindListStmt:
  case ExprStmt:
  case ListStmt:
  case PrintStmt:
  case ReturnStmt:
  case TriggerStmt:
  case VarStmt:
    bug ("unhandled case");
  }
}

static void
check_boolean_return (node_t* node, symtab_t* symtab)
{
  check_stmt (node, symtab);

  semval_t semval = node_get_semval (node_child (node, UNARY_CHILD));
  if (semval_is_undefined (semval))
    {
      return;
    }

  if (!semval_is_boolean (semval))
    {
      error_at_line (-1, 0, node_file (node), node_line (node), "expected boolean in return");
    }
}

static void
check_def (node_t * node, symtab_t * symtab)
{
  switch (node_def_type (node))
    {
    case ActionDef:
      {
	node_t *receiver = node_child (node, ACTION_RECEIVER);
	node_t *precondition = node_child (node, ACTION_PRECONDITION);
	node_t *body = node_child (node, ACTION_BODY);

	/* Create a new scope in which to enter the receiver. */
	symtab_t* scope = symtab_make (symtab);
        node_set_symtab (node, scope);

	/* Process the receiver. */
	const type_t *type = check_receiver (receiver, scope);

        assert (type != NULL);
        node_set_action (node, type_add_action (type));

	/* Check the precondition. */
        check_boolean_return (precondition, scope);

	/* Check the body. */
	check_stmt (body, scope);
      }
      break;

    case BindDef:
      {
        node_t* receiver = node_child (node, BIND_RECEIVER);
        node_t* list = node_child (node, BIND_BODY);

	/* Create a new scope in which to enter the receiver. */
	symtab_t* scope = symtab_make (symtab);
        node_set_symtab (node, scope);

	/* Process the receiver. */
	const type_t *type = check_receiver (receiver, scope);

        assert (type != NULL);
        node_set_bind_number (node, type_add_bind (type));

	/* Check the body. */
        NODE_FOREACH (stmt, list) {
          check_bind_stmt (stmt, scope);
        }
      }
      break;

    case InstanceDef:
      {
	node_t *instance_id = node_child (node, INSTANCE_IDENTIFIER);
	string_t instance_str = node_identifier (instance_id);
	node_t *type_id = node_child (node, INSTANCE_TYPE_IDENTIFIER);
	string_t type_str = node_identifier (type_id);

	/* Look up the type. */
	symbol_t *type_symbol = symtab_find (symtab, type_str);
	if (type_symbol == NULL)
	  {
            error_at_line (-1, 0, node_file (node), node_line (node), "%s is not defined in this scope", get (type_str));
	    return;
	  }

	if (!symbol_is_type (type_symbol))
	  {
	    printf ("%s is not a type\n", get (type_str));
	    return;
	  }

	const type_t *type = symbol_type_type (type_symbol);
	if (!type_is_component (type))
	  {
	    printf ("%s is not a component\n", get (type_str));
	    return;
	  }

        node_instance_set_type (node, type);

	/* Look up the instance. */
	symbol_t *instance_symbol = symtab_find (symtab, instance_str);
	if (instance_symbol != NULL)
	  {
	    printf ("%s is already defined in this scope\n",
		    get (instance_str));
	    return;
	  }

	instance_symbol = symbol_make_instance (instance_str, type);

	symtab_enter (symtab, instance_symbol);
      }
      break;

    case ListDef:
      {
        NODE_FOREACH (child, node)
	  {
	    check_def (child, symtab);
	  }
      }
      break;

    case ReactionDef:
      {
	node_t *receiver = node_child (node, REACTION_RECEIVER);
        node_t *identifier = node_child (node, REACTION_IDENTIFIER);
        string_t name = node_identifier (identifier);
        node_t *signature = node_child (node, REACTION_SIGNATURE);
	node_t *body = node_child (node, REACTION_BODY);

	/* Create a new scope in which to enter the receiver. */
	symtab_t* scope = symtab_make (symtab);
        node_set_symtab (node, scope);

	/* Process the receiver. */
	const type_t *type = check_receiver (receiver, scope);

        if (type == NULL) {
          return;
        }

        /* Process the signature. */
        signature_t* sig = check_signature (signature, scope);

	/* Check the body. */
	check_stmt (body, scope);

        /* Add the reaction to the type. */
        action_t* r = type_get_reaction (type, name);

        if (r == NULL) {
          r = type_add_reaction (type, name, sig);
          node_set_action (node, r);
        }
        else {
          unimplemented;
        }
      }
      break;

    case TypeDef:
      {
	node_t *identifier = node_child (node, TYPE_IDENTIFIER);
	string_t identifier_str = node_identifier (identifier);
	node_t *type_spec = node_child (node, TYPE_TYPE_SPEC);

	/* Is the identifier already in the symbol table? */
	symbol_t *symbol = symtab_find_current (symtab, identifier_str);
	if (symbol != NULL)
	  {
	    /* TODO:  Error reporting. */
	    printf ("%s is already defined in this scope\n",
		    get (identifier_str));
	    node_set_type (node, type_make_undefined ());
	    break;
	  }

	/* Process the type spec and name the type. */
	const type_t *type =
	  type_set_name (check_type_spec (type_spec, symtab), identifier_str);

	/* Enter into the symbol table. */
	symtab_enter (symtab, symbol_make_type (type));
      }
      break;

    }
}

#endif
