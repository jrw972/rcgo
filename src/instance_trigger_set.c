#include "instance_trigger_set.h"
#include "debug.h"
#include "util.h"
#include "trigger.h"

typedef struct
{
  instance_t *instance;
  trigger_t *trigger;
} element_t;

struct instance_trigger_set_t
{
  VECTOR_DECL (instances, element_t);
};

instance_trigger_set_t *
instance_trigger_set_make (void)
{
  instance_trigger_set_t *set = xmalloc (sizeof (instance_trigger_set_t));
  element_t e;
  VECTOR_INIT (set->instances, element_t, 0, e);
  return set;
}

bool
instance_trigger_set_contains (instance_trigger_set_t * set,
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
	trigger_get_action (pos->trigger) == TRIGGER_WRITE &&
	trigger_get_action (trigger) == TRIGGER_WRITE)
      {
	// Two different triggers are trying to mutate the state of the same instance.
	return true;
      }
  }
  return false;
}

void
instance_trigger_set_push (instance_trigger_set_t * set,
                           instance_t * instance, trigger_t * trigger)
{
element_t e = { instance: instance, trigger:trigger };
  VECTOR_PUSH (set->instances, element_t, e);
}

void
instance_trigger_set_pop (instance_trigger_set_t * set)
{
  VECTOR_POP (set->instances);
}

bool
instance_trigger_set_empty (const instance_trigger_set_t * set)
{
  return VECTOR_EMPTY (set->instances);
}
