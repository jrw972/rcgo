/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Scanner.hpp"

#include <sstream>

#include "Error.hpp"
#include "functions.hpp"

#define SPACE 0x20
#define TAB 0x09
#define CARRIAGE_RETURN 0x0D
#define NEWLINE 0x0A

namespace rcgo {

  static Runet octalValue (Runet s)
  {
    return s - '0';
  }

  static Runet decimalValue (Runet s)
  {
    return s - '0';
  }

  static Runet hexValue (Runet s)
  {
    if (s >= '0' && s <= '9')
      {
        return s - '0';
      }
    else if (s >= 'a' && s <= 'f')
      {
        return s - 'a' + 10;
      }
    else
      {
        return s - 'A' + 10;
      }
  }

  void Scanner::populate (MapType& map, const Token& token)
  {
    std::stringstream ss;
    ss << token;
    map[ss.str ()] = token;
  }

  Scanner::Scanner (ByteStream& byteStream,
                    ErrorReporter& aErrorReporter)
    : m_utf8_scanner (byteStream, aErrorReporter)
    , m_error_reporter (aErrorReporter)
    , m_last_token (Token::make (m_utf8_scanner.location (), Token::End))
  {
    populate (m_keywords, Token::make (Token::Break));
    populate (m_keywords, Token::make (Token::Case));
    populate (m_keywords, Token::make (Token::Const));
    populate (m_keywords, Token::make (Token::Continue));
    populate (m_keywords, Token::make (Token::Default));
    populate (m_keywords, Token::make (Token::Defer));
    populate (m_keywords, Token::make (Token::Else));
    populate (m_keywords, Token::make (Token::Fallthrough));
    populate (m_keywords, Token::make (Token::For));
    populate (m_keywords, Token::make (Token::Func));
    populate (m_keywords, Token::make (Token::Goto));
    populate (m_keywords, Token::make (Token::If));
    populate (m_keywords, Token::make (Token::Import));
    populate (m_keywords, Token::make (Token::Interface));
    populate (m_keywords, Token::make (Token::Map));
    populate (m_keywords, Token::make (Token::Package));
    populate (m_keywords, Token::make (Token::Range));
    populate (m_keywords, Token::make (Token::Return));
    populate (m_keywords, Token::make (Token::Struct));
    populate (m_keywords, Token::make (Token::Switch));
    populate (m_keywords, Token::make (Token::Type));
    populate (m_keywords, Token::make (Token::Var));

    populate (m_operators_delimiters, Token::make (Token::Posate));
    populate (m_operators_delimiters, Token::make (Token::Negate));
    populate (m_operators_delimiters, Token::make (Token::Pointer));
    populate (m_operators_delimiters, Token::make (Token::Divide));
    populate (m_operators_delimiters, Token::make (Token::Modulus));

    populate (m_operators_delimiters, Token::make (Token::Address));
    populate (m_operators_delimiters, Token::make (Token::BitNot));
    populate (m_operators_delimiters, Token::make (Token::BitOr));

    populate (m_operators_delimiters, Token::make (Token::LeftShift));
    populate (m_operators_delimiters, Token::make (Token::RightShift));
    populate (m_operators_delimiters, Token::make (Token::BitAndNot));

    populate (m_operators_delimiters, Token::make (Token::AddAssign));
    populate (m_operators_delimiters, Token::make (Token::SubtractAssign));
    populate (m_operators_delimiters, Token::make (Token::MultiplyAssign));
    populate (m_operators_delimiters, Token::make (Token::DivideAssign));
    populate (m_operators_delimiters, Token::make (Token::ModulusAssign));

    populate (m_operators_delimiters, Token::make (Token::BitAndAssign));
    populate (m_operators_delimiters, Token::make (Token::BitOrAssign));
    populate (m_operators_delimiters, Token::make (Token::BitXorAssign));
    populate (m_operators_delimiters, Token::make (Token::LeftShiftAssign));
    populate (m_operators_delimiters, Token::make (Token::RightShiftAssign));
    populate (m_operators_delimiters, Token::make (Token::BitAndNotAssign));

    populate (m_operators_delimiters, Token::make (Token::LogicalAnd));
    populate (m_operators_delimiters, Token::make (Token::LogicalOr));
    populate (m_operators_delimiters, Token::make (Token::Increment));
    populate (m_operators_delimiters, Token::make (Token::Decrement));

    populate (m_operators_delimiters, Token::make (Token::Equal));
    populate (m_operators_delimiters, Token::make (Token::LessThan));
    populate (m_operators_delimiters, Token::make (Token::GreaterThan));
    populate (m_operators_delimiters, Token::make (Token::Assign));
    populate (m_operators_delimiters, Token::make (Token::LogicalNot));

    populate (m_operators_delimiters, Token::make (Token::NotEqual));
    populate (m_operators_delimiters, Token::make (Token::LessThanEqual));
    populate (m_operators_delimiters, Token::make (Token::GreaterThanEqual));
    populate (m_operators_delimiters, Token::make (Token::ShortAssign));
    populate (m_operators_delimiters, Token::make (Token::Ellipsis));

    populate (m_operators_delimiters, Token::make (Token::LeftParen));
    populate (m_operators_delimiters, Token::make (Token::LeftBracket));
    populate (m_operators_delimiters, Token::make (Token::LeftBrace));
    populate (m_operators_delimiters, Token::make (Token::Comma));
    populate (m_operators_delimiters, Token::make (Token::Period));

    populate (m_operators_delimiters, Token::make (Token::RightParen));
    populate (m_operators_delimiters, Token::make (Token::RightBracket));
    populate (m_operators_delimiters, Token::make (Token::RightBrace));
    populate (m_operators_delimiters, Token::make (Token::Semicolon));
    populate (m_operators_delimiters, Token::make (Token::Colon));
  }

