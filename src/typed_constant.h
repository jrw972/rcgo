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

    /// Implementation of complex64 constants
struct complex64_t {
  complex64_t();
    /**
       Construct a complex64 with the given parts.
       @param a_real the real part
       @param a_imag the imaginary part
    */
  complex64_t(float a_real, float a_imag);

    /**
       @returns the real part of the complex constant
    */
  float real() const { return m_real; }
    /**
       @returns the imaginary part of the complex constant
    */
  float imag() const { return m_imag; }

    /**
       Add complex numbers.
       @param a_other The addend.
       @returns The sum
    */
  complex64_t operator+(const complex64_t& a_other) const;
    /**
       Subtract complex numbers.
       @param a_other The subtrahend.
       @returns The difference
    */
  complex64_t operator-(const complex64_t& a_other) const;
    /**
       Multiply complex numbers.
       @param a_other The multiplier.
       @returns The product
    */
  complex64_t operator*(const complex64_t& a_other) const;
    /**
       Divide complex numbers.
       @param a_other The divisor.
       @warning The divisor must not be zero.
       @returns The quotient
    */
  complex64_t operator/(const complex64_t& a_other) const;
    /**
       Posate a complex number.
       @returns The complex number
    */
  complex64_t operator+() const;
    /**
       Negate a complex number.
       @returns The additive inverse of the complex number
    */
  complex64_t operator-() const;

    /**
       Compare complex numbers for equality.
       @param a_other The complex number to compare.
       @returns True if the complex numbers are equal, false otherwise.
    */
  bool operator==(const complex64_t& a_other) const;
    /**
       Compare complex numbers for inequality.
       @param a_other The complex number to compare.
       @returns True if the complex numbers are not equal, false otherwise.
    */
  bool operator!=(const complex64_t& a_other) const;

 private:
  float m_real;
  float m_imag;
};

    /**
       Insert a complex constant into a stream.
       @param out the stream
       @param val the complex constant
       @returns the stream
    */
std::ostream& operator<<(std::ostream& out, const complex64_t& val);

    /// Implementation of complex128 constants
struct complex128_t {
  complex128_t();
    /**
       Construct a complex128 with the given parts.
       @param a_real the real part
       @param a_imag the imaginary part
    */
  complex128_t(double a_real, double a_imag);

    /**
       @returns the real part of the complex constant
    */
  double real() const { return m_real; }
    /**
       @returns the imaginary part of the complex constant
    */
  double imag() const { return m_imag; }

    /**
       Add complex numbers.
       @param a_other The addend.
       @returns The sum
    */
  complex128_t operator+(const complex128_t& a_other) const;
    /**
       Subtract complex numbers.
       @param a_other The subtrahend.
       @returns The difference
    */
  complex128_t operator-(const complex128_t& a_other) const;
    /**
       Multiply complex numbers.
       @param a_other The multiplier.
       @returns The product
    */
  complex128_t operator*(const complex128_t& a_other) const;
    /**
       Divide complex numbers.
       @param a_other The divisor.
       @warning The divisor must not be zero.
       @returns The quotient
    */
  complex128_t operator/(const complex128_t& a_other) const;
    /**
       Posate a complex number.
       @returns The complex number
    */
  complex128_t operator+() const;
    /**
       Negate a complex number.
       @returns The additive inverse of the complex number
    */
  complex128_t operator-() const;

    /**
       Compare complex numbers for equality.
       @param a_other The complex number to compare.
       @returns True if the complex numbers are equal, false otherwise.
    */
  bool operator==(const complex128_t& a_other) const;
    /**
       Compare complex numbers for inequality.
       @param a_other The complex number to compare.
       @returns True if the complex numbers are not equal, false otherwise.
    */
  bool operator!=(const complex128_t& a_other) const;

 private:
  double m_real;
  double m_imag;
};

    /**
       Insert a complex constant into a stream.
       @param out the stream
       @param val the complex constant
       @returns the stream
    */
std::ostream& operator<<(std::ostream& out, const complex128_t& val);

    /// Implementation of typed constants
struct TypedConstant {
    /// Construct an uninitialized typed constant
  TypedConstant();

    /**
       Construct a typed constant with the given type that is equivalent to the given untyped constant.
       If the conversion fails, the constant will be uninitialized.

       @param a_type the type of the resulting constant
       @param a_value the value of the resulting constant
       @pre a_value is initialized
     */
  TypedConstant (type::Type const * a_type,
                 UntypedConstant const & a_value);

    /**
       @returns an untyped constant equivalent to this constant
     */
  UntypedConstant ToUntypedConstant() const;

    /**
       @returns the type of this constant or nullptr if not initialized
    */
  type::Type const * type() const { return m_type; }

