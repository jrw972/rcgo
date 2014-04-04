/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "Parser.hpp"
#include "Ast.hpp"
#include "spies.hpp"
#include "ByteStream.hpp"
#include "Error.hpp"

using namespace rcgo;

#define PARSE_SUCCESS(method, str, args...) TEST_CASE ("Parser::" #method "(" #args ")" " on \"" str "\" success") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (str);                                         \
    Parser parser (seq, er, ss);                                        \
    parser.trace (true);                                                \
    Ast* node = parser.method (args);                                   \
    REQUIRE (er.empty ());                                              \
    REQUIRE (node != NULL);                                             \
    delete node;                                                        \
}

#define PARSE_FAILURE(method, str, type, err, args...) TEST_CASE ("Parser::" #method "(" #args ")" " on \"" str "\" failure") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 0, abortHandler);                             \
    StringByteStream seq (str);                                         \
    Parser parser (seq, er, ss);                                        \
    Ast* node = parser.method (args);                                   \
    REQUIRE (er.at (0) == std::string (err));                           \
    REQUIRE (node != NULL);                                             \
    REQUIRE (dynamic_cast<type*> (node) != NULL);                       \
    delete node;                                                        \
}

#define PARSE_NULL(method, str, err, args...) TEST_CASE ("Parser::" #method "(" #args ")" " on \"" str "\" failure") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 0, abortHandler);                             \
    StringByteStream seq (str);                                         \
    Parser parser (seq, er, ss);                                        \
    Ast* node = parser.method (args);                                   \
    REQUIRE (er.at (0) == std::string (err));                           \
    REQUIRE (node == NULL);                                             \
}

