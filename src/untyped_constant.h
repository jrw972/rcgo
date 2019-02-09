#ifndef SRC_UNTYPED_CONSTANT_H_
#define SRC_UNTYPED_CONSTANT_H_

// Copyright 2019 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <gmpxx.h>

#include <string>

#define PRECISION 256

namespace rcgo {
namespace value {

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

struct UntypedConstant {
  enum Kind {
    kUninitialized,
    kError,
    kBoolean,
    kString,
    kInteger,
    kRune,
    kFloat,
    kComplex,
  };

  UntypedConstant();

  static UntypedConstant MakeError();
  static UntypedConstant MakeBoolean(bool a_value);
  static UntypedConstant MakeInteger(mpz_class const & a_value);
  static UntypedConstant MakeRune(mpz_class const & a_value);
  static UntypedConstant MakeFloat(mpf_class const & a_value);
  static UntypedConstant MakeComplex(
      mpf_class const & a_real,
      mpf_class const & a_imag);
  static UntypedConstant MakeComplex(complex_t const & a_value);

  static UntypedConstant MakeString(std::string const & a_value);

  Kind kind() const { return m_kind; }
  bool boolean_value() const { return m_boolean_value; }
  std::string const & string_value() const { return  m_string_value; }
  mpz_class const & integer_value() const { return m_integer_value; }
  mpz_class const & rune_value() const { return m_rune_value; }
  mpf_class const & float_value() const { return m_float_value; }
  complex_t const & complex_value() const { return m_complex_value; }

  bool IsInitialized() const;
  bool IsError() const;
  bool IsArithmetic() const;
  bool IsSigned() const;
  bool IsInteger() const;
  bool IsZero() const;
  bool IsString() const;
  bool IsBoolean() const;

  // Operations.
  static UntypedConstant ToInteger(UntypedConstant const & x);
  static UntypedConstant Posate(UntypedConstant const & x);
  static UntypedConstant Negate(UntypedConstant const & x);
  static UntypedConstant LogicNot(UntypedConstant const & x);
  static UntypedConstant BitNot(UntypedConstant const & x);
  // Operations and implicit conversions.
  static UntypedConstant Add(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant Subtract(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant Multiply(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant Divide(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant Modulo(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant LeftShift(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant RightShift(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant BitAnd(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant BitAndNot(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant BitOr(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant BitXor(
      UntypedConstant const & x,
      UntypedConstant const & y);
  static UntypedConstant Equal(
      UntypedConstant const & x,
      UntypedConstant const & y);

  bool operator==(UntypedConstant const & a_other) const;

 private:
  explicit UntypedConstant(Kind a_kind);

  static bool IsArithmetic(Kind a_kind);
  static UntypedConstant PromoteTo(
      UntypedConstant const & a_x,
      Kind a_kind);

  Kind m_kind;
  bool m_boolean_value;
  std::string m_string_value;
  mpz_class m_integer_value;
  mpz_class m_rune_value;
  mpf_class m_float_value;
  complex_t m_complex_value;
};

std::ostream & operator<<(std::ostream & out, UntypedConstant const & value);

}  // namespace value
}  // namespace rcgo

#endif  // SRC_UNTYPED_CONSTANT_H_
