#include "semantics.h"
#include <stdio.h>
#include "symtab.h"
#include "debug.h"

static const type_t*
check_type_spec (node_t * node, symtab_t * symtab)
{
  switch (node_type_spec_type (node))
    {
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

        node_set_type (node, symbol_get_type (symbol));
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
  case IdentifierExpr:
    {
      /* Look up the identifier in the symbol table. */
      string_t identifier = node_identifier_expr_identifier (node);
      symbol_t *symbol = symtab_find (symtab, identifier);
      if (symbol != NULL)
        {
          node_set_semval (node, symbol_get_semval (symbol));
        }
      else
        {
          /* TODO:  Error reporting. */
          printf ("%s is not defined in this scope\n", get (identifier));
          node_set_semval (node, semval_undefined ());
        }
    }
    break;

  case ImplicitDereferenceExpr:
    {
      /* Process the child. */
      node_t *child = node_child (node);
      check_expr (child, symtab);
      semval_t child_sv = node_get_semval (child);

      semval_t this_sv;
      if (semval_is_undefined (child_sv))
        {
          this_sv = semval_undefined ();
        }
      else
        {
          this_sv = semval_dereference (child_sv);
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

  }

  return node_get_semval (node);
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
      /* Check each statment in the list. */
      node_t *child;
      for (child = node_child (node); child != NULL;
           child = node_sibling (child))
        {
          check_stmt (child, symtab);
        }
    }
    break;

  case PrintStmt:
    check_expr (node_child (node), symtab);
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
          symtab_enter (symtab,
                        symbol_make_semval (node_identifier (id),
                                            semval_make_reference
                                            (semval_make_value
                                             (abstract_value_make_typed (type)))));
        }
    }
    break;
  }
}

int
check_semantics (node_t * node)
{
  /* Populate the top-level symbol table. */
  symtab_t top;
  symtab_init (&top, NULL);

  /* Insert types. */
  symtab_enter (&top,
        	symbol_make_type (enter ("bool"),
                                  type_make_bool ()));

  /* Insert untyped boolean constants. */
  symtab_enter (&top,
		symbol_make_semval (enter ("true"),
                                    semval_make_reference (semval_make_value
                                                           (abstract_value_make_untyped_value
                                                            (untyped_value_make_bool
                                                             (true))))));
  symtab_enter (&top,
		symbol_make_semval (enter ("false"),
                                    semval_make_reference (semval_make_value
                                                           (abstract_value_make_untyped_value
                                                            (untyped_value_make_bool
                                                             (false))))));

  symtab_set_top (&top);

  check_stmt (node, &top);

  return 0;
}
