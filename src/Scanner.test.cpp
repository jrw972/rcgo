/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "Scanner.hpp"
#include "spies.hpp"
#include "ByteStream.hpp"
#include "Error.hpp"

using namespace rcgo;

#define EMPTY_TEST(str) TEST_CASE ("Scanner::Peek () on \"" str "\"") {      \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (str);                                         \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (t.kind () == Token::End);                              \
  }

#define IDENTIFIER_TEST(id) TEST_CASE ("Scanner::Peek () on \"" id "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (id);                                          \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (t.kind () == Token::Identifier);                       \
    REQUIRE (t.identifier () == id);                                    \
  }

#define KEYWORD_TEST(keyword, token) TEST_CASE ("Scanner::Peek () on \"" keyword "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (keyword);                                     \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (t.kind () == token);                                       \
  }

#define OPERATOR_DELIMITER_TEST(od, token) TEST_CASE ("Scanner::Peek () on \"" od "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (od);                                          \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (t.kind () == token);                                       \
  }

#define OPERATOR_DELIMITER_TEST2(od, token, getter, x) TEST_CASE ("Scanner::Peek () on \"" od "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (od);                                          \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (t.kind () == token);                                       \
    REQUIRE (t.getter () == x);                                         \
  }

#define INTEGER_LITERAL_TEST(literal, val) TEST_CASE ("Scanner::Peek () on \"" literal "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (literal);                                     \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (t.kind () == Token::Literal);                              \
    REQUIRE (t.value ().kind == Value::Integer);                        \
    REQUIRE (t.value ().Integer_value == val);                        \
  }

#define FLOATING_LITERAL_TEST(literal, val) TEST_CASE ("Scanner::Peek () on \"" literal "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (literal);                                     \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (t.kind () == Token::Literal);                              \
    REQUIRE (t.value ().kind == Value::Float);                          \
    REQUIRE (fabs (t.value ().Float_value.get_d () - val) < .0000001);  \
  }

#define IMAGINARY_LITERAL_TEST(literal, val) TEST_CASE ("Scanner::Peek () on \"" literal "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (literal);                                     \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (t.kind () == Token::Literal);                              \
    REQUIRE (t.value ().kind == Value::Complex);                        \
    REQUIRE (fabs (t.value ().Complex_value.imag.get_d () - val) < .0000001); \
  }

#define RUNE_LITERAL_TEST(literal, val) TEST_CASE ("Scanner::Peek () on \"" literal "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (literal);                                     \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (t.kind () == Token::Literal);                              \
    REQUIRE (t.value ().kind == Value::Rune);                           \
    REQUIRE (t.value ().Rune_value == val);                             \
  }

#define RUNE_LITERAL_TEST_ERROR(literal, err) TEST_CASE ("Scanner::Peek () on \"" literal "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 0, abortHandler);                             \
    StringByteStream seq (literal);                                     \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (er.at (0) == std::string (err));                           \
    REQUIRE (t.kind () == Token::Literal);                              \
    REQUIRE (t.value ().kind == Value::Rune);                           \
  }

#define STRING_LITERAL_TEST(literal, val) TEST_CASE ("Scanner::Peek () on \"" literal "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (literal);                                     \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (t.kind () == Token::Literal);                              \
    REQUIRE (t.value ().kind == Value::String);                         \
    REQUIRE (t.value ().String_value == val);                           \
  }

#define STRING_LITERAL_TEST_ERROR(literal, err) TEST_CASE ("Scanner::Peek () on \"" literal "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 0, abortHandler);                             \
    StringByteStream seq (literal);                                     \
    Scanner scanner (seq, er);                                          \
    Token t = scanner.peek ();                                          \
    REQUIRE (er.at (0) == std::string (err));                           \
    REQUIRE (t.kind () == Token::Literal);                              \
    REQUIRE (t.value ().kind == Value::String);                         \
  }

#define CONSUME_ERROR(str, k, err) TEST_CASE ("Scanner::Consume () on \"" str "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 0, abortHandler);                             \
    StringByteStream seq (str);                                         \
    Scanner scanner (seq, er);                                          \
    Token t0 = scanner.consume ();                                      \
    REQUIRE (er.at (0) == std::string (err));                           \
    REQUIRE (t0.kind () == k);                                  \
  }