  Scanner::~Scanner ()
  { }

  // https://golang.org/ref/spec#Tokens
  void Scanner::start ()
  {
    Runet r0 = m_utf8_scanner.peek (0);
    Runet r1 = m_utf8_scanner.peek (1);
    Runet r2 = m_utf8_scanner.peek (2);

    if (r0 == END_RUNE)
      {
        if (should_insert_semicolon ())
          {
            m_last_token = Token::make (m_utf8_scanner.location (), Token::End);
            push (Token::make (m_utf8_scanner.location (), Token::Semicolon));
            return;
          }
        push (Token::make (m_utf8_scanner.location (), Token::End));
        return;
      }

    // Consume white space.
    switch (r0)
      {
      case SPACE:
      case TAB:
      case CARRIAGE_RETURN:
        m_utf8_scanner.consume ();
        start ();
        return;
      case NEWLINE:
        m_utf8_scanner.consume ();
        if (should_insert_semicolon ())
          {
            m_last_token = Token::make (m_utf8_scanner.location (), Token::End);
            push (Token::make (m_utf8_scanner.location (), Token::Semicolon));
            return;
          }
        start ();
        return;
      }

    {
      // Three character operators/delimiters.
      std::string s;
      append (s, r0);
      append (s, r1);
      append (s, r2);
      MapType::const_iterator pos = m_operators_delimiters.find (s);
      if (pos != m_operators_delimiters.end ())
        {
          m_utf8_scanner.consume (3);
          push (Token::make (m_utf8_scanner.location (), pos->second));
          return;
        }
    }

    {
      // Two character operators/delimiters.
      std::string s;
      append (s, r0);
      append (s, r1);
      if (s == "//")
        {
          line_comment ();
          start ();
          return;
        }
      else if (s == "/*")
        {
          general_comment ();
          start ();
          return;
        }

      MapType::const_iterator pos = m_operators_delimiters.find (s);
      if (pos != m_operators_delimiters.end ())
        {
          m_utf8_scanner.consume (2);
          push (Token::make (m_utf8_scanner.location (), pos->second));
          return;
        }
    }

    if (r0 == '.' && is_decimal_digit (r1))
      {
        mpz_class value;
        finish_decimal (value, true);
        return;
      }

    {
      // One character operators/delimiters.
      std::string s;
      append (s, r0);
      MapType::const_iterator pos = m_operators_delimiters.find (s);
      if (pos != m_operators_delimiters.end ())
        {
          m_utf8_scanner.consume (1);
          push (Token::make (m_utf8_scanner.location (), pos->second));
          return;
        }
    }

    if (r0 == '0' &&
        (r1 == 'x' || r1 == 'X') &&
        is_hex_digit (r2))
      {
        m_utf8_scanner.consume (2);
        hex_literal ();
        return;
      }

    // Octal and decimal literals, rune literals.
    switch (r0)
      {
      case '0':
        octal_literal ();
        return;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        decimal_literal ();
        return;
      case '\'':
        rune_literal ();
        return;
      case '`':
        raw_string_literal ();
        return;
      case '"':
        interpreted_string_literal ();
        return;
      }

    if (is_letter (r0))
      {
        identifier ();
        return;
      }
    else
      {
        std::string x;
        append (x, r0);
        m_error_reporter << StrayRune (m_utf8_scanner.location (), x);
        m_utf8_scanner.consume ();
        // Recur.
        start ();
      }
  }

