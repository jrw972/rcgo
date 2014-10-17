#include "instance_set.h"
#include "debug.h"
#include "util.h"
#include "trigger.h"

typedef struct
{
  instance_t *instance;
  trigger_t *trigger;
} element_t;

struct instance_set_t
{
  VECTOR_DECL (instances, element_t);
};

instance_set_t *
instance_set_make (void)
{
  instance_set_t *set = xmalloc (sizeof (instance_set_t));
  element_t e;
  VECTOR_INIT (set->instances, element_t, 0, e);
  return set;
}

bool
instance_set_contains (instance_set_t * set,
		       instance_t * instance, trigger_t * trigger)
{
  VECTOR_FOREACH (pos, limit, set->instances, element_t)
  {
    if (pos->instance == instance && pos->trigger == trigger)
      {
	// Same instance and same trigger implies a recursive structure.
	return true;
      }
    if (pos->instance == instance &&
	trigger_get_mutates_receiver (pos->trigger) &&
	trigger_get_mutates_receiver (trigger))
      {
	// Two different triggers are tryingt to mutate the state of the same instance.
	return true;
      }
  }
  return false;
}

void
instance_set_push (instance_set_t * set,
		   instance_t * instance, trigger_t * trigger)
{
element_t e = { instance: instance, trigger:trigger };
  VECTOR_PUSH (set->instances, element_t, e);
}

void
instance_set_pop (instance_set_t * set)
{
  VECTOR_POP (set->instances);
}

bool
instance_set_empty (const instance_set_t * set)
{
  return VECTOR_EMPTY (set->instances);
}
