// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/parser.h"

#include <iostream>
#include <string>

#include "src/ast.h"
#include "src/catch.hpp"
#include "src/test_helpers.h"

namespace rcgo {
namespace test {

#define PARSE_NODE_SUCCESS(method, str, args...) \
  TEST_CASE("Parser::" #method "(" #args ")" " on \"" str "\" success") { \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(str);                                          \
    Parser parser(&seq, &er, ss);                                       \
    ast::Node* node = parser.method(args);                              \
    REQUIRE(er.Empty());                                                \
    REQUIRE(node != NULL);                                              \
    delete node;                                                        \
  }

#define PARSE_LIST_SUCCESS(method, str, args...) \
  TEST_CASE("Parser::" #method "(" #args ")" " on \"" str "\" success") { \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(str);                                          \
    Parser parser(&seq, &er, ss);                                       \
    ast::ListType list = parser.method(args);                           \
    REQUIRE(er.Empty());                                                \
    delete(new ast::Struct(location, list));                            \
  }

#define PARSE_NODE_FAILURE(method, str, type, err, args...) \
  TEST_CASE("Parser::" #method "(" #args ")" " on \"" str "\" failure") { \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 0, &abort_handler);                            \
    StringByteStream seq(str);                                          \
    Parser parser(&seq, &er, ss);                                       \
    ast::Node* node = parser.method(args);                              \
    REQUIRE(er.At(0) == std::string(err));                              \
    REQUIRE(node != NULL);                                              \
    REQUIRE(dynamic_cast<type*>(node) != NULL);                         \
    delete node;                                                        \
  }

#define PARSE_LIST_FAILURE(method, str, err, args...) \
  TEST_CASE("Parser::" #method "(" #args ")" " on \"" str "\" failure") { \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 0, &abort_handler);                            \
    StringByteStream seq(str);                                          \
    Parser parser(&seq, &er, ss);                                       \
    ast::ListType list = parser.method(args);                           \
    REQUIRE(er.At(0) == std::string(err));                              \
    delete(new ast::Struct(location, list));                            \
  }

#define PARSE_NULL(method, str, err, args...) \
  TEST_CASE("Parser::" #method "(" #args ")" " on \"" str "\" failure") { \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 0, &abort_handler);                            \
    StringByteStream seq(str);                                          \
    Parser parser(&seq, &er, ss);                                       \
    ast::Node* node = parser.method(args);                              \
    REQUIRE(er.At(0) == std::string(err));                              \
    REQUIRE(node == NULL);                                              \
  }

PARSE_NODE_SUCCESS(AddExpr, "x + y");
PARSE_NODE_SUCCESS(AddExpr, "x - y");
PARSE_NODE_SUCCESS(AddExpr, "x ^ y");
PARSE_NODE_SUCCESS(AddExpr, "x | y");
PARSE_NODE_SUCCESS(AndExpr, "x && y");
PARSE_NODE_SUCCESS(Block, "{ m := Bar{ii: x}\n return m.ii\n }");
PARSE_NODE_SUCCESS(Block, "{;}");
PARSE_NODE_SUCCESS(Block, "{}");
PARSE_NODE_SUCCESS(ConstDecl, "const(x, y, z; a = 4)");
PARSE_NODE_SUCCESS(ConstDecl, "const x");
PARSE_NODE_SUCCESS(ConstSpec, "x int = 4");
PARSE_NODE_SUCCESS(Declaration, "const()");
PARSE_NODE_SUCCESS(Declaration, "type()");
PARSE_NODE_SUCCESS(Declaration, "var()");
PARSE_LIST_SUCCESS(ExpressionList, "1, 2, 3");
PARSE_NODE_SUCCESS(FieldDecl, "*T `tag`");
PARSE_NODE_SUCCESS(FieldDecl, "*T");
PARSE_NODE_SUCCESS(FieldDecl, "S.T `tag`;");
PARSE_NODE_SUCCESS(FieldDecl, "T `tag`");
PARSE_NODE_SUCCESS(FieldDecl, "T");
PARSE_NODE_SUCCESS(FieldDecl, "apackage.T");
PARSE_NODE_SUCCESS(FieldDecl, "x, y, z T `tag`");
PARSE_NODE_SUCCESS(FuncDecl, "myfunc() { }");  // NOTE: no func
PARSE_LIST_SUCCESS(IdentifierList, "x, y, z");
PARSE_NODE_SUCCESS(ImportDecl, "import . \"math\"");
PARSE_NODE_SUCCESS(ImportDecl, "import mymath \"math\"");
PARSE_NODE_SUCCESS(ImportSpec, ". \"math\"");
PARSE_NODE_SUCCESS(ImportSpec, "mymath \"math\"");
PARSE_NODE_SUCCESS(KeyedElement, "identifier");
PARSE_NODE_SUCCESS(KeyedElement, "x: { 0 }");
PARSE_NODE_SUCCESS(KeyedElement, "{ 0 }");
PARSE_NODE_SUCCESS(LiteralValue, "{ 1, 2, 3, }");
PARSE_NODE_SUCCESS(LiteralValue, "{ }");
PARSE_NODE_SUCCESS(MethodDecl, "(this *T) mymethod() { }");  // NOTE: no func
PARSE_NODE_SUCCESS(MethodSpec, "MyMethod()");
PARSE_NODE_SUCCESS(MulExpr, "x % y");
PARSE_NODE_SUCCESS(MulExpr, "x & y");
PARSE_NODE_SUCCESS(MulExpr, "x &^ y");
PARSE_NODE_SUCCESS(MulExpr, "x * y");
PARSE_NODE_SUCCESS(MulExpr, "x / y");
PARSE_NODE_SUCCESS(MulExpr, "x << y");
PARSE_NODE_SUCCESS(MulExpr, "x >> y");
PARSE_NODE_SUCCESS(Operand, "'a'");
PARSE_NODE_SUCCESS(Operand, "(x)");
PARSE_NODE_SUCCESS(Operand, "1.0");
PARSE_NODE_SUCCESS(Operand, "1.0i");
PARSE_NODE_SUCCESS(Operand, "T { 1, 2, 3 }");
PARSE_NODE_SUCCESS(Operand, "[...]int { 1, 2, 3 }");
PARSE_NODE_SUCCESS(Operand, "[3]int { 1, 2, 3 }");
PARSE_NODE_SUCCESS(Operand, "[]int { 1, 2, 3 }");
PARSE_NODE_SUCCESS(Operand, "\"string\"");
PARSE_NODE_SUCCESS(Operand, "func() { }");
PARSE_NODE_SUCCESS(OrExpr, "x || y");
PARSE_NODE_SUCCESS(OrExpr, "x");
PARSE_LIST_SUCCESS(Parameters, "(T1, T2, T3)");
PARSE_LIST_SUCCESS(Parameters, "(interface{})");
PARSE_LIST_SUCCESS(Parameters, "(x T1, y T2, z T3,)");
PARSE_LIST_SUCCESS(Parameters, "(x, y T1, z T2)");
PARSE_LIST_SUCCESS(Parameters, "(x, y, z,)");
PARSE_NODE_SUCCESS(PrimaryExpr, "[3] int { 1, 2, 3}");
PARSE_NODE_SUCCESS(PrimaryExpr, "f()");
PARSE_NODE_SUCCESS(PrimaryExpr, "f(x ...)");
PARSE_NODE_SUCCESS(PrimaryExpr, "f(x)");
PARSE_NODE_SUCCESS(PrimaryExpr, "f(x,)");
PARSE_NODE_SUCCESS(PrimaryExpr, "f(x...,)");
PARSE_NODE_SUCCESS(PrimaryExpr, "x");
PARSE_NODE_SUCCESS(PrimaryExpr, "x.(T)");
PARSE_NODE_SUCCESS(PrimaryExpr, "x.y");
PARSE_NODE_SUCCESS(PrimaryExpr, "x.y.z");
PARSE_NODE_SUCCESS(PrimaryExpr, "x[4:5:6]");
PARSE_NODE_SUCCESS(PrimaryExpr, "x[4:5]");
PARSE_NODE_SUCCESS(PrimaryExpr, "x[4:]");
PARSE_NODE_SUCCESS(PrimaryExpr, "x[4]");
PARSE_NODE_SUCCESS(PrimaryExpr, "x[:4:5]");
PARSE_NODE_SUCCESS(PrimaryExpr, "x[:4]");
PARSE_NODE_SUCCESS(PrimaryExpr, "x[:]");
PARSE_NODE_SUCCESS(RelExpr, "x != y");
PARSE_NODE_SUCCESS(RelExpr, "x < y");
PARSE_NODE_SUCCESS(RelExpr, "x <= y");
PARSE_NODE_SUCCESS(RelExpr, "x == y");
PARSE_NODE_SUCCESS(RelExpr, "x > y");
PARSE_NODE_SUCCESS(RelExpr, "x >= y");
PARSE_NODE_SUCCESS(Signature, "()()");
PARSE_NODE_SUCCESS(Signature, "() int");
PARSE_NODE_SUCCESS(SimpleStmt, "x := y");
PARSE_NODE_SUCCESS(SourceFile, "package mypackage");
PARSE_NODE_SUCCESS(SourceFile, "package mypackage; import(\"math\")");
PARSE_NODE_SUCCESS(SourceFile, "package mypackage; import(\"math\";)");
PARSE_NODE_SUCCESS(SourceFile, "package mypackage; import \"math\"");
PARSE_NODE_SUCCESS(SourceFile, "package mypackage; var x T");
PARSE_NODE_SUCCESS(SourceFileHeader, "package mypackage");
PARSE_NODE_SUCCESS(SourceFileHeader, "package mypackage; import(\"math\")");
PARSE_NODE_SUCCESS(SourceFileHeader, "package mypackage; import(\"math\";)");
PARSE_NODE_SUCCESS(SourceFileHeader, "package mypackage; import \"math\"");
PARSE_NODE_SUCCESS(Statement, "break OuterLoop");
PARSE_NODE_SUCCESS(Statement, "break");
PARSE_NODE_SUCCESS(Statement, "continue OuterLoop");
PARSE_NODE_SUCCESS(Statement, "continue");
PARSE_NODE_SUCCESS(Statement, "defer f(x)");
PARSE_NODE_SUCCESS(Statement, "done: f(x)");
PARSE_NODE_SUCCESS(Statement, "fallthrough");
PARSE_NODE_SUCCESS(Statement, "for  ;  ;   { }");
PARSE_NODE_SUCCESS(Statement, "for  ;  ; z { }");
PARSE_NODE_SUCCESS(Statement, "for  ; y;   { }");
PARSE_NODE_SUCCESS(Statement, "for  ; y; z { }");
PARSE_NODE_SUCCESS(Statement, "for ;; { }");
PARSE_NODE_SUCCESS(Statement, "for range x { }");
PARSE_NODE_SUCCESS(Statement, "for x { }");
PARSE_NODE_SUCCESS(Statement, "for x;  ;   { }");
PARSE_NODE_SUCCESS(Statement, "for x;  ; z { }");
PARSE_NODE_SUCCESS(Statement, "for x; y;   { }");
PARSE_NODE_SUCCESS(Statement, "for x; y; z { }");
PARSE_NODE_SUCCESS(Statement, "for y := range x { }");
PARSE_NODE_SUCCESS(Statement, "for y = range x { }");
PARSE_NODE_SUCCESS(Statement, "for { }");
PARSE_NODE_SUCCESS(Statement, "goto done");
PARSE_NODE_SUCCESS(Statement, "if true { } else if false { }");
PARSE_NODE_SUCCESS(Statement, "if true { } else { }");
PARSE_NODE_SUCCESS(Statement, "if true { }");
PARSE_NODE_SUCCESS(Statement, "if x := y; true { }");
PARSE_NODE_SUCCESS(Statement, "return x, y");
PARSE_NODE_SUCCESS(Statement, "return");
PARSE_NODE_SUCCESS(Statement, "switch x { }");
PARSE_NODE_SUCCESS(Statement, "switch x; y { }");
PARSE_NODE_SUCCESS(Statement, "switch x; { }");
PARSE_NODE_SUCCESS(Statement, "switch { case 1, 2, 3: }");
PARSE_NODE_SUCCESS(Statement, "switch { default: }");
PARSE_NODE_SUCCESS(Statement, "switch { }");
PARSE_NODE_SUCCESS(Statement, "var x T");
PARSE_NODE_SUCCESS(Statement, "x %= y");
PARSE_NODE_SUCCESS(Statement, "x &= y");
PARSE_NODE_SUCCESS(Statement, "x &^= y");
PARSE_NODE_SUCCESS(Statement, "x *= y");
PARSE_NODE_SUCCESS(Statement, "x += y");
PARSE_NODE_SUCCESS(Statement, "x -= y");
PARSE_NODE_SUCCESS(Statement, "x /= y");
PARSE_NODE_SUCCESS(Statement, "x := y");
PARSE_NODE_SUCCESS(Statement, "x <<= y");
PARSE_NODE_SUCCESS(Statement, "x = y");
PARSE_NODE_SUCCESS(Statement, "x >>= y");
PARSE_NODE_SUCCESS(Statement, "x ^= y");
PARSE_NODE_SUCCESS(Statement, "x |= y");
PARSE_NODE_SUCCESS(Statement, "x++");
PARSE_NODE_SUCCESS(Statement, "x--");
PARSE_NODE_SUCCESS(Statement, "{ }");
PARSE_LIST_SUCCESS(StatementList, "s}");
PARSE_NODE_SUCCESS(SwitchStmt,
                   "switch val := i.(type) {\n"
                   "case *int:\n"
                   "  return val\n"
                   "case *int8:\n"
                   "  v := int(*val)\n"
                   "  return &v\n"
                   "}");
PARSE_NODE_SUCCESS(TopLevelDecl, "const()");
PARSE_NODE_SUCCESS(TopLevelDecl, "func(x T) method()");
PARSE_NODE_SUCCESS(TopLevelDecl, "func function()");
PARSE_NODE_SUCCESS(TopLevelDecl, "type()");
PARSE_NODE_SUCCESS(TopLevelDecl, "var()");
PARSE_NODE_SUCCESS(Type, "(int)");
PARSE_NODE_SUCCESS(Type, "*int");
PARSE_NODE_SUCCESS(Type, "[3]int");
PARSE_NODE_SUCCESS(Type, "[]int");
PARSE_NODE_SUCCESS(Type, "apackage.identifier");
PARSE_NODE_SUCCESS(Type, "func()");
PARSE_NODE_SUCCESS(Type, "identifier");
PARSE_NODE_SUCCESS(Type, "interface { X; Y }");
PARSE_NODE_SUCCESS(Type, "interface {}");
PARSE_NODE_SUCCESS(Type, "map[int]float { 0: 1, 1: 2 }");
PARSE_NODE_SUCCESS(Type, "map[string]int");
PARSE_NODE_SUCCESS(Type, "struct { x int; y float }");
PARSE_NODE_SUCCESS(Type, "struct {} { 1, 2, 3 }");
PARSE_NODE_SUCCESS(Type, "struct {}");
PARSE_NODE_SUCCESS(TypeDecl, "type(T int; U float)");
PARSE_NODE_SUCCESS(TypeDecl, "type T int");
PARSE_NODE_SUCCESS(UnaryExpr, "!x");
PARSE_NODE_SUCCESS(UnaryExpr, "&x");
PARSE_NODE_SUCCESS(UnaryExpr, "*x");
PARSE_NODE_SUCCESS(UnaryExpr, "+x");
PARSE_NODE_SUCCESS(UnaryExpr, "-x");
PARSE_NODE_SUCCESS(UnaryExpr, "^x");
PARSE_NODE_SUCCESS(UnaryExpr, "x");
PARSE_NODE_SUCCESS(VarDecl, "var(x int; y float = 3; z = 5)");
PARSE_NODE_SUCCESS(VarDecl, "var x int");

Location loc = Location::Make("undefined", 1);

PARSE_NODE_FAILURE(ConstDecl, "const( func )", ast::ConstDecl,
                   Expected1(loc, Token::kIdentifier));
PARSE_NODE_FAILURE(ConstDecl, "const( x; x T = 1 y; y )", ast::ConstDecl,
                   Expected1(loc, Token::kRightParen));
PARSE_NODE_FAILURE(ConstDecl, "const 4", ast::ConstDecl,
                   ExpectedASpecificationOrGroup(loc));
PARSE_NODE_FAILURE(ConstSpec, "x int = 4, 3", ast::ConstSpec,
                   ExpectedNExpressions(loc, 1));
PARSE_NODE_FAILURE(ImportDecl, "import( `x`; `y` func; `z` )", ast::ImportDecl,
                   Expected1(loc, Token::kRightParen));
PARSE_NODE_FAILURE(ImportDecl, "import( func )", ast::ImportDecl,
                   Expected1(loc, Token::kRightParen));
PARSE_NODE_FAILURE(LiteralValue, "{ 1, func, 2 }", ast::LiteralValue,
                   Expected1(loc, Token::kLeftParen));
PARSE_NODE_FAILURE(Operand, "+", ast::Error, ExpectedAnOperand(loc));
PARSE_LIST_FAILURE(Parameters, "(c chan *int)",
                   Expected1(loc, Token::kRightParen))
PARSE_LIST_FAILURE(Parameters, "(x T, 4, y T)",
                   Expected1(loc, Token::kIdentifier));
PARSE_LIST_FAILURE(Parameters, "(x T, x 4, y T)",
                   ExpectedAType(loc));
PARSE_LIST_FAILURE(Parameters, "(x, *y, z T)",
                   Expected1(loc, Token::kIdentifier));
PARSE_LIST_FAILURE(Parameters, "(x, y ... T1, z ... T2)",
                   IllegalVariadicParameter(loc));
PARSE_NODE_FAILURE(PrimaryExpr, "x[4 type]", ast::SliceOp,
                   Expected1(loc, Token::kColon));
PARSE_NODE_FAILURE(PrimaryExpr, "x[4:5 type]", ast::SliceOp, InvalidSlice(loc));
PARSE_NODE_FAILURE(PrimaryExpr, "x[:4 type]", ast::SliceOp, InvalidSlice(loc));
PARSE_NODE_FAILURE(SimpleStmt, "x, y", ast::ExpressionStatement,
                   UnexpectedExpressionList(loc));
PARSE_NODE_FAILURE(SimpleStmt, "x, y++", ast::Increment,
                   UnexpectedExpressionList(loc));
PARSE_NODE_FAILURE(SimpleStmt, "x, y--", ast::Decrement,
                   UnexpectedExpressionList(loc));
PARSE_NODE_FAILURE(SourceFile, "package mypackage; import func; var x T",
                   ast::SourceFile, ExpectedASpecificationOrGroup(loc));
PARSE_NODE_FAILURE(SourceFile, "package mypackage; range; var x T",
                   ast::SourceFile, ExpectedADeclaration(loc));
PARSE_NODE_FAILURE(Statement, "if true { } else blah", ast::If,
                   Expected2(loc, Token::kIf, Token::kLeftBrace));
PARSE_NODE_FAILURE(Statement, "switch x 3", ast::Switch,
                   Expected2(loc, Token::kLeftBrace, Token::kSemicolon));
PARSE_LIST_FAILURE(StatementList, "s; func; v;",
                   Expected1(loc, Token::kLeftParen));
PARSE_LIST_FAILURE(StatementList, "s; t u; v;",
                   Expected1(loc, Token::kSemicolon));
PARSE_NODE_FAILURE(Type, "interface { 4 4 4 ; f() }", ast::Interface,
                   ExpectedAMethodSpecification(loc));
PARSE_NODE_FAILURE(Type, "interface { f() type", ast::Interface,
                   Expected1(loc, Token::kSemicolon));
PARSE_NODE_FAILURE(Type, "interface { f(); x + y; g() }", ast::Interface,
                   Expected1(loc, Token::kLeftParen));
PARSE_NODE_FAILURE(Type, "struct { 4 4 4 ; x T }", ast::Struct,
                   ExpectedAFieldDeclaration(loc));
PARSE_NODE_FAILURE(Type, "struct { x T; x + y; y T }", ast::Struct,
                   ExpectedAType(loc));
PARSE_NODE_FAILURE(TypeDecl, "type( T int; U float func; V float )",
                   ast::TypeDecl, Expected1(loc, Token::kRightParen));
PARSE_NODE_FAILURE(TypeDecl, "type( func )", ast::TypeDecl,
                   Expected1(loc, Token::kIdentifier));
PARSE_NODE_FAILURE(TypeDecl, "type 4", ast::TypeDecl,
                   ExpectedASpecificationOrGroup(loc));
PARSE_NODE_FAILURE(VarDecl, "var( func )", ast::VarDecl,
                   Expected1(loc, Token::kIdentifier));
PARSE_NODE_FAILURE(VarDecl, "var( x = 1; y T = 1 a; z = 3 )", ast::VarDecl,
                   Expected1(loc, Token::kRightParen));
PARSE_NODE_FAILURE(VarDecl, "var 4", ast::VarDecl,
                   ExpectedASpecificationOrGroup(loc));

PARSE_NULL(Declaration, "not_a_decl", ExpectedADeclaration(loc));
PARSE_NULL(FieldDecl, "4", ExpectedAFieldDeclaration(loc));
PARSE_NULL(MethodSpec, "4()", ExpectedAMethodSpecification(loc));
PARSE_NULL(TopLevelDecl, "not_a_decl", ExpectedADeclaration(loc));

TEST_CASE("Parser::Parser(...)") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  StringByteStream seq("");
  Parser parser(&seq, &er, ss);
  REQUIRE(parser.trace() == false);
}

TEST_CASE("Parser::SetTrace()") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  StringByteStream seq("");
  Parser parser(&seq, &er, ss);
  parser.trace(true);
  REQUIRE(parser.trace() == true);
}

TEST_CASE("Parser tracing") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  StringByteStream seq("x");
  Parser parser(&seq, &er, ss);
  parser.trace(true);
  ast::Node* node = parser.Operand();
  REQUIRE(node != NULL);
  delete node;
  REQUIRE(ss.str() == "Consumed x\n");
}

}  // namespace test
}  // namespace rcgo