EMPTY_TEST ("");
EMPTY_TEST ("// This is a comment");
EMPTY_TEST ("/* This is a comment */");

IDENTIFIER_TEST ("a");
IDENTIFIER_TEST ("_x9");
IDENTIFIER_TEST ("ThisVariableIsExported");
IDENTIFIER_TEST ("αβ");

KEYWORD_TEST ("break", Token::Break);
KEYWORD_TEST ("case", Token::Case);
KEYWORD_TEST ("const", Token::Const);
KEYWORD_TEST ("continue", Token::Continue);
KEYWORD_TEST ("default", Token::Default);
KEYWORD_TEST ("defer", Token::Defer);
KEYWORD_TEST ("else", Token::Else);
KEYWORD_TEST ("fallthrough", Token::Fallthrough);
KEYWORD_TEST ("for", Token::For);
KEYWORD_TEST ("func", Token::Func);
KEYWORD_TEST ("goto", Token::Goto);
KEYWORD_TEST ("if", Token::If);
KEYWORD_TEST ("import", Token::Import);
KEYWORD_TEST ("interface", Token::Interface);
KEYWORD_TEST ("map", Token::Map);
KEYWORD_TEST ("package", Token::Package);
KEYWORD_TEST ("range", Token::Range);
KEYWORD_TEST ("return", Token::Return);
KEYWORD_TEST ("struct", Token::Struct);
KEYWORD_TEST ("switch", Token::Switch);
KEYWORD_TEST ("type", Token::Type);
KEYWORD_TEST ("var", Token::Var);

OPERATOR_DELIMITER_TEST2 ("+", Token::UnaryOp, unary_op_kind, Token::Posate);
OPERATOR_DELIMITER_TEST2 ("-", Token::UnaryOp, unary_op_kind, Token::Negate);
OPERATOR_DELIMITER_TEST ("*", Token::Pointer);
OPERATOR_DELIMITER_TEST2 ("/", Token::MulOp, mul_op_kind, Token::Divide);
OPERATOR_DELIMITER_TEST2 ("%", Token::MulOp, mul_op_kind, Token::Modulus);

OPERATOR_DELIMITER_TEST2 ("&", Token::UnaryOp, unary_op_kind, Token::Address);
OPERATOR_DELIMITER_TEST2 ("|", Token::AddOp, add_op_kind, Token::BitOr);
OPERATOR_DELIMITER_TEST2 ("^", Token::UnaryOp, unary_op_kind, Token::BitNot);
OPERATOR_DELIMITER_TEST2 ("<<", Token::MulOp, mul_op_kind, Token::LeftShift);
OPERATOR_DELIMITER_TEST2 (">>", Token::MulOp, mul_op_kind, Token::RightShift);
OPERATOR_DELIMITER_TEST2 ("&^", Token::MulOp, mul_op_kind, Token::BitAndNot);

OPERATOR_DELIMITER_TEST2 ("+=", Token::AssignOp, assign_op_kind, Token::AddAssign);
OPERATOR_DELIMITER_TEST2 ("-=", Token::AssignOp, assign_op_kind, Token::SubtractAssign);
OPERATOR_DELIMITER_TEST2 ("*=", Token::AssignOp, assign_op_kind, Token::MultiplyAssign);
OPERATOR_DELIMITER_TEST2 ("/=", Token::AssignOp, assign_op_kind, Token::DivideAssign);
OPERATOR_DELIMITER_TEST2 ("%=", Token::AssignOp, assign_op_kind, Token::ModulusAssign);

OPERATOR_DELIMITER_TEST2 ("&=", Token::AssignOp, assign_op_kind, Token::BitAndAssign);
OPERATOR_DELIMITER_TEST2 ("|=", Token::AssignOp, assign_op_kind, Token::BitOrAssign);
OPERATOR_DELIMITER_TEST2 ("^=", Token::AssignOp, assign_op_kind, Token::BitXorAssign);
OPERATOR_DELIMITER_TEST2 ("<<=", Token::AssignOp, assign_op_kind, Token::LeftShiftAssign);
OPERATOR_DELIMITER_TEST2 (">>=", Token::AssignOp, assign_op_kind, Token::RightShiftAssign);
OPERATOR_DELIMITER_TEST2 ("&^=", Token::AssignOp, assign_op_kind, Token::BitAndNotAssign);

