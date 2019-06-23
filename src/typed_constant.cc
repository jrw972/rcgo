// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/typed_constant.h"

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

TypedConstant::TypedConstant() : m_type(nullptr) {}

TypedConstant TypedConstant::MakeError() {
  TypedConstant tc;
  tc.m_type = &type::Error::instance;
  return tc;
}

struct ConvertVisitor : public type::DefaultVisitor {
  type::Type const * const type;
  UntypedConstant const & value;
  TypedConstant result;

  explicit ConvertVisitor(type::Type const * a_type,
                          UntypedConstant const & a_value)
      : type(a_type)
      , value(a_value)
  {}

  void Visit(const type::Bool&) override {
    if (value.IsBoolean()) {
      result.m_type = type;
      result.m_bool_value = value.boolean_value();
    } else {
      result = TypedConstant::MakeError();
    }
  }

  void Visit(const type::String&) override {
    if (value.IsString()) {
      result.m_type = type;
      result.m_string_value = value.string_value();
    } else {
      result = TypedConstant::MakeError();
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
    switch (value.kind()) {                                             \
      case UntypedConstant::kInteger:                                   \
        v = ValueType(value.integer_value().get_d(), 0);                \
        if (exact_check(value.integer_value(), v.real())) {             \
          result.m_type = type;                                         \
          result.ValueMember = v;                                       \
        } else {                                                        \
          result = TypedConstant::MakeError();                          \
        }                                                               \
        break;                                                          \
      case UntypedConstant::kRune:                                      \
        v = ValueType(value.rune_value().get_d(), 0);                   \
        if (exact_check(value.rune_value(), v.real())) {                \
          result.m_type = type;                                         \
          result.ValueMember = v;                                       \
        } else {                                                        \
          result = TypedConstant::MakeError();                          \
        }                                                               \
        break;                                                          \
      case UntypedConstant::kFloat:                                     \
        v = ValueType(value.float_value().get_d(), 0);                  \
        if (close_check(value.float_value(), v.real())) {               \
          result.m_type = type;                                         \
          result.ValueMember = v;                                       \
        } else {                                                        \
          result = TypedConstant::MakeError();                          \
        }                                                               \
        break;                                                          \
      case UntypedConstant::kComplex:                                   \
        v =                                                             \
            ValueType(value.complex_value().real().get_d(),             \
                      value.complex_value().imag().get_d());            \
        if (close_check(value.complex_value().real(), v.real()) &&      \
            close_check(value.complex_value().imag(), v.imag())) {      \
          result.m_type = type;                                         \
          result.ValueMember = v;                                       \
        } else {                                                        \
          result = TypedConstant::MakeError();                          \
        }                                                               \
        break;                                                          \
      default:                                                          \
        result = TypedConstant::MakeError();                            \
        break;                                                          \
    }                                                                   \
  }

  CONVERT_TO_COMPLEX(type::Complex64, m_complex64_value, complex64_t)
  CONVERT_TO_COMPLEX(type::Complex128, m_complex128_value, complex128_t)

#define CONVERT_TO_FLOAT(Type, ValueMember, ValueType)          \
  void Visit(const Type&) override                              \
  {                                                             \
    ValueType v;                                                \
    switch (value.kind()) {                                     \
      case UntypedConstant::kInteger:                           \
        v = value.integer_value().get_d();                      \
        if (exact_check(value.integer_value(), v)) {            \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      case UntypedConstant::kRune:                              \
        v = value.rune_value().get_d();                         \
        if (exact_check(value.rune_value(), v)) {               \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      case UntypedConstant::kFloat:                             \
        v = value.float_value().get_d();                        \
        if (close_check(value.float_value(), v)) {              \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      case UntypedConstant::kComplex:                           \
        v = value.complex_value().real().get_d();               \
        if (close_check(value.complex_value().real(), v) &&     \
            value.complex_value().imag() == 0) {                \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      default:                                                  \
        result = TypedConstant::MakeError();                    \
        break;                                                  \
    }                                                           \
  }

  CONVERT_TO_FLOAT(type::Float32, m_float32_value, float)
  CONVERT_TO_FLOAT(type::Float64, m_float64_value, double)

#define CONVERT_TO_INT(Type, ValueMember, ValueType)            \
  void Visit(const Type&) override                              \
  {                                                             \
    ValueType v;                                                \
    switch (value.kind()) {                                     \
      case UntypedConstant::kInteger:                           \
        v = value.integer_value().get_si();                     \
        if (exact_check(value.integer_value(), v)) {            \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      case UntypedConstant::kRune:                              \
        v = value.rune_value().get_si();                        \
        if (exact_check(value.rune_value(), v)) {               \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      case UntypedConstant::kFloat:                             \
        v = value.float_value().get_si();                       \
        if (close_check(value.float_value(), v)) {              \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      case UntypedConstant::kComplex:                           \
        v = value.complex_value().real().get_si();              \
        if (close_check(value.complex_value().real(), v) &&     \
            value.complex_value().imag() == 0) {                \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      default:                                                  \
        result = TypedConstant::MakeError();                    \
        break;                                                  \
    }                                                           \
  }

  CONVERT_TO_INT(type::Int, m_int_value, int)
  CONVERT_TO_INT(type::Int8, m_int8_value, int8_t)
  CONVERT_TO_INT(type::Int16, m_int16_value, int16_t)
  CONVERT_TO_INT(type::Int32, m_int32_value, int32_t)
  CONVERT_TO_INT(type::Int64, m_int64_value, int64_t)

#define CONVERT_TO_UINT(Type, ValueMember, ValueType)           \
  void Visit(const Type&) override                              \
  {                                                             \
    ValueType v;                                                \
    switch (value.kind()) {                                     \
      case UntypedConstant::kInteger:                           \
        v = value.integer_value().get_ui();                     \
        if (exact_check(value.integer_value(), v)) {            \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      case UntypedConstant::kRune:                              \
        v = value.rune_value().get_ui();                        \
        if (exact_check(value.rune_value(), v)) {               \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      case UntypedConstant::kFloat:                             \
        v = value.float_value().get_ui();                       \
        if (close_check(value.float_value(), v)) {              \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      case UntypedConstant::kComplex:                           \
        v = value.complex_value().real().get_ui();              \
        if (close_check(value.complex_value().real(), v) &&     \
            value.complex_value().imag() == 0) {                \
          result.m_type = type;                                 \
          result.ValueMember = v;                               \
        } else {                                                \
          result = TypedConstant::MakeError();                  \
        }                                                       \
        break;                                                  \
      default:                                                  \
        result = TypedConstant::MakeError();                    \
        break;                                                  \
    }                                                           \
  }

  CONVERT_TO_UINT(type::Uint, m_uint_value, unsigned int)
  CONVERT_TO_UINT(type::Uint8, m_uint8_value, uint8_t)
  CONVERT_TO_UINT(type::Uint16, m_uint16_value, uint16_t)
  CONVERT_TO_UINT(type::Uint32, m_uint32_value, uint32_t)
  CONVERT_TO_UINT(type::Uint64, m_uint64_value, uint64_t)
  CONVERT_TO_UINT(type::Uintptr, m_uintptr_value, unsigned int)
};

TypedConstant TypedConstant::Make(type::Type const * a_type,
                                  UntypedConstant const & a_value) {
  assert(a_value.IsInitialized());
  assert(!a_value.IsError());
  ConvertVisitor visitor(a_type, a_value);
  a_type->UnderlyingType()->Accept(&visitor);
  return visitor.result;
}

bool TypedConstant::IsArithmetic() const {
  if (m_type == nullptr) {
    return false;
  }

  return type::IsArithmetic(m_type);
}

bool TypedConstant::IsSigned() const {
  if (m_type == nullptr) {
    return false;
  }

  return type::IsSigned(m_type);
}

bool TypedConstant::IsInteger() const {
  if (m_type == nullptr) {
    return false;
  }

  return type::IsInteger(m_type);
}

bool TypedConstant::IsZero() const {
  struct Visitor : public type::DefaultVisitor {
    explicit Visitor(TypedConstant const & a_x) : x(a_x), flag(false) {}
    TypedConstant const & x;
    bool flag;

    void Visit(type::Int8 const &) override    { flag = x.m_int8_value == 0; }
    void Visit(type::Int16 const &) override   { flag = x.m_int16_value == 0; }
    void Visit(type::Int32 const &) override   { flag = x.m_int32_value == 0; }
    void Visit(type::Int64 const &) override   { flag = x.m_int64_value == 0; }
    void Visit(type::Uint8 const &) override   { flag = x.m_uint8_value == 0; }
    void Visit(type::Uint16 const &) override  { flag = x.m_uint16_value == 0; }
    void Visit(type::Uint32 const &) override  { flag = x.m_uint32_value == 0; }
    void Visit(type::Uint64 const &) override  { flag = x.m_uint64_value == 0; }
    void Visit(type::Int const &) override     { flag = x.m_int_value == 0; }
    void Visit(type::Uint const &) override    { flag = x.m_uint_value == 0; }
    void Visit(type::Uintptr const &) override { flag = x.m_uintptr_value == 0; }
    void Visit(type::Float32 const &) override { flag = x.m_float32_value == 0; }
    void Visit(type::Float64 const &) override { flag = x.m_float64_value == 0; }
    void Visit(type::Complex64 const &) override {
      flag = x.m_complex64_value == complex64_t(0, 0);
    }
    void Visit(type::Complex128 const &) override {
      flag = x.m_complex128_value == complex128_t(0, 0);
    }
  };

  if (m_type == nullptr) {
    return false;
  }

  Visitor visitor(*this);
  m_type->UnderlyingType()->Accept(&visitor);
  return visitor.flag;
}

bool TypedConstant::IsString() const {
  if (m_type == nullptr) {
    return false;
  }

  return type::IsString(m_type);
}

bool TypedConstant::IsBoolean() const {
  if (m_type == nullptr) {
    return false;
  }

  return type::IsBoolean(m_type);
}

bool TypedConstant::IsUninitialized() const {
  return m_type == nullptr;
}

bool TypedConstant::IsInitialized() const {
  return m_type != nullptr;
}

bool TypedConstant::IsError() const {
  return IsInitialized() && m_type == &type::Error::instance;
}

UntypedConstant TypedConstant::ToUntypedConstant() const {
  struct Visitor : public type::DefaultVisitor {
    explicit Visitor(TypedConstant const & a_x)
        : x(a_x) {}

    TypedConstant const & x;
    UntypedConstant value;

    void Visit(const type::Bool&) override {
      value = UntypedConstant::MakeBoolean(x.m_bool_value);
    }
    void Visit(const type::String&) override {
      value = UntypedConstant::MakeString(x.m_string_value);
    }
    void Visit(const type::Int8&) override {
      value = UntypedConstant::MakeInteger(x.m_int8_value);
    }
    void Visit(const type::Int16&) override {
      value = UntypedConstant::MakeInteger(x.m_int16_value);
    }
    void Visit(const type::Int32&) override {
      value = UntypedConstant::MakeInteger(x.m_int32_value);
    }
    void Visit(const type::Int64&) override {
      value = UntypedConstant::MakeInteger(x.m_int64_value);
    }
    void Visit(const type::Uint8&) override {
      value = UntypedConstant::MakeInteger(x.m_uint8_value);
    }
    void Visit(const type::Uint16&) override {
      value = UntypedConstant::MakeInteger(x.m_uint16_value);
    }
    void Visit(const type::Uint32&) override {
      value = UntypedConstant::MakeInteger(x.m_uint32_value);
    }
    void Visit(const type::Uint64&) override {
      value = UntypedConstant::MakeInteger(x.m_uint64_value);
    }
    void Visit(const type::Int&) override {
      value = UntypedConstant::MakeInteger(x.m_int_value);
    }
    void Visit(const type::Uint&) override {
      value = UntypedConstant::MakeInteger(x.m_uint_value);
    }
    void Visit(const type::Uintptr&) override {
      value = UntypedConstant::MakeInteger(x.m_uintptr_value);
    }
    void Visit(const type::Float32&) override {
      value = UntypedConstant::MakeFloat(x.m_float32_value);
    }
    void Visit(const type::Float64&) override {
      value = UntypedConstant::MakeFloat(x.m_float64_value);
    }
    void Visit(const type::Complex64&) override {
      value = UntypedConstant::MakeComplex(x.m_complex64_value.real(), x.m_complex64_value.imag());
    }
    void Visit(const type::Complex128&) override {
      value = UntypedConstant::MakeComplex(x.m_complex128_value.real(), x.m_complex128_value.imag());
    }
    void DefaultAction(const type::Type& type) override {
      abort();
    }
  };

  assert(this->IsInitialized());
  Visitor visitor(*this);
  this->m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

// TODO(jrw972):  Handle overflow and underflow.

TypedConstant TypedConstant::Posate(TypedConstant const & x) {
  struct Visitor : public type::DefaultVisitor {
    explicit Visitor(TypedConstant const & a_x)
        : x(a_x), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = x.m_type;
      value.m_int8_value = +x.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = x.m_type;
      value.m_int16_value = +x.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = x.m_type;
      value.m_int32_value = +x.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = x.m_type;
      value.m_int64_value = +x.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = x.m_type;
      value.m_uint8_value = +x.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = x.m_type;
      value.m_uint16_value = +x.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = x.m_type;
      value.m_uint32_value = +x.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = x.m_type;
      value.m_uint64_value = +x.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = x.m_type;
      value.m_int_value = +x.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = x.m_type;
      value.m_uint_value = +x.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = x.m_type;
      value.m_uintptr_value = +x.m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value.m_type = x.m_type;
      value.m_float32_value = +x.m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value.m_type = x.m_type;
      value.m_float64_value = +x.m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value.m_type = x.m_type;
      value.m_complex64_value = +x.m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value.m_type = x.m_type;
      value.m_complex128_value = +x.m_complex128_value;
    }
  };

  assert(x.IsInitialized());
  Visitor visitor(x);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::Negate(TypedConstant const & x) {
  struct Visitor : public type::DefaultVisitor {
    explicit Visitor(TypedConstant const & a_x)
        : x(a_x), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = x.m_type;
      value.m_int8_value = -x.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = x.m_type;
      value.m_int16_value = -x.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = x.m_type;
      value.m_int32_value = -x.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = x.m_type;
      value.m_int64_value = -x.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = x.m_type;
      value.m_uint8_value = -x.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = x.m_type;
      value.m_uint16_value = -x.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = x.m_type;
      value.m_uint32_value = -x.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = x.m_type;
      value.m_uint64_value = -x.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = x.m_type;
      value.m_int_value = -x.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = x.m_type;
      value.m_uint_value = -x.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = x.m_type;
      value.m_uintptr_value = -x.m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value.m_type = x.m_type;
      value.m_float32_value = -x.m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value.m_type = x.m_type;
      value.m_float64_value = -x.m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value.m_type = x.m_type;
      value.m_complex64_value = -x.m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value.m_type = x.m_type;
      value.m_complex128_value = -x.m_complex128_value;
    }
  };

  assert(x.IsInitialized());
  Visitor visitor(x);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::LogicNot(TypedConstant const & x) {
  struct Visitor : public type::DefaultVisitor {
    explicit Visitor(TypedConstant const & a_x)
        : x(a_x), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant value;

    void Visit(const type::Bool&) override {
      value.m_type = x.m_type;
      value.m_int8_value = !x.m_bool_value;
    }
  };

  assert(x.IsInitialized());
  Visitor visitor(x);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::BitNot(TypedConstant const & x) {
  struct Visitor : public type::DefaultVisitor {
    explicit Visitor(TypedConstant const & a_x)
        : x(a_x), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = x.m_type;
      value.m_int8_value = ~x.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = x.m_type;
      value.m_int16_value = ~x.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = x.m_type;
      value.m_int32_value = ~x.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = x.m_type;
      value.m_int64_value = ~x.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = x.m_type;
      value.m_uint8_value = ~x.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = x.m_type;
      value.m_uint16_value = ~x.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = x.m_type;
      value.m_uint32_value = ~x.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = x.m_type;
      value.m_uint64_value = ~x.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = x.m_type;
      value.m_int_value = ~x.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = x.m_type;
      value.m_uint_value = ~x.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = x.m_type;
      value.m_uintptr_value = ~x.m_uintptr_value;
    }
  };

  assert(x.IsInitialized());
  Visitor visitor(x);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::Add(TypedConstant const & x, TypedConstant const & y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x), y(a_y), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant const & y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int8_value = x.m_int8_value + y.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int16_value = x.m_int16_value + y.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int32_value = x.m_int32_value + y.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int64_value = x.m_int64_value + y.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint8_value = x.m_uint8_value + y.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint16_value = x.m_uint16_value + y.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint32_value = x.m_uint32_value + y.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint64_value = x.m_uint64_value + y.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int_value = x.m_int_value + y.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint_value = x.m_uint_value + y.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uintptr_value = x.m_uintptr_value + y.m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_float32_value = x.m_float32_value + y.m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_float64_value = x.m_float64_value + y.m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_complex64_value = x.m_complex64_value + y.m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_complex128_value = x.m_complex128_value + y.m_complex128_value;
    }
    void Visit(const type::String&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_string_value = x.m_string_value + y.m_string_value;
    }
  };

  assert(x.IsInitialized());
  assert(y.IsInitialized());

  if (!type::Identical(x.m_type, y.m_type)) {
    return MakeError();
  }

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::Subtract(TypedConstant const & x, TypedConstant const & y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x), y(a_y), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant const & y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int8_value = x.m_int8_value - y.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int16_value = x.m_int16_value - y.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int32_value = x.m_int32_value - y.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int64_value = x.m_int64_value - y.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint8_value = x.m_uint8_value - y.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint16_value = x.m_uint16_value - y.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint32_value = x.m_uint32_value - y.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint64_value = x.m_uint64_value - y.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int_value = x.m_int_value - y.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint_value = x.m_uint_value - y.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uintptr_value = x.m_uintptr_value - y.m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_float32_value = x.m_float32_value - y.m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_float64_value = x.m_float64_value - y.m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_complex64_value = x.m_complex64_value - y.m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_complex128_value = x.m_complex128_value - y.m_complex128_value;
    }
  };

  assert(x.IsInitialized());
  assert(y.IsInitialized());

  if (!type::Identical(x.m_type, y.m_type)) {
    return MakeError();
  }

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::Multiply(TypedConstant const & x, TypedConstant const & y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x), y(a_y), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant const & y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int8_value = x.m_int8_value * y.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int16_value = x.m_int16_value * y.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int32_value = x.m_int32_value * y.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int64_value = x.m_int64_value * y.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint8_value = x.m_uint8_value * y.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint16_value = x.m_uint16_value * y.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint32_value = x.m_uint32_value * y.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint64_value = x.m_uint64_value * y.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int_value = x.m_int_value * y.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint_value = x.m_uint_value * y.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uintptr_value = x.m_uintptr_value * y.m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_float32_value = x.m_float32_value * y.m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_float64_value = x.m_float64_value * y.m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_complex64_value = x.m_complex64_value * y.m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_complex128_value = x.m_complex128_value * y.m_complex128_value;
    }
  };

  assert(x.IsInitialized());
  assert(y.IsInitialized());

  if (!type::Identical(x.m_type, y.m_type)) {
    return MakeError();
  }

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::Divide(TypedConstant const & x, TypedConstant const & y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x), y(a_y), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant const & y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int8_value = x.m_int8_value / y.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int16_value = x.m_int16_value / y.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int32_value = x.m_int32_value / y.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int64_value = x.m_int64_value / y.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint8_value = x.m_uint8_value / y.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint16_value = x.m_uint16_value / y.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint32_value = x.m_uint32_value / y.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint64_value = x.m_uint64_value / y.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int_value = x.m_int_value / y.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint_value = x.m_uint_value / y.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uintptr_value = x.m_uintptr_value / y.m_uintptr_value;
    }
    void Visit(const type::Float32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_float32_value = x.m_float32_value / y.m_float32_value;
    }
    void Visit(const type::Float64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_float64_value = x.m_float64_value / y.m_float64_value;
    }
    void Visit(const type::Complex64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_complex64_value = x.m_complex64_value / y.m_complex64_value;
    }
    void Visit(const type::Complex128&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_complex128_value = x.m_complex128_value / y.m_complex128_value;
    }
  };

  assert(x.IsInitialized());
  assert(y.IsInitialized());

  if (!type::Identical(x.m_type, y.m_type)) {
    return MakeError();
  }

  if (y.IsZero()) {
    return MakeError();
  }

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::Modulo(TypedConstant const & x, TypedConstant const & y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x), y(a_y), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant const & y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int8_value = x.m_int8_value % y.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int16_value = x.m_int16_value % y.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int32_value = x.m_int32_value % y.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int64_value = x.m_int64_value % y.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint8_value = x.m_uint8_value % y.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint16_value = x.m_uint16_value % y.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint32_value = x.m_uint32_value % y.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint64_value = x.m_uint64_value % y.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int_value = x.m_int_value % y.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint_value = x.m_uint_value % y.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uintptr_value = x.m_uintptr_value % y.m_uintptr_value;
    }
  };

  assert(x.IsInitialized());
  assert(y.IsInitialized());

  if (!type::Identical(x.m_type, y.m_type)) {
    return MakeError();
  }

  if (y.IsZero()) {
    return MakeError();
  }

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::LeftShift(TypedConstant const & x, unsigned int y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, unsigned int a_y)
        : x(a_x) , y(a_y) , value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    unsigned int const y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = x.m_type;
      value.m_int8_value = x.m_int8_value << y;
    }
    void Visit(const type::Int16&) override {
      value.m_type = x.m_type;
      value.m_int16_value = x.m_int16_value << y;
    }
    void Visit(const type::Int32&) override {
      value.m_type = x.m_type;
      value.m_int32_value = x.m_int32_value << y;
    }
    void Visit(const type::Int64&) override {
      value.m_type = x.m_type;
      value.m_int64_value = x.m_int64_value << y;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = x.m_type;
      value.m_uint8_value = x.m_uint8_value << y;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = x.m_type;
      value.m_uint16_value = x.m_uint16_value << y;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = x.m_type;
      value.m_uint32_value = x.m_uint32_value << y;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = x.m_type;
      value.m_uint64_value = x.m_uint64_value << y;
    }
    void Visit(const type::Int&) override {
      value.m_type = x.m_type;
      value.m_int_value = x.m_int_value << y;
    }
    void Visit(const type::Uint&) override {
      value.m_type = x.m_type;
      value.m_uint_value = x.m_uint_value << y;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = x.m_type;
      value.m_uintptr_value = x.m_uintptr_value << y;
    }
  };

  assert(x.IsInitialized());

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::RightShift(TypedConstant const & x, unsigned int y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, unsigned int a_y)
        : x(a_x) , y(a_y) , value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    unsigned int const y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = x.m_type;
      value.m_int8_value = x.m_int8_value >> y;
    }
    void Visit(const type::Int16&) override {
      value.m_type = x.m_type;
      value.m_int16_value = x.m_int16_value >> y;
    }
    void Visit(const type::Int32&) override {
      value.m_type = x.m_type;
      value.m_int32_value = x.m_int32_value >> y;
    }
    void Visit(const type::Int64&) override {
      value.m_type = x.m_type;
      value.m_int64_value = x.m_int64_value >> y;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = x.m_type;
      value.m_uint8_value = x.m_uint8_value >> y;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = x.m_type;
      value.m_uint16_value = x.m_uint16_value >> y;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = x.m_type;
      value.m_uint32_value = x.m_uint32_value >> y;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = x.m_type;
      value.m_uint64_value = x.m_uint64_value >> y;
    }
    void Visit(const type::Int&) override {
      value.m_type = x.m_type;
      value.m_int_value = x.m_int_value >> y;
    }
    void Visit(const type::Uint&) override {
      value.m_type = x.m_type;
      value.m_uint_value = x.m_uint_value >> y;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = x.m_type;
      value.m_uintptr_value = x.m_uintptr_value >> y;
    }
  };

  assert(x.IsInitialized());

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::BitAnd(TypedConstant const & x, TypedConstant const & y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x), y(a_y), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant const & y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int8_value = x.m_int8_value & y.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int16_value = x.m_int16_value & y.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int32_value = x.m_int32_value & y.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int64_value = x.m_int64_value & y.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint8_value = x.m_uint8_value & y.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint16_value = x.m_uint16_value & y.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint32_value = x.m_uint32_value & y.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint64_value = x.m_uint64_value & y.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int_value = x.m_int_value & y.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint_value = x.m_uint_value & y.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uintptr_value = x.m_uintptr_value & y.m_uintptr_value;
    }
  };

  assert(x.IsInitialized());
  assert(y.IsInitialized());

  if (!type::Identical(x.m_type, y.m_type)) {
    return MakeError();
  }

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::BitAndNot(TypedConstant const & x, TypedConstant const & y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x), y(a_y), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant const & y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int8_value = x.m_int8_value & ~y.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int16_value = x.m_int16_value & ~y.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int32_value = x.m_int32_value & ~y.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int64_value = x.m_int64_value & ~y.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint8_value = x.m_uint8_value & ~y.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint16_value = x.m_uint16_value & ~y.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint32_value = x.m_uint32_value & ~y.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint64_value = x.m_uint64_value & ~y.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int_value = x.m_int_value & ~y.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint_value = x.m_uint_value & ~y.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uintptr_value = x.m_uintptr_value & ~y.m_uintptr_value;
    }
  };

  assert(x.IsInitialized());
  assert(y.IsInitialized());

  if (!type::Identical(x.m_type, y.m_type)) {
    return MakeError();
  }

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::BitOr(TypedConstant const & x, TypedConstant const & y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x), y(a_y), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant const & y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int8_value = x.m_int8_value | y.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int16_value = x.m_int16_value | y.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int32_value = x.m_int32_value | y.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int64_value = x.m_int64_value | y.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint8_value = x.m_uint8_value | y.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint16_value = x.m_uint16_value | y.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint32_value = x.m_uint32_value | y.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint64_value = x.m_uint64_value | y.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int_value = x.m_int_value | y.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint_value = x.m_uint_value | y.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uintptr_value = x.m_uintptr_value | y.m_uintptr_value;
    }
  };

  assert(x.IsInitialized());
  assert(y.IsInitialized());

  if (!type::Identical(x.m_type, y.m_type)) {
    return MakeError();
  }

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

