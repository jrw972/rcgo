#include "trigger_group.h"
#include "util.h"

struct trigger_group_t
{
  VECTOR_DECL (fields, field_t*);
};

trigger_group_t* trigger_group_make (void)
{
  trigger_group_t* tg = xmalloc (sizeof (trigger_group_t));
  VECTOR_INIT (tg->fields, field_t*, 0, NULL);
  return tg;
}

void trigger_group_add_field (trigger_group_t* group, field_t* field)
{
  VECTOR_PUSH (group->fields, field_t*, field);
}

field_t** trigger_group_begin (const trigger_group_t* trigger)
{
  return VECTOR_BEGIN (trigger->fields);
}

field_t** trigger_group_end (const trigger_group_t* trigger)
{
  return VECTOR_END (trigger->fields);
}

field_t** trigger_group_next (field_t** pos)
{
  return pos + 1;
}
