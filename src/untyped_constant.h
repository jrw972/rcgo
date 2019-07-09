#ifndef SRC_UNTYPED_CONSTANT_H_
#define SRC_UNTYPED_CONSTANT_H_

// Copyright 2019 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <gmpxx.h>

#include <complex>
#include <string>

#include "src/error.h"

/** @file */

/// Number of bits for untyped floating-point constants.
#define PRECISION 256

namespace rcgo {
namespace value {

/// Implementation of untyped complex numbers.
struct complex_t {
    /**
       Construct a complex number with real and imaginary parts set to zero.
       @returns A complex number with real and imaginary parts set to zero.
    */
  complex_t();
    /**
       Construct a complex number with the given parts.
       @param a_real the real part
       @param a_imag the imaginary part
       @returns A complex number with the given parts.
     */
  complex_t(const mpf_class& a_real, const mpf_class& a_imag);

    /**
       Access the real part of a complex number.
       @returns The real part of the complex number.
     */
  const mpf_class real() const { return m_real; }
    /**
       Access the imaginary part of a complex number.
       @returns The imaginary part of the complex number.
    */
  const mpf_class imag() const { return m_imag; }


    /**
       Add complex numbers.
       @param a_other The addend.
       @returns The sum
    */
  complex_t operator+(const complex_t& a_other) const;
    /**
       Subtract complex numbers.
       @param a_other The subtrahend.
       @returns The difference
    */
  complex_t operator-(const complex_t& a_other) const;
    /**
       Multiply complex numbers.
       @param a_other The multiplier.
       @returns The product
    */
  complex_t operator*(const complex_t& a_other) const;
    /**
       Divide complex numbers.
       @param a_other The divisor.
       @warning The divisor must not be zero.
       @returns The quotient
    */
  complex_t operator/(const complex_t& a_other) const;
    /**
       Posate a complex number.
       @returns The complex number
    */
  complex_t operator+() const;
    /**
       Negate a complex number.
       @returns The additive inverse of the complex number
    */
  complex_t operator-() const;

    /**
       Compare complex numbers for equality.
       @param a_other The complex number to compare.
       @returns True if the complex numbers are equal, false otherwise.
    */
  bool operator==(const complex_t& a_other) const;
    /**
       Compare complex numbers for inequality.
       @param a_other The complex number to compare.
       @returns True if the complex numbers are not equal, false otherwise.
    */
  bool operator!=(const complex_t& a_other) const;

 private:
  mpf_class m_real;
  mpf_class m_imag;
};

    /**
       Insert an untyped complex constant into a stream.
       @param out the stream
       @param val the untyped complex constant
       @returns the stream
     */
std::ostream& operator<<(std::ostream& out, const complex_t& val);

    /**
       Implementation of untyped constants.
    */
struct UntypedConstant {
  /// Indicates the different kinds of untyped constants.  @see kind()
  enum Kind {
    /// untyped constant is uninitialized
    kUninitialized,
    /// untyped constant is a boolean
    kBoolean,
    /// untyped constant is a string
    kString,
    /// untyped constant is an integer
    kInteger,
    /// untyped constant is a rune
    kRune,
    /// untyped constant is a float
    kFloat,
    /// untyped constant is a complex number
    kComplex,
  };

    /**
       Construct an uninitialized untyped constant.
       @returns An uninitialized untyped constant.
       @post kind() == kUninitialized
    */
  UntypedConstant();

