#ifndef typed_value_h
#define typed_value_h

#include "types.h"
#include "rtstring.h"
#include "strtab.h"

/*
  A typed value represents an actual value described by the type system.
 */
typedef struct typed_value_t typed_value_t;
struct typed_value_t
{
  type_t *type;
  bool has_value;
  struct {
    bool bool_value; /* For bools and untyped bools. */
    uint64_t uint_value;
    rtstring_t string_value;
    method_t* method_value;
    int64_t integer_value;
    void* pointer_value;
  };
};

typed_value_t typed_value_make (type_t * type);

typed_value_t typed_value_make_bool (type_t * type, bool v);

typed_value_t typed_value_make_uint (type_t * type, uint64_t u);

typed_value_t typed_value_make_string (type_t* type, rtstring_t s);

typed_value_t typed_value_make_integer (int64_t i);

typed_value_t typed_value_make_method (method_t* method);

typed_value_t typed_value_make_nil (void);

/* Can we apply logical operators to this typed value? */
bool typed_value_is_boolean (typed_value_t tv);

typed_value_t typed_value_logic_not (typed_value_t tv);

/* Can we dereference this typed value? */
bool typed_value_can_dereference (typed_value_t tv);

typed_value_t typed_value_dereference (typed_value_t tv);

/* Can we select from ths typed value? */
bool typed_value_can_select (typed_value_t tv, string_t name);

typed_value_t typed_value_select (typed_value_t tv, string_t name);

/* Is this an untyped value? */
bool typed_value_is_untyped (typed_value_t tv);

/* Can the untyped value be represented by this type? */
bool typed_value_can_convert (typed_value_t tv, type_t* type);

typed_value_t typed_value_convert (typed_value_t tv, type_t* type);

/* Can a value of source be assigned to a location of target? */
bool typed_value_convertible (typed_value_t target, typed_value_t source);

/* Can we apply arithmetic operators to this typed value? */
bool typed_value_is_arithmetic (typed_value_t tv);

/* Can we call the type? */
bool typed_value_callable (typed_value_t tv);

/* Can we copy values of this type (for call or assignment)? */
bool typed_value_copyable (typed_value_t tv);

/* Can we assign to this type? */
bool typed_value_can_assign (typed_value_t tv);

/* Can we move values of this type? */
bool typed_value_is_moveable (typed_value_t tv);

typed_value_t typed_value_move (typed_value_t tv);

/* Can we merge values of this type? */
bool typed_value_is_mergeable (typed_value_t tv);

typed_value_t typed_value_merge (typed_value_t tv);

/* Can we change value of this type? */
bool typed_value_is_changeable (typed_value_t tv);

#endif /* typed_value_h */
