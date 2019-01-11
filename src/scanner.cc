// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/scanner.h"

#include <cassert>
#include <sstream>

#include "src/error_reporter.h"

#define SPACE 0x20
#define TAB 0x09
#define CARRIAGE_RETURN 0x0D
#define NEWLINE 0x0A

namespace rcgo {

static Runet octalValue(Runet s) {
  return s - '0';
}

static Runet decimalValue(Runet s) {
  return s - '0';
}

static Runet hexValue(Runet s) {
  if (s >= '0' && s <= '9') {
    return s - '0';
  } else if (s >= 'a' && s <= 'f') {
    return s - 'a' + 10;
  } else {
    return s - 'A' + 10;
  }
}

void Scanner::Populate(MapType* map, const Token& token) {
  std::stringstream ss;
  ss << token;
  (*map)[ss.str()] = token;
}

Scanner::Scanner(ByteStreamI* byteStream,
                 ErrorReporter* aErrorReporter)
    : m_utf8_scanner(byteStream, aErrorReporter)
    , m_error_reporter(aErrorReporter)
    , m_last_token(Token::Make(m_utf8_scanner.GetLocation(), Token::kEnd)) {
  Populate(&m_keywords, Token::Make(Token::kBreak));
  Populate(&m_keywords, Token::Make(Token::kCase));
  Populate(&m_keywords, Token::Make(Token::kConst));
  Populate(&m_keywords, Token::Make(Token::kContinue));
  Populate(&m_keywords, Token::Make(Token::kDefault));
  Populate(&m_keywords, Token::Make(Token::kDefer));
  Populate(&m_keywords, Token::Make(Token::kElse));
  Populate(&m_keywords, Token::Make(Token::kFallthrough));
  Populate(&m_keywords, Token::Make(Token::kFor));
  Populate(&m_keywords, Token::Make(Token::kFunc));
  Populate(&m_keywords, Token::Make(Token::kGoto));
  Populate(&m_keywords, Token::Make(Token::kIf));
  Populate(&m_keywords, Token::Make(Token::kImport));
  Populate(&m_keywords, Token::Make(Token::kInterface));
  Populate(&m_keywords, Token::Make(Token::kMap));
  Populate(&m_keywords, Token::Make(Token::kPackage));
  Populate(&m_keywords, Token::Make(Token::kRange));
  Populate(&m_keywords, Token::Make(Token::kReturn));
  Populate(&m_keywords, Token::Make(Token::kStruct));
  Populate(&m_keywords, Token::Make(Token::kSwitch));
  Populate(&m_keywords, Token::Make(Token::kType));
  Populate(&m_keywords, Token::Make(Token::kVar));

  Populate(&m_operators_delimiters, Token::Make(Token::kPosate));
  Populate(&m_operators_delimiters, Token::Make(Token::kNegate));
  Populate(&m_operators_delimiters, Token::Make(Token::kPointer));
  Populate(&m_operators_delimiters, Token::Make(Token::kDivide));
  Populate(&m_operators_delimiters, Token::Make(Token::kModulus));

  Populate(&m_operators_delimiters, Token::Make(Token::kAddress));
  Populate(&m_operators_delimiters, Token::Make(Token::kBitNot));
  Populate(&m_operators_delimiters, Token::Make(Token::kBitOr));

  Populate(&m_operators_delimiters, Token::Make(Token::kLeftShift));
  Populate(&m_operators_delimiters, Token::Make(Token::kRightShift));
  Populate(&m_operators_delimiters, Token::Make(Token::kBitAndNot));

  Populate(&m_operators_delimiters, Token::Make(Token::kAddAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kSubtractAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kMultiplyAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kDivideAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kModulusAssign));

  Populate(&m_operators_delimiters, Token::Make(Token::kBitAndAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kBitOrAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kBitXorAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kLeftShiftAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kRightShiftAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kBitAndNotAssign));

  Populate(&m_operators_delimiters, Token::Make(Token::kLogicalAnd));
  Populate(&m_operators_delimiters, Token::Make(Token::kLogicalOr));
  Populate(&m_operators_delimiters, Token::Make(Token::kIncrement));
  Populate(&m_operators_delimiters, Token::Make(Token::kDecrement));

  Populate(&m_operators_delimiters, Token::Make(Token::kEqual));
  Populate(&m_operators_delimiters, Token::Make(Token::kLessThan));
  Populate(&m_operators_delimiters, Token::Make(Token::kGreaterThan));
  Populate(&m_operators_delimiters, Token::Make(Token::kAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kLogicalNot));

  Populate(&m_operators_delimiters, Token::Make(Token::kNotEqual));
  Populate(&m_operators_delimiters, Token::Make(Token::kLessThanEqual));
  Populate(&m_operators_delimiters, Token::Make(Token::kGreaterThanEqual));
  Populate(&m_operators_delimiters, Token::Make(Token::kShortAssign));
  Populate(&m_operators_delimiters, Token::Make(Token::kEllipsis));

  Populate(&m_operators_delimiters, Token::Make(Token::kLeftParen));
  Populate(&m_operators_delimiters, Token::Make(Token::kLeftBracket));
  Populate(&m_operators_delimiters, Token::Make(Token::kLeftBrace));
  Populate(&m_operators_delimiters, Token::Make(Token::kComma));
  Populate(&m_operators_delimiters, Token::Make(Token::kPeriod));

  Populate(&m_operators_delimiters, Token::Make(Token::kRightParen));
  Populate(&m_operators_delimiters, Token::Make(Token::kRightBracket));
  Populate(&m_operators_delimiters, Token::Make(Token::kRightBrace));
  Populate(&m_operators_delimiters, Token::Make(Token::kSemicolon));
  Populate(&m_operators_delimiters, Token::Make(Token::kColon));
}

Scanner::~Scanner() { }

// https://golang.org/ref/spec#Tokens
void Scanner::Start() {
  Runet r0 = m_utf8_scanner.Peek(0);
  Runet r1 = m_utf8_scanner.Peek(1);
  Runet r2 = m_utf8_scanner.Peek(2);

  if (r0 == kEndRune) {
    if (ShouldInsertSemicolon()) {
      m_last_token = Token::Make(m_utf8_scanner.GetLocation(), Token::kEnd);
      Push(Token::Make(m_utf8_scanner.GetLocation(), Token::kSemicolon));
      return;
    }
    Push(Token::Make(m_utf8_scanner.GetLocation(), Token::kEnd));
    return;
  }

  // Consume white space.
  switch (r0) {
    case SPACE:
    case TAB:
    case CARRIAGE_RETURN:
      m_utf8_scanner.Consume();
      Start();
      return;
    case NEWLINE:
      m_utf8_scanner.Consume();
      if (ShouldInsertSemicolon()) {
        m_last_token = Token::Make(m_utf8_scanner.GetLocation(), Token::kEnd);
        Push(Token::Make(m_utf8_scanner.GetLocation(), Token::kSemicolon));
        return;
      }
      Start();
      return;
  }

  {
    // Three character operators/delimiters.
    std::string s;
    Append(&s, r0);
    Append(&s, r1);
    Append(&s, r2);
    MapType::const_iterator pos = m_operators_delimiters.find(s);
    if (pos != m_operators_delimiters.end()) {
      m_utf8_scanner.Consume(3);
      Push(Token::Make(m_utf8_scanner.GetLocation(), pos->second));
      return;
    }
  }

  {
    // Two character operators/delimiters.
    std::string s;
    Append(&s, r0);
    Append(&s, r1);
    if (s == "//") {
      LineComment();
      Start();
      return;
    } else if (s == "/*") {
      GeneralComment();
      Start();
      return;
    }

    MapType::const_iterator pos = m_operators_delimiters.find(s);
    if (pos != m_operators_delimiters.end()) {
      m_utf8_scanner.Consume(2);
      Push(Token::Make(m_utf8_scanner.GetLocation(), pos->second));
      return;
    }
  }

  if (r0 == '.' && IsDecimalDigit(r1)) {
    mpz_class value;
    FinishDecimal(&value, true);
    return;
  }

  {
    // One character operators/delimiters.
    std::string s;
    Append(&s, r0);
    MapType::const_iterator pos = m_operators_delimiters.find(s);
    if (pos != m_operators_delimiters.end()) {
      m_utf8_scanner.Consume(1);
      Push(Token::Make(m_utf8_scanner.GetLocation(), pos->second));
      return;
    }
  }

  if (r0 == '0' &&
     (r1 == 'x' || r1 == 'X') &&
     IsHexDigit(r2)) {
    m_utf8_scanner.Consume(2);
    HexLiteral();
    return;
  }

  // Octal and decimal literals, rune literals.
  switch (r0) {
    case '0':
      OctalLiteral();
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
      DecimalLiteral();
      return;
    case '\'':
      RuneLiteral();
      return;
    case '`':
      RawStringLiteral();
      return;
    case '"':
      InterpretedStringLiteral();
      return;
  }

  if (IsLetter(r0)) {
    Identifier();
    return;
  } else {
    std::string x;
    Append(&x, r0);
    m_error_reporter->Insert(StrayRune(m_utf8_scanner.GetLocation(), x));
    m_utf8_scanner.Consume();
    // Recur.
    Start();
  }
}

// https://golang.org/ref/spec#Comments

void Scanner::LineComment() {
  for (;;) {
    Runet r = m_utf8_scanner.Peek();
    if (r == NEWLINE || r == kEndRune) {
      break;
    } else {
      m_utf8_scanner.Consume();
    }
  }
}

void Scanner::GeneralComment() {
  for (;;) {
    Runet r0 = m_utf8_scanner.Peek(0);
    Runet r1 = m_utf8_scanner.Peek(1);
    if (r0 == '*' && r1 == '/') {
      m_utf8_scanner.Consume(2);
      break;
    } else if (r0 == kEndRune) {
      m_error_reporter->Insert(
          UnterminatedGeneralComment(m_utf8_scanner.GetLocation()));
      break;
    }
    m_utf8_scanner.Consume();
  }
}

void Scanner::Identifier() {
  std::string s;
  Runet r = m_utf8_scanner.Peek();
  while (IsLetter(r) || IsUnicodeDigit(r)) {
    Append(&s, r);
    m_utf8_scanner.Consume();
    r = m_utf8_scanner.Peek();
  }

  EndOfWord();

  MapType::const_iterator pos = m_keywords.find(s);
  if (pos == m_keywords.end()) {
    Push(Token::MakeIdentifier(m_utf8_scanner.GetLocation(), s));
  } else {
    Push(Token::Make(m_utf8_scanner.GetLocation(), pos->second));
  }
}

void Scanner::DecimalLiteral() {
  mpz_class value(0);
  Runet r = m_utf8_scanner.Peek();
  while (IsDecimalDigit(r)) {
    value = value * 10 + decimalValue(r);
    m_utf8_scanner.Consume();
    r = m_utf8_scanner.Peek();
  }

  if (r == '.') {
    FinishDecimal(&value, false);
  } else if (r == 'e' || r == 'E') {
    FinishExponent(value);
  } else {
    EndOfWord();
    Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                            value::Value::MakeInteger(value)));
  }
}

