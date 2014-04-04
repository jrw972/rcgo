/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "Token.hpp"

using namespace rcgo;

TEST_CASE ("Token::make (location, kind)") {
  Token t = Token::make (Location::builtin, Token::Func);
  REQUIRE (t.location () == Location::builtin);
  REQUIRE (t.kind () == Token::Func);
}

TEST_CASE ("Token::make (RelOpKind)") {
  Token t = Token::make (Token::Equal);
  REQUIRE (t.location () == Location::builtin);
  REQUIRE (t.kind () == Token::RelOp);
  REQUIRE (t.rel_op_kind () == Token::Equal);
}

TEST_CASE ("Token::make (AddOpKind)") {
  Token t = Token::make (Token::BitOr);
  REQUIRE (t.location () == Location::builtin);
  REQUIRE (t.kind () == Token::AddOp);
  REQUIRE (t.add_op_kind () == Token::BitOr);
}

TEST_CASE ("Token::make (MulOpKind)") {
  Token t = Token::make (Token::Divide);
  REQUIRE (t.location () == Location::builtin);
  REQUIRE (t.kind () == Token::MulOp);
  REQUIRE (t.mul_op_kind () == Token::Divide);
}

TEST_CASE ("Token::make (UnaryOpKind)") {
  Token t = Token::make (Token::Posate);
  REQUIRE (t.location () == Location::builtin);
  REQUIRE (t.kind () == Token::UnaryOp);
  REQUIRE (t.unary_op_kind () == Token::Posate);
}

TEST_CASE ("Token::make (AssignOpKind)") {
  Token t = Token::make (Token::AddAssign);
  REQUIRE (t.location () == Location::builtin);
  REQUIRE (t.kind () == Token::AssignOp);
  REQUIRE (t.assign_op_kind () == Token::AddAssign);
}

TEST_CASE ("Token::make (kind)") {
  Token t = Token::make (Token::Func);
  REQUIRE (t.location () == Location::builtin);
  REQUIRE (t.kind () == Token::Func);
}

TEST_CASE ("Token::make (location, token)") {
  Location loc = Location::make ("blah", 3);
  Token s = Token::make (Token::Func);
  Token t = Token::make (loc, s);
  REQUIRE (t.location () == loc);
  REQUIRE (t.kind () == Token::Func);
}

TEST_CASE ("Token::make_identifier (location, token)") {
  Token t = Token::make_identifier (Location::builtin, "name");
  REQUIRE (t.location () == Location::builtin);
  REQUIRE (t.kind () == Token::Identifier);
  REQUIRE (t.identifier () == "name");
}

TEST_CASE ("Token::make_literal (location, value)") {
  Value v = Value::make_boolean (true);
  Token t = Token::make_literal (Location::builtin, v);
  REQUIRE (t.location () == Location::builtin);
  REQUIRE (t.kind () == Token::Literal);
  REQUIRE (t.value () == v);
}

TEST_CASE ("Token::operator== (Kind)") {
  Token t = Token::make (Token::Func);
  REQUIRE (t == Token::Func);
}

TEST_CASE ("Token::operator!= (Kind)") {
  Token t = Token::make (Token::Func);
  REQUIRE (t != Token::Goto);
}

TEST_CASE ("Token::is_add_op ()") {
  Token t = Token::make (Token::Posate);
  REQUIRE (t.is_add_op ());
  Token u = Token::make (Token::Address);
  REQUIRE (!u.is_add_op ());
}

TEST_CASE ("Token::add_op_kind ()") {
  Token t = Token::make (Token::Posate);
  REQUIRE (t.add_op_kind () == Token::Add);
  t = Token::make (Token::Negate);
  REQUIRE (t.add_op_kind () == Token::Subtract);
  t = Token::make (Token::BitNot);
  REQUIRE (t.add_op_kind () == Token::BitXor);
}

TEST_CASE ("Token::is_mul_op ()") {
  Token t = Token::make (Token::Pointer);
  REQUIRE (t.is_mul_op ());
  t = Token::make (Token::Address);
  REQUIRE (t.is_mul_op ());
}

TEST_CASE ("Token::mul_op_kind ()") {
  Token t = Token::make (Token::Pointer);
  REQUIRE (t.mul_op_kind () == Token::Multiply);
  t = Token::make (Token::Address);
  REQUIRE (t.mul_op_kind () == Token::BitAnd);
}

TEST_CASE ("Token::is_unary_op ()") {
  Token t = Token::make (Token::Pointer);
  REQUIRE (t.is_unary_op ());
}

TEST_CASE ("Token::unary_op_kind ()") {
  Token t = Token::make (Token::Pointer);
  REQUIRE (t.unary_op_kind () == Token::Indirection);
}

#define INSERT_TEST(k, s) {                     \
    std::stringstream ss;                       \
    ss << k;                                    \
    REQUIRE (ss.str () == s);                   \
  }

