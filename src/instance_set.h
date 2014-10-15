#ifndef instance_set_h
#define instance_set_h

#include "types.h"

instance_set_t* instance_set_make (void);

bool instance_set_contains (instance_set_t* set,
                            instance_t* instance,
                            trigger_t* trigger);

void instance_set_push (instance_set_t* set,
                        instance_t* instance,
                        trigger_t* trigger);

void instance_set_pop (instance_set_t* set);

bool instance_set_empty (const instance_set_t* set);

#endif /* instance_set_h */
