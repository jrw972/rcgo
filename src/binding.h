#ifndef binding_h
#define binding_h

#include "types.h"

binding_t* binding_make (void);

void binding_add_output (binding_t* binding, field_t* field);

void binding_add_input (binding_t* binding, field_t* field);

void binding_set_reaction (binding_t* binding, action_t* reaction);

action_t* binding_get_reaction (const binding_t* binding);

field_t** binding_output_begin (const binding_t* binding);

field_t** binding_output_end (const binding_t* binding);

field_t** binding_output_next (field_t** pos);

field_t** binding_input_begin (const binding_t* binding);

field_t** binding_input_end (const binding_t* binding);

field_t** binding_input_next (field_t** pos);

#endif /* binding_h */