    /**
       Construct a boolean untyped constant
       @param a_value The value of the constant
       @returns a boolean untyped constant with the given value
       @post kind() == kBoolean
    */
  static UntypedConstant MakeBoolean(bool a_value);
    /**
       Construct a string untyped constant
       @param a_value The value of the constant
       @returns a string untyped constant with the given value
       @post kind() == kString
    */
  static UntypedConstant MakeString(std::string const& a_value);
    /**
       Construct an integer untyped constant
       @param a_value The value of the constant
       @returns an integer untyped constant with the given value
       @post kind() == kInteger
    */
  static UntypedConstant MakeInteger(mpz_class const& a_value);
    /**
       Construct a rune untyped constant
       @param a_value The value of the constant
       @returns a rune untyped constant with the given value
       @post kind() == kRune
    */
  static UntypedConstant MakeRune(mpz_class const& a_value);
    /**
       Construct a float untyped constant
       @param a_value The value of the constant
       @returns a float untyped constant with the given value
       @post kind() == kFloat
    */
  static UntypedConstant MakeFloat(mpf_class const& a_value);
    /**
       Construct a complex untyped constant
       @param a_real The real part of the constant
       @param a_imag The imaginary part of the constant
       @returns a complex untyped constant with the given parts
       @post kind() == kComplex
    */
  static UntypedConstant MakeComplex(mpf_class const& a_real, mpf_class const& a_imag);
    /**
       Construct a complex untyped constant
       @param a_value The value of the constant
       @returns a complex untyped constant with the given value
       @post kind() == kComplex
    */
  static UntypedConstant MakeComplex(complex_t const& a_value);

    /**
       Get the kind of the untyped constant
       @returns the kind of the untyped constant
    */
  Kind kind() const { return m_kind; }
    /**
       Get the boolean value of the untyped constant
       @returns the boolean value of the untyped constant
       @pre kind() == kBoolean
    */
  bool boolean_value() const;
    /**
       Get the string value of the untyped constant
       @returns the string value of the untyped constant
       @pre kind() == kString
    */
  std::string const& string_value() const;
    /**
       Get the integer value of the untyped constant
       @returns the integer value of the untyped constant
       @pre kind() == kInteger
    */
  mpz_class const& integer_value() const;
    /**
       Get the rune value of the untyped constant
       @returns the rune value of the untyped constant
       @pre kind() == kRune
    */
  mpz_class const& rune_value() const;
    /**
       Get the float value of the untyped constant
       @returns the float value of the untyped constant
       @pre kind() == kFloat
    */
  mpf_class const& float_value() const;
    /**
       Get the complex value of the untyped constant
       @returns the complex value of the untyped constant
       @pre kind() == kComplex
    */
  complex_t const& complex_value() const;

    /**
       @returns true when the untyped constant is not uninitialized
    */
  bool IsInitialized() const;
    /**
       @returns true when the untyped constant is an integer, rune, float, or complex
    */
  bool IsArithmetic() const;
    /**
       @returns true when the untyped constant is an integer, rune, float, or complex
    */
  bool IsSigned() const;
    /**
       @returns true when the untyped constant is an integer or rune.
    */
  bool IsInteger() const;
    /**
       @returns true when the untyped constant is zero.
    */
  bool IsZero() const;
    /**
       @returns true when the untyped constant is a string.
    */
  bool IsString() const;
    /**
       @returns true when the untyped constant is a boolean.
    */
  bool IsBoolean() const;
   /**
      Determine if two untyped constants are comparable.
      The constants must either both be booleans, both be arithmetic, or both be strings.
      @param x
      @param y
      @returns true when the untyped constants are comparable.
    */
  static bool AreComparable(UntypedConstant const& x, UntypedConstant const& y);

