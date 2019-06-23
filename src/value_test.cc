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

typedef value::Value Value;
typedef value::UntypedConstant UntypedConstant;

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

TEST_CASE("value::MakeError()") {
  Value v = Value::MakeError();
  REQUIRE(v.kind() == Value::kError);
  REQUIRE(v.IsError());
}

TEST_CASE("value::MakeUntypedConstant()") {
  Value v = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  REQUIRE(v.kind() == Value::kUntypedConstant);
  REQUIRE(v.untyped_constant() == UntypedConstant::MakeBoolean(true));
}

// TEST_CASE("value::ConvertTo(Error -> bool)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeError();
//   REQUIRE(!x.ConvertTo(&type::Bool::instance));
//   REQUIRE(x.IsError());
// }

// TEST_CASE("value::ConvertTo(Boolean -> bool)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
//   REQUIRE(x.ConvertTo(&type::Bool::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Bool::instance);
//   REQUIRE(x.bool_value() == true);
// }

// TEST_CASE("value::ConvertTo(Integer -> complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Complex64::instance);
//   REQUIRE(x.complex64_value() == value::complex64_t(34, 0));
// }

// TEST_CASE("value::ConvertTo(Integer -> complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Complex128::instance);
//   REQUIRE(x.complex128_value() == value::complex128_t(34, 0));
// }

// TEST_CASE("value::ConvertTo(Integer -> float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Float32::instance);
//   REQUIRE(x.float32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Float64::instance);
//   REQUIRE(x.float64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int::instance);
//   REQUIRE(x.int_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int8::instance);
//   REQUIRE(x.int8_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int16::instance);
//   REQUIRE(x.int16_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int32::instance);
//   REQUIRE(x.int32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int64::instance);
//   REQUIRE(x.int64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint::instance);
//   REQUIRE(x.uint_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint8::instance);
//   REQUIRE(x.uint8_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint16::instance);
//   REQUIRE(x.uint16_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint32::instance);
//   REQUIRE(x.uint32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint64::instance);
//   REQUIRE(x.uint64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Integer -> uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uintptr::instance);
//   REQUIRE(x.uintptr_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float 0 -> complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(0));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Complex64::instance);
//   REQUIRE(x.complex64_value() == value::complex64_t(0, 0));
// }

// TEST_CASE("value::ConvertTo(Float -> complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Complex64::instance);
//   REQUIRE(x.complex64_value() == value::complex64_t(34, 0));
// }

// TEST_CASE("value::ConvertTo(Float -> complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Complex128::instance);
//   REQUIRE(x.complex128_value() == value::complex128_t(34, 0));
// }

// TEST_CASE("value::ConvertTo(Float -> float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Float32::instance);
//   REQUIRE(x.float32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Float64::instance);
//   REQUIRE(x.float64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int::instance);
//   REQUIRE(x.int_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int8::instance);
//   REQUIRE(x.int8_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int16::instance);
//   REQUIRE(x.int16_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int32::instance);
//   REQUIRE(x.int32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int64::instance);
//   REQUIRE(x.int64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint::instance);
//   REQUIRE(x.uint_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint8::instance);
//   REQUIRE(x.uint8_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint16::instance);
//   REQUIRE(x.uint16_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint32::instance);
//   REQUIRE(x.uint32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint64::instance);
//   REQUIRE(x.uint64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Float -> uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uintptr::instance);
//   REQUIRE(x.uintptr_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Complex64::instance);
//   REQUIRE(x.complex64_value() == value::complex64_t(1, 2));
// }

// TEST_CASE("value::ConvertTo(Complex -> complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Complex128::instance);
//   REQUIRE(x.complex128_value() == value::complex128_t(1, 2));
// }

// TEST_CASE("value::ConvertTo(Complex -> float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Float32::instance);
//   REQUIRE(x.float32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Float64::instance);
//   REQUIRE(x.float64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int::instance);
//   REQUIRE(x.int_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int8::instance);
//   REQUIRE(x.int8_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int16::instance);
//   REQUIRE(x.int16_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int32::instance);
//   REQUIRE(x.int32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int64::instance);
//   REQUIRE(x.int64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint::instance);
//   REQUIRE(x.uint_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint8::instance);
//   REQUIRE(x.uint8_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint16::instance);
//   REQUIRE(x.uint16_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint32::instance);
//   REQUIRE(x.uint32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint64::instance);
//   REQUIRE(x.uint64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Complex -> uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(34, 0));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uintptr::instance);
//   REQUIRE(x.uintptr_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Complex64::instance);
//   REQUIRE(x.complex64_value() == value::complex64_t(34, 0));
// }

