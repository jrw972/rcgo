#include "instance_set.h"
#include "debug.h"
#include "util.h"

struct instance_set_t {
  VECTOR_DECL (instances, instance_set_element_t);
};

instance_set_t* instance_set_make (void)
{
  instance_set_t* set = xmalloc (sizeof (instance_set_t));
  instance_set_element_t e;
  VECTOR_INIT (set->instances, instance_set_element_t, 0, e);
  return set;
}

void instance_set_insert (instance_set_t* set,
                          instance_t* instance,
                          TriggerAction action)
{
  VECTOR_FOREACH (pos, limit, set->instances, instance_set_element_t)
  {

    if (pos->instance == instance)
      {
        // In set.
        if (action > pos->action)
          {
            pos->action = action;
          }
        return;
      }
  }

  instance_set_element_t e = { instance: instance, action: action };
  VECTOR_PUSH (set->instances, instance_set_element_t, e);
}

static int compare_element (const void* x, const void* y)
{
  return (ptrdiff_t)((instance_set_element_t*)x)->instance - (ptrdiff_t)((instance_set_element_t*)y)->instance;
}

void instance_set_sort (instance_set_t* set)
{
  qsort (VECTOR_BEGIN(set->instances), VECTOR_SIZE(set->instances), sizeof (instance_set_element_t), compare_element);
}

instance_set_element_t* instance_set_begin (const instance_set_t* set)
{
  return VECTOR_BEGIN (set->instances);
}

instance_set_element_t* instance_set_end (const instance_set_t* set)
{
  return VECTOR_END (set->instances);
}

instance_set_element_t* instance_set_next (instance_set_element_t* pos)
{
  return VECTOR_NEXT (pos);
}
