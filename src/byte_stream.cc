// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/byte_stream.h"

namespace rcgo {

Byte StringByteStream::Peek(size_t offset) {
  const size_t p = m_pos + offset;
  if (p < m_buffer.size()) return m_buffer[p] & 0xFF;
  return kEndByte;
}

Byte StringByteStream::Consume() {
  if (m_pos < m_buffer.size()) return m_buffer[m_pos++];
  return kEndByte;
}

Byte FileByteStream::Peek(size_t offset) {
  while (m_queue.size() <= offset) {
    int x = m_stream.get();
    if (x != EOF) {
      m_queue.push_back(x);
    } else {
      m_queue.push_back(kEndByte);
    }
  }

  return m_queue[offset];
}

Byte FileByteStream::Consume() {
  Byte b;
  if (!m_queue.empty()) {
    b = m_queue.front();
    m_queue.pop_front();
  } else {
    int x = m_stream.get();
    if (x != EOF) {
      b = x;
    } else {
      b = kEndByte;
    }
  }
  return b;
}

}  // namespace rcgo
