#ifndef RC_SRC_VALUE_HPP
#define RC_SRC_VALUE_HPP

#include "types.hpp"
#include "type.hpp"

namespace semantic
{

struct Value
{
  Value () : present (false) { }
  bool representable (const type::Type* from, const type::Type* to) const;
  void convert (const type::Type* from, const type::Type* to);
  type::Int::ValueType to_int (const type::Type* type) const;

  bool present;
  union
  {
    type::Bool::ValueType bool_value;

    type::Uint8::ValueType uint8_value;
    type::Uint16::ValueType uint16_value;
    type::Uint32::ValueType uint32_value;
    type::Uint64::ValueType uint64_value;

    type::Int8::ValueType int8_value;
    type::Int16::ValueType int16_value;
    type::Int32::ValueType int32_value;
    type::Int64::ValueType int64_value;

    type::Float32::ValueType float32_value;
    type::Float64::ValueType float64_value;

    type::Complex64::ValueType complex64_value;
    type::Complex128::ValueType complex128_value;

    type::Uint::ValueType uint_value;
    type::Int::ValueType int_value;
    type::Uintptr::ValueType uintptr_value;

    type::Pointer::ValueType pointer_value;
    type::Slice::ValueType slice_value;
    type::StringU::ValueType stringu_value;

    type::Boolean::ValueType boolean_value;
    type::Rune::ValueType rune_value;
    type::Integer::ValueType integer_value;
    type::Float::ValueType float_value;
    type::Complex::ValueType complex_value;
    type::String::ValueType string_value;
  };
};

struct ValuePrinter
{
  const type::Type* type;
  const Value& value;
  ValuePrinter (const type::Type* t, const Value& v);
};

std::ostream& operator<< (std::ostream& out, const ValuePrinter& vp);

void equal (Value& out, const type::Type* type, const Value& left, const Value& right);
void not_equal (Value& out, const type::Type* type, const Value& left, const Value& right);
void less_than (Value& out, const type::Type* type, const Value& left, const Value& right);
void less_equal (Value& out, const type::Type* type, const Value& left, const Value& right);
void more_than (Value& out, const type::Type* type, const Value& left, const Value& right);
void more_equal (Value& out, const type::Type* type, const Value& left, const Value& right);

void multiply (Value& out, const type::Type* type, const Value& left, const Value& right);
void divide (Value& out, const type::Type* type, const Value& left, const Value& right);
void modulus (Value& out, const type::Type* type, const Value& left, const Value& right);
void add (Value& out, const type::Type* type, const Value& left, const Value& right);
void subtract (Value& out, const type::Type* type, const Value& left, const Value& right);

void bit_and (Value& out, const type::Type* type, const Value& left, const Value& right);
void bit_and_not (Value& out, const type::Type* type, const Value& left, const Value& right);
void bit_or (Value& out, const type::Type* type, const Value& left, const Value& right);
void bit_xor (Value& out, const type::Type* type, const Value& left, const Value& right);

}

#endif // RC_SRC_VALUE_HPP
