// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/value.h"

#include <cassert>
#include <iostream>

#include "src/type.h"
#include "src/utf8.h"

namespace rcgo {

complex64_t::complex64_t() : real(0), imag(0) {}

complex64_t::complex64_t(float a_real, float a_imag)
    : real(a_real), imag(a_imag) {}

complex64_t complex64_t::operator+(const complex64_t& a_other) const {
  return complex64_t(real + a_other.real, imag + a_other.imag);
}

complex64_t complex64_t::operator-(const complex64_t& a_other) const {
  return complex64_t(real - a_other.real, imag - a_other.imag);
}

complex64_t complex64_t::operator*(const complex64_t& a_other) const {
  return complex64_t(real * a_other.real - imag * a_other.imag,
                     real * a_other.imag + imag * a_other.real);
}

complex64_t complex64_t::operator/(const complex64_t& a_other) const {
  float denom = a_other.real * a_other.real + a_other.imag * a_other.imag;
  return complex64_t((real * a_other.real + imag * a_other.real) / denom,
                     (imag * a_other.real - real * a_other.imag) / denom);
}

complex64_t complex64_t::operator+() const {
  return complex64_t(+real, +imag);
}

complex64_t complex64_t::operator-() const {
  return complex64_t(-real, -imag);
}

bool complex64_t::operator==(const complex64_t& a_other) const {
  return this->real == a_other.real && this->imag == a_other.imag;
}

std::ostream& operator<<(std::ostream& out, const complex64_t& val) {
  out << val.real <<(val.imag >= 0 ? "+" : "") << val.imag << 'i';
  return out;
}

complex128_t::complex128_t() : real(0), imag(0) {}

complex128_t::complex128_t(double a_real, double a_imag)
    : real(a_real), imag(a_imag) {}

complex128_t complex128_t::operator+(const complex128_t& a_other) const {
  return complex128_t(real + a_other.real, imag + a_other.imag);
}

complex128_t complex128_t::operator-(const complex128_t& a_other) const {
  return complex128_t(real - a_other.real, imag - a_other.imag);
}

complex128_t complex128_t::operator*(const complex128_t& a_other) const {
  return complex128_t(real * a_other.real - imag * a_other.imag,
                      real * a_other.imag + imag * a_other.real);
}

complex128_t complex128_t::operator/(const complex128_t& a_other) const {
  float denom = a_other.real * a_other.real + a_other.imag * a_other.imag;
  return complex128_t((real * a_other.real + imag * a_other.real) / denom,
                      (imag * a_other.real - real * a_other.imag) / denom);
}

complex128_t complex128_t::operator+() const {
  return complex128_t(+real, +imag);
}

complex128_t complex128_t::operator-() const {
  return complex128_t(-real, -imag);
}

bool complex128_t::operator==(const complex128_t& a_other) const {
  return this->real == a_other.real && this->imag == a_other.imag;
}

std::ostream& operator<<(std::ostream& out, const complex128_t& val) {
  out << val.real <<(val.imag >= 0 ? "+" : "") << val.imag << 'i';
  return out;
}

complex_t::complex_t() : real(0, PRECISION), imag(0, PRECISION) {}

complex_t::complex_t(const mpf_class& a_real, const mpf_class& a_imag)
    : real(a_real), imag(a_imag) {}

bool complex_t::operator==(const complex_t& a_other) const {
  return this->real == a_other.real && this->imag == a_other.imag;
}

complex_t complex_t::operator+(const complex_t& a_other) const {
  return complex_t(real + a_other.real, imag + a_other.imag);
}

complex_t complex_t::operator-(const complex_t& a_other) const {
  return complex_t(real - a_other.real, imag - a_other.imag);
}

complex_t complex_t::operator*(const complex_t& a_other) const {
  return complex_t(real * a_other.real - imag * a_other.imag,
                   real * a_other.imag + imag * a_other.real);
}

complex_t complex_t::operator/(const complex_t& a_other) const {
  mpf_class denom = a_other.real * a_other.real + a_other.imag * a_other.imag;
  return complex_t((real * a_other.real + imag * a_other.real) / denom,
                   (imag * a_other.real - real * a_other.imag) / denom);
}

complex_t complex_t::operator+() const {
  return complex_t(+real, +imag);
}

complex_t complex_t::operator-() const {
  return complex_t(-real, -imag);
}

std::ostream& operator<<(std::ostream& out, const complex_t& val) {
  out << val.real <<(val.imag >= 0 ? "+" : "") << val.imag << 'i';
  return out;
}

Value::Value() : kind(kUnknown) {}

Value::~Value() {}

Value Value::MakeError() {
  Value v;
  v.kind = kError;
  return v;
}

Value Value::MakeBoolean(bool a_value) {
  Value v;
  v.kind = kBoolean;
  v.Boolean_value = a_value;
  return v;
}

Value Value::MakeInteger(const mpz_class& a_value) {
  Value v;
  v.kind = kInteger;
  v.Integer_value = a_value;
  return v;
}

Value Value::MakeFloat(const mpf_class& a_value) {
  Value v;
  v.kind = kFloat;
  v.Float_value = a_value;
  return v;
}

Value Value::MakeComplex(const mpf_class& a_real, const mpf_class& a_imag) {
  Value v;
  v.kind = kComplex;
  v.Complex_value = complex_t(a_real, a_imag);
  return v;
}

Value Value::MakeRune(const mpz_class& a_value) {
  Value v;
  v.kind = kRune;
  v.Rune_value = a_value;
  return v;
}

Value Value::MakeString(const std::string& a_value) {
  Value v;
  v.kind = kString;
  v.String_value = a_value;
  return v;
}

bool Value::IsUnknown() const {
  return kind == kUnknown;
}

bool Value::IsError() const {
  return kind == kError;
}

bool Value::IsArithmetic() const {
  assert(!IsUnknown());
  assert(!IsError());

  switch (kind) {
    case kInteger:
    case kRune:
    case kFloat:
    case kComplex:
      return true;
    case kTyped:
      return type::IsArithmetic(type);
    default:
      return false;
  }
}

bool Value::IsSigned() const {
  assert(!IsUnknown());
  assert(!IsError());

  switch (kind) {
    case kInteger:
    case kRune:
    case kFloat:
    case kComplex:
      return true;
    case kTyped:
      return type::IsSigned(type);
    default:
      return false;
  }
}

bool Value::IsInteger() const {
  assert(!IsUnknown());
  assert(!IsError());

  switch (kind) {
    case kInteger:
    case kRune:
      return true;
    case kTyped:
      return type::IsInteger(type);
    default:
      return false;
  }
}

bool Value::IsZero() const {
  struct Visitor : public type::DefaultVisitor {
    explicit Visitor(const Value& a_x) : x(a_x), flag(false) {}
    const Value& x;
    bool flag;

    void Visit(const type::Int8&) override      { flag = x.int8_value == 0; }
    void Visit(const type::Int16&) override     { flag = x.int16_value == 0; }
    void Visit(const type::Int32&) override     { flag = x.int32_value == 0; }
    void Visit(const type::Int64&) override     { flag = x.int64_value == 0; }
    void Visit(const type::Uint8&) override     { flag = x.uint8_value == 0; }
    void Visit(const type::Uint16&) override    { flag = x.uint16_value == 0; }
    void Visit(const type::Uint32&) override    { flag = x.uint32_value == 0; }
    void Visit(const type::Uint64&) override    { flag = x.uint64_value == 0; }
    void Visit(const type::Int&) override       { flag = x.int_value == 0; }
    void Visit(const type::Uint&) override      { flag = x.uint_value == 0; }
    void Visit(const type::Uintptr&) override   { flag = x.uintptr_value == 0; }
    void Visit(const type::Float32&) override   { flag = x.float32_value == 0; }
    void Visit(const type::Float64&) override   { flag = x.float64_value == 0; }
    void Visit(const type::Complex64&) override {
      flag = x.complex64_value == complex64_t(0, 0);
    }
    void Visit(const type::Complex128&) override {
      flag = x.complex128_value == complex128_t(0, 0);
    }
  };

  assert(IsArithmetic());

  switch (kind) {
    case kInteger:
      return Integer_value == 0;
    case kRune:
      return Rune_value == 0;
    case kFloat:
      return Float_value == 0;
    case kComplex:
      return Complex_value == complex_t(0, 0);
    case kTyped:
      {
        Visitor vis(*this);
        type->UnderlyingType()->Accept(&vis);
        return vis.flag;
      }
    default:
      abort();  // NOT_COVERED
  }
}

bool Value::IsString() const {
  assert(!IsUnknown());
  assert(!IsError());

  switch (kind) {
    case kString:
      return true;
    case kTyped:
      return type::IsString(type);
    default:
      return false;
  }
}

bool Value::IsBoolean() const {
  assert(!IsUnknown());
  assert(!IsError());

  switch (kind) {
    case kBoolean:
      return true;
    case kTyped:
      return type::IsBoolean(type);
    default:
      return false;
  }
}

namespace {

struct ConvertVisitor : public type::DefaultVisitor {
  const Value& value;
  Value new_value;