    /**
       Posate an untyped constant.
       @param x
       @returns the untyped constant
       @pre x must be arithmetic
    */
  static UntypedConstant Posate(UntypedConstant const& x);
    /**
       Negate an untyped constant.
       @param x
       @returns the additive inverse of the untyped constant
       @pre x must be arithmetic
    */
  static UntypedConstant Negate(UntypedConstant const& x);
    /**
       Logical invert an untyped constant.
       @param x
       @returns the logical inverse of the untyped constant
       @pre x must be boolean
    */
  static UntypedConstant LogicNot(UntypedConstant const& x);
    /**
       Compute the bit-wise complement of an untyped constant.
       @param x
       @returns the bit-wise complement of the untyped constant
       @pre x must be an integer or rune
    */
  static UntypedConstant BitNot(UntypedConstant const& x);
    /**
       Add or concatenate untyped constants.
       @param x the addend
       @param y the addend
       @returns the sum (arithemetic) or concatenation (string) of the untyped constants
       @pre x and y must be arithemtic or a string
    */
  static UntypedConstant Add(UntypedConstant const& x, UntypedConstant const& y);
    /**
       Subtract untyped constants.
       @param x the minuend
       @param y the subtrahend
       @returns the difference
       @pre x and y must be arithemtic
    */
  static UntypedConstant Subtract(UntypedConstant const& x, UntypedConstant const& y);
    /**
       Multiply untyped constants.
       @param x the multiplican
       @param y the multiplier
       @returns the product
       @pre x and y must be arithemtic
    */
  static UntypedConstant Multiply(UntypedConstant const& x, UntypedConstant const& y);
    /**
       Divide untyped constants.
       @param x the dividend
       @param y the divisor
       @returns the quotient
       @pre x and y must be arithemtic
       @pre y must not be zero
    */
  static UntypedConstant Divide(UntypedConstant const& x, UntypedConstant const& y);
    /**
       Compute the modulus of untyped constants.
       @param x the dividend
       @param y the divisor
       @returns the modulus
       @pre x and y must be arithemtic
       @pre y must not be zero
    */
  static UntypedConstant Modulo(UntypedConstant const& x, UntypedConstant const& y);
    /**
       Shift the bits of the untyped constant to the left.
       @param x the bits to shift
       @param y the amount to shift
       @returns the shifted constant
       @pre x must be convertible to an integer
    */
  static UntypedConstant LeftShift(UntypedConstant const& x, unsigned int y);
    /**
       Shift the bits of the untyped constant to the right.
       @param x the bits to shift
       @param y the amount to shift
       @returns the shifted constant
       @pre x must be convertible to an integer
    */
  static UntypedConstant RightShift(UntypedConstant const& x, unsigned int y);
    /**
       Compute the bit-wise logical and of untyped constants.
       @param x
       @param y
       @returns the bit-wise and
       @pre x and y must be integers
    */
  static UntypedConstant BitAnd(UntypedConstant const& x, UntypedConstant const& y);
    /**
       Compute the bit-wise logical and-not of untyped constants.
       @param x
       @param y
       @returns the bit-wise and of x and not y
       @pre x and y must be integers
    */
  static UntypedConstant BitAndNot(UntypedConstant const& x, UntypedConstant const& y);
    /**
       Compute the bit-wise logical or of untyped constants.
       @param x
       @param y
       @returns the bit-wise or
       @pre x and y must be integers
    */
  static UntypedConstant BitOr(UntypedConstant const& x, UntypedConstant const& y);
    /**
       Compute the bit-wise logical exclusive or of untyped constants.
       @param x
       @param y
       @returns the bit-wise exclusive or
       @pre x and y must be integers
    */
  static UntypedConstant BitXor(UntypedConstant const& x, UntypedConstant const& y);
    /**
       Compare untyped constants for equality.
       @param x
       @param y
       @returns true if x and y are equal, false otherwise
       @pre x and y must be comparable (AreComparable)
    */
  static UntypedConstant Equal(UntypedConstant const& x, UntypedConstant const& y);
    /**
       Convert the untyped constant to an integer or rune
       Float and complex constants are converted to integers if they can be represented by an integer.

       @returns an integer or rune if successful and an uninitialized constant otherwise
     */
  UntypedConstant ToInteger() const;

    /**
       Compare untyped constants
       @param a_other
       @returns true if the constants are equal, false otherwise
     */
  bool operator==(UntypedConstant const& a_other) const;

 private:
  explicit UntypedConstant(Kind a_kind);

  static bool IsArithmetic(Kind a_kind);
  static UntypedConstant PromoteTo(UntypedConstant const& a_x, Kind a_kind);

  Kind m_kind;
  bool m_boolean_value;
  std::string m_string_value;
  mpz_class m_integer_value;
  mpz_class m_rune_value;
  mpf_class m_float_value;
  complex_t m_complex_value;
};

    /**
       Insert an untyped constant into a stream.
       @param out the stream
       @param value the untyped constant
       @returns the stream
    */
std::ostream& operator<<(std::ostream& out, UntypedConstant const& value);

}  // namespace value
}  // namespace rcgo

#endif  // SRC_UNTYPED_CONSTANT_H_