TEST_CASE ("operator<< Token::Kind") {
  INSERT_TEST (Token::Identifier, "identifier");
  INSERT_TEST (Token::Break, "break");
  INSERT_TEST (Token::Case, "case");
  INSERT_TEST (Token::Const, "const");
  INSERT_TEST (Token::Continue, "continue");
  INSERT_TEST (Token::Default, "default");
  INSERT_TEST (Token::Defer, "defer");
  INSERT_TEST (Token::Else, "else");
  INSERT_TEST (Token::Fallthrough, "fallthrough");
  INSERT_TEST (Token::For, "for");
  INSERT_TEST (Token::Func, "func");
  INSERT_TEST (Token::Goto, "goto");
  INSERT_TEST (Token::If, "if");
  INSERT_TEST (Token::Import, "import");
  INSERT_TEST (Token::Interface, "interface");
  INSERT_TEST (Token::Map, "map");
  INSERT_TEST (Token::Package, "package");
  INSERT_TEST (Token::Range, "range");
  INSERT_TEST (Token::Return, "return");
  INSERT_TEST (Token::Struct, "struct");
  INSERT_TEST (Token::Switch, "switch");
  INSERT_TEST (Token::Type, "type");
  INSERT_TEST (Token::Var, "var");
  INSERT_TEST (Token::Assign, "=");
  INSERT_TEST (Token::Colon, ":");
  INSERT_TEST (Token::Comma, ",");
  INSERT_TEST (Token::Decrement, "--");
  INSERT_TEST (Token::Ellipsis, "...");
  INSERT_TEST (Token::Increment, "++");
  INSERT_TEST (Token::LeftBrace, "{");
  INSERT_TEST (Token::LeftBracket, "[");
  INSERT_TEST (Token::LeftParen, "(");
  INSERT_TEST (Token::LogicalAnd, "&&");
  INSERT_TEST (Token::LogicalOr, "||");
  INSERT_TEST (Token::Period, ".");
  INSERT_TEST (Token::Pointer, "*");
  INSERT_TEST (Token::RightBrace, "}");
  INSERT_TEST (Token::RightBracket, "]");
  INSERT_TEST (Token::RightParen, ")");
  INSERT_TEST (Token::Semicolon, ";");
  INSERT_TEST (Token::ShortAssign, ":=");
  INSERT_TEST (Token::RelOp, "RelOp");
  INSERT_TEST (Token::AddOp, "AddOp");
  INSERT_TEST (Token::MulOp, "MulOp");
  INSERT_TEST (Token::UnaryOp, "UnaryOp");
  INSERT_TEST (Token::AssignOp, "AssignOp");
  INSERT_TEST (Token::Literal, "Literal");
  INSERT_TEST (Token::End, "End");
  INSERT_TEST (Token::Equal, "==");
  INSERT_TEST (Token::GreaterThan, ">");
  INSERT_TEST (Token::GreaterThanEqual, ">=");
  INSERT_TEST (Token::LessThan, "<");
  INSERT_TEST (Token::LessThanEqual, "<=");
  INSERT_TEST (Token::NotEqual, "!=");
  INSERT_TEST (Token::Add, "+");
  INSERT_TEST (Token::BitOr, "|");
  INSERT_TEST (Token::BitXor, "^");
  INSERT_TEST (Token::Subtract, "-");
  INSERT_TEST (Token::Multiply, "*");
  INSERT_TEST (Token::Divide, "/");
  INSERT_TEST (Token::Modulus, "%");
  INSERT_TEST (Token::LeftShift, "<<");
  INSERT_TEST (Token::RightShift, ">>");
  INSERT_TEST (Token::BitAnd, "&");
  INSERT_TEST (Token::BitAndNot, "&^");
  INSERT_TEST (Token::Posate, "+");
  INSERT_TEST (Token::Negate, "-");
  INSERT_TEST (Token::LogicalNot, "!");
  INSERT_TEST (Token::BitNot, "^");
  INSERT_TEST (Token::Indirection, "*");
  INSERT_TEST (Token::Address, "&");
  INSERT_TEST (Token::AddAssign, "+=");
  INSERT_TEST (Token::BitAndAssign, "&=");
  INSERT_TEST (Token::BitAndNotAssign, "&^=");
  INSERT_TEST (Token::BitOrAssign, "|=");
  INSERT_TEST (Token::BitXorAssign, "^=");
  INSERT_TEST (Token::DivideAssign, "/=");
  INSERT_TEST (Token::LeftShiftAssign, "<<=");
  INSERT_TEST (Token::ModulusAssign, "%=");
  INSERT_TEST (Token::MultiplyAssign, "*=");
  INSERT_TEST (Token::RightShiftAssign, ">>=");
  INSERT_TEST (Token::SubtractAssign, "-=");
}

TEST_CASE ("operator<< Token") {
  {
    Token t = Token::make_identifier (Location::builtin, "name");
    std::stringstream ss;
    ss << t;
    REQUIRE (ss.str () == "name");
  }
  {
    Token t = Token::make_literal (Location::builtin, Value::make_boolean (true));
    std::stringstream ss;
    ss << t;
    REQUIRE (ss.str () == "true");
  }
  {
    Token t = Token::make (Token::Equal);
    std::stringstream ss;
    ss << t;
    REQUIRE (ss.str () == "==");
  }
  {
    Token t = Token::make (Token::BitOr);
    std::stringstream ss;
    ss << t;
    REQUIRE (ss.str () == "|");
  }
  {
    Token t = Token::make (Token::Divide);
    std::stringstream ss;
    ss << t;
    REQUIRE (ss.str () == "/");
  }
  {
    Token t = Token::make (Token::Posate);
    std::stringstream ss;
    ss << t;
    REQUIRE (ss.str () == "+");
  }
  {
    Token t = Token::make (Token::AddAssign);
    std::stringstream ss;
    ss << t;
    REQUIRE (ss.str () == "+=");
  }
  {
    Token t = Token::make (Token::Func);
    std::stringstream ss;
    ss << t;
    REQUIRE (ss.str () == "func");
  }

}