  explicit ConvertVisitor(const Value& a_value)
      : value(a_value)
      , new_value(Value::MakeError()) {}

  void Visit(const type::Bool& type) override {
    if (value.kind == Value::kBoolean) {
      new_value.kind = Value::kTyped;
      new_value.bool_value = value.Boolean_value;
    }
  }

  void Visit(const type::String& type) override {
    if (value.kind == Value::kString) {
      new_value.kind = Value::kTyped;
      new_value.string_value = value.String_value;
    }
  }

  template <typename T>
  static bool exact_check(const mpz_class& from, T to) {
    return mpz_class(to) == from;
  }

  template <typename T>
  static bool close_check(const mpf_class& from, T to) {
    if (from == 0) {
      return from == to;
    }
    mpf_class q = from / mpf_class(to);
    return q > .5 && q < 1.5;
  }

#define CONVERT_TO_COMPLEX(Type, ValueMember, ValueType) \
  void Visit(const Type& type) override                                 \
  {                                                                     \
    switch (value.kind)                                                 \
    {                                                                   \
      case Value::kInteger:                                             \
        new_value.ValueMember = ValueType(value.Integer_value.get_d(), 0); \
        if (exact_check(value.Integer_value, new_value.ValueMember.real)) \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kFloat:                                               \
        new_value.ValueMember = ValueType(value.Float_value.get_d(), 0); \
        if (close_check(value.Float_value, new_value.ValueMember.real)) \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kComplex:                                             \
        new_value.ValueMember =                                         \
            ValueType(value.Complex_value.real.get_d(),                 \
                      value.Complex_value.imag.get_d());                \
        if (close_check(value.Complex_value.real,                       \
                        new_value.ValueMember.real) &&                  \
            close_check(value.Complex_value.imag, new_value.ValueMember.imag)) \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kRune:                                                \
        new_value.ValueMember = ValueType(value.Rune_value.get_d(), 0); \
        if (exact_check(value.Rune_value, new_value.ValueMember.real))  \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      default:                                                          \
        break;                                                          \
    }                                                                   \
  }