    /**
       @returns the bool value of this constant
    */
  bool bool_value() const { return m_bool_value; }
    /**
       @returns the string value of this constant
    */
  std::string const & string_value() const { return m_string_value; }
    /**
       @returns the complex64 value of this constant
    */
  complex64_t complex64_value() const { return m_complex64_value; }
    /**
       @returns the complex128 value of this constant
    */
  complex128_t complex128_value() const { return m_complex128_value; }
    /**
       @returns the float32 value of this constant
    */
  float float32_value() const { return m_float32_value; }
    /**
       @returns the float64 value of this constant
    */
  double float64_value() const { return m_float64_value; }
    /**
       @returns the int value of this constant
    */
  int int_value() const { return m_int_value; }
    /**
       @returns the int8 value of this constant
    */
  int8_t int8_value() const { return m_int8_value; }
    /**
       @returns the int16 value of this constant
    */
  int16_t int16_value() const { return m_int16_value; }
    /**
       @returns the int32 value of this constant
    */
  int32_t int32_value() const { return m_int32_value; }
    /**
       @returns the int64 value of this constant
    */
  int64_t int64_value() const { return m_int64_value; }
    /**
       @returns the uint value of this constant
    */
  unsigned int uint_value() const { return m_uint_value; }
    /**
       @returns the uint8 value of this constant
    */
  uint8_t uint8_value() const { return m_uint8_value; }
    /**
       @returns the uint16 value of this constant
    */
  uint16_t uint16_value() const { return m_uint16_value; }
    /**
       @returns the uint32 value of this constant
    */
  uint32_t uint32_value() const { return m_uint32_value; }
    /**
       @returns the uint64 value of this constant
    */
  uint64_t uint64_value() const { return m_uint64_value; }
    /**
       @returns the uintptr value of this constant
    */
  unsigned int uintptr_value() const { return m_uintptr_value; }

    /**
       @returns true when the typed constant is not uninitialized
    */
  bool IsInitialized() const;
    /**
       @returns true when the typed constant is zero.
    */
  bool IsZero() const;

    /**
       Posate an typed constant.
       @param x
       @returns the typed constant
       @pre x must be arithmetic
    */
  static TypedConstant Posate(TypedConstant const & x);
    /**
       Negate an typed constant.
       @param x
       @returns the additive inverse of the typed constant
       @pre x must be arithmetic
    */
  static TypedConstant Negate(TypedConstant const & x);
    /**
       Logical invert an typed constant.
       @param x
       @returns the logical inverse of the typed constant
       @pre x must be boolean
    */
  static TypedConstant LogicNot(TypedConstant const & x);
    /**
       Compute the bit-wise complement of an typed constant.
       @param x
       @returns the bit-wise complement of the typed constant
       @pre x must be an integer or rune
    */
  static TypedConstant BitNot(TypedConstant const & x);
    /**
       Add or concatenate typed constants.
       @param x the addend
       @param y the addend
       @returns the sum (arithemetic) or concatenation (string) of the typed constants
       @pre x and y must be arithemtic or a string
    */
  static TypedConstant Add(TypedConstant const & x, TypedConstant const & y);
    /**
       Subtract typed constants.
       @param x the minuend
       @param y the subtrahend
       @returns the difference
       @pre x and y must be arithemtic
    */
  static TypedConstant Subtract(TypedConstant const & x, TypedConstant const & y);
    /**
       Multiply typed constants.
       @param x the multiplican
       @param y the multiplier
       @returns the product
       @pre x and y must be arithemtic
    */
  static TypedConstant Multiply(TypedConstant const & x, TypedConstant const & y);
    /**
       Divide typed constants.
       @param x the dividend
       @param y the divisor
       @returns the quotient
       @pre x and y must be arithemtic
       @pre y must not be zero
    */
  static TypedConstant Divide(TypedConstant const & x, TypedConstant const & y);
    /**
       Compute the modulus of typed constants.
       @param x the dividend
       @param y the divisor
       @returns the modulus
       @pre x and y must be arithemtic
       @pre y must not be zero
    */
  static TypedConstant Modulo(TypedConstant const & x, TypedConstant const & y);
    /**
       Shift the bits of the typed constant to the left.
       @param x the bits to shift
       @param y the amount to shift
       @returns the shifted constant
       @pre x must be convertible to an integer
    */
  static TypedConstant LeftShift(TypedConstant const & x, unsigned int y);
    /**
       Shift the bits of the typed constant to the right.
       @param x the bits to shift
       @param y the amount to shift
       @returns the shifted constant
       @pre x must be convertible to an integer
    */
  static TypedConstant RightShift(TypedConstant const & x, unsigned int y);
    /**
       Compute the bit-wise logical and of typed constants.
       @param x
       @param y
       @returns the bit-wise and
       @pre x and y must be integers
    */
  static TypedConstant BitAnd(TypedConstant const & x, TypedConstant const & y);
    /**
       Compute the bit-wise logical and-not of typed constants.
       @param x
       @param y
       @returns the bit-wise and of x and not y
       @pre x and y must be integers
    */
  static TypedConstant BitAndNot(TypedConstant const & x, TypedConstant const & y);
    /**
       Compute the bit-wise logical or of typed constants.
       @param x
       @param y
       @returns the bit-wise or
       @pre x and y must be integers
    */
  static TypedConstant BitOr(TypedConstant const & x, TypedConstant const & y);
    /**
       Compute the bit-wise logical exclusive or of typed constants.
       @param x
       @param y
       @returns the bit-wise exclusive or
       @pre x and y must be integers
    */
  static TypedConstant BitXor(TypedConstant const & x, TypedConstant const & y);
    /**
       Compare typed constants for equality.
       @param x
       @param y
       @returns true if x and y are equal, false otherwise
       @pre x and y must be comparable (AreComparable)
    */
  static UntypedConstant Equal(TypedConstant const & x, TypedConstant const & y);

    /**
       Compare typed constants for equality
       @param y
       @returns true if the constants are equal, false otherwise
    */
  bool operator==(TypedConstant const & y) const;
    /**
       Compare typed constants for inequality
       @param y
       @returns true if the constants are not equal, false otherwise
    */
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

    /**
       Insert an typed constant into a stream.
       @param out the stream
       @param value the untyped constant
       @returns the stream
    */
std::ostream& operator<<(std::ostream & out, TypedConstant const & value);

}  // namespace value
}  // namespace rcgo

#endif  // SRC_TYPED_CONSTANT_H_
