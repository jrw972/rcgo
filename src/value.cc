// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/value.h"

#include <algorithm>
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

Value::Value() : m_kind(kUninitialized), m_type(nullptr) {}

Value::Value(Kind a_kind) : m_kind(a_kind), m_type(nullptr) {}

Value Value::MakeUntypedConstant(UntypedConstant const & a_value) {
  assert(a_value.IsInitialized());
  if (a_value.IsError()) {
    return MakeError();
  }
  Value v(kUntypedConstant);
  v.m_untyped_constant = a_value;
  return v;
}

Value Value::MakeFunction(const type::Function* a_type) {
  Value v(kFunction);
  v.m_type = a_type;
  return Value(v);
}

Value Value::MakeLValue(const type::Type* a_type) {
  Value v(kLValue);
  v.m_type = a_type;
  return Value(v);
}

Value Value::MakeRValue(const type::Type* a_type) {
  Value v(kRValue);
  v.m_type = a_type;
  return Value(v);
}

Value Value::MakeType(const type::Type* a_type) {
  Value v(kType);
  v.m_type = a_type;
  return Value(v);
}

UntypedConstant const & Value::untyped_constant() const {
  assert(m_kind == kUntypedConstant);
  return m_untyped_constant;
}

bool Value::IsArithmetic() const {
  switch (m_kind) {
    case kUntypedConstant:
      return m_untyped_constant.IsArithmetic();
    case kTypedConstant:
    case kLValue:
    case kRValue:
      return type::IsArithmetic(m_type);
    default:
      return false;
  }
}

bool Value::IsSigned() const {
  switch (m_kind) {
    case kUntypedConstant:
      return m_untyped_constant.IsSigned();
    case kTypedConstant:
    case kLValue:
    case kRValue:
      return type::IsSigned(m_type);
    default:
      return false;
  }
}

bool Value::IsInteger() const {
  switch (m_kind) {
    case kUntypedConstant:
      return m_untyped_constant.IsInteger();
    case kTypedConstant:
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
    case kUntypedConstant:
      return m_untyped_constant.IsZero();
    case kTypedConstant:
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
    case kUntypedConstant:
      return m_untyped_constant.IsString();
    case kTypedConstant:
    case kLValue:
    case kRValue:
      return type::IsString(m_type);
    default:
      return false;
  }
}

