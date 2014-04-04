#ifndef RCGO_SRC_VALUE_HPP
#define RCGO_SRC_VALUE_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

#include <stdint.h>
#include <gmpxx.h>

namespace rcgo {

  struct complex64_t {
    float real;
    float imag;

    complex64_t ();
    complex64_t (float a_real, float a_imag);

    complex64_t operator+ (const complex64_t& a_other) const;
    complex64_t operator- (const complex64_t& a_other) const;
    complex64_t operator* (const complex64_t& a_other) const;
    complex64_t operator/ (const complex64_t& a_other) const;
    complex64_t operator+ () const;
    complex64_t operator- () const;

    bool operator== (const complex64_t& a_other) const;
  };

  std::ostream& operator<< (std::ostream& out, const complex64_t& val);

  struct complex128_t {
    double real;
    double imag;

    complex128_t ();
    complex128_t (double a_real, double a_imag);

    complex128_t operator+ (const complex128_t& a_other) const;
    complex128_t operator- (const complex128_t& a_other) const;
    complex128_t operator* (const complex128_t& a_other) const;
    complex128_t operator/ (const complex128_t& a_other) const;
    complex128_t operator+ () const;
    complex128_t operator- () const;

    bool operator== (const complex128_t& a_other) const;
  };

  std::ostream& operator<< (std::ostream& out, const complex128_t& val);

  struct complex_t { // NOT_COVERED
    mpf_class real;
    mpf_class imag;

    complex_t ();
    complex_t (const mpf_class& a_real, const mpf_class& a_imag);
    bool operator== (const complex_t& a_other) const;
    complex_t operator+ (const complex_t& a_other) const;
    complex_t operator- (const complex_t& a_other) const;
    complex_t operator* (const complex_t& a_other) const;
    complex_t operator/ (const complex_t& a_other) const;
    complex_t operator+ () const;
    complex_t operator- () const;
  };

  std::ostream& operator<< (std::ostream& out, const complex_t& val);

  struct Value { // NOT_COVERED
    enum Kind {
      Unknown,
      Error,
      Boolean,
      String,
      Integer,
      Rune,
      Float,
      Complex,
      Typed,
    };

    Value ();
    ~Value ();
    static Value make_error ();
    static Value make_boolean (bool a_value);
    static Value make_integer (const mpz_class& a_value);
    static Value make_rune (const mpz_class& a_value);
    static Value make_float (const mpf_class& a_value);
    static Value make_complex (const mpf_class& a_real, const mpf_class& a_imag);
    static Value make_string (const std::string& a_value);

    bool is_unknown () const;
    bool is_error () const;
    bool is_arithmetic () const;
    bool is_signed () const;
    bool is_integer () const;
    bool is_zero () const;
    bool is_string () const;
    bool is_boolean () const;
    Value convert (const Type* type) const;
    Value promote (const Value& other) const;
    Value to_integer () const;

    Value posate () const;
    Value negate () const;
    Value logic_not () const;
    Value bit_not () const;

    Value add (const Value& y) const;
    Value subtract (const Value& y) const;
    Value multiply (const Value& y) const;
    Value divide (const Value& y) const;
    Value modulo (const Value& y) const;
    Value left_shift (const Value& y) const;
    Value right_shift (const Value& y) const;
    Value bit_and (const Value& y) const;
    Value bit_and_not (const Value& y) const;
    Value bit_or (const Value& y) const;
    Value bit_xor (const Value& y) const;
    Value equal (const Value& y) const;

    bool operator== (const Value& other) const;

    Kind kind;
    const Type* type;

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

  std::ostream& operator<< (std::ostream& out, const Value& value);
}

#endif // RCGO_SRC_VALUE_HPP
