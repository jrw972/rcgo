#ifndef instance_set_h
#define instance_set_h

#include "types.h"

instance_set_t* instance_set_make (void);

bool instance_set_contains (instance_set_t* set,
                            instance_t* instance);

void instance_set_insert (instance_set_t* set,
                          instance_t* instance);

void instance_set_clear (instance_set_t* set);

#endif /* instance_set_h */
