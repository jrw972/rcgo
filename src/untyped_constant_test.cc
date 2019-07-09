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

UNARY_TEST(Posate, "Posate(Integer)", UC::MakeInteger(3), UC::MakeInteger(3))
UNARY_TEST(Posate, "Posate(Rune)", UC::MakeRune(3), UC::MakeRune(3))
UNARY_TEST(Posate, "Posate(Float)", UC::MakeFloat(1.5), UC::MakeFloat(1.5))
UNARY_TEST(Posate, "Posate(Complex)", UC::MakeComplex(1, 2), UC::MakeComplex(1,2 ))

UNARY_TEST(Negate, "Negate(Integer)", UC::MakeInteger(3), UC::MakeInteger(-3))
UNARY_TEST(Negate, "Negate(Rune)", UC::MakeRune(3), UC::MakeRune(-3))
UNARY_TEST(Negate, "Negate(Float)", UC::MakeFloat(1.5), UC::MakeFloat(-1.5))
UNARY_TEST(Negate, "Negate(Complex)", UC::MakeComplex(1, 2), UC::MakeComplex(-1,-2 ))

UNARY_TEST(LogicNot, "LogicNot(Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(false))

UNARY_TEST(BitNot, "BitNot(Integer)", UC::MakeInteger(3), UC::MakeInteger(-4));
UNARY_TEST(BitNot, "BitNot(Rune)", UC::MakeRune(3), UC::MakeRune(-4));

BINARY_TEST(Multiply, "Multiply(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(3), UC::MakeInteger(9));
BINARY_TEST(Multiply, "Multiply(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(3), UC::MakeRune(9));
BINARY_TEST(Multiply, "Multiply(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Multiply, "Multiply(Integer, Complex)", UC::MakeInteger(3), UC::MakeComplex(1, 2), UC::MakeComplex(3, 6));

BINARY_TEST(Multiply, "Multiply(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(3), UC::MakeRune(9));
BINARY_TEST(Multiply, "Multiply(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(3), UC::MakeRune(9));
BINARY_TEST(Multiply, "Multiply(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Multiply, "Multiply(Rune, Complex)", UC::MakeRune(3), UC::MakeComplex(1, 2), UC::MakeComplex(3, 6));

BINARY_TEST(Multiply, "Multiply(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeFloat(9));
BINARY_TEST(Multiply, "Multiply(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeFloat(9));
BINARY_TEST(Multiply, "Multiply(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Multiply, "Multiply(Float, Complex)", UC::MakeFloat(3), UC::MakeComplex(1, 2), UC::MakeComplex(3, 6));

BINARY_TEST(Multiply, "Multiply(Complex, Integer)", UC::MakeComplex(1, 2), UC::MakeInteger(3), UC::MakeComplex(3, 6));
BINARY_TEST(Multiply, "Multiply(Complex, Rune)", UC::MakeComplex(1, 2), UC::MakeRune(3), UC::MakeComplex(3, 6));
BINARY_TEST(Multiply, "Multiply(Complex, Float)", UC::MakeComplex(1, 2), UC::MakeFloat(1.5), UC::MakeComplex(1.5, 3));
BINARY_TEST(Multiply, "Multiply(Complex, Complex)", UC::MakeComplex(1, 2), UC::MakeComplex(1, 2), UC::MakeComplex(-3, 4));

BINARY_TEST(Divide, "Divide(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(3), UC::MakeInteger(1));
BINARY_TEST(Divide, "Divide(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(3), UC::MakeRune(1));
BINARY_TEST(Divide, "Divide(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeFloat(2));
BINARY_TEST(Divide, "Divide(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeComplex(0, -1));

BINARY_TEST(Divide, "Divide(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(3), UC::MakeRune(1));
BINARY_TEST(Divide, "Divide(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(3), UC::MakeRune(1));
BINARY_TEST(Divide, "Divide(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeFloat(2));
BINARY_TEST(Divide, "Divide(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeComplex(0, -1));

BINARY_TEST(Divide, "Divide(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeFloat(1));
BINARY_TEST(Divide, "Divide(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeFloat(1));
BINARY_TEST(Divide, "Divide(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeFloat(2));
BINARY_TEST(Divide, "Divide(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeComplex(0, -1));

BINARY_TEST(Divide, "Divide(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeComplex(1, 0));
BINARY_TEST(Divide, "Divide(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeComplex(1, 0));
BINARY_TEST(Divide, "Divide(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeComplex(1, 0));
BINARY_TEST(Divide, "Divide(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeComplex(0, -1));

BINARY_TEST(Modulo, "Modulo(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(3), UC::MakeInteger(0));
BINARY_TEST(Modulo, "Modulo(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(3), UC::MakeRune(0));

BINARY_TEST(Modulo, "Modulo(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(3), UC::MakeRune(0));
BINARY_TEST(Modulo, "Modulo(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(3), UC::MakeRune(0));

BINARY_TEST(LeftShift, "LeftShift(Integer, 1)", UC::MakeInteger(3), 1, UC::MakeInteger(6))
BINARY_TEST(LeftShift, "LeftShift(Rune, 1)", UC::MakeRune(3), 1, UC::MakeRune(6))

BINARY_TEST(RightShift, "RightShift(Integer, 1)", UC::MakeInteger(3), 1, UC::MakeInteger(1))
BINARY_TEST(RightShift, "RightShift(Rune, 1)", UC::MakeRune(3), 1, UC::MakeRune(1))

BINARY_TEST(BitAnd, "BitAnd(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(1));
BINARY_TEST(BitAnd, "BitAnd(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(1));

BINARY_TEST(BitAnd, "BitAnd(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(1));
BINARY_TEST(BitAnd, "BitAnd(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(1));

BINARY_TEST(BitAndNot, "BitAndNot(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(2));
BINARY_TEST(BitAndNot, "BitAndNot(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(2));

BINARY_TEST(BitAndNot, "BitAndNot(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(2));
BINARY_TEST(BitAndNot, "BitAndNot(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(2));

BINARY_TEST(Add, "Add(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeString("hellohello"))

BINARY_TEST(Add, "Add(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(4));
BINARY_TEST(Add, "Add(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(4));
BINARY_TEST(Add, "Add(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Add, "Add(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, 1));

BINARY_TEST(Add, "Add(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(4));
BINARY_TEST(Add, "Add(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(4));
BINARY_TEST(Add, "Add(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Add, "Add(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, 1));

BINARY_TEST(Add, "Add(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeFloat(6));
BINARY_TEST(Add, "Add(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeFloat(6));
BINARY_TEST(Add, "Add(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeFloat(4.5));
BINARY_TEST(Add, "Add(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, 1));

BINARY_TEST(Add, "Add(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeComplex(2, 0));
BINARY_TEST(Add, "Add(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeComplex(2, 0));
BINARY_TEST(Add, "Add(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeComplex(2, 0));
BINARY_TEST(Add, "Add(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeComplex(1, 1));

BINARY_TEST(Subtract, "Subtract(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(2));
BINARY_TEST(Subtract, "Subtract(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(2));
BINARY_TEST(Subtract, "Subtract(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeFloat(1.5));
BINARY_TEST(Subtract, "Subtract(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, -1));

BINARY_TEST(Subtract, "Subtract(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(2));
BINARY_TEST(Subtract, "Subtract(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(2));
BINARY_TEST(Subtract, "Subtract(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeFloat(1.5));
BINARY_TEST(Subtract, "Subtract(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, -1));

BINARY_TEST(Subtract, "Subtract(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeFloat(0));
BINARY_TEST(Subtract, "Subtract(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeFloat(0));
BINARY_TEST(Subtract, "Subtract(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeFloat(1.5));
BINARY_TEST(Subtract, "Subtract(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeComplex(1, -1));

BINARY_TEST(Subtract, "Subtract(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeComplex(0, 0));
BINARY_TEST(Subtract, "Subtract(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeComplex(0, 0));
BINARY_TEST(Subtract, "Subtract(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeComplex(0, 0));
BINARY_TEST(Subtract, "Subtract(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeComplex(1, -1));

BINARY_TEST(BitOr, "BitOr(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(3));
BINARY_TEST(BitOr, "BitOr(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(3));

BINARY_TEST(BitOr, "BitOr(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(3));
BINARY_TEST(BitOr, "BitOr(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(3));

BINARY_TEST(BitXor, "BitXor(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeInteger(2));
BINARY_TEST(BitXor, "BitXor(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeRune(2));

BINARY_TEST(BitXor, "BitXor(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeRune(2));
BINARY_TEST(BitXor, "BitXor(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeRune(2));

BINARY_TEST(Equal, "Equal(Boolean, Boolean)", UC::MakeBoolean(true), UC::MakeBoolean(true), UC::MakeBoolean(true));

BINARY_TEST(Equal, "Equal(String, String)", UC::MakeString("hello"), UC::MakeString("hello"), UC::MakeBoolean(true))

BINARY_TEST(Equal, "Equal(Integer, Integer)", UC::MakeInteger(3), UC::MakeInteger(1), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Integer, Rune)", UC::MakeInteger(3), UC::MakeRune(1), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Integer, Float)", UC::MakeInteger(3), UC::MakeFloat(1.5), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Integer, Complex)", UC::MakeInteger(1), UC::MakeComplex(0, 1), UC::MakeBoolean(false));

BINARY_TEST(Equal, "Equal(Rune, Integer)", UC::MakeRune(3), UC::MakeInteger(1), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Rune, Rune)", UC::MakeRune(3), UC::MakeRune(1), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Rune, Float)", UC::MakeRune(3), UC::MakeFloat(1.5), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Rune, Complex)", UC::MakeRune(1), UC::MakeComplex(0, 1), UC::MakeBoolean(false));

BINARY_TEST(Equal, "Equal(Float, Integer)", UC::MakeFloat(3), UC::MakeInteger(3), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Float, Rune)", UC::MakeFloat(3), UC::MakeRune(3), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Float, Float)", UC::MakeFloat(3), UC::MakeFloat(1.5), UC::MakeBoolean(false));
BINARY_TEST(Equal, "Equal(Float, Complex)", UC::MakeFloat(1), UC::MakeComplex(0, 1), UC::MakeBoolean(false));

BINARY_TEST(Equal, "Equal(Complex, Integer)", UC::MakeComplex(1, 0), UC::MakeInteger(1), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Complex, Rune)", UC::MakeComplex(1, 0), UC::MakeRune(1), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Complex, Float)", UC::MakeComplex(1, 0), UC::MakeFloat(1), UC::MakeBoolean(true));
BINARY_TEST(Equal, "Equal(Complex, Complex)", UC::MakeComplex(1, 0), UC::MakeComplex(0, 1), UC::MakeBoolean(false));

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
