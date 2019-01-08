// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/value.h"

#include <utility>

#include "src/catch.hpp"
#include "src/test_helpers.h"
#include "src/type.h"
#include "src/utf8.h"

namespace rcgo {
using std::rel_ops::operator!=;
namespace test {

TEST_CASE("complex64_t::complex64_t()") {
  value::complex64_t c;
  REQUIRE(c.real() == 0);
  REQUIRE(c.imag() == 0);
}

TEST_CASE("complex64_t::complex64_t(1,2)") {
  value::complex64_t c(1, 2);
  REQUIRE(c.real() == 1);
  REQUIRE(c.imag() == 2);
}

TEST_CASE("complex64_t::operator+") {
  value::complex64_t c1(1, 2);
  value::complex64_t c2(3, 4);
  value::complex64_t c3 = c1 + c2;
  REQUIRE(c3 == value::complex64_t(4, 6));
}

TEST_CASE("complex64_t::operator-") {
  value::complex64_t c1(1, 2);
  value::complex64_t c2(3, 4);
  value::complex64_t c3 = c1 - c2;
  REQUIRE(c3 == value::complex64_t(-2, -2));
}

TEST_CASE("complex64_t::operator*") {
  value::complex64_t c1(1, 2);
  value::complex64_t c2(3, 4);
  value::complex64_t c3 = c1 * c2;
  REQUIRE(c3 == value::complex64_t(-5, 10));
}

TEST_CASE("complex64_t::operator/") {
  value::complex64_t c1(1, 0);
  value::complex64_t c2(0, 1);
  value::complex64_t c3 = c1 / c2;
  REQUIRE(c3 == value::complex64_t(0, -1));
}

TEST_CASE("complex64_t::operator+ unary") {
  value::complex64_t c1(1, 1);
  value::complex64_t c2 = +c1;
  REQUIRE(c2 == value::complex64_t(1, 1));
}

TEST_CASE("complex64_t::operator- unary") {
  value::complex64_t c1(1, 1);
  value::complex64_t c2 = -c1;
  REQUIRE(c2 == value::complex64_t(-1, -1));
}

TEST_CASE("complex64_t::operator==") {
  value::complex64_t c1(1, 2);
  value::complex64_t c2(3, 4);
  REQUIRE(c1 == c1);
  REQUIRE(c1 != c2);
}

TEST_CASE("operator<< complex64_t") {
  std::stringstream s1;
  value::complex64_t c1(1, 2);
  s1 << c1;
  REQUIRE(s1.str() == "1+2i");
  std::stringstream s2;
  value::complex64_t c2(1, -2);
  s2 << c2;
  REQUIRE(s2.str() == "1-2i");
}

TEST_CASE("complex128_t::complex128_t()") {
  value::complex128_t c;
  REQUIRE(c.real() == 0);
  REQUIRE(c.imag() == 0);
}

TEST_CASE("complex128_t::complex128_t(1,2)") {
  value::complex128_t c(1, 2);
  REQUIRE(c.real() == 1);
  REQUIRE(c.imag() == 2);
}

TEST_CASE("complex128_t::operator+") {
  value::complex128_t c1(1, 2);
  value::complex128_t c2(3, 4);
  value::complex128_t c3 = c1 + c2;
  REQUIRE(c3 == value::complex128_t(4, 6));
}

TEST_CASE("complex128_t::operator-") {
  value::complex128_t c1(1, 2);
  value::complex128_t c2(3, 4);
  value::complex128_t c3 = c1 - c2;
  REQUIRE(c3 == value::complex128_t(-2, -2));
}

TEST_CASE("complex128_t::operator*") {
  value::complex128_t c1(1, 2);
  value::complex128_t c2(3, 4);
  value::complex128_t c3 = c1 * c2;
  REQUIRE(c3 == value::complex128_t(-5, 10));
}

TEST_CASE("complex128_t::operator/") {
  value::complex128_t c1(1, 0);
  value::complex128_t c2(0, 1);
  value::complex128_t c3 = c1 / c2;
  REQUIRE(c3 == value::complex128_t(0, -1));
}

TEST_CASE("complex128_t::operator+ unary") {
  value::complex128_t c1(1, 2);
  value::complex128_t c2 = +c1;
  REQUIRE(c2 == value::complex128_t(1, 2));
}

TEST_CASE("complex128_t::operator- unary") {
  value::complex128_t c1(1, 2);
  value::complex128_t c2 = -c1;
  REQUIRE(c2 == value::complex128_t(-1, -2));
}

TEST_CASE("complex128_t::operator==") {
  value::complex128_t c1(1, 2);
  value::complex128_t c2(3, 4);
  REQUIRE(c1 == c1);
  REQUIRE(c1 != c2);
}

TEST_CASE("operator<< complex128_t") {
  std::stringstream s1;
  value::complex128_t c1(1, 2);
  s1 << c1;
  REQUIRE(s1.str() == "1+2i");
  std::stringstream s2;
  value::complex128_t c2(1, -2);
  s2 << c2;
  REQUIRE(s2.str() == "1-2i");
}

TEST_CASE("complex_t::complex_t()") {
  value::complex_t c;
  REQUIRE(c.real() == 0);
  REQUIRE(c.imag() == 0);
}

TEST_CASE("complex_t::complex_t(1,2)") {
  value::complex_t c(1, 2);
  REQUIRE(c.real() == 1);
  REQUIRE(c.imag() == 2);
}

TEST_CASE("complex_t::operator+") {
  value::complex_t c1(1, 2);
  value::complex_t c2(3, 4);
  value::complex_t c3 = c1 + c2;
  REQUIRE(c3 == value::complex_t(4, 6));
}

TEST_CASE("complex_t::operator-") {
  value::complex_t c1(1, 2);
  value::complex_t c2(3, 4);
  value::complex_t c3 = c1 - c2;
  REQUIRE(c3 == value::complex_t(-2, -2));
}

TEST_CASE("complex_t::operator*") {
  value::complex_t c1(1, 2);
  value::complex_t c2(3, 4);
  value::complex_t c3 = c1 * c2;
  REQUIRE(c3 == value::complex_t(-5, 10));
}

TEST_CASE("complex_t::operator/") {
  value::complex_t c1(1, 0);
  value::complex_t c2(0, 1);
  value::complex_t c3 = c1 / c2;
  REQUIRE(c3 == value::complex_t(0, -1));
}

TEST_CASE("complex_t::operator+ unary") {
  value::complex_t c1(1, 2);
  value::complex_t c2 = +c1;
  REQUIRE(c2 == value::complex_t(1, 2));
}

TEST_CASE("complex_t::operator- unary") {
  value::complex_t c1(1, 2);
  value::complex_t c2 = -c1;
  REQUIRE(c2 == value::complex_t(-1, -2));
}

TEST_CASE("complex_t::operator==") {
  value::complex_t c1(1, 2);
  value::complex_t c2(3, 4);
  REQUIRE(c1 == c1);
  REQUIRE(c1 != c2);
}

TEST_CASE("operator<< complex_t") {
  std::stringstream s1;
  value::complex_t c1(1, 2);
  s1 << c1;
  REQUIRE(s1.str() == "1+2i");
  std::stringstream s2;
  value::complex_t c2(1, -2);
  s2 << c2;
  REQUIRE(s2.str() == "1-2i");
}

TEST_CASE("value::Value()") {
  value::Value v;
  REQUIRE(v.kind() == value::Value::kUnitialized);
  REQUIRE(v.IsUninitialized());
  REQUIRE(!v.IsInitialized());
}

TEST_CASE("value::MakeError()") {
  value::Value v = value::Value::MakeError();
  REQUIRE(v.kind() == value::Value::kError);
  REQUIRE(v.IsError());
}

TEST_CASE("value::MakeBoolean()") {
  value::Value v = value::Value::MakeBoolean(true);
  REQUIRE(v.kind() == value::Value::kBoolean);
  REQUIRE(v.Boolean_value() == true);
}

TEST_CASE("value::MakeInteger()") {
  value::Value v = value::Value::MakeInteger(3);
  REQUIRE(v.kind() == value::Value::kInteger);
  REQUIRE(v.Integer_value() == 3);
}

TEST_CASE("value::MakeRune()") {
  value::Value v = value::Value::MakeRune(123);
  REQUIRE(v.kind() == value::Value::kRune);
  REQUIRE(v.Rune_value() == 123);
}

TEST_CASE("value::MakeFloat()") {
  value::Value v = value::Value::MakeFloat(3.14);
  REQUIRE(v.kind() == value::Value::kFloat);
  REQUIRE(v.Float_value() == 3.14);
}

TEST_CASE("value::MakeComplex()") {
  value::Value v = value::Value::MakeComplex(1, 2);
  REQUIRE(v.kind() == value::Value::kComplex);
  REQUIRE(v.Complex_value() == value::complex_t(1, 2));
}

TEST_CASE("value::MakeString()") {
  value::Value v = value::Value::MakeString("hello");
  REQUIRE(v.kind() == value::Value::kString);
  REQUIRE(v.String_value() == "hello");
}

TEST_CASE("value::ConvertTo(Error -> bool)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  REQUIRE(!x.ConvertTo(&type::Bool::instance));
  REQUIRE(x.IsError());
}

TEST_CASE("value::ConvertTo(Boolean -> bool)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  REQUIRE(x.ConvertTo(&type::Bool::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Bool::instance);
  REQUIRE(x.bool_value() == true);
}

TEST_CASE("value::ConvertTo(Integer -> complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Complex64::instance);
  REQUIRE(x.complex64_value() == value::complex64_t(34, 0));
}

TEST_CASE("value::ConvertTo(Integer -> complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Complex128::instance);
  REQUIRE(x.complex128_value() == value::complex128_t(34, 0));
}

TEST_CASE("value::ConvertTo(Integer -> float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Float32::instance);
  REQUIRE(x.float32_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Float64::instance);
  REQUIRE(x.float64_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int::instance);
  REQUIRE(x.int_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int8::instance);
  REQUIRE(x.int8_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int16::instance);
  REQUIRE(x.int16_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int32::instance);
  REQUIRE(x.int32_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int64::instance);
  REQUIRE(x.int64_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint::instance);
  REQUIRE(x.uint_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint8::instance);
  REQUIRE(x.uint8_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint16::instance);
  REQUIRE(x.uint16_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint32::instance);
  REQUIRE(x.uint32_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint64::instance);
  REQUIRE(x.uint64_value() == 34);
}

TEST_CASE("value::ConvertTo(Integer -> uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uintptr::instance);
  REQUIRE(x.uintptr_value() == 34);
}

TEST_CASE("value::ConvertTo(Float 0 -> complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(0);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Complex64::instance);
  REQUIRE(x.complex64_value() == value::complex64_t(0, 0));
}

TEST_CASE("value::ConvertTo(Float -> complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Complex64::instance);
  REQUIRE(x.complex64_value() == value::complex64_t(34, 0));
}

TEST_CASE("value::ConvertTo(Float -> complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Complex128::instance);
  REQUIRE(x.complex128_value() == value::complex128_t(34, 0));
}

TEST_CASE("value::ConvertTo(Float -> float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Float32::instance);
  REQUIRE(x.float32_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Float64::instance);
  REQUIRE(x.float64_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int::instance);
  REQUIRE(x.int_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int8::instance);
  REQUIRE(x.int8_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int16::instance);
  REQUIRE(x.int16_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int32::instance);
  REQUIRE(x.int32_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int64::instance);
  REQUIRE(x.int64_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint::instance);
  REQUIRE(x.uint_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint8::instance);
  REQUIRE(x.uint8_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint16::instance);
  REQUIRE(x.uint16_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint32::instance);
  REQUIRE(x.uint32_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint64::instance);
  REQUIRE(x.uint64_value() == 34);
}

TEST_CASE("value::ConvertTo(Float -> uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(34);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uintptr::instance);
  REQUIRE(x.uintptr_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Complex64::instance);
  REQUIRE(x.complex64_value() == value::complex64_t(1, 2));
}

TEST_CASE("value::ConvertTo(Complex -> complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Complex128::instance);
  REQUIRE(x.complex128_value() == value::complex128_t(1, 2));
}

TEST_CASE("value::ConvertTo(Complex -> float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Float32::instance);
  REQUIRE(x.float32_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Float64::instance);
  REQUIRE(x.float64_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int::instance);
  REQUIRE(x.int_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int8::instance);
  REQUIRE(x.int8_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int16::instance);
  REQUIRE(x.int16_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int32::instance);
  REQUIRE(x.int32_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int64::instance);
  REQUIRE(x.int64_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint::instance);
  REQUIRE(x.uint_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint8::instance);
  REQUIRE(x.uint8_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint16::instance);
  REQUIRE(x.uint16_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint32::instance);
  REQUIRE(x.uint32_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint64::instance);
  REQUIRE(x.uint64_value() == 34);
}

TEST_CASE("value::ConvertTo(Complex -> uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(34, 0);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uintptr::instance);
  REQUIRE(x.uintptr_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Complex64::instance);
  REQUIRE(x.complex64_value() == value::complex64_t(34, 0));
}

TEST_CASE("value::ConvertTo(Rune -> complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Complex128::instance);
  REQUIRE(x.complex128_value() == value::complex128_t(34, 0));
}

TEST_CASE("value::ConvertTo(Rune -> float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Float32::instance);
  REQUIRE(x.float32_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Float64::instance);
  REQUIRE(x.float64_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int::instance);
  REQUIRE(x.int_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int8::instance);
  REQUIRE(x.int8_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int16::instance);
  REQUIRE(x.int16_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int32::instance);
  REQUIRE(x.int32_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Int64::instance);
  REQUIRE(x.int64_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint::instance);
  REQUIRE(x.uint_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint8::instance);
  REQUIRE(x.uint8_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint16::instance);
  REQUIRE(x.uint16_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint32::instance);
  REQUIRE(x.uint32_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uint64::instance);
  REQUIRE(x.uint64_value() == 34);
}

TEST_CASE("value::ConvertTo(Rune -> uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(34);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::Uintptr::instance);
  REQUIRE(x.uintptr_value() == 34);
}

TEST_CASE("value::ConvertTo(String -> string)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeString("hello");
  REQUIRE(x.ConvertTo(&type::String::instance));
  REQUIRE(x.kind() == value::Value::kConstant);
  REQUIRE(x.type() == &type::String::instance);
  REQUIRE(x.string_value() == "hello");
}

TEST_CASE("value::Value::Posate(Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::Posate(location, &x, &er);
  REQUIRE(y.IsError());
}

TEST_CASE("value::Value::Posate(Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::Posate(location, &x, &er);
  REQUIRE(er.Count() == 1);
  REQUIRE(y.IsError());
}

TEST_CASE("value::Value::Posate(Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::Posate(location, &x, &er);
  REQUIRE(y == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::Posate(Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(3);
  value::Value y = value::Value::Posate(location, &x, &er);
  REQUIRE(y == value::Value::MakeRune(3));
}

TEST_CASE("value::Value::Posate(Float)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(1.5);
  value::Value y = value::Value::Posate(location, &x, &er);
  REQUIRE(y == value::Value::MakeFloat(1.5));
}

TEST_CASE("value::Value::Posate(Complex)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  value::Value y = value::Value::Posate(location, &x, &er);
  REQUIRE(y == value::Value::MakeComplex(1, 2));
}

TEST_CASE("value::Value::Posate(int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Int8::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Int16::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Int32::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Int64::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Uint8::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Uint16::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Uint32::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Uint64::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Int::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Uint::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Uintptr::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Float32::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(3);
  REQUIRE(z.ConvertTo(&type::Float64::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeComplex(1, 2);
  REQUIRE(z.ConvertTo(&type::Complex64::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Posate(complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  value::Value y = value::Value::Posate(location, &x, &er);
  value::Value z = value::Value::MakeComplex(1, 2);
  REQUIRE(z.ConvertTo(&type::Complex128::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::Negate(location, &x, &er);
  REQUIRE(x.IsError());
}

TEST_CASE("value::Value::Negate(Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::Negate(location, &x, &er);
  REQUIRE(er.Count() == 1);
  REQUIRE(y.IsError());
}

TEST_CASE("value::Value::Negate(Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::Negate(location, &x, &er);
  REQUIRE(y == value::Value::MakeInteger(-3));
}

TEST_CASE("value::Value::Negate(Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(3);
  value::Value y = value::Value::Negate(location, &x, &er);
  REQUIRE(y == value::Value::MakeRune(-3));
}

TEST_CASE("value::Value::Negate(Float)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(1.5);
  value::Value y = value::Value::Negate(location, &x, &er);
  REQUIRE(y == value::Value::MakeFloat(-1.5));
}

TEST_CASE("value::Value::Negate(Complex)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  value::Value y = value::Value::Negate(location, &x, &er);
  REQUIRE(y == value::Value::MakeComplex(-1, -2));
}

TEST_CASE("value::Value::Negate(int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-3);
  REQUIRE(z.ConvertTo(&type::Int8::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-3);
  REQUIRE(z.ConvertTo(&type::Int16::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-3);
  REQUIRE(z.ConvertTo(&type::Int32::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-3);
  REQUIRE(z.ConvertTo(&type::Int64::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(253);
  REQUIRE(z.ConvertTo(&type::Uint8::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(65533);
  REQUIRE(z.ConvertTo(&type::Uint16::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(4294967293);
  REQUIRE(z.ConvertTo(&type::Uint32::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(18446744073709551613ul);
  REQUIRE(z.ConvertTo(&type::Uint64::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-3);
  REQUIRE(z.ConvertTo(&type::Int::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(4294967293);
  REQUIRE(z.ConvertTo(&type::Uint::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(4294967293);
  REQUIRE(z.ConvertTo(&type::Uintptr::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-3);
  REQUIRE(z.ConvertTo(&type::Float32::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-3);
  REQUIRE(z.ConvertTo(&type::Float64::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeComplex(-1, -2);
  REQUIRE(z.ConvertTo(&type::Complex64::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Negate(complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  value::Value y = value::Value::Negate(location, &x, &er);
  value::Value z = value::Value::MakeComplex(-1, -2);
  REQUIRE(z.ConvertTo(&type::Complex128::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::LogicNot(Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::LogicNot(location, &x, &er);
  REQUIRE(y.IsError());
}

TEST_CASE("value::Value::LogicNot(Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::LogicNot(location, &x, &er);
  REQUIRE(y == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::LogicNot(Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::LogicNot(location, &x, &er);
  REQUIRE(y.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::LogicNot(bool)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  REQUIRE(x.ConvertTo(&type::Bool::instance));
  value::Value y = value::Value::LogicNot(location, &x, &er);
  value::Value z = value::Value::MakeBoolean(false);
  REQUIRE(z.ConvertTo(&type::Bool::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::BitNot(location, &x, &er);
  REQUIRE(y.IsError());
}

TEST_CASE("value::Value::BitNot(Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::BitNot(location, &x, &er);
  REQUIRE(y.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::BitNot(Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::BitNot(location, &x, &er);
  REQUIRE(y == value::Value::MakeInteger(-4));
}

TEST_CASE("value::Value::BitNot(Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(3);
  value::Value y = value::Value::BitNot(location, &x, &er);
  REQUIRE(y == value::Value::MakeRune(-4));
}

TEST_CASE("value::Value::BitNot(int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-4);
  REQUIRE(z.ConvertTo(&type::Int8::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-4);
  REQUIRE(z.ConvertTo(&type::Int16::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-4);
  REQUIRE(z.ConvertTo(&type::Int32::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-4);
  REQUIRE(z.ConvertTo(&type::Int64::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(252);
  REQUIRE(z.ConvertTo(&type::Uint8::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(65532);
  REQUIRE(z.ConvertTo(&type::Uint16::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(4294967292);
  REQUIRE(z.ConvertTo(&type::Uint32::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(18446744073709551612ul);
  REQUIRE(z.ConvertTo(&type::Uint64::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(-4);
  REQUIRE(z.ConvertTo(&type::Int::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(4294967292);
  REQUIRE(z.ConvertTo(&type::Uint::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::BitNot(uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::BitNot(location, &x, &er);
  value::Value z = value::Value::MakeInteger(4294967292);
  REQUIRE(z.ConvertTo(&type::Uintptr::instance));
  REQUIRE(y == z);
}

TEST_CASE("value::Value::Multiply(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::Multiply(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Multiply(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(12));
}

TEST_CASE("value::Value::Multiply(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(12));
}

TEST_CASE("value::Value::Multiply(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeRune(4);
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(12));
}

TEST_CASE("value::Value::Multiply(Float, Float)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(1.5);
  value::Value y = value::Value::MakeFloat(2.25);
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeFloat(3.375));
}

TEST_CASE("value::Value::Multiply(Complex, Complex)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  value::Value y = value::Value::MakeComplex(3, 4);
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeComplex(-5, 10));
}

TEST_CASE("value::Value::Multiply(int8, int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int8::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(int16, int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int16::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(int32, int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int32::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(int64, int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int64::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(uint8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(uint16, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(uint32, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(uint64, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(int, int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(uint, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(uintptr, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(float32, float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  value::Value y = value::Value::MakeFloat(4);
  REQUIRE(y.ConvertTo(&type::Float32::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Float32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(float64, float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  value::Value y = value::Value::MakeFloat(4);
  REQUIRE(y.ConvertTo(&type::Float64::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(12);
  REQUIRE(t.ConvertTo(&type::Float64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(complex64, complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  value::Value y = value::Value::MakeComplex(3, 4);
  REQUIRE(y.ConvertTo(&type::Complex64::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeComplex(-5, 10);
  REQUIRE(t.ConvertTo(&type::Complex64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(complex128, complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  value::Value y = value::Value::MakeComplex(3, 4);
  REQUIRE(y.ConvertTo(&type::Complex128::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  value::Value t = value::Value::MakeComplex(-5, 10);
  REQUIRE(t.ConvertTo(&type::Complex128::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Multiply(int8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Multiply(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Divide(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::Divide(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Divide(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(0));
}

TEST_CASE("value::Value::Divide(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(0));
}

TEST_CASE("value::Value::Divide(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeRune(4);
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(0));
}

TEST_CASE("value::Value::Divide(Float, Float)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(1);
  value::Value y = value::Value::MakeFloat(2);
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeFloat(.5));
}

TEST_CASE("value::Value::Divide(Complex, Complex)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 0);
  value::Value y = value::Value::MakeComplex(0, 1);
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeComplex(0, -1));
}

TEST_CASE("value::Value::Divide(int8, int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int8::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(int16, int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int16::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(int32, int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int32::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(int64, int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int64::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(uint8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(uint16, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(uint32, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(uint64, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(int, int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(uint, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(uintptr, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(float32, float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  value::Value y = value::Value::MakeFloat(4);
  REQUIRE(y.ConvertTo(&type::Float32::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeFloat(.75);
  REQUIRE(t.ConvertTo(&type::Float32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(float64, float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  value::Value y = value::Value::MakeFloat(4);
  REQUIRE(y.ConvertTo(&type::Float64::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeFloat(.75);
  REQUIRE(t.ConvertTo(&type::Float64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(complex64, complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 0);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  value::Value y = value::Value::MakeComplex(0, -1);
  REQUIRE(y.ConvertTo(&type::Complex64::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeComplex(0, 1);
  REQUIRE(t.ConvertTo(&type::Complex64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(complex128, complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 0);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  value::Value y = value::Value::MakeComplex(0, -1);
  REQUIRE(y.ConvertTo(&type::Complex128::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  value::Value t = value::Value::MakeComplex(0, 1);
  REQUIRE(t.ConvertTo(&type::Complex128::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Divide(int8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Divide(Integer, 0)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(0);
  value::Value z = value::Value::Divide(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Modulo(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::Modulo(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Modulo(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::Modulo(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(3));
}

TEST_CASE("value::Value::Modulo(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeRune(4);
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(3));
}

TEST_CASE("value::Value::Modulo(int8, int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int8::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(int16, int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int16::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(int32, int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int32::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(int64, int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int64::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(uint8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(uint16, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(uint32, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(uint64, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(int, int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(uint, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(uintptr, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Modulo(int8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Modulo(Integer, 0)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(0);
  value::Value z = value::Value::Modulo(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::LeftShift(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::LeftShift(Boolean, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::LeftShift(Integer, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::LeftShift(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeRune(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Integer, Float)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeFloat(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Integer, Complex)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeComplex(4, 0);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Integer, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Integer, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Integer, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Integer, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Integer, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Integer, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(int8, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(int16, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(int32, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(int64, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(uint8, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(uint16, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(uint32, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(uint64, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(int, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(uint, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(uintptr, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(48);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::LeftShift(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Float, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::LeftShift(Complex, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(3, 0);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::LeftShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(48));
}

TEST_CASE("value::Value::RightShift(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::RightShift(Boolean, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::RightShift(Integer, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::RightShift(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  value::Value y = value::Value::MakeRune(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Integer, Float)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  value::Value y = value::Value::MakeFloat(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Integer, Complex)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  value::Value y = value::Value::MakeComplex(4, 0);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Integer, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Integer, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Integer, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Integer, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Integer, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Integer, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(int8, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(int16, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(int32, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(int64, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(uint8, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(uint16, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(uint32, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(uint64, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(int, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(uint, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(uintptr, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(48);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(3);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::RightShift(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(48);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Float, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(48);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::RightShift(Complex, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(48, 0);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::RightShift(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(3));
}

TEST_CASE("value::Value::BitAnd(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::BitAnd(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::BitAnd(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  value::Value y = value::Value::MakeInteger(-4);
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(8));
}

TEST_CASE("value::Value::BitAnd(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(10);
  value::Value y = value::Value::MakeInteger(-4);
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(8));
}

TEST_CASE("value::Value::BitAnd(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  value::Value y = value::Value::MakeRune(-4);
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(8));
}

TEST_CASE("value::Value::BitAnd(int8, int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int8::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(int16, int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int16::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(int32, int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int32::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(int64, int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int64::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(uint8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(0xFC);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(uint16, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(0xFFFC);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(uint32, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(uint64, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFFFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(int, int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(uint, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(uintptr, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(8);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAnd(int8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(0xFC);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::BitAnd(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::BitAndNot(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::BitAndNot(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::BitAndNot(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  value::Value y = value::Value::MakeInteger(-4);
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(2));
}

TEST_CASE("value::Value::BitAndNot(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(10);
  value::Value y = value::Value::MakeInteger(-4);
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(2));
}

TEST_CASE("value::Value::BitAndNot(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  value::Value y = value::Value::MakeRune(-4);
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(2));
}

TEST_CASE("value::Value::BitAndNot(int8, int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int8::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(int16, int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int16::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(int32, int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int32::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(int64, int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int64::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(uint8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(0xFC);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(uint16, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(0xFFFC);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(uint32, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(uint64, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFFFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(int, int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(uint, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(uintptr, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(2);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitAndNot(int8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(0xFC);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::BitAndNot(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Add(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::Add(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::Add(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::Add(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Add(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Add(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(7));
}

TEST_CASE("value::Value::Add(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Add(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(7));
}

TEST_CASE("value::Value::Add(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeRune(4);
  value::Value z = value::Value::Add(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(7));
}

TEST_CASE("value::Value::Add(Float, Float)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(1.5);
  value::Value y = value::Value::MakeFloat(2.25);
  value::Value z = value::Value::Add(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeFloat(3.75));
}

TEST_CASE("value::Value::Add(Complex, Complex)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  value::Value y = value::Value::MakeComplex(3, 4);
  value::Value z = value::Value::Add(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeComplex(4, 6));
}

TEST_CASE("value::Value::Add(String, String)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeString("hello ");
  value::Value y = value::Value::MakeString("world");
  value::Value z = value::Value::Add(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeString("hello world"));
}

TEST_CASE("value::Value::Add(int8, int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int8::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(int16, int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int16::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(int32, int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int32::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(int64, int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int64::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(uint8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(uint16, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(uint32, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(uint64, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(int, int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(uint, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(uintptr, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(7);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(float32, float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  value::Value y = value::Value::MakeFloat(4);
  REQUIRE(y.ConvertTo(&type::Float32::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeFloat(7);
  REQUIRE(t.ConvertTo(&type::Float32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(float64, float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  value::Value y = value::Value::MakeFloat(4);
  REQUIRE(y.ConvertTo(&type::Float64::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeFloat(7);
  REQUIRE(t.ConvertTo(&type::Float64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(complex64, complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  value::Value y = value::Value::MakeComplex(3, 4);
  REQUIRE(y.ConvertTo(&type::Complex64::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeComplex(4, 6);
  REQUIRE(t.ConvertTo(&type::Complex64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(complex128, complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  value::Value y = value::Value::MakeComplex(3, 4);
  REQUIRE(y.ConvertTo(&type::Complex128::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeComplex(4, 6);
  REQUIRE(t.ConvertTo(&type::Complex128::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(string, string)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeString("hello ");
  REQUIRE(x.ConvertTo(&type::String::instance));
  value::Value y = value::Value::MakeString("world");
  REQUIRE(y.ConvertTo(&type::String::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  value::Value t = value::Value::MakeString("hello world");
  REQUIRE(t.ConvertTo(&type::String::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Add(int8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Add(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Subtract(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::Subtract(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Subtract(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(-1));
}

TEST_CASE("value::Value::Subtract(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(-1));
}

TEST_CASE("value::Value::Subtract(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeRune(4);
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(-1));
}

TEST_CASE("value::Value::Subtract(Float, Float)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(1.5);
  value::Value y = value::Value::MakeFloat(2.25);
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeFloat(-0.75));
}

TEST_CASE("value::Value::Subtract(Complex, Complex)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  value::Value y = value::Value::MakeComplex(3, 4);
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeComplex(-2, -2));
}

TEST_CASE("value::Value::Subtract(int8, int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int8::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-1);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(int16, int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int16::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-1);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(int32, int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int32::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-1);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(int64, int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int64::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-1);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(uint8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(255);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(uint16, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(65535);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(uint32, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(4294967295);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(uint64, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(18446744073709551615UL);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(int, int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Int::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-1);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(uint, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(4294967295);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(uintptr, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(4294967295);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(float32, float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  value::Value y = value::Value::MakeFloat(4);
  REQUIRE(y.ConvertTo(&type::Float32::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeFloat(-1);
  REQUIRE(t.ConvertTo(&type::Float32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(float64, float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  value::Value y = value::Value::MakeFloat(4);
  REQUIRE(y.ConvertTo(&type::Float64::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeFloat(-1);
  REQUIRE(t.ConvertTo(&type::Float64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(complex64, complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  value::Value y = value::Value::MakeComplex(3, 4);
  REQUIRE(y.ConvertTo(&type::Complex64::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeComplex(-2, -2);
  REQUIRE(t.ConvertTo(&type::Complex64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(complex128, complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  value::Value y = value::Value::MakeComplex(3, 4);
  REQUIRE(y.ConvertTo(&type::Complex128::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  value::Value t = value::Value::MakeComplex(-2, -2);
  REQUIRE(t.ConvertTo(&type::Complex128::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::Subtract(int8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(4);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Subtract(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::BitOr(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::BitOr(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::BitOr(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  value::Value y = value::Value::MakeInteger(-4);
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(-2));
}

TEST_CASE("value::Value::BitOr(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(10);
  value::Value y = value::Value::MakeInteger(-4);
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(-2));
}

TEST_CASE("value::Value::BitOr(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  value::Value y = value::Value::MakeRune(-4);
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(-2));
}

TEST_CASE("value::Value::BitOr(int8, int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int8::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-2);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(int16, int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int16::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-2);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(int32, int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int32::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-2);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(int64, int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int64::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-2);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(uint8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(0xFC);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFE);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(uint16, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(0xFFFC);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFFFE);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(uint32, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFFFFFFFE);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(uint64, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFFFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFFFFFFFFFFFFFFFE);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(int, int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-2);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(uint, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFFFFFFFE);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(uintptr, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFFFFFFFE);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitOr(int8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(0xFC);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::BitOr(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::BitXor(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::BitXor(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::BitXor(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  value::Value y = value::Value::MakeInteger(-4);
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeInteger(-10));
}

TEST_CASE("value::Value::BitXor(Rune, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(10);
  value::Value y = value::Value::MakeInteger(-4);
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(-10));
}

TEST_CASE("value::Value::BitXor(Integer, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  value::Value y = value::Value::MakeRune(-4);
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeRune(-10));
}

TEST_CASE("value::Value::BitXor(int8, int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int8::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-10);
  REQUIRE(t.ConvertTo(&type::Int8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(int16, int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int16::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-10);
  REQUIRE(t.ConvertTo(&type::Int16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(int32, int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int32::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-10);
  REQUIRE(t.ConvertTo(&type::Int32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(int64, int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int64::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-10);
  REQUIRE(t.ConvertTo(&type::Int64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(uint8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  value::Value y = value::Value::MakeInteger(0xFC);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xF6);
  REQUIRE(t.ConvertTo(&type::Uint8::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(uint16, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(0xFFFC);
  REQUIRE(y.ConvertTo(&type::Uint16::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFFF6);
  REQUIRE(t.ConvertTo(&type::Uint16::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(uint32, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFFFFFFF6);
  REQUIRE(t.ConvertTo(&type::Uint32::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(uint64, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFFFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint64::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFFFFFFFFFFFFFFF6);
  REQUIRE(t.ConvertTo(&type::Uint64::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(int, int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeInteger(-4);
  REQUIRE(y.ConvertTo(&type::Int::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(-10);
  REQUIRE(t.ConvertTo(&type::Int::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(uint, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uint::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFFFFFFF6);
  REQUIRE(t.ConvertTo(&type::Uint::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(uintptr, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(0x0A);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  value::Value y = value::Value::MakeInteger(0xFFFFFFFC);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  value::Value t = value::Value::MakeInteger(0xFFFFFFF6);
  REQUIRE(t.ConvertTo(&type::Uintptr::instance));
  REQUIRE(z == t);
}

TEST_CASE("value::Value::BitXor(int8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(10);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(0xFC);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::BitXor(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Equal(Error, Error)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeError();
  value::Value y = value::Value::MakeError();
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z.IsError());
}

TEST_CASE("value::Value::Equal(true, true)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(true, false)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeBoolean(false);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(Bool, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeBoolean(true);
  value::Value y = value::Value::MakeInteger(0);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Equal(Integer, Bool)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(0);
  value::Value y = value::Value::MakeBoolean(true);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::Value::Equal(Integer, Integer)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  value::Value y = value::Value::MakeInteger(4);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(Rune, Rune)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeRune(3);
  value::Value y = value::Value::MakeRune(3);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(Float, Float)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(3);
  value::Value y = value::Value::MakeFloat(3.5);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(Complex, Complex)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(3.5, 4);
  value::Value y = value::Value::MakeComplex(3.5, 4);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(String, String)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeString("hello");
  value::Value y = value::Value::MakeString("hello, world");
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(bool, bool)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeBoolean(false);
  REQUIRE(x.ConvertTo(&type::Bool::instance));
  value::Value y = value::Value::MakeBoolean(false);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(complex64, complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  value::Value y = value::Value::MakeComplex(3, 4);
  REQUIRE(y.ConvertTo(&type::Complex64::instance));
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(complex128, complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeComplex(1, 2);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  value::Value y = value::Value::MakeComplex(1, 2);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(float32, float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(1);
  value::Value y = value::Value::MakeFloat(3);
  REQUIRE(y.ConvertTo(&type::Float32::instance));
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(float64, float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeFloat(1);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  value::Value y = value::Value::MakeFloat(1);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(int, int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  value::Value y = value::Value::MakeInteger(3);
  REQUIRE(y.ConvertTo(&type::Int::instance));
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(int8, int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(1);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(int16, int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  value::Value y = value::Value::MakeInteger(3);
  REQUIRE(y.ConvertTo(&type::Int16::instance));
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(int32, int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  value::Value y = value::Value::MakeInteger(1);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(int64, int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  value::Value y = value::Value::MakeInteger(3);
  REQUIRE(y.ConvertTo(&type::Int64::instance));
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(uint, uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  value::Value y = value::Value::MakeInteger(1);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(uint8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  value::Value y = value::Value::MakeInteger(3);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(uint16, uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  value::Value y = value::Value::MakeInteger(1);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(uint32, uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  value::Value y = value::Value::MakeInteger(3);
  REQUIRE(y.ConvertTo(&type::Uint32::instance));
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(uint64, uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  value::Value y = value::Value::MakeInteger(1);
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(uintptr, uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  value::Value y = value::Value::MakeInteger(3);
  REQUIRE(y.ConvertTo(&type::Uintptr::instance));
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(false));
}

TEST_CASE("value::Value::Equal(string, string)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeString("hello");
  REQUIRE(x.ConvertTo(&type::String::instance));
  value::Value y = value::Value::MakeString("hello");
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z == value::Value::MakeBoolean(true));
}

TEST_CASE("value::Value::Equal(int, string)") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  value::Value x = value::Value::MakeInteger(1);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  value::Value y = value::Value::MakeString("hello");
  REQUIRE(y.ConvertTo(&type::String::instance));
  value::Value z = value::Value::Equal(location, &x, &y, &er);
  REQUIRE(z.IsError());
  REQUIRE(er.Count() == 1);
}

TEST_CASE("value::operator==(Unknown, Error)") {
  value::Value x;
  value::Value y = value::Value::MakeError();
  REQUIRE(x != y);
}

TEST_CASE("value::operator==(Unknown, Unknown)") {
  value::Value x;
  value::Value y;
  REQUIRE(x == y);
}

TEST_CASE("value::operator==(int8, uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(3);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  value::Value y = value::Value::MakeInteger(3);
  REQUIRE(y.ConvertTo(&type::Uint8::instance));
  REQUIRE(x != y);
}

TEST_CASE("operator<<(Unknown)") {
  std::stringstream ss;
  value::Value x;
  ss << x;
  REQUIRE(ss.str() == "uninitialized");
}

TEST_CASE("operator<<(Error)") {
  std::stringstream ss;
  value::Value x = value::Value::MakeError();
  ss << x;
  REQUIRE(ss.str() == "error");
}

TEST_CASE("operator<<(Boolean)") {
  {
    std::stringstream ss;
    value::Value x = value::Value::MakeBoolean(true);
    ss << x;
    REQUIRE(ss.str() == "true");
  }
  {
    std::stringstream ss;
    value::Value x = value::Value::MakeBoolean(false);
    ss << x;
    REQUIRE(ss.str() == "false");
  }
}

TEST_CASE("operator<<(Integer)") {
  std::stringstream ss;
  value::Value x = value::Value::MakeInteger(34);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(Rune)") {
  {
    std::stringstream ss;
    value::Value x = value::Value::MakeRune('j');
    ss << x;
    REQUIRE(ss.str() == "'j'");
  }
  {
    std::stringstream ss;
    value::Value x = value::Value::MakeRune(INVALID);
    ss << x;
    REQUIRE(ss.str() == "1114112");
  }
  {
    std::stringstream ss;
    value::Value x = value::Value::MakeRune(SURROGATE_FIRST);
    ss << x;
    REQUIRE(ss.str() == "55296");
  }
}

TEST_CASE("operator<<(Float)") {
  std::stringstream ss;
  value::Value x = value::Value::MakeFloat(34.5);
  ss << x;
  REQUIRE(ss.str() == "34.5");
}

TEST_CASE("operator<<(Complex)") {
  std::stringstream ss;
  value::Value x = value::Value::MakeComplex(1, 2);
  ss << x;
  REQUIRE(ss.str() == "1+2i");
}

TEST_CASE("operator<<(String)") {
  std::stringstream ss;
  value::Value x = value::Value::MakeString("hello");
  ss << x;
  REQUIRE(ss.str() == "\"hello\"");
}

TEST_CASE("operator<<(bool)") {
  {
    std::stringstream ss;
    ErrorReporter er(ss, 1, &abort_handler);
    value::Value x = value::Value::MakeBoolean(true);
    REQUIRE(x.ConvertTo(&type::Bool::instance));
    ss << x;
    REQUIRE(ss.str() == "true");
  }
  {
    std::stringstream ss;
    ErrorReporter er(ss, 1, &abort_handler);
    value::Value x = value::Value::MakeBoolean(false);
    REQUIRE(x.ConvertTo(&type::Bool::instance));
    ss << x;
    REQUIRE(ss.str() == "false");
  }
}

TEST_CASE("operator<<(int8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Int8::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(int16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Int16::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(int32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Int32::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(int64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Int64::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uint8)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uint8::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uint16)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uint16::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uint32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uint32::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uint64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uint64::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(int)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Int::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uint)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uint::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(uintptr)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Uintptr::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(float32)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Float32::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(float64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Float64::instance));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("operator<<(complex64)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Complex64::instance));
  ss << x;
  REQUIRE(ss.str() == "34+0i");
}

TEST_CASE("operator<<(complex128)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeInteger(34);
  REQUIRE(x.ConvertTo(&type::Complex128::instance));
  ss << x;
  REQUIRE(ss.str() == "34+0i");
}

TEST_CASE("operator<<(string)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  value::Value x = value::Value::MakeString("hello");
  REQUIRE(x.ConvertTo(&type::String::instance));
  ss << x;
  REQUIRE(ss.str() == "\"hello\"");
}

}  // namespace test
}  // namespace rcgo