  // https://golang.org/ref/spec#Comments

  void Scanner::line_comment ()
  {
    for (;;)
      {
        Runet r = m_utf8_scanner.peek ();
        if (r == NEWLINE || r == END_RUNE)
          {
            break;
          }
        else
          {
            m_utf8_scanner.consume ();
          }
      }
  }

  void Scanner::general_comment ()
  {
    for (;;)
      {
        Runet r0 = m_utf8_scanner.peek (0);
        Runet r1 = m_utf8_scanner.peek (1);
        if (r0 == '*' && r1 == '/')
          {
            m_utf8_scanner.consume (2);
            break;
          }
        else if (r0 == END_RUNE)
          {
            m_error_reporter << UnterminatedGeneralComment (m_utf8_scanner.location ());
            break;
          }
        m_utf8_scanner.consume ();
      }
  }

  void Scanner::identifier ()
  {
    std::string s;
    Runet r = m_utf8_scanner.peek ();
    while (is_letter (r) || is_unicode_digit (r))
      {
        append (s, r);
        m_utf8_scanner.consume ();
        r = m_utf8_scanner.peek ();
      }

    end_of_word ();

    MapType::const_iterator pos = m_keywords.find (s);
    if (pos == m_keywords.end ())
      {
        push (Token::make_identifier (m_utf8_scanner.location (), s));
      }
    else
      {
        push (Token::make (m_utf8_scanner.location (), pos->second));
      }
  }

  void Scanner::decimal_literal ()
  {
    mpz_class value (0);
    Runet r = m_utf8_scanner.peek ();
    while (is_decimal_digit (r))
      {
        value = value * 10 + decimalValue (r);
        m_utf8_scanner.consume ();
        r = m_utf8_scanner.peek ();
      }

    if (r == '.')
      {
        finish_decimal (value, false);
      }
    else if (r == 'e' || r == 'E')
      {
        finish_exponent (value);
      }
    else
      {
        end_of_word ();
        push (Token::make_literal (m_utf8_scanner.location (), Value::make_integer (value)));
      }
  }

  void Scanner::octal_literal ()
  {
    mpz_class value (0);
    mpz_class dvalue (0);
    Runet r = m_utf8_scanner.peek ();
    while (is_octal_digit (r))
      {
        value = value * 8 + octalValue (r);
        dvalue = dvalue * 10 + decimalValue (r);
        m_utf8_scanner.consume ();
        r = m_utf8_scanner.peek ();
      }

    if (r == '.')
      {
        finish_decimal (dvalue, false);
      }
    else if (r == 'e' || r == 'E')
      {
        finish_exponent (dvalue);
      }
    else if (r == 'i')
      {
        m_utf8_scanner.consume ();
        end_of_word ();
        mpf_class f (dvalue, PRECISION);
        push (Token::make_literal (m_utf8_scanner.location (), Value::make_complex (0, f)));
      }
    else
      {
        end_of_word ();
        push (Token::make_literal (m_utf8_scanner.location (), Value::make_integer (value)));
      }
  }