void Scanner::OctalLiteral() {
  mpz_class value(0);
  mpz_class dvalue(0);
  Runet r = m_utf8_scanner.Peek();
  while (IsOctalDigit(r)) {
    value = value * 8 + octalValue(r);
    dvalue = dvalue * 10 + decimalValue(r);
    m_utf8_scanner.Consume();
    r = m_utf8_scanner.Peek();
  }

  if (r == '.') {
    FinishDecimal(&dvalue, false);
  } else if (r == 'e' || r == 'E') {
    FinishExponent(dvalue);
  } else if (r == 'i') {
    m_utf8_scanner.Consume();
    EndOfWord();
    mpf_class f(dvalue, PRECISION);
    Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                            value::Value::MakeComplex(0, f)));
  } else {
    EndOfWord();
    Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                            value::Value::MakeInteger(value)));
  }
}

void Scanner::HexLiteral() {
  mpz_class value(0);
  Runet r = m_utf8_scanner.Peek();
  while (IsHexDigit(r)) {
    value = value * 16 + hexValue(r);
    m_utf8_scanner.Consume();
    r = m_utf8_scanner.Peek();
  }

  EndOfWord();
  Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                          value::Value::MakeInteger(value)));
}

mpf_class Scanner::FormFloat(const mpz_class& mantissa,
                             const mpz_class& digitsAfter,
                             bool exponentNegative,
                             mpz_class* exponent) {
  static const mpf_class zero(0, PRECISION);
  static const mpf_class one(1, PRECISION);

  if (mantissa == 0) {
    return zero;
  }

  assert(mantissa > 0);

  if (exponentNegative) {
    *exponent += digitsAfter;
  } else {
    *exponent -= digitsAfter;
  }
  if (*exponent < 0) {
    *exponent = -*exponent;
    exponentNegative = true;
  }

  mpf_class e(1, PRECISION);
  mpf_class t(10, PRECISION);

  for (size_t bit = 0, bits = mpz_sizeinbase(exponent->get_mpz_t(), 2);
      bit != bits;
      ++bit) {
    if (mpz_tstbit(exponent->get_mpz_t(), bit)) {
      // No need to check here because the next check will suffice.
      e *= t;
    }
    t *= t;
    if (t < one) {
      m_error_reporter->Insert(
          FloatingPointOverflow(m_utf8_scanner.GetLocation()));
      return zero;
    }
  }

  mpf_class m(mantissa, PRECISION);
  // We know:
  //  mantissa is integral by construction
  //  mantissa >= 0 by construction, the sign is not part of the mantissa
  //  mantissa != 0 check upon entry
  //
  // Thus, m >= 1.

  if (m < one) {
    // Difficult to test because it requires a large mantissa.
    m_error_reporter->Insert(
        FloatingPointOverflow(m_utf8_scanner.GetLocation()));  // NOT_COVERED
    return zero;  // NOT_COVERED
  }

  // We can assume that Still assuming m >= 1.
  if (exponentNegative) {
    const bool eSmall = e < 1;
    m /= e;
    const bool rSmall = m < 1;
    if (eSmall && rSmall) {
      // Difficult to test because it requires a large mantissa.
      m_error_reporter->Insert(
          FloatingPointOverflow(m_utf8_scanner.GetLocation()));  // NOT_COVERED
      return zero;  // NOT_COVERED
    }
  } else {
    const bool eSmall = e < 1;
    m *= e;
    const bool rSmall = m < 1;
    if (!eSmall && rSmall) {
      // Difficult to test because it requires a large mantissa.
      m_error_reporter->Insert(
          FloatingPointOverflow(m_utf8_scanner.GetLocation()));  // NOT_COVERED
      return zero;  // NOT_COVERED
    }
  }

  return m;
}

