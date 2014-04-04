#ifndef RCGO_SRC_SCANNER_HPP
#define RCGO_SRC_SCANNER_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

#include "Token.hpp"

namespace rcgo {

  struct Scanner {
    Scanner (ByteStream& a_byte_stream,
             ErrorReporter& a_error_reporter);
    virtual ~Scanner ();
    Token peek (size_t a_offset = 0);
    Token consume ();

  private:
    typedef std::map<std::string, Token> MapType;

    static void populate (MapType& a_map, const Token& a_kind);
    void push (const Token& a_token);
    bool should_insert_semicolon () const;
    void end_of_word ();

    void start ();
    void line_comment ();
    void general_comment ();
    void identifier ();
    void decimal_literal ();
    void octal_literal ();
    void hex_literal ();
    void finish_decimal (mpz_class& a_value, bool a_require_decimals);
    void finish_exponent (const mpz_class& a_value);
    mpf_class form_float (const mpz_class& a_mantissa, const mpz_class& a_digits_after, bool a_exponent_sign, mpz_class& a_exponent);
    void optional_decimals (mpz_class& a_v, mpz_class& a_digits);
    void decimals (mpz_class& a_v, mpz_class& a_digits);
    void optional_exponent (bool& a_exponent_negative, mpz_class& a_exponent);
    void exponent (bool& a_exponent_negative, mpz_class& a_exponent);
    void rune_literal ();
    Runet escaped_rune (bool& a_error, bool& a_is_byte, bool a_escape_single_quote, bool a_escape_double_quote);
    void raw_string_literal ();
    void interpreted_string_literal ();

    Utf8Scanner m_utf8_scanner;
    ErrorReporter& m_error_reporter;
    std::deque<Token> m_tokens;
    MapType m_keywords;
    MapType m_operators_delimiters;
    Token m_last_token;
  };

}

#endif // RCGO_SRC_SCANNER_HPP
