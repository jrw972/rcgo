#include "function.hpp"
#include "util.hpp"
#include "debug.hpp"
#include "type.hpp"

struct function_t {
  function_t ()
    : locals_size (0)
  { }

  ast_t* node;
  string_t name;
  func_type_t* func_type;
  size_t locals_size;
};

function_t* function_make (ast_t* node,
                           string_t identifier,
                           const signature_type_t* signature,
                           const type_t* return_type)
{
  function_t* f = new function_t;
  f->node = node;
  f->name = identifier;
  f->func_type = new func_type_t (signature, return_type);
  return f;
}

type_t* function_type (const function_t* function)
{
  return function->func_type;
}

const signature_type_t* function_signature (const function_t* function)
{
  return function->func_type->signature ();
}

const type_t* function_return_type (const function_t* function)
{
  return function->func_type->return_type ();
}

void function_set_locals_size (function_t* function,
                             size_t size)
{
  function->locals_size = size;
}

size_t function_get_locals_size (const function_t * function)
{
  return function->locals_size;
}
