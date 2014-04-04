/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "Utf8.hpp"
#include "spies.hpp"
#include "ByteStream.hpp"
#include "Error.hpp"

using namespace rcgo;

#define PEEK_TEST(str, first) TEST_CASE ("Utf8Scanner::peek () on \"" str "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 1, abortHandler);                             \
    StringByteStream seq (str);                                         \
    Utf8Scanner scanner (seq, er);                                      \
    Runet r = scanner.peek ();                                          \
    REQUIRE (r == first);                                               \
  }

#define PEEK_TEST_ERROR(str, first) TEST_CASE ("Utf8Scanner::peek () on \"" str "\"") { \
    std::stringstream ss;                                               \
    ErrorReporter er (ss, 0, abortHandler);                             \
    StringByteStream seq (str);                                         \
    Utf8Scanner scanner (seq, er);                                      \
    Runet r = scanner.peek ();                                          \
    REQUIRE (r == first);                                               \
    REQUIRE (er.at (0) == std::string (IllegalUtf8Sequence (location)));     \
  }

PEEK_TEST ("\u0024", 0x24);
PEEK_TEST ("\u00A2", 0xA2);
PEEK_TEST ("\u20AC", 0x20AC);
PEEK_TEST ("\U00010348", 0x10348);
PEEK_TEST ("", END_RUNE);
PEEK_TEST_ERROR ("\xFFvalid", 'v'); // Invalid start byte.
PEEK_TEST_ERROR ("\xC0", END_RUNE); // Just a start byte.
PEEK_TEST_ERROR ("\xF0\x82\x82\xAC\u00A2", 0xA2); // Overlong sequence.
PEEK_TEST_ERROR ("\xC0\xFF\u20AC", 0x20AC); // Leading byte and invalid continuation byte.
PEEK_TEST_ERROR ("\xC0\xFF\xFF\U00010348", 0x10348); // Leading byte and multiple invalid continuation bytes.
PEEK_TEST_ERROR ("\xF7\x90\x8D\x88", END_RUNE); // String with invalid rune.
PEEK_TEST_ERROR ("\xED\xA0\x80", END_RUNE); // String with surrogate half.

TEST_CASE ("Utf8::consume () on \"" "\u0024\u00A2\u20AC\U00010348" "\"") {
  std::stringstream ss;
  ErrorReporter er (ss, 1, abortHandler);
  StringByteStream seq ("\u0024\u00A2\u20AC\U00010348");
  Utf8Scanner scanner (seq, er);
  Runet r0 = scanner.consume ();
  Runet r1 = scanner.consume ();
  Runet r2 = scanner.consume ();
  Runet r3 = scanner.consume ();
  Runet r4 = scanner.consume ();
  REQUIRE (r0 == 0x24);
  REQUIRE (r1 == 0xA2);
  REQUIRE (r2 == 0x20AC);
  REQUIRE (r3 == 0x10348);
  REQUIRE (r4 == END_RUNE);
}

TEST_CASE ("Unicode Predicates") {
  REQUIRE (is_unicode_char (0) == true);
  REQUIRE (is_unicode_char (0xD800) == false);
  REQUIRE (is_unicode_letter ('h') == true);
  REQUIRE (is_unicode_letter (0) == false);
  REQUIRE (is_unicode_digit ('4') == true);
  REQUIRE (is_unicode_digit ('h') == false);
  REQUIRE (is_unicode_upper ('H') == true);
  REQUIRE (is_unicode_upper ('h') == false);

  REQUIRE (is_letter ('h') == true);
  REQUIRE (is_letter (0) == false);
  REQUIRE (is_letter ('_') == true);

  for (char c = '0'; c <= '9'; ++c) {
    REQUIRE (is_decimal_digit (c) == true);
  }
  REQUIRE (is_decimal_digit ('x') == false);

  for (char c = '0'; c <= '7'; ++c) {
    REQUIRE (is_octal_digit (c) == true);
  }
  REQUIRE (is_octal_digit ('x') == false);

  for (char c = '0'; c <= '9'; ++c) {
    REQUIRE (is_hex_digit (c) == true);
  }
  for (char c = 'A'; c <= 'F'; ++c) {
    REQUIRE (is_hex_digit (c) == true);
  }
  for (char c = 'a'; c <= 'f'; ++c) {
    REQUIRE (is_hex_digit (c) == true);
  }
  REQUIRE (is_hex_digit ('x') == false);
}

#define PROTECT(...) __VA_ARGS__ 

#define APPEND_TEST(rune, sz, ex) TEST_CASE ("append (\"\", " #rune ")") { \
    std::string s;                                                      \
    int expected[] = ex;                                                \
    append (s, rune);                                                   \
    REQUIRE (s.size () == sz);                                          \
    for (size_t idx = 0; idx != sz; ++idx)                              \
      {                                                                 \
        REQUIRE (s[idx] == static_cast<char> (expected[idx]));          \
      }                                                                 \
  }

APPEND_TEST(0x0024, 1, PROTECT({ 0x24 }));
APPEND_TEST(0x00A2, 2, PROTECT({ 0xC2, 0xA2 }));
APPEND_TEST(0x20AC, 3, PROTECT({ 0xE2, 0x82, 0xAC }));
APPEND_TEST(0x10348, 4, PROTECT({ 0xF0, 0x90, 0x8D, 0x88 }));
APPEND_TEST(0x110000, 0, PROTECT({ }));
APPEND_TEST(0xD800, 0, PROTECT({ }));
APPEND_TEST(0xDFFF, 0, PROTECT({ }));

TEST_CASE ("Utf8::consume () on \"\\n\"") {
  std::stringstream ss;
  ErrorReporter er (ss, 1, abortHandler);
  StringByteStream seq ("\n");
  Utf8Scanner scanner (seq, er);
  Runet r0 = scanner.consume ();
  REQUIRE (r0 == '\n');
  REQUIRE (scanner.location ().line == 2);
}

TEST_CASE ("Utf8::consume (1) on \"ab\"") {
  std::stringstream ss;
  ErrorReporter er (ss, 1, abortHandler);
  StringByteStream seq ("ab");
  Utf8Scanner scanner (seq, er);
  scanner.consume (1);
  Runet r0 = scanner.consume ();
  REQUIRE (r0 == 'b');
}

TEST_CASE ("first_rune(\"a\")") {
  REQUIRE (first_rune ("a") == 'a');
}
