/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ByteStream.hpp"

namespace rcgo {

  ByteStream::~ByteStream ()
  { }

  StringByteStream::StringByteStream (const std::string& aBuffer)
    : m_buffer (aBuffer)
    , m_pos (0)
  { }

  StringByteStream::~StringByteStream ()
  { }

  Byte StringByteStream::peek (size_t offset)
  {
    size_t p = m_pos + offset;
    if (p < m_buffer.size ())
      {
        return m_buffer[p] & 0xFF;
      }

    return END_BYTE;
  }

  Byte StringByteStream::consume ()
  {
    if (m_pos < m_buffer.size ())
      {
        return m_buffer[m_pos++];
      }

    return END_BYTE;
  }

  const char* StringByteStream::path () const
  {
    return NULL;
  }

  FileByteStream::FileByteStream (const char* aPath)
    : m_path (aPath)
    , m_stream (m_path.c_str (), std::ios::binary)
  { }

  FileByteStream::~FileByteStream ()
  { }

  Byte FileByteStream::peek (size_t offset)
  {
    while (m_queue.size () <= offset)
      {
        int x = m_stream.get ();
        if (x != EOF)
          {
            m_queue.push_back (x);
          }
        else
          {
            m_queue.push_back (END_BYTE);
          }
      }

    return m_queue[offset];
  }

  Byte FileByteStream::consume ()
  {
    Byte b;
    if (!m_queue.empty ())
      {
        b = m_queue.front ();
        m_queue.pop_front ();
      }
    else
      {
        int x = m_stream.get ();
        if (x != EOF)
          {
            b = x;
          }
        else
          {
            b = END_BYTE;
          }
      }
    return b;
  }

  const char* FileByteStream::path () const
  {
    return m_path.c_str ();
  }

}