void Scanner::FinishDecimal(mpz_class* value, bool requireDecimals) {
  Runet r = m_utf8_scanner.Peek();
  assert(r == '.');
  m_utf8_scanner.Consume();

  mpz_class digitsAfter;
  if (requireDecimals) {
    Decimals(value, &digitsAfter);
  } else {
    OptionalDecimals(value, &digitsAfter);
  }

  bool exponentNegative;
  mpz_class z;
  OptionalExponent(&exponentNegative, &z);
  mpf_class d = FormFloat(*value, digitsAfter, exponentNegative, &z);

  r = m_utf8_scanner.Peek();

  if (r == 'i') {
    m_utf8_scanner.Consume();
    EndOfWord();
    Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                            value::Value::MakeComplex(0, d)));
  } else {
    EndOfWord();
    Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                            value::Value::MakeFloat(d)));
  }
}

void Scanner::FinishExponent(const mpz_class& value) {
  bool exponentNegative;
  mpz_class z;
  Exponent(&exponentNegative, &z);
  mpf_class d = FormFloat(value, mpz_class(0), exponentNegative, &z);

  Runet r = m_utf8_scanner.Peek();

  if (r == 'i') {
    m_utf8_scanner.Consume();
    EndOfWord();
    Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                            value::Value::MakeComplex(0, d)));
  } else {
    EndOfWord();
    Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                            value::Value::MakeFloat(d)));
  }
}

