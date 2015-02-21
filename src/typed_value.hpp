#ifndef typed_value_h
#define typed_value_h

#include "types.hpp"
#include "rtstring.hpp"
#include "strtab.hpp"
#include <ostream>

enum TypedValueKind
  {
    TV_MUTABLE,
    TV_IMMUTABLE,
    TV_FOREIGN,
    TV_CONSTANT,
  };

/*
  A typed value represents an actual value described by the type system.
 */
struct typed_value_t
{
  typed_value_t () : type (NULL), kind (TV_CONSTANT) { }

  typed_value_t (const type_t* t, TypedValueKind k) : type (t), kind (k) { }

  const type_t *type;
  TypedValueKind kind;
  bool has_value;
  bool bool_value;
  int64_t int_value;
  uint64_t uint_value;
  rtstring_t string_value; /* For strings and untyped strings. */
  method_t* method_value;
  function_t* function_value;
  reaction_t* reaction_value;
};

std::ostream&
operator<< (std::ostream& out, const typed_value_t& tv);

typed_value_t typed_value_make (const type_t * type, TypedValueKind kind);

typed_value_t typed_value_make_bool (const type_t * type, bool v);

typed_value_t typed_value_make_int (const type_t * type, int64_t u);

typed_value_t typed_value_make_uint (const type_t * type, uint64_t u);

typed_value_t typed_value_make_string (const type_t* type, rtstring_t s);

typed_value_t typed_value_make_method (method_t* method);

typed_value_t typed_value_make_function (function_t* function);

typed_value_t typed_value_make_reaction (reaction_t* reaction);

typed_value_t typed_value_make_nil (void);

int64_t typed_value_to_index (typed_value_t tv);

typed_value_t typed_value_dereference (const typed_value_t& in);

typed_value_t typed_value_address_of (const typed_value_t& in);

typed_value_t typed_value_binary_arithmetic (const typed_value_t& left, const typed_value_t& right);

#endif /* typed_value_h */
