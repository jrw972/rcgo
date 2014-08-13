#define _GNU_SOURCE
#include "reference.h"
#include "debug.h"

const char* reference_to_string (reference_t r)
{
  char* str;
  asprintf (&str, "&%s", abstract_value_to_string (r.value));
  return str;
}

reference_t
reference_make (abstract_value_t value)
{
reference_t retval = { value: value };
  return retval;
}

abstract_value_t
reference_value (reference_t r)
{
  return r.value;
}

bool
reference_assignable (reference_t target, abstract_value_t source)
{
  return abstract_value_assignable (target.value, source);
}

reference_t
reference_select (reference_t r, string_t identifier)
{
  return reference_make (abstract_value_select (r.value, identifier));
}

bool reference_is_undefined (reference_t r)
{
  return abstract_value_is_undefined (r.value);
}

bool reference_is_bindable (reference_t output, reference_t input)
{
  return abstract_value_is_bindable (output.value, input.value);
}

action_t* reference_get_reaction (reference_t r)
{
  return abstract_value_get_reaction (r.value);
}

select_result_t reference_selected_field (reference_t r)
{
  return abstract_value_selected_field (r.value);
}
