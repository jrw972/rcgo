#include "semantics.h"
#include <stdio.h>
#include "symtab.h"
#include "debug.h"

static void check (node_t * node, symtab_t * symtab);

static void
check_unary (node_t * node,
	     symtab_t * symtab,
	     semval_t (*func) (semval_t), const char *message)
{
  /* Process the child. */
  node_t *child = node_child (node);
  check (child, symtab);
  semval_t child_sv = node_get_semval (child);

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
  node_set_semval (node, this_sv);
}

static void
check_binary (node_t * node,
              symtab_t * symtab,
              semval_t (*func) (semval_t, semval_t), const char *message)
{
  /* Process the children. */
  node_t *left = node_child (node);
  node_t *right = node_sibling (left);
  check (left, symtab);
  check (right, symtab);
  semval_t left_sv = node_get_semval (left);
  semval_t right_sv = node_get_semval (right);

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
  node_set_semval (node, this_sv);
}

static void
check_type_spec (node_t* node, symtab_t* symtab)
{
  switch (node_type (node)) {
  case AssignmentStmt:
  case Dereference:
  case ExprStmt:
  case List:
  case LogicAnd:
  case LogicNot:
  case LogicOr:
  case PrintStmt:
  case VarStmt:
    bug ("node must be a type spec");

  case Identifier:
    {
      /* Look up the identifier in the symbol table. */
      string_t identifier = node_identifier (node);
      symbol_t *symbol = symtab_find (symtab, identifier);
      if (symbol == NULL)
        {
          /* TODO:  Error reporting. */
          printf ("%s is not defined in this scope\n", get (identifier));
          node_set_semval (node, semval_undefined ());
          return;
        }

      /* Ensure that the symbol refers to a type. */
      semval_t semval = symbol_get_semval (symbol);

      if (!semval_is_type (semval))
        {
          /* TODO:  Error reporting. */
          printf ("%s is not a type\n", get (identifier));
          node_set_semval (node, semval_undefined ());
          return;
        }

      node_set_semval (node, semval);
    }
    break;

  }
}

static void
check (node_t * node, symtab_t * symtab)
{
  switch (node_type (node))
    {
    case AssignmentStmt:
      {
        node_t* left = node_child (node);
        check (left, symtab);
        semval_t left_sv = node_get_semval (left);

        node_t* right = node_sibling (left);
        check (right, symtab);
        semval_t right_sv = node_get_semval (right);

        if (!semval_assignable (left_sv, right_sv)) {
          /* TODO:  Error reporting. */
          printf ("could not assign\n");
          semval_print (left_sv);
          printf (" = ");
          semval_print (right_sv);
          printf ("\n");
        }
      }
      break;

    case Dereference:
      {
	/* Process the child. */
	node_t *child = node_child (node);
	check (child, symtab);
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

    case ExprStmt:
      {
        /* Check the expression. */
        check (node_child (node), symtab);
      }
      break;

    case Identifier:
      {
	/* Look up the identifier in the symbol table. */
	string_t identifier = node_identifier (node);
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

    case List:
      {
        /* Check each statment in the list. */
        node_t* child;
        for (child = node_child (node); child != NULL; child = node_sibling (child)) {
          check (child, symtab);
        }
      }
      break;

    case LogicAnd:
      check_binary (node, symtab, semval_logic_and, "could not do logic and");
      break;

    case LogicNot:
      check_unary (node, symtab, semval_logic_not, "could not do logic not");
      break;

    case LogicOr:
      check_binary (node, symtab, semval_logic_or, "could not do logic or");
      break;

    case PrintStmt:
      check (node_child (node), symtab);
      break;

    case VarStmt:
      {
        /* Extract the children. */
        node_t* identifier_list = node_child (node);
        node_t* type_spec = node_sibling (identifier_list);

        /* Process the type spec. */
        check_type_spec (type_spec, symtab);
        semval_t type_semval = node_get_semval (type_spec);
        type_t* type = semval_get_type (type_semval);

        /* Enter the identifiers into the symbol table. */
        node_t* id;
        for (id = node_child (identifier_list); id != NULL; id = node_sibling (id)) {
          symtab_enter (symtab, symbol_make (node_identifier (id), semval_make_reference (semval_make_value (abstract_value_make_typed (type)))));
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
		symbol_make (enter ("bool"),
			     semval_make_type (type_make_bool ())));

  /* Insert untyped boolean constants. */
  symtab_enter (&top,
		symbol_make (enter ("true"),
			     semval_make_reference (semval_make_value
						    (abstract_value_make_untyped_value
						     (untyped_value_make_bool
						      (true))))));
  symtab_enter (&top,
		symbol_make (enter ("false"),
			     semval_make_reference (semval_make_value
						    (abstract_value_make_untyped_value
						     (untyped_value_make_bool
						      (false))))));

  symtab_set_top (&top);

  check (node, &top);

  return 0;
}
