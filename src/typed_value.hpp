#ifndef typed_value_h
#define typed_value_h

#include "types.hpp"
#include "rtstring.hpp"
#include "strtab.hpp"

/*
  A typed value represents an actual value described by the type system.
 */
struct typed_value_t
{
  const type_t *type;
  bool has_value;
  bool bool_value; /* For bools and untyped bools. */
  uint64_t uint_value;
  rtstring_t string_value; /* For strings and untyped strings. */
  method_t* method_value;
  function_t* function_value;
  int64_t integer_value;
  void* pointer_value;
};

typed_value_t typed_value_make (const type_t * type);

typed_value_t typed_value_make_bool (const type_t * type, bool v);

typed_value_t typed_value_make_uint (type_t * type, uint64_t u);

typed_value_t typed_value_make_string (const type_t* type, rtstring_t s);

typed_value_t typed_value_make_integer (int64_t i);

typed_value_t typed_value_make_method (method_t* method);

typed_value_t typed_value_make_function (function_t* function);

typed_value_t typed_value_make_nil (void);

typed_value_t typed_value_iota (void);

void typed_value_convert (typed_value_t& from, const typed_value_t& to);

#endif /* typed_value_h */
