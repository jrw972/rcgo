#include "method.h"
#include "debug.h"
#include "util.h"
#include "type.h"

struct method_t {
  type_t *named_type;	/* Back-pointer to named type. */
  ast_t* node;
  string_t name;
  type_t *func_type;
  size_t locals_size;
};

method_t*
method_make (type_t * named_type,
             ast_t* node,
             string_t identifier, type_t * signature, type_t* return_type)
{
  method_t* g = xmalloc (sizeof (method_t));
  g->named_type = named_type;
  g->node = node;
  g->name = identifier;
  g->func_type = type_make_func (signature, return_type);
  return g;
}

type_t*
method_named_type (const method_t* method)
{
  return method->named_type;
}

ast_t*
method_node (const method_t* method)
{
  return method->node;
}

type_t*
method_type (const method_t* method)
{
  return method->func_type;
}

const type_t*
method_signature (const method_t* method)
{
  return type_func_signature (method->func_type);
}

type_t*
method_return_type (const method_t* method)
{
  return type_func_return_type (method->func_type);
}

string_t
method_name (const method_t* method)
{
  return method->name;
}

void method_set_locals_size (method_t* method,
                             size_t size)
{
  method->locals_size = size;
}

size_t method_get_locals_size (const method_t * method)
{
  return method->locals_size;
}
