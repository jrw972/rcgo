#ifndef RCGO_SRC_UTF8_HPP
#define RCGO_SRC_UTF8_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

#include "Location.hpp"

#define INVALID 0x110000
#define SURROGATE_FIRST 0xD800
#define SURROGATE_LAST  0xDFFF

namespace rcgo {

  typedef unsigned int Runet;

  const Runet END_RUNE = -1;

  struct RuneTableEntry {
    Runet first;
    Runet last;
  };

  struct Utf8Scanner {
    Utf8Scanner (ByteStream& a_byte_stream,
                 ErrorReporter& a_error_reporter);
    ~Utf8Scanner ();
    Runet peek (size_t a_offset = 0);
    Runet consume ();
    void consume (size_t a_count);
    Location location () const;

  private:
    ByteStream& m_byte_stream;
    ErrorReporter& m_error_reporter;
    std::deque<Runet> m_buffer;
    unsigned int m_line;
  };

  bool is_unicode_char (Runet r);
  bool is_unicode_letter (Runet r);
  bool is_unicode_digit (Runet r);
  bool is_unicode_upper (Runet r);

  bool is_letter (Runet r);
  bool is_decimal_digit (Runet r);
  bool is_octal_digit (Runet r);
  bool is_hex_digit (Runet r);

  void append (std::string& s, Runet r);
  Runet first_rune (const std::string& s);

}

#endif // RCGO_SRC_UTF8_HPP
