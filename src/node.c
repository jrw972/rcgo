#include "node.h"
#include <stdlib.h>
#include <string.h>
#include <strtab.h>
#include <assert.h>

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
make (void)
{
  node_t *n = malloc (sizeof (node_t));
  memset (n, 0, sizeof (node_t));
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

static void
add_child (node_t * parent, node_t * child)
{
  if (parent->child == NULL)
    {
      parent->child = child;
    }
  else
    {
      add_sibling (parent->child, child);
    }
}

static node_t *
make1 (NodeType type, node_t * child)
{
  node_t *n = make ();
  n->type = type;
  add_child (n, child);
  return n;
}

node_t *
node_make_identifier (char *id)
{
  node_t *n = make ();
  n->type = Identifier;
  n->identifier.id = id;
  return n;
}

string_t
node_identifier (node_t * node)
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
node_make_to_typed_value (node_t * child)
{
  return make1 (ToTypedValue, child);
}

node_t *
node_make_logic_not (node_t * child)
{
  return make1 (LogicNot, child);
}

node_t *node_make_logic_and (node_t * left,
                             node_t * right)
{
  node_t *n = make ();
  n->type = LogicAnd;
  add_child (n, left);
  add_child (n, right);
  return n;
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
	case Dereference:
        case LogicAnd:
	case LogicNot:
	case Identifier:
	case ToTypedValue:
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
