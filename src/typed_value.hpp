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
  // The result of an expression is either a reference or a value.
  enum Kind
    {
      REFERENCE,
      VALUE,
    };

  // A reference refers to a location in a constant area, on the stack, or in the heap.
  enum Region
    {
      CONSTANT,
      STACK,
      HEAP,
    };

  /*
    A reference has:
    - a type
    - a region
    - an intrinsic mutability
    - a dereference mutability
    - an actual value (possibly)

    A value has:
    - a type
    - an intrinsic mutability
    - a dereference mutability
    - an actual value (possibly)

    The four operations that contributed to the "const correctness" of
    the language are implicit dereference, dereference (@), address of
    (&), and assignment (=).

    Implicit Dereference
    --------------------

    This operation converts a reference to a value.  The type,
    intrinsic mutability, dereference mutability, and actual value of
    the reference are passed on to the value.

                           | REFERENCE | VALUE |
    -----------------------+-----------+-------+
      Intrinsic Mutability |         A |     A |
    Dereference Mutability |         B |     B |


    Dereference (@)
    ---------------

    This operation converts a value to a reference and must be applied
    to a pointer.  The region of the reference is assumed to be the
    heap.  (This is points-to analysis which is undecidable in the
    general case.)  The intrinsic mutability and dereference
    mutability of the reference is determined by dereference
    mutability of the value.  The only actual values for a value with
    pointer type is nil but this could be improved.  Similarly, it is
    not defined how the actual value passes to the reference.

                           | VALUE | REFERENCE |
    -----------------------+-------+-----------+
      Intrinsic Mutability |     A |         B |
    Dereference Mutability |     B |         B |


    Address Of (&)
    --------------

    This operation converts a reference to a pointer value.  The type
    and mutability of the pointer is determined by the mutability of
    the reference.

                           | REFERENCE | VALUE |
    -----------------------+-----------+-------+
      Intrinsic Mutability |         A |     A |
    Dereference Mutability |         B |     B |

    Assignment (=)
    --------------

    Assignment must check if a value can be copied to the location
    indicated by a reference.  The two simple checks are (1) the types
    must be equivalent and (2) the reference should be mutable.  (All
    references to constant data should be immutable.)

    If the type being assigned does not contain a pointer, then no
    further checks are necessary as new references cannot be generated
    from the value.  However, if the type being assigned does contain
    a pointer, then we must be careful to ensure that any generated
    reference has the correct mutability.  To this end, we check the
    dereference mutability of the reference with the intrinsic
    mutability of the value.

                  MUTABLE | IMMUTABLE | FOREIGN (VALUE)
                +---------+-----------+--------
    MUTABLE     |   OK    |     NO    |    NO
    IMMUTABLE   |   OK    |     OK    |    NO
    FOREIGN     |   OK    |     OK    |    OK
    (REFERENCE)

    If the value has FOREIGN intrinsic mutability then we require that
    the reference refer to the STACK.

    (This check is currently checked externally.)

    Port Calls and Getters
    ----------------------

    Getters (component methods) can only be defined on immutable
    components and can only be called in the immutable phase.  There
    is no restriction on the values going in but the return values
    must have FOREIGN intrinsic mutability.  This allows a component
    to return pointers but prevents them from being saved in the heap.

    Parameters to ports and reactions must have FOREIGN dereference
    mutability if the associated type contains a pointer.
   */

  typed_value_t ()
    : type (NULL)
    , has_value (false)
    , has_offset (false)
  { }

  explicit typed_value_t (bool v)
    : type (bool_type_t::instance ())
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , has_value (true)
    , bool_value (v)
  { }

  explicit typed_value_t (int64_t v)
    : type (int_type_t::instance ())
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , has_value (true)
    , int_value (v)
  { }

  explicit typed_value_t (uint64_t v)
    : type (uint_type_t::instance ())
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , has_value (true)
    , uint_value (v)
  { }

  explicit typed_value_t (rtstring_t s)
    : type (string_type_t::instance ())
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , has_value (true)
    , string_value (s)
  { }

  explicit typed_value_t (function_t* f)
    : type (f->function_type)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , has_value (true)
    , function_value (f)
  { }

  explicit typed_value_t (method_t* m)
    : type (m->method_type)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , has_value (true)
    , method_value (m)
  { }

  explicit typed_value_t (reaction_t* r)
    : type (r->reaction_type ())
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , has_value (true)
    , reaction_value (r)
  { }

  const type_t *type;
  Kind kind;
  Region region;
  Mutability intrinsic_mutability;
  Mutability dereference_mutability;
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

  static typed_value_t make_value (const type_t* type, Region region, Mutability intrinsic, Mutability dereference);
  static typed_value_t make_ref (const type_t* type, Region region, Mutability intrinsic, Mutability dereference);
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

  std::ostream& print (std::ostream& o) const;
};

inline std::ostream&
operator<< (std::ostream& out, const typed_value_t& tv)
{
  return tv.print (out);
}

#endif /* typed_value_h */
