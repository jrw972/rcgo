#ifndef instance_trigger_set_h
#define instance_set_h

#include "types.h"

instance_trigger_set_t *instance_trigger_set_make (void);

bool instance_trigger_set_contains (instance_trigger_set_t * set,
                                    instance_t * instance, trigger_t * trigger);

void instance_trigger_set_push (instance_trigger_set_t * set,
                                instance_t * instance, trigger_t * trigger);

void instance_trigger_set_pop (instance_trigger_set_t * set);

bool instance_trigger_set_empty (const instance_trigger_set_t * set);

#endif /* instance_trigger_set_h */
