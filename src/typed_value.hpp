#ifndef typed_value_h
#define typed_value_h

#include "types.hpp"
#include "Location.hpp"
#include "type.hpp"
#include <ostream>
#include "value.hpp"

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
    - the value of the data to which this reference refers (possibly)

    A value has:
    - a type
    - an intrinsic mutability
    - a dereference mutability
    - an actual value (possibly)

    The four operations that contributed to the "const correctness" of
    the language are implicit dereference, dereference (*), address of
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


    Dereference (*)
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

    If the value has FOREIGN dereference mutability then we require that
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
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , has_offset (false)
  { }

  typed_value_t (const bool_type_t* t,
                 bool_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const int_type_t* t,
                 int_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const int8_type_t* t,
                 int8_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const int16_type_t* t,
                 int16_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const int32_type_t* t,
                 int32_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const int64_type_t* t,
                 int64_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const int128_type_t* t,
                 int128_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const uint_type_t* t,
                 uint_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const uint8_type_t* t,
                 uint8_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const uint16_type_t* t,
                 uint16_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const uint32_type_t* t,
                 uint32_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const uint64_type_t* t,
                 uint64_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const uint128_type_t* t,
                 uint128_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const named_type_t* type,
                 size_t e)
    : type (type)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (type, e)
    , has_offset (false)
  { }

  typed_value_t (const float32_type_t* t,
                 float32_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  typed_value_t (const float64_type_t* t,
                 float64_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  explicit typed_value_t (Callable* c);

  explicit typed_value_t (reaction_t* r);

  typed_value_t (const slice_type_t* t,
                 slice_type_t::ValueType v)
    : type (t)
    , kind (VALUE)
    , region (CONSTANT)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , value (t, v)
    , has_offset (false)
  { }

  const type_t *type;
  Kind kind;
  Region region;
  Mutability intrinsic_mutability;
  Mutability dereference_mutability;
  value_t value;
  value_t low_value;
  value_t high_value;
  bool has_offset;
  ptrdiff_t offset;

  static typed_value_t make_value (const type_t* type, Region region, Mutability intrinsic, Mutability dereference);
  static typed_value_t make_range (const typed_value_t& low, const typed_value_t& high, Region region, Mutability intrinsic, Mutability dereference);
  static typed_value_t make_ref (const type_t* type, Region region, Mutability intrinsic, Mutability dereference);
  static typed_value_t make_ref (typed_value_t tv);

  static typed_value_t nil ();

  static typed_value_t implicit_dereference (typed_value_t tv);
  static typed_value_t dereference (typed_value_t tv);
  static typed_value_t address_of (typed_value_t tv);
  static typed_value_t select (typed_value_t tv, const std::string& identifier);
  static typed_value_t index (const Location& location, typed_value_t tv, typed_value_t index);
  static typed_value_t slice (const Location& location, typed_value_t tv, typed_value_t low, typed_value_t high);

  static typed_value_t logic_not (typed_value_t tv);
  static typed_value_t merge (typed_value_t tv);
  static typed_value_t move (typed_value_t tv);
  static typed_value_t binary (const Location& location, BinaryArithmetic arithmetic, typed_value_t left, typed_value_t right);
  static typed_value_t cast (const Location& location, const type_t* type, const typed_value_t tv);
  static typed_value_t cast_exec (const type_t* type, const typed_value_t tv);
  static typed_value_t copy (const Location& location, const typed_value_t tv);
  static typed_value_t copy_exec (const typed_value_t tv);
  static typed_value_t change (const Location& location, typed_value_t tv);

  bool isError() const
  {
    return type == NULL;
  }
  bool isReference () const
  {
    return kind == REFERENCE;
  }
  bool isValue () const
  {
    return kind == VALUE;
  }

  bool is_foreign_safe () const
  {
    return !(type_contains_pointer (type) &&
             dereference_mutability != FOREIGN);
  }

  void zero ();

  int_type_t::ValueType integral_value () const
  {
    Location loc;
    return cast (loc, int_type_t::instance (), *this).value.ref (*int_type_t::instance ());
  }

  int_type_t::ValueType low_integral_value () const
  {
    Location loc;
    return cast (loc, int_type_t::instance (), *this).low_value.ref (*int_type_t::instance ());
  }

  int_type_t::ValueType high_integral_value () const
  {
    Location loc;
    return cast (loc, int_type_t::instance (), *this).high_value.ref (*int_type_t::instance ());
  }

  slice_type_t::ValueType slice_value () const
  {
    return value.slice_value ();
  }

  std::ostream& print (std::ostream& o) const;
};

inline std::ostream&
operator<< (std::ostream& out, const typed_value_t& tv)
{
  return tv.print (out);
}



#endif /* typed_value_h */