  void Scanner::hex_literal ()
  {
    mpz_class value (0);
    Runet r = m_utf8_scanner.peek ();
    while (is_hex_digit (r))
      {
        value = value * 16 + hexValue (r);
        m_utf8_scanner.consume ();
        r = m_utf8_scanner.peek ();
      }

    end_of_word ();
    push (Token::make_literal (m_utf8_scanner.location (), Value::make_integer (value)));
  }

  mpf_class Scanner::form_float (const mpz_class& mantissa,
                                 const mpz_class& digitsAfter,
                                 bool exponentNegative,
                                 mpz_class& exponent)
  {
    static const mpf_class zero (0, PRECISION);
    static const mpf_class one (1, PRECISION);

    if (mantissa == 0)
      {
        return zero;
      }

    assert (mantissa > 0);

    if (exponentNegative)
      {
        exponent += digitsAfter;
      }
    else
      {
        exponent -= digitsAfter;
      }
    if (exponent < 0)
      {
        exponent = -exponent;
        exponentNegative = true;
      }

    mpf_class e (1, PRECISION);
    mpf_class t (10, PRECISION);

    for (size_t bit = 0, bits = mpz_sizeinbase (exponent.get_mpz_t (), 2);
         bit != bits;
         ++bit)
      {
        if (mpz_tstbit (exponent.get_mpz_t (), bit))
          {
            // No need to check here because the next check will suffice.
            e *= t;
          }
        t *= t;
        if (t < one)
          {
            m_error_reporter << FloatingPointOverflow (m_utf8_scanner.location ());
            return zero;
          }
      }

    mpf_class m (mantissa, PRECISION);
    // We know:
    //  mantissa is integral by construction
    //  mantissa >= 0 by construction, the sign is not part of the mantissa
    //  mantissa != 0 check upon entry
    //
    // Thus, m >= 1.

    if (m < one)
      {
        // Difficult to test because it requires a large mantissa.
        m_error_reporter << FloatingPointOverflow (m_utf8_scanner.location ()); // NOT_COVERED
        return zero; // NOT_COVERED
      }

    // We can assume that Still assuming m >= 1.
    if (exponentNegative)
      {
        const bool eSmall = e < 1;
        m /= e;
        const bool rSmall = m < 1;
        if (eSmall && rSmall)
          {
            // Difficult to test because it requires a large mantissa.
            m_error_reporter << FloatingPointOverflow (m_utf8_scanner.location ()); // NOT_COVERED
            return zero; // NOT_COVERED
          }
      }
    else
      {
        const bool eSmall = e < 1;
        m *= e;
        const bool rSmall = m < 1;
        if (!eSmall && rSmall)
          {
            // Difficult to test because it requires a large mantissa.
            m_error_reporter << FloatingPointOverflow (m_utf8_scanner.location ()); // NOT_COVERED
            return zero; // NOT_COVERED
          }
      }

    return m;
  }

  void Scanner::finish_decimal (mpz_class& value, bool requireDecimals)
  {
    Runet r = m_utf8_scanner.peek ();
    assert (r == '.');
    m_utf8_scanner.consume ();

    mpz_class digitsAfter;
    if (requireDecimals)
      {
        decimals (value, digitsAfter);
      }
    else
      {
        optional_decimals (value, digitsAfter);
      }

    bool exponentNegative;
    mpz_class z;
    optional_exponent (exponentNegative, z);
    mpf_class d = form_float (value, digitsAfter, exponentNegative, z);

    r = m_utf8_scanner.peek ();

    if (r == 'i')
      {
        m_utf8_scanner.consume ();
        end_of_word ();
        push (Token::make_literal (m_utf8_scanner.location (), Value::make_complex (0, d)));
      }
    else
      {
        end_of_word ();
        push (Token::make_literal (m_utf8_scanner.location (), Value::make_float (d)));
      }
  }

