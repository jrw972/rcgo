#include "value.hpp"

#include <utility>
#include <cmath>

#include "type.hpp"

namespace semantic
{

using namespace type;
using namespace std::rel_ops;

UntypedComplex UntypedComplex::make (double r, double i)
{
  UntypedComplex retval;
  retval.real = r;
  retval.imag = i;
  return retval;
}

bool UntypedComplex::operator== (const UntypedComplex& other) const
{
  return this->real == other.real && this->imag == other.imag;
}

UntypedComplex& UntypedComplex::operator= (const double& x)
{
  this->real = x;
  this->imag = 0;
  return *this;
}

UntypedComplex& UntypedComplex::operator= (const runtime::Complex64& x)
{
  this->real = x.real;
  this->imag = x.imag;
  return *this;
}

UntypedComplex& UntypedComplex::operator= (const runtime::Complex128& x)
{
  this->real = x.real;
  this->imag = x.imag;
  return *this;
}

UntypedComplex::operator double() const
{
  return this->real;
}

UntypedComplex operator* (const UntypedComplex& x, const UntypedComplex& y)
{
  UntypedComplex uc;
  uc.real = x.real * y.real - x.imag * y.imag;
  uc.imag = x.real * y.imag + x.imag * y.real;
  return uc;
}
UntypedComplex operator/ (const UntypedComplex& x, const UntypedComplex& y)
{
  const double xmag = sqrt (x.real * x.real + x.imag * x.imag);
  const double xtheta = atan (x.imag / x.real);
  const double ymag = sqrt (y.real * y.real + y.imag * y.imag);
  const double ytheta = atan (y.imag / y.real);
  const double mag = xmag / ymag;
  const double theta = xtheta - ytheta;
  UntypedComplex uc;
  uc.real = mag * cos (theta);
  uc.imag = mag * sin (theta);
  return uc;
}
UntypedComplex operator+ (const UntypedComplex& x, const UntypedComplex& y)
{
  UntypedComplex uc;
  uc.real = x.real + y.real;
  uc.imag = x.imag + y.imag;
  return uc;
}
UntypedComplex operator- (const UntypedComplex& x, const UntypedComplex& y)
{
  UntypedComplex uc;
  uc.real = x.real - y.real;
  uc.imag = x.imag - y.imag;
  return uc;
}
UntypedComplex operator- (const UntypedComplex& x)
{
  UntypedComplex uc;
  uc.real = -x.real;
  uc.imag = -x.imag;
  return uc;
}

Value::Value () : present (false) { }

template <typename S, typename T>
static bool to_and_back_helper (const T& x)
{
  S y;
  y = x;
  T z;
  z = y;
  return x == z;
}

template <typename T>
static bool to_and_back (const T& x, const type::Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
      return to_and_back_helper<uint8_t> (x);
    case Uint16_Kind:
      return to_and_back_helper<uint16_t> (x);
    case Uint32_Kind:
      return to_and_back_helper<uint32_t> (x);
    case Uint64_Kind:
      return to_and_back_helper<uint64_t> (x);
    case Int8_Kind:
      return to_and_back_helper<int8_t> (x);
    case Int16_Kind:
      return to_and_back_helper<int16_t> (x);
    case Int32_Kind:
      return to_and_back_helper<int32_t> (x);
    case Int64_Kind:
      return to_and_back_helper<int64_t> (x);
    case Float32_Kind:
      return to_and_back_helper<float> (x);
    case Float64_Kind:
      return to_and_back_helper<double> (x);
    case Complex64_Kind:
      return to_and_back_helper<runtime::Complex64> (x);
    case Complex128_Kind:
      return to_and_back_helper<runtime::Complex128> (x);
    case Uint_Kind:
      return to_and_back_helper<unsigned long> (x);
    case Int_Kind:
      return to_and_back_helper<long> (x);
    case Uintptr_Kind:
      return to_and_back_helper<size_t> (x);
    default:
      return false;
    }
}