PARSE_SUCCESS (AddExpr, "x + y");
PARSE_SUCCESS (AddExpr, "x - y");
PARSE_SUCCESS (AddExpr, "x ^ y");
PARSE_SUCCESS (AddExpr, "x | y");
PARSE_SUCCESS (AndExpr, "x && y");
PARSE_SUCCESS (Block, "{ m := Bar{ii: x}\n return m.ii\n }");
PARSE_SUCCESS (Block, "{;}");
PARSE_SUCCESS (Block, "{}");
PARSE_SUCCESS (ConstDecl, "const (x, y, z; a = 4)");
PARSE_SUCCESS (ConstDecl, "const x");
PARSE_SUCCESS (ConstSpec, "x int = 4");
PARSE_SUCCESS (Declaration, "const ()");
PARSE_SUCCESS (Declaration, "type ()");
PARSE_SUCCESS (Declaration, "var ()");
PARSE_SUCCESS (ExpressionList, "1, 2, 3");
PARSE_SUCCESS (FieldDecl, "*T `tag`");
PARSE_SUCCESS (FieldDecl, "*T");
PARSE_SUCCESS (FieldDecl, "S.T `tag`;");
PARSE_SUCCESS (FieldDecl, "T `tag`");
PARSE_SUCCESS (FieldDecl, "T");
PARSE_SUCCESS (FieldDecl, "apackage.T");
PARSE_SUCCESS (FieldDecl, "x, y, z T `tag`");
PARSE_SUCCESS (FuncDecl, "myfunc () { }"); // NOTE: no func
PARSE_SUCCESS (IdentifierList, "x, y, z");
PARSE_SUCCESS (ImportDecl, "import . \"math\"");
PARSE_SUCCESS (ImportDecl, "import mymath \"math\"");
PARSE_SUCCESS (ImportSpec, ". \"math\"");
PARSE_SUCCESS (ImportSpec, "mymath \"math\"");
PARSE_SUCCESS (KeyedElement, "identifier");
PARSE_SUCCESS (KeyedElement, "x: { 0 }");
PARSE_SUCCESS (KeyedElement, "{ 0 }");
PARSE_SUCCESS (LiteralValue, "{ 1, 2, 3, }");
PARSE_SUCCESS (LiteralValue, "{ }");
PARSE_SUCCESS (MethodDecl, "(this *T) mymethod () { }"); // NOTE: no func
PARSE_SUCCESS (MethodSpec, "MyMethod ()");
PARSE_SUCCESS (MulExpr, "x % y");
PARSE_SUCCESS (MulExpr, "x & y");
PARSE_SUCCESS (MulExpr, "x &^ y");
PARSE_SUCCESS (MulExpr, "x * y");
PARSE_SUCCESS (MulExpr, "x / y");
PARSE_SUCCESS (MulExpr, "x << y");
PARSE_SUCCESS (MulExpr, "x >> y");
PARSE_SUCCESS (Operand, "'a'");
PARSE_SUCCESS (Operand, "(x)");
PARSE_SUCCESS (Operand, "1.0");
PARSE_SUCCESS (Operand, "1.0i");
PARSE_SUCCESS (Operand, "T { 1, 2, 3 }");
PARSE_SUCCESS (Operand, "[...]int { 1, 2, 3 }");
PARSE_SUCCESS (Operand, "[3]int { 1, 2, 3 }");
PARSE_SUCCESS (Operand, "[]int { 1, 2, 3 }");
PARSE_SUCCESS (Operand, "\"string\"");
PARSE_SUCCESS (Operand, "func () { }");
PARSE_SUCCESS (OrExpr, "x || y");
PARSE_SUCCESS (OrExpr, "x");
PARSE_SUCCESS (Parameters, "(T1, T2, T3)");
PARSE_SUCCESS (Parameters, "(interface{})");
PARSE_SUCCESS (Parameters, "(x T1, y T2, z T3,)");
PARSE_SUCCESS (Parameters, "(x, y T1, z T2)");
PARSE_SUCCESS (Parameters, "(x, y, z,)");
PARSE_SUCCESS (PrimaryExpr, "[3] int { 1, 2, 3}");
PARSE_SUCCESS (PrimaryExpr, "f ()");
PARSE_SUCCESS (PrimaryExpr, "f (x ...)");
PARSE_SUCCESS (PrimaryExpr, "f (x)");
PARSE_SUCCESS (PrimaryExpr, "f (x,)");
PARSE_SUCCESS (PrimaryExpr, "f (x...,)");
PARSE_SUCCESS (PrimaryExpr, "x");
PARSE_SUCCESS (PrimaryExpr, "x.(T)");
PARSE_SUCCESS (PrimaryExpr, "x.y");
PARSE_SUCCESS (PrimaryExpr, "x.y.z");
PARSE_SUCCESS (PrimaryExpr, "x[4:5:6]");
PARSE_SUCCESS (PrimaryExpr, "x[4:5]");
PARSE_SUCCESS (PrimaryExpr, "x[4:]");
PARSE_SUCCESS (PrimaryExpr, "x[4]");
PARSE_SUCCESS (PrimaryExpr, "x[:4:5]");
PARSE_SUCCESS (PrimaryExpr, "x[:4]");
PARSE_SUCCESS (PrimaryExpr, "x[:]");
PARSE_SUCCESS (RelExpr, "x != y");
PARSE_SUCCESS (RelExpr, "x < y");
PARSE_SUCCESS (RelExpr, "x <= y");
PARSE_SUCCESS (RelExpr, "x == y");
PARSE_SUCCESS (RelExpr, "x > y");
PARSE_SUCCESS (RelExpr, "x >= y");
PARSE_SUCCESS (Signature, "() ()");
PARSE_SUCCESS (Signature, "() int");
PARSE_SUCCESS (SimpleStmt, "x := y");
PARSE_SUCCESS (SourceFile, "package mypackage");
PARSE_SUCCESS (SourceFile, "package mypackage; import (\"math\")");
PARSE_SUCCESS (SourceFile, "package mypackage; import (\"math\";)");
PARSE_SUCCESS (SourceFile, "package mypackage; import \"math\"");
PARSE_SUCCESS (SourceFile, "package mypackage; var x T");
PARSE_SUCCESS (SourceFileHeader, "package mypackage");
PARSE_SUCCESS (SourceFileHeader, "package mypackage; import (\"math\")");
PARSE_SUCCESS (SourceFileHeader, "package mypackage; import (\"math\";)");
PARSE_SUCCESS (SourceFileHeader, "package mypackage; import \"math\"");
PARSE_SUCCESS (Statement, "break OuterLoop");
PARSE_SUCCESS (Statement, "break");
PARSE_SUCCESS (Statement, "continue OuterLoop");
PARSE_SUCCESS (Statement, "continue");
PARSE_SUCCESS (Statement, "defer f (x)");
PARSE_SUCCESS (Statement, "done: f(x)");
PARSE_SUCCESS (Statement, "fallthrough");
PARSE_SUCCESS (Statement, "for  ;  ;   { }");
PARSE_SUCCESS (Statement, "for  ;  ; z { }");
PARSE_SUCCESS (Statement, "for  ; y;   { }");
PARSE_SUCCESS (Statement, "for  ; y; z { }");
PARSE_SUCCESS (Statement, "for ;; { }");
PARSE_SUCCESS (Statement, "for range x { }");
PARSE_SUCCESS (Statement, "for x { }");
PARSE_SUCCESS (Statement, "for x;  ;   { }");
PARSE_SUCCESS (Statement, "for x;  ; z { }");
PARSE_SUCCESS (Statement, "for x; y;   { }");
PARSE_SUCCESS (Statement, "for x; y; z { }");
PARSE_SUCCESS (Statement, "for y := range x { }");
PARSE_SUCCESS (Statement, "for y = range x { }");
PARSE_SUCCESS (Statement, "for { }");
PARSE_SUCCESS (Statement, "goto done");
PARSE_SUCCESS (Statement, "if true { } else if false { }");
PARSE_SUCCESS (Statement, "if true { } else { }");
PARSE_SUCCESS (Statement, "if true { }");
PARSE_SUCCESS (Statement, "if x := y; true { }");
PARSE_SUCCESS (Statement, "return x, y");
PARSE_SUCCESS (Statement, "return");
PARSE_SUCCESS (Statement, "switch x { }");
PARSE_SUCCESS (Statement, "switch x; y { }");
PARSE_SUCCESS (Statement, "switch x; { }");
PARSE_SUCCESS (Statement, "switch { case 1, 2, 3: }");
PARSE_SUCCESS (Statement, "switch { default: }");
PARSE_SUCCESS (Statement, "switch { }");
PARSE_SUCCESS (Statement, "var x T");
PARSE_SUCCESS (Statement, "x %= y");
PARSE_SUCCESS (Statement, "x &= y");
PARSE_SUCCESS (Statement, "x &^= y");
PARSE_SUCCESS (Statement, "x *= y");
PARSE_SUCCESS (Statement, "x += y");
PARSE_SUCCESS (Statement, "x -= y");
PARSE_SUCCESS (Statement, "x /= y");
PARSE_SUCCESS (Statement, "x := y");
PARSE_SUCCESS (Statement, "x <<= y");
PARSE_SUCCESS (Statement, "x = y");
PARSE_SUCCESS (Statement, "x >>= y");
PARSE_SUCCESS (Statement, "x ^= y");
PARSE_SUCCESS (Statement, "x |= y");
PARSE_SUCCESS (Statement, "x++");
PARSE_SUCCESS (Statement, "x--");
PARSE_SUCCESS (Statement, "{ }");
PARSE_SUCCESS (StatementList, "s}");
PARSE_SUCCESS (SwitchStmt, "switch val := i.(type) {\ncase *int:\n  return val\ncase *int8:\n  v := int(*val)\n  return &v\n}");
PARSE_SUCCESS (TopLevelDecl, "const ()");
PARSE_SUCCESS (TopLevelDecl, "func () method ()");
PARSE_SUCCESS (TopLevelDecl, "func function ()");
PARSE_SUCCESS (TopLevelDecl, "type ()");
PARSE_SUCCESS (TopLevelDecl, "var ()");
PARSE_SUCCESS (Type, "(int)");
PARSE_SUCCESS (Type, "*int");
PARSE_SUCCESS (Type, "[3]int");
PARSE_SUCCESS (Type, "[]int");
PARSE_SUCCESS (Type, "apackage.identifier");
PARSE_SUCCESS (Type, "func ()");
PARSE_SUCCESS (Type, "identifier");
PARSE_SUCCESS (Type, "interface { X; Y }");
PARSE_SUCCESS (Type, "interface {}");
PARSE_SUCCESS (Type, "map[int]float { 0: 1, 1: 2 }");
PARSE_SUCCESS (Type, "map[string]int");
PARSE_SUCCESS (Type, "struct { x int; y float }");
PARSE_SUCCESS (Type, "struct {} { 1, 2, 3 }");
PARSE_SUCCESS (Type, "struct {}");
PARSE_SUCCESS (TypeDecl, "type (T int; U float)");
PARSE_SUCCESS (TypeDecl, "type T int");
PARSE_SUCCESS (UnaryExpr, "!x");
PARSE_SUCCESS (UnaryExpr, "&x");
PARSE_SUCCESS (UnaryExpr, "*x");
PARSE_SUCCESS (UnaryExpr, "+x");
PARSE_SUCCESS (UnaryExpr, "-x");
PARSE_SUCCESS (UnaryExpr, "^x");
PARSE_SUCCESS (UnaryExpr, "x");
PARSE_SUCCESS (VarDecl, "var (x int; y float = 3; z = 5)");
PARSE_SUCCESS (VarDecl, "var x int");

