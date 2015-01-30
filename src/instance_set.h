#ifndef instance_set_h
#define instance_set_h

#include "types.h"

typedef struct {
  instance_t* instance;
  TriggerAction action;
} instance_set_element_t;

instance_set_t* instance_set_make (void);

void instance_set_insert (instance_set_t* set,
                          instance_t* instance,
                          TriggerAction action);

void instance_set_sort (instance_set_t* set);

instance_set_element_t* instance_set_begin (const instance_set_t* set);

instance_set_element_t* instance_set_end (const instance_set_t* set);

instance_set_element_t* instance_set_next (instance_set_element_t* pos);

#endif /* instance_set_h */
