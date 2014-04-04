/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "Value.hpp"
#include "Type.hpp"
#include "Utf8.hpp"
#include <utility>

using namespace rcgo;
using namespace std::rel_ops;

TEST_CASE ("complex64_t::complex64_t ()") {
  complex64_t c;
  REQUIRE (c.real == 0);
  REQUIRE (c.imag == 0);
}

TEST_CASE ("complex64_t::complex64_t (1,2)") {
  complex64_t c (1, 2);
  REQUIRE (c.real == 1);
  REQUIRE (c.imag == 2);
}

TEST_CASE ("complex64_t::operator+") {
  complex64_t c1 (1, 2);
  complex64_t c2 (3, 4);
  complex64_t c3 = c1 + c2;
  REQUIRE (c3 == complex64_t (4, 6));
}

TEST_CASE ("complex64_t::operator-") {
  complex64_t c1 (1, 2);
  complex64_t c2 (3, 4);
  complex64_t c3 = c1 - c2;
  REQUIRE (c3 == complex64_t (-2, -2));
}

TEST_CASE ("complex64_t::operator*") {
  complex64_t c1 (1, 2);
  complex64_t c2 (3, 4);
  complex64_t c3 = c1 * c2;
  REQUIRE (c3 == complex64_t (-5, 10));
}

TEST_CASE ("complex64_t::operator/") {
  complex64_t c1 (1, 0);
  complex64_t c2 (0, 1);
  complex64_t c3 = c1 / c2;
  REQUIRE (c3 == complex64_t (0, -1));
}

TEST_CASE ("complex64_t::operator+ unary") {
  complex64_t c1 (1, 1);
  complex64_t c2 = +c1;
  REQUIRE (c2 == complex64_t (1, 1));
}

TEST_CASE ("complex64_t::operator- unary") {
  complex64_t c1 (1, 1);
  complex64_t c2 = -c1;
  REQUIRE (c2 == complex64_t (-1, -1));
}

TEST_CASE ("complex64_t::operator==") {
  complex64_t c1 (1, 2);
  complex64_t c2 (3, 4);
  REQUIRE (c1 == c1);
  REQUIRE (c1 != c2);
}

TEST_CASE ("operator<< complex64_t") {
  std::stringstream s1;
  complex64_t c1 (1, 2);
  s1 << c1;
  REQUIRE (s1.str () == "1+2i");
  std::stringstream s2;
  complex64_t c2 (1, -2);
  s2 << c2;
  REQUIRE (s2.str () == "1-2i");
}

TEST_CASE ("complex128_t::complex128_t ()") {
  complex128_t c;
  REQUIRE (c.real == 0);
  REQUIRE (c.imag == 0);
}

TEST_CASE ("complex128_t::complex128_t (1,2)") {
  complex128_t c (1, 2);
  REQUIRE (c.real == 1);
  REQUIRE (c.imag == 2);
}

TEST_CASE ("complex128_t::operator+") {
  complex128_t c1 (1, 2);
  complex128_t c2 (3, 4);
  complex128_t c3 = c1 + c2;
  REQUIRE (c3 == complex128_t (4, 6));
}

TEST_CASE ("complex128_t::operator-") {
  complex128_t c1 (1, 2);
  complex128_t c2 (3, 4);
  complex128_t c3 = c1 - c2;
  REQUIRE (c3 == complex128_t (-2, -2));
}

TEST_CASE ("complex128_t::operator*") {
  complex128_t c1 (1, 2);
  complex128_t c2 (3, 4);
  complex128_t c3 = c1 * c2;
  REQUIRE (c3 == complex128_t (-5, 10));
}

TEST_CASE ("complex128_t::operator/") {
  complex128_t c1 (1, 0);
  complex128_t c2 (0, 1);
  complex128_t c3 = c1 / c2;
  REQUIRE (c3 == complex128_t (0, -1));
}

TEST_CASE ("complex128_t::operator+ unary") {
  complex128_t c1 (1, 2);
  complex128_t c2 = +c1;
  REQUIRE (c2 == complex128_t (1, 2));
}

TEST_CASE ("complex128_t::operator- unary") {
  complex128_t c1 (1, 2);
  complex128_t c2 = -c1;
  REQUIRE (c2 == complex128_t (-1, -2));
}

TEST_CASE ("complex128_t::operator==") {
  complex128_t c1 (1, 2);
  complex128_t c2 (3, 4);
  REQUIRE (c1 == c1);
  REQUIRE (c1 != c2);
}

TEST_CASE ("operator<< complex128_t") {
  std::stringstream s1;
  complex128_t c1 (1, 2);
  s1 << c1;
  REQUIRE (s1.str () == "1+2i");
  std::stringstream s2;
  complex128_t c2 (1, -2);
  s2 << c2;
  REQUIRE (s2.str () == "1-2i");
}

TEST_CASE ("complex_t::complex_t ()") {
  complex_t c;
  REQUIRE (c.real == 0);
  REQUIRE (c.imag == 0);
}

TEST_CASE ("complex_t::complex_t (1,2)") {
  complex_t c (1, 2);
  REQUIRE (c.real == 1);
  REQUIRE (c.imag == 2);
}

TEST_CASE ("complex_t::operator+") {
  complex_t c1 (1, 2);
  complex_t c2 (3, 4);
  complex_t c3 = c1 + c2;
  REQUIRE (c3 == complex_t (4, 6));
}

TEST_CASE ("complex_t::operator-") {
  complex_t c1 (1, 2);
  complex_t c2 (3, 4);
  complex_t c3 = c1 - c2;
  REQUIRE (c3 == complex_t (-2, -2));
}

TEST_CASE ("complex_t::operator*") {
  complex_t c1 (1, 2);
  complex_t c2 (3, 4);
  complex_t c3 = c1 * c2;
  REQUIRE (c3 == complex_t (-5, 10));
}

TEST_CASE ("complex_t::operator/") {
  complex_t c1 (1, 0);
  complex_t c2 (0, 1);
  complex_t c3 = c1 / c2;
  REQUIRE (c3 == complex_t (0, -1));
}

TEST_CASE ("complex_t::operator+ unary") {
  complex_t c1 (1, 2);
  complex_t c2 = +c1;
  REQUIRE (c2 == complex_t (1, 2));
}

TEST_CASE ("complex_t::operator- unary") {
  complex_t c1 (1, 2);
  complex_t c2 = -c1;
  REQUIRE (c2 == complex_t (-1, -2));
}

TEST_CASE ("complex_t::operator==") {
  complex_t c1 (1, 2);
  complex_t c2 (3, 4);
  REQUIRE (c1 == c1);
  REQUIRE (c1 != c2);
}

TEST_CASE ("operator<< complex_t") {
  std::stringstream s1;
  complex_t c1 (1, 2);
  s1 << c1;
  REQUIRE (s1.str () == "1+2i");
  std::stringstream s2;
  complex_t c2 (1, -2);
  s2 << c2;
  REQUIRE (s2.str () == "1-2i");
}

TEST_CASE ("Value::Value ()") {
  Value v;
  REQUIRE (v.kind == Value::Unknown);
  REQUIRE (v.is_unknown ());
}

TEST_CASE ("Value::make_error ()") {
  Value v = Value::make_error ();
  REQUIRE (v.kind == Value::Error);
  REQUIRE (v.is_error ());
}

TEST_CASE ("Value::make_boolean ()") {
  Value v = Value::make_boolean (true);
  REQUIRE (v.kind == Value::Boolean);
  REQUIRE (v.Boolean_value == true);
}

TEST_CASE ("Value::make_integer ()") {
  Value v = Value::make_integer (3);
  REQUIRE (v.kind == Value::Integer);
  REQUIRE (v.is_arithmetic ());
  REQUIRE (v.Integer_value == 3);
}

TEST_CASE ("Value::make_rune ()") {
  Value v = Value::make_rune (123);
  REQUIRE (v.kind == Value::Rune);
  REQUIRE (v.is_arithmetic ());
  REQUIRE (v.Rune_value == 123);
}

TEST_CASE ("Value::make_float ()") {
  Value v = Value::make_float (3.14);
  REQUIRE (v.kind == Value::Float);
  REQUIRE (v.is_arithmetic ());
  REQUIRE (v.Float_value == 3.14);
}

TEST_CASE ("Value::make_complex ()") {
  Value v = Value::make_complex (1, 2);
  REQUIRE (v.kind == Value::Complex);
  REQUIRE (v.is_arithmetic ());
  REQUIRE (v.Complex_value == complex_t (1, 2));
}

TEST_CASE ("Value::make_string ()") {
  Value v = Value::make_string ("hello");
  REQUIRE (v.kind == Value::String);
  REQUIRE (v.is_string ());
  REQUIRE (v.String_value == "hello");
}

TEST_CASE ("Value::convert (Error -> bool)") {
  Value x = Value::make_error ();
  Value y = x.convert (&BoolType::instance);
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::convert (Boolean -> bool)") {
  Value x = Value::make_boolean (true);
  Value y = x.convert (&BoolType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &BoolType::instance);
  REQUIRE (y.bool_value == true);
}

TEST_CASE ("Value::convert (Integer -> complex64)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Complex64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.is_arithmetic ());
  REQUIRE (y.type == &Complex64Type::instance);
  REQUIRE (y.complex64_value == complex64_t (34, 0));
}

TEST_CASE ("Value::convert (Integer -> complex128)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Complex128Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Complex128Type::instance);
  REQUIRE (y.complex128_value == complex128_t (34, 0));
}

TEST_CASE ("Value::convert (Integer -> float32)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Float32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Float32Type::instance);
  REQUIRE (y.float32_value == 34);
}

TEST_CASE ("Value::convert (Integer -> float64)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Float64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Float64Type::instance);
  REQUIRE (y.float64_value == 34);
}

TEST_CASE ("Value::convert (Integer -> int)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&IntType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &IntType::instance);
  REQUIRE (y.int_value == 34);
}

TEST_CASE ("Value::convert (Integer -> int8)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Int8Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int8Type::instance);
  REQUIRE (y.int8_value == 34);
}

TEST_CASE ("Value::convert (Integer -> int16)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Int16Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int16Type::instance);
  REQUIRE (y.int16_value == 34);
}

TEST_CASE ("Value::convert (Integer -> int32)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Int32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int32Type::instance);
  REQUIRE (y.int32_value == 34);
}

TEST_CASE ("Value::convert (Integer -> int64)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Int64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int64Type::instance);
  REQUIRE (y.int64_value == 34);
}

