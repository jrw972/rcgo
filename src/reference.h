#ifndef reference_h
#define reference_h

#include "abstract_value.h"

/* A reference describes the location of a value. */
typedef struct reference_t reference_t;
struct reference_t
{
  abstract_value_t value;
};

const char* reference_to_string (reference_t r);

reference_t reference_make (abstract_value_t value);

abstract_value_t reference_value (reference_t r);

bool reference_assignable (reference_t target, abstract_value_t source);

reference_t reference_select (reference_t r, string_t identifier);

bool reference_is_undefined (reference_t r);

bool reference_is_bindable (reference_t output, reference_t input);

action_t* reference_get_reaction (reference_t r);

select_result_t reference_selected_field (reference_t r);

#endif /* reference_h */