// TEST_CASE("value::ConvertTo(Rune -> complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Complex128::instance);
//   REQUIRE(x.complex128_value() == value::complex128_t(34, 0));
// }

// TEST_CASE("value::ConvertTo(Rune -> float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Float32::instance);
//   REQUIRE(x.float32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Float64::instance);
//   REQUIRE(x.float64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int::instance);
//   REQUIRE(x.int_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int8::instance);
//   REQUIRE(x.int8_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int16::instance);
//   REQUIRE(x.int16_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int32::instance);
//   REQUIRE(x.int32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Int64::instance);
//   REQUIRE(x.int64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint::instance);
//   REQUIRE(x.uint_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint8::instance);
//   REQUIRE(x.uint8_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint16::instance);
//   REQUIRE(x.uint16_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint32::instance);
//   REQUIRE(x.uint32_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uint64::instance);
//   REQUIRE(x.uint64_value() == 34);
// }

// TEST_CASE("value::ConvertTo(Rune -> uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(34));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::Uintptr::instance);
//   REQUIRE(x.uintptr_value() == 34);
// }

// TEST_CASE("value::ConvertTo(String -> string)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello"));
//   REQUIRE(x.ConvertTo(&type::String::instance));
//   REQUIRE(x.kind() == Value::kTypedConstant);
//   REQUIRE(x.type() == &type::String::instance);
//   REQUIRE(x.string_value() == "hello");
// }

TEST_CASE("Value::Posate(Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::Posate(Location(), &x, &el);
  REQUIRE(y.IsError());
}

TEST_CASE("Value::Posate(Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::Posate(Location(), &x, &el);
  REQUIRE(el.size() == 1);
  REQUIRE(y.IsError());
}

TEST_CASE("Value::Posate(Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::Posate(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
}

TEST_CASE("Value::Posate(Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value y = Value::Posate(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeRune(3)));
}

TEST_CASE("Value::Posate(Float)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(1.5));
  Value y = Value::Posate(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeFloat(1.5)));
}

TEST_CASE("Value::Posate(Complex)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
  Value y = Value::Posate(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2)));
}

// TEST_CASE("Value::Posate(int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Int8::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Int16::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Int32::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Int64::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Uint8::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Uint16::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Uint32::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Uint64::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Int::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Uint::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Float32::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(z.ConvertTo(&type::Float64::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(z.ConvertTo(&type::Complex64::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Posate(complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   Value y = Value::Posate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(z.ConvertTo(&type::Complex128::instance));
//   REQUIRE(y == z);
// }

TEST_CASE("Value::Negate(Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::Negate(Location(), &x, &el);
  REQUIRE(y.IsError());
}

TEST_CASE("Value::Negate(Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::Negate(Location(), &x, &el);
  REQUIRE(el.size() == 1);
  REQUIRE(y.IsError());
}

TEST_CASE("Value::Negate(Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::Negate(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-3)));
}

TEST_CASE("Value::Negate(Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value y = Value::Negate(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeRune(-3)));
}

TEST_CASE("Value::Negate(Float)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(1.5));
  Value y = Value::Negate(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeFloat(-1.5)));
}

TEST_CASE("Value::Negate(Complex)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
  Value y = Value::Negate(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeComplex(-1, -2)));
}

// TEST_CASE("Value::Negate(int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-3));
//   REQUIRE(z.ConvertTo(&type::Int8::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-3));
//   REQUIRE(z.ConvertTo(&type::Int16::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-3));
//   REQUIRE(z.ConvertTo(&type::Int32::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-3));
//   REQUIRE(z.ConvertTo(&type::Int64::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(253));
//   REQUIRE(z.ConvertTo(&type::Uint8::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(65533));
//   REQUIRE(z.ConvertTo(&type::Uint16::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4294967293));
//   REQUIRE(z.ConvertTo(&type::Uint32::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(18446744073709551613ul));
//   REQUIRE(z.ConvertTo(&type::Uint64::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-3));
//   REQUIRE(z.ConvertTo(&type::Int::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4294967293));
//   REQUIRE(z.ConvertTo(&type::Uint::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4294967293));
//   REQUIRE(z.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-3));
//   REQUIRE(z.ConvertTo(&type::Float32::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-3));
//   REQUIRE(z.ConvertTo(&type::Float64::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(-1, -2));
//   REQUIRE(z.ConvertTo(&type::Complex64::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::Negate(complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   Value y = Value::Negate(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(-1, -2));
//   REQUIRE(z.ConvertTo(&type::Complex128::instance));
//   REQUIRE(y == z);
// }

