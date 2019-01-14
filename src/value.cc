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
namespace value {

complex64_t::complex64_t() : m_real(0), m_imag(0) {}

complex64_t::complex64_t(float a_real, float a_imag)
    : m_real(a_real), m_imag(a_imag) {}

complex64_t complex64_t::operator+(const complex64_t& a_other) const {
  return complex64_t(m_real + a_other.m_real, m_imag + a_other.m_imag);
}

complex64_t complex64_t::operator-(const complex64_t& a_other) const {
  return complex64_t(m_real - a_other.m_real, m_imag - a_other.m_imag);
}

complex64_t complex64_t::operator*(const complex64_t& a_other) const {
  return complex64_t(m_real * a_other.m_real - m_imag * a_other.m_imag,
                     m_real * a_other.m_imag + m_imag * a_other.m_real);
}

complex64_t complex64_t::operator/(const complex64_t& a_other) const {
  float denom =
      a_other.m_real * a_other.m_real +
      a_other.m_imag * a_other.m_imag;
  return
      complex64_t((m_real * a_other.m_real + m_imag * a_other.m_real) / denom,
                  (m_imag * a_other.m_real - m_real * a_other.m_imag) / denom);
}

complex64_t complex64_t::operator+() const {
  return complex64_t(+m_real, +m_imag);
}

complex64_t complex64_t::operator-() const {
  return complex64_t(-m_real, -m_imag);
}

bool complex64_t::operator==(const complex64_t& a_other) const {
  return this->m_real == a_other.m_real && this->m_imag == a_other.m_imag;
}

bool complex64_t::operator!=(const complex64_t& a_other) const {
  return !(*this == a_other);
}

std::ostream& operator<<(std::ostream& out, const complex64_t& val) {
  out << val.real() <<(val.imag() >= 0 ? "+" : "") << val.imag() << 'i';
  return out;
}

complex128_t::complex128_t() : m_real(0), m_imag(0) {}

complex128_t::complex128_t(double a_real, double a_imag)
    : m_real(a_real), m_imag(a_imag) {}

complex128_t complex128_t::operator+(const complex128_t& a_other) const {
  return complex128_t(m_real + a_other.m_real, m_imag + a_other.m_imag);
}

complex128_t complex128_t::operator-(const complex128_t& a_other) const {
  return complex128_t(m_real - a_other.m_real, m_imag - a_other.m_imag);
}

complex128_t complex128_t::operator*(const complex128_t& a_other) const {
  return complex128_t(m_real * a_other.m_real - m_imag * a_other.m_imag,
                      m_real * a_other.m_imag + m_imag * a_other.m_real);
}

complex128_t complex128_t::operator/(const complex128_t& a_other) const {
  float denom = a_other.m_real * a_other.m_real +
      a_other.m_imag * a_other.m_imag;
  return
      complex128_t((m_real * a_other.m_real + m_imag * a_other.m_real) / denom,
                   (m_imag * a_other.m_real - m_real * a_other.m_imag) / denom);
}

complex128_t complex128_t::operator+() const {
  return complex128_t(+m_real, +m_imag);
}

complex128_t complex128_t::operator-() const {
  return complex128_t(-m_real, -m_imag);
}

bool complex128_t::operator==(const complex128_t& a_other) const {
  return this->m_real == a_other.m_real && this->m_imag == a_other.m_imag;
}

bool complex128_t::operator!=(const complex128_t& a_other) const {
  return !(*this == a_other);
}

std::ostream& operator<<(std::ostream& out, const complex128_t& val) {
  out << val.real() <<(val.imag() >= 0 ? "+" : "") << val.imag() << 'i';
  return out;
}

complex_t::complex_t() : m_real(0, PRECISION), m_imag(0, PRECISION) {}

complex_t::complex_t(const mpf_class& a_real, const mpf_class& a_imag)
    : m_real(a_real), m_imag(a_imag) {}

bool complex_t::operator==(const complex_t& a_other) const {
  return this->m_real == a_other.m_real && this->m_imag == a_other.m_imag;
}

bool complex_t::operator!=(const complex_t& a_other) const {
  return !(*this == a_other);
}

complex_t complex_t::operator+(const complex_t& a_other) const {
  return complex_t(m_real + a_other.m_real, m_imag + a_other.m_imag);
}

complex_t complex_t::operator-(const complex_t& a_other) const {
  return complex_t(m_real - a_other.m_real, m_imag - a_other.m_imag);
}

complex_t complex_t::operator*(const complex_t& a_other) const {
  return complex_t(m_real * a_other.m_real - m_imag * a_other.m_imag,
                   m_real * a_other.m_imag + m_imag * a_other.m_real);
}

complex_t complex_t::operator/(const complex_t& a_other) const {
  mpf_class denom = a_other.m_real * a_other.m_real +
      a_other.m_imag * a_other.m_imag;
  return complex_t((m_real * a_other.m_real + m_imag * a_other.m_real) / denom,
                   (m_imag * a_other.m_real - m_real * a_other.m_imag) / denom);
}

complex_t complex_t::operator+() const {
  return complex_t(+m_real, +m_imag);
}

complex_t complex_t::operator-() const {
  return complex_t(-m_real, -m_imag);
}

std::ostream& operator<<(std::ostream& out, const complex_t& val) {
  out << val.real() <<(val.imag() >= 0 ? "+" : "") << val.imag() << 'i';
  return out;
}

Value::Value(Kind a_kind) : m_kind(a_kind), m_type(nullptr) {}

ValuePtr Value::DeepCopy() const {
  switch (m_kind) {
    case kError:
    case kBoolean:
    case kString:
    case kInteger:
    case kRune:
    case kFloat:
    case kComplex:
    case kConstant:
    case kFunction:
    case kLValue:
    case kRValue:
    case kType:
      return ValuePtr(new Value(*this));
    case kList:
      {
        ValuePtr x(new Value(kList));
        for (auto val : m_list) {
          x->m_list.push_back(val->DeepCopy());
        }
        return x;
      }
  }

  abort();
}


ValuePtr Value::MakeBoolean(bool a_value) {
  Value* v = new Value(kBoolean);
  v->m_Boolean_value = a_value;
  return ValuePtr(v);
}

ValuePtr Value::MakeInteger(const mpz_class& a_value) {
  Value* v = new Value(kInteger);
  v->m_Integer_value = a_value;
  return ValuePtr(v);
}

ValuePtr Value::MakeFloat(const mpf_class& a_value) {
  Value* v = new Value(kFloat);
  v->m_Float_value = a_value;
  return ValuePtr(v);
}

ValuePtr Value::MakeComplex(const mpf_class& a_real, const mpf_class& a_imag) {
  Value* v = new Value(kComplex);
  v->m_Complex_value = complex_t(a_real, a_imag);
  return ValuePtr(v);
}

ValuePtr Value::MakeRune(const mpz_class& a_value) {
  Value* v = new Value(kRune);
  v->m_Rune_value = a_value;
  return ValuePtr(v);
}

ValuePtr Value::MakeString(const std::string& a_value) {
  Value* v = new Value(kString);
  v->m_String_value = a_value;
  return ValuePtr(v);
}

ValuePtr Value::MakeFunction(const type::Function* a_type) {
  Value* v = new Value(kFunction);
  v->m_type = a_type;
  return ValuePtr(v);
}

ValuePtr Value::MakeLValue(const type::Type* a_type) {
  Value* v = new Value(kLValue);
  v->m_type = a_type;
  return ValuePtr(v);
}

ValuePtr Value::MakeRValue(const type::Type* a_type) {
  Value* v = new Value(kRValue);
  v->m_type = a_type;
  return ValuePtr(v);
}

ValuePtr Value::MakeType(const type::Type* a_type) {
  Value* v = new Value(kType);
  v->m_type = a_type;
  return ValuePtr(v);
}

ValuePtr Value::MakeList() {
  return ValuePtr(new Value(kList));
}

void Value::Push(ValuePtr element) {
  assert(m_kind == kList);
  assert(element.get() != nullptr);
  m_list.push_back(element);
}

bool Value::IsArithmetic() const {
  switch (m_kind) {
    case kInteger:
    case kRune:
    case kFloat:
    case kComplex:
      return true;
    case kConstant:
    case kLValue:
    case kRValue:
      return type::IsArithmetic(m_type);
    default:
      return false;
  }
}

bool Value::IsSigned() const {
  switch (m_kind) {
    case kInteger:
    case kRune:
    case kFloat:
    case kComplex:
      return true;
    case kConstant:
    case kLValue:
    case kRValue:
      return type::IsSigned(m_type);
    default:
      return false;
  }
}

bool Value::IsInteger() const {
  switch (m_kind) {
    case kInteger:
    case kRune:
      return true;
    case kConstant:
    case kLValue:
    case kRValue:
      return type::IsInteger(m_type);
    default:
      return false;
  }
}

bool Value::IsZero() const {
  struct Visitor : public type::DefaultVisitor {
    explicit Visitor(const Value& a_x) : x(a_x), flag(false) {}
    const Value& x;
    bool flag;

    void Visit(const type::Int8&) override    { flag = x.m_int8_value == 0; }
    void Visit(const type::Int16&) override   { flag = x.m_int16_value == 0; }
    void Visit(const type::Int32&) override   { flag = x.m_int32_value == 0; }
    void Visit(const type::Int64&) override   { flag = x.m_int64_value == 0; }
    void Visit(const type::Uint8&) override   { flag = x.m_uint8_value == 0; }
    void Visit(const type::Uint16&) override  { flag = x.m_uint16_value == 0; }
    void Visit(const type::Uint32&) override  { flag = x.m_uint32_value == 0; }
    void Visit(const type::Uint64&) override  { flag = x.m_uint64_value == 0; }
    void Visit(const type::Int&) override     { flag = x.m_int_value == 0; }
    void Visit(const type::Uint&) override    { flag = x.m_uint_value == 0; }
    void Visit(const type::Uintptr&) override { flag = x.m_uintptr_value == 0; }
    void Visit(const type::Float32&) override { flag = x.m_float32_value == 0; }
    void Visit(const type::Float64&) override { flag = x.m_float64_value == 0; }
    void Visit(const type::Complex64&) override {
      flag = x.m_complex64_value == complex64_t(0, 0);
    }
    void Visit(const type::Complex128&) override {
      flag = x.m_complex128_value == complex128_t(0, 0);
    }
  };

  switch (m_kind) {
    case kInteger:
      return m_Integer_value == 0;
    case kRune:
      return m_Rune_value == 0;
    case kFloat:
      return m_Float_value == 0;
    case kComplex:
      return m_Complex_value == complex_t(0, 0);
    case kConstant:
      {
        Visitor vis(*this);
        m_type->UnderlyingType()->Accept(&vis);
        return vis.flag;
      }
    default:
      return false;
  }
}

bool Value::IsString() const {
  switch (m_kind) {
    case kString:
      return true;
    case kConstant:
    case kLValue:
    case kRValue:
      return type::IsString(m_type);
    default:
      return false;
  }
}

bool Value::IsBoolean() const {
  switch (m_kind) {
    case kBoolean:
      return true;
    case kConstant:
    case kLValue:
    case kRValue:
      return type::IsBoolean(m_type);
    default:
      return false;
  }
}

bool Value::IsCallable() const {
  switch (m_kind) {
    case kFunction:
      return true;
    default:
      return false;
  }
}

struct ConvertVisitor : public type::DefaultVisitor {
  Value* value;
  const type::Type* type;
  bool flag;