static void
convert_numeric (Value& value, UntypedComplex x, const type::Type* type)
{
  if (!type->is_numeric ())
    {
      return;
    }

  switch (type->kind ())
    {
    case Uint8_Kind:
      value.uint8_value = x;
      break;
    case Uint16_Kind:
      value.uint16_value = x;
      break;
    case Uint32_Kind:
      value.uint32_value = x;
      break;
    case Uint64_Kind:
      value.uint64_value = x;
      break;
    case Int8_Kind:
      value.int8_value = x;
      break;
    case Int16_Kind:
      value.int16_value = x;
      break;
    case Int32_Kind:
      value.int32_value = x;
      break;
    case Int64_Kind:
      value.int64_value = x;
      break;
    case Uint_Kind:
      value.uint_value = x;
      break;
    case Int_Kind:
      value.int_value = x;
      break;
    case Uintptr_Kind:
      value.uintptr_value = x;
      break;
    case Float32_Kind:
      value.float32_value = x;
      break;
    case Float64_Kind:
      value.float64_value = x;
      break;
    case Complex64_Kind:
      value.complex64_value = x;
      break;
    case Complex128_Kind:
      value.complex128_value = x;
      break;
    case Untyped_Integer_Kind:
      value.untyped_integer_value = x;
      break;
    case Untyped_Float_Kind:
      value.untyped_float_value = x;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

bool
Value::representable (const type::Type* from, const type::Type* to) const
{
  assert (present);
  assert (from->is_untyped ());
  assert (!to->is_untyped ());

  switch (from->underlying_kind ())
    {
    case Untyped_Nil_Kind:
      return
        to->underlying_kind () == Pointer_Kind ||
        to->underlying_kind () == Slice_Kind;
    case Untyped_Boolean_Kind:
      return to->underlying_kind () == Bool_Kind;
    case Untyped_Rune_Kind:
      return to_and_back (untyped_rune_value, to);
    case Untyped_Integer_Kind:
      return to_and_back (untyped_integer_value, to);
    case Untyped_Float_Kind:
      return to_and_back (untyped_float_value, to);
    case Untyped_Complex_Kind:
      return to_and_back (untyped_complex_value, to);
    case Untyped_String_Kind:
      return to->underlying_kind () == String_Kind;
    default:
      NOT_REACHED;
    }
}

void
Value::convert (const type::Type* from, const type::Type* to)
{
  assert (present);
  from = from->underlying_type ();
  to = to->underlying_type ();

  switch (from->kind ())
    {
    case Untyped_Boolean_Kind:
    {
      if (to->kind () == Bool_Kind)
        {
          this->bool_value = this->untyped_boolean_value;
        }
    }
    break;

    case Untyped_Rune_Kind:
    {
      UntypedComplex x;
      x = this->untyped_rune_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Untyped_Integer_Kind:
    {
      UntypedComplex x;
      x = this->untyped_integer_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Untyped_Float_Kind:
    {
      UntypedComplex x;
      x = this->untyped_float_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Uint8_Kind:
    {
      UntypedComplex x;
      x = this->uint8_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Uint16_Kind:
    {
      UntypedComplex x;
      x = this->uint16_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Uint32_Kind:
    {
      UntypedComplex x;
      x = this->uint32_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Uint64_Kind:
    {
      UntypedComplex x;
      x = this->uint64_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Int8_Kind:
    {
      UntypedComplex x;
      x = this->int8_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Int16_Kind:
    {
      UntypedComplex x;
      x = this->int16_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Int32_Kind:
    {
      UntypedComplex x;
      x = this->int32_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Int64_Kind:
    {
      UntypedComplex x;
      x = this->int64_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Int_Kind:
    {
      UntypedComplex x;
      x = this->int_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Uint_Kind:
    {
      UntypedComplex x;
      x = this->uint_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Uintptr_Kind:
    {
      UntypedComplex x;
      x = this->uintptr_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Float32_Kind:
    {
      UntypedComplex x;
      x = this->float32_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Float64_Kind:
    {
      UntypedComplex x;
      x = this->float64_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Complex64_Kind:
    {
      UntypedComplex x;
      x = this->complex64_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Complex128_Kind:
    {
      UntypedComplex x;
      x = this->complex128_value;
      convert_numeric (*this, x, to);
    }
    break;

    case Untyped_String_Kind:
    {
      if (to->kind () == String_Kind)
        {
          this->string_value = this->untyped_string_value;
        }
    }
    break;

    case Untyped_Nil_Kind:
    {
      if (to->kind () == Pointer_Kind)
        {
          this->pointer_value = NULL;
        }
      else if (to->kind () == Slice_Kind)
        {
          this->slice_value = runtime::Slice::make ();
        }
    }
    break;

    default:
      TYPE_NOT_REACHED (*from);
    }
}

long
Value::to_int (const type::Type* type) const
{
  assert (type->is_typed_integer ());
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
      return uint8_value;
    case Uint16_Kind:
      return uint16_value;
    case Uint32_Kind:
      return uint32_value;
    case Uint64_Kind:
      return uint64_value;
    case Int8_Kind:
      return int8_value;
    case Int16_Kind:
      return int16_value;
    case Int32_Kind:
      return int32_value;
    case Int64_Kind:
      return int64_value;
    case Uint_Kind:
      return uint_value;
    case Int_Kind:
      return int_value;
    case Uintptr_Kind:
      return uintptr_value;
    default:
      NOT_REACHED;
    }
}

void equal (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Nil_Kind:
      out.untyped_boolean_value = true;
      break;
    case Untyped_Boolean_Kind:
      out.untyped_boolean_value = left.untyped_boolean_value == right.untyped_boolean_value;
      break;
    case Untyped_Rune_Kind:
      out.untyped_boolean_value = left.untyped_rune_value == right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_boolean_value = left.untyped_integer_value == right.untyped_integer_value;
      break;
    case Untyped_Float_Kind:
      out.untyped_boolean_value = left.untyped_float_value == right.untyped_float_value;
      break;
    case Untyped_Complex_Kind:
      out.untyped_boolean_value = left.untyped_complex_value == right.untyped_complex_value;
      break;
    case Untyped_String_Kind:
      out.untyped_boolean_value = left.untyped_string_value == right.untyped_string_value;
      break;
    case Bool_Kind:
      out.untyped_boolean_value = left.bool_value == right.bool_value;
      break;
    case Uint8_Kind:
      out.untyped_boolean_value = left.uint8_value == right.uint8_value;
      break;
    case Uint16_Kind:
      out.untyped_boolean_value = left.uint16_value == right.uint16_value;
      break;
    case Uint32_Kind:
      out.untyped_boolean_value = left.uint32_value == right.uint32_value;
      break;
    case Uint64_Kind:
      out.untyped_boolean_value = left.uint64_value == right.uint64_value;
      break;
    case Int8_Kind:
      out.untyped_boolean_value = left.int8_value == right.int8_value;
      break;
    case Int16_Kind:
      out.untyped_boolean_value = left.int16_value == right.int16_value;
      break;
    case Int32_Kind:
      out.untyped_boolean_value = left.int32_value == right.int32_value;
      break;
    case Int64_Kind:
      out.untyped_boolean_value = left.int64_value == right.int64_value;
      break;
    case Float32_Kind:
      out.untyped_boolean_value = left.float32_value == right.float32_value;
      break;
    case Float64_Kind:
      out.untyped_boolean_value = left.float64_value == right.float64_value;
      break;
    case Complex64_Kind:
      out.untyped_boolean_value = left.complex64_value == right.complex64_value;
      break;
    case Complex128_Kind:
      out.untyped_boolean_value = left.complex128_value == right.complex128_value;
      break;
    case Uint_Kind:
      out.untyped_boolean_value = left.uint_value == right.uint_value;
      break;
    case Int_Kind:
      out.untyped_boolean_value = left.int_value == right.int_value;
      break;
    case Uintptr_Kind:
      out.untyped_boolean_value = left.uintptr_value == right.uintptr_value;
      break;
    case String_Kind:
      out.untyped_boolean_value = left.string_value == right.string_value;
      break;
    case Pointer_Kind:
      out.untyped_boolean_value = left.pointer_value == right.pointer_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void not_equal (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Nil_Kind:
      out.untyped_boolean_value = false;
      break;
    case Untyped_Boolean_Kind:
      out.untyped_boolean_value = left.untyped_boolean_value != right.untyped_boolean_value;
      break;
    case Untyped_Rune_Kind:
      out.untyped_boolean_value = left.untyped_rune_value != right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_boolean_value = left.untyped_integer_value != right.untyped_integer_value;
      break;
    case Untyped_Float_Kind:
      out.untyped_boolean_value = left.untyped_float_value != right.untyped_float_value;
      break;
    case Untyped_Complex_Kind:
      out.untyped_boolean_value = left.untyped_complex_value != right.untyped_complex_value;
      break;
    case Untyped_String_Kind:
      out.untyped_boolean_value = left.untyped_string_value != right.untyped_string_value;
      break;
    case Bool_Kind:
      out.untyped_boolean_value = left.bool_value != right.bool_value;
      break;
    case Uint8_Kind:
      out.untyped_boolean_value = left.uint8_value != right.uint8_value;
      break;
    case Uint16_Kind:
      out.untyped_boolean_value = left.uint16_value != right.uint16_value;
      break;
    case Uint32_Kind:
      out.untyped_boolean_value = left.uint32_value != right.uint32_value;
      break;
    case Uint64_Kind:
      out.untyped_boolean_value = left.uint64_value != right.uint64_value;
      break;
    case Int8_Kind:
      out.untyped_boolean_value = left.int8_value != right.int8_value;
      break;
    case Int16_Kind:
      out.untyped_boolean_value = left.int16_value != right.int16_value;
      break;
    case Int32_Kind:
      out.untyped_boolean_value = left.int32_value != right.int32_value;
      break;
    case Int64_Kind:
      out.untyped_boolean_value = left.int64_value != right.int64_value;
      break;
    case Float32_Kind:
      out.untyped_boolean_value = left.float32_value != right.float32_value;
      break;
    case Float64_Kind:
      out.untyped_boolean_value = left.float64_value != right.float64_value;
      break;
    case Complex64_Kind:
      out.untyped_boolean_value = left.complex64_value != right.complex64_value;
      break;
    case Complex128_Kind:
      out.untyped_boolean_value = left.complex128_value != right.complex128_value;
      break;
    case Uint_Kind:
      out.untyped_boolean_value = left.uint_value != right.uint_value;
      break;
    case Int_Kind:
      out.untyped_boolean_value = left.int_value != right.int_value;
      break;
    case Uintptr_Kind:
      out.untyped_boolean_value = left.uintptr_value != right.uintptr_value;
      break;
    case String_Kind:
      out.untyped_boolean_value = left.string_value != right.string_value;
      break;
    case Pointer_Kind:
      out.untyped_boolean_value = left.pointer_value != right.pointer_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void less_than (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_boolean_value = left.untyped_rune_value < right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_boolean_value = left.untyped_integer_value < right.untyped_integer_value;
      break;
    case Untyped_Float_Kind:
      out.untyped_boolean_value = left.untyped_float_value < right.untyped_float_value;
      break;
    case Untyped_String_Kind:
      out.untyped_boolean_value = left.untyped_string_value < right.untyped_string_value;
      break;
    case Uint8_Kind:
      out.untyped_boolean_value = left.uint8_value < right.uint8_value;
      break;
    case Uint16_Kind:
      out.untyped_boolean_value = left.uint16_value < right.uint16_value;
      break;
    case Uint32_Kind:
      out.untyped_boolean_value = left.uint32_value < right.uint32_value;
      break;
    case Uint64_Kind:
      out.untyped_boolean_value = left.uint64_value < right.uint64_value;
      break;
    case Int8_Kind:
      out.untyped_boolean_value = left.int8_value < right.int8_value;
      break;
    case Int16_Kind:
      out.untyped_boolean_value = left.int16_value < right.int16_value;
      break;
    case Int32_Kind:
      out.untyped_boolean_value = left.int32_value < right.int32_value;
      break;
    case Int64_Kind:
      out.untyped_boolean_value = left.int64_value < right.int64_value;
      break;
    case Float32_Kind:
      out.untyped_boolean_value = left.float32_value < right.float32_value;
      break;
    case Float64_Kind:
      out.untyped_boolean_value = left.float64_value < right.float64_value;
      break;
    case Uint_Kind:
      out.untyped_boolean_value = left.uint_value < right.uint_value;
      break;
    case Int_Kind:
      out.untyped_boolean_value = left.int_value < right.int_value;
      break;
    case Uintptr_Kind:
      out.untyped_boolean_value = left.uintptr_value < right.uintptr_value;
      break;
    case String_Kind:
      out.untyped_boolean_value = left.string_value < right.string_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void less_equal (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_boolean_value = left.untyped_rune_value <= right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_boolean_value = left.untyped_integer_value <= right.untyped_integer_value;
      break;
    case Untyped_Float_Kind:
      out.untyped_boolean_value = left.untyped_float_value <= right.untyped_float_value;
      break;
    case Untyped_String_Kind:
      out.untyped_boolean_value = left.untyped_string_value <= right.untyped_string_value;
      break;
    case Uint8_Kind:
      out.untyped_boolean_value = left.uint8_value <= right.uint8_value;
      break;
    case Uint16_Kind:
      out.untyped_boolean_value = left.uint16_value <= right.uint16_value;
      break;
    case Uint32_Kind:
      out.untyped_boolean_value = left.uint32_value <= right.uint32_value;
      break;
    case Uint64_Kind:
      out.untyped_boolean_value = left.uint64_value <= right.uint64_value;
      break;
    case Int8_Kind:
      out.untyped_boolean_value = left.int8_value <= right.int8_value;
      break;
    case Int16_Kind:
      out.untyped_boolean_value = left.int16_value <= right.int16_value;
      break;
    case Int32_Kind:
      out.untyped_boolean_value = left.int32_value <= right.int32_value;
      break;
    case Int64_Kind:
      out.untyped_boolean_value = left.int64_value <= right.int64_value;
      break;
    case Float32_Kind:
      out.untyped_boolean_value = left.float32_value <= right.float32_value;
      break;
    case Float64_Kind:
      out.untyped_boolean_value = left.float64_value <= right.float64_value;
      break;
    case Uint_Kind:
      out.untyped_boolean_value = left.uint_value <= right.uint_value;
      break;
    case Int_Kind:
      out.untyped_boolean_value = left.int_value <= right.int_value;
      break;
    case Uintptr_Kind:
      out.untyped_boolean_value = left.uintptr_value <= right.uintptr_value;
      break;
    case String_Kind:
      out.untyped_boolean_value = left.string_value <= right.string_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void more_than (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_boolean_value = left.untyped_rune_value > right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_boolean_value = left.untyped_integer_value > right.untyped_integer_value;
      break;
    case Untyped_Float_Kind:
      out.untyped_boolean_value = left.untyped_float_value > right.untyped_float_value;
      break;
    case Untyped_String_Kind:
      out.untyped_boolean_value = left.untyped_string_value > right.untyped_string_value;
      break;
    case Uint8_Kind:
      out.untyped_boolean_value = left.uint8_value > right.uint8_value;
      break;
    case Uint16_Kind:
      out.untyped_boolean_value = left.uint16_value > right.uint16_value;
      break;
    case Uint32_Kind:
      out.untyped_boolean_value = left.uint32_value > right.uint32_value;
      break;
    case Uint64_Kind:
      out.untyped_boolean_value = left.uint64_value > right.uint64_value;
      break;
    case Int8_Kind:
      out.untyped_boolean_value = left.int8_value > right.int8_value;
      break;
    case Int16_Kind:
      out.untyped_boolean_value = left.int16_value > right.int16_value;
      break;
    case Int32_Kind:
      out.untyped_boolean_value = left.int32_value > right.int32_value;
      break;
    case Int64_Kind:
      out.untyped_boolean_value = left.int64_value > right.int64_value;
      break;
    case Float32_Kind:
      out.untyped_boolean_value = left.float32_value > right.float32_value;
      break;
    case Float64_Kind:
      out.untyped_boolean_value = left.float64_value > right.float64_value;
      break;
    case Uint_Kind:
      out.untyped_boolean_value = left.uint_value > right.uint_value;
      break;
    case Int_Kind:
      out.untyped_boolean_value = left.int_value > right.int_value;
      break;
    case Uintptr_Kind:
      out.untyped_boolean_value = left.uintptr_value > right.uintptr_value;
      break;
    case String_Kind:
      out.untyped_boolean_value = left.string_value > right.string_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void more_equal (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_boolean_value = left.untyped_rune_value >= right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_boolean_value = left.untyped_integer_value >= right.untyped_integer_value;
      break;
    case Untyped_Float_Kind:
      out.untyped_boolean_value = left.untyped_float_value >= right.untyped_float_value;
      break;
    case Untyped_String_Kind:
      out.untyped_boolean_value = left.untyped_string_value >= right.untyped_string_value;
      break;
    case Uint8_Kind:
      out.untyped_boolean_value = left.uint8_value >= right.uint8_value;
      break;
    case Uint16_Kind:
      out.untyped_boolean_value = left.uint16_value >= right.uint16_value;
      break;
    case Uint32_Kind:
      out.untyped_boolean_value = left.uint32_value >= right.uint32_value;
      break;
    case Uint64_Kind:
      out.untyped_boolean_value = left.uint64_value >= right.uint64_value;
      break;
    case Int8_Kind:
      out.untyped_boolean_value = left.int8_value >= right.int8_value;
      break;
    case Int16_Kind:
      out.untyped_boolean_value = left.int16_value >= right.int16_value;
      break;
    case Int32_Kind:
      out.untyped_boolean_value = left.int32_value >= right.int32_value;
      break;
    case Int64_Kind:
      out.untyped_boolean_value = left.int64_value >= right.int64_value;
      break;
    case Float32_Kind:
      out.untyped_boolean_value = left.float32_value >= right.float32_value;
      break;
    case Float64_Kind:
      out.untyped_boolean_value = left.float64_value >= right.float64_value;
      break;
    case Uint_Kind:
      out.untyped_boolean_value = left.uint_value >= right.uint_value;
      break;
    case Int_Kind:
      out.untyped_boolean_value = left.int_value >= right.int_value;
      break;
    case Uintptr_Kind:
      out.untyped_boolean_value = left.uintptr_value >= right.uintptr_value;
      break;
    case String_Kind:
      out.untyped_boolean_value = left.string_value >= right.string_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void multiply (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_rune_value = left.untyped_rune_value * right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_integer_value = left.untyped_integer_value * right.untyped_integer_value;
      break;
    case Untyped_Float_Kind:
      out.untyped_float_value = left.untyped_float_value * right.untyped_float_value;
      break;
    case Untyped_Complex_Kind:
      out.untyped_complex_value = left.untyped_complex_value * right.untyped_complex_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value * right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value * right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value * right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value * right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value * right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value * right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value * right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value * right.int64_value;
      break;
    case Float32_Kind:
      out.float32_value = left.float32_value * right.float32_value;
      break;
    case Float64_Kind:
      out.float64_value = left.float64_value * right.float64_value;
      break;
    case Complex64_Kind:
      out.complex64_value = left.complex64_value * right.complex64_value;
      break;
    case Complex128_Kind:
      out.complex128_value = left.complex128_value * right.complex128_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value * right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value * right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value * right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void divide (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_rune_value = left.untyped_rune_value / right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_integer_value = left.untyped_integer_value / right.untyped_integer_value;
      break;
    case Untyped_Float_Kind:
      out.untyped_float_value = left.untyped_float_value / right.untyped_float_value;
      break;
    case Untyped_Complex_Kind:
      out.untyped_complex_value = left.untyped_complex_value / right.untyped_complex_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value / right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value / right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value / right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value / right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value / right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value / right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value / right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value / right.int64_value;
      break;
    case Float32_Kind:
      out.float32_value = left.float32_value / right.float32_value;
      break;
    case Float64_Kind:
      out.float64_value = left.float64_value / right.float64_value;
      break;
    case Complex64_Kind:
      out.complex64_value = left.complex64_value / right.complex64_value;
      break;
    case Complex128_Kind:
      out.complex128_value = left.complex128_value / right.complex128_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value / right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value / right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value / right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void modulus (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_rune_value = left.untyped_rune_value % right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_integer_value = left.untyped_integer_value % right.untyped_integer_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value % right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value % right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value % right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value % right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value % right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value % right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value % right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value % right.int64_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value % right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value % right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value % right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void add (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_rune_value = left.untyped_rune_value + right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_integer_value = left.untyped_integer_value + right.untyped_integer_value;
      break;
    case Untyped_Float_Kind:
      out.untyped_float_value = left.untyped_float_value + right.untyped_float_value;
      break;
    case Untyped_Complex_Kind:
      out.untyped_complex_value = left.untyped_complex_value + right.untyped_complex_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value + right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value + right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value + right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value + right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value + right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value + right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value + right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value + right.int64_value;
      break;
    case Float32_Kind:
      out.float32_value = left.float32_value + right.float32_value;
      break;
    case Float64_Kind:
      out.float64_value = left.float64_value + right.float64_value;
      break;
    case Complex64_Kind:
      out.complex64_value = left.complex64_value + right.complex64_value;
      break;
    case Complex128_Kind:
      out.complex128_value = left.complex128_value + right.complex128_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value + right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value + right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value + right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void subtract (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_rune_value = left.untyped_rune_value - right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_integer_value = left.untyped_integer_value - right.untyped_integer_value;
      break;
    case Untyped_Float_Kind:
      out.untyped_float_value = left.untyped_float_value - right.untyped_float_value;
      break;
    case Untyped_Complex_Kind:
      out.untyped_complex_value = left.untyped_complex_value - right.untyped_complex_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value - right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value - right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value - right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value - right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value - right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value - right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value - right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value - right.int64_value;
      break;
    case Float32_Kind:
      out.float32_value = left.float32_value - right.float32_value;
      break;
    case Float64_Kind:
      out.float64_value = left.float64_value - right.float64_value;
      break;
    case Complex64_Kind:
      out.complex64_value = left.complex64_value - right.complex64_value;
      break;
    case Complex128_Kind:
      out.complex128_value = left.complex128_value - right.complex128_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value - right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value - right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value - right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void bit_and (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_rune_value = left.untyped_rune_value & right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_integer_value = left.untyped_integer_value & right.untyped_integer_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value & right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value & right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value & right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value & right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value & right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value & right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value & right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value & right.int64_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value & right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value & right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value & right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void bit_and_not (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_rune_value = left.untyped_rune_value & ~right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_integer_value = left.untyped_integer_value & ~right.untyped_integer_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value & ~right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value & ~right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value & ~right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value & ~right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value & ~right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value & ~right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value & ~right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value & ~right.int64_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value & ~right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value & ~right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value & ~right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void bit_xor (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_rune_value = left.untyped_rune_value ^ right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_integer_value = left.untyped_integer_value ^ right.untyped_integer_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value ^ right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value ^ right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value ^ right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value ^ right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value ^ right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value ^ right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value ^ right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value ^ right.int64_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value ^ right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value ^ right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value ^ right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

void bit_or (Value& out, const type::Type* type, const Value& left, const Value& right)
{
  assert (left.present);
  assert (right.present);
  out.present = true;
  switch (type->underlying_kind ())
    {
    case Untyped_Rune_Kind:
      out.untyped_rune_value = left.untyped_rune_value | right.untyped_rune_value;
      break;
    case Untyped_Integer_Kind:
      out.untyped_integer_value = left.untyped_integer_value | right.untyped_integer_value;
      break;
    case Uint8_Kind:
      out.uint8_value = left.uint8_value | right.uint8_value;
      break;
    case Uint16_Kind:
      out.uint16_value = left.uint16_value | right.uint16_value;
      break;
    case Uint32_Kind:
      out.uint32_value = left.uint32_value | right.uint32_value;
      break;
    case Uint64_Kind:
      out.uint64_value = left.uint64_value | right.uint64_value;
      break;
    case Int8_Kind:
      out.int8_value = left.int8_value | right.int8_value;
      break;
    case Int16_Kind:
      out.int16_value = left.int16_value | right.int16_value;
      break;
    case Int32_Kind:
      out.int32_value = left.int32_value | right.int32_value;
      break;
    case Int64_Kind:
      out.int64_value = left.int64_value | right.int64_value;
      break;
    case Uint_Kind:
      out.uint_value = left.uint_value | right.uint_value;
      break;
    case Int_Kind:
      out.int_value = left.int_value | right.int_value;
      break;
    case Uintptr_Kind:
      out.uintptr_value = left.uintptr_value | right.uintptr_value;
      break;
    default:
      TYPE_NOT_REACHED (*type);
    }
}

  void Value::print (const type::Type* type, std::ostream& out) const
  {
    if (present) {
      switch (type->underlying_kind ())
        {
        case Untyped_Nil_Kind:
          out << "<nil>";
          break;
        case Untyped_Boolean_Kind:
          out << (untyped_boolean_value ? "<true>" : "<false>");
          break;
        case Untyped_Rune_Kind:
          out << '<' << untyped_rune_value << '>';
          break;
        case Untyped_Integer_Kind:
          out << '<' << untyped_integer_value << '>';
          break;
        case Untyped_Float_Kind:
          out << '<' << untyped_float_value << '>';
          break;
        case Untyped_Complex_Kind:
          out << '<' << untyped_complex_value << '>';
          break;
        case Untyped_String_Kind:
          out << '<' << untyped_string_value << '>';
          break;
        case Bool_Kind:
          out << (bool_value ? "<true>" : "<false>");
          break;
        case Uint8_Kind:
          out << '<' << (int)uint8_value << '>';
          break;
        case Uint16_Kind:
          out << '<' << uint16_value << '>';
          break;
        case Uint32_Kind:
          out << '<' << uint32_value << '>';
          break;
        case Uint64_Kind:
          out << '<' << uint64_value << '>';
          break;
        case Int8_Kind:
          out << '<' << (int)int8_value << '>';
          break;
        case Int16_Kind:
          out << '<' << int16_value << '>';
          break;
        case Int32_Kind:
          out << '<' << int32_value << '>';
          break;
        case Int64_Kind:
          out << '<' << int64_value << '>';
          break;
        case Float32_Kind:
          out << '<' << float32_value << '>';
          break;
        case Float64_Kind:
          out << '<' << float64_value << '>';
          break;
        case Complex64_Kind:
          out << '<' << complex64_value << '>';
          break;
        case Complex128_Kind:
          out << '<' << complex128_value << '>';
          break;
        case Uint_Kind:
          out << '<' << uint_value << '>';
          break;
        case Int_Kind:
          out << '<' << int_value << '>';
          break;
        case Uintptr_Kind:
          out << '<' << uintptr_value << '>';
          break;
        case String_Kind:
          out << '<' << string_value << '>';
          break;
        case Pointer_Kind:
          out << '<' << pointer_value << '>';
          break;
        default:
          TYPE_NOT_REACHED (*type);
        }
    }
  }

ValuePrinter::ValuePrinter (const type::Type* t, const Value& v)
  : type (t)
  , value (v)
{ }

std::ostream& operator<< (std::ostream& out, const ValuePrinter& vp)
{
  vp.value.print (vp.type, out);
  return out;
}

}
