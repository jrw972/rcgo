#ifndef typed_value_h
#define typed_value_h

#include "untyped_value.h"
#include "types.h"

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
    rtstring_t string_value;
    getter_t* getter_value;
  };
};

bool typed_value_is_undefined (typed_value_t t);

typed_value_t typed_value_make_bool (type_t * type, bool v);

typed_value_t typed_value_make_string (type_t* type, rtstring_t s);

typed_value_t typed_value_make_getter (getter_t* getter);

type_t *typed_value_get_type (typed_value_t t);

typed_value_t typed_value_from_untyped (untyped_value_t u, type_t * t);

#endif /* typed_value_h */
