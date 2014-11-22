#include "parameter.h"
#include "debug.h"
#include "util.h"

struct parameter_t
{
  string_t name;
  type_t *type;
  bool is_receiver;
};

parameter_t *
parameter_make (string_t name, type_t * type, bool is_receiver)
{
  parameter_t *p = xmalloc (sizeof (parameter_t));
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

type_t *
parameter_type (const parameter_t * parameter)
{
  return parameter->type;
}

bool parameter_is_receiver (const parameter_t * parameter)
{
  return parameter->is_receiver;
}