  CONVERT_TO_COMPLEX(type::Complex64, complex64_value, complex64_t)
  CONVERT_TO_COMPLEX(type::Complex128, complex128_value, complex128_t)

#define CONVERT_TO_FLOAT(Type, ValueMember)                             \
  void Visit(const Type& type) override                                 \
  {                                                                     \
    switch (value.kind)                                                 \
    {                                                                   \
      case Value::kInteger:                                             \
        new_value.ValueMember = value.Integer_value.get_d();            \
        if (exact_check(value.Integer_value, new_value.ValueMember))    \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kFloat:                                               \
        new_value.ValueMember = value.Float_value.get_d();              \
        if (close_check(value.Float_value, new_value.ValueMember))      \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kComplex:                                             \
        new_value.ValueMember = value.Complex_value.real.get_d();       \
        if (close_check(value.Complex_value.real, new_value.ValueMember) && \
           value.Complex_value.imag == 0)                               \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kRune:                                                \
        new_value.ValueMember = value.Rune_value.get_d();               \
        if (exact_check(value.Rune_value, new_value.ValueMember))       \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      default:                                                          \
        break;                                                          \
    }                                                                   \
  }

  CONVERT_TO_FLOAT(type::Float32, float32_value)
  CONVERT_TO_FLOAT(type::Float64, float64_value)

#define CONVERT_TO_INT(Type, ValueMember)                               \
  void Visit(const Type& type) override                                 \
  {                                                                     \
    switch (value.kind)                                                 \
    {                                                                   \
      case Value::kInteger:                                             \
        new_value.ValueMember = value.Integer_value.get_si();           \
        if (exact_check(value.Integer_value, new_value.ValueMember))    \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kFloat:                                               \
        new_value.ValueMember = value.Float_value.get_si();             \
        if (close_check(value.Float_value, new_value.ValueMember))      \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kComplex:                                             \
        new_value.ValueMember = value.Complex_value.real.get_si();      \
        if (close_check(value.Complex_value.real, new_value.ValueMember) && \
           value.Complex_value.imag == 0)                               \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kRune:                                                \
        new_value.ValueMember = value.Rune_value.get_si();              \
        if (exact_check(value.Rune_value, new_value.ValueMember))        \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      default:                                                          \
        break;                                                          \
    }                                                                   \
  }

  CONVERT_TO_INT(type::Int, int_value)
  CONVERT_TO_INT(type::Int8, int8_value)
  CONVERT_TO_INT(type::Int16, int16_value)
  CONVERT_TO_INT(type::Int32, int32_value)
  CONVERT_TO_INT(type::Int64, int64_value)

#define CONVERT_TO_UINT(Type, ValueMember)                              \
  void Visit(const Type& type) override                                 \
  {                                                                     \
    switch (value.kind)                                                 \
    {                                                                   \
      case Value::kInteger:                                             \
        new_value.ValueMember = value.Integer_value.get_ui();           \
        if (exact_check(value.Integer_value, new_value.ValueMember))    \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kFloat:                                               \
        new_value.ValueMember = value.Float_value.get_ui();             \
        if (close_check(value.Float_value, new_value.ValueMember))      \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kComplex:                                             \
        new_value.ValueMember = value.Complex_value.real.get_ui();      \
        if (close_check(value.Complex_value.real, new_value.ValueMember) && \
           value.Complex_value.imag == 0)                               \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      case Value::kRune:                                                \
        new_value.ValueMember = value.Rune_value.get_ui();              \
        if (exact_check(value.Rune_value, new_value.ValueMember))       \
        {                                                               \
          new_value.kind = Value::kTyped;                               \
        }                                                               \
        break;                                                          \
      default:                                                          \
        break;                                                          \
    }                                                                   \
  }

  CONVERT_TO_UINT(type::Uint, uint_value)
  CONVERT_TO_UINT(type::Uint8, uint8_value)
  CONVERT_TO_UINT(type::Uint16, uint16_value)
  CONVERT_TO_UINT(type::Uint32, uint32_value)
  CONVERT_TO_UINT(type::Uint64, uint64_value)
  CONVERT_TO_UINT(type::Uintptr, uintptr_value)
};

}  // namespace

Value Value::ConvertTo(const type::Type* type) const {
  assert(!IsUnknown());

  if (IsError()) {
    return MakeError();
  }

  ConvertVisitor visitor(*this);
  type->UnderlyingType()->Accept(&visitor);
  if (!visitor.new_value.IsError()) {
    visitor.new_value.type = type;
  }
  return visitor.new_value;
}

Value Value::PromoteTo(const Value& other) const {
  assert(!IsUnknown());
  assert(!other.IsUnknown());
  assert(other.kind >= kind);

  if (kind == other.kind) {
    return *this;
  }

  if (other.kind == kTyped) {
    return ConvertTo(other.type);
  }

  switch (kind) {
    case kBoolean:
      return MakeError();
    case kString:
      return MakeError();
    case kInteger:
      return MakeRune(Integer_value).PromoteTo(other);
    case kRune:
      return MakeFloat(Rune_value).PromoteTo(other);
    case kFloat:
      return MakeComplex(Float_value, 0).PromoteTo(other);

    default:
      abort();  // NOT_COVERED
  }
}

Value Value::ToInteger() const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, Value* a_value) : x(a_x), value(a_value) {}
    const Value& x;
    Value* value;

    void Visit(const type::Int8&) override {
      *value = MakeInteger(x.int8_value);
    }
    void Visit(const type::Int16&) override {
      *value = MakeInteger(x.int16_value);
    }
    void Visit(const type::Int32&) override {
      *value = MakeInteger(x.int32_value);
    }
    void Visit(const type::Int64&) override {
      *value = MakeInteger(x.int64_value);
    }
    void Visit(const type::Uint8&) override {
      *value = MakeInteger(x.uint8_value);
    }
    void Visit(const type::Uint16&) override {
      *value = MakeInteger(x.uint16_value);
    }
    void Visit(const type::Uint32&) override {
      *value = MakeInteger(x.uint32_value);
    }
    void Visit(const type::Uint64&) override {
      *value = MakeInteger(x.uint64_value);
    }
    void Visit(const type::Int&) override {
      *value = MakeInteger(x.int_value);
    }
    void Visit(const type::Uint&) override {
      *value = MakeInteger(x.uint_value);
    }
    void Visit(const type::Uintptr&) override {
      *value = MakeInteger(x.uintptr_value);
    }
  };

  assert(!IsUnknown());

  switch (kind) {
    case kInteger:
      return *this;
    case kRune:
      return MakeInteger(Rune_value);
    case kFloat:
      {
        mpz_class x(Float_value);
        if (Float_value == mpf_class(x)) {
          return MakeInteger(x);
        } else {
          return MakeError();
        }
      }
      break;
    case kComplex:
      {
        mpz_class x(Complex_value.real);
        if (Complex_value == complex_t(x, 0)) {
          return MakeInteger(x);
        } else {
          return MakeError();
        }
      }
      break;
    case kTyped:
      if (type::IsInteger(type)) {
        Value v;
        Visitor vis(*this, &v);
        type->UnderlyingType()->Accept(&vis);
        return v;
      } else {
        return MakeError();
      }
      break;
    default:
      return MakeError();
  }
}

