// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/scanner.h"

#include "src/catch.hpp"
#include "src/test_helpers.h"

namespace rcgo {
namespace test {

#define EMPTY_TEST(str) TEST_CASE("Scanner::Peek() on \"" str "\"") {   \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(str);                                          \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(t.kind() == Token::kEnd);                                   \
  }

#define IDENTIFIER_TEST(id) TEST_CASE("Scanner::Peek() on \"" id "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(id);                                           \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(t.kind() == Token::kIdentifier);                            \
    REQUIRE(t.identifier() == id);                                      \
  }

#define KEYWORD_TEST(keyword, token)                                    \
  TEST_CASE("Scanner::Peek() on \"" keyword "\"") {                     \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(keyword);                                      \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(t.kind() == token);                                         \
  }

#define OPERATOR_DELIMITER_TEST(od, token)                              \
  TEST_CASE("Scanner::Peek() on \"" od "\"") {                          \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(od);                                           \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(t.kind() == token);                                         \
  }

#define OPERATOR_DELIMITER_TEST2(od, token, getter, x)                  \
  TEST_CASE("Scanner::Peek() on \"" od "\"") {                          \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(od);                                           \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(t.kind() == token);                                         \
    REQUIRE(t.getter() == x);                                           \
  }

#define INTEGER_LITERAL_TEST(literal, val)                              \
  TEST_CASE("Scanner::Peek() on \"" literal "\"") {                     \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(literal);                                      \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(t.kind() == Token::kLiteral);                               \
    REQUIRE(t.value().kind() == value::Value::kInteger);               \
    REQUIRE(t.value().Integer_value() == val);                         \
  }

#define FLOATING_LITERAL_TEST(literal, val)                             \
  TEST_CASE("Scanner::Peek() on \"" literal "\"") {                     \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(literal);                                      \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(t.kind() == Token::kLiteral);                               \
    REQUIRE(t.value().kind() == value::Value::kFloat);                 \
    REQUIRE(fabs(t.value().Float_value().get_d() - val) < .0000001);   \
  }

#define IMAGINARY_LITERAL_TEST(literal, val)                            \
  TEST_CASE("Scanner::Peek() on \"" literal "\"") {                     \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(literal);                                      \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(t.kind() == Token::kLiteral);                               \
    REQUIRE(t.value().kind() == value::Value::kComplex);                \
    REQUIRE(fabs(t.value().Complex_value().imag().get_d() - val) < .0000001); \
  }

#define RUNE_LITERAL_TEST(literal, val)                                 \
  TEST_CASE("Scanner::Peek() on \"" literal "\"") {                     \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(literal);                                      \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(t.kind() == Token::kLiteral);                               \
    REQUIRE(t.value().kind() == value::Value::kRune);                   \
    REQUIRE(t.value().Rune_value() == val);                             \
  }

#define RUNE_LITERAL_TEST_ERROR(literal, err)                           \
  TEST_CASE("Scanner::Peek() on \"" literal "\"") {                     \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 0, &abort_handler);                            \
    StringByteStream seq(literal);                                      \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(er.At(0) == std::string(err));                              \
    REQUIRE(t.kind() == Token::kLiteral);                               \
    REQUIRE(t.value().kind() == value::Value::kRune);                   \
  }

#define STRING_LITERAL_TEST(literal, val)                               \
  TEST_CASE("Scanner::Peek() on \"" literal "\"") {                     \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &abort_handler);                            \
    StringByteStream seq(literal);                                      \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(t.kind() == Token::kLiteral);                               \
    REQUIRE(t.value().kind() == value::Value::kString);                 \
    REQUIRE(t.value().String_value() == val);                           \
  }

#define STRING_LITERAL_TEST_ERROR(literal, err)                         \
  TEST_CASE("Scanner::Peek() on \"" literal "\"") {                     \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 0, &abort_handler);                            \
    StringByteStream seq(literal);                                      \
    Scanner scanner(&seq, &er);                                         \
    Token t = scanner.Peek();                                           \
    REQUIRE(er.At(0) == std::string(err));                              \
    REQUIRE(t.kind() == Token::kLiteral);                               \
    REQUIRE(t.value().kind() == value::Value::kString);                 \
  }