  void Scanner::finish_exponent (const mpz_class& value)
  {
    bool exponentNegative;
    mpz_class z;
    exponent (exponentNegative, z);
    mpf_class d = form_float (value, mpz_class (0), exponentNegative, z);

    Runet r = m_utf8_scanner.peek ();

    if (r == 'i')
      {
        m_utf8_scanner.consume ();
        end_of_word ();
        push (Token::make_literal (m_utf8_scanner.location (), Value::make_complex (0, d)));
      }
    else
      {
        end_of_word ();
        push (Token::make_literal (m_utf8_scanner.location (), Value::make_float (d)));
      }
  }

  void Scanner::optional_decimals (mpz_class& v, mpz_class& digits)
  {
    if (is_decimal_digit (m_utf8_scanner.peek ()))
      {
        decimals (v, digits);
      }
  }

  void Scanner::decimals (mpz_class& v, mpz_class& digits)
  {
    Runet r = m_utf8_scanner.peek ();
    while (is_decimal_digit (r))
      {
        v = v * 10 + decimalValue (r);
        m_utf8_scanner.consume ();
        r = m_utf8_scanner.peek ();
        digits += 1;
      }

    if (digits == 0)
      {
        m_error_reporter << ExpectedDecimalDigit (m_utf8_scanner.location ());
      }
  }

  void Scanner::optional_exponent (bool& exponentNegative, mpz_class& e)
  {
    exponentNegative = false;
    e = 0;
    Runet r = m_utf8_scanner.peek ();
    if (r == 'e' || r == 'E')
      {
        exponent (exponentNegative, e);
      }
  }

  void Scanner::exponent (bool& exponentNegative, mpz_class& e)
  {
    exponentNegative = false;
    e = 0;

    Runet r = m_utf8_scanner.peek ();
    assert (r == 'e' || r == 'E');
    m_utf8_scanner.consume ();

    r = m_utf8_scanner.peek ();
    if (r == '+')
      {
        m_utf8_scanner.consume ();
      }
    else if (r == '-')
      {
        m_utf8_scanner.consume ();
        exponentNegative = true;
      }

    mpz_class digits;
    decimals (e, digits);
  }

  void Scanner::rune_literal ()
  {
    Runet s = 0;
    bool error = false;
    bool isByte;

    // Opening '
    Runet r = m_utf8_scanner.peek ();
    assert (r == '\'');
    m_utf8_scanner.consume ();

    r = m_utf8_scanner.peek ();
    switch (r)
      {
      case END_RUNE:
      case NEWLINE:
        m_error_reporter << IncompleteRuneLiteral (m_utf8_scanner.location ());
        push (Token::make_literal (m_utf8_scanner.location (), Value::make_rune (s)));
        return;
      case '\'':
        m_error_reporter << EmptyRuneLiteral (m_utf8_scanner.location ());
        push (Token::make_literal (m_utf8_scanner.location (), Value::make_rune (s)));
        return;
      case '\\':
        s = escaped_rune (error, isByte, true, false);
        break;
      default:
        m_utf8_scanner.consume ();
        s = r;
        break;
      }

    // Closing '
    bool extra = false;
    for (;;)
      {
        r = m_utf8_scanner.peek ();
        if (r == '\'')
          {
            m_utf8_scanner.consume ();
            break;
          }
        else if (r == END_RUNE || r == NEWLINE)
          {
            m_error_reporter << IncompleteRuneLiteral (m_utf8_scanner.location ());
            break;
          }
        else
          {
            m_utf8_scanner.consume ();
            extra = true;
          }
      }

    if (extra && !error)
      {
        m_error_reporter << ExtraCharactersInRuneLiteral (m_utf8_scanner.location ());
      }

    push (Token::make_literal (m_utf8_scanner.location (), Value::make_rune (s)));
  }