void Scanner::OptionalDecimals(mpz_class* v, mpz_class* digits) {
  if (IsDecimalDigit(m_utf8_scanner.Peek())) {
    Decimals(v, digits);
  }
}

void Scanner::Decimals(mpz_class* v, mpz_class* digits) {
  Runet r = m_utf8_scanner.Peek();
  while (IsDecimalDigit(r)) {
    *v = *v * 10 + decimalValue(r);
    m_utf8_scanner.Consume();
    r = m_utf8_scanner.Peek();
    *digits += 1;
  }

  if (*digits == 0) {
    m_error_reporter->Insert(
        ExpectedDecimalDigit(m_utf8_scanner.GetLocation()));
  }
}

void Scanner::OptionalExponent(bool* exponentNegative, mpz_class* e) {
  *exponentNegative = false;
  *e = 0;
  Runet r = m_utf8_scanner.Peek();
  if (r == 'e' || r == 'E') {
    Exponent(exponentNegative, e);
  }
}

void Scanner::Exponent(bool* exponentNegative, mpz_class* e) {
  *exponentNegative = false;
  *e = 0;

  Runet r = m_utf8_scanner.Peek();
  assert(r == 'e' || r == 'E');
  m_utf8_scanner.Consume();

  r = m_utf8_scanner.Peek();
  if (r == '+') {
    m_utf8_scanner.Consume();
  } else if (r == '-') {
    m_utf8_scanner.Consume();
    *exponentNegative = true;
  }

  mpz_class digits;
  Decimals(e, &digits);
}

