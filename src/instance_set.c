#include "instance_set.h"
#include "debug.h"
#include "util.h"

struct instance_set_t {
  VECTOR_DECL (instances, instance_t*);
};

instance_set_t* instance_set_make (void)
{
  instance_set_t* set = xmalloc (sizeof (instance_set_t));
  VECTOR_INIT (set->instances, size_t, 0, 0);
  return set;
}

bool instance_set_contains (instance_set_t* set,
                            instance_t* instance)
{
  VECTOR_FOREACH (pos, limit, set->instances, instance_t*)
    {
      if (*pos == instance)
        {
          return true;
        }
    }
  return false;
}

void instance_set_insert (instance_set_t* set,
                          instance_t* instance)
{
  VECTOR_PUSH (set->instances, size_t, instance);
}

void instance_set_clear (instance_set_t* set)
{
  set->instances_size = 0;
}