TEST_CASE("Value::LogicNot(Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::LogicNot(Location(), &x, &el);
  REQUIRE(y.IsError());
}

TEST_CASE("Value::LogicNot(Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::LogicNot(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
}

TEST_CASE("Value::LogicNot(Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::LogicNot(Location(), &x, &el);
  REQUIRE(y.IsError());
  REQUIRE(el.size() == 1);
}

// TEST_CASE("Value::LogicNot(bool)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
//   REQUIRE(x.ConvertTo(&type::Bool::instance));
//   Value y = Value::LogicNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false));
//   REQUIRE(z.ConvertTo(&type::Bool::instance));
//   REQUIRE(y == z);
// }

TEST_CASE("Value::BitNot(Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::BitNot(Location(), &x, &el);
  REQUIRE(y.IsError());
}

TEST_CASE("Value::BitNot(Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::BitNot(Location(), &x, &el);
  REQUIRE(y.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::BitNot(Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::BitNot(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4)));
}

TEST_CASE("Value::BitNot(Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value y = Value::BitNot(Location(), &x, &el);
  REQUIRE(y == Value::MakeUntypedConstant(UntypedConstant::MakeRune(-4)));
}

// TEST_CASE("Value::BitNot(int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(z.ConvertTo(&type::Int8::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::BitNot(int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(z.ConvertTo(&type::Int16::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::BitNot(int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(z.ConvertTo(&type::Int32::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::BitNot(int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(z.ConvertTo(&type::Int64::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::BitNot(uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(252));
//   REQUIRE(z.ConvertTo(&type::Uint8::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::BitNot(uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(65532));
//   REQUIRE(z.ConvertTo(&type::Uint16::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::BitNot(uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4294967292));
//   REQUIRE(z.ConvertTo(&type::Uint32::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::BitNot(uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(18446744073709551612ul));
//   REQUIRE(z.ConvertTo(&type::Uint64::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::BitNot(int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(z.ConvertTo(&type::Int::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::BitNot(uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4294967292));
//   REQUIRE(z.ConvertTo(&type::Uint::instance));
//   REQUIRE(y == z);
// }

// TEST_CASE("Value::BitNot(uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::BitNot(Location(), &x, &el);
//   Value z = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4294967292));
//   REQUIRE(z.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(y == z);
// }

TEST_CASE("Value::Multiply(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::Multiply(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Multiply(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::Multiply(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::Multiply(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Multiply(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12)));
}

TEST_CASE("Value::Multiply(Rune, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Multiply(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(12)));
}

TEST_CASE("Value::Multiply(Integer, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(4));
  Value z = Value::Multiply(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(12)));
}

TEST_CASE("Value::Multiply(Float, Float)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(1.5));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(2.25));
  Value z = Value::Multiply(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3.375)));
}

TEST_CASE("Value::Multiply(Complex, Complex)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 4));
  Value z = Value::Multiply(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeComplex(-5, 10)));
}

// TEST_CASE("Value::Multiply(int8, int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int8::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(int16, int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int16::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(int32, int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int32::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(int64, int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int64::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(uint8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(uint16, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(uint32, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(uint64, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(int, int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(uint, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(uintptr, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(float32, float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(4));
//   REQUIRE(y.ConvertTo(&type::Float32::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Float32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(float64, float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(4));
//   REQUIRE(y.ConvertTo(&type::Float64::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(12));
//   REQUIRE(t.ConvertTo(&type::Float64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(complex64, complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 4));
//   REQUIRE(y.ConvertTo(&type::Complex64::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(-5, 10));
//   REQUIRE(t.ConvertTo(&type::Complex64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(complex128, complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 4));
//   REQUIRE(y.ConvertTo(&type::Complex128::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(-5, 10));
//   REQUIRE(t.ConvertTo(&type::Complex128::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Multiply(int8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Multiply(Location(), &x, &y, &el);
//   REQUIRE(z.IsError());
//   REQUIRE(el.size() == 1);
// }

