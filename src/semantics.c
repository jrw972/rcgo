#include "semantics.h"
#include "symtab.h"
#include "debug.h"
#include <error.h>
#include <string.h>
#include "util.h"
#include "instance_set.h"

static const type_t *
check_type_spec (node_t * node, symtab_t * symtab);

static signature_t*
check_signature (node_t * node, symtab_t * symtab)
{
  signature_t* signature = signature_make ();

  NODE_FOREACH (parameter, node) {
    node_t* identifier_list = node_child (parameter);
    node_t* type_spec = node_sibling (identifier_list);
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

	node_t *field;
	for (field = node_child (node); field != NULL;
	     field = node_sibling (field))
	  {
	    node_t *identifier_list = node_child (field);
	    node_t *type_spec = node_sibling (identifier_list);
	    const type_t *field_type = check_type_spec (type_spec, symtab);

	    node_t *identifier;
	    for (identifier = node_child (identifier_list);
		 identifier != NULL; identifier = node_sibling (identifier))
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
        node_t * signature = node_child (node);
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
  node_t *child = node_child (node);
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
  node_t *left = node_child (node);
  node_t *right = node_sibling (left);
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
        node_t* expr = node_child (node);
        node_t* args = node_sibling (expr);

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
	node_t *child = node_child (node);
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
	node_t *child = node_child (node);
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
	node_t *expr = node_child (node);
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
check_boolean_expr (node_t * node, symtab_t * symtab)
{
  semval_t semval = check_expr (node, symtab);
  if (semval_is_undefined (semval))
    {
      return;
    }

  if (!semval_is_boolean (semval))
    {
      /* TODO:  Error reporting. */
      printf ("Expression is not boolean\n");
      return;
    }
}

static void
check_stmt (node_t * node, symtab_t * symtab)
{
  switch (node_stmt_type (node))
    {
    case AssignmentStmt:
      {
	node_t *left = node_child (node);
	semval_t left_sv = check_expr (left, symtab);
	node_t *right = node_sibling (left);
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
	check_expr (node_child (node), symtab);
      }
      break;

    case ListStmt:
      {
	/* Create a new scope. */
	symtab_t scope;
	symtab_init (&scope, symtab);

	/* Check each statment in the list. */
	node_t *child;
	for (child = node_child (node); child != NULL;
	     child = node_sibling (child))
	  {
	    check_stmt (child, &scope);
	  }

	/* Destroy the scope. */
	symtab_fini (&scope);
      }
      break;

    case PrintStmt:
      check_expr (node_child (node), symtab);
      break;

    case TriggerStmt:
      {
        node_t *expr_list = node_child (node);
	node_t *body = node_sibling (expr_list);

        check_expr_list (expr_list, symtab);

	/* Create a new scope and enter this as a pointer to mutable. */
	symtab_t scope;
	symtab_init (&scope, symtab);

	symbol_t *this_symbol = symtab_get_this (&scope);
	const type_t *type = type_make_pointer (type_pointer_base_type
                                                (symbol_variable_type (this_symbol)));
	symtab_enter (&scope,
		      symbol_make_variable (symbol_identifier (this_symbol),
					    type));

	check_stmt (body, &scope);

	/* Destroy the scope. */
	symtab_fini (&scope);
      }
      break;

    case VarStmt:
      {
	/* Extract the children. */
	node_t *identifier_list = node_child (node);
	node_t *type_spec = node_sibling (identifier_list);

	/* Process the type spec. */
	const type_t *type = check_type_spec (type_spec, symtab);

	/* Enter the identifiers into the symbol table. */
	node_t *id;
	for (id = node_child (identifier_list); id != NULL;
	     id = node_sibling (id))
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

  node_t *this_identifier = node_child (node);
  string_t this_str = node_identifier (this_identifier);
  node_t *type_identifier = node_sibling (this_identifier);
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
      node_t* output = node_child (node);
      node_t* input = node_sibling (output);

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
  case TriggerStmt:
  case VarStmt:
    bug ("unhandled case");
  }
}

static void
check_def (node_t * node, symtab_t * symtab)
{
  switch (node_def_type (node))
    {
    case ActionDef:
      {
	node_t *receiver = node_child (node);
	node_t *precondition = node_sibling (receiver);
	node_t *body = node_sibling (precondition);

	/* Create a new scope in which to enter the receiver. */
	symtab_t scope;
	symtab_init (&scope, symtab);

	/* Process the receiver. */
	const type_t *type = check_receiver (receiver, &scope);

        assert (type != NULL);
        node_set_action (node, type_add_action (type));

	/* Check the precondition. */
	check_boolean_expr (precondition, &scope);

	/* Check the body. */
	check_stmt (body, &scope);

	/* Destroy the scope. */
	symtab_fini (&scope);
      }
      break;

    case BindDef:
      {
        node_t* receiver = node_child (node);
        node_t* list = node_sibling (receiver);

	/* Create a new scope in which to enter the receiver. */
	symtab_t scope;
	symtab_init (&scope, symtab);

	/* Process the receiver. */
	const type_t *type = check_receiver (receiver, &scope);

        assert (type != NULL);
        node_set_bind_number (node, type_add_bind (type));

	/* Check the body. */
        NODE_FOREACH (stmt, list) {
          check_bind_stmt (stmt, &scope);
        }

	/* Destroy the scope. */
	symtab_fini (&scope);
      }
      break;

    case InstanceDef:
      {
	node_t *instance_id = node_child (node);
	string_t instance_str = node_identifier (instance_id);
	node_t *type_id = node_sibling (instance_id);
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
	node_t *child;
	for (child = node_child (node); child != NULL;
	     child = node_sibling (child))
	  {
	    check_def (child, symtab);
	  }
      }
      break;

    case ReactionDef:
      {
	node_t *receiver = node_child (node);
        node_t *identifier = node_sibling (receiver);
        string_t name = node_identifier (identifier);
        node_t *signature = node_sibling (identifier);
	node_t *body = node_sibling (signature);

	/* Create a new scope in which to enter the receiver. */
	symtab_t scope;
	symtab_init (&scope, symtab);

	/* Process the receiver. */
	const type_t *type = check_receiver (receiver, &scope);

        if (type == NULL) {
          return;
        }

        /* Process the signature. */
        signature_t* sig = check_signature (signature, &scope);

	/* Check the body. */
	check_stmt (body, &scope);

        /* Add the reaction to the type. */
        action_t* r = type_get_reaction (type, name);

        if (r == NULL) {
          r = type_add_reaction (type, name, sig);
          node_set_action (node, r);
        }
        else {
          unimplemented;
        }

	/* Destroy the scope. */
	symtab_fini (&scope);
      }
      break;

    case TypeDef:
      {
	node_t *identifier = node_child (node);
	string_t identifier_str = node_identifier (identifier);
	node_t *type_spec = node_sibling (identifier);

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

typedef struct instance_type_t instance_type_t;
struct instance_type_t {
  size_t instance;
  const type_t* type;
};
static instance_type_t* instances = NULL;
static size_t instances_size = 0;
static size_t instances_capacity = 0;

static size_t
insert_instance_type (const type_t* type)
{
  size_t inst = instances_size;
  instance_type_t v = { instance: inst, type: type };
  VECTOR_PUSH (instances, instance_type_t, v);
  return inst;
}

typedef struct instance_field_instance_t instance_field_instance_t;
struct instance_field_instance_t {
  size_t instance;
  const field_t* field;
  size_t subinstance;
};
static instance_field_instance_t* subinstances = NULL;
static size_t subinstances_size = 0;
static size_t subinstances_capacity = 0;

static void
insert_instance_field_instance (size_t instance,
                                const field_t* field,
                                size_t subinstance)
{
  instance_field_instance_t v = { instance: instance, field: field, subinstance: subinstance };
  VECTOR_PUSH (subinstances, instance_field_instance_t, v);
}

static size_t
instantiate (const type_t* type)
{
  // Get the instance.
  size_t instance = insert_instance_type (type);

  const field_t* field;
  for (field = type_field_list (type); field != NULL; field = field_next (field)) {
    const type_t* fieldtype = field_type (field);
    if (type_is_component (fieldtype)) {
      size_t subinstance = instantiate (fieldtype);
      insert_instance_field_instance (instance, field, subinstance);
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
static void
enumerate_instances (node_t* node)
{
  switch (node_def_type (node))
    {
    case InstanceDef:
      {
        // Get the type.
        const type_t* type = node_instance_get_type (node);
        instantiate (type);
      }
      break;

    case ListDef:
      {
        NODE_FOREACH (child, node) {
          enumerate_instances (child);
        }
      }
      break;

    case ActionDef:
    case BindDef:
    case ReactionDef:
    case TypeDef:
      break;

    }
}

static const field_t*
extract_selected_field (node_t* node)
{
  semval_t semval = node_get_semval (node);
  select_result_t selected_field = semval_selected_field (semval);
  if (selected_field.kind == SelectResultField)
    {
      return selected_field.field;
    }
  else
    {
      switch (node_expr_type (node))
        {
        case CallExpr:
          return extract_selected_field (node_child (node));
        case ExplicitDereferenceExpr:
          unimplemented;
        case IdentifierExpr:
          unimplemented;
        case ImplicitDereferenceExpr:
          unimplemented;
        case LogicAndExpr:
          unimplemented;
        case LogicNotExpr:
          unimplemented;
        case LogicOrExpr:
          unimplemented;
        case SelectExpr:
          unimplemented;
        }

      bug ("unhandled case");
    }
}

static void
analyze_triggers (action_t* action,
                  node_t* node)
{
  switch (node_stmt_type (node))
    {
    case TriggerStmt:
      {
        trigger_group_t* trigger_group = action_add_trigger_group (action);
        node_t* expr_list = node_child (node);
        NODE_FOREACH (child, expr_list)
          {
            const field_t* field = extract_selected_field (child);
            assert (field != NULL);
            assert (type_is_port (field_type (field)));
            trigger_group_add_field (trigger_group, field);
          }
      }
      break;
    case ListStmt:
      {
        NODE_FOREACH (child, node)
          {
            analyze_triggers (action, child);
          }
      }
      break;
    case AssignmentStmt:
    case BindListStmt:
    case BindStmt:
    case ExprStmt:
    case PrintStmt:
    case VarStmt:
      break;

    }
}

/* The goal of this function is to determine which ports are triggered by each each trigger statement in each action. */
static void
analyze_actions (node_t* node)
{
  switch (node_def_type (node))
    {
    case ActionDef:
      {
        action_t* action = node_get_action (node);
	node_t *receiver = node_child (node);
	node_t *precondition = node_sibling (receiver);
	node_t *body = node_sibling (precondition);
        analyze_triggers (action, body);
      }
      break;

    case ReactionDef:
      {
        action_t* action = node_get_action (node);
	node_t *receiver = node_child (node);
        node_t *identifier = node_sibling (receiver);
        node_t *signature = node_sibling (identifier);
	node_t *body = node_sibling (signature);
        analyze_triggers (action , body);
      }
      break;

    case ListDef:
      {
        NODE_FOREACH (child, node) {
          analyze_actions (child);
        }
      }
      break;

    case InstanceDef:
    case BindDef:
    case TypeDef:
      break;

    }
}

static void
populate_binding (node_t* node,
                  binding_t* binding,
                  bool input)
{
  switch (node_expr_type (node))
    {
    case CallExpr:
      unimplemented;
    case IdentifierExpr:
      // Do nothing.
      break;
    case LogicAndExpr:
      unimplemented;
    case LogicNotExpr:
      unimplemented;
    case LogicOrExpr:
      unimplemented;
    case ImplicitDereferenceExpr:
    case ExplicitDereferenceExpr:
    case SelectExpr:
      populate_binding (node_child (node), binding, input);
      break;
    }

  semval_t semval = node_get_semval (node);
  select_result_t selected_field = semval_selected_field (semval);

  switch (selected_field.kind)
    {
    case SelectResultField:
    case SelectResultReaction:
      binding_add (binding, selected_field, input);
      break;
    case SelectResultUndefined:
      break;
    }
}

/* The goal of this function is to determine which ports drive which reactions. */
static void
analyze_bindings (node_t* node)
{
  switch (node_def_type (node))
    {
    case BindDef:
      {
        node_t* receiver = node_child (node);
        const type_t* type = node_receiver_get_type (receiver);
        node_t* list = node_sibling (receiver);

	/* Check the body. */
        NODE_FOREACH (stmt, list) {
          binding_t* binding = binding_make ();
          node_t* output = node_child (stmt);
          populate_binding (output, binding, false);
          node_t* input = node_sibling (output);
          populate_binding (input, binding, true);
          type_add_binding (type, binding);
        }

      }
      break;

    case ListDef:
      {
        NODE_FOREACH (child, node) {
          analyze_bindings (child);
        }
      }
      break;

    case ActionDef:
    case ReactionDef:
    case InstanceDef:
    case TypeDef:
      break;
    }
}

static size_t
lookup_subinstance (size_t instance,
                    const field_t* field)
{
  VECTOR_FOREACH (pos, limit, subinstances, instance_field_instance_t)
    {
      if (pos->instance == instance && pos->field == field)
        {
          return pos->subinstance;
        }
    }

  bug ("no subinstance\n");
}

typedef struct concrete_binding_t concrete_binding_t;
struct concrete_binding_t {
  size_t output_instance;
  const field_t* output_port;
  size_t input_instance;
  const action_t* input_reaction;
};
static concrete_binding_t* bindings = NULL;
static size_t bindings_size = 0;
static size_t bindings_capacity = 0;

static void
add_binding (size_t output_instance, const field_t* output_port,
             size_t input_instance, const action_t* input_reaction)
{
  concrete_binding_t v = { output_instance: output_instance,
                           output_port: output_port,
                           input_instance: input_instance,
                           input_reaction: input_reaction };
  VECTOR_PUSH (bindings, concrete_binding_t, v);
}

static void
enumerate_bindings ()
{
  VECTOR_INIT (bindings, concrete_binding_t);

  // For each instance.
  VECTOR_FOREACH (instance_pos, instance_limit, instances, instance_type_t)
    {
      size_t instance = instance_pos->instance;
      const type_t* type = instance_pos->type;
      // Enumerate the bindings.
      binding_t *binding_pos, *binding_limit;
      for (binding_pos = type_bindings_begin (type), binding_limit = type_bindings_end (type);
           binding_pos != binding_limit;
           binding_pos = type_bindings_next (binding_pos))
        {
          select_result_t *result_pos, *result_limit;

          size_t output_instance = instance;
          result_pos = binding_output_begin (binding_pos);
          result_limit = binding_output_end (binding_pos);
          select_result_t output_port = *result_pos;
          result_pos = binding_output_next (result_pos);
          while (result_pos != result_limit)
            {
              output_instance = lookup_subinstance (output_instance, output_port.field);
              output_port = *result_pos;
              result_pos = binding_output_next (result_pos);
            }

          size_t input_instance = instance;
          result_pos = binding_input_begin (binding_pos);
          result_limit = binding_input_end (binding_pos);
          select_result_t input_reaction = *result_pos;
          result_pos = binding_input_next (result_pos);
          while (result_pos != result_limit)
            {
              input_instance = lookup_subinstance (input_instance, input_reaction.field);
              input_reaction = *result_pos;
              result_pos = binding_input_next (result_pos);
            }

          add_binding (output_instance, output_port.field, input_instance, input_reaction.reaction);
        }
    }
}

static void
transitive_closure (instance_set_t* set,
                    size_t instance,
                    const action_t* action)
{
  if (instance_set_contains (set, instance))
    {
      error (-1, 0, "system is non-deterministic");
    }
  instance_set_insert (set, instance);

  trigger_group_t* trigger_pos, *trigger_limit;
  for (trigger_pos = action_trigger_begin (action), trigger_limit = action_trigger_end (action);
       trigger_pos != trigger_limit;
       trigger_pos = trigger_next (trigger_pos))
    {
      const field_t **field_pos, **field_limit;
      for (field_pos = trigger_field_begin (trigger_pos), field_limit = trigger_field_end (trigger_pos);
           field_pos != field_limit;
           field_pos = trigger_field_next (field_pos))
        {
          const field_t* field = *field_pos;
          VECTOR_FOREACH (binding_pos, binding_limit, bindings, concrete_binding_t)
            {
              if (binding_pos->output_instance == instance &&
                  binding_pos->output_port == field)
                {
                  transitive_closure (set,
                                      binding_pos->input_instance,
                                      binding_pos->input_reaction);
                }
            }
        }
    }
 }

static void
analyze_composition ()
{
  instance_set_t* set = instance_set_make ();

  // For each instance.
  VECTOR_FOREACH (instance_pos, instance_limit, instances, instance_type_t)
    {
      size_t instance = instance_pos->instance;
      const type_t* type = instance_pos->type;
      // For each action in the type.
      action_t *action_pos, *action_end;
      for (action_pos = type_actions_begin (type), action_end = type_actions_end (type);
           action_pos != action_end;
           action_pos = action_next (action_pos))
        {
          if (action_is_action (action_pos))
            {
              instance_set_clear (set);
              transitive_closure (set, instance, action_pos);
            }
        }
    }
}

int
check_semantics (node_t * node)
{
  /* Populate the top-level symbol table. */
  symtab_t top;
  symtab_init (&top, NULL);

  /* Insert types. */
  {
    const type_t *type = type_make_bool ();
    type_set_name (type, enter ("bool"));
    symtab_enter (&top, symbol_make_type (type));
  }

  /* Insert untyped boolean constants. */
  symtab_enter (&top,
		symbol_make_constant (enter ("true"),
				      abstract_value_make_untyped_value
				      (untyped_value_make_bool (true))));
  symtab_enter (&top,
		symbol_make_constant (enter ("false"),
				      abstract_value_make_untyped_value
				      (untyped_value_make_bool (false))));

  check_def (node, &top);

  symtab_fini (&top);

  VECTOR_INIT (instances, instance_type_t);
  VECTOR_INIT (subinstances, instance_field_instance_t);
  enumerate_instances (node);

  {
    printf ("%s\t%s\n", "instance", "type");
    VECTOR_FOREACH (ptr, limit, instances, instance_type_t)
      {
        printf ("%zd\t\t%s\n", ptr->instance, type_to_string (ptr->type));
      }
  }

  {
    printf ("%s\t%s\t%s\n", "instance", "field", "subinstance");
    VECTOR_FOREACH (ptr, limit, subinstances, instance_field_instance_t)
      {
        printf ("%zd\t\t%zd\t%zd\n", ptr->instance, field_number (ptr->field), ptr->subinstance);
      }
  }

  analyze_actions (node);

  analyze_bindings (node);

  enumerate_bindings ();

  analyze_composition (node);

  return 0;
}
