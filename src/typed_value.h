#ifndef typed_value_h
#define typed_value_h

#include <stdbool.h>
#include "type.h"
#include "untyped_value.h"

/*
  A typed value represents an actual value described by the type system.
 */
typedef struct typed_value_t typed_value_t;
struct typed_value_t
{
  type_t *type;
  union
  {
    bool bool_value;
  };
};

bool typed_value_is_undefined (typed_value_t t);

typed_value_t typed_value_make_bool (type_t * type, bool v);

type_t *typed_value_get_type (typed_value_t t);

typed_value_t typed_value_from_untyped (untyped_value_t u, const type_t * t);

#endif /* typed_value_h */
