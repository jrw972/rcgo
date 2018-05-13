// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/value.h"

#include <utility>

#include "src/catch.hpp"
#include "src/type.h"
#include "src/utf8.h"

namespace rcgo {
using std::rel_ops::operator!=;
namespace test {

TEST_CASE("complex64_t::complex64_t()") {
  complex64_t c;
  REQUIRE(c.real == 0);
  REQUIRE(c.imag == 0);
}

TEST_CASE("complex64_t::complex64_t(1,2)") {
  complex64_t c(1, 2);
  REQUIRE(c.real == 1);
  REQUIRE(c.imag == 2);
}

TEST_CASE("complex64_t::operator+") {
  complex64_t c1(1, 2);
  complex64_t c2(3, 4);
  complex64_t c3 = c1 + c2;
  REQUIRE(c3 == complex64_t(4, 6));
}

TEST_CASE("complex64_t::operator-") {
  complex64_t c1(1, 2);
  complex64_t c2(3, 4);
  complex64_t c3 = c1 - c2;
  REQUIRE(c3 == complex64_t(-2, -2));
}

TEST_CASE("complex64_t::operator*") {
  complex64_t c1(1, 2);
  complex64_t c2(3, 4);
  complex64_t c3 = c1 * c2;
  REQUIRE(c3 == complex64_t(-5, 10));
}

TEST_CASE("complex64_t::operator/") {
  complex64_t c1(1, 0);
  complex64_t c2(0, 1);
  complex64_t c3 = c1 / c2;
  REQUIRE(c3 == complex64_t(0, -1));
}

TEST_CASE("complex64_t::operator+ unary") {
  complex64_t c1(1, 1);
  complex64_t c2 = +c1;
  REQUIRE(c2 == complex64_t(1, 1));
}

TEST_CASE("complex64_t::operator- unary") {
  complex64_t c1(1, 1);
  complex64_t c2 = -c1;
  REQUIRE(c2 == complex64_t(-1, -1));
}

TEST_CASE("complex64_t::operator==") {
  complex64_t c1(1, 2);
  complex64_t c2(3, 4);
  REQUIRE(c1 == c1);
  REQUIRE(c1 != c2);
}

TEST_CASE("operator<< complex64_t") {
  std::stringstream s1;
  complex64_t c1(1, 2);
  s1 << c1;
  REQUIRE(s1.str() == "1+2i");
  std::stringstream s2;
  complex64_t c2(1, -2);
  s2 << c2;
  REQUIRE(s2.str() == "1-2i");
}

TEST_CASE("complex128_t::complex128_t()") {
  complex128_t c;
  REQUIRE(c.real == 0);
  REQUIRE(c.imag == 0);
}

TEST_CASE("complex128_t::complex128_t(1,2)") {
  complex128_t c(1, 2);
  REQUIRE(c.real == 1);
  REQUIRE(c.imag == 2);
}

TEST_CASE("complex128_t::operator+") {
  complex128_t c1(1, 2);
  complex128_t c2(3, 4);
  complex128_t c3 = c1 + c2;
  REQUIRE(c3 == complex128_t(4, 6));
}

TEST_CASE("complex128_t::operator-") {
  complex128_t c1(1, 2);
  complex128_t c2(3, 4);
  complex128_t c3 = c1 - c2;
  REQUIRE(c3 == complex128_t(-2, -2));
}

TEST_CASE("complex128_t::operator*") {
  complex128_t c1(1, 2);
  complex128_t c2(3, 4);
  complex128_t c3 = c1 * c2;
  REQUIRE(c3 == complex128_t(-5, 10));
}

TEST_CASE("complex128_t::operator/") {
  complex128_t c1(1, 0);
  complex128_t c2(0, 1);
  complex128_t c3 = c1 / c2;
  REQUIRE(c3 == complex128_t(0, -1));
}

TEST_CASE("complex128_t::operator+ unary") {
  complex128_t c1(1, 2);
  complex128_t c2 = +c1;
  REQUIRE(c2 == complex128_t(1, 2));
}

TEST_CASE("complex128_t::operator- unary") {
  complex128_t c1(1, 2);
  complex128_t c2 = -c1;
  REQUIRE(c2 == complex128_t(-1, -2));
}

TEST_CASE("complex128_t::operator==") {
  complex128_t c1(1, 2);
  complex128_t c2(3, 4);
  REQUIRE(c1 == c1);
  REQUIRE(c1 != c2);
}

TEST_CASE("operator<< complex128_t") {
  std::stringstream s1;
  complex128_t c1(1, 2);
  s1 << c1;
  REQUIRE(s1.str() == "1+2i");
  std::stringstream s2;
  complex128_t c2(1, -2);
  s2 << c2;
  REQUIRE(s2.str() == "1-2i");
}

TEST_CASE("complex_t::complex_t()") {
  complex_t c;
  REQUIRE(c.real == 0);
  REQUIRE(c.imag == 0);
}

TEST_CASE("complex_t::complex_t(1,2)") {
  complex_t c(1, 2);
  REQUIRE(c.real == 1);
  REQUIRE(c.imag == 2);
}

TEST_CASE("complex_t::operator+") {
  complex_t c1(1, 2);
  complex_t c2(3, 4);
  complex_t c3 = c1 + c2;
  REQUIRE(c3 == complex_t(4, 6));
}

TEST_CASE("complex_t::operator-") {
  complex_t c1(1, 2);
  complex_t c2(3, 4);
  complex_t c3 = c1 - c2;
  REQUIRE(c3 == complex_t(-2, -2));
}

TEST_CASE("complex_t::operator*") {
  complex_t c1(1, 2);
  complex_t c2(3, 4);
  complex_t c3 = c1 * c2;
  REQUIRE(c3 == complex_t(-5, 10));
}

TEST_CASE("complex_t::operator/") {
  complex_t c1(1, 0);
  complex_t c2(0, 1);
  complex_t c3 = c1 / c2;
  REQUIRE(c3 == complex_t(0, -1));
}

TEST_CASE("complex_t::operator+ unary") {
  complex_t c1(1, 2);
  complex_t c2 = +c1;
  REQUIRE(c2 == complex_t(1, 2));
}

TEST_CASE("complex_t::operator- unary") {
  complex_t c1(1, 2);
  complex_t c2 = -c1;
  REQUIRE(c2 == complex_t(-1, -2));
}

TEST_CASE("complex_t::operator==") {
  complex_t c1(1, 2);
  complex_t c2(3, 4);
  REQUIRE(c1 == c1);
  REQUIRE(c1 != c2);
}

TEST_CASE("operator<< complex_t") {
  std::stringstream s1;
  complex_t c1(1, 2);
  s1 << c1;
  REQUIRE(s1.str() == "1+2i");
  std::stringstream s2;
  complex_t c2(1, -2);
  s2 << c2;
  REQUIRE(s2.str() == "1-2i");
}

TEST_CASE("Value::Value()") {
  Value v;
  REQUIRE(v.kind == Value::kUnknown);
  REQUIRE(v.IsUnknown());
}

TEST_CASE("Value::MakeError()") {
  Value v = Value::MakeError();
  REQUIRE(v.kind == Value::kError);
  REQUIRE(v.IsError());
}

TEST_CASE("Value::MakeBoolean()") {
  Value v = Value::MakeBoolean(true);
  REQUIRE(v.kind == Value::kBoolean);
  REQUIRE(v.Boolean_value == true);
}

TEST_CASE("Value::MakeInteger()") {
  Value v = Value::MakeInteger(3);
  REQUIRE(v.kind == Value::kInteger);
  REQUIRE(v.IsArithmetic());
  REQUIRE(v.Integer_value == 3);
}

TEST_CASE("Value::MakeRune()") {
  Value v = Value::MakeRune(123);
  REQUIRE(v.kind == Value::kRune);
  REQUIRE(v.IsArithmetic());
  REQUIRE(v.Rune_value == 123);
}

TEST_CASE("Value::MakeFloat()") {
  Value v = Value::MakeFloat(3.14);
  REQUIRE(v.kind == Value::kFloat);
  REQUIRE(v.IsArithmetic());
  REQUIRE(v.Float_value == 3.14);
}

TEST_CASE("Value::MakeComplex()") {
  Value v = Value::MakeComplex(1, 2);
  REQUIRE(v.kind == Value::kComplex);
  REQUIRE(v.IsArithmetic());
  REQUIRE(v.Complex_value == complex_t(1, 2));
}

TEST_CASE("Value::MakeString()") {
  Value v = Value::MakeString("hello");
  REQUIRE(v.kind == Value::kString);
  REQUIRE(v.IsString());
  REQUIRE(v.String_value == "hello");
}

TEST_CASE("Value::ConvertTo(Error -> bool)") {
  Value x = Value::MakeError();
  Value y = x.ConvertTo(&type::Bool::instance);
  REQUIRE(y.IsError());
}

TEST_CASE("Value::ConvertTo(Boolean -> bool)") {
  Value x = Value::MakeBoolean(true);
  Value y = x.ConvertTo(&type::Bool::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Bool::instance);
  REQUIRE(y.bool_value == true);
}

TEST_CASE("Value::ConvertTo(Integer -> complex64)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Complex64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.IsArithmetic());
  REQUIRE(y.type == &type::Complex64::instance);
  REQUIRE(y.complex64_value == complex64_t(34, 0));
}

TEST_CASE("Value::ConvertTo(Integer -> complex128)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Complex128::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Complex128::instance);
  REQUIRE(y.complex128_value == complex128_t(34, 0));
}

TEST_CASE("Value::ConvertTo(Integer -> float32)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Float32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Float32::instance);
  REQUIRE(y.float32_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> float64)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Float64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Float64::instance);
  REQUIRE(y.float64_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> int)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Int::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int::instance);
  REQUIRE(y.int_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> int8)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Int8::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int8::instance);
  REQUIRE(y.int8_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> int16)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Int16::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int16::instance);
  REQUIRE(y.int16_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> int32)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Int32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int32::instance);
  REQUIRE(y.int32_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> int64)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Int64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int64::instance);
  REQUIRE(y.int64_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> uint)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Uint::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint::instance);
  REQUIRE(y.uint_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> uint8)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Uint8::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint8::instance);
  REQUIRE(y.uint8_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> uint16)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Uint16::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint16::instance);
  REQUIRE(y.uint16_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> uint32)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Uint32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint32::instance);
  REQUIRE(y.uint32_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> uint64)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Uint64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint64::instance);
  REQUIRE(y.uint64_value == 34);
}

