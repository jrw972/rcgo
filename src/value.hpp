#ifndef rc_src_value_hpp
#define rc_src_value_hpp

#include "types.hpp"
#include "type.hpp"

namespace semantic {

struct value_t
{
  value_t () : present (false) { }
  value_t (const type::Bool*  t, type::Bool::ValueType v)  : present (true), bool_value_ (v) { }
  value_t (const type::Int*   t, type::Int::ValueType  v)  : present (true), int_value_ (v)  { }
  value_t (const type::Int8*  t, type::Int8::ValueType v)  : present (true), int8_value_ (v) { }
  value_t (const type::Int16*  t, type::Int16::ValueType v)  : present (true), int16_value_ (v) { }
  value_t (const type::Int32*  t, type::Int32::ValueType v)  : present (true), int32_value_ (v) { }
  value_t (const type::Int64*  t, type::Int64::ValueType v)  : present (true), int64_value_ (v) { }
  value_t (const type::Uint*  t, type::Uint::ValueType v)  : present (true), uint_value_ (v) { }
  value_t (const type::Uint8* t, type::Uint8::ValueType v) : present (true), uint8_value_ (v) { }
  value_t (const type::Uint16* t, type::Uint16::ValueType v) : present (true), uint16_value_ (v) { }
  value_t (const type::Uint32* t, type::Uint32::ValueType v) : present (true), uint32_value_ (v) { }
  value_t (const type::Uint64* t, type::Uint64::ValueType v) : present (true), uint64_value_ (v) { }
  value_t (const type::Float32* t, type::Float32::ValueType v) : present (true), float32_value_ (v) { }
  value_t (const type::Float64* t, type::Float64::ValueType v) : present (true), float64_value_ (v) { }
  value_t (decl::Callable* c) : present (true), callable_value_ (c) { }
  value_t (decl::Template* t) : present (true), template_value_ (t) { }
  value_t (decl::reaction_t* r) : present (true), reaction_value_ (r) { }
  value_t (const type::Slice* t, type::Slice::ValueType v) : present (true), slice_value_ (v) { }
  value_t (const type::StringU* t, type::StringU::ValueType v) : present (true), stringu_value_ (v) { }
  value_t (const type::Boolean*  t, type::Boolean::ValueType v)  : present (true), boolean_value_ (v) { }
  value_t (const type::Rune*  t, type::Rune::ValueType v)  : present (true), rune_value_ (v) { }
  value_t (const type::Integer*  t, type::Integer::ValueType v)  : present (true), integer_value_ (v) { }
  value_t (const type::Float*  t, type::Float::ValueType v)  : present (true), float_value_ (v) { }
  value_t (const type::Complex*  t, type::Complex::ValueType v)  : present (true), complex_value_ (v) { }
  value_t (const type::String*  t, type::String::ValueType v)  : present (true), string_value_ (v) { }

  bool representable (const type::Type* from, const type::Type* to) const;
  void convert (const type::Type* from, const type::Type* to);
  void convert (type::Kind from, type::Kind to);

  static value_t make_reference (type::Pointer::ValueType v)
  {
    value_t r;
    r.present = true;
    r.reference_value_ = v;
    return r;
  }

  bool present;

  const type::Bool::ValueType& ref (const type::Bool&) const
  {
    return bool_value_;
  }
  type::Bool::ValueType& ref (const type::Bool&)
  {
    return bool_value_;
  }

  const type::Int::ValueType& ref (const type::Int&) const
  {
    return int_value_;
  }
  type::Int::ValueType& ref (const type::Int&)
  {
    return int_value_;
  }
  const type::Int8::ValueType& ref (const type::Int8&) const
  {
    return int8_value_;
  }
  type::Int8::ValueType& ref (const type::Int8&)
  {
    return int8_value_;
  }
  const type::Int16::ValueType& ref (const type::Int16&) const
  {
    return int16_value_;
  }
  type::Int16::ValueType& ref (const type::Int16&)
  {
    return int16_value_;
  }
  const type::Int32::ValueType& ref (const type::Int32&) const
  {
    return int32_value_;
  }
  type::Int32::ValueType& ref (const type::Int32&)
  {
    return int32_value_;
  }
  const type::Int64::ValueType& ref (const type::Int64&) const
  {
    return int64_value_;
  }
  type::Int64::ValueType& ref (const type::Int64&)
  {
    return int64_value_;
  }

  const type::Uintptr::ValueType& ref (const type::Uintptr&) const
  {
    return uintptr_value_;
  }
  type::Uintptr::ValueType& ref (const type::Uintptr&)
  {
    return uintptr_value_;
  }

  const type::Uint::ValueType& ref (const type::Uint&) const
  {
    return uint_value_;
  }
  type::Uint::ValueType& ref (const type::Uint&)
  {
    return uint_value_;
  }
  const type::Uint8::ValueType& ref (const type::Uint8&) const
  {
    return uint8_value_;
  }
  type::Uint8::ValueType& ref (const type::Uint8&)
  {
    return uint8_value_;
  }
  const type::Uint16::ValueType& ref (const type::Uint16&) const
  {
    return uint16_value_;
  }
  type::Uint16::ValueType& ref (const type::Uint16&)
  {
    return uint16_value_;
  }
  const type::Uint32::ValueType& ref (const type::Uint32&) const
  {
    return uint32_value_;
  }
  type::Uint32::ValueType& ref (const type::Uint32&)
  {
    return uint32_value_;
  }
  const type::Uint64::ValueType& ref (const type::Uint64&) const
  {
    return uint64_value_;
  }
  type::Uint64::ValueType& ref (const type::Uint64&)
  {
    return uint64_value_;
  }