TEST_CASE("Value::Divide(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::Divide(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Divide(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::Divide(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::Divide(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Divide(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0)));
}

TEST_CASE("Value::Divide(Rune, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Divide(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(0)));
}

TEST_CASE("Value::Divide(Integer, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(4));
  Value z = Value::Divide(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(0)));
}

TEST_CASE("Value::Divide(Float, Float)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(1));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(2));
  Value z = Value::Divide(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeFloat(.5)));
}

TEST_CASE("Value::Divide(Complex, Complex)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 0));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(0, 1));
  Value z = Value::Divide(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeComplex(0, -1)));
}

// TEST_CASE("Value::Divide(int8, int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int8::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(int16, int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int16::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(int32, int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int32::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(int64, int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int64::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(uint8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(uint16, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(uint32, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(uint64, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(int, int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(uint, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(uintptr, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(float32, float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(4));
//   REQUIRE(y.ConvertTo(&type::Float32::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(.75));
//   REQUIRE(t.ConvertTo(&type::Float32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(float64, float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(4));
//   REQUIRE(y.ConvertTo(&type::Float64::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(.75));
//   REQUIRE(t.ConvertTo(&type::Float64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(complex64, complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 0));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(0, -1));
//   REQUIRE(y.ConvertTo(&type::Complex64::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(0, 1));
//   REQUIRE(t.ConvertTo(&type::Complex64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(complex128, complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 0));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(0, -1));
//   REQUIRE(y.ConvertTo(&type::Complex128::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(0, 1));
//   REQUIRE(t.ConvertTo(&type::Complex128::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Divide(int8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Divide(Location(), &x, &y, &el);
//   REQUIRE(z.IsError());
//   REQUIRE(el.size() == 1);
// }

TEST_CASE("Value::Divide(Integer, 0)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
  Value z = Value::Divide(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::Modulo(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::Modulo(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Modulo(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::Modulo(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::Modulo(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Modulo(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
}

TEST_CASE("Value::Modulo(Rune, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Modulo(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(3)));
}

TEST_CASE("Value::Modulo(Integer, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(4));
  Value z = Value::Modulo(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(3)));
}

// TEST_CASE("Value::Modulo(int8, int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int8::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(int16, int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int16::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(int32, int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int32::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(int64, int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int64::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(uint8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(uint16, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(uint32, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(uint64, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(int, int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(uint, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(uintptr, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Modulo(int8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Modulo(Location(), &x, &y, &el);
//   REQUIRE(z.IsError());
//   REQUIRE(el.size() == 1);
// }

TEST_CASE("Value::Modulo(Integer, 0)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
  Value z = Value::Modulo(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::LeftShift(Error, Error)") {
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::LeftShift(&x, &y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::LeftShift(Boolean, Integer)") {
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::LeftShift(&x, &y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::LeftShift(Integer, Boolean)") {
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::LeftShift(&x, &y);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::LeftShift(Integer, Integer)") {
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::LeftShift(&x, &y);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
}

TEST_CASE("Value::LeftShift(Integer, Rune)") {
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(4));
  Value z = Value::LeftShift(&x, &y);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
}

TEST_CASE("Value::LeftShift(Integer, Float)") {
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(4));
  Value z = Value::LeftShift(&x, &y);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
}

TEST_CASE("Value::LeftShift(Integer, Complex)") {
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(4, 0));
  Value z = Value::LeftShift(&x, &y);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
}

// TEST_CASE("Value::LeftShift(Integer, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
// }

// TEST_CASE("Value::LeftShift(Integer, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
// }

// TEST_CASE("Value::LeftShift(Integer, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
// }

// TEST_CASE("Value::LeftShift(Integer, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
// }

// TEST_CASE("Value::LeftShift(Integer, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
// }

// TEST_CASE("Value::LeftShift(Integer, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
// }

// TEST_CASE("Value::LeftShift(int8, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::LeftShift(int16, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::LeftShift(int32, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::LeftShift(int64, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::LeftShift(uint8, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::LeftShift(uint16, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::LeftShift(uint32, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::LeftShift(uint64, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::LeftShift(int, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::LeftShift(uint, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::LeftShift(uintptr, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::LeftShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

TEST_CASE("Value::LeftShift(Rune, Integer)") {
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::LeftShift(&x, &y);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
}

TEST_CASE("Value::LeftShift(Float, Integer)") {
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::LeftShift(&x, &y);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
}

TEST_CASE("Value::LeftShift(Complex, Integer)") {
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 0));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::LeftShift(&x, &y);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48)));
}

TEST_CASE("Value::RightShift(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::RightShift(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::RightShift(Boolean, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::RightShift(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::RightShift(Integer, Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::RightShift(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::RightShift(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::RightShift(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
}

TEST_CASE("Value::RightShift(Integer, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(4));
  Value z = Value::RightShift(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
}

TEST_CASE("Value::RightShift(Integer, Float)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(4));
  Value z = Value::RightShift(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
}

TEST_CASE("Value::RightShift(Integer, Complex)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(4, 0));
  Value z = Value::RightShift(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
}

// TEST_CASE("Value::RightShift(Integer, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
// }

// TEST_CASE("Value::RightShift(Integer, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
// }

// TEST_CASE("Value::RightShift(Integer, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
// }

// TEST_CASE("Value::RightShift(Integer, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
// }

// TEST_CASE("Value::RightShift(Integer, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
// }

// TEST_CASE("Value::RightShift(Integer, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
// }

// TEST_CASE("Value::RightShift(int8, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::RightShift(int16, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::RightShift(int32, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::RightShift(int64, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::RightShift(uint8, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::RightShift(uint16, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::RightShift(uint32, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::RightShift(uint64, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::RightShift(int, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::RightShift(uint, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::RightShift(uintptr, Integer)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(48));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   Value z = Value::RightShift(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

TEST_CASE("Value::RightShift(Rune, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(48));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::RightShift(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
}

TEST_CASE("Value::RightShift(Float, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(48));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::RightShift(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
}

TEST_CASE("Value::RightShift(Complex, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(48, 0));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::RightShift(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3)));
}

TEST_CASE("Value::BitAnd(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::BitAnd(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitAnd(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::BitAnd(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::BitAnd(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
  Value z = Value::BitAnd(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8)));
}

TEST_CASE("Value::BitAnd(Rune, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
  Value z = Value::BitAnd(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(8)));
}

TEST_CASE("Value::BitAnd(Integer, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(-4));
  Value z = Value::BitAnd(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(8)));
}

