#ifndef untyped_value_h
#define untyped_value_h

#include <stdbool.h>
#include <stddef.h>
#include "rtstring.h"

/*
  An untyped value represents an actual value not described by the type system.
*/
typedef struct untyped_value_t untyped_value_t;

typedef enum
{
  UntypedUndefined, UntypedBool, UntypedString
} UntypedValueKind;

struct untyped_value_t
{
  UntypedValueKind kind;
  union
  {
    bool bool_value;
    rtstring_t string_value;
  };
};

const char *untyped_value_to_string (untyped_value_t u);

untyped_value_t untyped_value_make_bool (bool b);

untyped_value_t untyped_value_logic_not (untyped_value_t u);

untyped_value_t
untyped_value_logic_and (untyped_value_t x, untyped_value_t y);

untyped_value_t untyped_value_logic_or (untyped_value_t x, untyped_value_t y);

untyped_value_t untyped_value_make_string (rtstring_t s);

#endif /* untyped_value_h */
