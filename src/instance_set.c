#include "instance_set.h"
#include "debug.h"
#include "util.h"

typedef struct {
  instance_t* instance;
  trigger_group_t* trigger;
} element_t;

struct instance_set_t {
  VECTOR_DECL (instances, element_t);
};

instance_set_t* instance_set_make (void)
{
  instance_set_t* set = xmalloc (sizeof (instance_set_t));
  element_t e;
  VECTOR_INIT (set->instances, element_t, 0, e);
  return set;
}

bool instance_set_contains (instance_set_t* set,
                            instance_t* instance,
                            trigger_group_t* trigger)
{
  VECTOR_FOREACH (pos, limit, set->instances, element_t)
    {
      if (pos->instance == instance && pos->trigger == trigger)
        {
          return true;
        }
    }
  return false;
}

void instance_set_push (instance_set_t* set,
                        instance_t* instance,
                        trigger_group_t* trigger)
{
  element_t e = { instance: instance, trigger: trigger };
  VECTOR_PUSH (set->instances, element_t, e);
}

void instance_set_pop (instance_set_t* set)
{
  VECTOR_POP (set->instances);
}

bool instance_set_empty (const instance_set_t* set)
{
  return VECTOR_EMPTY (set->instances);
}
