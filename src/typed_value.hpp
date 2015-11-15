#ifndef typed_value_h
#define typed_value_h

#include "types.hpp"
#include "Location.hpp"
#include "Type.hpp"
#include <ostream>
#include "value.hpp"
#include <vector>

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
    TYPE,
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
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , component_state (false)
    , has_offset (false)
  { }

  template <typename T>
  typed_value_t (const T* t,
                 typename T::ValueType v)
    : type (t)
    , kind (VALUE)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , component_state (false)
    , value (t, v)
    , has_offset (false)
  {
    fix ();
  }

  typed_value_t (const Type::NamedType* type,
                 size_t e)
    : type (type)
    , kind (VALUE)
    , intrinsic_mutability (IMMUTABLE)
    , dereference_mutability (IMMUTABLE)
    , component_state (false)
    , value (type, e)
    , has_offset (false)
  {
    fix ();
  }

  explicit typed_value_t (Callable* c);

  explicit typed_value_t (Template* t);

  explicit typed_value_t (reaction_t* r);

  explicit typed_value_t (const Type::Type* t)
    : type (t)
    , kind (TYPE)
    , intrinsic_mutability (FOREIGN)
    , dereference_mutability (FOREIGN)
    , component_state (false)
    , has_offset (false)
  { }

  const Type::Type* type;
  Kind kind;
  Mutability intrinsic_mutability;
  Mutability dereference_mutability;
  // True if value or reference contains an address which refers to component state.
  bool component_state;
  value_t value;
  value_t low_value;
  value_t high_value;
  bool has_offset;
  ptrdiff_t offset;

  static typed_value_t make_value (const Type::Type* type, Mutability intrinsic, Mutability dereference, bool component_state);
  static typed_value_t make_range (const typed_value_t& low, const typed_value_t& high, Mutability intrinsic, Mutability dereference);
  static typed_value_t make_ref (const Type::Type* type, Mutability intrinsic, Mutability dereference, bool component_state);
  static typed_value_t make_ref (typed_value_t tv);

  static typed_value_t nil ();

  static typed_value_t implicit_dereference (typed_value_t tv);
  static typed_value_t dereference (typed_value_t tv);
  static typed_value_t address_of (typed_value_t tv);
  static typed_value_t select (typed_value_t tv, const std::string& identifier);
  static typed_value_t index (const Location& location, typed_value_t tv, typed_value_t index);
  static typed_value_t slice (const Location& location, typed_value_t tv, typed_value_t low, typed_value_t high);
  static typed_value_t merge (typed_value_t tv);
  static typed_value_t move (typed_value_t tv);
  static typed_value_t copy (const Location& location, const typed_value_t tv);
  static typed_value_t copy_exec (const typed_value_t tv);
  static typed_value_t change (const Location& location, typed_value_t tv);

  // Check if value can be represented by the given type.
  bool RepresentableBy (const Type::Type* type) const;
  bool AssignableTo (const Type::Type* type) const;

  // Does not check for errors.
  typed_value_t Convert (const Location& location, const Type::Type* type) const;

  static typed_value_t Equal (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t NotEqual (const Location& location, const typed_value_t& left, const typed_value_t& right);

  static typed_value_t LessThan (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t LessEqual (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t MoreThan (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t MoreEqual (const Location& location, const typed_value_t& left, const typed_value_t& right);

  typed_value_t LogicNot (const Location& location) const;
  static typed_value_t LogicOr (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t LogicAnd (const Location& location, const typed_value_t& left, const typed_value_t& right);

  typed_value_t Negate (const Location& location) const;
  static typed_value_t Multiply (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t Divide (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t Modulus (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t Add (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t Subtract (const Location& location, const typed_value_t& left, const typed_value_t& right);

  static typed_value_t LeftShift (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t RightShift (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t BitAnd (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t BitAndNot (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t BitOr (const Location& location, const typed_value_t& left, const typed_value_t& right);
  static typed_value_t BitXor (const Location& location, const typed_value_t& left, const typed_value_t& right);

  void ArrayDimension (const Location& locatin) const;

  bool IsError() const
  {
    return type == NULL;
  }
  bool isReference () const
  {
    return kind == REFERENCE;
  }
  bool IsValue () const
  {
    return kind == VALUE;
  }

  bool is_foreign_safe () const
  {
    return !(type_contains_pointer (type) &&
             dereference_mutability != FOREIGN);
  }

  void zero ();

  Type::Int::ValueType integral_value () const;

  Type::Int::ValueType low_integral_value () const;

  Type::Int::ValueType high_integral_value () const;

  Type::Slice::ValueType slice_value () const
  {
    return value.slice_value ();
  }

  void RequireValue (const Location loc) const;

  std::ostream& print (std::ostream& o) const;

  void fix ()
  {
    if (type != NULL &&
        (type->UnderlyingType () == Type::StringU::Instance () ||
         type->UnderlyingType () == Type::String::Instance ()))
      {
        dereference_mutability = std::max (IMMUTABLE, dereference_mutability);
      }
  }
};

inline std::ostream&
operator<< (std::ostream& out, const typed_value_t& tv)
{
  return tv.print (out);
}

typedef std::vector<typed_value_t> TypedValueListType;

#endif /* typed_value_h */
