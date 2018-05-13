// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/token.h"

#include "src/catch.hpp"

namespace rcgo {
namespace test {

TEST_CASE("Token::Make(location, kind)") {
  Token t = Token::Make(Location::builtin, Token::kFunc);
  REQUIRE(t.location() == Location::builtin);
  REQUIRE(t.kind() == Token::kFunc);
}

TEST_CASE("Token::Make(RelOpKind)") {
  Token t = Token::Make(Token::kEqual);
  REQUIRE(t.location() == Location::builtin);
  REQUIRE(t.kind() == Token::kRelOp);
  REQUIRE(t.rel_op_kind() == Token::kEqual);
}

TEST_CASE("Token::Make(AddOpKind)") {
  Token t = Token::Make(Token::kBitOr);
  REQUIRE(t.location() == Location::builtin);
  REQUIRE(t.kind() == Token::kAddOp);
  REQUIRE(t.add_op_kind() == Token::kBitOr);
}

TEST_CASE("Token::Make(MulOpKind)") {
  Token t = Token::Make(Token::kDivide);
  REQUIRE(t.location() == Location::builtin);
  REQUIRE(t.kind() == Token::kMulOp);
  REQUIRE(t.mul_op_kind() == Token::kDivide);
}

TEST_CASE("Token::Make(UnaryOpKind)") {
  Token t = Token::Make(Token::kPosate);
  REQUIRE(t.location() == Location::builtin);
  REQUIRE(t.kind() == Token::kUnaryOp);
  REQUIRE(t.unary_op_kind() == Token::kPosate);
}

TEST_CASE("Token::Make(AssignOpKind)") {
  Token t = Token::Make(Token::kAddAssign);
  REQUIRE(t.location() == Location::builtin);
  REQUIRE(t.kind() == Token::kAssignOp);
  REQUIRE(t.assign_op_kind() == Token::kAddAssign);
}

TEST_CASE("Token::Make(kind)") {
  Token t = Token::Make(Token::kFunc);
  REQUIRE(t.location() == Location::builtin);
  REQUIRE(t.kind() == Token::kFunc);
}

TEST_CASE("Token::Make(location, token)") {
  Location loc = Location::Make("blah", 3);
  Token s = Token::Make(Token::kFunc);
  Token t = Token::Make(loc, s);
  REQUIRE(t.location() == loc);
  REQUIRE(t.kind() == Token::kFunc);
}

TEST_CASE("Token::MakeIdentifier(location, token)") {
  Token t = Token::MakeIdentifier(Location::builtin, "name");
  REQUIRE(t.location() == Location::builtin);
  REQUIRE(t.kind() == Token::kIdentifier);
  REQUIRE(t.identifier() == "name");
}

TEST_CASE("Token::MakeLiteral(location, value)") {
  Value v = Value::MakeBoolean(true);
  Token t = Token::MakeLiteral(Location::builtin, v);
  REQUIRE(t.location() == Location::builtin);
  REQUIRE(t.kind() == Token::kLiteral);
  REQUIRE(t.value() == v);
}

TEST_CASE("Token::operator==(Kind)") {
  Token t = Token::Make(Token::kFunc);
  REQUIRE(t == Token::kFunc);
}

TEST_CASE("Token::operator!=(Kind)") {
  Token t = Token::Make(Token::kFunc);
  REQUIRE(t != Token::kGoto);
}

TEST_CASE("Token::IsAddOp()") {
  Token t = Token::Make(Token::kPosate);
  REQUIRE(t.IsAddOp());
  Token u = Token::Make(Token::kAddress);
  REQUIRE(!u.IsAddOp());
}

TEST_CASE("Token::add_op_kind()") {
  Token t = Token::Make(Token::kPosate);
  REQUIRE(t.add_op_kind() == Token::kAdd);
  t = Token::Make(Token::kNegate);
  REQUIRE(t.add_op_kind() == Token::kSubtract);
  t = Token::Make(Token::kBitNot);
  REQUIRE(t.add_op_kind() == Token::kBitXor);
}

TEST_CASE("Token::IsMulOp()") {
  Token t = Token::Make(Token::kPointer);
  REQUIRE(t.IsMulOp());
  t = Token::Make(Token::kAddress);
  REQUIRE(t.IsMulOp());
}

TEST_CASE("Token::mul_op_kind()") {
  Token t = Token::Make(Token::kPointer);
  REQUIRE(t.mul_op_kind() == Token::kMultiply);
  t = Token::Make(Token::kAddress);
  REQUIRE(t.mul_op_kind() == Token::kBitAnd);
}

TEST_CASE("Token::IsUnaryOp()") {
  Token t = Token::Make(Token::kPointer);
  REQUIRE(t.IsUnaryOp());
}

TEST_CASE("Token::unary_op_kind()") {
  Token t = Token::Make(Token::kPointer);
  REQUIRE(t.unary_op_kind() == Token::kIndirection);
}

#define INSERT_TEST(k, s) {                     \
    std::stringstream ss;                       \
    ss << k;                                    \
    REQUIRE(ss.str() == s);                   \
  }

TEST_CASE("operator<< Token::Kind") {
  INSERT_TEST(Token::kIdentifier, "identifier");
  INSERT_TEST(Token::kBreak, "break");
  INSERT_TEST(Token::kCase, "case");
  INSERT_TEST(Token::kConst, "const");
  INSERT_TEST(Token::kContinue, "continue");
  INSERT_TEST(Token::kDefault, "default");
  INSERT_TEST(Token::kDefer, "defer");
  INSERT_TEST(Token::kElse, "else");
  INSERT_TEST(Token::kFallthrough, "fallthrough");
  INSERT_TEST(Token::kFor, "for");
  INSERT_TEST(Token::kFunc, "func");
  INSERT_TEST(Token::kGoto, "goto");
  INSERT_TEST(Token::kIf, "if");
  INSERT_TEST(Token::kImport, "import");
  INSERT_TEST(Token::kInterface, "interface");
  INSERT_TEST(Token::kMap, "map");
  INSERT_TEST(Token::kPackage, "package");
  INSERT_TEST(Token::kRange, "range");
  INSERT_TEST(Token::kReturn, "return");
  INSERT_TEST(Token::kStruct, "struct");
  INSERT_TEST(Token::kSwitch, "switch");
  INSERT_TEST(Token::kType, "type");
  INSERT_TEST(Token::kVar, "var");
  INSERT_TEST(Token::kAssign, "=");
  INSERT_TEST(Token::kColon, ":");
  INSERT_TEST(Token::kComma, ",");
  INSERT_TEST(Token::kDecrement, "--");
  INSERT_TEST(Token::kEllipsis, "...");
  INSERT_TEST(Token::kIncrement, "++");
  INSERT_TEST(Token::kLeftBrace, "{");
  INSERT_TEST(Token::kLeftBracket, "[");
  INSERT_TEST(Token::kLeftParen, "(");
  INSERT_TEST(Token::kLogicalAnd, "&&");
  INSERT_TEST(Token::kLogicalOr, "||");
  INSERT_TEST(Token::kPeriod, ".");
  INSERT_TEST(Token::kPointer, "*");
  INSERT_TEST(Token::kRightBrace, "}");
  INSERT_TEST(Token::kRightBracket, "]");
  INSERT_TEST(Token::kRightParen, ")");
  INSERT_TEST(Token::kSemicolon, ";");
  INSERT_TEST(Token::kShortAssign, ":=");
  INSERT_TEST(Token::kRelOp, "RelOp");
  INSERT_TEST(Token::kAddOp, "AddOp");
  INSERT_TEST(Token::kMulOp, "MulOp");
  INSERT_TEST(Token::kUnaryOp, "UnaryOp");
  INSERT_TEST(Token::kAssignOp, "AssignOp");
  INSERT_TEST(Token::kLiteral, "Literal");
  INSERT_TEST(Token::kEnd, "End");
  INSERT_TEST(Token::kEqual, "==");
  INSERT_TEST(Token::kGreaterThan, ">");
  INSERT_TEST(Token::kGreaterThanEqual, ">=");
  INSERT_TEST(Token::kLessThan, "<");
  INSERT_TEST(Token::kLessThanEqual, "<=");
  INSERT_TEST(Token::kNotEqual, "!=");
  INSERT_TEST(Token::kAdd, "+");
  INSERT_TEST(Token::kBitOr, "|");
  INSERT_TEST(Token::kBitXor, "^");
  INSERT_TEST(Token::kSubtract, "-");
  INSERT_TEST(Token::kMultiply, "*");
  INSERT_TEST(Token::kDivide, "/");
  INSERT_TEST(Token::kModulus, "%");
  INSERT_TEST(Token::kLeftShift, "<<");
  INSERT_TEST(Token::kRightShift, ">>");
  INSERT_TEST(Token::kBitAnd, "&");
  INSERT_TEST(Token::kBitAndNot, "&^");
  INSERT_TEST(Token::kPosate, "+");
  INSERT_TEST(Token::kNegate, "-");
  INSERT_TEST(Token::kLogicalNot, "!");
  INSERT_TEST(Token::kBitNot, "^");
  INSERT_TEST(Token::kIndirection, "*");
  INSERT_TEST(Token::kAddress, "&");
  INSERT_TEST(Token::kAddAssign, "+=");
  INSERT_TEST(Token::kBitAndAssign, "&=");
  INSERT_TEST(Token::kBitAndNotAssign, "&^=");
  INSERT_TEST(Token::kBitOrAssign, "|=");
  INSERT_TEST(Token::kBitXorAssign, "^=");
  INSERT_TEST(Token::kDivideAssign, "/=");
  INSERT_TEST(Token::kLeftShiftAssign, "<<=");
  INSERT_TEST(Token::kModulusAssign, "%=");
  INSERT_TEST(Token::kMultiplyAssign, "*=");
  INSERT_TEST(Token::kRightShiftAssign, ">>=");
  INSERT_TEST(Token::kSubtractAssign, "-=");
}

TEST_CASE("operator<< Token") {
  {
    Token t = Token::MakeIdentifier(Location::builtin, "name");
    std::stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "name");
  }
  {
    Token t = Token::MakeLiteral(Location::builtin, Value::MakeBoolean(true));
    std::stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "true");
  }
  {
    Token t = Token::Make(Token::kEqual);
    std::stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "==");
  }
  {
    Token t = Token::Make(Token::kBitOr);
    std::stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "|");
  }
  {
    Token t = Token::Make(Token::kDivide);
    std::stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "/");
  }
  {
    Token t = Token::Make(Token::kPosate);
    std::stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "+");
  }
  {
    Token t = Token::Make(Token::kAddAssign);
    std::stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "+=");
  }
  {
    Token t = Token::Make(Token::kFunc);
    std::stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "func");
  }
}

}  // namespace test
}  // namespace rcgo
