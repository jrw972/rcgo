#ifndef typed_value_h
#define typed_value_h

#include "types.hpp"
#include "rtstring.hpp"
#include "strtab.hpp"
#include <ostream>
#include "function.hpp"
#include "method.hpp"
#include "action.hpp"

/*
  A typed value represents an actual value described by the type system.
 */
struct typed_value_t
{
  enum Kind
    {
      VALUE,
      REFERENCE,
    };

  enum ReferenceKind
    {
      MUTABLE,
      IMMUTABLE,
      FOREIGN,
      CONSTANT,
    };

  typed_value_t ()
    : type (NULL)
    , has_value (false)
    , has_offset (false)
  { }

  typed_value_t (bool v)
    : type (bool_type_t::instance ())
    , kind (VALUE)
    , reference_kind (IMMUTABLE)
    , has_value (true)
    , bool_value (v)
  { }

  typed_value_t (int64_t v)
    : type (int_type_t::instance ())
    , kind (VALUE)
    , reference_kind (IMMUTABLE)
    , has_value (true)
    , int_value (v)
  { }

  typed_value_t (uint64_t v)
    : type (uint_type_t::instance ())
    , kind (VALUE)
    , reference_kind (IMMUTABLE)
    , has_value (true)
    , uint_value (v)
  { }

  typed_value_t (rtstring_t s)
    : type (string_type_t::instance ())
    , kind (VALUE)
    , reference_kind (IMMUTABLE)
    , has_value (true)
    , string_value (s)
  { }

  typed_value_t (function_t* f)
    : type (f->func_type)
    , kind (VALUE)
    , reference_kind (IMMUTABLE)
    , has_value (true)
    , function_value (f)
  { }

  typed_value_t (method_t* m)
    : type (m->method_type)
    , kind (VALUE)
    , reference_kind (IMMUTABLE)
    , has_value (true)
    , method_value (m)
  { }

  typed_value_t (reaction_t* r)
    : type (r->reaction_type ())
    , kind (VALUE)
    , reference_kind (IMMUTABLE)
    , has_value (true)
    , reaction_value (r)
  { }

  const type_t *type;
  Kind kind;
  ReferenceKind reference_kind;
  bool has_value;
  bool has_offset;
  ptrdiff_t offset;

  union {
    bool bool_value;
    int64_t int_value;
    uint64_t uint_value;
    rtstring_t string_value; /* For strings and untyped strings. */
    method_t* method_value;
    function_t* function_value;
    reaction_t* reaction_value;
  };

  static typed_value_t make_value (const type_t* type, ReferenceKind k);
  static typed_value_t make_ref (const type_t* type, ReferenceKind k);
  static typed_value_t make_ref (typed_value_t tv);

  static typed_value_t nil ();

  static typed_value_t implicit_dereference (typed_value_t tv);
  static typed_value_t dereference (typed_value_t tv);
  static typed_value_t address_of (typed_value_t tv);
  static typed_value_t select (typed_value_t tv, string_t identifier);
  static typed_value_t index (typed_value_t tv, typed_value_t index);
  static typed_value_t logic_not (typed_value_t tv);
  static typed_value_t logic_or (typed_value_t left, typed_value_t right);
  static typed_value_t logic_and (typed_value_t left, typed_value_t right);
  static typed_value_t equal (typed_value_t left, typed_value_t right);
  static typed_value_t not_equal (typed_value_t left, typed_value_t right);
  static typed_value_t merge (typed_value_t tv);
  static typed_value_t move (typed_value_t tv);
  static typed_value_t add (typed_value_t left, typed_value_t right);
};

std::ostream&
operator<< (std::ostream& out, const typed_value_t& tv);

#endif /* typed_value_h */