#define CONSUME_ERROR(str, k, err)                                      \
  TEST_CASE("Scanner::Consume() on \"" str "\"") {                      \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 0, &abort_handler);                            \
    StringByteStream seq(str);                                          \
    Scanner scanner(&seq, &er);                                         \
    Token t0 = scanner.Consume();                                       \
    REQUIRE(er.At(0) == std::string(err));                              \
    REQUIRE(t0.kind() == k);                                            \
  }

EMPTY_TEST("");
EMPTY_TEST("// This is a comment");
EMPTY_TEST("/* This is a comment */");

IDENTIFIER_TEST("a");
IDENTIFIER_TEST("_x9");
IDENTIFIER_TEST("ThisVariableIsExported");
IDENTIFIER_TEST("αβ");

KEYWORD_TEST("break", Token::kBreak);
KEYWORD_TEST("case", Token::kCase);
KEYWORD_TEST("const", Token::kConst);
KEYWORD_TEST("continue", Token::kContinue);
KEYWORD_TEST("default", Token::kDefault);
KEYWORD_TEST("defer", Token::kDefer);
KEYWORD_TEST("else", Token::kElse);
KEYWORD_TEST("fallthrough", Token::kFallthrough);
KEYWORD_TEST("for", Token::kFor);
KEYWORD_TEST("func", Token::kFunc);
KEYWORD_TEST("goto", Token::kGoto);
KEYWORD_TEST("if", Token::kIf);
KEYWORD_TEST("import", Token::kImport);
KEYWORD_TEST("interface", Token::kInterface);
KEYWORD_TEST("map", Token::kMap);
KEYWORD_TEST("package", Token::kPackage);
KEYWORD_TEST("range", Token::kRange);
KEYWORD_TEST("return", Token::kReturn);
KEYWORD_TEST("struct", Token::kStruct);
KEYWORD_TEST("switch", Token::kSwitch);
KEYWORD_TEST("type", Token::kType);
KEYWORD_TEST("var", Token::kVar);

OPERATOR_DELIMITER_TEST2("+", Token::kUnaryOp, unary_op_kind, Token::kPosate);
OPERATOR_DELIMITER_TEST2("-", Token::kUnaryOp, unary_op_kind, Token::kNegate);
OPERATOR_DELIMITER_TEST("*", Token::kPointer);
OPERATOR_DELIMITER_TEST2("/", Token::kMulOp, mul_op_kind, Token::kDivide);
OPERATOR_DELIMITER_TEST2("%", Token::kMulOp, mul_op_kind, Token::kModulus);

OPERATOR_DELIMITER_TEST2("&", Token::kUnaryOp, unary_op_kind, Token::kAddress);
OPERATOR_DELIMITER_TEST2("|", Token::kAddOp, add_op_kind, Token::kBitOr);
OPERATOR_DELIMITER_TEST2("^", Token::kUnaryOp, unary_op_kind, Token::kBitNot);
OPERATOR_DELIMITER_TEST2("<<", Token::kMulOp, mul_op_kind, Token::kLeftShift);
OPERATOR_DELIMITER_TEST2(">>", Token::kMulOp, mul_op_kind, Token::kRightShift);
OPERATOR_DELIMITER_TEST2("&^", Token::kMulOp, mul_op_kind, Token::kBitAndNot);

OPERATOR_DELIMITER_TEST2("+=", Token::kAssignOp, assign_op_kind,
                         Token::kAddAssign);
OPERATOR_DELIMITER_TEST2("-=", Token::kAssignOp, assign_op_kind,
                         Token::kSubtractAssign);
OPERATOR_DELIMITER_TEST2("*=", Token::kAssignOp, assign_op_kind,
                         Token::kMultiplyAssign);
OPERATOR_DELIMITER_TEST2("/=", Token::kAssignOp, assign_op_kind,
                         Token::kDivideAssign);
OPERATOR_DELIMITER_TEST2("%=", Token::kAssignOp, assign_op_kind,
                         Token::kModulusAssign);

OPERATOR_DELIMITER_TEST2("&=", Token::kAssignOp, assign_op_kind,
                         Token::kBitAndAssign);