bool Value::IsBoolean() const {
  switch (m_kind) {
    case kUntypedConstant:
      return m_untyped_constant.IsBoolean();
    case kTypedConstant:
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
    if (value->m_kind == Value::kUntypedConstant &&
        value->m_untyped_constant.IsBoolean()) {
      value->m_kind = Value::kTypedConstant;
      value->m_bool_value = value->m_untyped_constant.boolean_value();
      value->m_type = type;
      flag = true;
    }
  }

  void Visit(const type::String&) override {
    if (value->m_kind == Value::kUntypedConstant &&
        value->m_untyped_constant.IsString()) {
      value->m_kind = Value::kTypedConstant;
      value->m_string_value = value->m_untyped_constant.string_value();
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
      case Value::kUntypedConstant:                                     \
        switch (value->m_untyped_constant.kind()) {                     \
          case UntypedConstant::kInteger:                               \
            v = ValueType(value->m_untyped_constant.integer_value().get_d(), 0); \
            if (exact_check(value->m_untyped_constant.integer_value(), v.real())) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kRune:                                \
            v = ValueType(value->m_untyped_constant.rune_value().get_d(), 0); \
            if (exact_check(value->m_untyped_constant.rune_value(), v.real())) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kFloat:                                 \
            v = ValueType(value->m_untyped_constant.float_value().get_d(), 0); \
            if (close_check(value->m_untyped_constant.float_value(), v.real())) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kComplex:                               \
            v =                                                         \
                ValueType(value->m_untyped_constant.complex_value().real().get_d(), \
                          value->m_untyped_constant.complex_value().imag().get_d()); \
            if (close_check(value->m_untyped_constant.complex_value().real(), v.real()) && \
                close_check(value->m_untyped_constant.complex_value().imag(), v.imag())) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          default:                                                      \
            break;                                                      \
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
      case Value::kUntypedConstant:                                     \
        switch (value->m_untyped_constant.kind()) {                     \
          case UntypedConstant::kInteger:                               \
            v = value->m_untyped_constant.integer_value().get_d();      \
            if (exact_check(value->m_untyped_constant.integer_value(), v)) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kRune:                                  \
            v = value->m_untyped_constant.rune_value().get_d();         \
            if (exact_check(value->m_untyped_constant.rune_value(), v)) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kFloat:                                 \
            v = value->m_untyped_constant.float_value().get_d();        \
            if (close_check(value->m_untyped_constant.float_value(), v)) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kComplex:                               \
            v = value->m_untyped_constant.complex_value().real().get_d();                  \
            if (close_check(value->m_untyped_constant.complex_value().real(), v) &&        \
                value->m_untyped_constant.complex_value().imag() == 0) {                   \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          default:                                                      \
            break;                                                      \
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
      case Value::kUntypedConstant:                                     \
        switch (value->m_untyped_constant.kind()) {                     \
          case UntypedConstant::kInteger:                               \
            v = value->m_untyped_constant.integer_value().get_si();     \
            if (exact_check(value->m_untyped_constant.integer_value(), v)) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kRune:                                  \
            v = value->m_untyped_constant.rune_value().get_si();        \
            if (exact_check(value->m_untyped_constant.rune_value(), v)) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kFloat:                                 \
            v = value->m_untyped_constant.float_value().get_si();       \
            if (close_check(value->m_untyped_constant.float_value(), v)) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kComplex:                               \
            v = value->m_untyped_constant.complex_value().real().get_si(); \
            if (close_check(value->m_untyped_constant.complex_value().real(), v) && \
                value->m_untyped_constant.complex_value().imag() == 0) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          default:                                                      \
            break;                                                      \
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
      case Value::kUntypedConstant:                                     \
        switch (value->m_untyped_constant.kind()) {                     \
          case UntypedConstant::kInteger:                               \
            v = value->m_untyped_constant.integer_value().get_ui();     \
            if (exact_check(value->m_untyped_constant.integer_value(), v)) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kRune:                                  \
            v = value->m_untyped_constant.rune_value().get_ui();        \
            if (exact_check(value->m_untyped_constant.rune_value(), v)) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kFloat:                                 \
            v = value->m_untyped_constant.float_value().get_ui();       \
            if (close_check(value->m_untyped_constant.float_value(), v)) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          case UntypedConstant::kComplex:                                         \
            v = value->m_untyped_constant.complex_value().real().get_ui(); \
            if (close_check(value->m_untyped_constant.complex_value().real(), v) && \
                value->m_untyped_constant.complex_value().imag() == 0) { \
              value->m_kind = Value::kTypedConstant;                    \
              value->m_type = type;                                     \
              value->ValueMember = v;                                   \
              flag = true;                                              \
            }                                                           \
            break;                                                      \
          default:                                                      \
            break;                                                      \
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
  // assert(IsArithmetic());
  // assert(other.IsArithmetic());
  assert(this->kind() <= other.kind());

  if (this->kind() == other.kind()) {
    return true;
  }

  if (other.kind() == Value::kTypedConstant) {
    return this->ConvertTo(other.type());
  }

  abort();
}

bool Value::ToInteger() {
  struct Visitor : public type::DefaultVisitor {
    explicit Visitor(Value* a_value) : value(a_value) {}
    Value* value;

    void Visit(const type::Int8&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_int8_value));
    }
    void Visit(const type::Int16&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_int16_value));
    }
    void Visit(const type::Int32&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_int32_value));
    }
    void Visit(const type::Int64&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_int64_value));
    }
    void Visit(const type::Uint8&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_uint8_value));
    }
    void Visit(const type::Uint16&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_uint16_value));
    }
    void Visit(const type::Uint32&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_uint32_value));
    }
    void Visit(const type::Uint64&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_uint64_value));
    }
    void Visit(const type::Int&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_int_value));
    }
    void Visit(const type::Uint&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_uint_value));
    }
    void Visit(const type::Uintptr&) override {
      *value = MakeUntypedConstant(UntypedConstant::MakeInteger(value->m_uintptr_value));
    }
  };

  switch (m_kind) {
    case Value::kUntypedConstant:
      {
        UntypedConstant uc = UntypedConstant::ToInteger(m_untyped_constant);
        if (!uc.IsError()) {
          m_untyped_constant = uc;
          return true;
        } else {
          return false;
        }
      }
    case Value::kTypedConstant:
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

bool Value::IsUninitialized() const {
  return m_kind == kUninitialized;
}

bool Value::IsInitialized() const {
  return m_kind != kUninitialized;
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
  //   case kTypedConstant:
  //     return true;
  //   default:
  //     return false;
  // }
}

// TODO(jrw972):  Handle overflow and underflow.

Value Value::Posate(
    Location const & location, Value * x,
    ErrorReporter * error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value * a_value) : x(a_x), value(a_value) {}
    Value const * x;
    Value * value;

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

  assert(x->IsInitialized());

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsArithmetic()) {
    error_reporter->Insert(CannotApply(location, '+', *x));
    return Value::MakeError();
  }

  // TODO(jrw972):  Remove.
  x->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::Posate(x->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        v.m_type = x->m_type;
        Visitor vis(x, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

Value Value::Negate(
    Location const & location, Value * x,
    ErrorReporter * error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value * a_value) : x(a_x), value(a_value) {}
    Value const * x;
    Value * value;

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

  assert(x->IsInitialized());

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsArithmetic()) {
    error_reporter->Insert(CannotApply(location, '-', *x));
    return Value::MakeError();
  }

  // TODO(jrw972):  Remove.
  x->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::Negate(x->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        v.m_type = x->m_type;
        Visitor vis(x, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

Value Value::LogicNot(
    Location const & location, Value * x,
    ErrorReporter * error_reporter) {
  assert(x->IsInitialized());

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsBoolean()) {
    error_reporter->Insert(CannotApply(location, '!', *x));
    return Value::MakeError();
  }

  // TODO(jrw972):  Remove.
  x->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::LogicNot(x->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      v.m_bool_value = !x->m_bool_value;
      v.m_type = x->m_type;
      break;
    case Value::kRValue:
      v.m_type = x->m_type;
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

Value Value::BitNot(
    Location const & location, Value * x,
    ErrorReporter * error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value * a_value) : x(a_x), value(a_value) {}
    Value const * x;
    Value * value;

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

  assert(x->IsInitialized());

  if (x->IsError()) {
    return Value::MakeError();
  }

  if (!x->IsInteger()) {
    error_reporter->Insert(CannotApply(location, '~', *x));
    return Value::MakeError();
  }

  // TODO(jrw972):  Remove.
  x->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::BitNot(x->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        v.m_type = x->m_type;
        Visitor vis(x, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

Value Value::Add(
    Location const & location, Value * x, Value * y,
    ErrorReporter * error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value const * a_y, Value * a_value)
        : x(a_x), y(a_y), value(a_value) {}
    Value const * x;
    Value const * y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!((x->IsArithmetic() && y->IsArithmetic()) ||
       (x->IsString() && y->IsString()))) {
    // TODO(jrw972):  Make this check and other look at the left and right
    // independently.
    error_reporter->Insert(CannotApply2(location, "+", *x, *y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "+", *x, *y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "+", *x, *y));
      return Value::MakeError();
    }
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::Add(x->m_untyped_constant, y->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "+", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
        Visitor vis(x, y, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "+", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

Value Value::Subtract(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value const * a_y, Value * a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    Value const * x;
    Value const * y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsArithmetic() && y->IsArithmetic())) {
    error_reporter->Insert(CannotApply2(location, "-", *x, *y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "-", *x, *y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "-", *x, *y));
      return Value::MakeError();
    }
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::Subtract(x->m_untyped_constant, y->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "-", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
        Visitor vis(x, y, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "-", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }

  return v;
}

Value Value::Multiply(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value const * a_y, Value * a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    Value const * x;
    Value const * y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsArithmetic() && y->IsArithmetic())) {
    error_reporter->Insert(CannotApply2(location, "*", *x, *y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "*", *x, *y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "*", *x, *y));
      return Value::MakeError();
    }
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::Multiply(x->m_untyped_constant, y->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "*", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
        Visitor vis(x, y, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "*", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::Divide(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value const * a_y, Value * a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    Value const * x;
    Value const * y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsArithmetic() && y->IsArithmetic())) {
    error_reporter->Insert(CannotApply2(location, "/", *x, *y));
    return Value::MakeError();
  }

  if (y->IsZero()) {
    error_reporter->Insert(DivisionByZero(location));
    return Value::MakeError();
  }

    if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "/", *x, *y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "/", *x, *y));
      return Value::MakeError();
    }
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::Divide(x->m_untyped_constant, y->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "/", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
        Visitor vis(x, y, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "/", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::Modulo(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value const * a_y, Value * a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    Value const * x;
    Value const * y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && x->IsInteger())) {
    error_reporter->Insert(CannotApply2(location, "%", *x, *y));
    return Value::MakeError();
  }

  if (y->IsZero()) {
    error_reporter->Insert(DivisionByZero(location));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "%", *x, *y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "%", *x, *y));
      return Value::MakeError();
    }
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::Modulo(x->m_untyped_constant, y->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "%", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
        Visitor vis(x, y, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "%", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::LeftShift(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, unsigned int a_y, Value * a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    Value const * x;
    unsigned int y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  {
    Value xx = *x;
    if (!xx.ToInteger()) {
      error_reporter->Insert(CannotApply2(location, "<<", *x, *y));
      return Value::MakeError();
    }
  }

  if (!y->ToInteger() ||
      !y->ConvertTo(&type::Uint::instance)) {
    error_reporter->Insert(CannotApply2(location, "<<", *x, *y));
    return Value::MakeError();
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (x->m_kind) {
    case Value::kUntypedConstant:
      v = MakeUntypedConstant(UntypedConstant::LeftShift(x->m_untyped_constant, y->m_uint_value));
      break;
    case Value::kTypedConstant:
      {
        v.m_kind = Value::kTypedConstant;
        v.m_type = x->m_type;
        Visitor vis(x, y->m_uint_value, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        v.m_kind = Value::kTypedConstant;
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::RightShift(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, unsigned int a_y, Value * a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    Value const * x;
    unsigned int y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  {
    Value xx = *x;
    if (!xx.ToInteger()) {
      error_reporter->Insert(CannotApply2(location, ">>", *x, *y));
      return Value::MakeError();
    }
  }

  if (!y->ToInteger() ||
      !y->ConvertTo(&type::Uint::instance)) {
    error_reporter->Insert(CannotApply2(location, ">>", *x, *y));
    return Value::MakeError();
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (x->m_kind) {
    case Value::kUntypedConstant:
      v = MakeUntypedConstant(UntypedConstant::RightShift(x->m_untyped_constant, y->m_uint_value));
      break;
    case Value::kTypedConstant:
      {
        v.m_kind = Value::kTypedConstant;
        v.m_type = x->m_type;
        Visitor vis(x, y->m_uint_value, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        v.m_kind = Value::kTypedConstant;
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::BitAnd(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value const * a_y, Value * a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    Value const * x;
    Value const * y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_reporter->Insert(CannotApply2(location, "&", *x, *y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "&", *x, *y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "&", *x, *y));
      return Value::MakeError();
    }
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::BitAnd(x->m_untyped_constant, y->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "&", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
        Visitor vis(x, y, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "&", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::BitAndNot(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value const * a_y, Value * a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    Value const * x;
    Value const * y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_reporter->Insert(CannotApply2(location, "&~", *x, *y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "&~", *x, *y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "&~", *x, *y));
      return Value::MakeError();
    }
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::BitAndNot(x->m_untyped_constant, y->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "&~", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
        Visitor vis(x, y, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "&~", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::BitOr(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value const * a_y, Value * a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    Value const * x;
    Value const * y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_reporter->Insert(CannotApply2(location, "|", *x, *y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "|", *x, *y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "|", *x, *y));
      return Value::MakeError();
    }
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::BitOr(x->m_untyped_constant, y->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "|", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
        Visitor vis(x, y, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "|", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::BitXor(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value const * a_y, Value * a_value)
        : x(a_x)
        , y(a_y)
        , value(a_value) {}
    Value const * x;
    Value const * y;
    Value * value;

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

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (!(x->IsInteger() && y->IsInteger())) {
    error_reporter->Insert(CannotApply2(location, "^", *x, *y));
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "^", *x, *y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "^", *x, *y));
      return Value::MakeError();
    }
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  Value v(x->m_kind);
  switch (v.m_kind) {
    case Value::kUntypedConstant:
      v.m_untyped_constant = UntypedConstant::BitXor(x->m_untyped_constant, y->m_untyped_constant);
      break;
    case Value::kTypedConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "^", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
        Visitor vis(x, y, &v);
        x->m_type->UnderlyingType()->Accept(&vis);
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "^", *x, *y));
          return Value::MakeError();
        }
        v.m_type = x->m_type;
      }
      break;
    default:
      abort();  // NOT_COVERED
  }
  return v;
}

Value Value::Equal(
    Location const & location, Value * x, Value * y,
    ErrorReporter* error_reporter) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(Value const * a_x, Value const * a_y)
        : x(a_x) , y(a_y) {}
    Value const * x;
    Value const * y;
    Value flag;

    void Visit(const type::Bool&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_bool_value == y->m_bool_value));
    }
    void Visit(const type::Complex64&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_complex64_value == y->m_complex64_value));
    }
    void Visit(const type::Complex128&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_complex128_value == y->m_complex128_value));
    }
    void Visit(const type::Float32&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_float32_value == y->m_float32_value));
    }
    void Visit(const type::Float64&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_float64_value == y->m_float64_value));
    }
    void Visit(const type::Int&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_int_value == y->m_int_value));
    }
    void Visit(const type::Int8&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_int8_value == y->m_int8_value));
    }
    void Visit(const type::Int16&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_int16_value == y->m_int16_value));
    }
    void Visit(const type::Int32&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_int32_value == y->m_int32_value));
    }
    void Visit(const type::Int64&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_int64_value == y->m_int64_value));
    }
    void Visit(const type::Uint&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_uint_value == y->m_uint_value));
    }
    void Visit(const type::Uint8&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_uint8_value == y->m_uint8_value));
    }
    void Visit(const type::Uint16&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_uint16_value == y->m_uint16_value));
    }
    void Visit(const type::Uint32&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_uint32_value == y->m_uint32_value));
    }
    void Visit(const type::Uint64&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_uint64_value == y->m_uint64_value));
    }
    void Visit(const type::Uintptr&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_uintptr_value == y->m_uintptr_value));
    }
    void Visit(const type::String&) override {
      flag = MakeUntypedConstant(UntypedConstant::MakeBoolean(x->m_string_value == y->m_string_value));
    }
  };

  assert(x->IsInitialized());
  assert(y->IsInitialized());

  if (x->IsError() || y->IsError()) {
    return Value::MakeError();
  }

  if (x->m_kind < y->m_kind) {
    if (!x->PromoteTo(*y)) {
      error_reporter->Insert(CannotApply2(location, "==", *x, *y));
      return Value::MakeError();
    }
  } else if (y->m_kind < x->m_kind) {
    if (!y->PromoteTo(*x)) {
      error_reporter->Insert(CannotApply2(location, "==", *x, *y));
      return Value::MakeError();
    }
  }

  // TODO(jrw972):  Remove.
  x->Dereference();
  y->Dereference();

  switch (x->m_kind) {
    case Value::kUntypedConstant:
      {
        UntypedConstant uc = UntypedConstant::Equal(x->m_untyped_constant, y->m_untyped_constant);
        if (uc.IsError()) {
          error_reporter->Insert(CannotApply2(location, "==", *x, *y));
        }
        return MakeUntypedConstant(uc);
      }
    case Value::kTypedConstant:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "==", *x, *y));
          return Value::MakeError();
        }
        Visitor vis(x, y);
        x->m_type->UnderlyingType()->Accept(&vis);
        return vis.flag;
      }
      break;
    case Value::kRValue:
      {
        if (x->m_type != y->m_type) {
          error_reporter->Insert(CannotApply2(location, "==", *x, *y));
          return Value::MakeError();
        }
        return MakeRValue(&type::Bool::instance);
      }
      break;
    default:
      abort();  // NOT_COVERED
      break;
  }

  abort();
  return Value::MakeError();  // NOT_COVERED
}

