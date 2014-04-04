/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Value.hpp"
#include "TypeVisitor.hpp"
#include "Type.hpp"
#include "Utf8.hpp"

#include <iostream>

namespace rcgo {

  complex64_t::complex64_t ()
    : real (0)
    , imag (0)
  { }

  complex64_t::complex64_t (float a_real, float a_imag)
    : real (a_real)
    , imag (a_imag)
  { }

  complex64_t complex64_t::operator+ (const complex64_t& a_other) const
  {
    return complex64_t (real + a_other.real, imag + a_other.imag);
  }

  complex64_t complex64_t::operator- (const complex64_t& a_other) const
  {
    return complex64_t (real - a_other.real, imag - a_other.imag);
  }

  complex64_t complex64_t::operator* (const complex64_t& a_other) const
  {
    return complex64_t (real * a_other.real - imag * a_other.imag, real * a_other.imag + imag * a_other.real);
  }

  complex64_t complex64_t::operator/ (const complex64_t& a_other) const
  {
    float denom = a_other.real * a_other.real + a_other.imag * a_other.imag;
    return complex64_t ((real * a_other.real + imag * a_other.real) / denom, (imag * a_other.real - real * a_other.imag) / denom);
  }

  complex64_t complex64_t::operator+ () const
  {
    return complex64_t (+real, +imag);
  }

  complex64_t complex64_t::operator- () const
  {
    return complex64_t (-real, -imag);
  }

  bool complex64_t::operator== (const complex64_t& a_other) const
  {
    return this->real == a_other.real && this->imag == a_other.imag;
  }

  std::ostream& operator<< (std::ostream& out, const complex64_t& val)
  {
    out << val.real << (val.imag >= 0 ? "+" : "") << val.imag << 'i';
    return out;
  }

  complex128_t::complex128_t ()
    : real (0)
    , imag (0)
  { }

  complex128_t::complex128_t (double a_real, double a_imag)
    : real (a_real)
    , imag (a_imag)
  { }

  complex128_t complex128_t::operator+ (const complex128_t& a_other) const
  {
    return complex128_t (real + a_other.real, imag + a_other.imag);
  }

  complex128_t complex128_t::operator- (const complex128_t& a_other) const
  {
    return complex128_t (real - a_other.real, imag - a_other.imag);
  }

  complex128_t complex128_t::operator* (const complex128_t& a_other) const
  {
    return complex128_t (real * a_other.real - imag * a_other.imag, real * a_other.imag + imag * a_other.real);
  }

  complex128_t complex128_t::operator/ (const complex128_t& a_other) const
  {
    float denom = a_other.real * a_other.real + a_other.imag * a_other.imag;
    return complex128_t ((real * a_other.real + imag * a_other.real) / denom, (imag * a_other.real - real * a_other.imag) / denom);
  }

  complex128_t complex128_t::operator+ () const
  {
    return complex128_t (+real, +imag);
  }

  complex128_t complex128_t::operator- () const
  {
    return complex128_t (-real, -imag);
  }

  bool complex128_t::operator== (const complex128_t& a_other) const
  {
    return this->real == a_other.real && this->imag == a_other.imag;
  }

  std::ostream& operator<< (std::ostream& out, const complex128_t& val)
  {
    out << val.real << (val.imag >= 0 ? "+" : "") << val.imag << 'i';
    return out;
  }

  complex_t::complex_t ()
    : real (0, PRECISION)
    , imag (0, PRECISION)
  { }

  complex_t::complex_t (const mpf_class& a_real, const mpf_class& a_imag)
    : real (a_real)
    , imag (a_imag)
  { }

  bool complex_t::operator== (const complex_t& a_other) const
  {
    return this->real == a_other.real && this->imag == a_other.imag;
  }

  complex_t complex_t::operator+ (const complex_t& a_other) const
  {
    return complex_t (real + a_other.real, imag + a_other.imag);
  }

  complex_t complex_t::operator- (const complex_t& a_other) const
  {
    return complex_t (real - a_other.real, imag - a_other.imag);
  }

  complex_t complex_t::operator* (const complex_t& a_other) const
  {
    return complex_t (real * a_other.real - imag * a_other.imag, real * a_other.imag + imag * a_other.real);
  }

  complex_t complex_t::operator/ (const complex_t& a_other) const
  {
    mpf_class denom = a_other.real * a_other.real + a_other.imag * a_other.imag;
    return complex_t ((real * a_other.real + imag * a_other.real) / denom, (imag * a_other.real - real * a_other.imag) / denom);
  }

  complex_t complex_t::operator+ () const
  {
    return complex_t (+real, +imag);
  }

  complex_t complex_t::operator- () const
  {
    return complex_t (-real, -imag);
  }

  std::ostream& operator<< (std::ostream& out, const complex_t& val)
  {
    out << val.real << (val.imag >= 0 ? "+" : "") << val.imag << 'i';
    return out;
  }

  Value::Value ()
    : kind (Unknown)
  { }

  Value::~Value ()
  { }

  Value Value::make_error ()
  {
    Value v;
    v.kind = Error;
    return v;
  }

  Value Value::make_boolean (bool a_value)
  {
    Value v;
    v.kind = Boolean;
    v.Boolean_value = a_value;
    return v;
  }

  Value Value::make_integer (const mpz_class& a_value)
  {
    Value v;
    v.kind = Integer;
    v.Integer_value = a_value;
    return v;
  }

  Value Value::make_float (const mpf_class& a_value)
  {
    Value v;
    v.kind = Float;
    v.Float_value = a_value;
    return v;
  }

  Value Value::make_complex (const mpf_class& a_real, const mpf_class& a_imag)
  {
    Value v;
    v.kind = Complex;
    v.Complex_value = complex_t (a_real, a_imag);
    return v;
  }

  Value Value::make_rune (const mpz_class& a_value)
  {
    Value v;
    v.kind = Rune;
    v.Rune_value = a_value;
    return v;
  }

  Value Value::make_string (const std::string& a_value)
  {
    Value v;
    v.kind = String;
    v.String_value = a_value;
    return v;
  }

  bool Value::is_unknown () const
  {
    return kind == Unknown;
  }