OPERATOR_DELIMITER_TEST ("&&", Token::LogicalAnd);
OPERATOR_DELIMITER_TEST ("||", Token::LogicalOr);
OPERATOR_DELIMITER_TEST ("++", Token::Increment);
OPERATOR_DELIMITER_TEST ("--", Token::Decrement);

OPERATOR_DELIMITER_TEST2 ("==", Token::RelOp, rel_op_kind, Token::Equal);
OPERATOR_DELIMITER_TEST2 ("<", Token::RelOp, rel_op_kind, Token::LessThan);
OPERATOR_DELIMITER_TEST2 (">", Token::RelOp, rel_op_kind, Token::GreaterThan);
OPERATOR_DELIMITER_TEST ("=", Token::Assign);
OPERATOR_DELIMITER_TEST2 ("!", Token::UnaryOp, unary_op_kind, Token::LogicalNot);

OPERATOR_DELIMITER_TEST2 ("!=", Token::RelOp, rel_op_kind, Token::NotEqual);
OPERATOR_DELIMITER_TEST2 ("<=", Token::RelOp, rel_op_kind, Token::LessThanEqual);
OPERATOR_DELIMITER_TEST2 (">=", Token::RelOp, rel_op_kind, Token::GreaterThanEqual);
OPERATOR_DELIMITER_TEST (":=", Token::ShortAssign);
OPERATOR_DELIMITER_TEST ("...", Token::Ellipsis);

OPERATOR_DELIMITER_TEST ("(", Token::LeftParen);
OPERATOR_DELIMITER_TEST ("[", Token::LeftBracket);
OPERATOR_DELIMITER_TEST ("{", Token::LeftBrace);
OPERATOR_DELIMITER_TEST (",", Token::Comma);
OPERATOR_DELIMITER_TEST (".", Token::Period);

OPERATOR_DELIMITER_TEST (")", Token::RightParen);
OPERATOR_DELIMITER_TEST ("]", Token::RightBracket);
OPERATOR_DELIMITER_TEST ("}", Token::RightBrace);
OPERATOR_DELIMITER_TEST (";", Token::Semicolon);
OPERATOR_DELIMITER_TEST (":", Token::Colon);

INTEGER_LITERAL_TEST ("42", 42);
INTEGER_LITERAL_TEST ("0600", 0600);
INTEGER_LITERAL_TEST ("0xBadFace", 0xBadFace);
INTEGER_LITERAL_TEST ("0x1234", 0x1234);
INTEGER_LITERAL_TEST ("170141183460469231731687303715884105727", mpz_class ("170141183460469231731687303715884105727"));

FLOATING_LITERAL_TEST ("0.", 0);
FLOATING_LITERAL_TEST ("72.40", 72.40);
FLOATING_LITERAL_TEST ("072.40", 72.40);
FLOATING_LITERAL_TEST ("2.71828", 2.71828);
FLOATING_LITERAL_TEST ("1.e+0", 1.0);
FLOATING_LITERAL_TEST ("6.67428e-11", 6.67428e-11);
FLOATING_LITERAL_TEST ("1E6", 1E6);
FLOATING_LITERAL_TEST ("0123E-6", 123E-6);
FLOATING_LITERAL_TEST (".25", .25);
FLOATING_LITERAL_TEST (".12345E+5", .12345E+5);

IMAGINARY_LITERAL_TEST ("0i", 0);
IMAGINARY_LITERAL_TEST ("011i", 11);
IMAGINARY_LITERAL_TEST ("0.i", 0);
IMAGINARY_LITERAL_TEST ("2.71828i", 2.71828);
IMAGINARY_LITERAL_TEST ("1.e+0i", 1.0);
IMAGINARY_LITERAL_TEST ("6.67428e-11i", 6.67428e-11);
IMAGINARY_LITERAL_TEST ("1E6i", 1E6);
IMAGINARY_LITERAL_TEST (".25i", .25);
IMAGINARY_LITERAL_TEST (".12345E+5i", .12345E+5);