TEST_CASE("Value::ConvertTo(Integer -> uintptr)") {
  Value x = Value::MakeInteger(34);
  Value y = x.ConvertTo(&type::Uintptr::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uintptr::instance);
  REQUIRE(y.uintptr_value == 34);
}

TEST_CASE("Value::ConvertTo(Float 0 -> complex64)") {
  Value x = Value::MakeFloat(0);
  Value y = x.ConvertTo(&type::Complex64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Complex64::instance);
  REQUIRE(y.complex64_value == complex64_t(0, 0));
}

TEST_CASE("Value::ConvertTo(Float -> complex64)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Complex64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Complex64::instance);
  REQUIRE(y.complex64_value == complex64_t(34, 0));
}

TEST_CASE("Value::ConvertTo(Float -> complex128)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Complex128::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Complex128::instance);
  REQUIRE(y.complex128_value == complex128_t(34, 0));
}

TEST_CASE("Value::ConvertTo(Float -> float32)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Float32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Float32::instance);
  REQUIRE(y.float32_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> float64)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Float64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Float64::instance);
  REQUIRE(y.float64_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> int)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Int::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int::instance);
  REQUIRE(y.int_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> int8)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Int8::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int8::instance);
  REQUIRE(y.int8_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> int16)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Int16::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int16::instance);
  REQUIRE(y.int16_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> int32)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Int32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int32::instance);
  REQUIRE(y.int32_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> int64)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Int64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int64::instance);
  REQUIRE(y.int64_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> uint)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Uint::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint::instance);
  REQUIRE(y.uint_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> uint8)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Uint8::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint8::instance);
  REQUIRE(y.uint8_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> uint16)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Uint16::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint16::instance);
  REQUIRE(y.uint16_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> uint32)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Uint32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint32::instance);
  REQUIRE(y.uint32_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> uint64)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Uint64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint64::instance);
  REQUIRE(y.uint64_value == 34);
}

TEST_CASE("Value::ConvertTo(Float -> uintptr)") {
  Value x = Value::MakeFloat(34);
  Value y = x.ConvertTo(&type::Uintptr::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uintptr::instance);
  REQUIRE(y.uintptr_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> complex64)") {
  Value x = Value::MakeComplex(1, 2);
  Value y = x.ConvertTo(&type::Complex64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Complex64::instance);
  REQUIRE(y.complex64_value == complex64_t(1, 2));
}

TEST_CASE("Value::ConvertTo(Complex -> complex128)") {
  Value x = Value::MakeComplex(1, 2);
  Value y = x.ConvertTo(&type::Complex128::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Complex128::instance);
  REQUIRE(y.complex128_value == complex128_t(1, 2));
}

TEST_CASE("Value::ConvertTo(Complex -> float32)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Float32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Float32::instance);
  REQUIRE(y.float32_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> float64)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Float64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Float64::instance);
  REQUIRE(y.float64_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> int)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Int::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int::instance);
  REQUIRE(y.int_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> int8)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Int8::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int8::instance);
  REQUIRE(y.int8_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> int16)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Int16::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int16::instance);
  REQUIRE(y.int16_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> int32)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Int32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int32::instance);
  REQUIRE(y.int32_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> int64)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Int64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int64::instance);
  REQUIRE(y.int64_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> uint)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Uint::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint::instance);
  REQUIRE(y.uint_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> uint8)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Uint8::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint8::instance);
  REQUIRE(y.uint8_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> uint16)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Uint16::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint16::instance);
  REQUIRE(y.uint16_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> uint32)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Uint32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint32::instance);
  REQUIRE(y.uint32_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> uint64)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Uint64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint64::instance);
  REQUIRE(y.uint64_value == 34);
}

TEST_CASE("Value::ConvertTo(Complex -> uintptr)") {
  Value x = Value::MakeComplex(34, 0);
  Value y = x.ConvertTo(&type::Uintptr::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uintptr::instance);
  REQUIRE(y.uintptr_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> complex64)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Complex64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Complex64::instance);
  REQUIRE(y.complex64_value == complex64_t(34, 0));
}

TEST_CASE("Value::ConvertTo(Rune -> complex128)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Complex128::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Complex128::instance);
  REQUIRE(y.complex128_value == complex128_t(34, 0));
}

