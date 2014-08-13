#include "instance_set.h"
#include "debug.h"
#include "util.h"

struct instance_set_t {
  size_t* instances;
  size_t instances_size;
  size_t instances_capacity;
};

instance_set_t* instance_set_make (void)
{
  instance_set_t* set = xmalloc (sizeof (instance_set_t));
  VECTOR_INIT (set->instances, size_t);
  return set;
}

bool instance_set_contains (instance_set_t* set,
                              size_t instance)
{
  VECTOR_FOREACH (pos, limit, set->instances, size_t)
    {
      if (*pos == instance)
        {
          return true;
        }
    }
  return false;
}

void instance_set_insert (instance_set_t* set,
                            size_t instance)
{
  VECTOR_PUSH (set->instances, size_t, instance);
}

void instance_set_clear (instance_set_t* set)
{
  set->instances_size = 0;
}
