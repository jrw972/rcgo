#include "typed_value.h"
#include <stdlib.h>
#include "debug.h"
#include "symtab.h"

bool
typed_value_is_undefined (typed_value_t t)
{
  return t.type == NULL;
}

typed_value_t
typed_value_make_bool (type_t * type, bool v)
{
typed_value_t retval = { type: type, bool_value:v };
  return retval;
}

type_t *
typed_value_get_type (typed_value_t t)
{
  return t.type;
}

typed_value_t
typed_value_from_untyped (untyped_value_t u, const type_t * t)
{
  unimplemented;
}