void Scanner::RuneLiteral() {
  Runet s = 0;
  bool error = false;
  bool isByte;

  // Opening '
  Runet r = m_utf8_scanner.Peek();
  assert(r == '\'');
  m_utf8_scanner.Consume();

  r = m_utf8_scanner.Peek();
  switch (r) {
    case kEndRune:
    case NEWLINE:
      m_error_reporter->Insert(
          IncompleteRuneLiteral(m_utf8_scanner.GetLocation()));
      Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                              value::Value::MakeRune(s)));
      return;
    case '\'':
      m_error_reporter->Insert(
          EmptyRuneLiteral(m_utf8_scanner.GetLocation()));
      Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                              value::Value::MakeRune(s)));
      return;
    case '\\':
      s = EscapedRune(&error, &isByte, true, false);
      break;
    default:
      m_utf8_scanner.Consume();
      s = r;
      break;
  }

  // Closing '
  bool extra = false;
  for (;;) {
    r = m_utf8_scanner.Peek();
    if (r == '\'') {
      m_utf8_scanner.Consume();
      break;
    } else if (r == kEndRune || r == NEWLINE) {
      m_error_reporter->Insert(
          IncompleteRuneLiteral(m_utf8_scanner.GetLocation()));
      break;
    } else {
      m_utf8_scanner.Consume();
      extra = true;
    }
  }

  if (extra && !error) {
    m_error_reporter->Insert(
        ExtraCharactersInRuneLiteral(m_utf8_scanner.GetLocation()));
  }

  Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                          value::Value::MakeRune(s)));
}

