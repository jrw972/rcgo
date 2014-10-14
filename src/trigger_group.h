#ifndef trigger_group_h
#define trigger_group_h

#include "types.h"

trigger_group_t* trigger_group_make (void);

void trigger_group_add_field (trigger_group_t* group, field_t* field);

field_t** trigger_group_begin (const trigger_group_t* trigger);

field_t** trigger_group_end (const trigger_group_t* trigger);

field_t** trigger_group_next (field_t** pos);

#endif /* trigger_group_h */