// TODO(jrw972):  Handle overflow and underflow.

Value Value::Posate() const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, Value* a_value) : x(a_x), value(a_value) {}
    const Value& x;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = +x.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = +x.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = +x.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = +x.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = +x.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = +x.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = +x.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = +x.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = +x.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = +x.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = +x.uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->float32_value = +x.float32_value;
    }
    void Visit(const type::Float64&) override {
      value->float64_value = +x.float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->complex64_value = +x.complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->complex128_value = +x.complex128_value;
    }
  };

  assert(!IsUnknown());

  if (IsError()) {
    return MakeError();
  }

  if (!IsArithmetic()) {
    return MakeError();
  }

  Value v;
  v.kind = this->kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = +Integer_value;
      break;
    case kRune:
      v.Rune_value = +Rune_value;
      break;
    case kFloat:
      v.Float_value = +Float_value;
      break;
    case kComplex:
      v.Complex_value = +Complex_value;
      break;
    case kTyped:
      {
        v.type = this->type;
        Visitor vis(*this, &v);
        this->type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::Negate() const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, Value* a_value) : x(a_x), value(a_value) {}
    const Value& x;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = -x.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = -x.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = -x.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = -x.int64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = -x.int_value;
    }
    void Visit(const type::Float32&) override {
      value->float32_value = -x.float32_value;
    }
    void Visit(const type::Float64&) override {
      value->float64_value = -x.float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->complex64_value = -x.complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->complex128_value = -x.complex128_value;
    }
  };

  assert(!IsUnknown());

  if (IsError()) {
    return MakeError();
  }

  if (!IsSigned()) {
    return MakeError();
  }

  Value v;
  v.kind = this->kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = -Integer_value;
      break;
    case kRune:
      v.Rune_value = -Rune_value;
      break;
    case kFloat:
      v.Float_value = -Float_value;
      break;
    case kComplex:
      v.Complex_value = -Complex_value;
      break;
    case kTyped:
      {
        v.type = this->type;
        Visitor vis(*this, &v);
        this->type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::LogicNot() const {
  assert(!IsUnknown());

  if (IsError()) {
    return MakeError();
  }

  if (!IsBoolean()) {
    return MakeError();
  }

  Value v;
  v.kind = this->kind;
  switch (v.kind) {
    case kBoolean:
      v.Boolean_value = !Boolean_value;
      break;
    case kTyped:
      v.bool_value = !bool_value;
      v.type = type;
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::BitNot() const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, Value* a_value) : x(a_x), value(a_value) {}
    const Value& x;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = ~x.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = ~x.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = ~x.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = ~x.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = ~x.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = ~x.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = ~x.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = ~x.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = ~x.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = ~x.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = ~x.uintptr_value;
    }
  };

  assert(!IsUnknown());

  if (IsError()) {
    return MakeError();
  }

  if (!IsInteger()) {
    return MakeError();
  }

  Value v;
  v.kind = this->kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = ~Integer_value;
      break;
    case kRune:
      v.Rune_value = ~Rune_value;
      break;
    case kTyped:
      {
        v.type = this->type;
        Visitor vis(*this, &v);
        this->type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::Add(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y, Value* a_value)
        : x(a_x), y(a_y), value(a_value) {}
    const Value& x;
    const Value& y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value + y.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value + y.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value + y.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value + y.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value + y.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value + y.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value + y.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value + y.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value + y.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value + y.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value + y.uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->float32_value = x.float32_value + y.float32_value;
    }
    void Visit(const type::Float64&) override {
      value->float64_value = x.float64_value + y.float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->complex64_value = x.complex64_value + y.complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->complex128_value = x.complex128_value + y.complex128_value;
    }
    void Visit(const type::String&) override {
      value->string_value = x.string_value + y.string_value;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  if (IsError() || y.IsError()) {
    return MakeError();
  }

  if (!((IsArithmetic() && y.IsArithmetic()) ||
       (IsString() && y.IsString()))) {
    return MakeError();
  }

  Value xx(*this);
  Value yy(y);
  if (xx.kind < yy.kind) {
    xx = xx.PromoteTo(yy);
  } else if (yy.kind < xx.kind) {
    yy = yy.PromoteTo(xx);
  }

  Value v;
  v.kind = xx.kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = xx.Integer_value + yy.Integer_value;
      break;
    case kRune:
      v.Rune_value = xx.Rune_value + yy.Rune_value;
      break;
    case kFloat:
      v.Float_value = xx.Float_value + yy.Float_value;
      break;
    case kComplex:
      v.Complex_value = xx.Complex_value + yy.Complex_value;
      break;
    case kString:
      v.String_value = xx.String_value + yy.String_value;
      break;
    case kTyped:
      {
        if (xx.type != yy.type) {
          return Value::MakeError();
        }
        v.type = xx.type;
        Visitor vis(xx, yy, &v);
        xx.type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::Subtract(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y, Value* a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    const Value& x;
    const Value& y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value - y.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value - y.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value - y.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value - y.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value - y.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value - y.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value - y.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value - y.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value - y.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value - y.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value - y.uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->float32_value = x.float32_value - y.float32_value;
    }
    void Visit(const type::Float64&) override {
      value->float64_value = x.float64_value - y.float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->complex64_value = x.complex64_value - y.complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->complex128_value = x.complex128_value - y.complex128_value;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  if (IsError() || y.IsError()) {
    return MakeError();
  }

  if (!(IsArithmetic() && y.IsArithmetic())) {
    return MakeError();
  }

  Value xx(*this);
  Value yy(y);
  if (xx.kind < yy.kind) {
    xx = xx.PromoteTo(yy);
  } else if (yy.kind < xx.kind) {
    yy = yy.PromoteTo(xx);
  }

  Value v;
  v.kind = xx.kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = xx.Integer_value - yy.Integer_value;
      break;
    case kRune:
      v.Rune_value = xx.Rune_value - yy.Rune_value;
      break;
    case kFloat:
      v.Float_value = xx.Float_value - yy.Float_value;
      break;
    case kComplex:
      v.Complex_value = xx.Complex_value - yy.Complex_value;
      break;
    case kTyped:
      {
        if (xx.type != yy.type) {
          return Value::MakeError();
        }
        v.type = xx.type;
        Visitor vis(xx, yy, &v);
        xx.type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::Multiply(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y, Value* a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    const Value& x;
    const Value& y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value * y.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value * y.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value * y.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value * y.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value * y.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value * y.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value * y.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value * y.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value * y.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value * y.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value * y.uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->float32_value = x.float32_value * y.float32_value;
    }
    void Visit(const type::Float64&) override {
      value->float64_value = x.float64_value * y.float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->complex64_value = x.complex64_value * y.complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->complex128_value = x.complex128_value * y.complex128_value;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  if (IsError() || y.IsError()) {
    return MakeError();
  }

  if (!(IsArithmetic() && y.IsArithmetic())) {
    return MakeError();
  }

  Value xx(*this);
  Value yy(y);
  if (xx.kind < yy.kind) {
    xx = xx.PromoteTo(yy);
  } else if (yy.kind < xx.kind) {
    yy = yy.PromoteTo(xx);
  }

  Value v;
  v.kind = xx.kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = xx.Integer_value * yy.Integer_value;
      break;
    case kRune:
      v.Rune_value = xx.Rune_value * yy.Rune_value;
      break;
    case kFloat:
      v.Float_value = xx.Float_value * yy.Float_value;
      break;
    case kComplex:
      v.Complex_value = xx.Complex_value * yy.Complex_value;
      break;
    case kTyped:
      {
        if (xx.type != yy.type) {
          return Value::MakeError();
        }
        v.type = xx.type;
        Visitor vis(xx, yy, &v);
        xx.type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::Divide(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y, Value* a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    const Value& x;
    const Value& y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value / y.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value / y.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value / y.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value / y.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value / y.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value / y.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value / y.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value / y.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value / y.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value / y.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value / y.uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->float32_value = x.float32_value / y.float32_value;
    }
    void Visit(const type::Float64&) override {
      value->float64_value = x.float64_value / y.float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->complex64_value = x.complex64_value / y.complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->complex128_value = x.complex128_value / y.complex128_value;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  if (IsError() || y.IsError()) {
    return MakeError();
  }

  if (!(IsArithmetic() && y.IsArithmetic())) {
    return MakeError();
  }

  if (y.IsZero()) {
    return MakeError();
  }

  Value xx(*this);
  Value yy(y);
  if (xx.kind < yy.kind) {
    xx = xx.PromoteTo(yy);
  } else if (yy.kind < xx.kind) {
    yy = yy.PromoteTo(xx);
  }

  Value v;
  v.kind = xx.kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = xx.Integer_value / yy.Integer_value;
      break;
    case kRune:
      v.Rune_value = xx.Rune_value / yy.Rune_value;
      break;
    case kFloat:
      v.Float_value = xx.Float_value / yy.Float_value;
      break;
    case kComplex:
      v.Complex_value = xx.Complex_value / yy.Complex_value;
      break;
    case kTyped:
      {
        if (xx.type != yy.type) {
          return Value::MakeError();
        }
        v.type = xx.type;
        Visitor vis(xx, yy, &v);
        xx.type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::Modulo(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y, Value* a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    const Value& x;
    const Value& y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value % y.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value % y.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value % y.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value % y.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value % y.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value % y.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value % y.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value % y.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value % y.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value % y.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value % y.uintptr_value;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  if (IsError() || y.IsError()) {
    return MakeError();
  }

  if (!(IsInteger() && y.IsInteger())) {
    return MakeError();
  }

  if (y.IsZero()) {
    return MakeError();
  }

  Value xx(*this);
  Value yy(y);
  if (xx.kind < yy.kind) {
    xx = xx.PromoteTo(yy);
  } else if (yy.kind < xx.kind) {
    yy = yy.PromoteTo(xx);
  }

  Value v;
  v.kind = xx.kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = xx.Integer_value % yy.Integer_value;
      break;
    case kRune:
      v.Rune_value = xx.Rune_value % yy.Rune_value;
      break;
    case kTyped:
      {
        if (xx.type != yy.type) {
          return Value::MakeError();
        }
        v.type = xx.type;
        Visitor vis(xx, yy, &v);
        xx.type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::LeftShift(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, unsigned int a_y, Value* a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    const Value& x;
    unsigned int y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value << y;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value << y;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value << y;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value << y;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value << y;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value << y;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value << y;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value << y;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value << y;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value << y;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value << y;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  const Value yy = y.ToInteger().ConvertTo(&type::Uint::instance);
  {
    const Value xx = ToInteger();
    if (xx.IsError() || yy.IsError()) {
      return MakeError();
    }
  }

  Value v;
  switch (this->kind) {
    case kInteger:
      v = MakeInteger(Integer_value << yy.uint_value);
      break;
    case kRune:
      v = MakeInteger(Rune_value << yy.uint_value);
      break;
    case kFloat:
      v = MakeInteger(mpz_class(Float_value) << yy.uint_value);
      break;
    case kComplex:
      v = MakeInteger(mpz_class(Complex_value.real) << yy.uint_value);
      break;
    case kTyped:
      {
        v.kind = kTyped;
        v.type = this->type;
        Visitor vis(*this, yy.uint_value, &v);
        this->type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::RightShift(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, unsigned int a_y, Value* a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    const Value& x;
    unsigned int y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value >> y;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value >> y;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value >> y;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value >> y;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value >> y;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value >> y;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value >> y;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value >> y;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value >> y;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value >> y;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value >> y;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  const Value yy = y.ToInteger().ConvertTo(&type::Uint::instance);
  {
    const Value xx = ToInteger();
    if (xx.IsError() || yy.IsError()) {
      return MakeError();
    }
  }

  Value v;
  switch (this->kind) {
    case kInteger:
      v = MakeInteger(Integer_value >> yy.uint_value);
      break;
    case kRune:
      v = MakeInteger(Rune_value >> yy.uint_value);
      break;
    case kFloat:
      v = MakeInteger(mpz_class(Float_value) >> yy.uint_value);
      break;
    case kComplex:
      v = MakeInteger(mpz_class(Complex_value.real) >> yy.uint_value);
      break;
    case kTyped:
      {
        v.kind = kTyped;
        v.type = this->type;
        Visitor vis(*this, yy.uint_value, &v);
        this->type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::BitAnd(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y, Value* a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    const Value& x;
    const Value& y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value & y.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value & y.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value & y.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value & y.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value & y.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value & y.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value & y.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value & y.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value & y.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value & y.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value & y.uintptr_value;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  if (IsError() || y.IsError()) {
    return MakeError();
  }

  if (!(IsInteger() && y.IsInteger())) {
    return MakeError();
  }

  Value xx(*this);
  Value yy(y);
  if (xx.kind < yy.kind) {
    xx = xx.PromoteTo(yy);
  } else if (yy.kind < xx.kind) {
    yy = yy.PromoteTo(xx);
  }

  Value v;
  v.kind = xx.kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = xx.Integer_value & yy.Integer_value;
      break;
    case kRune:
      v.Rune_value = xx.Rune_value & yy.Rune_value;
      break;
    case kTyped:
      {
        if (xx.type != yy.type) {
          return Value::MakeError();
        }
        v.type = xx.type;
        Visitor vis(xx, yy, &v);
        xx.type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::BitAndNot(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y, Value* a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    const Value& x;
    const Value& y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value & ~y.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value & ~y.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value & ~y.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value & ~y.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value & ~y.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value & ~y.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value & ~y.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value & ~y.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value & ~y.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value & ~y.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value & ~y.uintptr_value;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  if (IsError() || y.IsError()) {
    return MakeError();
  }

  if (!(IsInteger() && y.IsInteger())) {
    return MakeError();
  }

  Value xx(*this);
  Value yy(y);
  if (xx.kind < yy.kind) {
    xx = xx.PromoteTo(yy);
  } else if (yy.kind < xx.kind) {
    yy = yy.PromoteTo(xx);
  }

  Value v;
  v.kind = xx.kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = xx.Integer_value & ~yy.Integer_value;
      break;
    case kRune:
      v.Rune_value = xx.Rune_value & ~yy.Rune_value;
      break;
    case kTyped:
      {
        if (xx.type != yy.type) {
          return Value::MakeError();
        }
        v.type = xx.type;
        Visitor vis(xx, yy, &v);
        xx.type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::BitOr(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y, Value* a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    const Value& x;
    const Value& y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value | y.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value | y.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value | y.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value | y.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value | y.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value | y.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value | y.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value | y.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value | y.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value | y.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value | y.uintptr_value;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  if (IsError() || y.IsError()) {
    return MakeError();
  }

  if (!(IsInteger() && y.IsInteger())) {
    return MakeError();
  }

  Value xx(*this);
  Value yy(y);
  if (xx.kind < yy.kind) {
    xx = xx.PromoteTo(yy);
  } else if (yy.kind < xx.kind) {
    yy = yy.PromoteTo(xx);
  }

  Value v;
  v.kind = xx.kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = xx.Integer_value | yy.Integer_value;
      break;
    case kRune:
      v.Rune_value = xx.Rune_value | yy.Rune_value;
      break;
    case kTyped:
      {
        if (xx.type != yy.type) {
          return Value::MakeError();
        }
        v.type = xx.type;
        Visitor vis(xx, yy, &v);
        xx.type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::BitXor(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y, Value* a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    const Value& x;
    const Value& y;
    Value* value;

    void Visit(const type::Int8&) override {
      value->int8_value = x.int8_value ^ y.int8_value;
    }
    void Visit(const type::Int16&) override {
      value->int16_value = x.int16_value ^ y.int16_value;
    }
    void Visit(const type::Int32&) override {
      value->int32_value = x.int32_value ^ y.int32_value;
    }
    void Visit(const type::Int64&) override {
      value->int64_value = x.int64_value ^ y.int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->uint8_value = x.uint8_value ^ y.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->uint16_value = x.uint16_value ^ y.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->uint32_value = x.uint32_value ^ y.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->uint64_value = x.uint64_value ^ y.uint64_value;
    }
    void Visit(const type::Int&) override {
      value->int_value = x.int_value ^ y.int_value;
    }
    void Visit(const type::Uint&) override {
      value->uint_value = x.uint_value ^ y.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->uintptr_value = x.uintptr_value ^ y.uintptr_value;
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  if (IsError() || y.IsError()) {
    return MakeError();
  }

  if (!(IsInteger() && y.IsInteger())) {
    return MakeError();
  }

  Value xx(*this);
  Value yy(y);
  if (xx.kind < yy.kind) {
    xx = xx.PromoteTo(yy);
  } else if (yy.kind < xx.kind) {
    yy = yy.PromoteTo(xx);
  }

  Value v;
  v.kind = xx.kind;
  switch (v.kind) {
    case kInteger:
      v.Integer_value = xx.Integer_value ^ yy.Integer_value;
      break;
    case kRune:
      v.Rune_value = xx.Rune_value ^ yy.Rune_value;
      break;
    case kTyped:
      {
        if (xx.type != yy.type) {
          return Value::MakeError();
        }
        v.type = xx.type;
        Visitor vis(xx, yy, &v);
        xx.type->UnderlyingType()->Accept(&vis);
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::Equal(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y)
        : x(a_x)
        , y(a_y) {}
    const Value& x;
    const Value& y;
    Value flag;

    void Visit(const type::Bool&) override {
      flag = MakeBoolean(x.bool_value == y.bool_value);
    }
    void Visit(const type::Complex64&) override {
      flag = MakeBoolean(x.complex64_value == y.complex64_value);
    }
    void Visit(const type::Complex128&) override {
      flag = MakeBoolean(x.complex128_value == y.complex128_value);
    }
    void Visit(const type::Float32&) override {
      flag = MakeBoolean(x.float32_value == y.float32_value);
    }
    void Visit(const type::Float64&) override {
      flag = MakeBoolean(x.float64_value == y.float64_value);
    }
    void Visit(const type::Int&) override {
      flag = MakeBoolean(x.int_value == y.int_value);
    }
    void Visit(const type::Int8&) override {
      flag = MakeBoolean(x.int8_value == y.int8_value);
    }
    void Visit(const type::Int16&) override {
      flag = MakeBoolean(x.int16_value == y.int16_value);
    }
    void Visit(const type::Int32&) override {
      flag = MakeBoolean(x.int32_value == y.int32_value);
    }
    void Visit(const type::Int64&) override {
      flag = MakeBoolean(x.int64_value == y.int64_value);
    }
    void Visit(const type::Uint&) override {
      flag = MakeBoolean(x.uint_value == y.uint_value);
    }
    void Visit(const type::Uint8&) override {
      flag = MakeBoolean(x.uint8_value == y.uint8_value);
    }
    void Visit(const type::Uint16&) override {
      flag = MakeBoolean(x.uint16_value == y.uint16_value);
    }
    void Visit(const type::Uint32&) override {
      flag = MakeBoolean(x.uint32_value == y.uint32_value);
    }
    void Visit(const type::Uint64&) override {
      flag = MakeBoolean(x.uint64_value == y.uint64_value);
    }
    void Visit(const type::Uintptr&) override {
      flag = MakeBoolean(x.uintptr_value == y.uintptr_value);
    }
    void Visit(const type::String&) override {
      flag = MakeBoolean(x.string_value == y.string_value);
    }
  };

  assert(!IsUnknown());
  assert(!y.IsUnknown());

  if (IsError() || y.IsError()) {
    return MakeError();
  }

  Value xx(*this);
  Value yy(y);

  if (xx.kind < yy.kind) {
    xx = xx.PromoteTo(yy);
  } else if (yy.kind < xx.kind) {
    yy = yy.PromoteTo(xx);
  }

  if (xx.IsError() || yy.IsError()) {
    return MakeError();
  }

  switch (xx.kind) {
    case kBoolean:
      return MakeBoolean(xx.Boolean_value == yy.Boolean_value);
    case kInteger:
      return MakeBoolean(xx.Integer_value == yy.Integer_value);
    case kRune:
      return MakeBoolean(xx.Rune_value == yy.Rune_value);
    case kFloat:
      return MakeBoolean(xx.Float_value == yy.Float_value);
    case kComplex:
      return MakeBoolean(xx.Complex_value == yy.Complex_value);
    case kString:
      return MakeBoolean(xx.String_value == yy.String_value);
    case kTyped:
      {
        if (xx.type != yy.type) {
          return Value::MakeError();
        }
        Visitor vis(xx, yy);
        xx.type->UnderlyingType()->Accept(&vis);
        return vis.flag;
      }
      break;
    default:
      abort();  // NOT_COVERED
      break;
  }

  return MakeError();  // NOT_COVERED
}

bool Value::operator==(const Value& other) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y)
        : x(a_x)
        , y(a_y)
        , flag(false) {}

    const Value& x;
    const Value& y;
    bool flag;

    void Visit(const type::Bool&) override {
      flag = x.bool_value == y.bool_value;
    }
    void Visit(const type::Int8&) override {
      flag = x.int8_value == y.int8_value;
    }
    void Visit(const type::Int16&) override {
      flag = x.int16_value == y.int16_value;
    }
    void Visit(const type::Int32&) override {
      flag = x.int32_value == y.int32_value;
    }
    void Visit(const type::Int64&) override {
      flag = x.int64_value == y.int64_value;
    }
    void Visit(const type::Uint8&) override {
      flag = x.uint8_value == y.uint8_value;
    }
    void Visit(const type::Uint16&) override {
      flag = x.uint16_value == y.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      flag = x.uint32_value == y.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      flag = x.uint64_value == y.uint64_value;
    }
    void Visit(const type::Int&) override {
      flag = x.int_value == y.int_value;
    }
    void Visit(const type::Uint&) override {
      flag = x.uint_value == y.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      flag = x.uintptr_value == y.uintptr_value;
    }
    void Visit(const type::Float32&) override {
      flag = x.float32_value == y.float32_value;
    }
    void Visit(const type::Float64&) override {
      flag = x.float64_value == y.float64_value;
    }
    void Visit(const type::Complex64&) override {
      flag = x.complex64_value == y.complex64_value;
    }
    void Visit(const type::Complex128&) override {
      flag = x.complex128_value == y.complex128_value;
    }
    void Visit(const type::String&) override {
      flag = x.string_value == y.string_value;
    }
  };

  if (this->kind != other.kind) {
    return false;
  }

  switch (this->kind) {
    case Value::kUnknown:
    case Value::kError:
      return true;
    case Value::kBoolean:
      return this->Boolean_value == other.Boolean_value;
    case Value::kInteger:
      return this->Integer_value == other.Integer_value;
    case Value::kFloat:
      return this->Float_value == other.Float_value;
    case Value::kComplex:
      return this->Complex_value == other.Complex_value;
    case Value::kRune:
      return this->Rune_value == other.Rune_value;
    case Value::kString:
      return this->string_value == other.string_value;
    case Value::kTyped:
      {
        if (this->type != other.type) {
          return false;
        }
        Visitor visitor(*this, other);
        this->type->UnderlyingType()->Accept(&visitor);
        return visitor.flag;
      }
  }

  abort();  // NOT_COVERED
}

std::ostream& operator<<(std::ostream& out, const Value& value) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(std::ostream& a_out, const Value& a_value)
        : out(a_out)
        , value(a_value) {}

    std::ostream& out;
    const Value& value;

    void Visit(const type::Bool&) override {
      out <<(value.bool_value ? "true" : "false");
    }
    void Visit(const type::Int8&) override {
      out << static_cast<int>(value.int8_value);
    }
    void Visit(const type::Int16&) override {
      out << value.int16_value;
    }
    void Visit(const type::Int32&) override {
      out << value.int32_value;
    }
    void Visit(const type::Int64&) override {
      out << value.int64_value;
    }
    void Visit(const type::Uint8&) override {
      out << static_cast<int>(value.uint8_value);
    }
    void Visit(const type::Uint16&) override {
      out << value.uint16_value;
    }
    void Visit(const type::Uint32&) override {
      out << value.uint32_value;
    }
    void Visit(const type::Uint64&) override {
      out << value.uint64_value;
    }
    void Visit(const type::Int&) override {
      out << value.int_value;
    }
    void Visit(const type::Uint&) override {
      out << value.uint_value;
    }
    void Visit(const type::Uintptr&) override {
      out << value.uintptr_value;
    }
    void Visit(const type::Float32&) override {
      out << value.float32_value;
    }
    void Visit(const type::Float64&) override {
      out << value.float64_value;
    }
    void Visit(const type::Complex64&) override {
      out << value.complex64_value;
    }
    void Visit(const type::Complex128&) override {
      out << value.complex128_value;
    }
    void Visit(const type::String&) override {
      out << '"' << value.string_value << '"';
    }
  };

  switch (value.kind) {
    case Value::kUnknown:
      out << "unknown";
      break;
    case Value::kError:
      out << "error";
      break;
    case Value::kBoolean:
      out <<(value.Boolean_value ? "true" : "false");
      break;
    case Value::kInteger:
      out << value.Integer_value;
      break;
    case Value::kRune:
      if (value.Rune_value < INVALID &&
         (value.Rune_value < SURROGATE_FIRST ||
          value.Rune_value > SURROGATE_LAST)) {
        std::string s;
        Append(&s, value.Rune_value.get_ui());
        out << "'" << s << "'";
      } else {
        out << value.Rune_value;
      }
      break;
    case Value::kFloat:
      out << value.Float_value;
      break;
    case Value::kComplex:
      out << value.Complex_value;
      break;
    case Value::kString:
      out << '"' << value.String_value << '"';
      break;
    case Value::kTyped:
      {
        Visitor v(out, value);
        value.type->UnderlyingType()->Accept(&v);
      }
      break;
  }
  return out;
}

}  // namespace rcgo
