// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <utility>

#include "src/catch.hpp"
#include "src/test_helpers.h"
#include "src/untyped_constant.h"
#include "src/utf8.h"

namespace rcgo {
using std::rel_ops::operator!=;
namespace test {

typedef value::UntypedConstant UC;

TEST_CASE("value::complex_t::complex_t()") {
  value::complex_t c;
  REQUIRE(c.real() == 0);
  REQUIRE(c.imag() == 0);
}

TEST_CASE("value::complex_t::complex_t(1,2)") {
  value::complex_t c(1, 2);
  REQUIRE(c.real() == 1);
  REQUIRE(c.imag() == 2);
}

TEST_CASE("value::complex_t::operator+") {
  value::complex_t c1(1, 2);
  value::complex_t c2(3, 4);
  value::complex_t c3 = c1 + c2;
  REQUIRE(c3 == value::complex_t(4, 6));
}

TEST_CASE("value::complex_t::operator-") {
  value::complex_t c1(1, 2);
  value::complex_t c2(3, 4);
  value::complex_t c3 = c1 - c2;
  REQUIRE(c3 == value::complex_t(-2, -2));
}

TEST_CASE("value::complex_t::operator*") {
  value::complex_t c1(1, 2);
  value::complex_t c2(3, 4);
  value::complex_t c3 = c1 * c2;
  REQUIRE(c3 == value::complex_t(-5, 10));
}

TEST_CASE("value::complex_t::operator/") {
  value::complex_t c1(1, 0);
  value::complex_t c2(0, 1);
  value::complex_t c3 = c1 / c2;
  REQUIRE(c3 == value::complex_t(0, -1));
}

TEST_CASE("value::complex_t::operator+ unary") {
  value::complex_t c1(1, 2);
  value::complex_t c2 = +c1;
  REQUIRE(c2 == value::complex_t(1, 2));
}

TEST_CASE("value::complex_t::operator- unary") {
  value::complex_t c1(1, 2);
  value::complex_t c2 = -c1;
  REQUIRE(c2 == value::complex_t(-1, -2));
}

TEST_CASE("value::complex_t::operator==") {
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

TEST_CASE("value::UntypedConstant::MakeError()") {
  UC v = UC::MakeError();
  REQUIRE(v.kind() == UC::kError);
  REQUIRE(v.IsError());
}

TEST_CASE("value::UntypedConstant::MakeBoolean()") {
  UC v = UC::MakeBoolean(true);
  REQUIRE(v.kind() == UC::kBoolean);
  REQUIRE(v.boolean_value() == true);
}

TEST_CASE("value::UntypedConstant::MakeInteger()") {
  UC v = UC::MakeInteger(3);
  REQUIRE(v.kind() == UC::kInteger);
  REQUIRE(v.integer_value() == 3);
}

TEST_CASE("value::UntypedConstant::MakeRune()") {
  UC v = UC::MakeRune(123);
  REQUIRE(v.kind() == UC::kRune);
  REQUIRE(v.rune_value() == 123);
}

TEST_CASE("value::UntypedConstant::MakeFloat()") {
  UC v = UC::MakeFloat(3.14);
  REQUIRE(v.kind() == UC::kFloat);
  REQUIRE(v.float_value() == 3.14);
}

TEST_CASE("value::UntypedConstant::MakeComplex()") {
  UC v = UC::MakeComplex(1, 2);
  REQUIRE(v.kind() == UC::kComplex);
  REQUIRE(v.complex_value() == value::complex_t(1, 2));
}

TEST_CASE("value::UntypedConstant::MakeString()") {
  UC v = UC::MakeString("hello");
  REQUIRE(v.kind() == UC::kString);
  REQUIRE(v.string_value() == "hello");
}

#define UNARY_TEST(method, name, x, expected) TEST_CASE("value::UntypedConstant::" name) { \
    UC y = UC::method(x);                                               \
    REQUIRE(y == expected);                                             \
}

#define BINARY_TEST(method, name, x, y, expected) TEST_CASE("value::UntypedConstant::" name) { \
    UC z = UC::method(x, y);                                            \
    REQUIRE(z == expected);                                             \
}

UNARY_TEST(Posate, "Posate(Error)", UC::MakeError(), UC::MakeError())
UNARY_TEST(Posate, "Posate(Boolean)", UC::MakeBoolean(true), UC::MakeError())
UNARY_TEST(Posate, "Posate(String)", UC::MakeString("hello"), UC::MakeError())
UNARY_TEST(Posate, "Posate(Integer)", UC::MakeInteger(3), UC::MakeInteger(3))
UNARY_TEST(Posate, "Posate(Rune)", UC::MakeRune(3), UC::MakeRune(3))
UNARY_TEST(Posate, "Posate(Float)", UC::MakeFloat(1.5), UC::MakeFloat(1.5))
UNARY_TEST(Posate, "Posate(Complex)", UC::MakeComplex(1, 2), UC::MakeComplex(1,2 ))

UNARY_TEST(Negate, "Negate(Error)", UC::MakeError(), UC::MakeError())
UNARY_TEST(Negate, "Negate(Boolean)", UC::MakeBoolean(true), UC::MakeError())
UNARY_TEST(Negate, "Negate(String)", UC::MakeString("hello"), UC::MakeError())
UNARY_TEST(Negate, "Negate(Integer)", UC::MakeInteger(3), UC::MakeInteger(-3))
UNARY_TEST(Negate, "Negate(Rune)", UC::MakeRune(3), UC::MakeRune(-3))
UNARY_TEST(Negate, "Negate(Float)", UC::MakeFloat(1.5), UC::MakeFloat(-1.5))
UNARY_TEST(Negate, "Negate(Complex)", UC::MakeComplex(1, 2), UC::MakeComplex(-1,-2 ))

UNARY_TEST(LogicNot, "LogicNot(Error)", UC::MakeError(), UC::MakeError())
UNARY_TEST(LogicNot, "LogicNot(Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(false))
UNARY_TEST(LogicNot, "LogicNot(String)", UC::MakeString("hello"), UC::MakeError())
UNARY_TEST(LogicNot, "LogicNot(Integer)", UC::MakeInteger(3), UC::MakeError());
UNARY_TEST(LogicNot, "LogicNot(Rune)", UC::MakeRune(3), UC::MakeError());
UNARY_TEST(LogicNot, "LogicNot(Float)", UC::MakeFloat(1.5), UC::MakeError());
UNARY_TEST(LogicNot, "LogicNot(Complex)", UC::MakeComplex(1, 2), UC::MakeError());

UNARY_TEST(BitNot, "BitNot(Error)", UC::MakeError(), UC::MakeError())
UNARY_TEST(BitNot, "BitNot(Boolean)", UC::MakeBoolean(true), UC::MakeError());
UNARY_TEST(BitNot, "BitNot(String)", UC::MakeString("hello"), UC::MakeError())
UNARY_TEST(BitNot, "BitNot(Integer)", UC::MakeInteger(3), UC::MakeInteger(-4));
UNARY_TEST(BitNot, "BitNot(Rune)", UC::MakeRune(3), UC::MakeRune(-4));
UNARY_TEST(BitNot, "BitNot(Float)", UC::MakeFloat(1.5), UC::MakeError());
UNARY_TEST(BitNot, "BitNot(Complex)", UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Multiply, "Multiply(Error, Error)", UC::MakeError(), UC::MakeError(), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Error, Boolean)", UC::MakeError(), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Error, String)", UC::MakeError(), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Error, Integer)", UC::MakeError(), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Error, Rune)", UC::MakeError(), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Error, Float)", UC::MakeError(), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Error, Complex)", UC::MakeError(), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Multiply, "Multiply(Boolean, Error)", UC::MakeBoolean(true), UC::MakeError(), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Boolean, String)", UC::MakeBoolean(true), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Boolean, Integer)", UC::MakeBoolean(true), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Boolean, Rune)", UC::MakeBoolean(true), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Boolean, Float)", UC::MakeBoolean(true), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Boolean, Complex)", UC::MakeBoolean(true), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Multiply, "Multiply(String, Error)", UC::MakeString("hello"), UC::MakeError(), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(String, Boolean)", UC::MakeString("hello"), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(String, Integer)", UC::MakeString("hello"), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(String, Rune)", UC::MakeString("hello"), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(String, Float)", UC::MakeString("hello"), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(String, Complex)", UC::MakeString("hello"), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Multiply, "Multiply(Integer, Error)", UC::MakeInteger(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Integer, Boolean)", UC::MakeInteger(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Integer, String)", UC::MakeInteger(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(3), UC::MakeInteger(9));
BINARY_TEST(Multiply, "Multiply(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(3), UC::MakeRune(9));
BINARY_TEST(Multiply, "Multiply(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Multiply, "Multiply(Integer, Complex)", UC::MakeInteger(3), UC::MakeComplex(1, 2), UC::MakeComplex(3, 6));

BINARY_TEST(Multiply, "Multiply(Rune, Error)", UC::MakeRune(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Rune, Boolean)", UC::MakeRune(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Rune, String)", UC::MakeRune(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(3), UC::MakeRune(9));
BINARY_TEST(Multiply, "Multiply(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(3), UC::MakeRune(9));
BINARY_TEST(Multiply, "Multiply(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Multiply, "Multiply(Rune, Complex)", UC::MakeRune(3), UC::MakeComplex(1, 2), UC::MakeComplex(3, 6));

BINARY_TEST(Multiply, "Multiply(Float, Error)", UC::MakeFloat(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Float, Boolean)", UC::MakeFloat(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Float, String)", UC::MakeFloat(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeFloat(9));
BINARY_TEST(Multiply, "Multiply(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeFloat(9));
BINARY_TEST(Multiply, "Multiply(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Multiply, "Multiply(Float, Complex)", UC::MakeFloat(3), UC::MakeComplex(1, 2), UC::MakeComplex(3, 6));

BINARY_TEST(Multiply, "Multiply(Complex, Error)", UC::MakeComplex(1, 2), UC::MakeError(), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Complex, Boolean)", UC::MakeComplex(1, 2), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Multiply, "Multiply(Complex, String)", UC::MakeComplex(1, 2), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Multiply, "Multiply(Complex, Integer)", UC::MakeComplex(1, 2), UC::MakeInteger(3), UC::MakeComplex(3, 6));
BINARY_TEST(Multiply, "Multiply(Complex, Rune)", UC::MakeComplex(1, 2), UC::MakeRune(3), UC::MakeComplex(3, 6));
BINARY_TEST(Multiply, "Multiply(Complex, Float)", UC::MakeComplex(1, 2), UC::MakeFloat(1.5), UC::MakeComplex(1.5, 3));
BINARY_TEST(Multiply, "Multiply(Complex, Complex)", UC::MakeComplex(1, 2), UC::MakeComplex(1, 2), UC::MakeComplex(-3, 4));

BINARY_TEST(Divide, "Divide(Error, Error)", UC::MakeError(), UC::MakeError(), UC::MakeError())
BINARY_TEST(Divide, "Divide(Error, Boolean)", UC::MakeError(), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Divide, "Divide(Error, String)", UC::MakeError(), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Divide, "Divide(Error, Integer)", UC::MakeError(), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Divide, "Divide(Error, Rune)", UC::MakeError(), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Divide, "Divide(Error, Float)", UC::MakeError(), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Divide, "Divide(Error, Complex)", UC::MakeError(), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Divide, "Divide(Boolean, Error)", UC::MakeBoolean(true), UC::MakeError(), UC::MakeError())
BINARY_TEST(Divide, "Divide(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Divide, "Divide(Boolean, String)", UC::MakeBoolean(true), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Divide, "Divide(Boolean, Integer)", UC::MakeBoolean(true), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Divide, "Divide(Boolean, Rune)", UC::MakeBoolean(true), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Divide, "Divide(Boolean, Float)", UC::MakeBoolean(true), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Divide, "Divide(Boolean, Complex)", UC::MakeBoolean(true), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Divide, "Divide(String, Error)", UC::MakeString("hello"), UC::MakeError(), UC::MakeError())
BINARY_TEST(Divide, "Divide(String, Boolean)", UC::MakeString("hello"), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Divide, "Divide(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Divide, "Divide(String, Integer)", UC::MakeString("hello"), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Divide, "Divide(String, Rune)", UC::MakeString("hello"), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Divide, "Divide(String, Float)", UC::MakeString("hello"), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Divide, "Divide(String, Complex)", UC::MakeString("hello"), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Divide, "Divide(Integer, Error)", UC::MakeInteger(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Divide, "Divide(Integer, Boolean)", UC::MakeInteger(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Divide, "Divide(Integer, String)", UC::MakeInteger(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Divide, "Divide(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(3), UC::MakeInteger(1));
BINARY_TEST(Divide, "Divide(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(3), UC::MakeRune(1));
BINARY_TEST(Divide, "Divide(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeFloat(2));
BINARY_TEST(Divide, "Divide(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeComplex(0, -1));

BINARY_TEST(Divide, "Divide(Rune, Error)", UC::MakeRune(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Divide, "Divide(Rune, Boolean)", UC::MakeRune(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Divide, "Divide(Rune, String)", UC::MakeRune(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Divide, "Divide(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(3), UC::MakeRune(1));
BINARY_TEST(Divide, "Divide(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(3), UC::MakeRune(1));
BINARY_TEST(Divide, "Divide(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeFloat(2));
BINARY_TEST(Divide, "Divide(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeComplex(0, -1));

BINARY_TEST(Divide, "Divide(Float, Error)", UC::MakeFloat(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Divide, "Divide(Float, Boolean)", UC::MakeFloat(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Divide, "Divide(Float, String)", UC::MakeFloat(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Divide, "Divide(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeFloat(1));
BINARY_TEST(Divide, "Divide(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeFloat(1));
BINARY_TEST(Divide, "Divide(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeFloat(2));
BINARY_TEST(Divide, "Divide(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeComplex(0, -1));

BINARY_TEST(Divide, "Divide(Complex, Error)", UC::MakeComplex(1, 2), UC::MakeError(), UC::MakeError())
BINARY_TEST(Divide, "Divide(Complex, Boolean)", UC::MakeComplex(1, 2), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Divide, "Divide(Complex, String)", UC::MakeComplex(1, 2), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Divide, "Divide(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeComplex(1, 0));
BINARY_TEST(Divide, "Divide(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeComplex(1, 0));
BINARY_TEST(Divide, "Divide(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeComplex(1, 0));
BINARY_TEST(Divide, "Divide(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeComplex(0, -1));

BINARY_TEST(Modulo, "Modulo(Error, Error)", UC::MakeError(), UC::MakeError(), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Error, Boolean)", UC::MakeError(), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Error, String)", UC::MakeError(), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Error, Integer)", UC::MakeError(), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Error, Rune)", UC::MakeError(), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Error, Float)", UC::MakeError(), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Error, Complex)", UC::MakeError(), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Modulo, "Modulo(Boolean, Error)", UC::MakeBoolean(true), UC::MakeError(), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Boolean, String)", UC::MakeBoolean(true), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Boolean, Integer)", UC::MakeBoolean(true), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Boolean, Rune)", UC::MakeBoolean(true), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Boolean, Float)", UC::MakeBoolean(true), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Boolean, Complex)", UC::MakeBoolean(true), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Modulo, "Modulo(String, Error)", UC::MakeString("hello"), UC::MakeError(), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(String, Boolean)", UC::MakeString("hello"), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(String, Integer)", UC::MakeString("hello"), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(String, Rune)", UC::MakeString("hello"), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(String, Float)", UC::MakeString("hello"), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(String, Complex)", UC::MakeString("hello"), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Modulo, "Modulo(Integer, Error)", UC::MakeInteger(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Integer, Boolean)", UC::MakeInteger(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Integer, String)", UC::MakeInteger(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(3), UC::MakeInteger(0));
BINARY_TEST(Modulo, "Modulo(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(3), UC::MakeRune(0));
BINARY_TEST(Modulo, "Modulo(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(Modulo, "Modulo(Rune, Error)", UC::MakeRune(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Rune, Boolean)", UC::MakeRune(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Rune, String)", UC::MakeRune(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(3), UC::MakeRune(0));
BINARY_TEST(Modulo, "Modulo(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(3), UC::MakeRune(0));
BINARY_TEST(Modulo, "Modulo(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(Modulo, "Modulo(Float, Error)", UC::MakeFloat(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Float, Boolean)", UC::MakeFloat(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Float, String)", UC::MakeFloat(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(Modulo, "Modulo(Complex, Error)", UC::MakeComplex(1, 2), UC::MakeError(), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Complex, Boolean)", UC::MakeComplex(1, 2), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Complex, String)", UC::MakeComplex(1, 2), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Modulo, "Modulo(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeError());
BINARY_TEST(Modulo, "Modulo(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(LeftShift, "LeftShift(Error, 1)", UC::MakeError(), 1, UC::MakeError())
BINARY_TEST(LeftShift, "LeftShift(Boolean, 1)", UC::MakeBoolean(true), 1, UC::MakeError())
BINARY_TEST(LeftShift, "LeftShift(String, 1)", UC::MakeString("hello"), 1, UC::MakeError())
BINARY_TEST(LeftShift, "LeftShift(Integer, 1)", UC::MakeInteger(3), 1, UC::MakeInteger(6))
BINARY_TEST(LeftShift, "LeftShift(Rune, 1)", UC::MakeRune(3), 1, UC::MakeInteger(6))
BINARY_TEST(LeftShift, "LeftShift(Float, 1)", UC::MakeFloat(1.5), 1, UC::MakeError());
BINARY_TEST(LeftShift, "LeftShift(Complex, 1)", UC::MakeComplex(1, 2), 1, UC::MakeError());

BINARY_TEST(RightShift, "RightShift(Error, 1)", UC::MakeError(), 1, UC::MakeError())
BINARY_TEST(RightShift, "RightShift(Boolean, 1)", UC::MakeBoolean(true), 1, UC::MakeError())
BINARY_TEST(RightShift, "RightShift(String, 1)", UC::MakeString("hello"), 1, UC::MakeError())
BINARY_TEST(RightShift, "RightShift(Integer, 1)", UC::MakeInteger(3), 1, UC::MakeInteger(1))
BINARY_TEST(RightShift, "RightShift(Rune, 1)", UC::MakeRune(3), 1, UC::MakeInteger(1))
BINARY_TEST(RightShift, "RightShift(Float, 1)", UC::MakeFloat(1.5), 1, UC::MakeError());
BINARY_TEST(RightShift, "RightShift(Complex, 1)", UC::MakeComplex(1, 2), 1, UC::MakeError());

BINARY_TEST(BitAnd, "BitAnd(Error, Error)", UC::MakeError(), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Error, Boolean)", UC::MakeError(), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Error, String)", UC::MakeError(), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Error, Integer)", UC::MakeError(), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Error, Rune)", UC::MakeError(), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Error, Float)", UC::MakeError(), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Error, Complex)", UC::MakeError(), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitAnd, "BitAnd(Boolean, Error)", UC::MakeBoolean(true), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Boolean, String)", UC::MakeBoolean(true), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Boolean, Integer)", UC::MakeBoolean(true), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Boolean, Rune)", UC::MakeBoolean(true), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Boolean, Float)", UC::MakeBoolean(true), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Boolean, Complex)", UC::MakeBoolean(true), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitAnd, "BitAnd(String, Error)", UC::MakeString("hello"), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(String, Boolean)", UC::MakeString("hello"), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(String, Integer)", UC::MakeString("hello"), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(String, Rune)", UC::MakeString("hello"), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(String, Float)", UC::MakeString("hello"), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(String, Complex)", UC::MakeString("hello"), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitAnd, "BitAnd(Integer, Error)", UC::MakeInteger(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Integer, Boolean)", UC::MakeInteger(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Integer, String)", UC::MakeInteger(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(1));
BINARY_TEST(BitAnd, "BitAnd(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(1));
BINARY_TEST(BitAnd, "BitAnd(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitAnd, "BitAnd(Rune, Error)", UC::MakeRune(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Rune, Boolean)", UC::MakeRune(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Rune, String)", UC::MakeRune(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(1));
BINARY_TEST(BitAnd, "BitAnd(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(1));
BINARY_TEST(BitAnd, "BitAnd(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitAnd, "BitAnd(Float, Error)", UC::MakeFloat(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Float, Boolean)", UC::MakeFloat(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Float, String)", UC::MakeFloat(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitAnd, "BitAnd(Complex, Error)", UC::MakeComplex(1, 2), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Complex, Boolean)", UC::MakeComplex(1, 2), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Complex, String)", UC::MakeComplex(1, 2), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAnd, "BitAnd(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeError());
BINARY_TEST(BitAnd, "BitAnd(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitAndNot, "BitAndNot(Error, Error)", UC::MakeError(), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Error, Boolean)", UC::MakeError(), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Error, String)", UC::MakeError(), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Error, Integer)", UC::MakeError(), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Error, Rune)", UC::MakeError(), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Error, Float)", UC::MakeError(), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Error, Complex)", UC::MakeError(), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitAndNot, "BitAndNot(Boolean, Error)", UC::MakeBoolean(true), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Boolean, String)", UC::MakeBoolean(true), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Boolean, Integer)", UC::MakeBoolean(true), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Boolean, Rune)", UC::MakeBoolean(true), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Boolean, Float)", UC::MakeBoolean(true), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Boolean, Complex)", UC::MakeBoolean(true), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitAndNot, "BitAndNot(String, Error)", UC::MakeString("hello"), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(String, Boolean)", UC::MakeString("hello"), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(String, Integer)", UC::MakeString("hello"), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(String, Rune)", UC::MakeString("hello"), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(String, Float)", UC::MakeString("hello"), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(String, Complex)", UC::MakeString("hello"), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitAndNot, "BitAndNot(Integer, Error)", UC::MakeInteger(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Integer, Boolean)", UC::MakeInteger(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Integer, String)", UC::MakeInteger(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(2));
BINARY_TEST(BitAndNot, "BitAndNot(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(2));
BINARY_TEST(BitAndNot, "BitAndNot(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitAndNot, "BitAndNot(Rune, Error)", UC::MakeRune(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Rune, Boolean)", UC::MakeRune(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Rune, String)", UC::MakeRune(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(2));
BINARY_TEST(BitAndNot, "BitAndNot(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(2));
BINARY_TEST(BitAndNot, "BitAndNot(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitAndNot, "BitAndNot(Float, Error)", UC::MakeFloat(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Float, Boolean)", UC::MakeFloat(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Float, String)", UC::MakeFloat(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitAndNot, "BitAndNot(Complex, Error)", UC::MakeComplex(1, 2), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Complex, Boolean)", UC::MakeComplex(1, 2), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Complex, String)", UC::MakeComplex(1, 2), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitAndNot, "BitAndNot(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeError());
BINARY_TEST(BitAndNot, "BitAndNot(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(Add, "Add(Error, Error)", UC::MakeError(), UC::MakeError(), UC::MakeError())
BINARY_TEST(Add, "Add(Error, Boolean)", UC::MakeError(), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Add, "Add(Error, String)", UC::MakeError(), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Add, "Add(Error, Integer)", UC::MakeError(), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Add, "Add(Error, Rune)", UC::MakeError(), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Add, "Add(Error, Float)", UC::MakeError(), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Add, "Add(Error, Complex)", UC::MakeError(), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Add, "Add(Boolean, Error)", UC::MakeBoolean(true), UC::MakeError(), UC::MakeError())
BINARY_TEST(Add, "Add(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Add, "Add(Boolean, String)", UC::MakeBoolean(true), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Add, "Add(Boolean, Integer)", UC::MakeBoolean(true), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Add, "Add(Boolean, Rune)", UC::MakeBoolean(true), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Add, "Add(Boolean, Float)", UC::MakeBoolean(true), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Add, "Add(Boolean, Complex)", UC::MakeBoolean(true), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Add, "Add(String, Error)", UC::MakeString("hello"), UC::MakeError(), UC::MakeError())
BINARY_TEST(Add, "Add(String, Boolean)", UC::MakeString("hello"), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Add, "Add(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeString("hellohello"))
BINARY_TEST(Add, "Add(String, Integer)", UC::MakeString("hello"), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Add, "Add(String, Rune)", UC::MakeString("hello"), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Add, "Add(String, Float)", UC::MakeString("hello"), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Add, "Add(String, Complex)", UC::MakeString("hello"), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Add, "Add(Integer, Error)", UC::MakeInteger(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Add, "Add(Integer, Boolean)", UC::MakeInteger(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Add, "Add(Integer, String)", UC::MakeInteger(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Add, "Add(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(4));
BINARY_TEST(Add, "Add(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(4));
BINARY_TEST(Add, "Add(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Add, "Add(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, 1));

BINARY_TEST(Add, "Add(Rune, Error)", UC::MakeRune(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Add, "Add(Rune, Boolean)", UC::MakeRune(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Add, "Add(Rune, String)", UC::MakeRune(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Add, "Add(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(4));
BINARY_TEST(Add, "Add(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(4));
BINARY_TEST(Add, "Add(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Add, "Add(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, 1));

BINARY_TEST(Add, "Add(Float, Error)", UC::MakeFloat(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Add, "Add(Float, Boolean)", UC::MakeFloat(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Add, "Add(Float, String)", UC::MakeFloat(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Add, "Add(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeFloat(6));
BINARY_TEST(Add, "Add(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeFloat(6));
BINARY_TEST(Add, "Add(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Add, "Add(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, 1));

BINARY_TEST(Add, "Add(Complex, Error)", UC::MakeComplex(1, 2), UC::MakeError(), UC::MakeError())
BINARY_TEST(Add, "Add(Complex, Boolean)", UC::MakeComplex(1, 2), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Add, "Add(Complex, String)", UC::MakeComplex(1, 2), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Add, "Add(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeComplex(2, 0));
BINARY_TEST(Add, "Add(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeComplex(2, 0));
BINARY_TEST(Add, "Add(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeComplex(2, 0));
BINARY_TEST(Add, "Add(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeComplex(1, 1));

BINARY_TEST(Subtract, "Subtract(Error, Error)", UC::MakeError(), UC::MakeError(), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Error, Boolean)", UC::MakeError(), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Error, String)", UC::MakeError(), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Error, Integer)", UC::MakeError(), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Error, Rune)", UC::MakeError(), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Error, Float)", UC::MakeError(), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Error, Complex)", UC::MakeError(), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Subtract, "Subtract(Boolean, Error)", UC::MakeBoolean(true), UC::MakeError(), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Boolean, String)", UC::MakeBoolean(true), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Boolean, Integer)", UC::MakeBoolean(true), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Boolean, Rune)", UC::MakeBoolean(true), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Boolean, Float)", UC::MakeBoolean(true), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Boolean, Complex)", UC::MakeBoolean(true), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Subtract, "Subtract(String, Error)", UC::MakeString("hello"), UC::MakeError(), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(String, Boolean)", UC::MakeString("hello"), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(String, Integer)", UC::MakeString("hello"), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(String, Rune)", UC::MakeString("hello"), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(String, Float)", UC::MakeString("hello"), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(String, Complex)", UC::MakeString("hello"), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Subtract, "Subtract(Integer, Error)", UC::MakeInteger(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Integer, Boolean)", UC::MakeInteger(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Integer, String)", UC::MakeInteger(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(2));
BINARY_TEST(Subtract, "Subtract(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(2));
BINARY_TEST(Subtract, "Subtract(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeFloat(1.5));
BINARY_TEST(Subtract, "Subtract(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, -1));

BINARY_TEST(Subtract, "Subtract(Rune, Error)", UC::MakeRune(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Rune, Boolean)", UC::MakeRune(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Rune, String)", UC::MakeRune(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(2));
BINARY_TEST(Subtract, "Subtract(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(2));
BINARY_TEST(Subtract, "Subtract(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeFloat(1.5));
BINARY_TEST(Subtract, "Subtract(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, -1));

BINARY_TEST(Subtract, "Subtract(Float, Error)", UC::MakeFloat(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Float, Boolean)", UC::MakeFloat(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Float, String)", UC::MakeFloat(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeFloat(0));
BINARY_TEST(Subtract, "Subtract(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeFloat(0));
BINARY_TEST(Subtract, "Subtract(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeFloat(1.5));
BINARY_TEST(Subtract, "Subtract(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, -1));

BINARY_TEST(Subtract, "Subtract(Complex, Error)", UC::MakeComplex(1, 2), UC::MakeError(), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Complex, Boolean)", UC::MakeComplex(1, 2), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Subtract, "Subtract(Complex, String)", UC::MakeComplex(1, 2), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Subtract, "Subtract(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeComplex(0, 0));
BINARY_TEST(Subtract, "Subtract(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeComplex(0, 0));
BINARY_TEST(Subtract, "Subtract(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeComplex(0, 0));
BINARY_TEST(Subtract, "Subtract(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeComplex(1, -1));

BINARY_TEST(BitOr, "BitOr(Error, Error)", UC::MakeError(), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Error, Boolean)", UC::MakeError(), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Error, String)", UC::MakeError(), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Error, Integer)", UC::MakeError(), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Error, Rune)", UC::MakeError(), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Error, Float)", UC::MakeError(), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Error, Complex)", UC::MakeError(), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitOr, "BitOr(Boolean, Error)", UC::MakeBoolean(true), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Boolean, String)", UC::MakeBoolean(true), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Boolean, Integer)", UC::MakeBoolean(true), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Boolean, Rune)", UC::MakeBoolean(true), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Boolean, Float)", UC::MakeBoolean(true), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Boolean, Complex)", UC::MakeBoolean(true), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitOr, "BitOr(String, Error)", UC::MakeString("hello"), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(String, Boolean)", UC::MakeString("hello"), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(String, Integer)", UC::MakeString("hello"), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(String, Rune)", UC::MakeString("hello"), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(String, Float)", UC::MakeString("hello"), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(String, Complex)", UC::MakeString("hello"), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitOr, "BitOr(Integer, Error)", UC::MakeInteger(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Integer, Boolean)", UC::MakeInteger(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Integer, String)", UC::MakeInteger(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(3));
BINARY_TEST(BitOr, "BitOr(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(3));
BINARY_TEST(BitOr, "BitOr(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitOr, "BitOr(Rune, Error)", UC::MakeRune(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Rune, Boolean)", UC::MakeRune(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Rune, String)", UC::MakeRune(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(3));
BINARY_TEST(BitOr, "BitOr(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(3));
BINARY_TEST(BitOr, "BitOr(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitOr, "BitOr(Float, Error)", UC::MakeFloat(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Float, Boolean)", UC::MakeFloat(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Float, String)", UC::MakeFloat(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitOr, "BitOr(Complex, Error)", UC::MakeComplex(1, 2), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Complex, Boolean)", UC::MakeComplex(1, 2), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Complex, String)", UC::MakeComplex(1, 2), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitOr, "BitOr(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeError());
BINARY_TEST(BitOr, "BitOr(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitXor, "BitXor(Error, Error)", UC::MakeError(), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Error, Boolean)", UC::MakeError(), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Error, String)", UC::MakeError(), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Error, Integer)", UC::MakeError(), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Error, Rune)", UC::MakeError(), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Error, Float)", UC::MakeError(), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Error, Complex)", UC::MakeError(), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitXor, "BitXor(Boolean, Error)", UC::MakeBoolean(true), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Boolean, String)", UC::MakeBoolean(true), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Boolean, Integer)", UC::MakeBoolean(true), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Boolean, Rune)", UC::MakeBoolean(true), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Boolean, Float)", UC::MakeBoolean(true), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Boolean, Complex)", UC::MakeBoolean(true), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitXor, "BitXor(String, Error)", UC::MakeString("hello"), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(String, Boolean)", UC::MakeString("hello"), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(String, Integer)", UC::MakeString("hello"), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(String, Rune)", UC::MakeString("hello"), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(String, Float)", UC::MakeString("hello"), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(String, Complex)", UC::MakeString("hello"), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(BitXor, "BitXor(Integer, Error)", UC::MakeInteger(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Integer, Boolean)", UC::MakeInteger(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Integer, String)", UC::MakeInteger(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(2));
BINARY_TEST(BitXor, "BitXor(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(2));
BINARY_TEST(BitXor, "BitXor(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitXor, "BitXor(Rune, Error)", UC::MakeRune(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Rune, Boolean)", UC::MakeRune(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Rune, String)", UC::MakeRune(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(2));
BINARY_TEST(BitXor, "BitXor(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(2));
BINARY_TEST(BitXor, "BitXor(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitXor, "BitXor(Float, Error)", UC::MakeFloat(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Float, Boolean)", UC::MakeFloat(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Float, String)", UC::MakeFloat(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(BitXor, "BitXor(Complex, Error)", UC::MakeComplex(1, 2), UC::MakeError(), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Complex, Boolean)", UC::MakeComplex(1, 2), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Complex, String)", UC::MakeComplex(1, 2), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(BitXor, "BitXor(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeError());
BINARY_TEST(BitXor, "BitXor(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeError());

BINARY_TEST(Equal, "Equal(Error, Error)", UC::MakeError(), UC::MakeError(), UC::MakeError())
BINARY_TEST(Equal, "Equal(Error, Boolean)", UC::MakeError(), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Equal, "Equal(Error, String)", UC::MakeError(), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Equal, "Equal(Error, Integer)", UC::MakeError(), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Equal, "Equal(Error, Rune)", UC::MakeError(), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Equal, "Equal(Error, Float)", UC::MakeError(), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Equal, "Equal(Error, Complex)", UC::MakeError(), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Equal, "Equal(Boolean, Error)", UC::MakeBoolean(true), UC::MakeError(), UC::MakeError())
BINARY_TEST(Equal, "Equal(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Boolean, String)", UC::MakeBoolean(true), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Equal, "Equal(Boolean, Integer)", UC::MakeBoolean(true), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Equal, "Equal(Boolean, Rune)", UC::MakeBoolean(true), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Equal, "Equal(Boolean, Float)", UC::MakeBoolean(true), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Equal, "Equal(Boolean, Complex)", UC::MakeBoolean(true), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Equal, "Equal(String, Error)", UC::MakeString("hello"), UC::MakeError(), UC::MakeError())
BINARY_TEST(Equal, "Equal(String, Boolean)", UC::MakeString("hello"), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Equal, "Equal(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeBoolean(true))
BINARY_TEST(Equal, "Equal(String, Integer)", UC::MakeString("hello"), UC::MakeInteger(3), UC::MakeError());
BINARY_TEST(Equal, "Equal(String, Rune)", UC::MakeString("hello"), UC::MakeRune(3), UC::MakeError());
BINARY_TEST(Equal, "Equal(String, Float)", UC::MakeString("hello"), UC::MakeFloat(1.5), UC::MakeError());
BINARY_TEST(Equal, "Equal(String, Complex)", UC::MakeString("hello"), UC::MakeComplex(1, 2), UC::MakeError());

BINARY_TEST(Equal, "Equal(Integer, Error)", UC::MakeInteger(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Equal, "Equal(Integer, Boolean)", UC::MakeInteger(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Equal, "Equal(Integer, String)", UC::MakeInteger(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Equal, "Equal(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeBoolean(false));

BINARY_TEST(Equal, "Equal(Rune, Error)", UC::MakeRune(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Equal, "Equal(Rune, Boolean)", UC::MakeRune(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Equal, "Equal(Rune, String)", UC::MakeRune(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Equal, "Equal(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeBoolean(false));

BINARY_TEST(Equal, "Equal(Float, Error)", UC::MakeFloat(3), UC::MakeError(), UC::MakeError())
BINARY_TEST(Equal, "Equal(Float, Boolean)", UC::MakeFloat(3), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Equal, "Equal(Float, String)", UC::MakeFloat(3), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Equal, "Equal(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeBoolean(false));

BINARY_TEST(Equal, "Equal(Complex, Error)", UC::MakeComplex(1, 2), UC::MakeError(), UC::MakeError())
BINARY_TEST(Equal, "Equal(Complex, Boolean)", UC::MakeComplex(1, 2), UC::MakeBoolean(true), UC::MakeError());
BINARY_TEST(Equal, "Equal(Complex, String)", UC::MakeComplex(1, 2), UC::MakeString("hello"), UC::MakeError())
BINARY_TEST(Equal, "Equal(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeBoolean(false));

TEST_CASE("UntypedConstant operator<<(Error)") {
  std::stringstream ss;
  UC x = UC::MakeError();
  ss << x;
  REQUIRE(ss.str() == "<error>");
}

TEST_CASE("UntypedConstant operator<<(Boolean)") {
  {
    std::stringstream ss;
    UC x = UC::MakeBoolean(true);
    ss << x;
    REQUIRE(ss.str() == "true");
  }
  {
    std::stringstream ss;
    UC x = UC::MakeBoolean(false);
    ss << x;
    REQUIRE(ss.str() == "false");
  }
}

TEST_CASE("UntypedConstant operator<<(Integer)") {
  std::stringstream ss;
  UC x = UC::MakeInteger(34);
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("UntypedConstant operator<<(Rune)") {
  {
    std::stringstream ss;
    UC x = UC::MakeRune('j');
    ss << x;
    REQUIRE(ss.str() == "'j'");
  }
  {
    std::stringstream ss;
    UC x = UC::MakeRune(INVALID);
    ss << x;
    REQUIRE(ss.str() == "1114112");
  }
  {
    std::stringstream ss;
    UC x = UC::MakeRune(SURROGATE_FIRST);
    ss << x;
    REQUIRE(ss.str() == "55296");
  }
}

TEST_CASE("UntypedConstant operator<<(Float)") {
  std::stringstream ss;
  UC x = UC::MakeFloat(34.5);
  ss << x;
  REQUIRE(ss.str() == "34.5");
}

TEST_CASE("UntypedConstant operator<<(Complex)") {
  std::stringstream ss;
  UC x = UC::MakeComplex(1, 2);
  ss << x;
  REQUIRE(ss.str() == "1+2i");
}

TEST_CASE("UntypedConstant operator<<(String)") {
  std::stringstream ss;
  UC x = UC::MakeString("hello");
  ss << x;
  REQUIRE(ss.str() == "\"hello\"");
}

}  // namespace test
}  // namespace rcgo
