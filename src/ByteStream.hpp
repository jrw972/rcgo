#ifndef RCGO_SRC_BYTE_STREAM_HPP
#define RCGO_SRC_BYTE_STREAM_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

#include <fstream>

namespace rcgo {

  typedef unsigned int Byte;

  const Byte END_BYTE = -1;

  struct ByteStream {
    virtual ~ByteStream ();
    virtual Byte peek (size_t offset = 0) = 0;
    virtual Byte consume () = 0;
    virtual const char* path () const = 0;
  };

  struct StringByteStream : public ByteStream {
    StringByteStream (const std::string& buffer);
    ~StringByteStream ();
    virtual Byte peek (size_t offset = 0);
    virtual Byte consume ();
    virtual const char* path () const;

  private:
    std::string const m_buffer;
    size_t m_pos;
  };

  struct FileByteStream : public ByteStream {
    FileByteStream (const char* path);
    ~FileByteStream ();
    virtual Byte peek (size_t offset = 0);
    virtual Byte consume ();
    virtual const char* path () const;

  private:
    std::string const m_path;
    std::ifstream m_stream;
    std::deque<Byte> m_queue;
  };

}

#endif // RCGO_SRC_BYTE_STREAM_HPP