  const type::Float32::ValueType& ref (const type::Float32&) const
  {
    return float32_value_;
  }
  type::Float32::ValueType& ref (const type::Float32&)
  {
    return float32_value_;
  }
  const type::Float64::ValueType& ref (const type::Float64&) const
  {
    return float64_value_;
  }
  type::Float64::ValueType& ref (const type::Float64&)
  {
    return float64_value_;
  }

  const type::Complex64::ValueType& ref (const type::Complex64&) const
  {
    return complex64_value_;
  }
  type::Complex64::ValueType& ref (const type::Complex64&)
  {
    return complex64_value_;
  }
  const type::Complex128::ValueType& ref (const type::Complex128&) const
  {
    return complex128_value_;
  }
  type::Complex128::ValueType& ref (const type::Complex128&)
  {
    return complex128_value_;
  }


  type::Pointer::ValueType& ref (const type::Pointer&)
  {
    return pointer_value_;
  }
  const type::Pointer::ValueType& ref (const type::Pointer&) const
  {
    return pointer_value_;
  }
  type::StringU::ValueType& ref (const type::StringU&)
  {
    return stringu_value_;
  }
  const type::StringU::ValueType& ref (const type::StringU&) const
  {
    return stringu_value_;
  }

  type::Slice::ValueType& ref (const type::Slice&)
  {
    return slice_value_;
  }
  const type::Slice::ValueType& ref (const type::Slice&) const
  {
    return slice_value_;
  }
  const type::Boolean::ValueType& ref (const type::Boolean&) const
  {
    return boolean_value_;
  }
  type::Boolean::ValueType& ref (const type::Boolean&)
  {
    return boolean_value_;
  }
  const type::Rune::ValueType& ref (const type::Rune&) const
  {
    return rune_value_;
  }
  type::Rune::ValueType& ref (const type::Rune&)
  {
    return rune_value_;
  }
  const type::Integer::ValueType& ref (const type::Integer&) const
  {
    return integer_value_;
  }
  type::Integer::ValueType& ref (const type::Integer&)
  {
    return integer_value_;
  }
  const type::Float::ValueType& ref (const type::Float&) const
  {
    return float_value_;
  }
  type::Float::ValueType& ref (const type::Float&)
  {
    return float_value_;
  }
  const type::Complex::ValueType& ref (const type::Complex&) const
  {
    return complex_value_;
  }
  type::Complex::ValueType& ref (const type::Complex&)
  {
    return complex_value_;
  }

  const type::String::ValueType& ref (const type::String&) const
  {
    return string_value_;
  }
  type::String::ValueType& ref (const type::String&)
  {
    return string_value_;
  }

  decl::Callable* callable_value () const
  {
    return callable_value_;
  }
  decl::Template* template_value () const
  {
    return template_value_;
  }
  decl::reaction_t* reaction_value () const
  {
    return reaction_value_;
  }
  type::Pointer::ValueType reference_value () const
  {
    return reference_value_;
  }
  type::Slice::ValueType slice_value () const
  {
    return slice_value_;
  }

  type::Int::ValueType to_int (const type::Type* type) const;

  void print (std::ostream& out, const type::Type* type) const;

  union
  {
    type::Bool::ValueType bool_value_;

    type::Uint8::ValueType uint8_value_;
    type::Uint16::ValueType uint16_value_;
    type::Uint32::ValueType uint32_value_;
    type::Uint64::ValueType uint64_value_;

    type::Int8::ValueType int8_value_;
    type::Int16::ValueType int16_value_;
    type::Int32::ValueType int32_value_;
    type::Int64::ValueType int64_value_;

    type::Float32::ValueType float32_value_;
    type::Float64::ValueType float64_value_;

    type::Complex64::ValueType complex64_value_;
    type::Complex128::ValueType complex128_value_;

    type::Uint::ValueType uint_value_;
    type::Int::ValueType int_value_;
    type::Uintptr::ValueType uintptr_value_;

    type::Pointer::ValueType pointer_value_;
    type::Slice::ValueType slice_value_;
    type::StringU::ValueType stringu_value_;

    decl::Callable* callable_value_;
    decl::Template* template_value_;
    decl::reaction_t* reaction_value_;
    type::Pointer::ValueType reference_value_;

    type::Boolean::ValueType boolean_value_;
    type::Rune::ValueType rune_value_;
    type::Integer::ValueType integer_value_;
    type::Float::ValueType float_value_;
    type::Complex::ValueType complex_value_;
    type::String::ValueType string_value_;
  };
};

void equal (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void not_equal (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void less_than (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void less_equal (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void more_than (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void more_equal (value_t& out, const type::Type* type, const value_t& left, const value_t& right);

void multiply (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void divide (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void modulus (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void add (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void subtract (value_t& out, const type::Type* type, const value_t& left, const value_t& right);

void bit_and (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void bit_and_not (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void bit_or (value_t& out, const type::Type* type, const value_t& left, const value_t& right);
void bit_xor (value_t& out, const type::Type* type, const value_t& left, const value_t& right);

}

#endif // rc_src_value_hpp