  bool Value::is_error () const
  {
    return kind == Error;
  }

  bool Value::is_arithmetic () const
  {
    assert (!is_unknown ());
    assert (!is_error ());

    switch (kind)
      {
      case Integer:
      case Rune:
      case Float:
      case Complex:
        return true;
      case Typed:
        return rcgo::is_arithmetic (type);
      default:
        return false;
      }
  }

  bool Value::is_signed () const
  {
    assert (!is_unknown ());
    assert (!is_error ());

    switch (kind)
      {
      case Integer:
      case Rune:
      case Float:
      case Complex:
        return true;
      case Typed:
        return rcgo::is_signed (type);
      default:
        return false;
      }
  }

  bool Value::is_integer () const
  {
    assert (!is_unknown ());
    assert (!is_error ());

    switch (kind)
      {
      case Integer:
      case Rune:
        return true;
      case Typed:
        return rcgo::is_integer (type);
      default:
        return false;
      }
  }

  bool Value::is_zero () const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x)
        : x (a_x)
        , flag (false)
      { }
      const Value& x;
      bool flag;

      void visit (const Int8Type&)       { flag = x.int8_value == 0; }
      void visit (const Int16Type&)      { flag = x.int16_value == 0; }
      void visit (const Int32Type&)      { flag = x.int32_value == 0; }
      void visit (const Int64Type&)      { flag = x.int64_value == 0; }
      void visit (const Uint8Type&)      { flag = x.uint8_value == 0; }
      void visit (const Uint16Type&)     { flag = x.uint16_value == 0; }
      void visit (const Uint32Type&)     { flag = x.uint32_value == 0; }
      void visit (const Uint64Type&)     { flag = x.uint64_value == 0; }
      void visit (const IntType&)        { flag = x.int_value == 0; }
      void visit (const UintType&)       { flag = x.uint_value == 0; }
      void visit (const UintptrType&)    { flag = x.uintptr_value == 0; }
      void visit (const Float32Type&)    { flag = x.float32_value == 0; }
      void visit (const Float64Type&)    { flag = x.float64_value == 0; }
      void visit (const Complex64Type&)  { flag = x.complex64_value == complex64_t (0, 0); }
      void visit (const Complex128Type&) { flag = x.complex128_value == complex128_t (0, 0); }
    };

    assert (is_arithmetic ());

    switch (kind)
      {
      case Integer:
        return Integer_value == 0;
      case Rune:
        return Rune_value == 0;
      case Float:
        return Float_value == 0;
      case Complex:
        return Complex_value == complex_t (0, 0);
      case Typed:
        {
          Visitor vis (*this);
          type->underlying_type ()->accept (vis);
          return vis.flag;
        }
      default:
        abort (); // NOT_COVERED
      }
  }

  bool Value::is_string () const
  {
    assert (!is_unknown ());
    assert (!is_error ());

    switch (kind)
      {
      case String:
        return true;
      case Typed:
        return rcgo::is_string (type);
      default:
        return false;
      }
  }

  bool Value::is_boolean () const
  {
    assert (!is_unknown ());
    assert (!is_error ());

    switch (kind)
      {
      case Boolean:
        return true;
      case Typed:
        return rcgo::is_boolean (type);
      default:
        return false;
      }
  }

  namespace {

    struct ConvertVisitor : public DefaultTypeVisitor {
      const Value& value;
      Value new_value;

      ConvertVisitor (const Value& a_value)
        : value (a_value)
        , new_value (Value::make_error ())
      { }

      void visit (const BoolType& type)
      {
        if (value.kind == Value::Boolean)
          {
            new_value.kind = Value::Typed;
            new_value.bool_value = value.Boolean_value;
          }
      }

      void visit (const StringType& type)
      {
        if (value.kind == Value::String)
          {
            new_value.kind = Value::Typed;
            new_value.string_value = value.String_value;
          }
      }

      template <typename T>
      static bool exact_check (const mpz_class& from, T to)
      {
        return mpz_class (to) == from;
      }

      template <typename T>
      static bool close_check (const mpf_class& from, T to)
      {
        if (from == 0)
          {
            return from == to;
          }
        mpf_class q = from / mpf_class (to);
        return q > .5 && q < 1.5;
      }

#define CONVERT_TO_COMPLEX(Type, ValueMember, ValueType) void visit (const Type& type) \
      {                                                                 \
        switch (value.kind)                                             \
          {                                                             \
          case Value::Integer:                                          \
            new_value.ValueMember = ValueType (value.Integer_value.get_d (), 0); \
            if (exact_check (value.Integer_value, new_value.ValueMember.real)) \
              {                                                         \
                new_value.kind = Value::Typed;                          \
              }                                                         \
            break;                                                      \
          case Value::Float:                                            \
            new_value.ValueMember = ValueType (value.Float_value.get_d (), 0); \
            if (close_check (value.Float_value, new_value.ValueMember.real)) \
              {                                                         \
                new_value.kind = Value::Typed;                          \
              }                                                         \
            break;                                                      \
          case Value::Complex:                                          \
            new_value.ValueMember = ValueType (value.Complex_value.real.get_d (), value.Complex_value.imag.get_d ()); \
            if (close_check (value.Complex_value.real, new_value.ValueMember.real) && \
                close_check (value.Complex_value.imag, new_value.ValueMember.imag)) \
              {                                                         \
                new_value.kind = Value::Typed;                          \
              }                                                         \
            break;                                                      \
          case Value::Rune:                                             \
            new_value.ValueMember = ValueType (value.Rune_value.get_d (), 0); \
            if (exact_check (value.Rune_value, new_value.ValueMember.real)) \
              {                                                         \
                new_value.kind = Value::Typed;                          \
              }                                                         \
            break;                                                      \
          default:                                                      \
            break;                                                      \
          }                                                             \
      }

      CONVERT_TO_COMPLEX(Complex64Type, complex64_value, complex64_t)
      CONVERT_TO_COMPLEX(Complex128Type, complex128_value, complex128_t)

#define CONVERT_TO_FLOAT(Type, ValueMember) void visit (const Type& type) \
      {                                                                 \
      switch (value.kind)                                               \
        {                                                               \
        case Value::Integer:                                            \
          new_value.ValueMember = value.Integer_value.get_d ();         \
          if (exact_check (value.Integer_value, new_value.ValueMember)) \
            {                                                           \
              new_value.kind = Value::Typed;                            \
            }                                                           \
          break;                                                        \
        case Value::Float:                                              \
          new_value.ValueMember = value.Float_value.get_d ();           \
          if (close_check (value.Float_value, new_value.ValueMember))   \
            {                                                           \
              new_value.kind = Value::Typed;                            \
            }                                                           \
          break;                                                        \
        case Value::Complex:                                            \
          new_value.ValueMember = value.Complex_value.real.get_d ();    \
          if (close_check (value.Complex_value.real, new_value.ValueMember) && \
              value.Complex_value.imag == 0)                            \
            {                                                           \
              new_value.kind = Value::Typed;                            \
            }                                                           \
          break;                                                        \
        case Value::Rune:                                               \
          new_value.ValueMember = value.Rune_value.get_d ();            \
          if (exact_check (value.Rune_value, new_value.ValueMember))    \
            {                                                           \
              new_value.kind = Value::Typed;                            \
            }                                                           \
          break;                                                        \
        default:                                                        \
          break;                                                        \
        }                                                               \
      }

      CONVERT_TO_FLOAT(Float32Type, float32_value)
      CONVERT_TO_FLOAT(Float64Type, float64_value)

#define CONVERT_TO_INT(Type, ValueMember) void visit (const Type& type) \
      {                                                                 \
        switch (value.kind)                                             \
          {                                                             \
          case Value::Integer:                                          \
            new_value.ValueMember = value.Integer_value.get_si ();      \
            if (exact_check (value.Integer_value, new_value.ValueMember)) \
              {                                                         \
                new_value.kind = Value::Typed;                          \
              }                                                         \
            break;                                                      \
          case Value::Float:                                            \
            new_value.ValueMember = value.Float_value.get_si ();        \
            if (close_check (value.Float_value, new_value.ValueMember)) \
              {                                                         \
                new_value.kind = Value::Typed;                          \
              }                                                         \
            break;                                                      \
          case Value::Complex:                                          \
            new_value.ValueMember = value.Complex_value.real.get_si (); \
            if (close_check (value.Complex_value.real, new_value.ValueMember) && \
                value.Complex_value.imag == 0)                          \
              {                                                         \
                new_value.kind = Value::Typed;                          \
              }                                                         \
            break;                                                      \
          case Value::Rune:                                             \
            new_value.ValueMember = value.Rune_value.get_si ();         \
            if (exact_check (value.Rune_value, new_value.ValueMember))  \
              {                                                         \
                new_value.kind = Value::Typed;                          \
              }                                                         \
            break;                                                      \
          default:                                                      \
            break;                                                      \
          }                                                             \
      }

      CONVERT_TO_INT(IntType, int_value)
      CONVERT_TO_INT(Int8Type, int8_value)
      CONVERT_TO_INT(Int16Type, int16_value)
      CONVERT_TO_INT(Int32Type, int32_value)
      CONVERT_TO_INT(Int64Type, int64_value)

#define CONVERT_TO_UINT(Type, ValueMember) void visit (const Type& type) \
      {                                                                 \
      switch (value.kind)                                               \
        {                                                               \
        case Value::Integer:                                            \
          new_value.ValueMember = value.Integer_value.get_ui ();        \
          if (exact_check (value.Integer_value, new_value.ValueMember)) \
            {                                                           \
              new_value.kind = Value::Typed;                            \
            }                                                           \
          break;                                                        \
        case Value::Float:                                              \
          new_value.ValueMember = value.Float_value.get_ui ();          \
          if (close_check (value.Float_value, new_value.ValueMember))   \
            {                                                           \
              new_value.kind = Value::Typed;                            \
            }                                                           \
          break;                                                        \
        case Value::Complex:                                            \
          new_value.ValueMember = value.Complex_value.real.get_ui ();   \
          if (close_check (value.Complex_value.real, new_value.ValueMember) && \
              value.Complex_value.imag == 0)                            \
            {                                                           \
              new_value.kind = Value::Typed;                            \
            }                                                           \
          break;                                                        \
        case Value::Rune:                                               \
          new_value.ValueMember = value.Rune_value.get_ui ();           \
          if (exact_check (value.Rune_value, new_value.ValueMember))    \
            {                                                           \
              new_value.kind = Value::Typed;                            \
            }                                                           \
          break;                                                        \
        default:                                                        \
          break;                                                        \
        }                                                               \
      }

      CONVERT_TO_UINT(UintType, uint_value)
      CONVERT_TO_UINT(Uint8Type, uint8_value)
      CONVERT_TO_UINT(Uint16Type, uint16_value)
      CONVERT_TO_UINT(Uint32Type, uint32_value)
      CONVERT_TO_UINT(Uint64Type, uint64_value)
      CONVERT_TO_UINT(UintptrType, uintptr_value)
    };

  }

  Value Value::convert (const Type* type) const
  {
    assert (!is_unknown ());

    if (is_error ())
      {
        return make_error ();
      }

    ConvertVisitor visitor (*this);
    type->underlying_type ()->accept (visitor);
    if (!visitor.new_value.is_error ())
      {
        visitor.new_value.type = type;
      }
    return visitor.new_value;
  }

  Value Value::promote (const Value& other) const
  {
    assert (!is_unknown ());
    assert (!other.is_unknown ());
    assert (other.kind >= kind);

    if (kind == other.kind)
      {
        return *this;
      }

    if (other.kind == Typed)
      {
        return convert (other.type);
      }

    switch (kind)
      {
      case Boolean:
        return make_error ();
      case String:
        return make_error ();
      case Integer:
        return make_rune (Integer_value).promote (other);
      case Rune:
        return make_float (Rune_value).promote (other);
      case Float:
        return make_complex (Float_value, 0).promote (other);

      default:
        abort (); // NOT_COVERED
      }
  }

  Value Value::to_integer () const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, Value& a_value)
        : x (a_x)
        , value (a_value)
      { }
      const Value& x;
      Value& value;

      void visit (const Int8Type&) { value = make_integer (x.int8_value); }
      void visit (const Int16Type&) { value = make_integer (x.int16_value); }
      void visit (const Int32Type&) { value = make_integer (x.int32_value); }
      void visit (const Int64Type&) { value = make_integer (x.int64_value); }
      void visit (const Uint8Type&) { value = make_integer (x.uint8_value); }
      void visit (const Uint16Type&) { value = make_integer (x.uint16_value); }
      void visit (const Uint32Type&) { value = make_integer (x.uint32_value); }
      void visit (const Uint64Type&) { value = make_integer (x.uint64_value); }
      void visit (const IntType&) { value = make_integer (x.int_value); }
      void visit (const UintType&) { value = make_integer (x.uint_value); }
      void visit (const UintptrType&) { value = make_integer (x.uintptr_value); }
    };

    assert (!is_unknown ());

    switch (kind)
      {
      case Integer:
        return *this;
      case Rune:
        return make_integer (Rune_value);
      case Float:
        {
          mpz_class x (Float_value);
          if (Float_value == mpf_class (x))
            {
              return make_integer (x);
            }
          else
            {
              return make_error ();
            }
        }
        break;
      case Complex:
        {
          mpz_class x (Complex_value.real);
          if (Complex_value == complex_t (x, 0))
            {
              return make_integer (x);
            }
          else
            {
              return make_error ();
            }
        }
        break;
      case Typed:
        if (rcgo::is_integer (type))
          {
            Value v;
            Visitor vis (*this, v);
            type->underlying_type ()->accept (vis);
            return v;
          }
        else
          {
            return make_error ();
          }
        break;
      default:
        return make_error ();
      }
  }

  // TODO:  Handle overflow and underflow.

  Value Value::posate () const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, Value& a_value)
        : x (a_x)
        , value (a_value)
      { }
      const Value& x;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = +x.int8_value; }
      void visit (const Int16Type&) { value.int16_value = +x.int16_value; }
      void visit (const Int32Type&) { value.int32_value = +x.int32_value; }
      void visit (const Int64Type&) { value.int64_value = +x.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = +x.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = +x.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = +x.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = +x.uint64_value; }
      void visit (const IntType&) { value.int_value = +x.int_value; }
      void visit (const UintType&) { value.uint_value = +x.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = +x.uintptr_value; }
      void visit (const Float32Type&) { value.float32_value = +x.float32_value; }
      void visit (const Float64Type&) { value.float64_value = +x.float64_value; }
      void visit (const Complex64Type&) { value.complex64_value = +x.complex64_value; }
      void visit (const Complex128Type&) { value.complex128_value = +x.complex128_value; }
    };

    assert (!is_unknown ());

    if (is_error ())
      {
        return make_error ();
      }

    if (!is_arithmetic ())
      {
        return make_error ();
      }

    Value v;
    v.kind = this->kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = +Integer_value;
        break;
      case Rune:
        v.Rune_value = +Rune_value;
        break;
      case Float:
        v.Float_value = +Float_value;
        break;
      case Complex:
        v.Complex_value = +Complex_value;
        break;
      case Typed:
        {
          v.type = this->type;
          Visitor vis (*this, v);
          this->type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::negate () const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, Value& a_value)
        : x (a_x)
        , value (a_value)
      { }
      const Value& x;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = -x.int8_value; }
      void visit (const Int16Type&) { value.int16_value = -x.int16_value; }
      void visit (const Int32Type&) { value.int32_value = -x.int32_value; }
      void visit (const Int64Type&) { value.int64_value = -x.int64_value; }
      void visit (const IntType&) { value.int_value = -x.int_value; }
      void visit (const Float32Type&) { value.float32_value = -x.float32_value; }
      void visit (const Float64Type&) { value.float64_value = -x.float64_value; }
      void visit (const Complex64Type&) { value.complex64_value = -x.complex64_value; }
      void visit (const Complex128Type&) { value.complex128_value = -x.complex128_value; }
    };

    assert (!is_unknown ());

    if (is_error ())
      {
        return make_error ();
      }

    if (!is_signed ())
      {
        return make_error ();
      }

    Value v;
    v.kind = this->kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = -Integer_value;
        break;
      case Rune:
        v.Rune_value = -Rune_value;
        break;
      case Float:
        v.Float_value = -Float_value;
        break;
      case Complex:
        v.Complex_value = -Complex_value;
        break;
      case Typed:
        {
          v.type = this->type;
          Visitor vis (*this, v);
          this->type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::logic_not () const
  {
    assert (!is_unknown ());

    if (is_error ())
      {
        return make_error ();
      }

    if (!is_boolean ())
      {
        return make_error ();
      }

    Value v;
    v.kind = this->kind;
    switch (v.kind)
      {
      case Boolean:
        v.Boolean_value = !Boolean_value;
        break;
      case Typed:
        v.bool_value = !bool_value;
        v.type = type;
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::bit_not () const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, Value& a_value)
        : x (a_x)
        , value (a_value)
      { }
      const Value& x;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = ~x.int8_value; }
      void visit (const Int16Type&) { value.int16_value = ~x.int16_value; }
      void visit (const Int32Type&) { value.int32_value = ~x.int32_value; }
      void visit (const Int64Type&) { value.int64_value = ~x.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = ~x.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = ~x.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = ~x.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = ~x.uint64_value; }
      void visit (const IntType&) { value.int_value = ~x.int_value; }
      void visit (const UintType&) { value.uint_value = ~x.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = ~x.uintptr_value; }
    };

    assert (!is_unknown ());

    if (is_error ())
      {
        return make_error ();
      }

    if (!is_integer ())
      {
        return make_error ();
      }

    Value v;
    v.kind = this->kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = ~Integer_value;
        break;
      case Rune:
        v.Rune_value = ~Rune_value;
        break;
      case Typed:
        {
          v.type = this->type;
          Visitor vis (*this, v);
          this->type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::add (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      const Value& y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value + y.int8_value; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value + y.int16_value; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value + y.int32_value; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value + y.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value + y.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value + y.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value + y.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value + y.uint64_value; }
      void visit (const IntType&) { value.int_value = x.int_value + y.int_value; }
      void visit (const UintType&) { value.uint_value = x.uint_value + y.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value + y.uintptr_value; }
      void visit (const Float32Type&) { value.float32_value = x.float32_value + y.float32_value; }
      void visit (const Float64Type&) { value.float64_value = x.float64_value + y.float64_value; }
      void visit (const Complex64Type&) { value.complex64_value = x.complex64_value + y.complex64_value; }
      void visit (const Complex128Type&) { value.complex128_value = x.complex128_value + y.complex128_value; }
      void visit (const StringType&) { value.string_value = x.string_value + y.string_value; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    if (is_error () || y.is_error ())
      {
        return make_error ();
      }

    if (!((is_arithmetic () && y.is_arithmetic ()) ||
          (is_string () && y.is_string ())))
      {
        return make_error ();
      }

    Value xx (*this);
    Value yy (y);
    if (xx.kind < yy.kind)
      {
        xx = xx.promote (yy);
      }
    else if (yy.kind < xx.kind)
      {
        yy = yy.promote (xx);
      }

    Value v;
    v.kind = xx.kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = xx.Integer_value + yy.Integer_value;
        break;
      case Rune:
        v.Rune_value = xx.Rune_value + yy.Rune_value;
        break;
      case Float:
        v.Float_value = xx.Float_value + yy.Float_value;
        break;
      case Complex:
        v.Complex_value = xx.Complex_value + yy.Complex_value;
        break;
      case String:
        v.String_value = xx.String_value + yy.String_value;
        break;
      case Typed:
        {
          if (xx.type != yy.type)
            {
              return Value::make_error ();
            }
          v.type = xx.type;
          Visitor vis (xx, yy, v);
          xx.type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::subtract (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      const Value& y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value - y.int8_value; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value - y.int16_value; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value - y.int32_value; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value - y.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value - y.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value - y.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value - y.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value - y.uint64_value; }
      void visit (const IntType&) { value.int_value = x.int_value - y.int_value; }
      void visit (const UintType&) { value.uint_value = x.uint_value - y.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value - y.uintptr_value; }
      void visit (const Float32Type&) { value.float32_value = x.float32_value - y.float32_value; }
      void visit (const Float64Type&) { value.float64_value = x.float64_value - y.float64_value; }
      void visit (const Complex64Type&) { value.complex64_value = x.complex64_value - y.complex64_value; }
      void visit (const Complex128Type&) { value.complex128_value = x.complex128_value - y.complex128_value; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    if (is_error () || y.is_error ())
      {
        return make_error ();
      }

    if (!(is_arithmetic () && y.is_arithmetic ()))
      {
        return make_error ();
      }

    Value xx (*this);
    Value yy (y);
    if (xx.kind < yy.kind)
      {
        xx = xx.promote (yy);
      }
    else if (yy.kind < xx.kind)
      {
        yy = yy.promote (xx);
      }

    Value v;
    v.kind = xx.kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = xx.Integer_value - yy.Integer_value;
        break;
      case Rune:
        v.Rune_value = xx.Rune_value - yy.Rune_value;
        break;
      case Float:
        v.Float_value = xx.Float_value - yy.Float_value;
        break;
      case Complex:
        v.Complex_value = xx.Complex_value - yy.Complex_value;
        break;
      case Typed:
        {
          if (xx.type != yy.type)
            {
              return Value::make_error ();
            }
          v.type = xx.type;
          Visitor vis (xx, yy, v);
          xx.type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::multiply (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      const Value& y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value * y.int8_value; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value * y.int16_value; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value * y.int32_value; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value * y.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value * y.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value * y.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value * y.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value * y.uint64_value; }
      void visit (const IntType&) { value.int_value = x.int_value * y.int_value; }
      void visit (const UintType&) { value.uint_value = x.uint_value * y.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value * y.uintptr_value; }
      void visit (const Float32Type&) { value.float32_value = x.float32_value * y.float32_value; }
      void visit (const Float64Type&) { value.float64_value = x.float64_value * y.float64_value; }
      void visit (const Complex64Type&) { value.complex64_value = x.complex64_value * y.complex64_value; }
      void visit (const Complex128Type&) { value.complex128_value = x.complex128_value * y.complex128_value; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    if (is_error () || y.is_error ())
      {
        return make_error ();
      }

    if (!(is_arithmetic () && y.is_arithmetic ()))
      {
        return make_error ();
      }

    Value xx (*this);
    Value yy (y);
    if (xx.kind < yy.kind)
      {
        xx = xx.promote (yy);
      }
    else if (yy.kind < xx.kind)
      {
        yy = yy.promote (xx);
      }

    Value v;
    v.kind = xx.kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = xx.Integer_value * yy.Integer_value;
        break;
      case Rune:
        v.Rune_value = xx.Rune_value * yy.Rune_value;
        break;
      case Float:
        v.Float_value = xx.Float_value * yy.Float_value;
        break;
      case Complex:
        v.Complex_value = xx.Complex_value * yy.Complex_value;
        break;
      case Typed:
        {
          if (xx.type != yy.type)
            {
              return Value::make_error ();
            }
          v.type = xx.type;
          Visitor vis (xx, yy, v);
          xx.type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::divide (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      const Value& y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value / y.int8_value; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value / y.int16_value; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value / y.int32_value; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value / y.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value / y.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value / y.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value / y.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value / y.uint64_value; }
      void visit (const IntType&) { value.int_value = x.int_value / y.int_value; }
      void visit (const UintType&) { value.uint_value = x.uint_value / y.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value / y.uintptr_value; }
      void visit (const Float32Type&) { value.float32_value = x.float32_value / y.float32_value; }
      void visit (const Float64Type&) { value.float64_value = x.float64_value / y.float64_value; }
      void visit (const Complex64Type&) { value.complex64_value = x.complex64_value / y.complex64_value; }
      void visit (const Complex128Type&) { value.complex128_value = x.complex128_value / y.complex128_value; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    if (is_error () || y.is_error ())
      {
        return make_error ();
      }

    if (!(is_arithmetic () && y.is_arithmetic ()))
      {
        return make_error ();
      }

    if (y.is_zero ())
      {
        return make_error ();
      }

    Value xx (*this);
    Value yy (y);
    if (xx.kind < yy.kind)
      {
        xx = xx.promote (yy);
      }
    else if (yy.kind < xx.kind)
      {
        yy = yy.promote (xx);
      }

    Value v;
    v.kind = xx.kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = xx.Integer_value / yy.Integer_value;
        break;
      case Rune:
        v.Rune_value = xx.Rune_value / yy.Rune_value;
        break;
      case Float:
        v.Float_value = xx.Float_value / yy.Float_value;
        break;
      case Complex:
        v.Complex_value = xx.Complex_value / yy.Complex_value;
        break;
      case Typed:
        {
          if (xx.type != yy.type)
            {
              return Value::make_error ();
            }
          v.type = xx.type;
          Visitor vis (xx, yy, v);
          xx.type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::modulo (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      const Value& y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value % y.int8_value; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value % y.int16_value; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value % y.int32_value; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value % y.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value % y.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value % y.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value % y.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value % y.uint64_value; }
      void visit (const IntType&) { value.int_value = x.int_value % y.int_value; }
      void visit (const UintType&) { value.uint_value = x.uint_value % y.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value % y.uintptr_value; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    if (is_error () || y.is_error ())
      {
        return make_error ();
      }

    if (!(is_integer () && y.is_integer ()))
      {
        return make_error ();
      }

    if (y.is_zero ())
      {
        return make_error ();
      }

    Value xx (*this);
    Value yy (y);
    if (xx.kind < yy.kind)
      {
        xx = xx.promote (yy);
      }
    else if (yy.kind < xx.kind)
      {
        yy = yy.promote (xx);
      }

    Value v;
    v.kind = xx.kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = xx.Integer_value % yy.Integer_value;
        break;
      case Rune:
        v.Rune_value = xx.Rune_value % yy.Rune_value;
        break;
      case Typed:
        {
          if (xx.type != yy.type)
            {
              return Value::make_error ();
            }
          v.type = xx.type;
          Visitor vis (xx, yy, v);
          xx.type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::left_shift (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, unsigned int a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      unsigned int y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value << y; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value << y; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value << y; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value << y; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value << y; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value << y; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value << y; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value << y; }
      void visit (const IntType&) { value.int_value = x.int_value << y; }
      void visit (const UintType&) { value.uint_value = x.uint_value << y; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value << y; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    const Value yy = y.to_integer ().convert (&UintType::instance);
    {
      const Value xx = to_integer ();
      if (xx.is_error () || yy.is_error ())
        {
          return make_error ();
        }
    }

    Value v;
    switch (this->kind)
      {
      case Integer:
        v = make_integer (Integer_value << yy.uint_value);
        break;
      case Rune:
        v = make_integer (Rune_value << yy.uint_value);
        break;
      case Float:
        v = make_integer (mpz_class (Float_value) << yy.uint_value);
        break;
      case Complex:
        v = make_integer (mpz_class (Complex_value.real) << yy.uint_value);
        break;
      case Typed:
        {
          v.kind = Typed;
          v.type = this->type;
          Visitor vis (*this, yy.uint_value, v);
          this->type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::right_shift (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, unsigned int a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      unsigned int y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value >> y; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value >> y; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value >> y; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value >> y; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value >> y; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value >> y; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value >> y; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value >> y; }
      void visit (const IntType&) { value.int_value = x.int_value >> y; }
      void visit (const UintType&) { value.uint_value = x.uint_value >> y; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value >> y; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    const Value yy = y.to_integer ().convert (&UintType::instance);
    {
      const Value xx = to_integer ();
      if (xx.is_error () || yy.is_error ())
        {
          return make_error ();
        }
    }

    Value v;
    switch (this->kind)
      {
      case Integer:
        v = make_integer (Integer_value >> yy.uint_value);
        break;
      case Rune:
        v = make_integer (Rune_value >> yy.uint_value);
        break;
      case Float:
        v = make_integer (mpz_class (Float_value) >> yy.uint_value);
        break;
      case Complex:
        v = make_integer (mpz_class (Complex_value.real) >> yy.uint_value);
        break;
      case Typed:
        {
          v.kind = Typed;
          v.type = this->type;
          Visitor vis (*this, yy.uint_value, v);
          this->type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::bit_and (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      const Value& y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value & y.int8_value; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value & y.int16_value; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value & y.int32_value; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value & y.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value & y.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value & y.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value & y.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value & y.uint64_value; }
      void visit (const IntType&) { value.int_value = x.int_value & y.int_value; }
      void visit (const UintType&) { value.uint_value = x.uint_value & y.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value & y.uintptr_value; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    if (is_error () || y.is_error ())
      {
        return make_error ();
      }

    if (!(is_integer () && y.is_integer ()))
      {
        return make_error ();
      }

    Value xx (*this);
    Value yy (y);
    if (xx.kind < yy.kind)
      {
        xx = xx.promote (yy);
      }
    else if (yy.kind < xx.kind)
      {
        yy = yy.promote (xx);
      }

    Value v;
    v.kind = xx.kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = xx.Integer_value & yy.Integer_value;
        break;
      case Rune:
        v.Rune_value = xx.Rune_value & yy.Rune_value;
        break;
      case Typed:
        {
          if (xx.type != yy.type)
            {
              return Value::make_error ();
            }
          v.type = xx.type;
          Visitor vis (xx, yy, v);
          xx.type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::bit_and_not (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      const Value& y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value & ~y.int8_value; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value & ~y.int16_value; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value & ~y.int32_value; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value & ~y.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value & ~y.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value & ~y.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value & ~y.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value & ~y.uint64_value; }
      void visit (const IntType&) { value.int_value = x.int_value & ~y.int_value; }
      void visit (const UintType&) { value.uint_value = x.uint_value & ~y.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value & ~y.uintptr_value; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    if (is_error () || y.is_error ())
      {
        return make_error ();
      }

    if (!(is_integer () && y.is_integer ()))
      {
        return make_error ();
      }

    Value xx (*this);
    Value yy (y);
    if (xx.kind < yy.kind)
      {
        xx = xx.promote (yy);
      }
    else if (yy.kind < xx.kind)
      {
        yy = yy.promote (xx);
      }

    Value v;
    v.kind = xx.kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = xx.Integer_value & ~yy.Integer_value;
        break;
      case Rune:
        v.Rune_value = xx.Rune_value & ~yy.Rune_value;
        break;
      case Typed:
        {
          if (xx.type != yy.type)
            {
              return Value::make_error ();
            }
          v.type = xx.type;
          Visitor vis (xx, yy, v);
          xx.type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::bit_or (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      const Value& y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value | y.int8_value; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value | y.int16_value; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value | y.int32_value; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value | y.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value | y.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value | y.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value | y.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value | y.uint64_value; }
      void visit (const IntType&) { value.int_value = x.int_value | y.int_value; }
      void visit (const UintType&) { value.uint_value = x.uint_value | y.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value | y.uintptr_value; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    if (is_error () || y.is_error ())
      {
        return make_error ();
      }

    if (!(is_integer () && y.is_integer ()))
      {
        return make_error ();
      }

    Value xx (*this);
    Value yy (y);
    if (xx.kind < yy.kind)
      {
        xx = xx.promote (yy);
      }
    else if (yy.kind < xx.kind)
      {
        yy = yy.promote (xx);
      }

    Value v;
    v.kind = xx.kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = xx.Integer_value | yy.Integer_value;
        break;
      case Rune:
        v.Rune_value = xx.Rune_value | yy.Rune_value;
        break;
      case Typed:
        {
          if (xx.type != yy.type)
            {
              return Value::make_error ();
            }
          v.type = xx.type;
          Visitor vis (xx, yy, v);
          xx.type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::bit_xor (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y, Value& a_value)
        : x (a_x)
        , y (a_y)
        , value (a_value)
      { }
      const Value& x;
      const Value& y;
      Value& value;

      void visit (const Int8Type&) { value.int8_value = x.int8_value ^ y.int8_value; }
      void visit (const Int16Type&) { value.int16_value = x.int16_value ^ y.int16_value; }
      void visit (const Int32Type&) { value.int32_value = x.int32_value ^ y.int32_value; }
      void visit (const Int64Type&) { value.int64_value = x.int64_value ^ y.int64_value; }
      void visit (const Uint8Type&) { value.uint8_value = x.uint8_value ^ y.uint8_value; }
      void visit (const Uint16Type&) { value.uint16_value = x.uint16_value ^ y.uint16_value; }
      void visit (const Uint32Type&) { value.uint32_value = x.uint32_value ^ y.uint32_value; }
      void visit (const Uint64Type&) { value.uint64_value = x.uint64_value ^ y.uint64_value; }
      void visit (const IntType&) { value.int_value = x.int_value ^ y.int_value; }
      void visit (const UintType&) { value.uint_value = x.uint_value ^ y.uint_value; }
      void visit (const UintptrType&) { value.uintptr_value = x.uintptr_value ^ y.uintptr_value; }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    if (is_error () || y.is_error ())
      {
        return make_error ();
      }

    if (!(is_integer () && y.is_integer ()))
      {
        return make_error ();
      }

    Value xx (*this);
    Value yy (y);
    if (xx.kind < yy.kind)
      {
        xx = xx.promote (yy);
      }
    else if (yy.kind < xx.kind)
      {
        yy = yy.promote (xx);
      }

    Value v;
    v.kind = xx.kind;
    switch (v.kind)
      {
      case Integer:
        v.Integer_value = xx.Integer_value ^ yy.Integer_value;
        break;
      case Rune:
        v.Rune_value = xx.Rune_value ^ yy.Rune_value;
        break;
      case Typed:
        {
          if (xx.type != yy.type)
            {
              return Value::make_error ();
            }
          v.type = xx.type;
          Visitor vis (xx, yy, v);
          xx.type->underlying_type ()->accept (vis);
        }
        break;
      default:
        abort (); // NOT_COVERED
      }
    return v;
  }

  Value Value::equal (const Value& y) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y)
        : x (a_x)
        , y (a_y)
      { }
      const Value& x;
      const Value& y;
      Value flag;

      void visit (const BoolType&) { flag = make_boolean (x.bool_value == y.bool_value); }
      void visit (const Complex64Type&) { flag = make_boolean (x.complex64_value == y.complex64_value); }
      void visit (const Complex128Type&) { flag = make_boolean (x.complex128_value == y.complex128_value); }
      void visit (const Float32Type&) { flag = make_boolean (x.float32_value == y.float32_value); }
      void visit (const Float64Type&) { flag = make_boolean (x.float64_value == y.float64_value); }
      void visit (const IntType&) { flag = make_boolean (x.int_value == y.int_value); }
      void visit (const Int8Type&) { flag = make_boolean (x.int8_value == y.int8_value); }
      void visit (const Int16Type&) { flag = make_boolean (x.int16_value == y.int16_value); }
      void visit (const Int32Type&) { flag = make_boolean (x.int32_value == y.int32_value); }
      void visit (const Int64Type&) { flag = make_boolean (x.int64_value == y.int64_value); }
      void visit (const UintType&) { flag = make_boolean (x.uint_value == y.uint_value); }
      void visit (const Uint8Type&) { flag = make_boolean (x.uint8_value == y.uint8_value); }
      void visit (const Uint16Type&) { flag = make_boolean (x.uint16_value == y.uint16_value); }
      void visit (const Uint32Type&) { flag = make_boolean (x.uint32_value == y.uint32_value); }
      void visit (const Uint64Type&) { flag = make_boolean (x.uint64_value == y.uint64_value); }
      void visit (const UintptrType&) { flag = make_boolean (x.uintptr_value == y.uintptr_value); }
      void visit (const StringType&) { flag = make_boolean (x.string_value == y.string_value); }
    };

    assert (!is_unknown ());
    assert (!y.is_unknown ());

    if (is_error () || y.is_error ())
      {
        return make_error ();
      }

    Value xx (*this);
    Value yy (y);

    if (xx.kind < yy.kind)
      {
        xx = xx.promote (yy);
      }
    else if (yy.kind < xx.kind)
      {
        yy = yy.promote (xx);
      }

    if (xx.is_error () || yy.is_error ())
      {
        return make_error ();
      }

    switch (xx.kind)
      {
      case Boolean:
        return make_boolean (xx.Boolean_value == yy.Boolean_value);
      case Integer:
        return make_boolean (xx.Integer_value == yy.Integer_value);
      case Rune:
        return make_boolean (xx.Rune_value == yy.Rune_value);
      case Float:
        return make_boolean (xx.Float_value == yy.Float_value);
      case Complex:
        return make_boolean (xx.Complex_value == yy.Complex_value);
      case String:
        return make_boolean (xx.String_value == yy.String_value);
      case Typed:
        {
          if (xx.type != yy.type)
            {
              return Value::make_error ();
            }
          Visitor vis (xx, yy);
          xx.type->underlying_type ()->accept (vis);
          return vis.flag;
        }
        break;
      default:
        abort (); // NOT_COVERED
        break;
      }

    return make_error (); // NOT_COVERED
  }

  bool Value::operator== (const Value& other) const
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (const Value& a_x, const Value& a_y)
        : x (a_x)
        , y (a_y)
        , flag (false)
      { }

      const Value& x;
      const Value& y;
      bool flag;

      void visit (const BoolType&) { flag = x.bool_value == y.bool_value; }
      void visit (const Int8Type&) { flag = x.int8_value == y.int8_value; }
      void visit (const Int16Type&) { flag = x.int16_value == y.int16_value; }
      void visit (const Int32Type&) { flag = x.int32_value == y.int32_value; }
      void visit (const Int64Type&) { flag = x.int64_value == y.int64_value; }
      void visit (const Uint8Type&) { flag = x.uint8_value == y.uint8_value; }
      void visit (const Uint16Type&) { flag = x.uint16_value == y.uint16_value; }
      void visit (const Uint32Type&) { flag = x.uint32_value == y.uint32_value; }
      void visit (const Uint64Type&) { flag = x.uint64_value == y.uint64_value; }
      void visit (const IntType&) { flag = x.int_value == y.int_value; }
      void visit (const UintType&) { flag = x.uint_value == y.uint_value; }
      void visit (const UintptrType&) { flag = x.uintptr_value == y.uintptr_value; }
      void visit (const Float32Type&) { flag = x.float32_value == y.float32_value; }
      void visit (const Float64Type&) { flag = x.float64_value == y.float64_value; }
      void visit (const Complex64Type&) { flag = x.complex64_value == y.complex64_value; }
      void visit (const Complex128Type&) { flag = x.complex128_value == y.complex128_value; }
      void visit (const StringType&) { flag = x.string_value == y.string_value; }
    };

    if (this->kind != other.kind) {
      return false;
    }

    switch (this->kind)
      {
      case Value::Unknown:
      case Value::Error:
        return true;
      case Value::Boolean:
        return this->Boolean_value == other.Boolean_value;
      case Value::Integer:
        return this->Integer_value == other.Integer_value;
      case Value::Float:
        return this->Float_value == other.Float_value;
      case Value::Complex:
        return this->Complex_value == other.Complex_value;
      case Value::Rune:
        return this->Rune_value == other.Rune_value;
      case Value::String:
        return this->string_value == other.string_value;
      case Value::Typed:
        {
          if (this->type != other.type)
            {
              return false;
            }
          Visitor visitor (*this, other);
          this->type->underlying_type ()->accept (visitor);
          return visitor.flag;
        }
      }

    abort (); // NOT_COVERED
  }

  std::ostream& operator<< (std::ostream& out, const Value& value)
  {
    struct Visitor : public DefaultTypeVisitor {
      Visitor (std::ostream& a_out, const Value& a_value)
        : out (a_out)
        , value (a_value)
      { }

      std::ostream& out;
      const Value& value;

      void visit (const BoolType&) { out << (value.bool_value ? "true" : "false"); }
      void visit (const Int8Type&) { out << static_cast<int> (value.int8_value); }
      void visit (const Int16Type&) { out << value.int16_value; }
      void visit (const Int32Type&) { out << value.int32_value; }
      void visit (const Int64Type&) { out << value.int64_value; }
      void visit (const Uint8Type&) { out << static_cast<int> (value.uint8_value); }
      void visit (const Uint16Type&) { out << value.uint16_value; }
      void visit (const Uint32Type&) { out << value.uint32_value; }
      void visit (const Uint64Type&) { out << value.uint64_value; }
      void visit (const IntType&) { out << value.int_value; }
      void visit (const UintType&) { out << value.uint_value; }
      void visit (const UintptrType&) { out << value.uintptr_value; }
      void visit (const Float32Type&) { out << value.float32_value; }
      void visit (const Float64Type&) { out << value.float64_value; }
      void visit (const Complex64Type&) { out << value.complex64_value; }
      void visit (const Complex128Type&) { out << value.complex128_value; }
      void visit (const StringType&) { out << '"' << value.string_value << '"'; }
    };

    switch (value.kind)
      {
      case Value::Unknown:
        out << "unknown";
        break;
      case Value::Error:
        out << "error";
        break;
      case Value::Boolean:
        out << (value.Boolean_value ? "true" : "false");
        break;
      case Value::Integer:
        out << value.Integer_value;
        break;
      case Value::Rune:
        if (value.Rune_value < INVALID &&
            (value.Rune_value < SURROGATE_FIRST ||
             value.Rune_value > SURROGATE_LAST))
          {
            std::string s;
            append (s, value.Rune_value.get_ui ());
            out << "'" << s << "'";
          }
        else
          {
            out << value.Rune_value;
          }
        break;
      case Value::Float:
        out << value.Float_value;
        break;
      case Value::Complex:
        out << value.Complex_value;
        break;
      case Value::String:
        out << '"' << value.String_value << '"';
        break;
      case Value::Typed:
        {
          Visitor v (out, value);
          value.type->underlying_type ()->accept (v);
        }
        break;
      }
    return out;
  }

}