TEST_CASE ("Value::convert (Integer -> uint)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&UintType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &UintType::instance);
  REQUIRE (y.uint_value == 34);
}

TEST_CASE ("Value::convert (Integer -> uint8)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Uint8Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint8Type::instance);
  REQUIRE (y.uint8_value == 34);
}

TEST_CASE ("Value::convert (Integer -> uint16)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Uint16Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint16Type::instance);
  REQUIRE (y.uint16_value == 34);
}

TEST_CASE ("Value::convert (Integer -> uint32)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Uint32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint32Type::instance);
  REQUIRE (y.uint32_value == 34);
}

TEST_CASE ("Value::convert (Integer -> uint64)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&Uint64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint64Type::instance);
  REQUIRE (y.uint64_value == 34);
}

TEST_CASE ("Value::convert (Integer -> uintptr)") {
  Value x = Value::make_integer (34);
  Value y = x.convert (&UintptrType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &UintptrType::instance);
  REQUIRE (y.uintptr_value == 34);
}

TEST_CASE ("Value::convert (Float 0 -> complex64)") {
  Value x = Value::make_float (0);
  Value y = x.convert (&Complex64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Complex64Type::instance);
  REQUIRE (y.complex64_value == complex64_t (0, 0));
}

TEST_CASE ("Value::convert (Float -> complex64)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Complex64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Complex64Type::instance);
  REQUIRE (y.complex64_value == complex64_t (34, 0));
}

TEST_CASE ("Value::convert (Float -> complex128)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Complex128Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Complex128Type::instance);
  REQUIRE (y.complex128_value == complex128_t (34, 0));
}

TEST_CASE ("Value::convert (Float -> float32)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Float32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Float32Type::instance);
  REQUIRE (y.float32_value == 34);
}

TEST_CASE ("Value::convert (Float -> float64)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Float64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Float64Type::instance);
  REQUIRE (y.float64_value == 34);
}

TEST_CASE ("Value::convert (Float -> int)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&IntType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &IntType::instance);
  REQUIRE (y.int_value == 34);
}

TEST_CASE ("Value::convert (Float -> int8)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Int8Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int8Type::instance);
  REQUIRE (y.int8_value == 34);
}

TEST_CASE ("Value::convert (Float -> int16)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Int16Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int16Type::instance);
  REQUIRE (y.int16_value == 34);
}

TEST_CASE ("Value::convert (Float -> int32)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Int32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int32Type::instance);
  REQUIRE (y.int32_value == 34);
}

TEST_CASE ("Value::convert (Float -> int64)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Int64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int64Type::instance);
  REQUIRE (y.int64_value == 34);
}

TEST_CASE ("Value::convert (Float -> uint)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&UintType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &UintType::instance);
  REQUIRE (y.uint_value == 34);
}

TEST_CASE ("Value::convert (Float -> uint8)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Uint8Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint8Type::instance);
  REQUIRE (y.uint8_value == 34);
}

TEST_CASE ("Value::convert (Float -> uint16)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Uint16Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint16Type::instance);
  REQUIRE (y.uint16_value == 34);
}

TEST_CASE ("Value::convert (Float -> uint32)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Uint32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint32Type::instance);
  REQUIRE (y.uint32_value == 34);
}

TEST_CASE ("Value::convert (Float -> uint64)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&Uint64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint64Type::instance);
  REQUIRE (y.uint64_value == 34);
}

TEST_CASE ("Value::convert (Float -> uintptr)") {
  Value x = Value::make_float (34);
  Value y = x.convert (&UintptrType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &UintptrType::instance);
  REQUIRE (y.uintptr_value == 34);
}

TEST_CASE ("Value::convert (Complex -> complex64)") {
  Value x = Value::make_complex (1, 2);
  Value y = x.convert (&Complex64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Complex64Type::instance);
  REQUIRE (y.complex64_value == complex64_t (1, 2));
}

TEST_CASE ("Value::convert (Complex -> complex128)") {
  Value x = Value::make_complex (1, 2);
  Value y = x.convert (&Complex128Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Complex128Type::instance);
  REQUIRE (y.complex128_value == complex128_t (1, 2));
}

TEST_CASE ("Value::convert (Complex -> float32)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&Float32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Float32Type::instance);
  REQUIRE (y.float32_value == 34);
}

TEST_CASE ("Value::convert (Complex -> float64)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&Float64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Float64Type::instance);
  REQUIRE (y.float64_value == 34);
}

TEST_CASE ("Value::convert (Complex -> int)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&IntType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &IntType::instance);
  REQUIRE (y.int_value == 34);
}

TEST_CASE ("Value::convert (Complex -> int8)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&Int8Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int8Type::instance);
  REQUIRE (y.int8_value == 34);
}

TEST_CASE ("Value::convert (Complex -> int16)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&Int16Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int16Type::instance);
  REQUIRE (y.int16_value == 34);
}

TEST_CASE ("Value::convert (Complex -> int32)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&Int32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int32Type::instance);
  REQUIRE (y.int32_value == 34);
}

TEST_CASE ("Value::convert (Complex -> int64)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&Int64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int64Type::instance);
  REQUIRE (y.int64_value == 34);
}

TEST_CASE ("Value::convert (Complex -> uint)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&UintType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &UintType::instance);
  REQUIRE (y.uint_value == 34);
}

TEST_CASE ("Value::convert (Complex -> uint8)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&Uint8Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint8Type::instance);
  REQUIRE (y.uint8_value == 34);
}

TEST_CASE ("Value::convert (Complex -> uint16)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&Uint16Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint16Type::instance);
  REQUIRE (y.uint16_value == 34);
}

TEST_CASE ("Value::convert (Complex -> uint32)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&Uint32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint32Type::instance);
  REQUIRE (y.uint32_value == 34);
}

TEST_CASE ("Value::convert (Complex -> uint64)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&Uint64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint64Type::instance);
  REQUIRE (y.uint64_value == 34);
}

TEST_CASE ("Value::convert (Complex -> uintptr)") {
  Value x = Value::make_complex (34, 0);
  Value y = x.convert (&UintptrType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &UintptrType::instance);
  REQUIRE (y.uintptr_value == 34);
}

TEST_CASE ("Value::convert (Rune -> complex64)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Complex64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Complex64Type::instance);
  REQUIRE (y.complex64_value == complex64_t (34, 0));
}

TEST_CASE ("Value::convert (Rune -> complex128)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Complex128Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Complex128Type::instance);
  REQUIRE (y.complex128_value == complex128_t (34, 0));
}

TEST_CASE ("Value::convert (Rune -> float32)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Float32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Float32Type::instance);
  REQUIRE (y.float32_value == 34);
}

TEST_CASE ("Value::convert (Rune -> float64)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Float64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Float64Type::instance);
  REQUIRE (y.float64_value == 34);
}

TEST_CASE ("Value::convert (Rune -> int)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&IntType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &IntType::instance);
  REQUIRE (y.int_value == 34);
}

TEST_CASE ("Value::convert (Rune -> int8)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Int8Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int8Type::instance);
  REQUIRE (y.int8_value == 34);
}

TEST_CASE ("Value::convert (Rune -> int16)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Int16Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int16Type::instance);
  REQUIRE (y.int16_value == 34);
}

TEST_CASE ("Value::convert (Rune -> int32)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Int32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int32Type::instance);
  REQUIRE (y.int32_value == 34);
}

TEST_CASE ("Value::convert (Rune -> int64)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Int64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Int64Type::instance);
  REQUIRE (y.int64_value == 34);
}

TEST_CASE ("Value::convert (Rune -> uint)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&UintType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &UintType::instance);
  REQUIRE (y.uint_value == 34);
}

TEST_CASE ("Value::convert (Rune -> uint8)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Uint8Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint8Type::instance);
  REQUIRE (y.uint8_value == 34);
}

TEST_CASE ("Value::convert (Rune -> uint16)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Uint16Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint16Type::instance);
  REQUIRE (y.uint16_value == 34);
}

TEST_CASE ("Value::convert (Rune -> uint32)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Uint32Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint32Type::instance);
  REQUIRE (y.uint32_value == 34);
}

TEST_CASE ("Value::convert (Rune -> uint64)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&Uint64Type::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &Uint64Type::instance);
  REQUIRE (y.uint64_value == 34);
}

TEST_CASE ("Value::convert (Rune -> uintptr)") {
  Value x = Value::make_rune (34);
  Value y = x.convert (&UintptrType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &UintptrType::instance);
  REQUIRE (y.uintptr_value == 34);
}

TEST_CASE ("Value::convert (String -> string)") {
  Value x = Value::make_string ("hello");
  Value y = x.convert (&StringType::instance);
  REQUIRE (y.kind == Value::Typed);
  REQUIRE (y.type == &StringType::instance);
  REQUIRE (y.string_value == "hello");
}

TEST_CASE ("Value::promote (Rune -> Rune)") {
  Value x = Value::make_rune (34);
  Value y = Value::make_rune (56);
  Value z = x.promote (y);
  REQUIRE (z == Value::make_rune (34));
}

TEST_CASE ("Value::promote (Rune -> int)") {
  Value x = Value::make_rune (34);
  Value y = Value::make_integer (1).convert (&IntType::instance);
  Value z = x.promote (y);
  REQUIRE (z == Value::make_integer (34).convert (&IntType::instance));
}

TEST_CASE ("Value::promote (Integer -> Rune)") {
  Value x = Value::make_integer (34);
  Value y = Value::make_rune (56);
  Value z = x.promote (y);
  REQUIRE (z == Value::make_rune (34));
}

TEST_CASE ("Value::promote (Rune -> Float)") {
  Value x = Value::make_rune (34);
  Value y = Value::make_float (56);
  Value z = x.promote (y);
  REQUIRE (z == Value::make_float (34));
}

TEST_CASE ("Value::promote (Float -> Complex)") {
  Value x = Value::make_float (34);
  Value y = Value::make_complex (56, 78);
  Value z = x.promote (y);
  REQUIRE (z == Value::make_complex (34, 0));
}

