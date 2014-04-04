/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Utf8.hpp"

#include <cstdlib>

#include "ByteStream.hpp"
#include "Error.hpp"
#include "functions.hpp"

namespace rcgo {

  Utf8Scanner::Utf8Scanner (ByteStream& aByteStream,
                            ErrorReporter& aErrorReporter)
    : m_byte_stream (aByteStream)
    , m_error_reporter (aErrorReporter)
    , m_line (1)
  { }

  Utf8Scanner::~Utf8Scanner ()
  { }

  namespace {

    struct UScanner {
      UScanner (ByteStream& a_byte_stream)
        : byte_stream (a_byte_stream)
        , rune (0)
        , remaining_continuation_bytes (0)
      { }
      virtual ~UScanner () { }
      virtual void illegal_utf8_sequence () { /* NOT_COVERED */ }

      ByteStream& byte_stream;
      Runet rune;
      size_t remaining_continuation_bytes;
      Runet first_rune;
    };

    struct ReportingUScanner : public UScanner {
      ReportingUScanner (ByteStream& a_byte_stream,
                         Utf8Scanner& a_scanner,
                         ErrorReporter& a_error_reporter)
        : UScanner (a_byte_stream)
        , scanner (a_scanner)
        , error_reporter (a_error_reporter)
      { }
      void illegal_utf8_sequence ()
      {
        error_reporter << IllegalUtf8Sequence (scanner.location ());
      }

      Utf8Scanner& scanner;
      ErrorReporter& error_reporter;
    };

    struct State {
      State (State (*aFunc) (UScanner&))
        : func (aFunc)
      { }

      State (*func) (UScanner&);

      bool done () const { return func == NULL; }

      State operator() (UScanner& scanner)
      {
        return func (scanner);
      }
    };

    State Leading (UScanner& scanner);
    State Continuation (UScanner& scanner);
    State ConsumeTrash (UScanner& scanner);

    // Expect a leading byte.
    State Leading (UScanner& scanner)
    {
      // Peek at the byte.
      Byte b = scanner.byte_stream.peek ();
      scanner.byte_stream.consume ();
      if (b == END_BYTE)
        {
          scanner.rune = END_RUNE;
          return State (NULL);
        }

      if ((b & 0x80) == 0x00)
        {
          // 1 byte sequence
          scanner.rune = b;
          return State (NULL);
        }
      else if ((b & 0xE0) == 0xC0)
        {
          // 2 byte sequence
          scanner.rune = b & 0x1F;
          scanner.remaining_continuation_bytes = 1;
          scanner.first_rune = 0x0080;
          return State (Continuation);
        }
      else if ((b & 0xF0) == 0xE0)
        {
          // 3 byte sequence
          scanner.rune = b & 0x0F;
          scanner.remaining_continuation_bytes = 2;
          scanner.first_rune = 0x0800;
          return State (Continuation);
        }
      else if ((b & 0xF8) == 0xF0)
        {
          // 4 byte sequence
          scanner.rune = b & 0x07;
          scanner.remaining_continuation_bytes = 3;
          scanner.first_rune = 0x10000;
          return State (Continuation);
        }
      else
        {
          // Illegal leading byte
          scanner.illegal_utf8_sequence ();
          return State (ConsumeTrash);
        }
    }

    // Expect a continuation byte.
    State Continuation (UScanner& scanner)
    {
      // Peek at the character.
      Byte b = scanner.byte_stream.peek ();
      scanner.byte_stream.consume ();
      if (b == END_BYTE)
        {
          scanner.illegal_utf8_sequence ();
          scanner.rune = END_RUNE;
          return State (NULL);
        }

      if ((b & 0xC0) == 0x80)
        {
          // Valid continuation byte.
          scanner.rune = (scanner.rune << 6) | (b & 0x3F);
          --scanner.remaining_continuation_bytes;
          if (scanner.remaining_continuation_bytes != 0)
            {
              return State (Continuation);
            }

          if (scanner.rune < scanner.first_rune)
            {
              // Overlong.
              scanner.illegal_utf8_sequence ();
              return State (ConsumeTrash);
            }


          if (scanner.rune >= INVALID)
            {
              // Invalid.
              scanner.illegal_utf8_sequence ();
              return State (ConsumeTrash);
            }

          if (scanner.rune >= SURROGATE_FIRST && scanner.rune <= SURROGATE_LAST)
            {
              // Illegal surrogate half.
              scanner.illegal_utf8_sequence ();
              return State (ConsumeTrash);
            }

          return State (NULL);
        }
      else
        {
          // Not a valid continuation byte.
          scanner.illegal_utf8_sequence ();
          return State (ConsumeTrash);
        }
    }

