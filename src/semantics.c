#include "semantics.h"
#include <stdio.h>
#include "symtab.h"
#include "debug.h"

static const type_t*
check_type_spec (node_t * node, symtab_t * symtab)
{
  switch (node_type_spec_type (node))
    {
    case FieldListTypeSpec:
      {
        type_t* type = NULL;

        switch (node_get_field_list_type (node)) {
        case Component:
          type = type_make_component ();
          break;
        }

        node_t* field;
        for (field = node_child (node); field != NULL; field = node_sibling (field)) {
          node_t* identifier_list = node_child (field);
          node_t* type_spec = node_sibling (identifier_list);
          const type_t* field_type = check_type_spec (type_spec, symtab);

          node_t* identifier;
          for (identifier = node_child (identifier_list); identifier != NULL; identifier = node_sibling (identifier)) {
            string_t id = node_identifier (identifier);
            if (!type_append_field (type, id, field_type)) {
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
            /* TODO:  Error reporting. */
            printf ("%s is not defined in this scope\n", get (identifier));
            node_set_type (node, type_make_undefined ());
            break;
          }

        /* Ensure that the symbol refers to a type. */
        if (!symbol_is_type (symbol)) {
          /* TODO:  Error reporting. */
          printf ("%s is not a type\n", get (identifier));
          node_set_type (node, type_make_undefined ());
          break;
        }

        node_set_type (node, symbol_type_type (symbol));
      }
      break;
    }

  return node_get_type (node);
}

static semval_t
check_expr (node_t * node, symtab_t * symtab);

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

static semval_t
check_expr (node_t * node, symtab_t * symtab)
{
  switch (node_expr_type (node)) {
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
          /* TODO:  Error reporting. */
          printf ("%s is not defined in this scope\n", get (identifier));
          node_set_semval (node, semval_undefined ());
          break;
        }

      if (symbol_is_variable (symbol)) {
        node_set_semval (node, semval_make_reference (reference_make (abstract_value_make_typed (symbol_variable_type (symbol)), true)));
      }
      else if (symbol_is_constant (symbol)) {
        node_set_semval (node, semval_make_reference (reference_make (symbol_constant_value (symbol), false)));
      }
      else {
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
      node_t* expr = node_child (node);
      string_t identifier = node_get_select_identifier (node);
      semval_t semval = check_expr (expr, symtab);

      if (semval_is_undefined (semval))
        {
          node_set_semval (node, semval_undefined ());
          break;
        }

      semval = semval_select (semval, identifier);
      if (semval_is_undefined (semval))
        {
          /* TODO:  Error reporting. */
          printf ("could not select\n");
        }
      node_set_semval (node, semval);
    }
    break;
  }

  return node_get_semval (node);
}

static void
check_boolean_expr (node_t* node, symtab_t* symtab)
{
  semval_t semval = check_expr (node, symtab);
  if (semval_is_undefined (semval)) {
    return;
  }

  if (!semval_is_boolean (semval)) {
    /* TODO:  Error reporting. */
    printf ("Expression is not boolean\n");
    return;
  }
}

static void
check_stmt (node_t * node, symtab_t * symtab)
{
  switch (node_stmt_type (node)) {
  case AssignmentStmt:
    {
      node_t *left = node_child (node);
      semval_t left_sv = check_expr (left, symtab);
      node_t *right = node_sibling (left);
      semval_t right_sv = check_expr (right, symtab);

      if (!semval_assignable (left_sv, right_sv))
        {
          /* TODO:  Error reporting. */
          printf ("could not assign\n");
          semval_print (left_sv);
          printf (" = ");
          semval_print (right_sv);
          printf ("\n");
        }
    }
    break;

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
      node_t* body = node_child (node);

      /* Create a new scope and enter this as a pointer to mutable. */
      symtab_t scope;
      symtab_init (&scope, symtab);

      symbol_t* this_symbol = symtab_get_this (&scope);
      const type_t* type = type_get_pointer_to_mutable (type_pointer_base_type (symbol_variable_type (this_symbol)));
      symtab_enter (&scope, symbol_make_variable (symbol_identifier (this_symbol), type));

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
      const type_t* type = check_type_spec (type_spec, symtab);

      /* Enter the identifiers into the symbol table. */
      node_t *id;
      for (id = node_child (identifier_list); id != NULL;
           id = node_sibling (id))
        {
          string_t id_str = node_identifier (id);
          if (symtab_find_current (symtab, id_str) == NULL) {
            symtab_enter (symtab, symbol_make_variable (id_str, type));
          }
          else {
            /* TODO:  Error reporting. */
            printf ("%s is already defined in this scope\n", get (id_str));
          }
        }
    }
    break;
  }
}