TEST_CASE ("Value::promote (Boolean -> Integer)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_integer (56);
  Value z = x.promote (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::promote (String -> Integer)") {
  Value x = Value::make_string ("hello");
  Value y = Value::make_integer (56);
  Value z = x.promote (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::to_integer (int8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (int16)") {
  Value x = Value::make_integer (3).convert (&Int16Type::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (int32)") {
  Value x = Value::make_integer (3).convert (&Int32Type::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (int64)") {
  Value x = Value::make_integer (3).convert (&Int64Type::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (uint8)") {
  Value x = Value::make_integer (3).convert (&Uint8Type::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (uint16)") {
  Value x = Value::make_integer (3).convert (&Uint16Type::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (uint32)") {
  Value x = Value::make_integer (3).convert (&Uint32Type::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (uint64)") {
  Value x = Value::make_integer (3).convert (&Uint64Type::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (int)") {
  Value x = Value::make_integer (3).convert (&IntType::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (uint)") {
  Value x = Value::make_integer (3).convert (&UintType::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (uintptr)") {
  Value x = Value::make_integer (3).convert (&UintptrType::instance);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (Integer)") {
  Value x = Value::make_integer (3);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (Rune)") {
  Value x = Value::make_rune (3);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (Float yes)") {
  Value x = Value::make_float (3);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (Float no)") {
  Value x = Value::make_float (3.5);
  Value y = x.to_integer ();
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::to_integer (Complex yes)") {
  Value x = Value::make_complex (3, 0);
  Value y = x.to_integer ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::to_integer (Complex no)") {
  Value x = Value::make_complex (3, 1);
  Value y = x.to_integer ();
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::to_integer (bool)") {
  Value x = Value::make_boolean (true).convert (&BoolType::instance);
  Value y = x.to_integer ();
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::posate (Error)") {
  Value x = Value::make_error ();
  Value y = x.posate ();
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::posate (Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = x.posate ();
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::posate (Integer)") {
  Value x = Value::make_integer (3);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3));
}

TEST_CASE ("Value::posate (Rune)") {
  Value x = Value::make_rune (3);
  Value y = x.posate ();
  REQUIRE (y == Value::make_rune (3));
}

TEST_CASE ("Value::posate (Float)") {
  Value x = Value::make_float (1.5);
  Value y = x.posate ();
  REQUIRE (y == Value::make_float (1.5));
}

TEST_CASE ("Value::posate (Complex)") {
  Value x = Value::make_complex (1, 2);
  Value y = x.posate ();
  REQUIRE (y == Value::make_complex (1, 2));
}

TEST_CASE ("Value::posate (int8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&Int8Type::instance));
}

TEST_CASE ("Value::posate (int16)") {
  Value x = Value::make_integer (3).convert (&Int16Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&Int16Type::instance));
}

TEST_CASE ("Value::posate (int32)") {
  Value x = Value::make_integer (3).convert (&Int32Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&Int32Type::instance));
}

TEST_CASE ("Value::posate (int64)") {
  Value x = Value::make_integer (3).convert (&Int64Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&Int64Type::instance));
}

TEST_CASE ("Value::posate (uint8)") {
  Value x = Value::make_integer (3).convert (&Uint8Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::posate (uint16)") {
  Value x = Value::make_integer (3).convert (&Uint16Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::posate (uint32)") {
  Value x = Value::make_integer (3).convert (&Uint32Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::posate (uint64)") {
  Value x = Value::make_integer (3).convert (&Uint64Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::posate (int)") {
  Value x = Value::make_integer (3).convert (&IntType::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&IntType::instance));
}

TEST_CASE ("Value::posate (uint)") {
  Value x = Value::make_integer (3).convert (&UintType::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&UintType::instance));
}

TEST_CASE ("Value::posate (uintptr)") {
  Value x = Value::make_integer (3).convert (&UintptrType::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_integer (3).convert (&UintptrType::instance));
}

TEST_CASE ("Value::posate (float32)") {
  Value x = Value::make_float (3).convert (&Float32Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_float (3).convert (&Float32Type::instance));
}

TEST_CASE ("Value::posate (float64)") {
  Value x = Value::make_float (3).convert (&Float64Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_float (3).convert (&Float64Type::instance));
}

TEST_CASE ("Value::posate (complex64)") {
  Value x = Value::make_complex (1, 2).convert (&Complex64Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_complex (1, 2).convert (&Complex64Type::instance));
}

TEST_CASE ("Value::posate (complex128)") {
  Value x = Value::make_complex (1, 2).convert (&Complex128Type::instance);
  Value y = x.posate ();
  REQUIRE (y == Value::make_complex (1, 2).convert (&Complex128Type::instance));
}

TEST_CASE ("Value::negate (Error)") {
  Value x = Value::make_error ();
  Value y = x.negate ();
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::negate (Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = x.negate ();
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::negate (Integer)") {
  Value x = Value::make_integer (3);
  Value y = x.negate ();
  REQUIRE (y == Value::make_integer (-3));
}

TEST_CASE ("Value::negate (Rune)") {
  Value x = Value::make_rune (3);
  Value y = x.negate ();
  REQUIRE (y == Value::make_rune (-3));
}

TEST_CASE ("Value::negate (Float)") {
  Value x = Value::make_float (1.5);
  Value y = x.negate ();
  REQUIRE (y == Value::make_float (-1.5));
}

TEST_CASE ("Value::negate (Complex)") {
  Value x = Value::make_complex (1, 2);
  Value y = x.negate ();
  REQUIRE (y == Value::make_complex (-1, -2));
}

TEST_CASE ("Value::negate (int8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_integer (-3).convert (&Int8Type::instance));
}

TEST_CASE ("Value::negate (int16)") {
  Value x = Value::make_integer (3).convert (&Int16Type::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_integer (-3).convert (&Int16Type::instance));
}

TEST_CASE ("Value::negate (int32)") {
  Value x = Value::make_integer (3).convert (&Int32Type::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_integer (-3).convert (&Int32Type::instance));
}

TEST_CASE ("Value::negate (int64)") {
  Value x = Value::make_integer (3).convert (&Int64Type::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_integer (-3).convert (&Int64Type::instance));
}

TEST_CASE ("Value::negate (uint8)") {
  Value x = Value::make_integer (3).convert (&Uint8Type::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_error ());
}

TEST_CASE ("Value::negate (int)") {
  Value x = Value::make_integer (3).convert (&IntType::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_integer (-3).convert (&IntType::instance));
}

TEST_CASE ("Value::negate (uintptr)") {
  Value x = Value::make_integer (3).convert (&UintptrType::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_integer (-3).convert (&UintptrType::instance));
}

TEST_CASE ("Value::negate (float32)") {
  Value x = Value::make_float (3).convert (&Float32Type::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_float (-3).convert (&Float32Type::instance));
}

TEST_CASE ("Value::negate (float64)") {
  Value x = Value::make_float (3).convert (&Float64Type::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_float (-3).convert (&Float64Type::instance));
}

TEST_CASE ("Value::negate (complex64)") {
  Value x = Value::make_complex (1, 2).convert (&Complex64Type::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_complex (-1, -2).convert (&Complex64Type::instance));
}

TEST_CASE ("Value::negate (complex128)") {
  Value x = Value::make_complex (1, 2).convert (&Complex128Type::instance);
  Value y = x.negate ();
  REQUIRE (y == Value::make_complex (-1, -2).convert (&Complex128Type::instance));
}

TEST_CASE ("Value::logic_not (Error)") {
  Value x = Value::make_error ();
  Value y = x.logic_not ();
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::logic_not (Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = x.logic_not ();
  REQUIRE (y == Value::make_boolean (false));
}

TEST_CASE ("Value::logic_not (Integer)") {
  Value x = Value::make_integer (3);
  Value y = x.logic_not ();
  REQUIRE (y == Value::make_error ());
}

TEST_CASE ("Value::logic_not (bool)") {
  Value x = Value::make_boolean (true).convert (&BoolType::instance);
  Value y = x.logic_not ();
  REQUIRE (y == Value::make_boolean (false).convert (&BoolType::instance));
}

TEST_CASE ("Value::bit_not (Error)") {
  Value x = Value::make_error ();
  Value y = x.bit_not ();
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::bit_not (Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = x.bit_not ();
  REQUIRE (y.is_error ());
}

TEST_CASE ("Value::bit_not (Integer)") {
  Value x = Value::make_integer (3);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (-4));
}

TEST_CASE ("Value::bit_not (Rune)") {
  Value x = Value::make_rune (3);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_rune (-4));
}

TEST_CASE ("Value::bit_not (int8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (-4).convert (&Int8Type::instance));
}

TEST_CASE ("Value::bit_not (int16)") {
  Value x = Value::make_integer (3).convert (&Int16Type::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (-4).convert (&Int16Type::instance));
}

TEST_CASE ("Value::bit_not (int32)") {
  Value x = Value::make_integer (3).convert (&Int32Type::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (-4).convert (&Int32Type::instance));
}

TEST_CASE ("Value::bit_not (int64)") {
  Value x = Value::make_integer (3).convert (&Int64Type::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (-4).convert (&Int64Type::instance));
}

TEST_CASE ("Value::bit_not (uint8)") {
  Value x = Value::make_integer (3).convert (&Uint8Type::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (252).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::bit_not (uint16)") {
  Value x = Value::make_integer (3).convert (&Uint16Type::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (65532).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::bit_not (uint32)") {
  Value x = Value::make_integer (3).convert (&Uint32Type::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (4294967292).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::bit_not (uint64)") {
  Value x = Value::make_integer (3).convert (&Uint64Type::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (18446744073709551612UL).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::bit_not (int)") {
  Value x = Value::make_integer (3).convert (&IntType::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (-4).convert (&IntType::instance));
}

TEST_CASE ("Value::bit_not (uint)") {
  Value x = Value::make_integer (3).convert (&UintType::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (4294967292).convert (&UintType::instance));
}

TEST_CASE ("Value::bit_not (uintptr)") {
  Value x = Value::make_integer (3).convert (&UintptrType::instance);
  Value y = x.bit_not ();
  REQUIRE (y == Value::make_integer (4294967292).convert (&UintptrType::instance));
}

TEST_CASE ("Value::multiply (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.multiply (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::multiply (Boolean, Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (true);
  Value z = x.multiply (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::multiply (Integer, Integer)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12));
}

TEST_CASE ("Value::multiply (Rune, Integer)") {
  Value x = Value::make_rune (3);
  Value y = Value::make_integer (4);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_rune (12));
}

TEST_CASE ("Value::multiply (Integer, Rune)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_rune (4);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_rune (12));
}

TEST_CASE ("Value::multiply (Float, Float)") {
  Value x = Value::make_float (1.5);
  Value y = Value::make_float (2.25);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_float (3.375));
}

TEST_CASE ("Value::multiply (Complex, Complex)") {
  Value x = Value::make_complex (1, 2);
  Value y = Value::make_complex (3, 4);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_complex (-5, 10));
}

TEST_CASE ("Value::multiply (int8, int8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4).convert (&Int8Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&Int8Type::instance));
}

TEST_CASE ("Value::multiply (int16, int16)") {
  Value x = Value::make_integer (3).convert (&Int16Type::instance);
  Value y = Value::make_integer (4).convert (&Int16Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&Int16Type::instance));
}

TEST_CASE ("Value::multiply (int32, int32)") {
  Value x = Value::make_integer (3).convert (&Int32Type::instance);
  Value y = Value::make_integer (4).convert (&Int32Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&Int32Type::instance));
}

TEST_CASE ("Value::multiply (int64, int64)") {
  Value x = Value::make_integer (3).convert (&Int64Type::instance);
  Value y = Value::make_integer (4).convert (&Int64Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&Int64Type::instance));
}

TEST_CASE ("Value::multiply (uint8, uint8)") {
  Value x = Value::make_integer (3).convert (&Uint8Type::instance);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::multiply (uint16, uint16)") {
  Value x = Value::make_integer (3).convert (&Uint16Type::instance);
  Value y = Value::make_integer (4).convert (&Uint16Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::multiply (uint32, uint32)") {
  Value x = Value::make_integer (3).convert (&Uint32Type::instance);
  Value y = Value::make_integer (4).convert (&Uint32Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::multiply (uint64, uint64)") {
  Value x = Value::make_integer (3).convert (&Uint64Type::instance);
  Value y = Value::make_integer (4).convert (&Uint64Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::multiply (int, int)") {
  Value x = Value::make_integer (3).convert (&IntType::instance);
  Value y = Value::make_integer (4).convert (&IntType::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&IntType::instance));
}

TEST_CASE ("Value::multiply (uint, uint)") {
  Value x = Value::make_integer (3).convert (&UintType::instance);
  Value y = Value::make_integer (4).convert (&UintType::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&UintType::instance));
}

TEST_CASE ("Value::multiply (uintptr, uintptr)") {
  Value x = Value::make_integer (3).convert (&UintptrType::instance);
  Value y = Value::make_integer (4).convert (&UintptrType::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_integer (12).convert (&UintptrType::instance));
}

TEST_CASE ("Value::multiply (float32, float32)") {
  Value x = Value::make_float (3).convert (&Float32Type::instance);
  Value y = Value::make_float (4).convert (&Float32Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_float (12).convert (&Float32Type::instance));
}

TEST_CASE ("Value::multiply (float64, float64)") {
  Value x = Value::make_float (3).convert (&Float64Type::instance);
  Value y = Value::make_float (4).convert (&Float64Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_float (12).convert (&Float64Type::instance));
}

TEST_CASE ("Value::multiply (complex64, complex64)") {
  Value x = Value::make_complex (1, 2).convert (&Complex64Type::instance);
  Value y = Value::make_complex (3, 4).convert (&Complex64Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_complex (-5, 10).convert (&Complex64Type::instance));
}

TEST_CASE ("Value::multiply (complex128, complex128)") {
  Value x = Value::make_complex (1, 2).convert (&Complex128Type::instance);
  Value y = Value::make_complex (3, 4).convert (&Complex128Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z == Value::make_complex (-5, 10).convert (&Complex128Type::instance));
}

TEST_CASE ("Value::multiply (int8, uint8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.multiply (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::divide (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.divide (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::divide (Boolean, Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (true);
  Value z = x.divide (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::divide (Integer, Integer)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0));
}

TEST_CASE ("Value::divide (Rune, Integer)") {
  Value x = Value::make_rune (3);
  Value y = Value::make_integer (4);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_rune (0));
}

TEST_CASE ("Value::divide (Integer, Rune)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_rune (4);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_rune (0));
}

TEST_CASE ("Value::divide (Float, Float)") {
  Value x = Value::make_float (1);
  Value y = Value::make_float (2);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_float (.5));
}

TEST_CASE ("Value::divide (Complex, Complex)") {
  Value x = Value::make_complex (1, 0);
  Value y = Value::make_complex (0, 1);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_complex (0, -1));
}

TEST_CASE ("Value::divide (int8, int8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4).convert (&Int8Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&Int8Type::instance));
}

TEST_CASE ("Value::divide (int16, int16)") {
  Value x = Value::make_integer (3).convert (&Int16Type::instance);
  Value y = Value::make_integer (4).convert (&Int16Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&Int16Type::instance));
}

TEST_CASE ("Value::divide (int32, int32)") {
  Value x = Value::make_integer (3).convert (&Int32Type::instance);
  Value y = Value::make_integer (4).convert (&Int32Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&Int32Type::instance));
}

TEST_CASE ("Value::divide (int64, int64)") {
  Value x = Value::make_integer (3).convert (&Int64Type::instance);
  Value y = Value::make_integer (4).convert (&Int64Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&Int64Type::instance));
}

TEST_CASE ("Value::divide (uint8, uint8)") {
  Value x = Value::make_integer (3).convert (&Uint8Type::instance);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::divide (uint16, uint16)") {
  Value x = Value::make_integer (3).convert (&Uint16Type::instance);
  Value y = Value::make_integer (4).convert (&Uint16Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::divide (uint32, uint32)") {
  Value x = Value::make_integer (3).convert (&Uint32Type::instance);
  Value y = Value::make_integer (4).convert (&Uint32Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::divide (uint64, uint64)") {
  Value x = Value::make_integer (3).convert (&Uint64Type::instance);
  Value y = Value::make_integer (4).convert (&Uint64Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::divide (int, int)") {
  Value x = Value::make_integer (3).convert (&IntType::instance);
  Value y = Value::make_integer (4).convert (&IntType::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&IntType::instance));
}

TEST_CASE ("Value::divide (uint, uint)") {
  Value x = Value::make_integer (3).convert (&UintType::instance);
  Value y = Value::make_integer (4).convert (&UintType::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&UintType::instance));
}

TEST_CASE ("Value::divide (uintptr, uintptr)") {
  Value x = Value::make_integer (3).convert (&UintptrType::instance);
  Value y = Value::make_integer (4).convert (&UintptrType::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_integer (0).convert (&UintptrType::instance));
}

TEST_CASE ("Value::divide (float32, float32)") {
  Value x = Value::make_float (3).convert (&Float32Type::instance);
  Value y = Value::make_float (4).convert (&Float32Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_float (.75).convert (&Float32Type::instance));
}

TEST_CASE ("Value::divide (float64, float64)") {
  Value x = Value::make_float (3).convert (&Float64Type::instance);
  Value y = Value::make_float (4).convert (&Float64Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_float (.75).convert (&Float64Type::instance));
}

TEST_CASE ("Value::divide (complex64, complex64)") {
  Value x = Value::make_complex (1, 0).convert (&Complex64Type::instance);
  Value y = Value::make_complex (0, -1).convert (&Complex64Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_complex (0, 1).convert (&Complex64Type::instance));
}

TEST_CASE ("Value::divide (complex128, complex128)") {
  Value x = Value::make_complex (1, 0).convert (&Complex128Type::instance);
  Value y = Value::make_complex (0, -1).convert (&Complex128Type::instance);
  Value z = x.divide (y);
  REQUIRE (z == Value::make_complex (0, 1).convert (&Complex128Type::instance));
}

TEST_CASE ("Value::divide (int8, uint8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.divide (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::divide (Integer, 0)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (0);
  Value z = x.divide (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::modulo (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.modulo (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::modulo (Boolean, Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (true);
  Value z = x.modulo (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::modulo (Integer, Integer)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::modulo (Rune, Integer)") {
  Value x = Value::make_rune (3);
  Value y = Value::make_integer (4);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_rune (3));
}

TEST_CASE ("Value::modulo (Integer, Rune)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_rune (4);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_rune (3));
}

TEST_CASE ("Value::modulo (int8, int8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4).convert (&Int8Type::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&Int8Type::instance));
}

TEST_CASE ("Value::modulo (int16, int16)") {
  Value x = Value::make_integer (3).convert (&Int16Type::instance);
  Value y = Value::make_integer (4).convert (&Int16Type::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&Int16Type::instance));
}

TEST_CASE ("Value::modulo (int32, int32)") {
  Value x = Value::make_integer (3).convert (&Int32Type::instance);
  Value y = Value::make_integer (4).convert (&Int32Type::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&Int32Type::instance));
}

TEST_CASE ("Value::modulo (int64, int64)") {
  Value x = Value::make_integer (3).convert (&Int64Type::instance);
  Value y = Value::make_integer (4).convert (&Int64Type::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&Int64Type::instance));
}

TEST_CASE ("Value::modulo (uint8, uint8)") {
  Value x = Value::make_integer (3).convert (&Uint8Type::instance);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::modulo (uint16, uint16)") {
  Value x = Value::make_integer (3).convert (&Uint16Type::instance);
  Value y = Value::make_integer (4).convert (&Uint16Type::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::modulo (uint32, uint32)") {
  Value x = Value::make_integer (3).convert (&Uint32Type::instance);
  Value y = Value::make_integer (4).convert (&Uint32Type::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::modulo (uint64, uint64)") {
  Value x = Value::make_integer (3).convert (&Uint64Type::instance);
  Value y = Value::make_integer (4).convert (&Uint64Type::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::modulo (int, int)") {
  Value x = Value::make_integer (3).convert (&IntType::instance);
  Value y = Value::make_integer (4).convert (&IntType::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&IntType::instance));
}

TEST_CASE ("Value::modulo (uint, uint)") {
  Value x = Value::make_integer (3).convert (&UintType::instance);
  Value y = Value::make_integer (4).convert (&UintType::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&UintType::instance));
}

TEST_CASE ("Value::modulo (uintptr, uintptr)") {
  Value x = Value::make_integer (3).convert (&UintptrType::instance);
  Value y = Value::make_integer (4).convert (&UintptrType::instance);
  Value z = x.modulo (y);
  REQUIRE (z == Value::make_integer (3).convert (&UintptrType::instance));
}

TEST_CASE ("Value::modulo (int8, uint8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.modulo (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::modulo (Integer, 0)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (0);
  Value z = x.modulo (y);
  REQUIRE (z.is_error ());
}



TEST_CASE ("Value::left_shift (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.left_shift (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::left_shift (Boolean, Integer)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::left_shift (Integer, Boolean)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_boolean (true);
  Value z = x.left_shift (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::left_shift (Integer, Integer)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Integer, Rune)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_rune (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Integer, Float)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_float (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Integer, Complex)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_complex (4, 0);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Integer, uint8)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Integer, uint16)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4).convert (&Uint16Type::instance);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Integer, uint32)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4).convert (&Uint32Type::instance);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Integer, uint64)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4).convert (&Uint64Type::instance);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Integer, uint)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4).convert (&UintType::instance);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Integer, uintptr)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4).convert (&UintptrType::instance);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (int8, Integer)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&Int8Type::instance));
}