OPERATOR_DELIMITER_TEST2("|=", Token::kAssignOp, assign_op_kind,
                         Token::kBitOrAssign);
OPERATOR_DELIMITER_TEST2("^=", Token::kAssignOp, assign_op_kind,
                         Token::kBitXorAssign);
OPERATOR_DELIMITER_TEST2("<<=", Token::kAssignOp, assign_op_kind,
                         Token::kLeftShiftAssign);
OPERATOR_DELIMITER_TEST2(">>=", Token::kAssignOp, assign_op_kind,
                         Token::kRightShiftAssign);
OPERATOR_DELIMITER_TEST2("&^=", Token::kAssignOp, assign_op_kind,
                         Token::kBitAndNotAssign);

OPERATOR_DELIMITER_TEST("&&", Token::kLogicalAnd);
OPERATOR_DELIMITER_TEST("||", Token::kLogicalOr);
OPERATOR_DELIMITER_TEST("++", Token::kIncrement);
OPERATOR_DELIMITER_TEST("--", Token::kDecrement);

OPERATOR_DELIMITER_TEST2("==", Token::kRelOp, rel_op_kind, Token::kEqual);
OPERATOR_DELIMITER_TEST2("<", Token::kRelOp, rel_op_kind, Token::kLessThan);
OPERATOR_DELIMITER_TEST2(">", Token::kRelOp, rel_op_kind, Token::kGreaterThan);
OPERATOR_DELIMITER_TEST("=", Token::kAssign);
OPERATOR_DELIMITER_TEST2("!", Token::kUnaryOp, unary_op_kind,
                         Token::kLogicalNot);

OPERATOR_DELIMITER_TEST2("!=", Token::kRelOp, rel_op_kind, Token::kNotEqual);
OPERATOR_DELIMITER_TEST2("<=", Token::kRelOp, rel_op_kind,
                         Token::kLessThanEqual);
OPERATOR_DELIMITER_TEST2(">=", Token::kRelOp, rel_op_kind,
                         Token::kGreaterThanEqual);
OPERATOR_DELIMITER_TEST(":=", Token::kShortAssign);
OPERATOR_DELIMITER_TEST("...", Token::kEllipsis);

OPERATOR_DELIMITER_TEST("(", Token::kLeftParen);
OPERATOR_DELIMITER_TEST("[", Token::kLeftBracket);
OPERATOR_DELIMITER_TEST("{", Token::kLeftBrace);
OPERATOR_DELIMITER_TEST(",", Token::kComma);
OPERATOR_DELIMITER_TEST(".", Token::kPeriod);

OPERATOR_DELIMITER_TEST(")", Token::kRightParen);
OPERATOR_DELIMITER_TEST("]", Token::kRightBracket);
OPERATOR_DELIMITER_TEST("}", Token::kRightBrace);
OPERATOR_DELIMITER_TEST(";", Token::kSemicolon);
OPERATOR_DELIMITER_TEST(":", Token::kColon);

INTEGER_LITERAL_TEST("42", 42);
INTEGER_LITERAL_TEST("0600", 0600);
INTEGER_LITERAL_TEST("0xBadFace", 0xBadFace);
INTEGER_LITERAL_TEST("0x1234", 0x1234);
INTEGER_LITERAL_TEST("170141183460469231731687303715884105727",
                     mpz_class("170141183460469231731687303715884105727"));

FLOATING_LITERAL_TEST("0.", 0);
FLOATING_LITERAL_TEST("72.40", 72.40);
FLOATING_LITERAL_TEST("072.40", 72.40);
FLOATING_LITERAL_TEST("2.71828", 2.71828);
FLOATING_LITERAL_TEST("1.e+0", 1.0);
FLOATING_LITERAL_TEST("6.67428e-11", 6.67428e-11);
FLOATING_LITERAL_TEST("1E6", 1E6);
FLOATING_LITERAL_TEST("0123E-6", 123E-6);
FLOATING_LITERAL_TEST(".25", .25);
FLOATING_LITERAL_TEST(".12345E+5", .12345E+5);

