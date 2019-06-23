#ifndef SRC_SCANNER_H_
#define SRC_SCANNER_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <deque>
#include <map>
#include <string>

#include "src/token.h"
#include "src/utf8.h"

namespace rcgo {

struct Scanner {
  Scanner(ByteStreamI* a_byte_stream, ErrorList* a_error_list);
  virtual ~Scanner();
  Token Peek(size_t a_offset = 0);
  Token Consume();

 private:
  typedef std::map<std::string, Token> MapType;

  static void Populate(MapType* a_map, const Token& a_kind);
  void Push(const Token& a_token);
  bool ShouldInsertSemicolon() const;
  void EndOfWord();

  void Start();
  void LineComment();
  void GeneralComment();
  void Identifier();
  void DecimalLiteral();
  void OctalLiteral();
  void HexLiteral();
  void FinishDecimal(mpz_class* a_value, bool a_require_decimals);
  void FinishExponent(const mpz_class& a_value);
  mpf_class FormFloat(const mpz_class& a_mantissa,
                      const mpz_class& a_digits_after, bool a_exponent_sign,
                      mpz_class* a_exponent);
  void OptionalDecimals(mpz_class* a_v, mpz_class* a_digits);
  void Decimals(mpz_class* a_v, mpz_class* a_digits);
  void OptionalExponent(bool* a_exponent_negative, mpz_class* a_exponent);
  void Exponent(bool* a_exponent_negative, mpz_class* a_exponent);
  void RuneLiteral();
  Runet EscapedRune(bool* a_error, bool* a_is_byte, bool a_escape_single_quote,
                    bool a_escape_double_quote);
  void RawStringLiteral();
  void InterpretedStringLiteral();

  Utf8Scanner m_utf8_scanner;
  ErrorList* m_error_list;
  std::deque<Token> m_tokens;
  MapType m_keywords;
  MapType m_operators_delimiters;
  Token m_last_token;
};

Error StrayRune(const Location& a_location, const std::string& a_rune);
Error UnterminatedGeneralComment(const Location& a_location);
Error FloatingPointOverflow(const Location& a_location);
Error ExpectedDecimalDigit(const Location& a_location);
Error IncompleteRuneLiteral(const Location& a_location);
Error EmptyRuneLiteral(const Location& a_location);
Error ExtraCharactersInRuneLiteral(const Location& a_location);
Error OctalValueOutOfRange(const Location& a_location);
Error TooFewOctalDigitsInRuneLiteral(const Location& a_location);
Error TooFewHexadecimalDigitsInRuneLiteral(const Location& a_location);
Error InvalidUnicodeCodePoint(const Location& a_location);
Error IllegalEscapeSequence(const Location& a_location);
Error IncompleteStringLiteral(const Location& a_location);
Error ExtraCharactersInToken(const Location& a_location);

}  // namespace rcgo

#endif  // SRC_SCANNER_H_