  Runet Scanner::escaped_rune (bool& error, bool& isByte, bool escapeSingleQuote, bool escapeDoubleQuote)
  {
    error = false;
    isByte = false;

    Runet r = m_utf8_scanner.peek ();
    assert (r == '\\');
    m_utf8_scanner.consume ();

    Runet r0 = m_utf8_scanner.peek (0);
    Runet r1 = m_utf8_scanner.peek (1);
    Runet r2 = m_utf8_scanner.peek (2);
    Runet r3 = m_utf8_scanner.peek (3);
    Runet r4 = m_utf8_scanner.peek (4);
    Runet r5 = m_utf8_scanner.peek (5);
    Runet r6 = m_utf8_scanner.peek (6);
    Runet r7 = m_utf8_scanner.peek (7);
    Runet r8 = m_utf8_scanner.peek (8);

    // Check octal.
    if (is_octal_digit (r0))
      {
        isByte = true;
        if (is_octal_digit (r1) && is_octal_digit (r2))
          {
            m_utf8_scanner.consume (3);
            Runet s = (octalValue (r0) << 6) | (octalValue (r1) << 3) | (octalValue (r2) << 0);
            if (s > 255)
              {
                m_error_reporter << OctalValueOutOfRange (m_utf8_scanner.location ());
              }
            return s;
          }
        else
          {
            m_error_reporter << TooFewOctalDigitsInRuneLiteral (m_utf8_scanner.location ());
            error = true;
            return 0;
          }
      }

    switch (r0)
      {
      case 'x':
        if (is_hex_digit (r1) && is_hex_digit (r2))
          {
            isByte = true;
            m_utf8_scanner.consume (3);
            return (hexValue (r1) << 4) | (hexValue (r2) << 0);
          }
        else
          {
            m_error_reporter << TooFewHexadecimalDigitsInRuneLiteral (m_utf8_scanner.location ());
            error = true;
            return 0;
          }

      case 'u':
        if (is_hex_digit (r1) && is_hex_digit (r2) && is_hex_digit (r3) && is_hex_digit (r4))
          {
            m_utf8_scanner.consume (5);
            Runet s = (hexValue (r1) << 12) | (hexValue (r2) << 8) | (hexValue (r3) << 4) | (hexValue (r4) << 0);
            if (!is_unicode_char (s))
              {
                m_error_reporter << InvalidUnicodeCodePoint (m_utf8_scanner.location ());
              }
            return s;
          }
        else
          {
            m_error_reporter << TooFewHexadecimalDigitsInRuneLiteral (m_utf8_scanner.location ());
            error = true;
            return 0;
          }

      case 'U':
        if (is_hex_digit (r1) && is_hex_digit (r2) && is_hex_digit (r3) && is_hex_digit (r4) &&
            is_hex_digit (r5) && is_hex_digit (r6) && is_hex_digit (r7) && is_hex_digit (r8))
          {
            m_utf8_scanner.consume (9);
            Runet s =
              (hexValue (r1) << 28) | (hexValue (r2) << 24) | (hexValue (r3) << 20) | (hexValue (r4) << 16) |
              (hexValue (r5) << 12) | (hexValue (r6) << 8) | (hexValue (r7) << 4) | (hexValue (r8) << 0);
            if (!is_unicode_char (s))
              {
                m_error_reporter << InvalidUnicodeCodePoint (m_utf8_scanner.location ());
              }
            return s;
          }
        else
          {
            m_error_reporter << TooFewHexadecimalDigitsInRuneLiteral (m_utf8_scanner.location ());
            error = true;
            return 0;
          }

      case 'a':
        m_utf8_scanner.consume ();
        return 0x0007;
      case 'b':
        m_utf8_scanner.consume ();
        return 0x0008;
      case 'f':
        m_utf8_scanner.consume ();
        return 0x000C;
      case 'n':
        m_utf8_scanner.consume ();
        return 0x000A;
      case 'r':
        m_utf8_scanner.consume ();
        return 0x000D;
      case 't':
        m_utf8_scanner.consume ();
        return 0x0009;
      case 'v':
        m_utf8_scanner.consume ();
        return 0x000B;
      case '\\':
        m_utf8_scanner.consume ();
        return 0x005C;
      case '\'':
        if (escapeSingleQuote)
          {
            m_utf8_scanner.consume ();
            return 0x0027;
          }
        break;
      case '"':
        if (escapeDoubleQuote)
          {
            m_utf8_scanner.consume ();
            return 0x0022;
          }
        break;
      }

    m_error_reporter << IllegalEscapeSequence (m_utf8_scanner.location ());
    error = true;
    return 0;
  }

