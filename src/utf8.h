#ifndef SRC_UTF8_H_
#define SRC_UTF8_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <deque>
#include <string>

#include "src/byte_stream.h"
#include "src/error_reporter.h"
#include "src/location.h"

#define INVALID 0x110000
#define SURROGATE_FIRST 0xD800
#define SURROGATE_LAST  0xDFFF

namespace rcgo {

typedef unsigned int Runet;

const Runet kEndRune = -1;

struct RuneTableEntry {
  Runet first;
  Runet last;
};

struct Utf8Scanner {
  Utf8Scanner(ByteStreamI* a_byte_stream, ErrorReporter* a_error_reporter)
      : m_byte_stream(a_byte_stream) , m_error_reporter(a_error_reporter),
        m_line(1) {}

  Runet Peek(size_t a_offset = 0);
  Runet Consume();
  void Consume(size_t a_count);
  Location GetLocation() const;

 private:
  ByteStreamI* m_byte_stream;
  ErrorReporter* m_error_reporter;
  std::deque<Runet> m_buffer;
  unsigned int m_line;
};

bool IsUnicodeChar(Runet r);
bool IsUnicodeLetter(Runet r);
bool IsUnicodeDigit(Runet r);
bool IsUnicodeUpper(Runet r);

bool IsLetter(Runet r);
bool IsDecimalDigit(Runet r);
bool IsOctalDigit(Runet r);
bool IsHexDigit(Runet r);

void Append(std::string* s, Runet r);
Runet FirstRune(const std::string& s);

}  // namespace rcgo

#endif  // SRC_UTF8_H_