TEST_CASE ("Value::left_shift (int16, Integer)") {
  Value x = Value::make_integer (3).convert (&Int16Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&Int16Type::instance));
}

TEST_CASE ("Value::left_shift (int32, Integer)") {
  Value x = Value::make_integer (3).convert (&Int32Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&Int32Type::instance));
}

TEST_CASE ("Value::left_shift (int64, Integer)") {
  Value x = Value::make_integer (3).convert (&Int64Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&Int64Type::instance));
}

TEST_CASE ("Value::left_shift (uint8, Integer)") {
  Value x = Value::make_integer (3).convert (&Uint8Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::left_shift (uint16, Integer)") {
  Value x = Value::make_integer (3).convert (&Uint16Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::left_shift (uint32, Integer)") {
  Value x = Value::make_integer (3).convert (&Uint32Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::left_shift (uint64, Integer)") {
  Value x = Value::make_integer (3).convert (&Uint64Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::left_shift (int, Integer)") {
  Value x = Value::make_integer (3).convert (&IntType::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&IntType::instance));
}

TEST_CASE ("Value::left_shift (uint, Integer)") {
  Value x = Value::make_integer (3).convert (&UintType::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&UintType::instance));
}

TEST_CASE ("Value::left_shift (uintptr, Integer)") {
  Value x = Value::make_integer (3).convert (&UintptrType::instance);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48).convert (&UintptrType::instance));
}

