#include "getter.h"
#include "debug.h"
#include "util.h"
#include "type.h"

struct getter_t {
  type_t *component_type;	/* Back-pointer to component type. */
  ast_t* node;
  string_t name;
  type_t *getter_type;
};

getter_t*
getter_make (type_t * type,
             ast_t* node,
             string_t identifier, type_t * signature, type_t* return_type)
{
  getter_t* g = xmalloc (sizeof (getter_t));
  g->component_type = type;
  g->node = node;
  g->name = identifier;
  g->getter_type = type_make_getter (signature, return_type);
  return g;
}

type_t*
getter_component_type (const getter_t* getter)
{
  return getter->component_type;
}

ast_t*
getter_node (const getter_t* getter)
{
  return getter->node;
}

type_t*
getter_type (const getter_t* getter)
{
  return getter->getter_type;
}

const type_t*
getter_signature (const getter_t* getter)
{
  return type_getter_signature (getter->getter_type);
}

type_t*
getter_return_type (const getter_t* getter)
{
  return type_getter_return_type (getter->getter_type);
}

string_t
getter_name (const getter_t* getter)
{
  return getter->name;
}

void
getter_add_getter_call (getter_t* getter,
                        getter_call_t* gc)
{
  unimplemented;
}
