#ifndef SRC_TYPED_CONSTANT_H_
#define SRC_TYPED_CONSTANT_H_

// Copyright 2019 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <stdint.h>

#include <string>

#include "src/untyped_constant.h"

namespace rcgo {

// Forward.
namespace type {
struct Type;
}

namespace value {

struct complex64_t {
  complex64_t();
  complex64_t(float a_real, float a_imag);

  float real() const { return m_real; }
  float imag() const { return m_imag; }

  complex64_t operator+(const complex64_t& a_other) const;
  complex64_t operator-(const complex64_t& a_other) const;
  complex64_t operator*(const complex64_t& a_other) const;
  complex64_t operator/(const complex64_t& a_other) const;
  complex64_t operator+() const;
  complex64_t operator-() const;

  bool operator==(const complex64_t& a_other) const;
  bool operator!=(const complex64_t& a_other) const;

 private:
  float m_real;
  float m_imag;
};

std::ostream& operator<<(std::ostream& out, const complex64_t& val);

struct complex128_t {
  complex128_t();
  complex128_t(double a_real, double a_imag);

  double real() const { return m_real; }
  double imag() const { return m_imag; }

  complex128_t operator+(const complex128_t& a_other) const;
  complex128_t operator-(const complex128_t& a_other) const;
  complex128_t operator*(const complex128_t& a_other) const;
  complex128_t operator/(const complex128_t& a_other) const;
  complex128_t operator+() const;
  complex128_t operator-() const;

  bool operator==(const complex128_t& a_other) const;
  bool operator!=(const complex128_t& a_other) const;

 private:
  double m_real;
  double m_imag;
};

std::ostream& operator<<(std::ostream& out, const complex128_t& val);

struct TypedConstant {
  TypedConstant();

  static TypedConstant MakeError();
  static TypedConstant Make(type::Type const * a_type,
                            UntypedConstant const & a_value);

  type::Type const * type() const { return m_type; }

  bool bool_value() const { return m_bool_value; }
  std::string const & string_value() const { return m_string_value; }
  complex64_t complex64_value() const { return m_complex64_value; }
  complex128_t complex128_value() const { return m_complex128_value; }
  float float32_value() const { return m_float32_value; }
  double float64_value() const { return m_float64_value; }
  int int_value() const { return m_int_value; }
  int8_t int8_value() const { return m_int8_value; }
  int16_t int16_value() const { return m_int16_value; }
  int32_t int32_value() const { return m_int32_value; }
  int64_t int64_value() const { return m_int64_value; }
  unsigned int uint_value() const { return m_uint_value; }
  uint8_t uint8_value() const { return m_uint8_value; }
  uint16_t uint16_value() const { return m_uint16_value; }
  uint32_t uint32_value() const { return m_uint32_value; }
  uint64_t uint64_value() const { return m_uint64_value; }
  unsigned int uintptr_value() const { return m_uintptr_value; }

  bool IsUninitialized() const;
  bool IsInitialized() const;
  bool IsError() const;

  bool IsZero() const;

  UntypedConstant ToUntypedConstant() const;
  static TypedConstant Posate(TypedConstant const & x);
  static TypedConstant Negate(TypedConstant const & x);
  static TypedConstant LogicNot(TypedConstant const & x);
  static TypedConstant BitNot(TypedConstant const & x);
  static TypedConstant Add(TypedConstant const & x, TypedConstant const & y);
  static TypedConstant Subtract(TypedConstant const & x, TypedConstant const & y);
  static TypedConstant Multiply(TypedConstant const & x, TypedConstant const & y);
  static TypedConstant Divide(TypedConstant const & x, TypedConstant const & y);
  static TypedConstant Modulo(TypedConstant const & x, TypedConstant const & y);
  static TypedConstant LeftShift(TypedConstant const & x, unsigned int y);
  static TypedConstant RightShift(TypedConstant const & x, unsigned int y);
  static TypedConstant BitAnd(TypedConstant const & x, TypedConstant const & y);
  static TypedConstant BitAndNot(TypedConstant const & x, TypedConstant const & y);
  static TypedConstant BitOr(TypedConstant const & x, TypedConstant const & y);
  static TypedConstant BitXor(TypedConstant const & x, TypedConstant const & y);
  static UntypedConstant Equal(TypedConstant const & x, TypedConstant const & y);

  bool operator==(TypedConstant const & y) const;
  bool operator!=(TypedConstant const & y) const;

 private:
  bool IsArithmetic() const;
  bool IsSigned() const;
  bool IsInteger() const;
  bool IsString() const;
  bool IsBoolean() const;

  type::Type const * m_type;

  bool m_bool_value;
  std::string m_string_value;
  complex64_t m_complex64_value;
  complex128_t m_complex128_value;
  float m_float32_value;
  double m_float64_value;
  int m_int_value;
  int8_t m_int8_value;
  int16_t m_int16_value;
  int32_t m_int32_value;
  int64_t m_int64_value;
  unsigned int m_uint_value;
  uint8_t m_uint8_value;
  uint16_t m_uint16_value;
  uint32_t m_uint32_value;
  uint64_t m_uint64_value;
  unsigned int m_uintptr_value;

  friend struct ConvertVisitor;
};

std::ostream& operator<<(std::ostream & out, TypedConstant const & value);

}  // namespace value
}  // namespace rcgo

#endif  // SRC_TYPED_CONSTANT_H_