TEST_CASE ("Value::left_shift (Rune, Integer)") {
  Value x = Value::make_rune (3);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Float, Integer)") {
  Value x = Value::make_float (3);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}

TEST_CASE ("Value::left_shift (Complex, Integer)") {
  Value x = Value::make_complex (3, 0);
  Value y = Value::make_integer (4);
  Value z = x.left_shift (y);
  REQUIRE (z == Value::make_integer (48));
}



TEST_CASE ("Value::right_shift (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.right_shift (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::right_shift (Boolean, Integer)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::right_shift (Integer, Boolean)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_boolean (true);
  Value z = x.right_shift (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::right_shift (Integer, Integer)") {
  Value x = Value::make_integer (48);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Integer, Rune)") {
  Value x = Value::make_integer (48);
  Value y = Value::make_rune (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Integer, Float)") {
  Value x = Value::make_integer (48);
  Value y = Value::make_float (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Integer, Complex)") {
  Value x = Value::make_integer (48);
  Value y = Value::make_complex (4, 0);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Integer, uint8)") {
  Value x = Value::make_integer (48);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Integer, uint16)") {
  Value x = Value::make_integer (48);
  Value y = Value::make_integer (4).convert (&Uint16Type::instance);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Integer, uint32)") {
  Value x = Value::make_integer (48);
  Value y = Value::make_integer (4).convert (&Uint32Type::instance);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Integer, uint64)") {
  Value x = Value::make_integer (48);
  Value y = Value::make_integer (4).convert (&Uint64Type::instance);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Integer, uint)") {
  Value x = Value::make_integer (48);
  Value y = Value::make_integer (4).convert (&UintType::instance);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Integer, uintptr)") {
  Value x = Value::make_integer (48);
  Value y = Value::make_integer (4).convert (&UintptrType::instance);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (int8, Integer)") {
  Value x = Value::make_integer (48).convert (&Int8Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&Int8Type::instance));
}

TEST_CASE ("Value::right_shift (int16, Integer)") {
  Value x = Value::make_integer (48).convert (&Int16Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&Int16Type::instance));
}

TEST_CASE ("Value::right_shift (int32, Integer)") {
  Value x = Value::make_integer (48).convert (&Int32Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&Int32Type::instance));
}

TEST_CASE ("Value::right_shift (int64, Integer)") {
  Value x = Value::make_integer (48).convert (&Int64Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&Int64Type::instance));
}

TEST_CASE ("Value::right_shift (uint8, Integer)") {
  Value x = Value::make_integer (48).convert (&Uint8Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::right_shift (uint16, Integer)") {
  Value x = Value::make_integer (48).convert (&Uint16Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::right_shift (uint32, Integer)") {
  Value x = Value::make_integer (48).convert (&Uint32Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::right_shift (uint64, Integer)") {
  Value x = Value::make_integer (48).convert (&Uint64Type::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::right_shift (int, Integer)") {
  Value x = Value::make_integer (48).convert (&IntType::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&IntType::instance));
}

TEST_CASE ("Value::right_shift (uint, Integer)") {
  Value x = Value::make_integer (48).convert (&UintType::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&UintType::instance));
}

TEST_CASE ("Value::right_shift (uintptr, Integer)") {
  Value x = Value::make_integer (48).convert (&UintptrType::instance);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3).convert (&UintptrType::instance));
}

TEST_CASE ("Value::right_shift (Rune, Integer)") {
  Value x = Value::make_rune (48);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Float, Integer)") {
  Value x = Value::make_float (48);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}

TEST_CASE ("Value::right_shift (Complex, Integer)") {
  Value x = Value::make_complex (48, 0);
  Value y = Value::make_integer (4);
  Value z = x.right_shift (y);
  REQUIRE (z == Value::make_integer (3));
}



TEST_CASE ("Value::bit_and (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.bit_and (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::bit_and (Boolean, Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (true);
  Value z = x.bit_and (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::bit_and (Integer, Integer)") {
  Value x = Value::make_integer (10);
  Value y = Value::make_integer (-4);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8));
}

TEST_CASE ("Value::bit_and (Rune, Integer)") {
  Value x = Value::make_rune (10);
  Value y = Value::make_integer (-4);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_rune (8));
}

TEST_CASE ("Value::bit_and (Integer, Rune)") {
  Value x = Value::make_integer (10);
  Value y = Value::make_rune (-4);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_rune (8));
}

TEST_CASE ("Value::bit_and (int8, int8)") {
  Value x = Value::make_integer (10).convert (&Int8Type::instance);
  Value y = Value::make_integer (-4).convert (&Int8Type::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&Int8Type::instance));
}

TEST_CASE ("Value::bit_and (int16, int16)") {
  Value x = Value::make_integer (10).convert (&Int16Type::instance);
  Value y = Value::make_integer (-4).convert (&Int16Type::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&Int16Type::instance));
}

TEST_CASE ("Value::bit_and (int32, int32)") {
  Value x = Value::make_integer (10).convert (&Int32Type::instance);
  Value y = Value::make_integer (-4).convert (&Int32Type::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&Int32Type::instance));
}

TEST_CASE ("Value::bit_and (int64, int64)") {
  Value x = Value::make_integer (10).convert (&Int64Type::instance);
  Value y = Value::make_integer (-4).convert (&Int64Type::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&Int64Type::instance));
}

TEST_CASE ("Value::bit_and (uint8, uint8)") {
  Value x = Value::make_integer (0x0A).convert (&Uint8Type::instance);
  Value y = Value::make_integer (0xFC).convert (&Uint8Type::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::bit_and (uint16, uint16)") {
  Value x = Value::make_integer (0x0A).convert (&Uint16Type::instance);
  Value y = Value::make_integer (0xFFFC).convert (&Uint16Type::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::bit_and (uint32, uint32)") {
  Value x = Value::make_integer (0x0A).convert (&Uint32Type::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&Uint32Type::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::bit_and (uint64, uint64)") {
  Value x = Value::make_integer (0x0A).convert (&Uint64Type::instance);
  Value y = Value::make_integer (0xFFFFFFFFFFFFFFFC).convert (&Uint64Type::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::bit_and (int, int)") {
  Value x = Value::make_integer (10).convert (&IntType::instance);
  Value y = Value::make_integer (-4).convert (&IntType::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&IntType::instance));
}

TEST_CASE ("Value::bit_and (uint, uint)") {
  Value x = Value::make_integer (0x0A).convert (&UintType::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&UintType::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&UintType::instance));
}

TEST_CASE ("Value::bit_and (uintptr, uintptr)") {
  Value x = Value::make_integer (0x0A).convert (&UintptrType::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&UintptrType::instance);
  Value z = x.bit_and (y);
  REQUIRE (z == Value::make_integer (8).convert (&UintptrType::instance));
}

TEST_CASE ("Value::bit_and (int8, uint8)") {
  Value x = Value::make_integer (10).convert (&Int8Type::instance);
  Value y = Value::make_integer (0xFC).convert (&Uint8Type::instance);
  Value z = x.bit_and (y);
  REQUIRE (z.is_error ());
}



TEST_CASE ("Value::bit_and_not (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.bit_and_not (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::bit_and_not (Boolean, Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (true);
  Value z = x.bit_and_not (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::bit_and_not (Integer, Integer)") {
  Value x = Value::make_integer (10);
  Value y = Value::make_integer (-4);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2));
}

TEST_CASE ("Value::bit_and_not (Rune, Integer)") {
  Value x = Value::make_rune (10);
  Value y = Value::make_integer (-4);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_rune (2));
}

TEST_CASE ("Value::bit_and_not (Integer, Rune)") {
  Value x = Value::make_integer (10);
  Value y = Value::make_rune (-4);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_rune (2));
}

TEST_CASE ("Value::bit_and_not (int8, int8)") {
  Value x = Value::make_integer (10).convert (&Int8Type::instance);
  Value y = Value::make_integer (-4).convert (&Int8Type::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&Int8Type::instance));
}

TEST_CASE ("Value::bit_and_not (int16, int16)") {
  Value x = Value::make_integer (10).convert (&Int16Type::instance);
  Value y = Value::make_integer (-4).convert (&Int16Type::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&Int16Type::instance));
}

TEST_CASE ("Value::bit_and_not (int32, int32)") {
  Value x = Value::make_integer (10).convert (&Int32Type::instance);
  Value y = Value::make_integer (-4).convert (&Int32Type::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&Int32Type::instance));
}

TEST_CASE ("Value::bit_and_not (int64, int64)") {
  Value x = Value::make_integer (10).convert (&Int64Type::instance);
  Value y = Value::make_integer (-4).convert (&Int64Type::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&Int64Type::instance));
}