TEST_CASE("Value::ConvertTo(Rune -> float32)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Float32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Float32::instance);
  REQUIRE(y.float32_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> float64)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Float64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Float64::instance);
  REQUIRE(y.float64_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> int)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Int::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int::instance);
  REQUIRE(y.int_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> int8)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Int8::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int8::instance);
  REQUIRE(y.int8_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> int16)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Int16::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int16::instance);
  REQUIRE(y.int16_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> int32)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Int32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int32::instance);
  REQUIRE(y.int32_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> int64)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Int64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Int64::instance);
  REQUIRE(y.int64_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> uint)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Uint::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint::instance);
  REQUIRE(y.uint_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> uint8)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Uint8::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint8::instance);
  REQUIRE(y.uint8_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> uint16)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Uint16::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint16::instance);
  REQUIRE(y.uint16_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> uint32)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Uint32::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint32::instance);
  REQUIRE(y.uint32_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> uint64)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Uint64::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uint64::instance);
  REQUIRE(y.uint64_value == 34);
}

TEST_CASE("Value::ConvertTo(Rune -> uintptr)") {
  Value x = Value::MakeRune(34);
  Value y = x.ConvertTo(&type::Uintptr::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::Uintptr::instance);
  REQUIRE(y.uintptr_value == 34);
}

TEST_CASE("Value::ConvertTo(String -> string)") {
  Value x = Value::MakeString("hello");
  Value y = x.ConvertTo(&type::String::instance);
  REQUIRE(y.kind == Value::kTyped);
  REQUIRE(y.type == &type::String::instance);
  REQUIRE(y.string_value == "hello");
}

TEST_CASE("Value::PromoteTo(Rune -> Rune)") {
  Value x = Value::MakeRune(34);
  Value y = Value::MakeRune(56);
  Value z = x.PromoteTo(y);
  REQUIRE(z == Value::MakeRune(34));
}