// TEST_CASE("Value::BitAnd(int8, int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int8::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(int16, int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int16::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(int32, int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int32::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(int64, int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int64::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(uint8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFC));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(uint16, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(uint32, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(uint64, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFFFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(int, int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(uint, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(uintptr, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(8));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAnd(int8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFC));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::BitAnd(Location(), &x, &y, &el);
//   REQUIRE(z.IsError());
//   REQUIRE(el.size() == 1);
// }

TEST_CASE("Value::BitAndNot(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::BitAndNot(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitAndNot(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::BitAndNot(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::BitAndNot(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
  Value z = Value::BitAndNot(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2)));
}

TEST_CASE("Value::BitAndNot(Rune, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
  Value z = Value::BitAndNot(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(2)));
}

TEST_CASE("Value::BitAndNot(Integer, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(-4));
  Value z = Value::BitAndNot(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(2)));
}

// TEST_CASE("Value::BitAndNot(int8, int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int8::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(int16, int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int16::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(int32, int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int32::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(int64, int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int64::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(uint8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFC));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(uint16, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(uint32, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(uint64, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFFFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(int, int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(uint, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(uintptr, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(2));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitAndNot(int8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFC));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::BitAndNot(Location(), &x, &y, &el);
//   REQUIRE(z.IsError());
//   REQUIRE(el.size() == 1);
// }

TEST_CASE("Value::Add(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::Add(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Add(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::Add(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::Add(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Add(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7)));
}

TEST_CASE("Value::Add(Rune, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Add(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(7)));
}

TEST_CASE("Value::Add(Integer, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(4));
  Value z = Value::Add(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(7)));
}

TEST_CASE("Value::Add(Float, Float)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(1.5));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(2.25));
  Value z = Value::Add(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3.75)));
}

TEST_CASE("Value::Add(Complex, Complex)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 4));
  Value z = Value::Add(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeComplex(4, 6)));
}

TEST_CASE("Value::Add(String, String)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello "));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeString("world"));
  Value z = Value::Add(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeString("hello world")));
}