TEST_CASE ("Value::bit_and_not (uint8, uint8)") {
  Value x = Value::make_integer (0x0A).convert (&Uint8Type::instance);
  Value y = Value::make_integer (0xFC).convert (&Uint8Type::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::bit_and_not (uint16, uint16)") {
  Value x = Value::make_integer (0x0A).convert (&Uint16Type::instance);
  Value y = Value::make_integer (0xFFFC).convert (&Uint16Type::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::bit_and_not (uint32, uint32)") {
  Value x = Value::make_integer (0x0A).convert (&Uint32Type::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&Uint32Type::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::bit_and_not (uint64, uint64)") {
  Value x = Value::make_integer (0x0A).convert (&Uint64Type::instance);
  Value y = Value::make_integer (0xFFFFFFFFFFFFFFFC).convert (&Uint64Type::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::bit_and_not (int, int)") {
  Value x = Value::make_integer (10).convert (&IntType::instance);
  Value y = Value::make_integer (-4).convert (&IntType::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&IntType::instance));
}

TEST_CASE ("Value::bit_and_not (uint, uint)") {
  Value x = Value::make_integer (0x0A).convert (&UintType::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&UintType::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&UintType::instance));
}

TEST_CASE ("Value::bit_and_not (uintptr, uintptr)") {
  Value x = Value::make_integer (0x0A).convert (&UintptrType::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&UintptrType::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z == Value::make_integer (2).convert (&UintptrType::instance));
}

TEST_CASE ("Value::bit_and_not (int8, uint8)") {
  Value x = Value::make_integer (10).convert (&Int8Type::instance);
  Value y = Value::make_integer (0xFC).convert (&Uint8Type::instance);
  Value z = x.bit_and_not (y);
  REQUIRE (z.is_error ());
}



TEST_CASE ("Value::add (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.add (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::add (Boolean, Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (true);
  Value z = x.add (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::add (Integer, Integer)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7));
}

TEST_CASE ("Value::add (Rune, Integer)") {
  Value x = Value::make_rune (3);
  Value y = Value::make_integer (4);
  Value z = x.add (y);
  REQUIRE (z == Value::make_rune (7));
}

TEST_CASE ("Value::add (Integer, Rune)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_rune (4);
  Value z = x.add (y);
  REQUIRE (z == Value::make_rune (7));
}

TEST_CASE ("Value::add (Float, Float)") {
  Value x = Value::make_float (1.5);
  Value y = Value::make_float (2.25);
  Value z = x.add (y);
  REQUIRE (z == Value::make_float (3.75));
}

TEST_CASE ("Value::add (Complex, Complex)") {
  Value x = Value::make_complex (1, 2);
  Value y = Value::make_complex (3, 4);
  Value z = x.add (y);
  REQUIRE (z == Value::make_complex (4, 6));
}

TEST_CASE ("Value::add (String, String)") {
  Value x = Value::make_string ("hello ");
  Value y = Value::make_string ("world");
  Value z = x.add (y);
  REQUIRE (z == Value::make_string ("hello world"));
}

TEST_CASE ("Value::add (int8, int8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4).convert (&Int8Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&Int8Type::instance));
}

TEST_CASE ("Value::add (int16, int16)") {
  Value x = Value::make_integer (3).convert (&Int16Type::instance);
  Value y = Value::make_integer (4).convert (&Int16Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&Int16Type::instance));
}

TEST_CASE ("Value::add (int32, int32)") {
  Value x = Value::make_integer (3).convert (&Int32Type::instance);
  Value y = Value::make_integer (4).convert (&Int32Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&Int32Type::instance));
}

TEST_CASE ("Value::add (int64, int64)") {
  Value x = Value::make_integer (3).convert (&Int64Type::instance);
  Value y = Value::make_integer (4).convert (&Int64Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&Int64Type::instance));
}

TEST_CASE ("Value::add (uint8, uint8)") {
  Value x = Value::make_integer (3).convert (&Uint8Type::instance);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::add (uint16, uint16)") {
  Value x = Value::make_integer (3).convert (&Uint16Type::instance);
  Value y = Value::make_integer (4).convert (&Uint16Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::add (uint32, uint32)") {
  Value x = Value::make_integer (3).convert (&Uint32Type::instance);
  Value y = Value::make_integer (4).convert (&Uint32Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::add (uint64, uint64)") {
  Value x = Value::make_integer (3).convert (&Uint64Type::instance);
  Value y = Value::make_integer (4).convert (&Uint64Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::add (int, int)") {
  Value x = Value::make_integer (3).convert (&IntType::instance);
  Value y = Value::make_integer (4).convert (&IntType::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&IntType::instance));
}

TEST_CASE ("Value::add (uint, uint)") {
  Value x = Value::make_integer (3).convert (&UintType::instance);
  Value y = Value::make_integer (4).convert (&UintType::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&UintType::instance));
}

TEST_CASE ("Value::add (uintptr, uintptr)") {
  Value x = Value::make_integer (3).convert (&UintptrType::instance);
  Value y = Value::make_integer (4).convert (&UintptrType::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_integer (7).convert (&UintptrType::instance));
}

TEST_CASE ("Value::add (float32, float32)") {
  Value x = Value::make_float (3).convert (&Float32Type::instance);
  Value y = Value::make_float (4).convert (&Float32Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_float (7).convert (&Float32Type::instance));
}

TEST_CASE ("Value::add (float64, float64)") {
  Value x = Value::make_float (3).convert (&Float64Type::instance);
  Value y = Value::make_float (4).convert (&Float64Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_float (7).convert (&Float64Type::instance));
}

TEST_CASE ("Value::add (complex64, complex64)") {
  Value x = Value::make_complex (1, 2).convert (&Complex64Type::instance);
  Value y = Value::make_complex (3, 4).convert (&Complex64Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_complex (4, 6).convert (&Complex64Type::instance));
}

TEST_CASE ("Value::add (complex128, complex128)") {
  Value x = Value::make_complex (1, 2).convert (&Complex128Type::instance);
  Value y = Value::make_complex (3, 4).convert (&Complex128Type::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_complex (4, 6).convert (&Complex128Type::instance));
}

TEST_CASE ("Value::add (string, string)") {
  Value x = Value::make_string ("hello ").convert (&StringType::instance);
  Value y = Value::make_string ("world").convert (&StringType::instance);
  Value z = x.add (y);
  REQUIRE (z == Value::make_string ("hello world").convert (&StringType::instance));
}

TEST_CASE ("Value::add (int8, uint8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.add (y);
  REQUIRE (z.is_error ());
}



TEST_CASE ("Value::subtract (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.subtract (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::subtract (Boolean, Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (true);
  Value z = x.subtract (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::subtract (Integer, Integer)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (-1));
}

TEST_CASE ("Value::subtract (Rune, Integer)") {
  Value x = Value::make_rune (3);
  Value y = Value::make_integer (4);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_rune (-1));
}

TEST_CASE ("Value::subtract (Integer, Rune)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_rune (4);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_rune (-1));
}

TEST_CASE ("Value::subtract (Float, Float)") {
  Value x = Value::make_float (1.5);
  Value y = Value::make_float (2.25);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_float (-0.75));
}

TEST_CASE ("Value::subtract (Complex, Complex)") {
  Value x = Value::make_complex (1, 2);
  Value y = Value::make_complex (3, 4);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_complex (-2, -2));
}

TEST_CASE ("Value::subtract (int8, int8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4).convert (&Int8Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (-1).convert (&Int8Type::instance));
}

TEST_CASE ("Value::subtract (int16, int16)") {
  Value x = Value::make_integer (3).convert (&Int16Type::instance);
  Value y = Value::make_integer (4).convert (&Int16Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (-1).convert (&Int16Type::instance));
}

TEST_CASE ("Value::subtract (int32, int32)") {
  Value x = Value::make_integer (3).convert (&Int32Type::instance);
  Value y = Value::make_integer (4).convert (&Int32Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (-1).convert (&Int32Type::instance));
}

TEST_CASE ("Value::subtract (int64, int64)") {
  Value x = Value::make_integer (3).convert (&Int64Type::instance);
  Value y = Value::make_integer (4).convert (&Int64Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (-1).convert (&Int64Type::instance));
}