RUNE_LITERAL_TEST ("'a'", 'a');
RUNE_LITERAL_TEST ("'ä'", 228);
RUNE_LITERAL_TEST ("'本'", 26412);
RUNE_LITERAL_TEST ("'\\a'", '\a');
RUNE_LITERAL_TEST ("'\\b'", '\b');
RUNE_LITERAL_TEST ("'\\f'", '\f');
RUNE_LITERAL_TEST ("'\\n'", '\n');
RUNE_LITERAL_TEST ("'\\r'", '\r');
RUNE_LITERAL_TEST ("'\\t'", '\t');
RUNE_LITERAL_TEST ("'\\v'", '\v');
RUNE_LITERAL_TEST ("'\\\\'", '\\');
RUNE_LITERAL_TEST ("'\\000'", 0);
RUNE_LITERAL_TEST ("'\\007'", 007);
RUNE_LITERAL_TEST ("'\\377'", 0377);
RUNE_LITERAL_TEST ("'\\x07'", 0x07);
RUNE_LITERAL_TEST ("'\\xff'", 0xFF);
RUNE_LITERAL_TEST ("'\\u12e4'", 0x12E4);
RUNE_LITERAL_TEST ("'\\U00101234'", 0x00101234);
RUNE_LITERAL_TEST ("'\\''", '\'');
RUNE_LITERAL_TEST_ERROR ("'aa'", ExtraCharactersInRuneLiteral (location));
RUNE_LITERAL_TEST_ERROR ("'\\xa'", TooFewHexadecimalDigitsInRuneLiteral (location));
RUNE_LITERAL_TEST_ERROR ("'\\ua'", TooFewHexadecimalDigitsInRuneLiteral (location));
RUNE_LITERAL_TEST_ERROR ("'\\Ua'", TooFewHexadecimalDigitsInRuneLiteral (location));
RUNE_LITERAL_TEST_ERROR ("'\\0'", TooFewOctalDigitsInRuneLiteral (location));
RUNE_LITERAL_TEST_ERROR ("'\\777'", OctalValueOutOfRange (location));
RUNE_LITERAL_TEST_ERROR ("'\\uDFFF'", InvalidUnicodeCodePoint (location));
RUNE_LITERAL_TEST_ERROR ("'\\U00110000'", InvalidUnicodeCodePoint (location));
RUNE_LITERAL_TEST_ERROR ("'", IncompleteRuneLiteral (location));
RUNE_LITERAL_TEST_ERROR ("''", EmptyRuneLiteral (location));
RUNE_LITERAL_TEST_ERROR ("'a\n", IncompleteRuneLiteral (location));
RUNE_LITERAL_TEST_ERROR ("'\\c'", IllegalEscapeSequence (location));
RUNE_LITERAL_TEST_ERROR ("'\\\"'", IllegalEscapeSequence (location));

STRING_LITERAL_TEST ("`abc`", "abc");
STRING_LITERAL_TEST ("`\\n\n\\n`", "\\n\n\\n");
STRING_LITERAL_TEST ("\"\\n\"", "\n");
STRING_LITERAL_TEST ("\"\\\"\"", "\"");
STRING_LITERAL_TEST ("\"Hello, world!\\n\"", "Hello, world!\n");
STRING_LITERAL_TEST ("\"日本語\"", "日本語");
STRING_LITERAL_TEST ("`日本語`", "日本語");
STRING_LITERAL_TEST ("\"\\u65e5\\u672c\\u8a9e\"", "日本語");
STRING_LITERAL_TEST ("\"\\U000065e5\\U0000672c\\U00008a9e\"", "日本語");
STRING_LITERAL_TEST ("\"\\xe6\\x97\\xa5\\xe6\\x9c\\xac\\xe8\\xaa\\x9e\"", "日本語");
STRING_LITERAL_TEST ("\"\u65e5本\U00008a9ea\"", "日本語a");
STRING_LITERAL_TEST ("\"\\xff\\u00FF\"", "\xff\xc3\xbf");
STRING_LITERAL_TEST ("`\r`", "");
STRING_LITERAL_TEST_ERROR ("\"\\uD800\"", InvalidUnicodeCodePoint (location));
STRING_LITERAL_TEST_ERROR ("\"\\U00110000\"", InvalidUnicodeCodePoint (location));
STRING_LITERAL_TEST_ERROR ("\"\\'\"", IllegalEscapeSequence (location));
STRING_LITERAL_TEST_ERROR ("`", IncompleteStringLiteral (location));
STRING_LITERAL_TEST_ERROR ("\"", IncompleteStringLiteral (location));
STRING_LITERAL_TEST_ERROR ("\"\n\"", IncompleteStringLiteral (location));