IMAGINARY_LITERAL_TEST("0i", 0);
IMAGINARY_LITERAL_TEST("011i", 11);
IMAGINARY_LITERAL_TEST("0.i", 0);
IMAGINARY_LITERAL_TEST("2.71828i", 2.71828);
IMAGINARY_LITERAL_TEST("1.e+0i", 1.0);
IMAGINARY_LITERAL_TEST("6.67428e-11i", 6.67428e-11);
IMAGINARY_LITERAL_TEST("1E6i", 1E6);
IMAGINARY_LITERAL_TEST(".25i", .25);
IMAGINARY_LITERAL_TEST(".12345E+5i", .12345E+5);

RUNE_LITERAL_TEST("'a'", 'a');
RUNE_LITERAL_TEST("'ä'", 228);
RUNE_LITERAL_TEST("'本'", 26412);
RUNE_LITERAL_TEST("'\\a'", '\a');
RUNE_LITERAL_TEST("'\\b'", '\b');
RUNE_LITERAL_TEST("'\\f'", '\f');
RUNE_LITERAL_TEST("'\\n'", '\n');
RUNE_LITERAL_TEST("'\\r'", '\r');
RUNE_LITERAL_TEST("'\\t'", '\t');
RUNE_LITERAL_TEST("'\\v'", '\v');
RUNE_LITERAL_TEST("'\\\\'", '\\');
RUNE_LITERAL_TEST("'\\000'", 0);
RUNE_LITERAL_TEST("'\\007'", 007);
RUNE_LITERAL_TEST("'\\377'", 0377);
RUNE_LITERAL_TEST("'\\x07'", 0x07);
RUNE_LITERAL_TEST("'\\xff'", 0xFF);
RUNE_LITERAL_TEST("'\\u12e4'", 0x12E4);
RUNE_LITERAL_TEST("'\\U00101234'", 0x00101234);
RUNE_LITERAL_TEST("'\\''", '\'');
RUNE_LITERAL_TEST_ERROR("'aa'", ExtraCharactersInRuneLiteral(location));
RUNE_LITERAL_TEST_ERROR("'\\xa'",
                        TooFewHexadecimalDigitsInRuneLiteral(location));
RUNE_LITERAL_TEST_ERROR("'\\ua'",
                        TooFewHexadecimalDigitsInRuneLiteral(location));
RUNE_LITERAL_TEST_ERROR("'\\Ua'",
                        TooFewHexadecimalDigitsInRuneLiteral(location));
RUNE_LITERAL_TEST_ERROR("'\\0'", TooFewOctalDigitsInRuneLiteral(location));
RUNE_LITERAL_TEST_ERROR("'\\777'", OctalValueOutOfRange(location));
RUNE_LITERAL_TEST_ERROR("'\\uDFFF'", InvalidUnicodeCodePoint(location));
RUNE_LITERAL_TEST_ERROR("'\\U00110000'", InvalidUnicodeCodePoint(location));
RUNE_LITERAL_TEST_ERROR("'", IncompleteRuneLiteral(location));
RUNE_LITERAL_TEST_ERROR("''", EmptyRuneLiteral(location));
RUNE_LITERAL_TEST_ERROR("'a\n", IncompleteRuneLiteral(location));
RUNE_LITERAL_TEST_ERROR("'\\c'", IllegalEscapeSequence(location));
RUNE_LITERAL_TEST_ERROR("'\\\"'", IllegalEscapeSequence(location));

STRING_LITERAL_TEST("`abc`", "abc");
STRING_LITERAL_TEST("`\\n\n\\n`", "\\n\n\\n");
STRING_LITERAL_TEST("\"\\n\"", "\n");
STRING_LITERAL_TEST("\"\\\"\"", "\"");
STRING_LITERAL_TEST("\"Hello, world!\\n\"", "Hello, world!\n");
STRING_LITERAL_TEST("\"日本語\"", "日本語");
STRING_LITERAL_TEST("`日本語`", "日本語");
STRING_LITERAL_TEST("\"\\u65e5\\u672c\\u8a9e\"", "日本語");
STRING_LITERAL_TEST("\"\\U000065e5\\U0000672c\\U00008a9e\"", "日本語");
STRING_LITERAL_TEST("\"\\xe6\\x97\\xa5\\xe6\\x9c\\xac\\xe8\\xaa\\x9e\"",
                    "日本語");
