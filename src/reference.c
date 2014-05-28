#include "reference.h"
#include "debug.h"

void
reference_print (reference_t r)
{
  printf ("&");
  abstract_value_print (r.value);
}

reference_t
reference_make (abstract_value_t value,
                bool mutable)
{
  reference_t retval = { value:value, mutable:mutable };
  return retval;
}

abstract_value_t reference_value (reference_t r)
{
  return r.value;
}

bool reference_mutable (reference_t r)
{
  return r.mutable;
}

bool
reference_assignable (reference_t target, abstract_value_t source)
{
  return abstract_value_assignable (target.value, source);
}

reference_t reference_select (reference_t r, string_t identifier)
{
  return reference_make (abstract_value_select (r.value, identifier), r.mutable);
}