TypedConstant TypedConstant::BitXor(TypedConstant const & x, TypedConstant const & y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x), y(a_y), value(TypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant const & y;
    TypedConstant value;

    void Visit(const type::Int8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int8_value = x.m_int8_value ^ y.m_int8_value;
    }
    void Visit(const type::Int16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int16_value = x.m_int16_value ^ y.m_int16_value;
    }
    void Visit(const type::Int32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int32_value = x.m_int32_value ^ y.m_int32_value;
    }
    void Visit(const type::Int64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int64_value = x.m_int64_value ^ y.m_int64_value;
    }
    void Visit(const type::Uint8&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint8_value = x.m_uint8_value ^ y.m_uint8_value;
    }
    void Visit(const type::Uint16&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint16_value = x.m_uint16_value ^ y.m_uint16_value;
    }
    void Visit(const type::Uint32&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint32_value = x.m_uint32_value ^ y.m_uint32_value;
    }
    void Visit(const type::Uint64&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint64_value = x.m_uint64_value ^ y.m_uint64_value;
    }
    void Visit(const type::Int&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_int_value = x.m_int_value ^ y.m_int_value;
    }
    void Visit(const type::Uint&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uint_value = x.m_uint_value ^ y.m_uint_value;
    }
    void Visit(const type::Uintptr&) override {
      value.m_type = type::Choose(x.m_type, y.m_type);
      value.m_uintptr_value = x.m_uintptr_value ^ y.m_uintptr_value;
    }
  };

  assert(x.IsInitialized());
  assert(y.IsInitialized());

  if (!type::Identical(x.m_type, y.m_type)) {
    return MakeError();
  }

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.value;
}

