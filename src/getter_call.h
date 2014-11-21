#ifndef getter_call_h
#define getter_call_h

#include "types.h"

getter_call_t*
getter_call_make (void);

void
getter_call_add_field (getter_call_t* gc,
                       field_t* field);

#endif /* getter_call_h */