static const type_t*
check_receiver (node_t* node,
                symtab_t * symtab)
{
  node_t* this_identifier = node_child (node);
  string_t this_str = node_identifier (this_identifier);
  node_t* type_identifier = node_sibling (this_identifier);
  string_t type_str = node_identifier (type_identifier);

  /* Look up the type. */
  symbol_t* symbol = symtab_find (symtab, type_str);
  if (symbol == NULL) {
    /* TODO:  Error reporting. */
    printf ("%s is not defined in this scope\n", get (type_str));
    return NULL;
  }

  if (!symbol_is_type (symbol)) {
    /* TODO:  Error reporting. */
    printf ("%s does not refer to a type\n", get (type_str));
    return NULL;
  }

  const type_t* type = symbol_type_type (symbol);

  /* Check that the identifier is not defined in the local scope. */
  symbol = symtab_find_current (symtab, this_str);
  if (symbol != NULL) {
    /* TODO:  Error reporting. */
    printf ("%s is already defined in this scope\n", get (this_str));
    return NULL;
  }

  /* Define this_str to the appropriate type. */
  const type_t* pointer_type = NULL;
  switch (node_receiver_type (node)) {
  case PointerToImmutable:
    pointer_type = type_get_pointer_to_immutable (type);
    break;
  }

  symbol_t* s = symbol_make_variable (this_str, pointer_type);
  symbol_set_as_this (s);
  symtab_enter (symtab, s);

  node_receiver_set_type (node, type);
  return type;
}

static void
check_def (node_t * node, symtab_t * symtab)
{
  switch (node_def_type (node)) {
  case ActionDef:
    {
      node_t* receiver = node_child (node);
      node_t* precondition = node_sibling (receiver);
      node_t* body = node_sibling (precondition);

      /* Create a new scope in which to enter the receiver. */
      symtab_t scope;
      symtab_init (&scope, symtab);

      /* Process the receiver. */
      const type_t* type = check_receiver (receiver, &scope);

      if (type != NULL) {
        node_set_action_number (node, type_add_action (type));
      }

      /* Check the precondition. */
      check_boolean_expr (precondition, &scope);

      /* Check the body. */
      check_stmt (body, &scope);

      /* Destroy the scope. */
      symtab_fini (&scope);
    }
    break;

  case InstanceDef:
    {
      node_t* instance_id = node_child (node);
      string_t instance_str = node_identifier (instance_id);
      node_t* type_id = node_sibling (instance_id);
      string_t type_str = node_identifier (type_id);

      /* Look up the type. */
      symbol_t* type_symbol = symtab_find (symtab, type_str);
      if (type_symbol == NULL) {
        printf ("%s is not defined in this scope\n", get (type_str));
        return;
      }

      if (!symbol_is_type (type_symbol)) {
        printf ("%s is not a type\n", get (type_str));
        return;
      }

      const type_t* type = symbol_type_type (type_symbol);
      if (!type_is_component (type)) {
        printf ("%s is not a component\n", get (type_str));
        return;
      }

      /* Look up the instance. */
      symbol_t* instance_symbol = symtab_find (symtab, instance_str);
      if (instance_symbol != NULL) {
        printf ("%s is already defined in this scope\n", get (instance_str));
        return;
      }

      instance_symbol = symbol_make_instance (instance_str, type);

      symtab_enter (symtab, instance_symbol);
    }
    break;

  case ListDef:
    {
      node_t* child;
      for (child = node_child (node); child != NULL; child = node_sibling (child)) {
        check_def (child, symtab);
      }
    }
    break;

  case TypeDef:
    {
      node_t* identifier = node_child (node);
      string_t identifier_str = node_identifier (identifier);
      node_t* type_spec = node_sibling (identifier);

      /* Is the identifier already in the symbol table? */
      symbol_t *symbol = symtab_find_current (symtab, identifier_str);
      if (symbol != NULL)
        {
          /* TODO:  Error reporting. */
          printf ("%s is already defined in this scope\n", get (identifier_str));
          node_set_type (node, type_make_undefined ());
          break;
        }

      /* Process the type spec and name the type. */
      const type_t* type = type_set_name (check_type_spec (type_spec, symtab), identifier_str);

      /* Enter into the symbol table. */
      symtab_enter (symtab, symbol_make_type (type));
    }
    break;
    check_stmt (node, symtab); /* TODO:  Remove this line. */
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
    const type_t* type = type_make_bool ();
    type_set_name (type, enter ("bool"));
    symtab_enter (&top, symbol_make_type (type));
  }

  /* Insert untyped boolean constants. */
  symtab_enter (&top,
		symbol_make_constant (enter ("true"),
                                      abstract_value_make_untyped_value
                                      (untyped_value_make_bool
                                       (true))));
  symtab_enter (&top,
		symbol_make_constant (enter ("false"),
                                      abstract_value_make_untyped_value
                                      (untyped_value_make_bool
                                       (false))));

  check_def (node, &top);

  symtab_fini (&top);

  return 0;
}
