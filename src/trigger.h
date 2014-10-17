#ifndef trigger_h
#define trigger_h

#include "types.h"

trigger_t *trigger_make (void);

void trigger_add_field (trigger_t * trigger, field_t * field);

field_t **trigger_begin (const trigger_t * trigger);

field_t **trigger_end (const trigger_t * trigger);

field_t **trigger_next (field_t ** pos);

void trigger_set_mutates_receiver (trigger_t * trigger, bool value);

bool trigger_get_mutates_receiver (const trigger_t * trigger);

#endif /* trigger_h */
