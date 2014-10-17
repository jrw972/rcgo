#include "binding.h"
#include "util.h"

struct binding_t
{
  VECTOR_DECL (outputs, field_t *);
  VECTOR_DECL (inputs, field_t *);
  action_t *reaction;
};

binding_t *
binding_make (void)
{
  binding_t *binding = xmalloc (sizeof (binding_t));
  VECTOR_INIT (binding->outputs, field_t *, 0, NULL);
  VECTOR_INIT (binding->inputs, field_t *, 0, NULL);
  return binding;
}

void
binding_add_output (binding_t * binding, field_t * field)
{
  VECTOR_PUSH (binding->outputs, field_t *, field);
}

void
binding_add_input (binding_t * binding, field_t * field)
{
  VECTOR_PUSH (binding->inputs, field_t *, field);
}

void
binding_set_reaction (binding_t * binding, action_t * reaction)
{
  binding->reaction = reaction;
}

action_t *
binding_get_reaction (const binding_t * binding)
{
  return binding->reaction;
}

field_t **
binding_output_begin (const binding_t * binding)
{
  return VECTOR_BEGIN (binding->outputs);
}

field_t **
binding_output_end (const binding_t * binding)
{
  return VECTOR_END (binding->outputs);
}

field_t **
binding_output_next (field_t ** pos)
{
  return pos + 1;
}

field_t **
binding_input_begin (const binding_t * binding)
{
  return VECTOR_BEGIN (binding->inputs);
}

field_t **
binding_input_end (const binding_t * binding)
{
  return VECTOR_END (binding->inputs);
}

field_t **
binding_input_next (field_t ** pos)
{
  return pos + 1;
}
