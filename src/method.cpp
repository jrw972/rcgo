#include "method.hpp"
#include "debug.hpp"
#include "util.hpp"
#include "type.hpp"

struct method_t {
  method_t ()
    : locals_size (0)
  { }

  named_type_t *named_type;	/* Back-pointer to named type. */
  ast_t* node;
  string_t name;
  func_type_t *func_type;
  size_t locals_size;
};

method_t*
method_make (named_type_t * named_type,
             ast_t* node,
             string_t identifier, const signature_type_t * signature, const type_t* return_type)
{
  method_t* g = new method_t;
  g->named_type = named_type;
  g->node = node;
  g->name = identifier;
  g->func_type = new func_type_t (signature, return_type);
  return g;
}

named_type_t*
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

const signature_type_t*
method_signature (const method_t* method)
{
  return method->func_type->signature ();
}

const type_t*
method_return_type (const method_t* method)
{
  return method->func_type->return_type ();
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

size_t method_arity (const method_t * method)
{
  return method->func_type->signature ()->arity ();
}
