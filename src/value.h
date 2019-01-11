
#ifndef SRC_VALUE_H_
#define SRC_VALUE_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <gmpxx.h>
#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "src/location.h"

#define PRECISION 256

namespace rcgo {

// Forward.
namespace type {
struct Type;
struct Function;
}

struct ErrorReporter;

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

struct complex_t {
  complex_t();
  complex_t(const mpf_class& a_real, const mpf_class& a_imag);

  const mpf_class real() const { return m_real; }
  const mpf_class imag() const { return m_imag; }

  complex_t operator+(const complex_t& a_other) const;
  complex_t operator-(const complex_t& a_other) const;
  complex_t operator*(const complex_t& a_other) const;
  complex_t operator/(const complex_t& a_other) const;
  complex_t operator+() const;
  complex_t operator-() const;

  bool operator==(const complex_t& a_other) const;
  bool operator!=(const complex_t& a_other) const;

 private:
  mpf_class m_real;
  mpf_class m_imag;
};

std::ostream& operator<<(std::ostream& out, const complex_t& val);

struct Value;

typedef std::shared_ptr<Value> ValuePtr;
typedef std::shared_ptr<const Value> ConstValuePtr;

struct Value {
  enum Kind {
    kError,
    kBoolean,
    kString,
    kInteger,
    kRune,
    kFloat,
    kComplex,
    kConstant,  // with a type
    kFunction,
    kLValue,
    kRValue,
    kType,
    kList
  };

  ValuePtr DeepCopy() const;

  static ValuePtr MakeError();
  static ValuePtr MakeBoolean(bool a_value);
  static ValuePtr MakeInteger(const mpz_class& a_value);
  static ValuePtr MakeRune(const mpz_class& a_value);
  static ValuePtr MakeFloat(const mpf_class& a_value);
  static ValuePtr MakeComplex(const mpf_class& a_real, const mpf_class& a_imag);
  static ValuePtr MakeString(const std::string& a_value);
  static ValuePtr MakeFunction(const type::Function* a_type);
  static ValuePtr MakeLValue(const type::Type* a_type);
  static ValuePtr MakeRValue(const type::Type* a_type);
  static ValuePtr MakeType(const type::Type* a_type);
  static ValuePtr MakeList();

  void Push(ValuePtr element);

  Kind kind() const { return m_kind; }
  const type::Type* type() const { return m_type; }
  bool Boolean_value() const { return m_Boolean_value; }
  const std::string& String_value() const { return  m_String_value; }
  const mpz_class& Integer_value() const { return m_Integer_value; }
  const mpz_class& Rune_value() const { return m_Rune_value; }
  const mpf_class& Float_value() const { return m_Float_value; }
  const complex_t& Complex_value() const { return m_Complex_value; }

  bool bool_value() const { return m_bool_value; }
  const std::string& string_value() const { return m_string_value; }
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

  bool IsError() const;

  bool RequireConstant(ErrorReporter* error_reporter) const;
  bool ConvertTo(const type::Type* type);

  // Operations.
  static ValuePtr Posate(const Location& location, ValuePtr x,
                         ErrorReporter* error_reporter);
  static ValuePtr Negate(const Location& location, ValuePtr x,
                         ErrorReporter* error_reporter);
  static ValuePtr LogicNot(const Location& location, ValuePtr x,
                           ErrorReporter* error_reporter);
  static ValuePtr BitNot(const Location& location, ValuePtr x,
                         ErrorReporter* error_reporter);
  // Operations and implicit conversions.
  static ValuePtr Add(const Location& location, ValuePtr x, ValuePtr y,
                      ErrorReporter* error_reporter);
  static ValuePtr Subtract(const Location& location, ValuePtr x, ValuePtr y,
                           ErrorReporter* error_reporter);
  static ValuePtr Multiply(const Location& location, ValuePtr x, ValuePtr y,
                           ErrorReporter* error_reporter);
  static ValuePtr Divide(const Location& location, ValuePtr x, ValuePtr y,
                         ErrorReporter* error_reporter);
  static ValuePtr Modulo(const Location& location, ValuePtr x, ValuePtr y,
                         ErrorReporter* error_reporter);
  static ValuePtr LeftShift(const Location& location, ValuePtr x, ValuePtr y,
                            ErrorReporter* error_reporter);
  static ValuePtr RightShift(const Location& location, ValuePtr x, ValuePtr y,
                             ErrorReporter* error_reporter);
  static ValuePtr BitAnd(const Location& location, ValuePtr x, ValuePtr y,
                         ErrorReporter* error_reporter);
  static ValuePtr BitAndNot(const Location& location, ValuePtr x, ValuePtr y,
                            ErrorReporter* error_reporter);
  static ValuePtr BitOr(const Location& location, ValuePtr x, ValuePtr y,
                        ErrorReporter* error_reporter);
  static ValuePtr BitXor(const Location& location, ValuePtr x, ValuePtr y,
                         ErrorReporter* error_reporter);
  static ValuePtr Equal(const Location& location, ValuePtr x, ValuePtr y,
                        ErrorReporter* error_reporter);

  static ValuePtr Call(const Location& location, ValuePtr operand,
                       ValuePtr arguments, const LocationList& locations,
                       ErrorReporter* error_reporter);

  bool operator==(const Value& y) const;
  bool operator!=(const Value& y) const;

 private:
  explicit Value(Kind kind);

  bool IsArithmetic() const;
  bool IsSigned() const;
  bool IsInteger() const;
  bool IsZero() const;
  bool IsString() const;
  bool IsBoolean() const;
  bool IsCallable() const;

  // Explicit conversions and checks.
  bool PromoteTo(const Value& other);
  bool ToInteger();
  void Dereference();

  Kind m_kind;
  const type::Type* m_type;

  bool m_Boolean_value;
  std::string m_String_value;
  mpz_class m_Integer_value;
  mpz_class m_Rune_value;
  mpf_class m_Float_value;
  complex_t m_Complex_value;

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

  typedef std::vector<ValuePtr> ListType;
  ListType m_list;

  friend struct ConvertVisitor;
};

std::ostream& operator<<(std::ostream& out, const Value& value);

}  // namespace value
}  // namespace rcgo

#endif  // SRC_VALUE_H_