STRING_LITERAL_TEST("\"\u65e5本\U00008a9ea\"", "日本語a");
STRING_LITERAL_TEST("\"\\xff\\u00FF\"", "\xff\xc3\xbf");
STRING_LITERAL_TEST("`\r`", "");
STRING_LITERAL_TEST_ERROR("\"\\uD800\"", InvalidUnicodeCodePoint(location));
STRING_LITERAL_TEST_ERROR("\"\\U00110000\"", InvalidUnicodeCodePoint(location));
STRING_LITERAL_TEST_ERROR("\"\\'\"", IllegalEscapeSequence(location));
STRING_LITERAL_TEST_ERROR("`", IncompleteStringLiteral(location));
STRING_LITERAL_TEST_ERROR("\"", IncompleteStringLiteral(location));
STRING_LITERAL_TEST_ERROR("\"\n\"", IncompleteStringLiteral(location));

TEST_CASE("Scanner::Consume() on \"y = m * x + b // equation of a line\"") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  StringByteStream seq("y = m * x + b // equation of a line");
  Scanner scanner(&seq, &er);
  Token t0 = scanner.Consume();
  Token t1 = scanner.Consume();
  Token t2 = scanner.Consume();
  Token t3 = scanner.Consume();
  Token t4 = scanner.Consume();
  Token t5 = scanner.Consume();
  Token t6 = scanner.Consume();
  Token t7 = scanner.Consume();
  Token t8 = scanner.Consume();
  REQUIRE(t0.kind() == Token::kIdentifier);
  REQUIRE(t0.identifier() == "y");
  REQUIRE(t1.kind() == Token::kAssign);
  REQUIRE(t2.kind() == Token::kIdentifier);
  REQUIRE(t2.identifier() == "m");
  REQUIRE(t3.kind() == Token::kPointer);
  REQUIRE(t4.kind() == Token::kIdentifier);
  REQUIRE(t4.identifier() == "x");
  REQUIRE(t5.kind() == Token::kUnaryOp);
  REQUIRE(t6.kind() == Token::kIdentifier);
  REQUIRE(t6.identifier() == "b");
  REQUIRE(t7.kind() == Token::kSemicolon);
  REQUIRE(t8.kind() == Token::kEnd);
}

TEST_CASE("Scanner::Consume() on \"y += x /* add something */\\n\\n\"") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &abort_handler);
  StringByteStream seq("y += x /* add something */\n\n");
  Scanner scanner(&seq, &er);
  Token t0 = scanner.Consume();
  Token t1 = scanner.Consume();
  Token t2 = scanner.Consume();
  Token t3 = scanner.Consume();
  Token t4 = scanner.Consume();
  REQUIRE(t0.kind() == Token::kIdentifier);
  REQUIRE(t0.identifier() == "y");
  REQUIRE(t1.kind() == Token::kAssignOp);
  REQUIRE(t2.kind() == Token::kIdentifier);
  REQUIRE(t2.identifier() == "x");
  REQUIRE(t3.kind() == Token::kSemicolon);
  REQUIRE(t4.kind() == Token::kEnd);
}

CONSUME_ERROR("@", Token::kEnd, StrayRune(location, "@"));
CONSUME_ERROR("/*", Token::kEnd, UnterminatedGeneralComment(location));
CONSUME_ERROR(".12345E+", Token::kLiteral, ExpectedDecimalDigit(location));
CONSUME_ERROR("123xyz", Token::kLiteral, ExtraCharactersInToken(location));
// CONSUME_ERROR("'\\777'", Token::Rune);

TEST_CASE("Scanner::Consume() on \"1E99999999999999999999999999999999\"") {
  std::stringstream ss;
  ErrorReporter er(ss, 0, &abort_handler);
  StringByteStream seq("1E99999999999999999999999999999999");
  Scanner scanner(&seq, &er);
  Token t0 = scanner.Consume();
  REQUIRE(er.At(0) == std::string(FloatingPointOverflow(location)));
  REQUIRE(t0.kind() == Token::kLiteral);
  REQUIRE(t0.value().kind() == value::Value::kFloat);
}

}  // namespace test
}  // namespace rcgo