Runet Scanner::EscapedRune(bool* error, bool* isByte, bool escapeSingleQuote,
                           bool escapeDoubleQuote) {
  *error = false;
  *isByte = false;

  Runet r = m_utf8_scanner.Peek();
  assert(r == '\\');
  m_utf8_scanner.Consume();

  Runet r0 = m_utf8_scanner.Peek(0);
  Runet r1 = m_utf8_scanner.Peek(1);
  Runet r2 = m_utf8_scanner.Peek(2);
  Runet r3 = m_utf8_scanner.Peek(3);
  Runet r4 = m_utf8_scanner.Peek(4);
  Runet r5 = m_utf8_scanner.Peek(5);
  Runet r6 = m_utf8_scanner.Peek(6);
  Runet r7 = m_utf8_scanner.Peek(7);
  Runet r8 = m_utf8_scanner.Peek(8);

  // Check octal.
  if (IsOctalDigit(r0)) {
    *isByte = true;
    if (IsOctalDigit(r1) && IsOctalDigit(r2)) {
      m_utf8_scanner.Consume(3);
      Runet s = (octalValue(r0) << 6) | (octalValue(r1) << 3) |
          (octalValue(r2) << 0);
      if (s > 255) {
        m_error_reporter->Insert(
            OctalValueOutOfRange(m_utf8_scanner.GetLocation()));
      }
      return s;
    } else {
      m_error_reporter->Insert(
          TooFewOctalDigitsInRuneLiteral(m_utf8_scanner.GetLocation()));
      *error = true;
      return 0;
    }
  }

  switch (r0) {
    case 'x':
      if (IsHexDigit(r1) && IsHexDigit(r2)) {
        *isByte = true;
        m_utf8_scanner.Consume(3);
        return(hexValue(r1) << 4) | (hexValue(r2) << 0);
      } else {
        m_error_reporter->Insert(
            TooFewHexadecimalDigitsInRuneLiteral(m_utf8_scanner.GetLocation()));
        *error = true;
        return 0;
      }

    case 'u':
      if (IsHexDigit(r1) && IsHexDigit(r2) && IsHexDigit(r3)
          && IsHexDigit(r4)) {
        m_utf8_scanner.Consume(5);
        Runet s = (hexValue(r1) << 12) | (hexValue(r2) << 8) |
            (hexValue(r3) << 4) | (hexValue(r4) << 0);
        if (!IsUnicodeChar(s)) {
          m_error_reporter->Insert(
              InvalidUnicodeCodePoint(m_utf8_scanner.GetLocation()));
        }
        return s;
      } else {
        m_error_reporter->Insert(
            TooFewHexadecimalDigitsInRuneLiteral(m_utf8_scanner.GetLocation()));
        *error = true;
        return 0;
      }

    case 'U':
      if (IsHexDigit(r1) && IsHexDigit(r2) && IsHexDigit(r3) &&
          IsHexDigit(r4) && IsHexDigit(r5) && IsHexDigit(r6) &&
          IsHexDigit(r7) && IsHexDigit(r8)) {
        m_utf8_scanner.Consume(9);
        Runet s =
            (hexValue(r1) << 28) | (hexValue(r2) << 24) | (hexValue(r3) << 20) |
            (hexValue(r4) << 16) | (hexValue(r5) << 12) | (hexValue(r6) << 8) |
            (hexValue(r7) << 4) | (hexValue(r8) << 0);
        if (!IsUnicodeChar(s)) {
          m_error_reporter->Insert(
              InvalidUnicodeCodePoint(m_utf8_scanner.GetLocation()));
        }
        return s;
      } else {
        m_error_reporter->Insert(
            TooFewHexadecimalDigitsInRuneLiteral(m_utf8_scanner.GetLocation()));
        *error = true;
        return 0;
      }

    case 'a':
      m_utf8_scanner.Consume();
      return 0x0007;
    case 'b':
      m_utf8_scanner.Consume();
      return 0x0008;
    case 'f':
      m_utf8_scanner.Consume();
      return 0x000C;
    case 'n':
      m_utf8_scanner.Consume();
      return 0x000A;
    case 'r':
      m_utf8_scanner.Consume();
      return 0x000D;
    case 't':
      m_utf8_scanner.Consume();
      return 0x0009;
    case 'v':
      m_utf8_scanner.Consume();
      return 0x000B;
    case '\\':
      m_utf8_scanner.Consume();
      return 0x005C;
    case '\'':
      if (escapeSingleQuote) {
        m_utf8_scanner.Consume();
        return 0x0027;
      }
      break;
    case '"':
      if (escapeDoubleQuote) {
        m_utf8_scanner.Consume();
        return 0x0022;
      }
      break;
  }

  m_error_reporter->Insert(IllegalEscapeSequence(m_utf8_scanner.GetLocation()));
  *error = true;
  return 0;
}

