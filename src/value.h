#ifndef SRC_VALUE_H_
#define SRC_VALUE_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <stdint.h>

#include <string>
#include <vector>

#include "src/location.h"
#include "src/untyped_constant.h"

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

struct Value {
  enum Kind {
    kUninitialized,
    kError,
    kUntypedConstant,
    kInteger,
    kRune,
    kFloat,
    kComplex,
    kConstant,  // with a type
    kFunction,
    kLValue,
    kRValue,
    kType,
    kVoid,
    kList
  };

  Value();
  Value(Value const & a_value);

  static Value MakeError();
  static Value MakeUntypedConstant(UntypedConstant const & a_value);
  static Value MakeBoolean(bool a_value);
  static Value MakeInteger(mpz_class const & a_value);
  static Value MakeRune(mpz_class const & a_value);
  static Value MakeFloat(mpf_class const & a_value);
  static Value MakeComplex(mpf_class const & a_real, mpf_class const & a_imag);
  static Value MakeString(std::string const & a_value);
  static Value MakeFunction(type::Function const * a_type);
  static Value MakeLValue(type::Type const * a_type);
  static Value MakeRValue(type::Type const * a_type);
  static Value MakeType(type::Type const * a_type);

  Kind kind() const { return m_kind; }
  type::Type const * type() const { return m_type; }
  UntypedConstant const & untyped_constant() const { return m_untyped_constant; }
  bool Boolean_value() const { return m_untyped_constant.boolean_value(); }
  std::string const & String_value() const { return  m_untyped_constant.string_value(); }
  mpz_class const & Integer_value() const { return m_Integer_value; }
  mpz_class const & Rune_value() const { return m_Rune_value; }
  mpf_class const & Float_value() const { return m_Float_value; }
  complex_t const & Complex_value() const { return m_Complex_value; }

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

  bool RequireConstant(ErrorReporter * error_reporter) const;
  bool ConvertTo(type::Type const * type);

  // Operations.
  static Value Posate(Location const & location, Value * x,
                      ErrorReporter * error_reporter);
  static Value Negate(Location const & location, Value * x,
                      ErrorReporter * error_reporter);
  static Value LogicNot(Location const & location, Value * x,
                        ErrorReporter * error_reporter);
  static Value BitNot(Location const & location, Value * x,
                      ErrorReporter * error_reporter);
  // Operations and implicit conversions.
  static Value Add(Location const & location, Value * x, Value * y,
                   ErrorReporter * error_reporter);
  static Value Subtract(Location const & location, Value * x, Value * y,
                        ErrorReporter * error_reporter);
  static Value Multiply(Location const & location, Value * x, Value * y,
                        ErrorReporter * error_reporter);
  static Value Divide(Location const & location, Value * x, Value * y,
                      ErrorReporter * error_reporter);
  static Value Modulo(Location const & location, Value * x, Value * y,
                      ErrorReporter * error_reporter);
  static Value LeftShift(Location const & location, Value * x, Value * y,
                         ErrorReporter * error_reporter);
  static Value RightShift(Location const & location, Value * x, Value * y,
                          ErrorReporter * error_reporter);
  static Value BitAnd(Location const & location, Value * x, Value * y,
                      ErrorReporter * error_reporter);
  static Value BitAndNot(Location const & location, Value * x, Value * y,
                         ErrorReporter * error_reporter);
  static Value BitOr(Location const & location, Value * x, Value * y,
                     ErrorReporter * error_reporter);
  static Value BitXor(Location const & location, Value * x, Value * y,
                      ErrorReporter * error_reporter);
  static Value Equal(Location const & location, Value * x, Value * y,
                     ErrorReporter * error_reporter);

  static Value Call(Location const & location, Value * operand,
                    std::vector<Value *> const & arguments,
                    LocationList const & locations,
                    ErrorReporter * error_reporter);

  bool operator==(Value const & y) const;
  bool operator!=(Value const & y) const;

 private:
  explicit Value(Kind a_kind);
  bool IsArithmetic() const;
  bool IsSigned() const;
  bool IsInteger() const;
  bool IsZero() const;
  bool IsString() const;
  bool IsBoolean() const;
  bool IsCallable() const;

  // Explicit conversions and checks.
  bool PromoteTo(Value const & other);
  bool ToInteger();
  void Dereference();

  Kind m_kind;
  type::Type const * m_type;

  UntypedConstant m_untyped_constant;
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

  typedef std::vector<Value> ListType;
  ListType m_list;

  friend struct ConvertVisitor;
};

std::ostream& operator<<(std::ostream & out, Value const & value);

}  // namespace value
}  // namespace rcgo

#endif  // SRC_VALUE_H_
