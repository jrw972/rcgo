#ifndef value_hpp
#define value_hpp

#include "types.hpp"
#include "Type.hpp"

struct value_t
{
  value_t () : present (false) { }
  value_t (const Type::Bool*  t, Type::Bool::ValueType v)  : present (true), bool_value_ (v) { }
  value_t (const Type::Int*   t, Type::Int::ValueType  v)  : present (true), int_value_ (v)  { }
  value_t (const Type::Int8*  t, Type::Int8::ValueType v)  : present (true), int8_value_ (v) { }
  value_t (const Type::Int16*  t, Type::Int16::ValueType v)  : present (true), int16_value_ (v) { }
  value_t (const Type::Int32*  t, Type::Int32::ValueType v)  : present (true), int32_value_ (v) { }
  value_t (const Type::Int64*  t, Type::Int64::ValueType v)  : present (true), int64_value_ (v) { }
  value_t (const Type::Uint*  t, Type::Uint::ValueType v)  : present (true), uint_value_ (v) { }
  value_t (const Type::Uint8* t, Type::Uint8::ValueType v) : present (true), uint8_value_ (v) { }
  value_t (const Type::Uint16* t, Type::Uint16::ValueType v) : present (true), uint16_value_ (v) { }
  value_t (const Type::Uint32* t, Type::Uint32::ValueType v) : present (true), uint32_value_ (v) { }
  value_t (const Type::Uint64* t, Type::Uint64::ValueType v) : present (true), uint64_value_ (v) { }
  value_t (const Type::NamedType* t, Type::Enum::ValueType v) : present (true), enum_value_ (v) { }
  value_t (const Type::Float32* t, Type::Float32::ValueType v) : present (true), float32_value_ (v) { }
  value_t (const Type::Float64* t, Type::Float64::ValueType v) : present (true), float64_value_ (v) { }
  value_t (Callable* c) : present (true), callable_value_ (c) { }
  value_t (::Template* t) : present (true), template_value_ (t) { }
  value_t (reaction_t* r) : present (true), reaction_value_ (r) { }
  value_t (const Type::Slice* t, Type::Slice::ValueType v) : present (true), slice_value_ (v) { }
  value_t (const Type::StringU* t, Type::StringU::ValueType v) : present (true), stringu_value_ (v) { }
  value_t (const Type::Boolean*  t, Type::Boolean::ValueType v)  : present (true), boolean_value_ (v) { }
  value_t (const Type::Rune*  t, Type::Rune::ValueType v)  : present (true), rune_value_ (v) { }
  value_t (const Type::Integer*  t, Type::Integer::ValueType v)  : present (true), integer_value_ (v) { }
  value_t (const Type::Float*  t, Type::Float::ValueType v)  : present (true), float_value_ (v) { }
  value_t (const Type::Complex*  t, Type::Complex::ValueType v)  : present (true), complex_value_ (v) { }
  value_t (const Type::String*  t, Type::String::ValueType v)  : present (true), string_value_ (v) { }

  bool representable (const Type::Type* from, const Type::Type* to) const;
  void convert (const Type::Type* from, const Type::Type* to);

  static value_t make_reference (Type::Pointer::ValueType v)
  {
    value_t r;
    r.present = true;
    r.reference_value_ = v;
    return r;
  }

  bool present;

  const Type::Bool::ValueType& ref (const Type::Bool&) const
  {
    return bool_value_;
  }
  Type::Bool::ValueType& ref (const Type::Bool&)
  {
    return bool_value_;
  }

  const Type::Int::ValueType& ref (const Type::Int&) const
  {
    return int_value_;
  }
  Type::Int::ValueType& ref (const Type::Int&)
  {
    return int_value_;
  }
  const Type::Int8::ValueType& ref (const Type::Int8&) const
  {
    return int8_value_;
  }
  Type::Int8::ValueType& ref (const Type::Int8&)
  {
    return int8_value_;
  }
  const Type::Int16::ValueType& ref (const Type::Int16&) const
  {
    return int16_value_;
  }
  Type::Int16::ValueType& ref (const Type::Int16&)
  {
    return int16_value_;
  }
  const Type::Int32::ValueType& ref (const Type::Int32&) const
  {
    return int32_value_;
  }
  Type::Int32::ValueType& ref (const Type::Int32&)
  {
    return int32_value_;
  }
  const Type::Int64::ValueType& ref (const Type::Int64&) const
  {
    return int64_value_;
  }
  Type::Int64::ValueType& ref (const Type::Int64&)
  {
    return int64_value_;
  }

  const Type::Uintptr::ValueType& ref (const Type::Uintptr&) const
  {
    return uintptr_value_;
  }
  Type::Uintptr::ValueType& ref (const Type::Uintptr&)
  {
    return uintptr_value_;
  }

  const Type::Uint::ValueType& ref (const Type::Uint&) const
  {
    return uint_value_;
  }
  Type::Uint::ValueType& ref (const Type::Uint&)
  {
    return uint_value_;
  }
  const Type::Uint8::ValueType& ref (const Type::Uint8&) const
  {
    return uint8_value_;
  }
  Type::Uint8::ValueType& ref (const Type::Uint8&)
  {
    return uint8_value_;
  }
  const Type::Uint16::ValueType& ref (const Type::Uint16&) const
  {
    return uint16_value_;
  }
  Type::Uint16::ValueType& ref (const Type::Uint16&)
  {
    return uint16_value_;
  }
  const Type::Uint32::ValueType& ref (const Type::Uint32&) const
  {
    return uint32_value_;
  }
  Type::Uint32::ValueType& ref (const Type::Uint32&)
  {
    return uint32_value_;
  }
  const Type::Uint64::ValueType& ref (const Type::Uint64&) const
  {
    return uint64_value_;
  }
  Type::Uint64::ValueType& ref (const Type::Uint64&)
  {
    return uint64_value_;
  }