    State ConsumeTrash (UScanner& scanner)
    {
      // Peek at the byte.
      Byte b = scanner.byte_stream.peek ();
      if (b == END_BYTE ||
          (b & 0x80) == 0x00 ||
          (b & 0xE0) == 0xC0 ||
          (b & 0xF0) == 0xE0 ||
          (b & 0xF8) == 0xF0)
        {
          return State (Leading);
        }
      else
        {
          scanner.byte_stream.consume ();
          return State (ConsumeTrash);
        }
    }
  }

  Runet Utf8Scanner::peek (size_t offset)
  {
    while (offset >= m_buffer.size ())
      {
        ReportingUScanner scanner (m_byte_stream, *this, m_error_reporter);
        for (State state = State (Leading); !state.done(); state = state (scanner))
          ;;
        m_buffer.push_back (scanner.rune);
      }

    return m_buffer[offset];
  }

  Runet Utf8Scanner::consume ()
  {
    peek ();
    Runet r = m_buffer.front ();
    if (r == '\n')
      {
        ++m_line;
      }
    m_buffer.pop_front ();
    return r;
  }

  void Utf8Scanner::consume (size_t count)
  {
    for (; count != 0; --count)
      {
        consume ();
      }
  }

  Location Utf8Scanner::location () const
  {
    return Location::make (m_byte_stream.path (), m_line);
  }

  // https://golang.org/ref/spec#Characters

  bool is_unicode_char (Runet r)
  {
    return (r < INVALID) && (r < SURROGATE_FIRST || r > SURROGATE_LAST);
  }

  static int comp (const void* key,
                   const void* el)
  {
    Runet k = *(const Runet*)key;
    const RuneTableEntry* e = (const RuneTableEntry*)el;
    if (k < e->first) {
      return -1;
    }
    else if (k > e->last) {
      return 1;
    }
    else {
      return 0;
    }
  }

  extern RuneTableEntry letter_table[];
  extern size_t letter_table_size;

  bool is_unicode_letter (Runet r)
  {
    // Unicode categories Lu, Ll, Lt, Lm, Lo.
    return bsearch (&r, letter_table, letter_table_size, sizeof (RuneTableEntry), comp) != 0;
  }

  extern RuneTableEntry digit_table[];
  extern size_t digit_table_size;

  bool is_unicode_digit (Runet r)
  {
    // Unicode category Nd.
    return bsearch (&r, digit_table, digit_table_size, sizeof (RuneTableEntry), comp) != 0;
  }

  extern RuneTableEntry upper_table[];
  extern size_t upper_table_size;

  bool is_unicode_upper (Runet r)
  {
    // Unicode category Lu.
    return bsearch (&r, upper_table, upper_table_size, sizeof (RuneTableEntry), comp) != 0;
  }

  // https://golang.org/ref/spec#Letters_and_digits

  bool is_letter (Runet r)
  {
    return is_unicode_letter (r) || r == 0x5F;
  }

  bool is_decimal_digit (Runet r)
  {
    return r >= '0' && r <= '9';
  }

  bool is_octal_digit (Runet r)
  {
    return r >= '0' && r <= '7';
  }

  bool is_hex_digit (Runet r)
  {
    return (r >= '0' && r <= '9') ||
      (r >= 'A' && r <= 'F') ||
      (r >= 'a' && r <= 'f');
  }

  void append (std::string& s, Runet r)
  {
    if ((r >= SURROGATE_FIRST && r <= SURROGATE_LAST) ||
        r >= INVALID)
      {
        return;
      }

    if (r <= 0x7F)
      {
        // 1 byte
        s += static_cast<char> (r);
      }
    else if (r <= 0x7FF)
      {
        // 2 bytes
        s += static_cast<char> (((r & (0x1F << 6)) >> 6) | 0xC0);
        s += static_cast<char> (((r & (0x3F << 0)) >> 0) | 0x80);
      }
    else if (r <= 0xFFFF)
      {
        // 3 bytes
        s += static_cast<char> (((r & (0x0F << 12)) >> 12) | 0xE0);
        s += static_cast<char> (((r & (0x3F << 6)) >> 6) | 0x80);
        s += static_cast<char> (((r & (0x3F << 0)) >> 0) | 0x80);
      }
    else if (r <= 0x10FFFF)
      {
        // 4 bytes
        s += static_cast<char> (((r & (0x07 << 18)) >> 18) | 0xF0);
        s += static_cast<char> (((r & (0x3F << 12)) >> 12) | 0x80);
        s += static_cast<char> (((r & (0x3F << 6)) >> 6) | 0x80);
        s += static_cast<char> (((r & (0x3F << 0)) >> 0) | 0x80);

      }
  }

  Runet first_rune (const std::string& s)
  {
    StringByteStream sbs (s);
    UScanner scanner (sbs);
    for (State state = State (Leading); !state.done(); state = state (scanner))
      ;;
    return scanner.rune;
  }

}