TEST_CASE ("Value::subtract (uint8, uint8)") {
  Value x = Value::make_integer (3).convert (&Uint8Type::instance);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (255).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::subtract (uint16, uint16)") {
  Value x = Value::make_integer (3).convert (&Uint16Type::instance);
  Value y = Value::make_integer (4).convert (&Uint16Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (65535).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::subtract (uint32, uint32)") {
  Value x = Value::make_integer (3).convert (&Uint32Type::instance);
  Value y = Value::make_integer (4).convert (&Uint32Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (4294967295).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::subtract (uint64, uint64)") {
  Value x = Value::make_integer (3).convert (&Uint64Type::instance);
  Value y = Value::make_integer (4).convert (&Uint64Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (18446744073709551615UL).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::subtract (int, int)") {
  Value x = Value::make_integer (3).convert (&IntType::instance);
  Value y = Value::make_integer (4).convert (&IntType::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (-1).convert (&IntType::instance));
}

TEST_CASE ("Value::subtract (uint, uint)") {
  Value x = Value::make_integer (3).convert (&UintType::instance);
  Value y = Value::make_integer (4).convert (&UintType::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (4294967295).convert (&UintType::instance));
}

TEST_CASE ("Value::subtract (uintptr, uintptr)") {
  Value x = Value::make_integer (3).convert (&UintptrType::instance);
  Value y = Value::make_integer (4).convert (&UintptrType::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_integer (4294967295).convert (&UintptrType::instance));
}

TEST_CASE ("Value::subtract (float32, float32)") {
  Value x = Value::make_float (3).convert (&Float32Type::instance);
  Value y = Value::make_float (4).convert (&Float32Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_float (-1).convert (&Float32Type::instance));
}

TEST_CASE ("Value::subtract (float64, float64)") {
  Value x = Value::make_float (3).convert (&Float64Type::instance);
  Value y = Value::make_float (4).convert (&Float64Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_float (-1).convert (&Float64Type::instance));
}

TEST_CASE ("Value::subtract (complex64, complex64)") {
  Value x = Value::make_complex (1, 2).convert (&Complex64Type::instance);
  Value y = Value::make_complex (3, 4).convert (&Complex64Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_complex (-2, -2).convert (&Complex64Type::instance));
}

TEST_CASE ("Value::subtract (complex128, complex128)") {
  Value x = Value::make_complex (1, 2).convert (&Complex128Type::instance);
  Value y = Value::make_complex (3, 4).convert (&Complex128Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z == Value::make_complex (-2, -2).convert (&Complex128Type::instance));
}

TEST_CASE ("Value::subtract (int8, uint8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (4).convert (&Uint8Type::instance);
  Value z = x.subtract (y);
  REQUIRE (z.is_error ());
}



TEST_CASE ("Value::bit_or (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.bit_or (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::bit_or (Boolean, Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (true);
  Value z = x.bit_or (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::bit_or (Integer, Integer)") {
  Value x = Value::make_integer (10);
  Value y = Value::make_integer (-4);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (-2));
}

TEST_CASE ("Value::bit_or (Rune, Integer)") {
  Value x = Value::make_rune (10);
  Value y = Value::make_integer (-4);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_rune (-2));
}

TEST_CASE ("Value::bit_or (Integer, Rune)") {
  Value x = Value::make_integer (10);
  Value y = Value::make_rune (-4);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_rune (-2));
}

TEST_CASE ("Value::bit_or (int8, int8)") {
  Value x = Value::make_integer (10).convert (&Int8Type::instance);
  Value y = Value::make_integer (-4).convert (&Int8Type::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (-2).convert (&Int8Type::instance));
}

TEST_CASE ("Value::bit_or (int16, int16)") {
  Value x = Value::make_integer (10).convert (&Int16Type::instance);
  Value y = Value::make_integer (-4).convert (&Int16Type::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (-2).convert (&Int16Type::instance));
}

TEST_CASE ("Value::bit_or (int32, int32)") {
  Value x = Value::make_integer (10).convert (&Int32Type::instance);
  Value y = Value::make_integer (-4).convert (&Int32Type::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (-2).convert (&Int32Type::instance));
}

TEST_CASE ("Value::bit_or (int64, int64)") {
  Value x = Value::make_integer (10).convert (&Int64Type::instance);
  Value y = Value::make_integer (-4).convert (&Int64Type::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (-2).convert (&Int64Type::instance));
}

TEST_CASE ("Value::bit_or (uint8, uint8)") {
  Value x = Value::make_integer (0x0A).convert (&Uint8Type::instance);
  Value y = Value::make_integer (0xFC).convert (&Uint8Type::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (0xFE).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::bit_or (uint16, uint16)") {
  Value x = Value::make_integer (0x0A).convert (&Uint16Type::instance);
  Value y = Value::make_integer (0xFFFC).convert (&Uint16Type::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (0xFFFE).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::bit_or (uint32, uint32)") {
  Value x = Value::make_integer (0x0A).convert (&Uint32Type::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&Uint32Type::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (0xFFFFFFFE).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::bit_or (uint64, uint64)") {
  Value x = Value::make_integer (0x0A).convert (&Uint64Type::instance);
  Value y = Value::make_integer (0xFFFFFFFFFFFFFFFC).convert (&Uint64Type::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (0xFFFFFFFFFFFFFFFE).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::bit_or (int, int)") {
  Value x = Value::make_integer (10).convert (&IntType::instance);
  Value y = Value::make_integer (-4).convert (&IntType::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (-2).convert (&IntType::instance));
}

TEST_CASE ("Value::bit_or (uint, uint)") {
  Value x = Value::make_integer (0x0A).convert (&UintType::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&UintType::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (0xFFFFFFFE).convert (&UintType::instance));
}

TEST_CASE ("Value::bit_or (uintptr, uintptr)") {
  Value x = Value::make_integer (0x0A).convert (&UintptrType::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&UintptrType::instance);
  Value z = x.bit_or (y);
  REQUIRE (z == Value::make_integer (0xFFFFFFFE).convert (&UintptrType::instance));
}

TEST_CASE ("Value::bit_or (int8, uint8)") {
  Value x = Value::make_integer (10).convert (&Int8Type::instance);
  Value y = Value::make_integer (0xFC).convert (&Uint8Type::instance);
  Value z = x.bit_or (y);
  REQUIRE (z.is_error ());
}



TEST_CASE ("Value::bit_xor (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.bit_xor (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::bit_xor (Boolean, Boolean)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (true);
  Value z = x.bit_xor (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::bit_xor (Integer, Integer)") {
  Value x = Value::make_integer (10);
  Value y = Value::make_integer (-4);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (-10));
}

TEST_CASE ("Value::bit_xor (Rune, Integer)") {
  Value x = Value::make_rune (10);
  Value y = Value::make_integer (-4);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_rune (-10));
}

TEST_CASE ("Value::bit_xor (Integer, Rune)") {
  Value x = Value::make_integer (10);
  Value y = Value::make_rune (-4);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_rune (-10));
}

TEST_CASE ("Value::bit_xor (int8, int8)") {
  Value x = Value::make_integer (10).convert (&Int8Type::instance);
  Value y = Value::make_integer (-4).convert (&Int8Type::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (-10).convert (&Int8Type::instance));
}

TEST_CASE ("Value::bit_xor (int16, int16)") {
  Value x = Value::make_integer (10).convert (&Int16Type::instance);
  Value y = Value::make_integer (-4).convert (&Int16Type::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (-10).convert (&Int16Type::instance));
}

TEST_CASE ("Value::bit_xor (int32, int32)") {
  Value x = Value::make_integer (10).convert (&Int32Type::instance);
  Value y = Value::make_integer (-4).convert (&Int32Type::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (-10).convert (&Int32Type::instance));
}

TEST_CASE ("Value::bit_xor (int64, int64)") {
  Value x = Value::make_integer (10).convert (&Int64Type::instance);
  Value y = Value::make_integer (-4).convert (&Int64Type::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (-10).convert (&Int64Type::instance));
}

TEST_CASE ("Value::bit_xor (uint8, uint8)") {
  Value x = Value::make_integer (0x0A).convert (&Uint8Type::instance);
  Value y = Value::make_integer (0xFC).convert (&Uint8Type::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (0xF6).convert (&Uint8Type::instance));
}

TEST_CASE ("Value::bit_xor (uint16, uint16)") {
  Value x = Value::make_integer (0x0A).convert (&Uint16Type::instance);
  Value y = Value::make_integer (0xFFFC).convert (&Uint16Type::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (0xFFF6).convert (&Uint16Type::instance));
}

TEST_CASE ("Value::bit_xor (uint32, uint32)") {
  Value x = Value::make_integer (0x0A).convert (&Uint32Type::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&Uint32Type::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (0xFFFFFFF6).convert (&Uint32Type::instance));
}

TEST_CASE ("Value::bit_xor (uint64, uint64)") {
  Value x = Value::make_integer (0x0A).convert (&Uint64Type::instance);
  Value y = Value::make_integer (0xFFFFFFFFFFFFFFFC).convert (&Uint64Type::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (0xFFFFFFFFFFFFFFF6).convert (&Uint64Type::instance));
}

TEST_CASE ("Value::bit_xor (int, int)") {
  Value x = Value::make_integer (10).convert (&IntType::instance);
  Value y = Value::make_integer (-4).convert (&IntType::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (-10).convert (&IntType::instance));
}

TEST_CASE ("Value::bit_xor (uint, uint)") {
  Value x = Value::make_integer (0x0A).convert (&UintType::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&UintType::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (0xFFFFFFF6).convert (&UintType::instance));
}

TEST_CASE ("Value::bit_xor (uintptr, uintptr)") {
  Value x = Value::make_integer (0x0A).convert (&UintptrType::instance);
  Value y = Value::make_integer (0xFFFFFFFC).convert (&UintptrType::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z == Value::make_integer (0xFFFFFFF6).convert (&UintptrType::instance));
}

TEST_CASE ("Value::bit_xor (int8, uint8)") {
  Value x = Value::make_integer (10).convert (&Int8Type::instance);
  Value y = Value::make_integer (0xFC).convert (&Uint8Type::instance);
  Value z = x.bit_xor (y);
  REQUIRE (z.is_error ());
}



TEST_CASE ("Value::equal (Error, Error)") {
  Value x = Value::make_error ();
  Value y = Value::make_error ();
  Value z = x.equal (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::equal (true, true)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (true);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (true, false)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_boolean (false);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (Bool, Integer)") {
  Value x = Value::make_boolean (true);
  Value y = Value::make_integer (0);
  Value z = x.equal (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::equal (Integer, Bool)") {
  Value x = Value::make_integer (0);
  Value y = Value::make_boolean (true);
  Value z = x.equal (y);
  REQUIRE (z.is_error ());
}

TEST_CASE ("Value::equal (Integer, Integer)") {
  Value x = Value::make_integer (3);
  Value y = Value::make_integer (4);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (Rune, Rune)") {
  Value x = Value::make_rune (3);
  Value y = Value::make_rune (3);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (Float, Float)") {
  Value x = Value::make_float (3);
  Value y = Value::make_float (3.5);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (Complex, Complex)") {
  Value x = Value::make_complex (3.5, 4);
  Value y = Value::make_complex (3.5, 4);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (String, String)") {
  Value x = Value::make_string ("hello");
  Value y = Value::make_string ("hello, world");
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (bool, bool)") {
  Value x = Value::make_boolean (false).convert (&BoolType::instance);
  Value y = Value::make_boolean (false);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (complex64, complex64)") {
  Value x = Value::make_complex (1, 2);
  Value y = Value::make_complex (3, 4).convert (&Complex64Type::instance);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (complex128, complex128)") {
  Value x = Value::make_complex (1, 2).convert (&Complex128Type::instance);
  Value y = Value::make_complex (1, 2);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (float32, float32)") {
  Value x = Value::make_float (1);
  Value y = Value::make_float (3).convert (&Float32Type::instance);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (float64, float64)") {
  Value x = Value::make_float (1).convert (&Float64Type::instance);
  Value y = Value::make_float (1);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (int, int)") {
  Value x = Value::make_integer (1);
  Value y = Value::make_integer (3).convert (&IntType::instance);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (int8, int8)") {
  Value x = Value::make_integer (1).convert (&Int8Type::instance);
  Value y = Value::make_integer (1);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (int16, int16)") {
  Value x = Value::make_integer (1);
  Value y = Value::make_integer (3).convert (&Int16Type::instance);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (int32, int32)") {
  Value x = Value::make_integer (1).convert (&Int32Type::instance);
  Value y = Value::make_integer (1);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (int64, int64)") {
  Value x = Value::make_integer (1);
  Value y = Value::make_integer (3).convert (&Int64Type::instance);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (uint, uint)") {
  Value x = Value::make_integer (1).convert (&UintType::instance);
  Value y = Value::make_integer (1);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (uint8, uint8)") {
  Value x = Value::make_integer (1);
  Value y = Value::make_integer (3).convert (&Uint8Type::instance);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (uint16, uint16)") {
  Value x = Value::make_integer (1).convert (&Uint16Type::instance);
  Value y = Value::make_integer (1);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (uint32, uint32)") {
  Value x = Value::make_integer (1);
  Value y = Value::make_integer (3).convert (&Uint32Type::instance);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (uint64, uint64)") {
  Value x = Value::make_integer (1).convert (&Uint64Type::instance);
  Value y = Value::make_integer (1);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (uintptr, uintptr)") {
  Value x = Value::make_integer (1);
  Value y = Value::make_integer (3).convert (&UintptrType::instance);
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (false));
}

TEST_CASE ("Value::equal (string, string)") {
  Value x = Value::make_string ("hello").convert (&StringType::instance);
  Value y = Value::make_string ("hello");
  Value z = x.equal (y);
  REQUIRE (z == Value::make_boolean (true));
}

TEST_CASE ("Value::equal (int, string)") {
  Value x = Value::make_integer (1).convert (&IntType::instance);
  Value y = Value::make_string ("hello").convert (&StringType::instance);
  Value z = x.equal (y);
  REQUIRE (z.is_error ());
}



// The bitwise logical and shift operators apply to integers only.

// The right operand in a shift expression must have unsigned integer
// type or be an untyped constant representable by a value of type
// uint. If the left operand of a non-constant shift expression is an
// untyped constant, it is first converted to the type it would assume
// if the shift expression were replaced by its left operand alone.

// A constant comparison always yields an untyped boolean constant. If
// the left operand of a constant shift expression is an untyped
// constant, the result is an integer constant; otherwise it is a
// constant of the same type as the left operand, which must be of
// integer type. Applying all other operators to untyped constants
// results in an untyped constant of the same kind (that is, a
// boolean, integer, floating-point, complex, or string constant).

// TODO equal
// TODO not_equal
// TODO less_than
// TODO less_than_equal
// TODO greater_than
// TODO greater_than_equal
// TODO logic_and
// TODO logic_or

// For instance, 3.0 can be given any integer or any floating-point type, while 2147483648.0 (equal to 1<<31) can be given the types float32, float64, or uint32 but not int32 or string.

// An untyped constant has a default type which is the type to which the constant is implicitly converted in contexts where a typed value is required, for instance, in a short variable declaration such as i := 0 where there is no explicit type. The default type of an untyped constant is bool, rune, int, float64, complex128 or string respectively, depending on whether it is a boolean, rune, integer, floating-point, complex, or string constant.

// Implementation restriction: Although numeric constants have arbitrary precision in the language, a compiler may implement them using an internal representation with limited precision. That said, every implementation must:

// Represent integer constants with at least 256 bits.
// Represent floating-point constants, including the parts of a complex constant, with a mantissa of at least 256 bits and a signed binary exponent of at least 16 bits.
// Give an error if unable to represent an integer constant precisely.
// Give an error if unable to represent a floating-point or complex constant due to overflow.
// Round to the nearest representable constant if unable to represent a floating-point or complex constant due to limits on precision.
// These requirements apply both to literal constants and to the result of evaluating constant expressions.

// Constant expressions may contain only constant operands and are evaluated at compile time.

// Untyped boolean, numeric, and string constants may be used as operands wherever it is legal to use an operand of boolean, numeric, or string type, respectively. Except for shift operations, if the operands of a binary operation are different kinds of untyped constants, the operation and, for non-boolean operations, the result use the kind that appears later in this list: integer, rune, floating-point, complex. For example, an untyped integer constant divided by an untyped complex constant yields an untyped complex constant.

// A constant comparison always yields an untyped boolean constant. If the left operand of a constant shift expression is an untyped constant, the result is an integer constant; otherwise it is a constant of the same type as the left operand, which must be of integer type. Applying all other operators to untyped constants results in an untyped constant of the same kind (that is, a boolean, integer, floating-point, complex, or string constant).

// const a = 2 + 3.0          // a == 5.0   (untyped floating-point constant)
// const b = 15 / 4           // b == 3     (untyped integer constant)
// const c = 15 / 4.0         // c == 3.75  (untyped floating-point constant)
// const Θ float64 = 3/2      // Θ == 1.0   (type float64, 3/2 is integer division)
// const Π float64 = 3/2.     // Π == 1.5   (type float64, 3/2. is float division)
// const d = 1 << 3.0         // d == 8     (untyped integer constant)
// const e = 1.0 << 3         // e == 8     (untyped integer constant)
// const f = int32(1) << 33   // illegal    (constant 8589934592 overflows int32)
// const g = float64(2) >> 1  // illegal    (float64(2) is a typed floating-point constant)
// const h = "foo" > "bar"    // h == true  (untyped boolean constant)
// const j = true             // j == true  (untyped boolean constant)
// const k = 'w' + 1          // k == 'x'   (untyped rune constant)
// const l = "hi"             // l == "hi"  (untyped string constant)
// const m = string(k)        // m == "x"   (type string)
// const Σ = 1 - 0.707i       //            (untyped complex constant)
// const Δ = Σ + 2.0e-4       //            (untyped complex constant)
// const Φ = iota*1i - 1/1i   //            (untyped complex constant)
// Applying the built-in function complex to untyped integer, rune, or floating-point constants yields an untyped complex constant.

// const ic = complex(0, c)   // ic == 3.75i  (untyped complex constant)
// const iΘ = complex(0, Θ)   // iΘ == 1i     (type complex128)
// Constant expressions are always evaluated exactly; intermediate values and the constants themselves may require precision significantly larger than supported by any predeclared type in the language. The following are legal declarations:

// const Huge = 1 << 100         // Huge == 1267650600228229401496703205376  (untyped integer constant)
// const Four int8 = Huge >> 98  // Four == 4                                (type int8)
// The divisor of a constant division or remainder operation must not be zero:

// 3.14 / 0.0   // illegal: division by zero
// The values of typed constants must always be accurately representable as values of the constant type. The following constant expressions are illegal:

// uint(-1)     // -1 cannot be represented as a uint
// int(3.14)    // 3.14 cannot be represented as an int
// int64(Huge)  // 1267650600228229401496703205376 cannot be represented as an int64
// Four * 300   // operand 300 cannot be represented as an int8 (type of Four)
// Four * 100   // product 400 cannot be represented as an int8 (type of Four)
// The mask used by the unary bitwise complement operator ^ matches the rule for non-constants: the mask is all 1s for unsigned constants and -1 for signed and untyped constants.

// ^1         // untyped integer constant, equal to -2
// uint8(^1)  // illegal: same as uint8(-2), -2 cannot be represented as a uint8
// ^uint8(1)  // typed uint8 constant, same as 0xFF ^ uint8(1) = uint8(0xFE)
// int8(^1)   // same as int8(-2)
// ^int8(1)   // same as -1 ^ int8(1) = -2
// Implementation restriction: A compiler may use rounding while computing untyped floating-point or complex constant expressions; see the implementation restriction in the section on constants. This rounding may cause a floating-point constant expression to be invalid in an integer context, even if it would be integral when calculated using infinite precision, and vice versa.

TEST_CASE ("Value::operator== (Unknown, Error)") {
  Value x;
  Value y = Value::make_error ();
  REQUIRE (x != y);
}

TEST_CASE ("Value::operator== (Unknown, Unknown)") {
  Value x;
  Value y;
  REQUIRE (x == y);
}

TEST_CASE ("Value::operator== (int8, uint8)") {
  Value x = Value::make_integer (3).convert (&Int8Type::instance);
  Value y = Value::make_integer (3).convert (&Uint8Type::instance);
  REQUIRE (x != y);
}

TEST_CASE ("operator<< (Unknown)") {
  std::stringstream ss;
  Value x;
  ss << x;
  REQUIRE (ss.str () == "unknown");
}

TEST_CASE ("operator<< (Error)") {
  std::stringstream ss;
  Value x = Value::make_error ();
  ss << x;
  REQUIRE (ss.str () == "error");
}

TEST_CASE ("operator<< (Boolean)") {
  {
    std::stringstream ss;
    Value x = Value::make_boolean (true);
    ss << x;
    REQUIRE (ss.str () == "true");
  }
  {
    std::stringstream ss;
    Value x = Value::make_boolean (false);
    ss << x;
    REQUIRE (ss.str () == "false");
  }
}

TEST_CASE ("operator<< (Integer)") {
  std::stringstream ss;
  Value x = Value::make_integer (34);
  ss << x;
  REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (Rune)") {
  {
    std::stringstream ss;
    Value x = Value::make_rune ('j');
    ss << x;
    REQUIRE (ss.str () == "'j'");
  }
  {
    std::stringstream ss;
    Value x = Value::make_rune (INVALID);
    ss << x;
    REQUIRE (ss.str () == "1114112");
  }
  {
    std::stringstream ss;
    Value x = Value::make_rune (SURROGATE_FIRST);
    ss << x;
    REQUIRE (ss.str () == "55296");
  }
}

TEST_CASE ("operator<< (Float)") {
  std::stringstream ss;
  Value x = Value::make_float (34.5);
  ss << x;
  REQUIRE (ss.str () == "34.5");
}

TEST_CASE ("operator<< (Complex)") {
  std::stringstream ss;
  Value x = Value::make_complex (1, 2);
  ss << x;
  REQUIRE (ss.str () == "1+2i");
}

TEST_CASE ("operator<< (String)") {
  std::stringstream ss;
  Value x = Value::make_string ("hello");
  ss << x;
  REQUIRE (ss.str () == "\"hello\"");
}

TEST_CASE ("operator<< (bool)") {
  {
    std::stringstream ss;
    Value x = Value::make_boolean (true).convert (&BoolType::instance);
    ss << x;
    REQUIRE (ss.str () == "true");
  }
  {
    std::stringstream ss;
    Value x = Value::make_boolean (false).convert (&BoolType::instance);
    ss << x;
    REQUIRE (ss.str () == "false");
  }
}

TEST_CASE ("operator<< (int8)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Int8Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (int16)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Int16Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (int32)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Int32Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (int64)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Int64Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (uint8)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Uint8Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (uint16)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Uint16Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (uint32)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Uint32Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (uint64)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Uint64Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (int)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&IntType::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (uint)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&UintType::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (uintptr)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&UintptrType::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (float32)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Float32Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (float64)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Float64Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34");
}

TEST_CASE ("operator<< (complex64)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Complex64Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34+0i");
}

TEST_CASE ("operator<< (complex128)") {
    std::stringstream ss;
    Value x = Value::make_integer (34).convert (&Complex128Type::instance);
    ss << x;
    REQUIRE (ss.str () == "34+0i");
}

TEST_CASE ("operator<< (string)") {
    std::stringstream ss;
    Value x = Value::make_string ("hello").convert (&StringType::instance);
    ss << x;
    REQUIRE (ss.str () == "\"hello\"");
}