// TEST_CASE("Value::Add(int8, int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int8::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(int16, int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int16::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(int32, int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int32::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(int64, int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int64::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(uint8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(uint16, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(uint32, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(uint64, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(int, int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(uint, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(uintptr, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(7));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(float32, float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(4));
//   REQUIRE(y.ConvertTo(&type::Float32::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(7));
//   REQUIRE(t.ConvertTo(&type::Float32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(float64, float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(4));
//   REQUIRE(y.ConvertTo(&type::Float64::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(7));
//   REQUIRE(t.ConvertTo(&type::Float64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(complex64, complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 4));
//   REQUIRE(y.ConvertTo(&type::Complex64::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(4, 6));
//   REQUIRE(t.ConvertTo(&type::Complex64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(complex128, complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 4));
//   REQUIRE(y.ConvertTo(&type::Complex128::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(4, 6));
//   REQUIRE(t.ConvertTo(&type::Complex128::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(string, string)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello "));
//   REQUIRE(x.ConvertTo(&type::String::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeString("world"));
//   REQUIRE(y.ConvertTo(&type::String::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello world"));
//   REQUIRE(t.ConvertTo(&type::String::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Add(int8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Add(Location(), &x, &y, &el);
//   REQUIRE(z.IsError());
//   REQUIRE(el.size() == 1);
// }

TEST_CASE("Value::Subtract(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::Subtract(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Subtract(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::Subtract(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::Subtract(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Subtract(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-1)));
}

TEST_CASE("Value::Subtract(Rune, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Subtract(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(-1)));
}

TEST_CASE("Value::Subtract(Integer, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(4));
  Value z = Value::Subtract(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(-1)));
}

TEST_CASE("Value::Subtract(Float, Float)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(1.5));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(2.25));
  Value z = Value::Subtract(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeFloat(-0.75)));
}

TEST_CASE("Value::Subtract(Complex, Complex)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 4));
  Value z = Value::Subtract(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeComplex(-2, -2)));
}

// TEST_CASE("Value::Subtract(int8, int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int8::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-1));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(int16, int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int16::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-1));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(int32, int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int32::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-1));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(int64, int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int64::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-1));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(uint8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(255));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(uint16, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(65535));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(uint32, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4294967295));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(uint64, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(18446744073709551615UL));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(int, int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Int::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-1));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(uint, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4294967295));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(uintptr, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4294967295));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(float32, float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(4));
//   REQUIRE(y.ConvertTo(&type::Float32::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(-1));
//   REQUIRE(t.ConvertTo(&type::Float32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(float64, float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(4));
//   REQUIRE(y.ConvertTo(&type::Float64::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(-1));
//   REQUIRE(t.ConvertTo(&type::Float64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(complex64, complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 4));
//   REQUIRE(y.ConvertTo(&type::Complex64::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(-2, -2));
//   REQUIRE(t.ConvertTo(&type::Complex64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(complex128, complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 4));
//   REQUIRE(y.ConvertTo(&type::Complex128::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(-2, -2));
//   REQUIRE(t.ConvertTo(&type::Complex128::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::Subtract(int8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Subtract(Location(), &x, &y, &el);
//   REQUIRE(z.IsError());
//   REQUIRE(el.size() == 1);
// }

TEST_CASE("Value::BitOr(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::BitOr(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitOr(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::BitOr(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::BitOr(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
  Value z = Value::BitOr(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-2)));
}

TEST_CASE("Value::BitOr(Rune, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
  Value z = Value::BitOr(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(-2)));
}

TEST_CASE("Value::BitOr(Integer, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(-4));
  Value z = Value::BitOr(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(-2)));
}

// TEST_CASE("Value::BitOr(int8, int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int8::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-2));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(int16, int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int16::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-2));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(int32, int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int32::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-2));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(int64, int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int64::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-2));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(uint8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFC));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFE));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(uint16, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFE));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(uint32, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFE));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(uint64, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFFFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFFFFFFFFFE));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(int, int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-2));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(uint, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFE));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(uintptr, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFE));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitOr(int8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFC));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::BitOr(Location(), &x, &y, &el);
//   REQUIRE(z.IsError());
//   REQUIRE(el.size() == 1);
// }

TEST_CASE("Value::BitXor(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::BitXor(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::BitXor(Boolean, Boolean)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::BitXor(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::BitXor(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
  Value z = Value::BitXor(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-10)));
}

