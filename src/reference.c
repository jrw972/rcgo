#include "reference.h"
#include "debug.h"

void
reference_print (reference_t r)
{
  printf ("&");
  abstract_value_print (r.value);
}

reference_t
reference_make (abstract_value_t value)
{
reference_t retval = { value:value };
  return retval;
}

bool
reference_assignable (reference_t target, abstract_value_t source)
{
  return abstract_value_assignable (target.value, source);
}