UntypedConstant TypedConstant::Equal(TypedConstant const & x, TypedConstant const & y) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x) , y(a_y), flag(UntypedConstant::MakeError()) {}

    TypedConstant const & x;
    TypedConstant const & y;
    UntypedConstant flag;

    void Visit(const type::Bool&) override {
      flag = UntypedConstant::MakeBoolean(x.m_bool_value == y.m_bool_value);
    }
    void Visit(const type::Complex64&) override {
      flag = UntypedConstant::MakeBoolean(x.m_complex64_value == y.m_complex64_value);
    }
    void Visit(const type::Complex128&) override {
      flag = UntypedConstant::MakeBoolean(x.m_complex128_value == y.m_complex128_value);
    }
    void Visit(const type::Float32&) override {
      flag = UntypedConstant::MakeBoolean(x.m_float32_value == y.m_float32_value);
    }
    void Visit(const type::Float64&) override {
      flag = UntypedConstant::MakeBoolean(x.m_float64_value == y.m_float64_value);
    }
    void Visit(const type::Int&) override {
      flag = UntypedConstant::MakeBoolean(x.m_int_value == y.m_int_value);
    }
    void Visit(const type::Int8&) override {
      flag = UntypedConstant::MakeBoolean(x.m_int8_value == y.m_int8_value);
    }
    void Visit(const type::Int16&) override {
      flag = UntypedConstant::MakeBoolean(x.m_int16_value == y.m_int16_value);
    }
    void Visit(const type::Int32&) override {
      flag = UntypedConstant::MakeBoolean(x.m_int32_value == y.m_int32_value);
    }
    void Visit(const type::Int64&) override {
      flag = UntypedConstant::MakeBoolean(x.m_int64_value == y.m_int64_value);
    }
    void Visit(const type::Uint&) override {
      flag = UntypedConstant::MakeBoolean(x.m_uint_value == y.m_uint_value);
    }
    void Visit(const type::Uint8&) override {
      flag = UntypedConstant::MakeBoolean(x.m_uint8_value == y.m_uint8_value);
    }
    void Visit(const type::Uint16&) override {
      flag = UntypedConstant::MakeBoolean(x.m_uint16_value == y.m_uint16_value);
    }
    void Visit(const type::Uint32&) override {
      flag = UntypedConstant::MakeBoolean(x.m_uint32_value == y.m_uint32_value);
    }
    void Visit(const type::Uint64&) override {
      flag = UntypedConstant::MakeBoolean(x.m_uint64_value == y.m_uint64_value);
    }
    void Visit(const type::Uintptr&) override {
      flag = UntypedConstant::MakeBoolean(x.m_uintptr_value == y.m_uintptr_value);
    }
    void Visit(const type::String&) override {
      flag = UntypedConstant::MakeBoolean(x.m_string_value == y.m_string_value);
    }
  };

  assert(x.IsInitialized());
  assert(y.IsInitialized());

  if (!type::Identical(x.m_type, y.m_type)) {
    return UntypedConstant::MakeError();
  }

  Visitor visitor(x, y);
  x.m_type->UnderlyingType()->Accept(&visitor);
  return visitor.flag;
}

bool TypedConstant::operator==(const TypedConstant& y) const {
  struct Visitor : public type::DefaultVisitor {
    Visitor(TypedConstant const & a_x, TypedConstant const & a_y)
        : x(a_x)
        , y(a_y)
        , flag(false) {}

    TypedConstant const & x;
    TypedConstant const & y;
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

  if (this->m_type != y.m_type) {
    return false;
  }

  if (this->m_type == nullptr) {
    return true;
  }

  Visitor visitor(*this, y);
  this->m_type->UnderlyingType()->Accept(&visitor);
  return visitor.flag;
}

bool TypedConstant::operator!=(const TypedConstant& y) const {
  return !(*this == y);
}

std::ostream& operator<<(std::ostream& out, const TypedConstant& value) {
  struct Visitor : public type::DefaultVisitor {
    Visitor(std::ostream& a_out, const TypedConstant& a_value)
        : out(a_out)
        , value(a_value) {}

    std::ostream& out;
    const TypedConstant& value;

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

  if (value.IsInitialized()) {
    Visitor visitor(out, value);
    value.type()->UnderlyingType()->Accept(&visitor);
  } else {
    out << "<uninitialized>";
  }

  return out;
}

}  // namespace value
}  // namespace rcgo