  explicit ConvertVisitor(Value* a_value, const type::Type* a_type)
      : value(a_value)
      , type(a_type)
      , flag(false) {}

  void Visit(const type::Bool&) override {
    if (value->m_kind == Value::kBoolean) {
      value->m_kind = Value::kConstant;
      value->m_bool_value = value->m_Boolean_value;
      value->m_type = type;
      flag = true;
    }
  }

  void Visit(const type::String&) override {
    if (value->m_kind == Value::kString) {
      value->m_kind = Value::kConstant;
      value->m_string_value = value->m_String_value;
      value->m_type = type;
      flag = true;
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

#define CONVERT_TO_COMPLEX(Type, ValueMember, ValueType)                \
  void Visit(const Type&) override                                      \
  {                                                                     \
    ValueType v;                                                        \
    switch (value->m_kind)                                              \
    {                                                                   \
      case Value::kInteger:                                             \
        v = ValueType(value->m_Integer_value.get_d(), 0);               \
        if (exact_check(value->m_Integer_value, v.real())) {            \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kFloat:                                               \
        v = ValueType(value->m_Float_value.get_d(), 0);                 \
        if (close_check(value->m_Float_value, v.real())) {              \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kComplex:                                             \
        v =                                                             \
            ValueType(value->m_Complex_value.real().get_d(),            \
                      value->m_Complex_value.imag().get_d());           \
        if (close_check(value->m_Complex_value.real(), v.real()) &&     \
            close_check(value->m_Complex_value.imag(), v.imag())) {     \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kRune:                                                \
        v = ValueType(value->m_Rune_value.get_d(), 0);                  \
        if (exact_check(value->m_Rune_value, v.real())) {               \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      default:                                                          \
        break;                                                          \
    }                                                                   \
  }

  CONVERT_TO_COMPLEX(type::Complex64, m_complex64_value, complex64_t)
  CONVERT_TO_COMPLEX(type::Complex128, m_complex128_value, complex128_t)

#define CONVERT_TO_FLOAT(Type, ValueMember, ValueType)                  \
  void Visit(const Type&) override                                      \
  {                                                                     \
    ValueType v;                                                        \
    switch (value->m_kind)                                              \
    {                                                                   \
      case Value::kInteger:                                             \
        v = value->m_Integer_value.get_d();                             \
        if (exact_check(value->m_Integer_value, v)) {                   \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kFloat:                                               \
        v = value->m_Float_value.get_d();                               \
        if (close_check(value->m_Float_value, v)) {                     \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kComplex:                                             \
        v = value->m_Complex_value.real().get_d();                      \
        if (close_check(value->m_Complex_value.real(), v) &&            \
            value->m_Complex_value.imag() == 0) {                       \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kRune:                                                \
        v = value->m_Rune_value.get_d();                                \
        if (exact_check(value->m_Rune_value, v)) {                      \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      default:                                                          \
        break;                                                          \
    }                                                                   \
  }

  CONVERT_TO_FLOAT(type::Float32, m_float32_value, float)
  CONVERT_TO_FLOAT(type::Float64, m_float64_value, double)

#define CONVERT_TO_INT(Type, ValueMember, ValueType)                    \
  void Visit(const Type&) override                                      \
  {                                                                     \
    ValueType v;                                                        \
    switch (value->m_kind)                                              \
    {                                                                   \
      case Value::kInteger:                                             \
        v = value->m_Integer_value.get_si();                            \
        if (exact_check(value->m_Integer_value, v)) {                   \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kFloat:                                               \
        v = value->m_Float_value.get_si();                              \
        if (close_check(value->m_Float_value, v)) {                     \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kComplex:                                             \
        v = value->m_Complex_value.real().get_si();                     \
        if (close_check(value->m_Complex_value.real(), v) &&            \
            value->m_Complex_value.imag() == 0) {                       \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kRune:                                                \
        v = value->m_Rune_value.get_si();                               \
        if (exact_check(value->m_Rune_value, v)) {                      \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      default:                                                          \
        break;                                                          \
    }                                                                   \
  }

  CONVERT_TO_INT(type::Int, m_int_value, int)
  CONVERT_TO_INT(type::Int8, m_int8_value, int8_t)
  CONVERT_TO_INT(type::Int16, m_int16_value, int16_t)
  CONVERT_TO_INT(type::Int32, m_int32_value, int32_t)
  CONVERT_TO_INT(type::Int64, m_int64_value, int64_t)

#define CONVERT_TO_UINT(Type, ValueMember, ValueType)                   \
  void Visit(const Type&) override                                      \
  {                                                                     \
    ValueType v;                                                        \
    switch (value->m_kind)                                              \
    {                                                                   \
      case Value::kInteger:                                             \
        v = value->m_Integer_value.get_ui();                            \
        if (exact_check(value->m_Integer_value, v)) {                   \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kFloat:                                               \
        v = value->m_Float_value.get_ui();                              \
        if (close_check(value->m_Float_value, v)) {                     \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kComplex:                                             \
        v = value->m_Complex_value.real().get_ui();                     \
        if (close_check(value->m_Complex_value.real(), v) &&            \
            value->m_Complex_value.imag() == 0) {                       \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      case Value::kRune:                                                \
        v = value->m_Rune_value.get_ui();                               \
        if (exact_check(value->m_Rune_value, v)) {                      \
          value->m_kind = Value::kConstant;                             \
          value->m_type = type;                                         \
          value->ValueMember = v;                                       \
          flag = true;                                                  \
        }                                                               \
        break;                                                          \
      default:                                                          \
        break;                                                          \
    }                                                                   \
  }

  CONVERT_TO_UINT(type::Uint, m_uint_value, unsigned int)
  CONVERT_TO_UINT(type::Uint8, m_uint8_value, uint8_t)
  CONVERT_TO_UINT(type::Uint16, m_uint16_value, uint16_t)
  CONVERT_TO_UINT(type::Uint32, m_uint32_value, uint32_t)
  CONVERT_TO_UINT(type::Uint64, m_uint64_value, uint64_t)
  CONVERT_TO_UINT(type::Uintptr, m_uintptr_value, unsigned int)
};

bool Value::ConvertTo(const type::Type* type) {
  ConvertVisitor visitor(this, type);
  type->UnderlyingType()->Accept(&visitor);
  return visitor.flag;
}

bool Value::PromoteTo(const Value& other) {
  assert(this->kind() <= other.kind());

  if (this->kind() == other.kind()) {
    return true;
  }

  if (other.kind() == Value::kConstant) {
    return this->ConvertTo(other.type());
  }

  switch (this->kind()) {
    case Value::kBoolean:
      return false;
    case Value::kString:
      return false;
    case Value::kInteger:
      this->m_kind = kRune;
      this->m_Rune_value = this->m_Integer_value;
      return PromoteTo(other);
    case Value::kRune:
      this->m_kind = kFloat;
      this->m_Float_value = this->m_Rune_value;
      return PromoteTo(other);
    case Value::kFloat:
      this->m_kind = kComplex;
      this->m_Complex_value = complex_t(this->m_Float_value, 0);
      return PromoteTo(other);

    default:
      abort();  // NOT_COVERED
  }
}

bool Value::ToInteger() {
  struct Visitor : public type::DefaultVisitor {
    explicit Visitor(Value* a_value) : value(a_value) {}
    Value* value;

    void Visit(const type::Int8&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_kind = kInteger;
      value->m_Integer_value = value->m_uintptr_value;
    }
  };

  switch (m_kind) {
    case Value::kInteger:
      return true;
      break;
    case Value::kRune:
      this->m_kind = kInteger;
      this->m_Integer_value = this->m_Rune_value;
      return true;
      break;
    case Value::kFloat:
      {
        mpz_class x(m_Float_value);
        if (m_Float_value == mpf_class(x)) {
          this->m_kind = kInteger;
          this->m_Integer_value = x;
          return true;
        } else {
          return false;
        }
      }
      break;
    case Value::kComplex:
      {
        mpz_class x(m_Complex_value.real());
        if (m_Complex_value == complex_t(x, 0)) {
          this->m_kind = kInteger;
          this->m_Integer_value = x;
          return true;
        } else {
          return false;
        }
      }
      break;
    case Value::kConstant:
      if (type::IsInteger(m_type)) {
        Visitor vis(this);
        m_type->UnderlyingType()->Accept(&vis);
        return true;
      } else {
        return false;
      }
      break;
    default:
      return false;
      break;
  }
}

void Value::Dereference() {
  if (m_kind == kLValue) {
    m_kind = kRValue;
  }
}

bool Value::IsError() const {
  return m_kind == kError;
}

bool Value::RequireConstant(ErrorReporter* error_reporter) const {
  // TODO(jrw972):  Do this.
  abort();
  // switch (m_kind) {
  //   case kBoolean:
  //   case kString:
  //   case kInteger:
  //   case kRune:
  //   case kFloat:
  //   case kComplex:
  //   case kConstant:
  //     return true;
  //   default:
  //     return false;
  // }
}

// TODO(jrw972):  Handle overflow and underflow.

ValuePtr Value::Posate(
    const Location& location, ValuePtr x,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ValuePtr a_value) : x(a_x), value(a_value) {}
    ConstValuePtr x;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = +x->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = +x->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = +x->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = +x->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = +x->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = +x->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = +x->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = +x->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = +x->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = +x->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = +x->m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->m_float32_value = +x->m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value->m_float64_value = +x->m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->m_complex64_value = +x->m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->m_complex128_value = +x->m_complex128_value;
    }
  };

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsArithmetic()) {
    error_reporter->Insert(CannotApply(location, '+', x));
    return Value::MakeError();
  }

  x->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = +x->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = +x->m_Rune_value;
      break;
    case Value::kFloat:
      v->m_Float_value = +x->m_Float_value;
      break;
    case Value::kComplex:
      v->m_Complex_value = +x->m_Complex_value;
      break;
    case Value::kConstant:
      {
        v->m_type = x->m_type;
        Visitor vis(x, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

ValuePtr Value::Negate(
    const Location& location, ValuePtr x,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ValuePtr a_value) : x(a_x), value(a_value) {}
    ConstValuePtr x;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = -x->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = -x->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = -x->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = -x->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = -x->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = -x->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = -x->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = -x->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = -x->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = -x->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = -x->m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->m_float32_value = -x->m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value->m_float64_value = -x->m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->m_complex64_value = -x->m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->m_complex128_value = -x->m_complex128_value;
    }
  };

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsArithmetic()) {
    error_reporter->Insert(CannotApply(location, '-', x));
    return Value::MakeError();
  }

  x->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = -x->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = -x->m_Rune_value;
      break;
    case Value::kFloat:
      v->m_Float_value = -x->m_Float_value;
      break;
    case Value::kComplex:
      v->m_Complex_value = -x->m_Complex_value;
      break;
    case Value::kConstant:
      {
        v->m_type = x->m_type;
        Visitor vis(x, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

ValuePtr Value::LogicNot(
    const Location& location, ValuePtr x,
    ErrorReporter* error_reporter) {
  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsBoolean()) {
    error_reporter->Insert(CannotApply(location, '!', x));
    return Value::MakeError();
  }

  x->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kBoolean:
      v->m_Boolean_value = !x->m_Boolean_value;
      break;
    case Value::kConstant:
      v->m_bool_value = !x->m_bool_value;
      v->m_type = x->m_type;
      break;
    case Value::kRValue:
      v->m_type = x->m_type;
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

ValuePtr Value::BitNot(
    const Location& location, ValuePtr x,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ValuePtr a_value) : x(a_x), value(a_value) {}
    ConstValuePtr x;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = ~x->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = ~x->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = ~x->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = ~x->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = ~x->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = ~x->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = ~x->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = ~x->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = ~x->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = ~x->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = ~x->m_uintptr_value;
    }
  };

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsInteger()) {
    error_reporter->Insert(CannotApply(location, '~', x));
    return Value::MakeError();
  }

  x->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = ~x->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = ~x->m_Rune_value;
      break;
    case Value::kConstant:
      {
        v->m_type = x->m_type;
        Visitor vis(x, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

ValuePtr Value::Add(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ConstValuePtr a_y, ValuePtr a_value)
        : x(a_x), y(a_y), value(a_value) {}
    ConstValuePtr x;
    ConstValuePtr y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value + y->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value + y->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value + y->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value + y->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value + y->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value + y->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value + y->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value + y->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value + y->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value + y->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value + y->m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->m_float32_value = x->m_float32_value + y->m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value->m_float64_value = x->m_float64_value + y->m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->m_complex64_value = x->m_complex64_value + y->m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->m_complex128_value = x->m_complex128_value + y->m_complex128_value;
    }
    void Visit(const type::String&) override {
      value->m_string_value = x->m_string_value + y->m_string_value;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!((x->IsArithmetic() && y->IsArithmetic()) ||
       (x->IsString() && y->IsString()))) {
    // TODO(jrw972):  Make this check and other look at the left and right
    // independently.
    error_reporter->Insert(CannotApply2(location, "+", x, y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "+", x, y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "+", x, y));
      return Value::MakeError();
    }
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = x->m_Integer_value + y->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = x->m_Rune_value + y->m_Rune_value;
      break;
    case Value::kFloat:
      v->m_Float_value = x->m_Float_value + y->m_Float_value;
      break;
    case Value::kComplex:
      v->m_Complex_value = x->m_Complex_value + y->m_Complex_value;
      break;
    case Value::kString:
      v->m_String_value = x->m_String_value + y->m_String_value;
      break;
    case Value::kConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "+", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
        Visitor vis(x, y, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "+", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

ValuePtr Value::Subtract(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ConstValuePtr a_y, ValuePtr a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    ConstValuePtr x;
    ConstValuePtr y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value - y->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value - y->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value - y->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value - y->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value - y->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value - y->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value - y->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value - y->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value - y->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value - y->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value - y->m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->m_float32_value = x->m_float32_value - y->m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value->m_float64_value = x->m_float64_value - y->m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->m_complex64_value = x->m_complex64_value - y->m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->m_complex128_value = x->m_complex128_value - y->m_complex128_value;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsArithmetic() && y->IsArithmetic())) {
    error_reporter->Insert(CannotApply2(location, "-", x, y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "-", x, y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "-", x, y));
      return Value::MakeError();
    }
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = x->m_Integer_value - y->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = x->m_Rune_value - y->m_Rune_value;
      break;
    case Value::kFloat:
      v->m_Float_value = x->m_Float_value - y->m_Float_value;
      break;
    case Value::kComplex:
      v->m_Complex_value = x->m_Complex_value - y->m_Complex_value;
      break;
    case Value::kConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "-", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
        Visitor vis(x, y, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "-", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

ValuePtr Value::Multiply(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ConstValuePtr a_y, ValuePtr a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    ConstValuePtr x;
    ConstValuePtr y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value * y->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value * y->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value * y->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value * y->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value * y->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value * y->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value * y->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value * y->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value * y->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value * y->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value * y->m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->m_float32_value = x->m_float32_value * y->m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value->m_float64_value = x->m_float64_value * y->m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->m_complex64_value = x->m_complex64_value * y->m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->m_complex128_value = x->m_complex128_value * y->m_complex128_value;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsArithmetic() && y->IsArithmetic())) {
    error_reporter->Insert(CannotApply2(location, "*", x, y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "*", x, y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "*", x, y));
      return Value::MakeError();
    }
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = x->m_Integer_value * y->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = x->m_Rune_value * y->m_Rune_value;
      break;
    case Value::kFloat:
      v->m_Float_value = x->m_Float_value * y->m_Float_value;
      break;
    case Value::kComplex:
      v->m_Complex_value = x->m_Complex_value * y->m_Complex_value;
      break;
    case Value::kConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "*", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
        Visitor vis(x, y, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "*", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

ValuePtr Value::Divide(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ConstValuePtr a_y, ValuePtr a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    ConstValuePtr x;
    ConstValuePtr y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value / y->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value / y->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value / y->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value / y->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value / y->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value / y->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value / y->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value / y->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value / y->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value / y->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value / y->m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value->m_float32_value = x->m_float32_value / y->m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value->m_float64_value = x->m_float64_value / y->m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value->m_complex64_value = x->m_complex64_value / y->m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value->m_complex128_value = x->m_complex128_value / y->m_complex128_value;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsArithmetic() && y->IsArithmetic())) {
    error_reporter->Insert(CannotApply2(location, "/", x, y));
    return Value::MakeError();
  }

  if (y->IsZero()) {
    error_reporter->Insert(DivisionByZero(location));
    return Value::MakeError();
  }

    if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "/", x, y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "/", x, y));
      return Value::MakeError();
    }
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = x->m_Integer_value / y->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = x->m_Rune_value / y->m_Rune_value;
      break;
    case Value::kFloat:
      v->m_Float_value = x->m_Float_value / y->m_Float_value;
      break;
    case Value::kComplex:
      v->m_Complex_value = x->m_Complex_value / y->m_Complex_value;
      break;
    case Value::kConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "/", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
        Visitor vis(x, y, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "/", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

ValuePtr Value::Modulo(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ConstValuePtr a_y, ValuePtr a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    ConstValuePtr x;
    ConstValuePtr y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value % y->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value % y->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value % y->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value % y->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value % y->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value % y->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value % y->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value % y->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value % y->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value % y->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value % y->m_uintptr_value;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && x->IsInteger())) {
    error_reporter->Insert(CannotApply2(location, "%", x, y));
    return Value::MakeError();
  }

  if (y->IsZero()) {
    error_reporter->Insert(DivisionByZero(location));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "%", x, y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "%", x, y));
      return Value::MakeError();
    }
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = x->m_Integer_value % y->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = x->m_Rune_value % y->m_Rune_value;
      break;
    case Value::kConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "%", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
        Visitor vis(x, y, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "%", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

ValuePtr Value::LeftShift(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, unsigned int a_y, ValuePtr a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    ConstValuePtr x;
    unsigned int y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value << y;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value << y;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value << y;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value << y;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value << y;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value << y;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value << y;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value << y;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value << y;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value << y;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value << y;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  {
    Value xx = *x;
    if (!xx.ToInteger()) {
      error_reporter->Insert(CannotApply2(location, "<<", x, y));
      return Value::MakeError();
    }
  }

  if (!y->ToInteger() ||
      !y->ConvertTo(&type::Uint::instance)) {
    error_reporter->Insert(CannotApply2(location, "<<", x, y));
    return Value::MakeError();
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (x->m_kind) {
    case Value::kInteger:
      v = MakeInteger(x->m_Integer_value << y->m_uint_value);
      break;
    case Value::kRune:
      v = MakeInteger(x->m_Rune_value << y->m_uint_value);
      break;
    case Value::kFloat:
      v = MakeInteger(mpz_class(x->m_Float_value) << y->m_uint_value);
      break;
    case Value::kComplex:
      v = MakeInteger(mpz_class(x->m_Complex_value.real()) << y->m_uint_value);
      break;
    case Value::kConstant:
      {
        v->m_kind = Value::kConstant;
        v->m_type = x->m_type;
        Visitor vis(x, y->m_uint_value, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        v->m_kind = Value::kConstant;
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

ValuePtr Value::RightShift(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, unsigned int a_y, ValuePtr a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    ConstValuePtr x;
    unsigned int y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value >> y;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value >> y;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value >> y;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value >> y;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value >> y;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value >> y;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value >> y;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value >> y;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value >> y;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value >> y;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value >> y;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  {
    Value xx = *x;
    if (!xx.ToInteger()) {
      error_reporter->Insert(CannotApply2(location, ">>", x, y));
      return Value::MakeError();
    }
  }

  if (!y->ToInteger() ||
      !y->ConvertTo(&type::Uint::instance)) {
    error_reporter->Insert(CannotApply2(location, ">>", x, y));
    return Value::MakeError();
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (x->m_kind) {
    case Value::kInteger:
      v = MakeInteger(x->m_Integer_value >> y->m_uint_value);
      break;
    case Value::kRune:
      v = MakeInteger(x->m_Rune_value >> y->m_uint_value);
      break;
    case Value::kFloat:
      v = MakeInteger(mpz_class(x->m_Float_value) >> y->m_uint_value);
      break;
    case Value::kComplex:
      v = MakeInteger(mpz_class(x->m_Complex_value.real()) >> y->m_uint_value);
      break;
    case Value::kConstant:
      {
        v->m_kind = Value::kConstant;
        v->m_type = x->m_type;
        Visitor vis(x, y->m_uint_value, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        v->m_kind = Value::kConstant;
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

ValuePtr Value::BitAnd(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ConstValuePtr a_y, ValuePtr a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    ConstValuePtr x;
    ConstValuePtr y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value & y->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value & y->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value & y->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value & y->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value & y->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value & y->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value & y->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value & y->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value & y->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value & y->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value & y->m_uintptr_value;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_reporter->Insert(CannotApply2(location, "&", x, y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "&", x, y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "&", x, y));
      return Value::MakeError();
    }
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = x->m_Integer_value & y->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = x->m_Rune_value & y->m_Rune_value;
      break;
    case Value::kConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "&", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
        Visitor vis(x, y, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "&", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

ValuePtr Value::BitAndNot(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ConstValuePtr a_y, ValuePtr a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    ConstValuePtr x;
    ConstValuePtr y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value & ~y->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value & ~y->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value & ~y->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value & ~y->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value & ~y->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value & ~y->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value & ~y->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value & ~y->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value & ~y->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value & ~y->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value & ~y->m_uintptr_value;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_reporter->Insert(CannotApply2(location, "&~", x, y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "&~", x, y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "&~", x, y));
      return Value::MakeError();
    }
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = x->m_Integer_value & ~y->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = x->m_Rune_value & ~y->m_Rune_value;
      break;
    case Value::kConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "&~", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
        Visitor vis(x, y, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "&~", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

ValuePtr Value::BitOr(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ConstValuePtr a_y, ValuePtr a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    ConstValuePtr x;
    ConstValuePtr y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value | y->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value | y->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value | y->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value | y->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value | y->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value | y->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value | y->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value | y->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value | y->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value | y->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value | y->m_uintptr_value;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_reporter->Insert(CannotApply2(location, "|", x, y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "|", x, y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "|", x, y));
      return Value::MakeError();
    }
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = x->m_Integer_value | y->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = x->m_Rune_value | y->m_Rune_value;
      break;
    case Value::kConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "|", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
        Visitor vis(x, y, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "|", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

ValuePtr Value::BitXor(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ConstValuePtr a_y, ValuePtr a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    ConstValuePtr x;
    ConstValuePtr y;
    ValuePtr value;

    void Visit(const type::Int8&) override {
      value->m_int8_value = x->m_int8_value ^ y->m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value->m_int16_value = x->m_int16_value ^ y->m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value->m_int32_value = x->m_int32_value ^ y->m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value->m_int64_value = x->m_int64_value ^ y->m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value->m_uint8_value = x->m_uint8_value ^ y->m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value->m_uint16_value = x->m_uint16_value ^ y->m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value->m_uint32_value = x->m_uint32_value ^ y->m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value->m_uint64_value = x->m_uint64_value ^ y->m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value->m_int_value = x->m_int_value ^ y->m_int_value;
    }
    void Visit(const type::Uint&) override {
      value->m_uint_value = x->m_uint_value ^ y->m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value->m_uintptr_value = x->m_uintptr_value ^ y->m_uintptr_value;
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_reporter->Insert(CannotApply2(location, "^", x, y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "^", x, y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "^", x, y));
      return Value::MakeError();
    }
  }

  x->Dereference();
  y->Dereference();

  ValuePtr v(new Value(x->m_kind));
  switch (v->m_kind) {
    case Value::kInteger:
      v->m_Integer_value = x->m_Integer_value ^ y->m_Integer_value;
      break;
    case Value::kRune:
      v->m_Rune_value = x->m_Rune_value ^ y->m_Rune_value;
      break;
    case Value::kConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "^", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
        Visitor vis(x, y, v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "^", x, y));
          return Value::MakeError();
        }
        v->m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

ValuePtr Value::Equal(
    const Location& location, ValuePtr x, ValuePtr y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(ConstValuePtr a_x, ConstValuePtr a_y)
        : x(a_x) , y(a_y) {}
    ConstValuePtr x;
    ConstValuePtr y;
    ValuePtr flag;

    void Visit(const type::Bool&) override {
      flag = MakeBoolean(x->m_bool_value == y->m_bool_value);
    }
    void Visit(const type::Complex64&) override {
      flag = MakeBoolean(x->m_complex64_value == y->m_complex64_value);
    }
    void Visit(const type::Complex128&) override {
      flag = MakeBoolean(x->m_complex128_value == y->m_complex128_value);
    }
    void Visit(const type::Float32&) override {
      flag = MakeBoolean(x->m_float32_value == y->m_float32_value);
    }
    void Visit(const type::Float64&) override {
      flag = MakeBoolean(x->m_float64_value == y->m_float64_value);
    }
    void Visit(const type::Int&) override {
      flag = MakeBoolean(x->m_int_value == y->m_int_value);
    }
    void Visit(const type::Int8&) override {
      flag = MakeBoolean(x->m_int8_value == y->m_int8_value);
    }
    void Visit(const type::Int16&) override {
      flag = MakeBoolean(x->m_int16_value == y->m_int16_value);
    }
    void Visit(const type::Int32&) override {
      flag = MakeBoolean(x->m_int32_value == y->m_int32_value);
    }
    void Visit(const type::Int64&) override {
      flag = MakeBoolean(x->m_int64_value == y->m_int64_value);
    }
    void Visit(const type::Uint&) override {
      flag = MakeBoolean(x->m_uint_value == y->m_uint_value);
    }
    void Visit(const type::Uint8&) override {
      flag = MakeBoolean(x->m_uint8_value == y->m_uint8_value);
    }
    void Visit(const type::Uint16&) override {
      flag = MakeBoolean(x->m_uint16_value == y->m_uint16_value);
    }
    void Visit(const type::Uint32&) override {
      flag = MakeBoolean(x->m_uint32_value == y->m_uint32_value);
    }
    void Visit(const type::Uint64&) override {
      flag = MakeBoolean(x->m_uint64_value == y->m_uint64_value);
    }
    void Visit(const type::Uintptr&) override {
      flag = MakeBoolean(x->m_uintptr_value == y->m_uintptr_value);
    }
    void Visit(const type::String&) override {
      flag = MakeBoolean(x->m_string_value == y->m_string_value);
    }
  };

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "==", x, y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "==", x, y));
      return Value::MakeError();
    }
  }

  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kBoolean:
      return ValuePtr(MakeBoolean(x->m_Boolean_value == y->m_Boolean_value));
    case Value::kInteger:
      return ValuePtr(MakeBoolean(x->m_Integer_value == y->m_Integer_value));
    case Value::kRune:
      return ValuePtr(MakeBoolean(x->m_Rune_value == y->m_Rune_value));
    case Value::kFloat:
      return ValuePtr(MakeBoolean(x->m_Float_value == y->m_Float_value));
    case Value::kComplex:
      return ValuePtr(MakeBoolean(x->m_Complex_value == y->m_Complex_value));
    case Value::kString:
      return ValuePtr(MakeBoolean(x->m_String_value == y->m_String_value));
    case Value::kConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "==", x, y));
          return Value::MakeError();
        }
        Visitor vis(x, y);
        x->m_type->UnderlyingType()->Accept(&vis);
        return ValuePtr(vis.flag);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "==", x, y));
          return Value::MakeError();
        }
        return ValuePtr(MakeRValue(&type::Bool::instance));
      }
      break;
    default:
      abort();  // NOT_COVERED
      break;
  }

  abort();
  return Value::MakeError();  // NOT_COVERED
}

ValuePtr Value::Call(
    const Location& location, ValuePtr operand, ValuePtr arguments,
    const LocationList& locations, ErrorReporter* error_reporter) {
  assert(arguments->m_kind == kList);
  assert(arguments->m_list.size() == locations.size());

  if (operand->IsError()) {
    return Value::MakeError();
  }

  for (auto argument : arguments->m_list) {
    if (argument->IsError()) {
      return Value::MakeError();
    }
  }

  if (!operand->IsCallable()) {
    error_reporter->Insert(OperandCannotBeCalled(location));
    return Value::MakeError();
  }

  const type::Function* function =
      type::Cast<const type::Function>(operand->m_type);

  if (arguments->m_list.size() != function->ParameterCount()) {
    error_reporter->Insert(
        CallExpectsNArguments(
            location, function->ParameterCount(), arguments->m_list.size()));
    return Value::MakeError();
  }

  for (size_t idx = 0, limit = arguments->m_list.size(); idx != limit; ++idx) {
    ValuePtr argument = arguments->m_list.at(idx);
    const symbol::Parameter* parameter = function->ParameterAt(idx);
    if (!argument->ConvertTo(parameter->type)) {
      error_reporter->Insert(CannotConvert(locations.at(idx), argument,
                                           parameter->type));
      return Value::MakeError();
    }
    argument->Dereference();
  }

  ValuePtr retval(new Value(kList));
  for (auto pos = function->ResultBegin(), limit = function->ResultEnd();
       pos != limit; ++pos) {
    const symbol::Parameter* parameter = *pos;
    retval->m_list.push_back(Value::MakeRValue(parameter->type));
  }

  return retval;
}

bool Value::operator==(const Value& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(const Value& a_x, const Value& a_y)
        : x(a_x)
        , y(a_y)
        , flag(false) {}

    const Value& x;
    const Value& y;
    bool flag;

    void Visit(const type::Bool&) override {
      flag = x.m_bool_value == y.m_bool_value;
    }
    void Visit(const type::Int8&) override {
      flag = x.m_int8_value == y.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      flag = x.m_int16_value == y.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      flag = x.m_int32_value == y.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      flag = x.m_int64_value == y.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      flag = x.m_uint8_value == y.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      flag = x.m_uint16_value == y.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      flag = x.m_uint32_value == y.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      flag = x.m_uint64_value == y.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      flag = x.m_int_value == y.m_int_value;
    }
    void Visit(const type::Uint&) override {
      flag = x.m_uint_value == y.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      flag = x.m_uintptr_value == y.m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      flag = x.m_float32_value == y.m_float32_value;
    }
    void Visit(const type::Float64&) override {
      flag = x.m_float64_value == y.m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      flag = x.m_complex64_value == y.m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      flag = x.m_complex128_value == y.m_complex128_value;
    }
    void Visit(const type::String&) override {
      flag = x.m_string_value == y.m_string_value;
    }
  };

  if (this->m_kind != y.m_kind) {
    return false;
  }

  switch (this->m_kind) {
    case Value::kError:
      return true;
    case Value::kBoolean:
      return this->m_Boolean_value == y.m_Boolean_value;
    case Value::kInteger:
      return this->m_Integer_value == y.m_Integer_value;
    case Value::kFloat:
      return this->m_Float_value == y.m_Float_value;
    case Value::kComplex:
      return this->m_Complex_value == y.m_Complex_value;
    case Value::kRune:
      return this->m_Rune_value == y.m_Rune_value;
    case Value::kString:
      return this->m_string_value == y.m_string_value;
    case Value::kConstant:
      {
        if (this->m_type != y.m_type) {
          return false;
        }
        Visitor visitor(*this, y);
        this->m_type->UnderlyingType()->Accept(&visitor);
        return visitor.flag;
      }
    case Value::kFunction:
    case Value::kRValue:
    case Value::kLValue:
      {
        return this->m_type == y.m_type;
      }
    case Value::kType:
      {
        return this->m_type == y.m_type;
      }
    case Value::kList:
      if (this->m_list.size() != y.m_list.size()) {
        return false;
      }
      for (ListType::const_iterator xpos = this->m_list.begin(),
               xlimit = this->m_list.end(), ypos = y.m_list.begin();
           xpos != xlimit; ++xpos, ++ypos) {
        if (*xpos != *ypos) {
          return false;
        }
      }
      return false;
  }

  abort();  // NOT_COVERED
}

bool Value::operator!=(const Value& y) const {
  return !(*this == y);
}

ValuePtr Value::MakeError() {
  return ValuePtr(new Value(kError));
}

std::ostream& operator<<(std::ostream& out, const Value& value) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(std::ostream& a_out, const Value& a_value)
        : out(a_out)
        , value(a_value) {}

    std::ostream& out;
    const Value& value;

    void Visit(const type::Bool&) override {
      out <<(value.bool_value() ? "true" : "false");
    }
    void Visit(const type::Int8&) override {
      out << static_cast<int>(value.int8_value());
    }
    void Visit(const type::Int16&) override {
      out << value.int16_value();
    }
    void Visit(const type::Int32&) override {
      out << value.int32_value();
    }
    void Visit(const type::Int64&) override {
      out << value.int64_value();
    }
    void Visit(const type::Uint8&) override {
      out << static_cast<int>(value.uint8_value());
    }
    void Visit(const type::Uint16&) override {
      out << value.uint16_value();
    }
    void Visit(const type::Uint32&) override {
      out << value.uint32_value();
    }
    void Visit(const type::Uint64&) override {
      out << value.uint64_value();
    }
    void Visit(const type::Int&) override {
      out << value.int_value();
    }
    void Visit(const type::Uint&) override {
      out << value.uint_value();
    }
    void Visit(const type::Uintptr&) override {
      out << value.uintptr_value();
    }
    void Visit(const type::Float32&) override {
      out << value.float32_value();
    }
    void Visit(const type::Float64&) override {
      out << value.float64_value();
    }
    void Visit(const type::Complex64&) override {
      out << value.complex64_value();
    }
    void Visit(const type::Complex128&) override {
      out << value.complex128_value();
    }
    void Visit(const type::String&) override {
      out << '"' << value.string_value() << '"';
    }
  };

  switch (value.kind()) {
    case Value::kError:
      out << "error";
      break;
    case Value::kBoolean:
      out <<(value.Boolean_value() ? "true" : "false");
      break;
    case Value::kInteger:
      out << value.Integer_value();
      break;
    case Value::kRune:
      if (value.Rune_value() < INVALID &&
          (value.Rune_value() < SURROGATE_FIRST ||
           value.Rune_value() > SURROGATE_LAST)) {
        std::string s;
        Append(&s, value.Rune_value().get_ui());
        out << "'" << s << "'";
      } else {
        out << value.Rune_value();
      }
      break;
    case Value::kFloat:
      out << value.Float_value();
      break;
    case Value::kComplex:
      out << value.Complex_value();
      break;
    case Value::kString:
      out << '"' << value.String_value() << '"';
      break;
    case Value::kConstant:
      {
        Visitor v(out, value);
        value.type()->UnderlyingType()->Accept(&v);
      }
      break;
    case Value::kFunction:
      out << "<function>";
      break;
    case Value::kLValue:
      out << "<lvalue>";
      break;
    case Value::kRValue:
      out << "<rvalue>";
      break;
    case Value::kType:
      out << "<type>";
      break;
    case Value::kList:
      out << "<list>";
      break;
  }
  return out;
}

}  // namespace value
}  // namespace rcgo