TEST_CASE ("Scanner::Consume () on \"y = m * x + b // equation of a line\"") {
  std::stringstream ss;
  ErrorReporter er (ss, 1, abortHandler);
  StringByteStream seq ("y = m * x + b // equation of a line");
  Scanner scanner (seq, er);
  Token t0 = scanner.consume ();
  Token t1 = scanner.consume ();
  Token t2 = scanner.consume ();
  Token t3 = scanner.consume ();
  Token t4 = scanner.consume ();
  Token t5 = scanner.consume ();
  Token t6 = scanner.consume ();
  Token t7 = scanner.consume ();
  Token t8 = scanner.consume ();
  REQUIRE (t0.kind () == Token::Identifier);
  REQUIRE (t0.identifier () == "y");
  REQUIRE (t1.kind () == Token::Assign);
  REQUIRE (t2.kind () == Token::Identifier);
  REQUIRE (t2.identifier () == "m");
  REQUIRE (t3.kind () == Token::Pointer);
  REQUIRE (t4.kind () == Token::Identifier);
  REQUIRE (t4.identifier () == "x");
  REQUIRE (t5.kind () == Token::UnaryOp);
  REQUIRE (t6.kind () == Token::Identifier);
  REQUIRE (t6.identifier () == "b");
  REQUIRE (t7.kind () == Token::Semicolon);
  REQUIRE (t8.kind () == Token::End);
}

TEST_CASE ("Scanner::Consume () on \"y += x /* add something */\\n\\n\"") {
  std::stringstream ss;
  ErrorReporter er (ss, 1, abortHandler);
  StringByteStream seq ("y += x /* add something */\n\n");
  Scanner scanner (seq, er);
  Token t0 = scanner.consume ();
  Token t1 = scanner.consume ();
  Token t2 = scanner.consume ();
  Token t3 = scanner.consume ();
  Token t4 = scanner.consume ();
  REQUIRE (t0.kind () == Token::Identifier);
  REQUIRE (t0.identifier () == "y");
  REQUIRE (t1.kind () == Token::AssignOp);
  REQUIRE (t2.kind () == Token::Identifier);
  REQUIRE (t2.identifier () == "x");
  REQUIRE (t3.kind () == Token::Semicolon);
  REQUIRE (t4.kind () == Token::End);
}

CONSUME_ERROR ("@", Token::End, StrayRune (location, "@"));
CONSUME_ERROR ("/*", Token::End, UnterminatedGeneralComment (location));
CONSUME_ERROR (".12345E+", Token::Literal, ExpectedDecimalDigit (location));
CONSUME_ERROR ("123xyz", Token::Literal, ExtraCharactersInToken (location));
//CONSUME_ERROR ("'\\777'", Token::Rune);

TEST_CASE ("Scanner::Consume () on \"1E99999999999999999999999999999999\"") {
  std::stringstream ss;
  ErrorReporter er (ss, 0, abortHandler);
  StringByteStream seq ("1E99999999999999999999999999999999");
  Scanner scanner (seq, er);
  Token t0 = scanner.consume ();
  REQUIRE (er.at (0) == std::string (FloatingPointOverflow (location)));
  REQUIRE (t0.kind () == Token::Literal);
  REQUIRE (t0.value ().kind == Value::Float);
}
