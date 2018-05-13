#ifndef SRC_BYTE_STREAM_H_
#define SRC_BYTE_STREAM_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <deque>
#include <fstream>
#include <string>

namespace rcgo {

typedef unsigned int Byte;

const Byte kEndByte = -1;

struct ByteStreamI {
  virtual ~ByteStreamI() {}
  virtual Byte Peek(size_t offset = 0) = 0;
  virtual Byte Consume() = 0;
  virtual const char* Path() const = 0;
};

struct StringByteStream : public ByteStreamI {
  explicit StringByteStream(const std::string& a_buffer)
      : m_buffer(a_buffer), m_pos(0) {}
  Byte Peek(size_t offset = 0) override;
  Byte Consume() override;
  const char* Path() const override { return NULL; }

 private:
  std::string const m_buffer;
  size_t m_pos;
};

struct FileByteStream : public ByteStreamI {
  explicit FileByteStream(const char* a_path)
      : m_path(a_path), m_stream(m_path.c_str(), std::ios::binary) { }

  Byte Peek(size_t offset = 0) override;
  Byte Consume() override;
  const char* Path() const override { return m_path.c_str(); }

 private:
  std::string const m_path;
  std::ifstream m_stream;
  std::deque<Byte> m_queue;
};

}  // namespace rcgo

#endif  // SRC_BYTE_STREAM_H_
