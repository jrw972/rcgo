#include "node.h"
#include <stdlib.h>
#include <string.h>
#include <strtab.h>
#include "debug.h"

struct node_t
{
  NodeType type;
  node_t *child;
  node_t *sibling;
  union
  {
    struct
    {
      string_t id;
    } identifier;
  };
  semval_t semval;
};

static node_t *
make (NodeType type)
{
  node_t *n = malloc (sizeof (node_t));
  memset (n, 0, sizeof (node_t));
  n->type = type;
  n->semval = semval_undefined ();
  return n;
}

static void
add_sibling (node_t * c1, node_t * c2)
{
  if (c1->sibling == NULL)
    {
      c1->sibling = c2;
    }
  else
    {
      add_sibling (c1->sibling, c2);
    }
}

node_t*
node_add_child (node_t * parent, node_t * child)
{
  if (parent->child == NULL)
    {
      parent->child = child;
    }
  else
    {
      add_sibling (parent->child, child);
    }
  return parent;
}

static node_t *
make1 (NodeType type, node_t * child)
{
  node_t *n = make (type);
  return node_add_child (n, child);
}

static node_t *
make2 (NodeType type, node_t * child1, node_t* child2)
{
  node_t *n = make (type);
  node_add_child (n, child1);
  return node_add_child (n, child2);
}

void node_print (const node_t* node,
                 size_t indent)
{
  size_t i;
  for (i = 0; i != indent; ++i) {
    printf (" ");
  }

  switch (node->type) {
  case AssignmentStmt:
    printf ("AssignmentStmt");
    break;
  case Dereference:
    printf ("Dereference");
    break;
  case ExprStmt:
    printf ("ExprStmt");
    break;
  case Identifier:
    printf ("Identifier: %s", get (node_identifier (node)));
    break;
  case List:
    printf ("List");
    break;
  case LogicAnd:
    printf ("LogicAnd");
    break;
  case LogicNot:
    printf ("LogicNot");
    break;
  case LogicOr:
    printf ("LogicOr");
    break;
  case PrintStmt:
    printf ("PrintStmt");
    break;
  case VarStmt:
    printf ("VarStmt");
    break;
  }

  printf ("\n");

  node_t* child;
  for (child = node->child; child != NULL; child = node_sibling (child)) {
    node_print (child, indent + 2);
  }
}

node_t *
node_make_identifier (char *id)
{
  node_t *n = make (Identifier);
  n->identifier.id = id;
  return n;
}

string_t
node_identifier (const node_t * node)
{
  assert (node->type == Identifier);
  return node->identifier.id;
}

node_t *
node_make_dereference (node_t * child)
{
  return make1 (Dereference, child);
}

node_t *
node_make_logic_not (node_t * child)
{
  return make1 (LogicNot, child);
}

node_t *node_make_logic_and (node_t * left,
                             node_t * right)
{
  return make2 (LogicAnd, left, right);
}

node_t *node_make_logic_or (node_t * left,
                            node_t * right)
{
  return make2 (LogicOr, left, right);
}

node_t *node_make_list (void)
{
  return make (List);
}

node_t *node_make_expr_stmt (node_t * expr)
{
  return make1 (ExprStmt, expr);
}

node_t * node_make_print_stmt (node_t* expr)
{
  return make1 (PrintStmt, expr);
}

node_t *node_make_var_stmt (node_t * identifier_list,
                            node_t * type_spec)
{
  return make2 (VarStmt, identifier_list, type_spec);
}

node_t *node_make_assignment_stmt (node_t * lvalue,
                                   node_t * rvalue)
{
  return make2 (AssignmentStmt, lvalue, rvalue);
}

void
node_free (node_t * node)
{
  if (node != NULL)
    {
      node_free (node->child);
      node_free (node->sibling);

      switch (node->type)
	{
        case AssignmentStmt:
	case Dereference:
        case ExprStmt:
        case List:
        case LogicAnd:
	case LogicNot:
	case LogicOr:
	case Identifier:
        case PrintStmt:
        case VarStmt:
	  break;
	}
    }
}

NodeType
node_type (node_t * node)
{
  return node->type;
}

node_t *
node_child (node_t * node)
{
  return node->child;
}

node_t *
node_sibling (node_t * node)
{
  return node->sibling;
}

void
node_set_semval (node_t * node, semval_t semval)
{
  node->semval = semval;
}

semval_t
node_get_semval (node_t * node)
{
  return node->semval;
}