Location loc = Location::make ("undefined", 1);

PARSE_FAILURE (ConstDecl, "const ( func )", ListAst, Expected1 (loc, Token::Identifier));
PARSE_FAILURE (ConstDecl, "const ( x; x T = 1 y; y )", ListAst, Expected1 (loc, Token::RightParen));
PARSE_FAILURE (ConstDecl, "const 4", ListAst, ExpectedASpecificationOrGroup (loc));
PARSE_FAILURE (ConstSpec, "x int = 4, 3", ConstSpecAst, ExpectedNExpressions (loc, 1));
PARSE_FAILURE (ImportDecl, "import ( `x`; `y` func; `z` )", ListAst, Expected1 (loc, Token::RightParen));
PARSE_FAILURE (ImportDecl, "import ( func )", ListAst, Expected1 (loc, Token::RightParen));
PARSE_FAILURE (LiteralValue, "{ 1, func, 2 }", ListAst, Expected1 (loc, Token::LeftParen));
PARSE_FAILURE (Operand, "+", ErrorAst, ExpectedAnOperand (loc));
PARSE_FAILURE (Parameters, "(c chan *int)", ListAst, Expected1 (loc, Token::RightParen))
PARSE_FAILURE (Parameters, "(x T, 4, y T)", ListAst, Expected1 (loc, Token::Identifier));
PARSE_FAILURE (Parameters, "(x T, x 4, y T)", ListAst, ExpectedAType (loc));
PARSE_FAILURE (Parameters, "(x, *y, z T)", ListAst, Expected1 (loc, Token::Identifier));
PARSE_FAILURE (Parameters, "(x, y ... T1, z ... T2)", ListAst, IllegalVariadicParameter (loc));
PARSE_FAILURE (PrimaryExpr, "x[4 type]", SliceAst, Expected1 (loc, Token::Colon));
PARSE_FAILURE (PrimaryExpr, "x[4:5 type]", SliceAst, InvalidSlice (loc));
PARSE_FAILURE (PrimaryExpr, "x[:4 type]", SliceAst, InvalidSlice (loc));
PARSE_FAILURE (SimpleStmt, "x, y", ExpressionStatementAst, UnexpectedExpressionList (loc));
PARSE_FAILURE (SimpleStmt, "x, y++", IncrementAst, UnexpectedExpressionList (loc));
PARSE_FAILURE (SimpleStmt, "x, y--", DecrementAst, UnexpectedExpressionList (loc));
PARSE_FAILURE (SourceFile, "package mypackage; import func; var x T", SourceFileAst, ExpectedASpecificationOrGroup (loc));
PARSE_FAILURE (SourceFile, "package mypackage; range; var x T", SourceFileAst, ExpectedADeclaration (loc));
PARSE_FAILURE (Statement, "if true { } else blah", IfAst, Expected2 (loc, Token::If, Token::LeftBrace));
PARSE_FAILURE (Statement, "switch x 3", SwitchAst, Expected2 (loc, Token::LeftBrace, Token::Semicolon));
PARSE_FAILURE (StatementList, "s; func; v;", ListAst, Expected1 (loc, Token::LeftParen));
PARSE_FAILURE (StatementList, "s; t u; v;", ListAst, Expected1 (loc, Token::Semicolon));
PARSE_FAILURE (Type, "interface { 4 4 4 ; f () }", ListAst, ExpectedAMethodSpecification (loc));
PARSE_FAILURE (Type, "interface { f () type", ListAst, Expected1 (loc, Token::Semicolon));
PARSE_FAILURE (Type, "interface { f (); x + y; g () }", ListAst, Expected1 (loc, Token::LeftParen));
PARSE_FAILURE (Type, "struct { 4 4 4 ; x T }", StructAst, ExpectedAFieldDeclaration (loc));
PARSE_FAILURE (Type, "struct { x T; x + y; y T }", StructAst, ExpectedAType (loc));
PARSE_FAILURE (TypeDecl, "type ( T int; U float func; V float )", ListAst, Expected1 (loc, Token::RightParen));
PARSE_FAILURE (TypeDecl, "type ( func )", ListAst, Expected1 (loc, Token::Identifier));
PARSE_FAILURE (TypeDecl, "type 4", ListAst, ExpectedASpecificationOrGroup (loc));
PARSE_FAILURE (VarDecl, "var ( func )", ListAst, Expected1 (loc, Token::Identifier));
PARSE_FAILURE (VarDecl, "var ( x = 1; y T = 1 a; z = 3 )", ListAst, Expected1 (loc, Token::RightParen));
PARSE_FAILURE (VarDecl, "var 4", ListAst, ExpectedASpecificationOrGroup (loc));

