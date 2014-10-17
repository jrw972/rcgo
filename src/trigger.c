#include "trigger.h"
#include "util.h"

struct trigger_t
{
  VECTOR_DECL (fields, field_t *);
  bool mutates_receiver;
};

trigger_t *
trigger_make (void)
{
  trigger_t *tg = xmalloc (sizeof (trigger_t));
  VECTOR_INIT (tg->fields, field_t *, 0, NULL);
  return tg;
}

void
trigger_add_field (trigger_t * group, field_t * field)
{
  VECTOR_PUSH (group->fields, field_t *, field);
}

field_t **
trigger_begin (const trigger_t * trigger)
{
  return VECTOR_BEGIN (trigger->fields);
}

field_t **
trigger_end (const trigger_t * trigger)
{
  return VECTOR_END (trigger->fields);
}

field_t **
trigger_next (field_t ** pos)
{
  return pos + 1;
}

void
trigger_set_mutates_receiver (trigger_t * trigger, bool value)
{
  trigger->mutates_receiver = value;
}

bool
trigger_get_mutates_receiver (const trigger_t * trigger)
{
  return trigger->mutates_receiver;
}