TEST_CASE("Value::BitXor(Rune, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
  Value z = Value::BitXor(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(-10)));
}

TEST_CASE("Value::BitXor(Integer, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(-4));
  Value z = Value::BitXor(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeRune(-10)));
}

// TEST_CASE("Value::BitXor(int8, int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int8::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-10));
//   REQUIRE(t.ConvertTo(&type::Int8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(int16, int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int16::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-10));
//   REQUIRE(t.ConvertTo(&type::Int16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(int32, int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int32::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-10));
//   REQUIRE(t.ConvertTo(&type::Int32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(int64, int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int64::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-10));
//   REQUIRE(t.ConvertTo(&type::Int64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(uint8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFC));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xF6));
//   REQUIRE(t.ConvertTo(&type::Uint8::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(uint16, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint16::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFF6));
//   REQUIRE(t.ConvertTo(&type::Uint16::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(uint32, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFF6));
//   REQUIRE(t.ConvertTo(&type::Uint32::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(uint64, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFFFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint64::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFFFFFFFFF6));
//   REQUIRE(t.ConvertTo(&type::Uint64::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(int, int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-4));
//   REQUIRE(y.ConvertTo(&type::Int::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(-10));
//   REQUIRE(t.ConvertTo(&type::Int::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(uint, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uint::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFF6));
//   REQUIRE(t.ConvertTo(&type::Uint::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(uintptr, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0x0A));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFFC));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   Value t = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFFFFFFF6));
//   REQUIRE(t.ConvertTo(&type::Uintptr::instance));
//   REQUIRE(z == t);
// }

// TEST_CASE("Value::BitXor(int8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(10));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0xFC));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::BitXor(Location(), &x, &y, &el);
//   REQUIRE(z.IsError());
//   REQUIRE(el.size() == 1);
// }

TEST_CASE("Value::Equal(Error, Error)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeError();
  Value y = Value::MakeError();
  Value z = Value::Equal(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
}

TEST_CASE("Value::Equal(true, true)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::Equal(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
}

TEST_CASE("Value::Equal(true, false)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false));
  Value z = Value::Equal(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
}

TEST_CASE("Value::Equal(Bool, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
  Value z = Value::Equal(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::Equal(Integer, Bool)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(0));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
  Value z = Value::Equal(Location(), &x, &y, &el);
  REQUIRE(z.IsError());
  REQUIRE(el.size() == 1);
}

TEST_CASE("Value::Equal(Integer, Integer)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(4));
  Value z = Value::Equal(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
}

TEST_CASE("Value::Equal(Rune, Rune)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeRune(3));
  Value z = Value::Equal(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
}

TEST_CASE("Value::Equal(Float, Float)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3.5));
  Value z = Value::Equal(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
}

TEST_CASE("Value::Equal(Complex, Complex)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3.5, 4));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3.5, 4));
  Value z = Value::Equal(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
}

TEST_CASE("Value::Equal(String, String)") {
  std::stringstream ss;
  ErrorList el;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello"));
  Value y = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello, world"));
  Value z = Value::Equal(Location(), &x, &y, &el);
  REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
}

// TEST_CASE("Value::Equal(bool, bool)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false));
//   REQUIRE(x.ConvertTo(&type::Bool::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
// }

// TEST_CASE("Value::Equal(complex64, complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(3, 4));
//   REQUIRE(y.ConvertTo(&type::Complex64::instance));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
// }

// TEST_CASE("Value::Equal(complex128, complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
// }

// TEST_CASE("Value::Equal(float32, float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(1));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(3));
//   REQUIRE(y.ConvertTo(&type::Float32::instance));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
// }

// TEST_CASE("Value::Equal(float64, float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(1));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(1));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
// }

// TEST_CASE("Value::Equal(int, int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(y.ConvertTo(&type::Int::instance));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
// }

// TEST_CASE("Value::Equal(int8, int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
// }

// TEST_CASE("Value::Equal(int16, int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(y.ConvertTo(&type::Int16::instance));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
// }

// TEST_CASE("Value::Equal(int32, int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
// }

// TEST_CASE("Value::Equal(int64, int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(y.ConvertTo(&type::Int64::instance));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
// }

// TEST_CASE("Value::Equal(uint, uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
// }

// TEST_CASE("Value::Equal(uint8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
// }

// TEST_CASE("Value::Equal(uint16, uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
// }

// TEST_CASE("Value::Equal(uint32, uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(y.ConvertTo(&type::Uint32::instance));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
// }

// TEST_CASE("Value::Equal(uint64, uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
// }

// TEST_CASE("Value::Equal(uintptr, uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(y.ConvertTo(&type::Uintptr::instance));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false)));
// }

// TEST_CASE("Value::Equal(string, string)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello"));
//   REQUIRE(x.ConvertTo(&type::String::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello"));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z == Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true)));
// }

// TEST_CASE("Value::Equal(int, string)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(1));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello"));
//   REQUIRE(y.ConvertTo(&type::String::instance));
//   Value z = Value::Equal(Location(), &x, &y, &el);
//   REQUIRE(z.IsError());
//   REQUIRE(el.size() == 1);
// }

// TEST_CASE("value::operator==(int8, uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   Value y = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(3));
//   REQUIRE(y.ConvertTo(&type::Uint8::instance));
//   REQUIRE(x != y);
// }

TEST_CASE("Value operator<<(Error)") {
  std::stringstream ss;
  Value x = Value::MakeError();
  ss << x;
  REQUIRE(ss.str() == "error");
}

TEST_CASE("Value operator<<(Boolean)") {
  {
    std::stringstream ss;
    Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
    ss << x;
    REQUIRE(ss.str() == "true");
  }
  {
    std::stringstream ss;
    Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false));
    ss << x;
    REQUIRE(ss.str() == "false");
  }
}

TEST_CASE("Value operator<<(Integer)") {
  std::stringstream ss;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
  ss << x;
  REQUIRE(ss.str() == "34");
}

TEST_CASE("Value operator<<(Rune)") {
  {
    std::stringstream ss;
    Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune('j'));
    ss << x;
    REQUIRE(ss.str() == "'j'");
  }
  {
    std::stringstream ss;
    Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(INVALID));
    ss << x;
    REQUIRE(ss.str() == "1114112");
  }
  {
    std::stringstream ss;
    Value x = Value::MakeUntypedConstant(UntypedConstant::MakeRune(SURROGATE_FIRST));
    ss << x;
    REQUIRE(ss.str() == "55296");
  }
}

TEST_CASE("Value operator<<(Float)") {
  std::stringstream ss;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeFloat(34.5));
  ss << x;
  REQUIRE(ss.str() == "34.5");
}

TEST_CASE("Value operator<<(Complex)") {
  std::stringstream ss;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeComplex(1, 2));
  ss << x;
  REQUIRE(ss.str() == "1+2i");
}