void Scanner::RawStringLiteral() {
  std::string s;

  Runet r = m_utf8_scanner.Peek();
  assert(r == '`');
  m_utf8_scanner.Consume();

  for (;;) {
    r = m_utf8_scanner.Peek();
    switch (r) {
      case kEndRune:
        m_error_reporter->Insert(
            IncompleteStringLiteral(m_utf8_scanner.GetLocation()));
        Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                                value::Value::MakeString(s)));
        return;
      case CARRIAGE_RETURN:
        // Skipped according to https://golang.org/ref/spec#String_literals
        m_utf8_scanner.Consume();
        break;
      case '`':
        m_utf8_scanner.Consume();
        Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                                value::Value::MakeString(s)));
        return;
      default:
        m_utf8_scanner.Consume();
        Append(&s, r);
        break;
    }
  }
}

void Scanner::InterpretedStringLiteral() {
  std::string s;

  Runet r = m_utf8_scanner.Peek();
  assert(r == '"');
  m_utf8_scanner.Consume();

  for (;;) {
    r = m_utf8_scanner.Peek();
    switch (r) {
      case kEndRune:
      case NEWLINE:
        m_error_reporter->Insert(
            IncompleteStringLiteral(m_utf8_scanner.GetLocation()));
        Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                                value::Value::MakeString(s)));
        return;
      case '"':
        m_utf8_scanner.Consume();
        Push(Token::MakeLiteral(m_utf8_scanner.GetLocation(),
                                value::Value::MakeString(s)));
        return;
      case '\\':
        {
          bool error;
          bool isByte;
          Runet t = EscapedRune(&error, &isByte, false, true);
          if (isByte) {
            s += static_cast<char>(t);
          } else {
            Append(&s, t);
          }
        }
        break;
      default:
        m_utf8_scanner.Consume();
        Append(&s, r);
        break;
    }
  }
}


Token Scanner::Peek(size_t offset) {
  while (offset >= m_tokens.size()) {
    Start();
  }

  return m_tokens[offset];
}

Token Scanner::Consume() {
  Peek();
  Token t = m_tokens.front();
  m_tokens.pop_front();
  return t;
}

void Scanner::Push(const Token& token) {
  m_tokens.push_back(token);
  m_last_token = token;
}

// https://golang.org/ref/spec#Semicolons
bool Scanner::ShouldInsertSemicolon() const {
  switch (m_last_token.kind()) {
    case Token::kIdentifier:
    case Token::kLiteral:
    case Token::kBreak:
    case Token::kContinue:
    case Token::kFallthrough:
    case Token::kReturn:
    case Token::kIncrement:
    case Token::kDecrement:
    case Token::kRightParen:
    case Token::kRightBracket:
    case Token::kRightBrace:
      return true;
    default:
      return false;
  }
}

void Scanner::EndOfWord() {
  // Identifiers, keywords, and literals(words) should be separated by
  // operators, delimiters, whitespace, and comments.  This check is
  // to cause something like 1234abc to get parsed as one token(1234)
  // and an error instead of two tokens(1234 and abc);

  std::string s;

  for (;;) {
    Runet r = m_utf8_scanner.Peek();
    switch (r) {
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
      case kEndRune:
        goto done;
      default:
        m_utf8_scanner.Consume();
        Append(&s, r);
        break;
    }
  }
 done:

  if (!s.empty()) {
    m_error_reporter->Insert(
        ExtraCharactersInToken(m_utf8_scanner.GetLocation()));
  }
}

}  // namespace rcgo