TEST_CASE("Value::PromoteTo(Rune -> int)") {
  Value x = Value::MakeRune(34);
  Value y = Value::MakeInteger(1).ConvertTo(&type::Int::instance);
  Value z = x.PromoteTo(y);
  REQUIRE(z == Value::MakeInteger(34).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::PromoteTo(Integer -> Rune)") {
  Value x = Value::MakeInteger(34);
  Value y = Value::MakeRune(56);
  Value z = x.PromoteTo(y);
  REQUIRE(z == Value::MakeRune(34));
}

TEST_CASE("Value::PromoteTo(Rune -> Float)") {
  Value x = Value::MakeRune(34);
  Value y = Value::MakeFloat(56);
  Value z = x.PromoteTo(y);
  REQUIRE(z == Value::MakeFloat(34));
}

TEST_CASE("Value::PromoteTo(Float -> Complex)") {
  Value x = Value::MakeFloat(34);
  Value y = Value::MakeComplex(56, 78);
  Value z = x.PromoteTo(y);
  REQUIRE(z == Value::MakeComplex(34, 0));
}

TEST_CASE("Value::PromoteTo(Boolean -> Integer)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeInteger(56);
  Value z = x.PromoteTo(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::PromoteTo(String -> Integer)") {
  Value x = Value::MakeString("hello");
  Value y = Value::MakeInteger(56);
  Value z = x.PromoteTo(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::ToInteger(int8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(int16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(int32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int32::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(int64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(uint16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint16::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(uint32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint32::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(uint64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint64::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(int)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(uint)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(uintptr)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(Rune)") {
  Value x = Value::MakeRune(3);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(Float yes)") {
  Value x = Value::MakeFloat(3);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(Float no)") {
  Value x = Value::MakeFloat(3.5);
  Value y = x.ToInteger();
  REQUIRE(y.IsError());
}

TEST_CASE("Value::ToInteger(Complex yes)") {
  Value x = Value::MakeComplex(3, 0);
  Value y = x.ToInteger();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::ToInteger(Complex no)") {
  Value x = Value::MakeComplex(3, 1);
  Value y = x.ToInteger();
  REQUIRE(y.IsError());
}

TEST_CASE("Value::ToInteger(bool)") {
  Value x = Value::MakeBoolean(true).ConvertTo(&type::Bool::instance);
  Value y = x.ToInteger();
  REQUIRE(y.IsError());
}

TEST_CASE("Value::Posate(Error)") {
  Value x = Value::MakeError();
  Value y = x.Posate();
  REQUIRE(y.IsError());
}

TEST_CASE("Value::Posate(Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = x.Posate();
  REQUIRE(y.IsError());
}

TEST_CASE("Value::Posate(Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3));
}

TEST_CASE("Value::Posate(Rune)") {
  Value x = Value::MakeRune(3);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeRune(3));
}

TEST_CASE("Value::Posate(Float)") {
  Value x = Value::MakeFloat(1.5);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeFloat(1.5));
}

TEST_CASE("Value::Posate(Complex)") {
  Value x = Value::MakeComplex(1, 2);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeComplex(1, 2));
}

TEST_CASE("Value::Posate(int8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::Posate(int16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::Posate(int32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int32::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::Posate(int64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::Posate(uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::Posate(uint16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint16::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::Posate(uint32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint32::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::Posate(uint64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint64::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::Posate(int)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::Posate(uint)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::Posate(uintptr)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::Posate(float32)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float32::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeFloat(3).ConvertTo(&type::Float32::instance));
}

TEST_CASE("Value::Posate(float64)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float64::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeFloat(3).ConvertTo(&type::Float64::instance));
}

TEST_CASE("Value::Posate(complex64)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex64::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeComplex(1, 2).ConvertTo(&type::Complex64::instance));
}

TEST_CASE("Value::Posate(complex128)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex128::instance);
  Value y = x.Posate();
  REQUIRE(y == Value::MakeComplex(1, 2).ConvertTo(&type::Complex128::instance));
}

TEST_CASE("Value::Negate(Error)") {
  Value x = Value::MakeError();
  Value y = x.Negate();
  REQUIRE(y.IsError());
}

TEST_CASE("Value::Negate(Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = x.Negate();
  REQUIRE(y.IsError());
}

TEST_CASE("Value::Negate(Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeInteger(-3));
}

TEST_CASE("Value::Negate(Rune)") {
  Value x = Value::MakeRune(3);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeRune(-3));
}

TEST_CASE("Value::Negate(Float)") {
  Value x = Value::MakeFloat(1.5);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeFloat(-1.5));
}

TEST_CASE("Value::Negate(Complex)") {
  Value x = Value::MakeComplex(1, 2);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeComplex(-1, -2));
}

TEST_CASE("Value::Negate(int8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeInteger(-3).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::Negate(int16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeInteger(-3).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::Negate(int32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int32::instance);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeInteger(-3).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::Negate(int64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeInteger(-3).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::Negate(uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeError());
}

TEST_CASE("Value::Negate(int)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeInteger(-3).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::Negate(uintptr)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeInteger(-3).ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::Negate(float32)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float32::instance);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeFloat(-3).ConvertTo(&type::Float32::instance));
}

TEST_CASE("Value::Negate(float64)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float64::instance);
  Value y = x.Negate();
  REQUIRE(y == Value::MakeFloat(-3).ConvertTo(&type::Float64::instance));
}

TEST_CASE("Value::Negate(complex64)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex64::instance);
  Value y = x.Negate();
  REQUIRE(y ==
          Value::MakeComplex(-1, -2).ConvertTo(&type::Complex64::instance));
}

TEST_CASE("Value::Negate(complex128)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex128::instance);
  Value y = x.Negate();
  REQUIRE(y ==
          Value::MakeComplex(-1, -2).ConvertTo(&type::Complex128::instance));
}

TEST_CASE("Value::LogicNot(Error)") {
  Value x = Value::MakeError();
  Value y = x.LogicNot();
  REQUIRE(y.IsError());
}

TEST_CASE("Value::LogicNot(Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = x.LogicNot();
  REQUIRE(y == Value::MakeBoolean(false));
}

TEST_CASE("Value::LogicNot(Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = x.LogicNot();
  REQUIRE(y == Value::MakeError());
}

TEST_CASE("Value::LogicNot(bool)") {
  Value x = Value::MakeBoolean(true).ConvertTo(&type::Bool::instance);
  Value y = x.LogicNot();
  REQUIRE(y == Value::MakeBoolean(false).ConvertTo(&type::Bool::instance));
}

TEST_CASE("Value::BitNot(Error)") {
  Value x = Value::MakeError();
  Value y = x.BitNot();
  REQUIRE(y.IsError());
}

TEST_CASE("Value::BitNot(Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = x.BitNot();
  REQUIRE(y.IsError());
}

TEST_CASE("Value::BitNot(Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(-4));
}

TEST_CASE("Value::BitNot(Rune)") {
  Value x = Value::MakeRune(3);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeRune(-4));
}

TEST_CASE("Value::BitNot(int8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(-4).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::BitNot(int16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(-4).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::BitNot(int32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int32::instance);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(-4).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::BitNot(int64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(-4).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::BitNot(uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(252).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::BitNot(uint16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint16::instance);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(65532).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::BitNot(uint32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint32::instance);
  Value y = x.BitNot();
  REQUIRE(y ==
          Value::MakeInteger(4294967292).ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::BitNot(uint64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint64::instance);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(18446744073709551612UL)
          .ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::BitNot(int)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(-4).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::BitNot(uint)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint::instance);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(4294967292).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::BitNot(uintptr)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value y = x.BitNot();
  REQUIRE(y == Value::MakeInteger(4294967292)
          .ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::Multiply(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.Multiply(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Multiply(Boolean, Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(true);
  Value z = x.Multiply(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Multiply(Integer, Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12));
}

TEST_CASE("Value::Multiply(Rune, Integer)") {
  Value x = Value::MakeRune(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeRune(12));
}

TEST_CASE("Value::Multiply(Integer, Rune)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeRune(4);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeRune(12));
}

TEST_CASE("Value::Multiply(Float, Float)") {
  Value x = Value::MakeFloat(1.5);
  Value y = Value::MakeFloat(2.25);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeFloat(3.375));
}

TEST_CASE("Value::Multiply(Complex, Complex)") {
  Value x = Value::MakeComplex(1, 2);
  Value y = Value::MakeComplex(3, 4);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeComplex(-5, 10));
}

TEST_CASE("Value::Multiply(int8, int8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int8::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::Multiply(int16, int16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int16::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::Multiply(int32, int32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int32::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::Multiply(int64, int64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int64::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::Multiply(uint8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::Multiply(uint16, uint16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint16::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::Multiply(uint32, uint32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint32::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::Multiply(uint64, uint64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint64::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::Multiply(int, int)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::Multiply(uint, uint)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::Multiply(uintptr, uintptr)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uintptr::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeInteger(12).ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::Multiply(float32, float32)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float32::instance);
  Value y = Value::MakeFloat(4).ConvertTo(&type::Float32::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeFloat(12).ConvertTo(&type::Float32::instance));
}

TEST_CASE("Value::Multiply(float64, float64)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float64::instance);
  Value y = Value::MakeFloat(4).ConvertTo(&type::Float64::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeFloat(12).ConvertTo(&type::Float64::instance));
}

TEST_CASE("Value::Multiply(complex64, complex64)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex64::instance);
  Value y = Value::MakeComplex(3, 4).ConvertTo(&type::Complex64::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeComplex(-5, 10)
          .ConvertTo(&type::Complex64::instance));
}

TEST_CASE("Value::Multiply(complex128, complex128)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex128::instance);
  Value y = Value::MakeComplex(3, 4).ConvertTo(&type::Complex128::instance);
  Value z = x.Multiply(y);
  REQUIRE(z == Value::MakeComplex(-5, 10)
          .ConvertTo(&type::Complex128::instance));
}

TEST_CASE("Value::Multiply(int8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.Multiply(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Divide(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.Divide(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Divide(Boolean, Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(true);
  Value z = x.Divide(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Divide(Integer, Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0));
}

TEST_CASE("Value::Divide(Rune, Integer)") {
  Value x = Value::MakeRune(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeRune(0));
}

TEST_CASE("Value::Divide(Integer, Rune)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeRune(4);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeRune(0));
}

TEST_CASE("Value::Divide(Float, Float)") {
  Value x = Value::MakeFloat(1);
  Value y = Value::MakeFloat(2);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeFloat(.5));
}

TEST_CASE("Value::Divide(Complex, Complex)") {
  Value x = Value::MakeComplex(1, 0);
  Value y = Value::MakeComplex(0, 1);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeComplex(0, -1));
}

TEST_CASE("Value::Divide(int8, int8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int8::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::Divide(int16, int16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int16::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::Divide(int32, int32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int32::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::Divide(int64, int64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int64::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::Divide(uint8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::Divide(uint16, uint16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint16::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::Divide(uint32, uint32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint32::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::Divide(uint64, uint64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint64::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::Divide(int, int)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::Divide(uint, uint)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::Divide(uintptr, uintptr)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uintptr::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeInteger(0).ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::Divide(float32, float32)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float32::instance);
  Value y = Value::MakeFloat(4).ConvertTo(&type::Float32::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeFloat(.75).ConvertTo(&type::Float32::instance));
}

TEST_CASE("Value::Divide(float64, float64)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float64::instance);
  Value y = Value::MakeFloat(4).ConvertTo(&type::Float64::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeFloat(.75).ConvertTo(&type::Float64::instance));
}

TEST_CASE("Value::Divide(complex64, complex64)") {
  Value x = Value::MakeComplex(1, 0).ConvertTo(&type::Complex64::instance);
  Value y = Value::MakeComplex(0, -1).ConvertTo(&type::Complex64::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeComplex(0, 1).ConvertTo(&type::Complex64::instance));
}

TEST_CASE("Value::Divide(complex128, complex128)") {
  Value x = Value::MakeComplex(1, 0).ConvertTo(&type::Complex128::instance);
  Value y = Value::MakeComplex(0, -1).ConvertTo(&type::Complex128::instance);
  Value z = x.Divide(y);
  REQUIRE(z == Value::MakeComplex(0, 1).ConvertTo(&type::Complex128::instance));
}

TEST_CASE("Value::Divide(int8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.Divide(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Divide(Integer, 0)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(0);
  Value z = x.Divide(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Modulo(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.Modulo(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Modulo(Boolean, Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(true);
  Value z = x.Modulo(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Modulo(Integer, Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::Modulo(Rune, Integer)") {
  Value x = Value::MakeRune(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeRune(3));
}

TEST_CASE("Value::Modulo(Integer, Rune)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeRune(4);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeRune(3));
}

TEST_CASE("Value::Modulo(int8, int8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int8::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::Modulo(int16, int16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int16::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::Modulo(int32, int32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int32::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::Modulo(int64, int64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int64::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::Modulo(uint8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::Modulo(uint16, uint16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint16::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::Modulo(uint32, uint32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint32::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::Modulo(uint64, uint64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint64::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::Modulo(int, int)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::Modulo(uint, uint)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::Modulo(uintptr, uintptr)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uintptr::instance);
  Value z = x.Modulo(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::Modulo(int8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.Modulo(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Modulo(Integer, 0)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(0);
  Value z = x.Modulo(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::LeftShift(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.LeftShift(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::LeftShift(Boolean, Integer)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::LeftShift(Integer, Boolean)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeBoolean(true);
  Value z = x.LeftShift(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::LeftShift(Integer, Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Integer, Rune)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeRune(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Integer, Float)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeFloat(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Integer, Complex)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeComplex(4, 0);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Integer, uint8)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Integer, uint16)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint16::instance);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Integer, uint32)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint32::instance);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Integer, uint64)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint64::instance);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Integer, uint)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint::instance);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Integer, uintptr)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uintptr::instance);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(int8, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::LeftShift(int16, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::LeftShift(int32, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::LeftShift(int64, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::LeftShift(uint8, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::LeftShift(uint16, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::LeftShift(uint32, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::LeftShift(uint64, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::LeftShift(int, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::LeftShift(uint, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::LeftShift(uintptr, Integer)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48).ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::LeftShift(Rune, Integer)") {
  Value x = Value::MakeRune(3);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Float, Integer)") {
  Value x = Value::MakeFloat(3);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::LeftShift(Complex, Integer)") {
  Value x = Value::MakeComplex(3, 0);
  Value y = Value::MakeInteger(4);
  Value z = x.LeftShift(y);
  REQUIRE(z == Value::MakeInteger(48));
}

TEST_CASE("Value::RightShift(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.RightShift(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::RightShift(Boolean, Integer)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::RightShift(Integer, Boolean)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeBoolean(true);
  Value z = x.RightShift(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::RightShift(Integer, Integer)") {
  Value x = Value::MakeInteger(48);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Integer, Rune)") {
  Value x = Value::MakeInteger(48);
  Value y = Value::MakeRune(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Integer, Float)") {
  Value x = Value::MakeInteger(48);
  Value y = Value::MakeFloat(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Integer, Complex)") {
  Value x = Value::MakeInteger(48);
  Value y = Value::MakeComplex(4, 0);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Integer, uint8)") {
  Value x = Value::MakeInteger(48);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Integer, uint16)") {
  Value x = Value::MakeInteger(48);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint16::instance);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Integer, uint32)") {
  Value x = Value::MakeInteger(48);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint32::instance);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Integer, uint64)") {
  Value x = Value::MakeInteger(48);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint64::instance);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Integer, uint)") {
  Value x = Value::MakeInteger(48);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint::instance);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Integer, uintptr)") {
  Value x = Value::MakeInteger(48);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uintptr::instance);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(int8, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::RightShift(int16, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::RightShift(int32, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::RightShift(int64, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::RightShift(uint8, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::RightShift(uint16, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::RightShift(uint32, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::RightShift(uint64, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::RightShift(int, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::RightShift(uint, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::RightShift(uintptr, Integer)") {
  Value x = Value::MakeInteger(48).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::RightShift(Rune, Integer)") {
  Value x = Value::MakeRune(48);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Float, Integer)") {
  Value x = Value::MakeFloat(48);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::RightShift(Complex, Integer)") {
  Value x = Value::MakeComplex(48, 0);
  Value y = Value::MakeInteger(4);
  Value z = x.RightShift(y);
  REQUIRE(z == Value::MakeInteger(3));
}

TEST_CASE("Value::BitAnd(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.BitAnd(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitAnd(Boolean, Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(true);
  Value z = x.BitAnd(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitAnd(Integer, Integer)") {
  Value x = Value::MakeInteger(10);
  Value y = Value::MakeInteger(-4);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8));
}

TEST_CASE("Value::BitAnd(Rune, Integer)") {
  Value x = Value::MakeRune(10);
  Value y = Value::MakeInteger(-4);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeRune(8));
}

TEST_CASE("Value::BitAnd(Integer, Rune)") {
  Value x = Value::MakeInteger(10);
  Value y = Value::MakeRune(-4);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeRune(8));
}

TEST_CASE("Value::BitAnd(int8, int8)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int8::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::BitAnd(int16, int16)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int16::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::BitAnd(int32, int32)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int32::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::BitAnd(int64, int64)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int64::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::BitAnd(uint8, uint8)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(0xFC).ConvertTo(&type::Uint8::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::BitAnd(uint16, uint16)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(0xFFFC).ConvertTo(&type::Uint16::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::BitAnd(uint32, uint32)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uint32::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::BitAnd(uint64, uint64)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(0xFFFFFFFFFFFFFFFC)
      .ConvertTo(&type::Uint64::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::BitAnd(int, int)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::BitAnd(uint, uint)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uint::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::BitAnd(uintptr, uintptr)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uintptr::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z == Value::MakeInteger(8).ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::BitAnd(int8, uint8)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(0xFC).ConvertTo(&type::Uint8::instance);
  Value z = x.BitAnd(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitAndNot(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.BitAndNot(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitAndNot(Boolean, Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(true);
  Value z = x.BitAndNot(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitAndNot(Integer, Integer)") {
  Value x = Value::MakeInteger(10);
  Value y = Value::MakeInteger(-4);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2));
}

TEST_CASE("Value::BitAndNot(Rune, Integer)") {
  Value x = Value::MakeRune(10);
  Value y = Value::MakeInteger(-4);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeRune(2));
}

TEST_CASE("Value::BitAndNot(Integer, Rune)") {
  Value x = Value::MakeInteger(10);
  Value y = Value::MakeRune(-4);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeRune(2));
}

TEST_CASE("Value::BitAndNot(int8, int8)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int8::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::BitAndNot(int16, int16)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int16::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::BitAndNot(int32, int32)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int32::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::BitAndNot(int64, int64)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int64::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::BitAndNot(uint8, uint8)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(0xFC).ConvertTo(&type::Uint8::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::BitAndNot(uint16, uint16)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(0xFFFC).ConvertTo(&type::Uint16::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::BitAndNot(uint32, uint32)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uint32::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::BitAndNot(uint64, uint64)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(0xFFFFFFFFFFFFFFFC)
      .ConvertTo(&type::Uint64::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::BitAndNot(int, int)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::BitAndNot(uint, uint)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uint::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::BitAndNot(uintptr, uintptr)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uintptr::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z == Value::MakeInteger(2).ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::BitAndNot(int8, uint8)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(0xFC).ConvertTo(&type::Uint8::instance);
  Value z = x.BitAndNot(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Add(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.Add(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Add(Boolean, Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(true);
  Value z = x.Add(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Add(Integer, Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7));
}

TEST_CASE("Value::Add(Rune, Integer)") {
  Value x = Value::MakeRune(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeRune(7));
}

TEST_CASE("Value::Add(Integer, Rune)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeRune(4);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeRune(7));
}

TEST_CASE("Value::Add(Float, Float)") {
  Value x = Value::MakeFloat(1.5);
  Value y = Value::MakeFloat(2.25);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeFloat(3.75));
}

TEST_CASE("Value::Add(Complex, Complex)") {
  Value x = Value::MakeComplex(1, 2);
  Value y = Value::MakeComplex(3, 4);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeComplex(4, 6));
}

TEST_CASE("Value::Add(String, String)") {
  Value x = Value::MakeString("hello ");
  Value y = Value::MakeString("world");
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeString("hello world"));
}

TEST_CASE("Value::Add(int8, int8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int8::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::Add(int16, int16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int16::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::Add(int32, int32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int32::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::Add(int64, int64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int64::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::Add(uint8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::Add(uint16, uint16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint16::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::Add(uint32, uint32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint32::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::Add(uint64, uint64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint64::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::Add(int, int)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::Add(uint, uint)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::Add(uintptr, uintptr)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uintptr::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeInteger(7).ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::Add(float32, float32)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float32::instance);
  Value y = Value::MakeFloat(4).ConvertTo(&type::Float32::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeFloat(7).ConvertTo(&type::Float32::instance));
}

TEST_CASE("Value::Add(float64, float64)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float64::instance);
  Value y = Value::MakeFloat(4).ConvertTo(&type::Float64::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeFloat(7).ConvertTo(&type::Float64::instance));
}

TEST_CASE("Value::Add(complex64, complex64)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex64::instance);
  Value y = Value::MakeComplex(3, 4).ConvertTo(&type::Complex64::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeComplex(4, 6).ConvertTo(&type::Complex64::instance));
}

TEST_CASE("Value::Add(complex128, complex128)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex128::instance);
  Value y = Value::MakeComplex(3, 4).ConvertTo(&type::Complex128::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeComplex(4, 6).ConvertTo(&type::Complex128::instance));
}

TEST_CASE("Value::Add(string, string)") {
  Value x = Value::MakeString("hello ").ConvertTo(&type::String::instance);
  Value y = Value::MakeString("world").ConvertTo(&type::String::instance);
  Value z = x.Add(y);
  REQUIRE(z == Value::MakeString("hello world")
          .ConvertTo(&type::String::instance));
}

TEST_CASE("Value::Add(int8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.Add(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Subtract(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.Subtract(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Subtract(Boolean, Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(true);
  Value z = x.Subtract(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Subtract(Integer, Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(-1));
}

TEST_CASE("Value::Subtract(Rune, Integer)") {
  Value x = Value::MakeRune(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeRune(-1));
}

TEST_CASE("Value::Subtract(Integer, Rune)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeRune(4);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeRune(-1));
}

TEST_CASE("Value::Subtract(Float, Float)") {
  Value x = Value::MakeFloat(1.5);
  Value y = Value::MakeFloat(2.25);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeFloat(-0.75));
}

TEST_CASE("Value::Subtract(Complex, Complex)") {
  Value x = Value::MakeComplex(1, 2);
  Value y = Value::MakeComplex(3, 4);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeComplex(-2, -2));
}

TEST_CASE("Value::Subtract(int8, int8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int8::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(-1).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::Subtract(int16, int16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int16::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(-1).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::Subtract(int32, int32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int32::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(-1).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::Subtract(int64, int64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int64::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(-1).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::Subtract(uint8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(255).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::Subtract(uint16, uint16)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint16::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(65535).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::Subtract(uint32, uint32)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint32::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(4294967295)
          .ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::Subtract(uint64, uint64)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint64::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(18446744073709551615UL)
          .ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::Subtract(int, int)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Int::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(-1).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::Subtract(uint, uint)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(4294967295).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::Subtract(uintptr, uintptr)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uintptr::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeInteger(4294967295)
          .ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::Subtract(float32, float32)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float32::instance);
  Value y = Value::MakeFloat(4).ConvertTo(&type::Float32::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeFloat(-1).ConvertTo(&type::Float32::instance));
}

TEST_CASE("Value::Subtract(float64, float64)") {
  Value x = Value::MakeFloat(3).ConvertTo(&type::Float64::instance);
  Value y = Value::MakeFloat(4).ConvertTo(&type::Float64::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeFloat(-1).ConvertTo(&type::Float64::instance));
}

TEST_CASE("Value::Subtract(complex64, complex64)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex64::instance);
  Value y = Value::MakeComplex(3, 4).ConvertTo(&type::Complex64::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeComplex(-2, -2)
          .ConvertTo(&type::Complex64::instance));
}

TEST_CASE("Value::Subtract(complex128, complex128)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex128::instance);
  Value y = Value::MakeComplex(3, 4).ConvertTo(&type::Complex128::instance);
  Value z = x.Subtract(y);
  REQUIRE(z == Value::MakeComplex(-2, -2)
          .ConvertTo(&type::Complex128::instance));
}

TEST_CASE("Value::Subtract(int8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(4).ConvertTo(&type::Uint8::instance);
  Value z = x.Subtract(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitOr(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.BitOr(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitOr(Boolean, Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(true);
  Value z = x.BitOr(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitOr(Integer, Integer)") {
  Value x = Value::MakeInteger(10);
  Value y = Value::MakeInteger(-4);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(-2));
}

TEST_CASE("Value::BitOr(Rune, Integer)") {
  Value x = Value::MakeRune(10);
  Value y = Value::MakeInteger(-4);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeRune(-2));
}

TEST_CASE("Value::BitOr(Integer, Rune)") {
  Value x = Value::MakeInteger(10);
  Value y = Value::MakeRune(-4);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeRune(-2));
}

TEST_CASE("Value::BitOr(int8, int8)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int8::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(-2).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::BitOr(int16, int16)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int16::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(-2).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::BitOr(int32, int32)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int32::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(-2).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::BitOr(int64, int64)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int64::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(-2).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::BitOr(uint8, uint8)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(0xFC).ConvertTo(&type::Uint8::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(0xFE).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::BitOr(uint16, uint16)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(0xFFFC).ConvertTo(&type::Uint16::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(0xFFFE).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::BitOr(uint32, uint32)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uint32::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(0xFFFFFFFE)
          .ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::BitOr(uint64, uint64)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(0xFFFFFFFFFFFFFFFC)
      .ConvertTo(&type::Uint64::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(0xFFFFFFFFFFFFFFFE)
          .ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::BitOr(int, int)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(-2).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::BitOr(uint, uint)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uint::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(0xFFFFFFFE).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::BitOr(uintptr, uintptr)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uintptr::instance);
  Value z = x.BitOr(y);
  REQUIRE(z == Value::MakeInteger(0xFFFFFFFE)
          .ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::BitOr(int8, uint8)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(0xFC).ConvertTo(&type::Uint8::instance);
  Value z = x.BitOr(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitXor(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.BitXor(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitXor(Boolean, Boolean)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(true);
  Value z = x.BitXor(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitXor(Integer, Integer)") {
  Value x = Value::MakeInteger(10);
  Value y = Value::MakeInteger(-4);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(-10));
}

TEST_CASE("Value::BitXor(Rune, Integer)") {
  Value x = Value::MakeRune(10);
  Value y = Value::MakeInteger(-4);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeRune(-10));
}

TEST_CASE("Value::BitXor(Integer, Rune)") {
  Value x = Value::MakeInteger(10);
  Value y = Value::MakeRune(-4);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeRune(-10));
}

TEST_CASE("Value::BitXor(int8, int8)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int8::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(-10).ConvertTo(&type::Int8::instance));
}

TEST_CASE("Value::BitXor(int16, int16)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int16::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int16::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(-10).ConvertTo(&type::Int16::instance));
}

TEST_CASE("Value::BitXor(int32, int32)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int32::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(-10).ConvertTo(&type::Int32::instance));
}

TEST_CASE("Value::BitXor(int64, int64)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int64::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int64::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(-10).ConvertTo(&type::Int64::instance));
}

TEST_CASE("Value::BitXor(uint8, uint8)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint8::instance);
  Value y = Value::MakeInteger(0xFC).ConvertTo(&type::Uint8::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(0xF6).ConvertTo(&type::Uint8::instance));
}

TEST_CASE("Value::BitXor(uint16, uint16)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(0xFFFC).ConvertTo(&type::Uint16::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(0xFFF6).ConvertTo(&type::Uint16::instance));
}

TEST_CASE("Value::BitXor(uint32, uint32)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint32::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uint32::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(0xFFFFFFF6)
          .ConvertTo(&type::Uint32::instance));
}

TEST_CASE("Value::BitXor(uint64, uint64)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(0xFFFFFFFFFFFFFFFC)
      .ConvertTo(&type::Uint64::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(0xFFFFFFFFFFFFFFF6)
          .ConvertTo(&type::Uint64::instance));
}

TEST_CASE("Value::BitXor(int, int)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int::instance);
  Value y = Value::MakeInteger(-4).ConvertTo(&type::Int::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(-10).ConvertTo(&type::Int::instance));
}

TEST_CASE("Value::BitXor(uint, uint)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uint::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(0xFFFFFFF6).ConvertTo(&type::Uint::instance));
}

TEST_CASE("Value::BitXor(uintptr, uintptr)") {
  Value x = Value::MakeInteger(0x0A).ConvertTo(&type::Uintptr::instance);
  Value y = Value::MakeInteger(0xFFFFFFFC).ConvertTo(&type::Uintptr::instance);
  Value z = x.BitXor(y);
  REQUIRE(z == Value::MakeInteger(0xFFFFFFF6)
          .ConvertTo(&type::Uintptr::instance));
}

TEST_CASE("Value::BitXor(int8, uint8)") {
  Value x = Value::MakeInteger(10).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(0xFC).ConvertTo(&type::Uint8::instance);
  Value z = x.BitXor(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Equal(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = x.Equal(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Equal(true, true)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(true);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(true, false)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeBoolean(false);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(Bool, Integer)") {
  Value x = Value::MakeBoolean(true);
  Value y = Value::MakeInteger(0);
  Value z = x.Equal(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Equal(Integer, Bool)") {
  Value x = Value::MakeInteger(0);
  Value y = Value::MakeBoolean(true);
  Value z = x.Equal(y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Equal(Integer, Integer)") {
  Value x = Value::MakeInteger(3);
  Value y = Value::MakeInteger(4);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(Rune, Rune)") {
  Value x = Value::MakeRune(3);
  Value y = Value::MakeRune(3);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(Float, Float)") {
  Value x = Value::MakeFloat(3);
  Value y = Value::MakeFloat(3.5);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(Complex, Complex)") {
  Value x = Value::MakeComplex(3.5, 4);
  Value y = Value::MakeComplex(3.5, 4);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(String, String)") {
  Value x = Value::MakeString("hello");
  Value y = Value::MakeString("hello, world");
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(bool, bool)") {
  Value x = Value::MakeBoolean(false).ConvertTo(&type::Bool::instance);
  Value y = Value::MakeBoolean(false);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(complex64, complex64)") {
  Value x = Value::MakeComplex(1, 2);
  Value y = Value::MakeComplex(3, 4).ConvertTo(&type::Complex64::instance);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(complex128, complex128)") {
  Value x = Value::MakeComplex(1, 2).ConvertTo(&type::Complex128::instance);
  Value y = Value::MakeComplex(1, 2);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(float32, float32)") {
  Value x = Value::MakeFloat(1);
  Value y = Value::MakeFloat(3).ConvertTo(&type::Float32::instance);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(float64, float64)") {
  Value x = Value::MakeFloat(1).ConvertTo(&type::Float64::instance);
  Value y = Value::MakeFloat(1);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(int, int)") {
  Value x = Value::MakeInteger(1);
  Value y = Value::MakeInteger(3).ConvertTo(&type::Int::instance);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(int8, int8)") {
  Value x = Value::MakeInteger(1).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(1);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(int16, int16)") {
  Value x = Value::MakeInteger(1);
  Value y = Value::MakeInteger(3).ConvertTo(&type::Int16::instance);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(int32, int32)") {
  Value x = Value::MakeInteger(1).ConvertTo(&type::Int32::instance);
  Value y = Value::MakeInteger(1);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(int64, int64)") {
  Value x = Value::MakeInteger(1);
  Value y = Value::MakeInteger(3).ConvertTo(&type::Int64::instance);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(uint, uint)") {
  Value x = Value::MakeInteger(1).ConvertTo(&type::Uint::instance);
  Value y = Value::MakeInteger(1);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(uint8, uint8)") {
  Value x = Value::MakeInteger(1);
  Value y = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(uint16, uint16)") {
  Value x = Value::MakeInteger(1).ConvertTo(&type::Uint16::instance);
  Value y = Value::MakeInteger(1);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(uint32, uint32)") {
  Value x = Value::MakeInteger(1);
  Value y = Value::MakeInteger(3).ConvertTo(&type::Uint32::instance);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(uint64, uint64)") {
  Value x = Value::MakeInteger(1).ConvertTo(&type::Uint64::instance);
  Value y = Value::MakeInteger(1);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(uintptr, uintptr)") {
  Value x = Value::MakeInteger(1);
  Value y = Value::MakeInteger(3).ConvertTo(&type::Uintptr::instance);
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(false));
}

TEST_CASE("Value::Equal(string, string)") {
  Value x = Value::MakeString("hello").ConvertTo(&type::String::instance);
  Value y = Value::MakeString("hello");
  Value z = x.Equal(y);
  REQUIRE(z == Value::MakeBoolean(true));
}

TEST_CASE("Value::Equal(int, string)") {
  Value x = Value::MakeInteger(1).ConvertTo(&type::Int::instance);
  Value y = Value::MakeString("hello").ConvertTo(&type::String::instance);
  Value z = x.Equal(y);
  REQUIRE(z.IsError());
}

// The bitwise logical and shift operators apply to integers only.

// The right operand in a shift expression must have unsigned integer
// type or be an untyped constant representable by a value of type
// uint. If the left operand of a non-constant shift expression is an
// untyped constant, it is first ConvertToed to the type it would assume
// if the shift expression were replaced by its left operand alone.

// A constant comparison always yields an untyped boolean constant. If
// the left operand of a constant shift expression is an untyped
// constant, the result is an integer constant; otherwise it is a
// constant of the same type as the left operand, which must be of
// integer type. Applying all other operators to untyped constants
// results in an untyped constant of the same kind(that is, a
// boolean, integer, floating-point, complex, or string constant).

// TODO(jrw972):  equal
// TODO(jrw972):  not_equal
// TODO(jrw972):  less_than
// TODO(jrw972):  less_than_equal
// TODO(jrw972):  greater_than
// TODO(jrw972):  greater_than_equal
// TODO(jrw972):  logic_and
// TODO(jrw972):  logic_or

// For instance, 3.0 can be given any integer or any floating-point
// type, while 2147483648.0(equal to 1<<31) can be given the types
// float32, float64, or uint32 but not int32 or string.

// An untyped constant has a default type which is the type to which
// the constant is implicitly ConvertToed in contexts where a typed
// value is required, for instance, in a short variable declaration
// such as i := 0 where there is no explicit type. The default type of
// an untyped constant is bool, rune, int, float64, complex128 or
// string respectively, depending on whether it is a boolean, rune,
// integer, floating-point, complex, or string constant.

// Implementation restriction: Although numeric constants have
// arbitrary precision in the language, a compiler may implement them
// using an internal representation with limited precision. That said,
// every implementation must:

// Represent integer constants with at least 256 bits.  Represent
// floating-point constants, including the parts of a complex
// constant, with a mantissa of at least 256 bits and a signed binary
// exponent of at least 16 bits.  Give an error if unable to represent
// an integer constant precisely.  Give an error if unable to
// represent a floating-point or complex constant due to overflow.
// Round to the nearest representable constant if unable to represent
// a floating-point or complex constant due to limits on precision.
// These requirements apply both to literal constants and to the
// result of evaluating constant expressions.

// Constant expressions may contain only constant operands and are
// evaluated at compile time.

// Untyped boolean, numeric, and string constants may be used as
// operands wherever it is legal to use an operand of boolean,
// numeric, or string type, respectively. Except for shift operations,
// if the operands of a binary operation are different kinds of
// untyped constants, the operation and, for non-boolean operations,
// the result use the kind that appears later in this list: integer,
// rune, floating-point, complex. For example, an untyped integer
// constant divided by an untyped complex constant yields an untyped
// complex constant.

// A constant comparison always yields an untyped boolean constant. If
// the left operand of a constant shift expression is an untyped
// constant, the result is an integer constant; otherwise it is a
// constant of the same type as the left operand, which must be of
// integer type. Applying all other operators to untyped constants
// results in an untyped constant of the same kind(that is, a boolean,
// integer, floating-point, complex, or string constant).

// const a = 2 + 3.0          // a == 5.0  (untyped floating-point constant)
// const b = 15 / 4           // b == 3    (untyped integer constant)
// const c = 15 / 4.0         // c == 3.75 (untyped floating-point constant)
// const Θ float64 = 3/2      // Θ == 1.0  (type float64, 3/2 is integer
//                                          division)
// const Π float64 = 3/2.     // Π == 1.5  (type float64, 3/2. is float
//                                          division)
// const d = 1 << 3.0         // d == 8    (untyped integer constant)
// const e = 1.0 << 3         // e == 8    (untyped integer constant)
// const f = int32(1) << 33   // illegal   (constant 8589934592 overflows int32)
// const g = float64(2) >> 1  // illegal   (float64(2) is a typed floating-point
//                                          constant)
// const h = "foo" > "bar"    // h == true (untyped boolean constant)
// const j = true             // j == true (untyped boolean constant)
// const k = 'w' + 1          // k == 'x'  (untyped rune constant)
// const l = "hi"             // l == "hi" (untyped string constant)
// const m = string(k)        // m == "x"  (type string)
// const Σ = 1 - 0.707i       //           (untyped complex constant)
// const Δ = Σ + 2.0e-4       //           (untyped complex constant)
// const Φ = iota*1i - 1/1i   //           (untyped complex constant)
// Applying the built-in function complex to untyped integer, rune, or
// floating-point constants yields an untyped complex constant.

// const ic = complex(0, c)   // ic == 3.75i (untyped complex constant)
// const iΘ = complex(0, Θ)   // iΘ == 1i    (type complex128)
// Constant expressions are always evaluated exactly; intermediate values and
// the constants themselves may require precision significantly larger than
// supported by any predeclared type in the language. The following are legal
// declarations:

// const Huge = 1 << 100         // Huge == 1267650600228229401496703205376
//                                          (untyped integer constant)
// const Four int8 = Huge >> 98  // Four == 4
//                                          (type int8)
// The divisor of a constant division or remainder operation must not be zero:

// 3.14 / 0.0   // illegal: division by zero
// The values of typed constants must always be accurately representable as
// values of the constant type. The following constant expressions are illegal:

// uint(-1)     // -1 cannot be represented as a uint
// int(3.14)    // 3.14 cannot be represented as an int
// int64(Huge)  // 1267650600228229401496703205376 cannot be represented as an
//              // int64
// Four * 300   // operand 300 cannot be represented as an int8(type of Four)
// Four * 100   // product 400 cannot be represented as an int8(type of Four)
// The mask used by the unary bitwise complement operator ^ matches the rule for
// non-constants: the mask is all 1s for unsigned constants and -1 for signed
// and untyped constants.

// ^1         // untyped integer constant, equal to -2
// uint8(^1)  // illegal: same as uint8(-2), -2 cannot be represented as a uint8
// ^uint8(1)  // typed uint8 constant, same as 0xFF ^ uint8(1) = uint8(0xFE)
// int8(^1)   // same as int8(-2)
// ^int8(1)   // same as -1 ^ int8(1) = -2
// Implementation restriction: A compiler may use rounding while computing
// untyped floating-point or complex constant expressions; see the
// implementation restriction in the section on constants. This rounding may
// cause a floating-point constant expression to be invalid in an integer
// context, even if it would be integral when calculated using infinite
// precision, and vice versa.

TEST_CASE("Value::operator==(Unknown, Error)") {
  Value x;
  Value y = Value::MakeError();
  REQUIRE(x != y);
}

TEST_CASE("Value::operator==(Unknown, Unknown)") {
  Value x;
  Value y;
  REQUIRE(x == y);
}

TEST_CASE("Value::operator==(int8, uint8)") {
  Value x = Value::MakeInteger(3).ConvertTo(&type::Int8::instance);
  Value y = Value::MakeInteger(3).ConvertTo(&type::Uint8::instance);
  REQUIRE(x != y);
}

TEST_CASE("operator<<(Unknown)") {
  std::stringstream ss;
  Value x;
  ss << x;
  REQUIRE(ss.str() == "unknown");
}

TEST_CASE("operator<<(Error)") {
  std::stringstream ss;
  Value x = Value::MakeError();
  ss << x;
  REQUIRE(ss.str() == "error");
}

TEST_CASE("operator<<(Boolean)") {
  {
    std::stringstream ss;
    Value x = Value::MakeBoolean(true);
    ss << x;
    REQUIRE(ss.str() == "true");
  }
  {
    std::stringstream ss;
    Value x = Value::MakeBoolean(false);
    ss << x;
    REQUIRE(ss.str() == "false");
  }
}

TEST_CASE("operator<<(Integer)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(Rune)") {
  {
    std::stringstream ss;
    Value x = Value::MakeRune('j');
    ss << x;
    REQUIRE(ss.str() == "'j'");
  }
  {
    std::stringstream ss;
    Value x = Value::MakeRune(INVALID);
    ss << x;
    REQUIRE(ss.str() == "1114112");
  }
  {
    std::stringstream ss;
    Value x = Value::MakeRune(SURROGATE_FIRST);
    ss << x;
    REQUIRE(ss.str() == "55296");
  }
}

TEST_CASE("operator<<(Float)") {
  std::stringstream ss;
  Value x = Value::MakeFloat(34.5);
  ss << x;
  REQUIRE(ss.str() == "34.5");
}

TEST_CASE("operator<<(Complex)") {
  std::stringstream ss;
  Value x = Value::MakeComplex(1, 2);
  ss << x;
  REQUIRE(ss.str() == "1+2i");
}

TEST_CASE("operator<<(String)") {
  std::stringstream ss;
  Value x = Value::MakeString("hello");
  ss << x;
  REQUIRE(ss.str() == "\"hello\"");
}

TEST_CASE("operator<<(bool)") {
  {
    std::stringstream ss;
    Value x = Value::MakeBoolean(true).ConvertTo(&type::Bool::instance);
    ss << x;
    REQUIRE(ss.str() == "true");
  }
  {
    std::stringstream ss;
    Value x = Value::MakeBoolean(false).ConvertTo(&type::Bool::instance);
    ss << x;
    REQUIRE(ss.str() == "false");
  }
}

TEST_CASE("operator<<(int8)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Int8::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(int16)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Int16::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(int32)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Int32::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(int64)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Int64::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uint8)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Uint8::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uint16)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Uint16::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uint32)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Uint32::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uint64)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Uint64::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(int)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Int::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uint)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Uint::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uintptr)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Uintptr::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(float32)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Float32::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(float64)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Float64::instance);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(complex64)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Complex64::instance);
  ss << x;
  REQUIRE(ss.str() == "34+0i");
}

TEST_CASE("operator<<(complex128)") {
  std::stringstream ss;
  Value x = Value::MakeInteger(34).ConvertTo(&type::Complex128::instance);
  ss << x;
  REQUIRE(ss.str() == "34+0i");
}

TEST_CASE("operator<<(string)") {
  std::stringstream ss;
  Value x = Value::MakeString("hello").ConvertTo(&type::String::instance);
  ss << x;
  REQUIRE(ss.str() == "\"hello\"");
}

}  // namespace test
}  // namespace rcgo