  void Scanner::raw_string_literal ()
  {
    std::string s;

    Runet r = m_utf8_scanner.peek ();
    assert (r == '`');
    m_utf8_scanner.consume ();

    for (;;)
      {
        r = m_utf8_scanner.peek ();
        switch (r)
          {
          case END_RUNE:
            m_error_reporter << IncompleteStringLiteral (m_utf8_scanner.location ());
            push (Token::make_literal (m_utf8_scanner.location (), Value::make_string (s)));
            return;
          case CARRIAGE_RETURN:
            // Skipped according to https://golang.org/ref/spec#String_literals
            m_utf8_scanner.consume ();
            break;
          case '`':
            m_utf8_scanner.consume ();
            push (Token::make_literal (m_utf8_scanner.location (), Value::make_string (s)));
            return;
          default:
            m_utf8_scanner.consume ();
            append (s, r);
            break;
          }
      }
  }

  void Scanner::interpreted_string_literal ()
  {
    std::string s;

    Runet r = m_utf8_scanner.peek ();
    assert (r == '"');
    m_utf8_scanner.consume ();

    for (;;)
      {
        r = m_utf8_scanner.peek ();
        switch (r)
          {
          case END_RUNE:
          case NEWLINE:
            m_error_reporter << IncompleteStringLiteral (m_utf8_scanner.location ());
            push (Token::make_literal (m_utf8_scanner.location (), Value::make_string (s)));
            return;
          case '"':
            m_utf8_scanner.consume ();
            push (Token::make_literal (m_utf8_scanner.location (), Value::make_string (s)));
            return;
          case '\\':
            {
              bool error;
              bool isByte;
              Runet t = escaped_rune (error, isByte, false, true);
              if (isByte)
                {
                  s += static_cast<char> (t);
                }
              else
                {
                  append (s, t);
                }
            }
            break;
          default:
            m_utf8_scanner.consume ();
            append (s, r);
            break;
          }
      }
  }


  Token Scanner::peek (size_t offset)
  {
    while (offset >= m_tokens.size ())
      {
        start ();
      }

    return m_tokens[offset];
  }

  Token Scanner::consume ()
  {
    peek ();
    Token t = m_tokens.front ();
    m_tokens.pop_front ();
    return t;
  }

  void Scanner::push (const Token& token)
  {
    m_tokens.push_back (token);
    m_last_token = token;
  }

  // https://golang.org/ref/spec#Semicolons
  bool Scanner::should_insert_semicolon () const
  {
    switch (m_last_token.kind ())
      {
      case Token::Identifier:
      case Token::Literal:
      case Token::Break:
      case Token::Continue:
      case Token::Fallthrough:
      case Token::Return:
      case Token::Increment:
      case Token::Decrement:
      case Token::RightParen:
      case Token::RightBracket:
      case Token::RightBrace:
        return true;
      default:
        return false;
      }
  }

  void Scanner::end_of_word ()
  {
    // Identifiers, keywords, and literals (words) should be separated by operators, delimiters, whitespace, and comments.
    // This check is to cause something like 1234abc to get parsed as one token (1234) and an error instead of two tokens (1234 and abc);

    std::string s;

    for (;;)
      {
        Runet r = m_utf8_scanner.peek ();
        switch (r)
          {
          case '!':
          case '%':
          case '&':
          case '(':
          case ')':
          case '*':
          case '+':
          case ',':
          case '-':
          case '.':
          case '/':
          case ':':
          case ';':
          case '<':
          case '=':
          case '>':
          case '[':
          case ']':
          case '^':
          case '{':
          case '|':
          case '}':
          case SPACE:
          case TAB:
          case CARRIAGE_RETURN:
          case NEWLINE:
          case END_RUNE:
            goto done;
          default:
            m_utf8_scanner.consume ();
            append (s, r);
            break;
          }
      }
  done:

    if (!s.empty ())
      {
        m_error_reporter << ExtraCharactersInToken (m_utf8_scanner.location ());
      }
  }

}