Value Value::Call(
    Location const & location, Value * operand,
    std::vector<Value *> const & arguments,
    const LocationList& locations, ErrorReporter* error_reporter) {
  assert(operand->IsInitialized());
  assert(arguments.size() == locations.size());

  if (operand->IsError()) {
    return Value::MakeError();
  }

  if (!operand->IsCallable()) {
    error_reporter->Insert(OperandCannotBeCalled(location));
    return Value::MakeError();
  }

  const type::Function* function =
      type::Cast<const type::Function>(operand->m_type);

  if (arguments.size() != function->ParameterCount()) {
    error_reporter->Insert(
        CallExpectsNArguments(
            location, function->ParameterCount(), arguments.size()));
  }

  for (size_t idx = 0,
           limit = std::min(arguments.size(), function->ParameterCount());
       idx != limit; ++idx) {
    Value * argument = arguments.at(idx);
    if (argument->IsError()) {
      continue;
    }
    symbol::Parameter const * parameter = function->ParameterAt(idx);
    if (!argument->ConvertTo(parameter->type)) {
      error_reporter->Insert(CannotConvert(locations.at(idx), *argument,
                                           parameter->type));
    }
    // TODO(jrw972):  Remove.
    argument->Dereference();
  }

  if (function->ResultCount() == 0) {
    return Value(kVoid);
  } else if (function->ResultCount() == 1) {
    return Value::MakeRValue((*function->ResultBegin())->type);
  } else {
    Value retval(kList);
    for (auto pos = function->ResultBegin(), limit = function->ResultEnd();
         pos != limit; ++pos) {
      const symbol::Parameter* parameter = *pos;
      retval.m_list.push_back(Value::MakeRValue(parameter->type));
    }
    return retval;
  }
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
    case Value::kUninitialized:
      return true;
    case Value::kError:
      return true;
    case Value::kUntypedConstant:
      return this->m_untyped_constant == y.m_untyped_constant;
    case Value::kTypedConstant:
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
      return this->m_type == y.m_type;
    case Value::kType:
      return this->m_type == y.m_type;
    case Value::kVoid:
      return true;
    case Value::kList:
      return this->m_list == y.m_list;
  }

  abort();  // NOT_COVERED
}

bool Value::operator!=(const Value& y) const {
  return !(*this == y);
}

Value Value::MakeError() {
  return Value(kError);
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
    case Value::kUninitialized:
      out << "uninitialized";
      break;
    case Value::kError:
      out << "error";
      break;
    case Value::kUntypedConstant:
      out << value.untyped_constant();
      break;
    case Value::kTypedConstant:
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
    case Value::kVoid:
      out << "<void>";
      break;
    case Value::kList:
      out << "<list>";
      break;
  }
  return out;
}

}  // namespace value
}  // namespace rcgo
