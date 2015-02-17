#include "parameter.hpp"
#include "debug.hpp"
#include "util.hpp"
#include <string.h>

struct parameter_t
{
  string_t name;
  const type_t *type;
  bool is_receiver;
};

parameter_t *
parameter_make (string_t name, const type_t * type, bool is_receiver)
{
  parameter_t *p = (parameter_t*)malloc (sizeof (parameter_t));
  memset (p, 0, sizeof (parameter_t));
  p->name = name;
  p->type = type;
  p->is_receiver = is_receiver;
  return p;
}

string_t
parameter_name (const parameter_t * parameter)
{
  return parameter->name;
}

const type_t *
parameter_type (const parameter_t * parameter)
{
  return parameter->type;
}

bool parameter_is_receiver (const parameter_t * parameter)
{
  return parameter->is_receiver;
}
