// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/utf8.h"

#include <cstdlib>

namespace rcgo {

namespace {

struct UScanner {
  explicit UScanner(ByteStreamI* a_byte_stream)
      : byte_stream(a_byte_stream), rune(0), remaining_continuation_bytes(0) {}
  virtual ~UScanner() {}
  virtual void ReportIllegalUtf8Sequence() { /* NOT_COVERED */ }

  ByteStreamI* byte_stream;
  Runet rune;
  size_t remaining_continuation_bytes;
  Runet first_rune;
};

struct ReportingUScanner : public UScanner {
  ReportingUScanner(ByteStreamI* a_byte_stream, Utf8Scanner* a_scanner,
                    ErrorList* a_error_list)
      : UScanner(a_byte_stream), scanner(a_scanner),
        error_list(a_error_list) {}

  void ReportIllegalUtf8Sequence() override {
    error_list->push_back(IllegalUtf8Sequence(scanner->GetLocation()));
  }

  Utf8Scanner* scanner;
  ErrorList* error_list;
};

struct State {
  explicit State(State(*a_func)(UScanner*)) : func(a_func) {}

  State(*func)(UScanner*);  // NOLINT(runtime/explicit)

  bool done() const { return func == NULL; }

  State operator()(UScanner* scanner) {
    return func(scanner);
  }
};

State Leading(UScanner* scanner);
State Continuation(UScanner* scanner);
State ConsumeTrash(UScanner* scanner);

// Expect a leading byte.
State Leading(UScanner* scanner) {
  // Peek at the byte.
  Byte b = scanner->byte_stream->Peek();
  scanner->byte_stream->Consume();
  if (b == kEndByte) {
    scanner->rune = kEndRune;
    return State(NULL);
  }

  if ((b & 0x80) == 0x00) {
    // 1 byte sequence
    scanner->rune = b;
    return State(NULL);
  } else if ((b & 0xE0) == 0xC0) {
    // 2 byte sequence
    scanner->rune = b & 0x1F;
    scanner->remaining_continuation_bytes = 1;
    scanner->first_rune = 0x0080;
    return State(Continuation);
  } else if ((b & 0xF0) == 0xE0) {
    // 3 byte sequence
    scanner->rune = b & 0x0F;
    scanner->remaining_continuation_bytes = 2;
    scanner->first_rune = 0x0800;
    return State(Continuation);
  } else if ((b & 0xF8) == 0xF0) {
    // 4 byte sequence
    scanner->rune = b & 0x07;
    scanner->remaining_continuation_bytes = 3;
    scanner->first_rune = 0x10000;
    return State(Continuation);
  } else {
    // Illegal leading byte
    scanner->ReportIllegalUtf8Sequence();
    return State(ConsumeTrash);
  }
}

// Expect a continuation byte.
State Continuation(UScanner* scanner) {
  // Peek at the character.
  Byte b = scanner->byte_stream->Peek();
  scanner->byte_stream->Consume();
  if (b == kEndByte) {
    scanner->ReportIllegalUtf8Sequence();
    scanner->rune = kEndRune;
    return State(NULL);
  }

  if ((b & 0xC0) == 0x80) {
    // Valid continuation byte.
    scanner->rune =(scanner->rune << 6) |(b & 0x3F);
    --scanner->remaining_continuation_bytes;
    if (scanner->remaining_continuation_bytes != 0) {
      return State(Continuation);
    }

    if (scanner->rune < scanner->first_rune) {
      // Overlong.
      scanner->ReportIllegalUtf8Sequence();
      return State(ConsumeTrash);
    }


    if (scanner->rune >= INVALID) {
      // Invalid.
      scanner->ReportIllegalUtf8Sequence();
      return State(ConsumeTrash);
    }

    if (scanner->rune >= SURROGATE_FIRST && scanner->rune <= SURROGATE_LAST) {
      // Illegal surrogate half.
      scanner->ReportIllegalUtf8Sequence();
      return State(ConsumeTrash);
    }

    return State(NULL);
  } else {
    // Not a valid continuation byte.
    scanner->ReportIllegalUtf8Sequence();
    return State(ConsumeTrash);
  }
}

State ConsumeTrash(UScanner* scanner) {
  // Peek at the byte.
  Byte b = scanner->byte_stream->Peek();
  if (b == kEndByte ||
     (b & 0x80) == 0x00 ||
     (b & 0xE0) == 0xC0 ||
     (b & 0xF0) == 0xE0 ||
     (b & 0xF8) == 0xF0) {
    return State(Leading);
  } else {
    scanner->byte_stream->Consume();
    return State(ConsumeTrash);
  }
}

}  // namespace

Runet Utf8Scanner::Peek(size_t offset) {
  while (offset >= m_buffer.size()) {
    ReportingUScanner scanner(m_byte_stream, this, m_error_list);
    for (State state = State(Leading); !state.done(); state = state(&scanner))
      ;;
    m_buffer.push_back(scanner.rune);
  }

  return m_buffer[offset];
}

Runet Utf8Scanner::Consume() {
  Peek();
  Runet r = m_buffer.front();
  if (r == '\n') {
    ++m_line;
  }
  m_buffer.pop_front();
  return r;
}

void Utf8Scanner::Consume(size_t count) {
  for (; count != 0; --count) {
    Consume();
  }
}

Location Utf8Scanner::GetLocation() const {
  return Location(m_byte_stream->Path(), m_line);
}

// https://golang.org/ref/spec#Characters

bool IsUnicodeChar(Runet r) {
  return(r < INVALID) &&(r < SURROGATE_FIRST || r > SURROGATE_LAST);
}

static int comp(const void* key, const void* el) {
  Runet k = *(const Runet*)key;
  const RuneTableEntry* e =(const RuneTableEntry*)el;
  if (k < e->first) {
    return -1;
  } else if (k > e->last) {
    return 1;
  } else {
    return 0;
  }
}

extern RuneTableEntry letter_table[];
extern size_t letter_table_size;

bool IsUnicodeLetter(Runet r) {
  // Unicode categories Lu, Ll, Lt, Lm, Lo.
  return bsearch(&r, letter_table, letter_table_size, sizeof(RuneTableEntry),
                 comp) != 0;
}

extern RuneTableEntry digit_table[];
extern size_t digit_table_size;

bool IsUnicodeDigit(Runet r) {
  // Unicode category Nd.
  return bsearch(&r, digit_table, digit_table_size, sizeof(RuneTableEntry),
                 comp) != 0;
}

extern RuneTableEntry upper_table[];
extern size_t upper_table_size;

bool IsUnicodeUpper(Runet r) {
  // Unicode category Lu.
  return bsearch(&r, upper_table, upper_table_size, sizeof(RuneTableEntry),
                 comp) != 0;
}

// https://golang.org/ref/spec#Letters_and_digits

bool IsLetter(Runet r) {
  return IsUnicodeLetter(r) || r == 0x5F;
}

bool IsDecimalDigit(Runet r) {
  return r >= '0' && r <= '9';
}

bool IsOctalDigit(Runet r) {
  return r >= '0' && r <= '7';
}

bool IsHexDigit(Runet r) {
  return(r >= '0' && r <= '9') ||
     (r >= 'A' && r <= 'F') ||
     (r >= 'a' && r <= 'f');
}

void Append(std::string* s, Runet r) {
  if ((r >= SURROGATE_FIRST && r <= SURROGATE_LAST) ||
      r >= INVALID) {
    return;
  }

  if (r <= 0x7F) {
    // 1 byte
    *s += static_cast<char>(r);
  } else if (r <= 0x7FF) {
    // 2 bytes
    *s += static_cast<char>(((r &(0x1F << 6)) >> 6) | 0xC0);
    *s += static_cast<char>(((r &(0x3F << 0)) >> 0) | 0x80);
  } else if (r <= 0xFFFF) {
    // 3 bytes
    *s += static_cast<char>(((r &(0x0F << 12)) >> 12) | 0xE0);
    *s += static_cast<char>(((r &(0x3F << 6)) >> 6) | 0x80);
    *s += static_cast<char>(((r &(0x3F << 0)) >> 0) | 0x80);
  } else if (r <= 0x10FFFF) {
    // 4 bytes
    *s += static_cast<char>(((r &(0x07 << 18)) >> 18) | 0xF0);
    *s += static_cast<char>(((r &(0x3F << 12)) >> 12) | 0x80);
    *s += static_cast<char>(((r &(0x3F << 6)) >> 6) | 0x80);
    *s += static_cast<char>(((r &(0x3F << 0)) >> 0) | 0x80);
  }
}

Runet FirstRune(const std::string& s) {
  StringByteStream sbs("", s);
  UScanner scanner(&sbs);
  for (State state = State(Leading); !state.done(); state = state(&scanner))
    ;;
  return scanner.rune;
}

Error IllegalUtf8Sequence(const Location& a_location) {
  Error error(a_location);
  error.message << "error: illegal UTF-8 sequence" << std::endl;
  return error;
}

}  // namespace rcgo