PARSE_NULL (Declaration, "not_a_decl", ExpectedADeclaration (loc));
PARSE_NULL (FieldDecl, "4", ExpectedAFieldDeclaration (loc));
PARSE_NULL (MethodSpec, "4 ()", ExpectedAMethodSpecification (loc));
PARSE_NULL (TopLevelDecl, "not_a_decl", ExpectedADeclaration (loc));

TEST_CASE ("Parser::Parser (...)") {
  std::stringstream ss;
  ErrorReporter er (ss, 1, abortHandler);
  StringByteStream seq ("");
  Parser parser (seq, er, ss);
  REQUIRE (parser.trace () == false);
}

TEST_CASE ("Parser::SetTrace ()") {
  std::stringstream ss;
  ErrorReporter er (ss, 1, abortHandler);
  StringByteStream seq ("");
  Parser parser (seq, er, ss);
  parser.trace (true);
  REQUIRE (parser.trace () == true);
}

TEST_CASE ("Parser tracing") {
  std::stringstream ss;
  ErrorReporter er (ss, 1, abortHandler);
  StringByteStream seq ("x");
  Parser parser (seq, er, ss);
  parser.trace (true);
  Ast* node = parser.Operand ();
  REQUIRE (node != NULL);
  delete node;
  REQUIRE (ss.str () == "Consumed x\n");
}
