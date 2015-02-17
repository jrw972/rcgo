#include "instance_trigger_set.hpp"
#include "debug.hpp"
#include "util.hpp"
#include "trigger.hpp"
#include <vector>

typedef struct
{
  instance_t *instance;
  trigger_t *trigger;
} element_t;

struct instance_trigger_set_t
{
  std::vector<element_t> instances;
};

instance_trigger_set_t *
instance_trigger_set_make (void)
{
  instance_trigger_set_t *set = new instance_trigger_set_t;
  return set;
}

bool
instance_trigger_set_contains (instance_trigger_set_t * set,
                               instance_t * instance, trigger_t * trigger)
{
  for (std::vector<element_t>::const_iterator pos = set->instances.begin (),
         limit = set->instances.end ();
       pos != limit;
       ++pos)
  {
    if (pos->instance == instance && pos->trigger == trigger)
      {
	// Same instance and same trigger implies a recursive structure.
	return true;
      }
    if (pos->instance == instance &&
	pos->trigger->action == TRIGGER_WRITE &&
	trigger->action == TRIGGER_WRITE)
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
  set->instances.push_back (e);
}

void
instance_trigger_set_pop (instance_trigger_set_t * set)
{
  set->instances.pop_back ();
}

bool
instance_trigger_set_empty (const instance_trigger_set_t * set)
{
  return set->instances.empty ();
}
