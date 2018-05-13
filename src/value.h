#ifndef SRC_VALUE_H_
#define SRC_VALUE_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <gmpxx.h>
#include <stdint.h>

#include <string>

#define PRECISION 256

namespace rcgo {

// Forward.
namespace type {
struct Type;
}

struct complex64_t {
  float real;
  float imag;

  complex64_t();
  complex64_t(float a_real, float a_imag);

  complex64_t operator+(const complex64_t& a_other) const;
  complex64_t operator-(const complex64_t& a_other) const;
  complex64_t operator*(const complex64_t& a_other) const;
  complex64_t operator/(const complex64_t& a_other) const;
  complex64_t operator+() const;
  complex64_t operator-() const;

  bool operator==(const complex64_t& a_other) const;
};

std::ostream& operator<<(std::ostream& out, const complex64_t& val);

struct complex128_t {
  double real;
  double imag;

  complex128_t();
  complex128_t(double a_real, double a_imag);

  complex128_t operator+(const complex128_t& a_other) const;
  complex128_t operator-(const complex128_t& a_other) const;
  complex128_t operator*(const complex128_t& a_other) const;
  complex128_t operator/(const complex128_t& a_other) const;
  complex128_t operator+() const;
  complex128_t operator-() const;

  bool operator==(const complex128_t& a_other) const;
};

std::ostream& operator<<(std::ostream& out, const complex128_t& val);

struct complex_t {  // NOT_COVERED
  mpf_class real;
  mpf_class imag;

  complex_t();
  complex_t(const mpf_class& a_real, const mpf_class& a_imag);
  bool operator==(const complex_t& a_other) const;
  complex_t operator+(const complex_t& a_other) const;
  complex_t operator-(const complex_t& a_other) const;
  complex_t operator*(const complex_t& a_other) const;
  complex_t operator/(const complex_t& a_other) const;
  complex_t operator+() const;
  complex_t operator-() const;
};

std::ostream& operator<<(std::ostream& out, const complex_t& val);

struct Value {  // NOT_COVERED
  enum Kind {
    kUnknown,
    kError,
    kBoolean,
    kString,
    kInteger,
    kRune,
    kFloat,
    kComplex,
    kTyped,
  };

  Value();
  ~Value();
  static Value MakeError();
  static Value MakeBoolean(bool a_value);
  static Value MakeInteger(const mpz_class& a_value);
  static Value MakeRune(const mpz_class& a_value);
  static Value MakeFloat(const mpf_class& a_value);
  static Value MakeComplex(const mpf_class& a_real, const mpf_class& a_imag);
  static Value MakeString(const std::string& a_value);

  bool IsUnknown() const;
  bool IsError() const;
  bool IsArithmetic() const;
  bool IsSigned() const;
  bool IsInteger() const;
  bool IsZero() const;
  bool IsString() const;
  bool IsBoolean() const;
  Value ConvertTo(const type::Type* type) const;
  Value PromoteTo(const Value& other) const;
  Value ToInteger() const;

  Value Posate() const;
  Value Negate() const;
  Value LogicNot() const;
  Value BitNot() const;

  Value Add(const Value& y) const;
  Value Subtract(const Value& y) const;
  Value Multiply(const Value& y) const;
  Value Divide(const Value& y) const;
  Value Modulo(const Value& y) const;
  Value LeftShift(const Value& y) const;
  Value RightShift(const Value& y) const;
  Value BitAnd(const Value& y) const;
  Value BitAndNot(const Value& y) const;
  Value BitOr(const Value& y) const;
  Value BitXor(const Value& y) const;
  Value Equal(const Value& y) const;

  bool operator==(const Value& other) const;

  Kind kind;
  const type::Type* type;

  bool Boolean_value;
  std::string String_value;
  mpz_class Integer_value;
  mpz_class Rune_value;
  mpf_class Float_value;
  complex_t Complex_value;

  bool bool_value;
  std::string string_value;
  complex64_t complex64_value;
  complex128_t complex128_value;
  float float32_value;
  double float64_value;
  int int_value;
  int8_t int8_value;
  int16_t int16_value;
  int32_t int32_value;
  int64_t int64_value;
  unsigned int uint_value;
  uint8_t uint8_value;
  uint16_t uint16_value;
  uint32_t uint32_value;
  uint64_t uint64_value;
  unsigned int uintptr_value;
};

std::ostream& operator<<(std::ostream& out, const Value& value);

}  // namespace rcgo

#endif  // SRC_VALUE_H_