  const Type::Float32::ValueType& ref (const Type::Float32&) const
  {
    return float32_value_;
  }
  Type::Float32::ValueType& ref (const Type::Float32&)
  {
    return float32_value_;
  }
  const Type::Float64::ValueType& ref (const Type::Float64&) const
  {
    return float64_value_;
  }
  Type::Float64::ValueType& ref (const Type::Float64&)
  {
    return float64_value_;
  }

  const Type::Complex64::ValueType& ref (const Type::Complex64&) const
  {
    return complex64_value_;
  }
  Type::Complex64::ValueType& ref (const Type::Complex64&)
  {
    return complex64_value_;
  }
  const Type::Complex128::ValueType& ref (const Type::Complex128&) const
  {
    return complex128_value_;
  }
  Type::Complex128::ValueType& ref (const Type::Complex128&)
  {
    return complex128_value_;
  }


  const Type::Enum::ValueType& ref (const Type::Enum&) const
  {
    return enum_value_;
  }
  Type::Pointer::ValueType& ref (const Type::Pointer&)
  {
    return pointer_value_;
  }
  const Type::Pointer::ValueType& ref (const Type::Pointer&) const
  {
    return pointer_value_;
  }
  Type::StringU::ValueType& ref (const Type::StringU&)
  {
    return stringu_value_;
  }
  const Type::StringU::ValueType& ref (const Type::StringU&) const
  {
    return stringu_value_;
  }

  Type::Slice::ValueType& ref (const Type::Slice&)
  {
    return slice_value_;
  }
  const Type::Slice::ValueType& ref (const Type::Slice&) const
  {
    return slice_value_;
  }
  const Type::Boolean::ValueType& ref (const Type::Boolean&) const
  {
    return boolean_value_;
  }
  Type::Boolean::ValueType& ref (const Type::Boolean&)
  {
    return boolean_value_;
  }
  const Type::Rune::ValueType& ref (const Type::Rune&) const
  {
    return rune_value_;
  }
  Type::Rune::ValueType& ref (const Type::Rune&)
  {
    return rune_value_;
  }
  const Type::Integer::ValueType& ref (const Type::Integer&) const
  {
    return integer_value_;
  }
  Type::Integer::ValueType& ref (const Type::Integer&)
  {
    return integer_value_;
  }
  const Type::Float::ValueType& ref (const Type::Float&) const
  {
    return float_value_;
  }
  Type::Float::ValueType& ref (const Type::Float&)
  {
    return float_value_;
  }
  const Type::Complex::ValueType& ref (const Type::Complex&) const
  {
    return complex_value_;
  }
  Type::Complex::ValueType& ref (const Type::Complex&)
  {
    return complex_value_;
  }

  const Type::String::ValueType& ref (const Type::String&) const
  {
    return string_value_;
  }
  Type::String::ValueType& ref (const Type::String&)
  {
    return string_value_;
  }

  Callable* callable_value () const
  {
    return callable_value_;
  }
  ::Template* template_value () const
  {
    return template_value_;
  }
  reaction_t* reaction_value () const
  {
    return reaction_value_;
  }
  Type::Pointer::ValueType reference_value () const
  {
    return reference_value_;
  }
  Type::Slice::ValueType slice_value () const
  {
    return slice_value_;
  }

  void print (std::ostream& out, const Type::Type* type) const;

  union
  {
    Type::Bool::ValueType bool_value_;
    Type::Enum::ValueType enum_value_;

    Type::Uint8::ValueType uint8_value_;
    Type::Uint16::ValueType uint16_value_;
    Type::Uint32::ValueType uint32_value_;
    Type::Uint64::ValueType uint64_value_;

    Type::Int8::ValueType int8_value_;
    Type::Int16::ValueType int16_value_;
    Type::Int32::ValueType int32_value_;
    Type::Int64::ValueType int64_value_;

    Type::Float32::ValueType float32_value_;
    Type::Float64::ValueType float64_value_;

    Type::Complex64::ValueType complex64_value_;
    Type::Complex128::ValueType complex128_value_;

    Type::Uint::ValueType uint_value_;
    Type::Int::ValueType int_value_;
    Type::Uintptr::ValueType uintptr_value_;

    Type::Pointer::ValueType pointer_value_;
    Type::Slice::ValueType slice_value_;
    Type::StringU::ValueType stringu_value_;

    Callable* callable_value_;
    ::Template* template_value_;
    reaction_t* reaction_value_;
    Type::Pointer::ValueType reference_value_;

    Type::Boolean::ValueType boolean_value_;
    Type::Rune::ValueType rune_value_;
    Type::Integer::ValueType integer_value_;
    Type::Float::ValueType float_value_;
    Type::Complex::ValueType complex_value_;
    Type::String::ValueType string_value_;
  };
};

#endif /* value_hpp */
