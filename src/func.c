#include "func.h"
#include "debug.h"
#include "util.h"
#include "type.h"

struct func_t {
  type_t *component_type;	/* Back-pointer to component type. */
  ast_t* node;
  string_t name;
  type_t *func_type;
  size_t locals_size;
};

func_t*
func_make (type_t * type,
             ast_t* node,
             string_t identifier, type_t * signature, type_t* return_type)
{
  func_t* g = xmalloc (sizeof (func_t));
  g->component_type = type;
  g->node = node;
  g->name = identifier;
  g->func_type = type_make_func (signature, return_type);
  return g;
}

type_t*
func_component_type (const func_t* func)
{
  return func->component_type;
}

ast_t*
func_node (const func_t* func)
{
  return func->node;
}

type_t*
func_type (const func_t* func)
{
  return func->func_type;
}

const type_t*
func_signature (const func_t* func)
{
  return type_func_signature (func->func_type);
}

type_t*
func_return_type (const func_t* func)
{
  return type_func_return_type (func->func_type);
}

string_t
func_name (const func_t* func)
{
  return func->name;
}

void func_set_locals_size (func_t* func,
                             size_t size)
{
  func->locals_size = size;
}

size_t func_get_locals_size (const func_t * func)
{
  return func->locals_size;
}