TEST_CASE("Value operator<<(String)") {
  std::stringstream ss;
  Value x = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello"));
  ss << x;
  REQUIRE(ss.str() == "\"hello\"");
}

// TEST_CASE("operator<<(bool)") {
//   {
//     std::stringstream ss;
//     ErrorList el;
//     Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(true));
//     REQUIRE(x.ConvertTo(&type::Bool::instance));
//     ss << x;
//     REQUIRE(ss.str() == "true");
//   }
//   {
//     std::stringstream ss;
//     ErrorList el;
//     Value x = Value::MakeUntypedConstant(UntypedConstant::MakeBoolean(false));
//     REQUIRE(x.ConvertTo(&type::Bool::instance));
//     ss << x;
//     REQUIRE(ss.str() == "false");
//   }
// }

// TEST_CASE("operator<<(int8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Int8::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(int16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Int16::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(int32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Int32::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(int64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Int64::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(uint8)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uint8::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(uint16)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uint16::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(uint32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uint32::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(uint64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uint64::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(int)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Int::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(uint)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uint::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(uintptr)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Uintptr::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(float32)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Float32::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(float64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Float64::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34");
// }

// TEST_CASE("operator<<(complex64)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Complex64::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34+0i");
// }

// TEST_CASE("operator<<(complex128)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeInteger(34));
//   REQUIRE(x.ConvertTo(&type::Complex128::instance));
//   ss << x;
//   REQUIRE(ss.str() == "34+0i");
// }

// TEST_CASE("operator<<(string)") {
//   std::stringstream ss;
//   ErrorList el;
//   Value x = Value::MakeUntypedConstant(UntypedConstant::MakeString("hello"));
//   REQUIRE(x.ConvertTo(&type::String::instance));
//   ss << x;
//   REQUIRE(ss.str() == "\"hello\"");
// }

}  // namespace test
}  // namespace rcgo
