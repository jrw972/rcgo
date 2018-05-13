// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/utf8.h"

#include "src/catch.hpp"
#include "src/test_helpers.h"

namespace rcgo {
namespace test {

#define PEEK_TEST(str, first) \
  TEST_CASE("Utf8Scanner::peek() on \"" str "\"") {                     \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 1, &test::abort_handler);                      \
    StringByteStream seq(str);                                          \
    Utf8Scanner scanner(&seq, &er);                                     \
    Runet r = scanner.Peek();                                           \
    REQUIRE(r == first);                                                \
  }

#define PEEK_TEST_ERROR(str, first) \
  TEST_CASE("Utf8Scanner::peek() on \"" str "\"") {                     \
    std::stringstream ss;                                               \
    ErrorReporter er(ss, 0, &test::abort_handler);                      \
    StringByteStream seq(str);                                          \
    Utf8Scanner scanner(&seq, &er);                                     \
    Runet r = scanner.Peek();                                           \
    REQUIRE(r == first);                                                \
    REQUIRE(er.At(0) == std::string(IllegalUtf8Sequence(test::location))); \
  }

PEEK_TEST("\u0024", 0x24);
PEEK_TEST("\u00A2", 0xA2);
PEEK_TEST("\u20AC", 0x20AC);
PEEK_TEST("\U00010348", 0x10348);
PEEK_TEST("", kEndRune);
// Invalid start byte.
PEEK_TEST_ERROR("\xFFvalid", 'v');
// Just a start byte.
PEEK_TEST_ERROR("\xC0", kEndRune);
// Overlong sequence.
PEEK_TEST_ERROR("\xF0\x82\x82\xAC\u00A2", 0xA2);
// Leading byte and invalid continuation byte.
PEEK_TEST_ERROR("\xC0\xFF\u20AC", 0x20AC);
// Leading byte and multiple invalid continuation bytes.
PEEK_TEST_ERROR("\xC0\xFF\xFF\U00010348", 0x10348);
// String with invalid rune.
PEEK_TEST_ERROR("\xF7\x90\x8D\x88", kEndRune);
// String with surrogate half.
PEEK_TEST_ERROR("\xED\xA0\x80", kEndRune);

TEST_CASE("Utf8::consume() on \"" "\u0024\u00A2\u20AC\U00010348" "\"") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &test::abort_handler);
  StringByteStream seq("\u0024\u00A2\u20AC\U00010348");
  Utf8Scanner scanner(&seq, &er);
  Runet r0 = scanner.Consume();
  Runet r1 = scanner.Consume();
  Runet r2 = scanner.Consume();
  Runet r3 = scanner.Consume();
  Runet r4 = scanner.Consume();
  REQUIRE(r0 == 0x24);
  REQUIRE(r1 == 0xA2);
  REQUIRE(r2 == 0x20AC);
  REQUIRE(r3 == 0x10348);
  REQUIRE(r4 == kEndRune);
}

TEST_CASE("Unicode Predicates") {
  REQUIRE(IsUnicodeChar(0) == true);
  REQUIRE(IsUnicodeChar(0xD800) == false);
  REQUIRE(IsUnicodeLetter('h') == true);
  REQUIRE(IsUnicodeLetter(0) == false);
  REQUIRE(IsUnicodeDigit('4') == true);
  REQUIRE(IsUnicodeDigit('h') == false);
  REQUIRE(IsUnicodeUpper('H') == true);
  REQUIRE(IsUnicodeUpper('h') == false);

  REQUIRE(IsLetter('h') == true);
  REQUIRE(IsLetter(0) == false);
  REQUIRE(IsLetter('_') == true);

  for (char c = '0'; c <= '9'; ++c) {
    REQUIRE(IsDecimalDigit(c) == true);
  }
  REQUIRE(IsDecimalDigit('x') == false);

  for (char c = '0'; c <= '7'; ++c) {
    REQUIRE(IsOctalDigit(c) == true);
  }
  REQUIRE(IsOctalDigit('x') == false);

  for (char c = '0'; c <= '9'; ++c) {
    REQUIRE(IsHexDigit(c) == true);
  }
  for (char c = 'A'; c <= 'F'; ++c) {
    REQUIRE(IsHexDigit(c) == true);
  }
  for (char c = 'a'; c <= 'f'; ++c) {
    REQUIRE(IsHexDigit(c) == true);
  }
  REQUIRE(IsHexDigit('x') == false);
}

#define PROTECT(...) __VA_ARGS__

#define APPEND_TEST(rune, sz, ex) TEST_CASE("append(\"\", " #rune ")") { \
    std::string s;                                                      \
    int expected[] = ex;                                                \
    Append(&s, rune);                                                   \
    REQUIRE(s.size() == sz);                                            \
    for (size_t idx = 0; idx != sz; ++idx) {                            \
      REQUIRE(s[idx] == static_cast<char>(expected[idx]));              \
    }                                                                   \
  }

APPEND_TEST(0x0024, 1, PROTECT({ 0x24 }));
APPEND_TEST(0x00A2, 2, PROTECT({ 0xC2, 0xA2 }));
APPEND_TEST(0x20AC, 3, PROTECT({ 0xE2, 0x82, 0xAC }));
APPEND_TEST(0x10348, 4, PROTECT({ 0xF0, 0x90, 0x8D, 0x88 }));
APPEND_TEST(0x110000, 0, PROTECT({ }));
APPEND_TEST(0xD800, 0, PROTECT({ }));
APPEND_TEST(0xDFFF, 0, PROTECT({ }));

TEST_CASE("Utf8::consume() on \"\\n\"") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &test::abort_handler);
  StringByteStream seq("\n");
  Utf8Scanner scanner(&seq, &er);
  Runet r0 = scanner.Consume();
  REQUIRE(r0 == '\n');
  REQUIRE(scanner.GetLocation().line == 2);
}

TEST_CASE("Utf8::consume(1) on \"ab\"") {
  std::stringstream ss;
  ErrorReporter er(ss, 1, &test::abort_handler);
  StringByteStream seq("ab");
  Utf8Scanner scanner(&seq, &er);
  scanner.Consume(1);
  Runet r0 = scanner.Consume();
  REQUIRE(r0 == 'b');
}

TEST_CASE("first_rune(\"a\")") {
  REQUIRE(FirstRune("a") == 'a');
}

}  // namespace test
}  // namespace rcgo
