/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "ByteStream.hpp"

using namespace rcgo;

TEST_CASE ("StringByteStream::Peek () on \"\"") {
  StringByteStream seq ("");
  Byte b = seq.peek ();
  REQUIRE (b == END_BYTE);
}

TEST_CASE ("StringByteStream::Consume () on \"\"") {
  StringByteStream seq ("");
  seq.consume ();
  Byte b = seq.peek ();
  REQUIRE (b == END_BYTE);
}

TEST_CASE ("StringByteStream::Peek () on \"hello\"") {
  StringByteStream seq ("hello");
  Byte b = seq.peek ();
  REQUIRE (b == 'h');
}

TEST_CASE ("StringByteStream::Peek (1) on \"hello\"") {
  StringByteStream seq ("hello");
  Byte b = seq.peek (1);
  REQUIRE (b == 'e');
}

TEST_CASE ("StringByteStream::Peek (5) on \"hello\"") {
  StringByteStream seq ("hello");
  Byte b = seq.peek (5);
  REQUIRE (b == END_BYTE);
}

TEST_CASE ("StringByteStream::Consume () on \"hello\"") {
  StringByteStream seq ("hello");
  seq.consume ();
  Byte b = seq.peek ();
  REQUIRE (b == 'e');
}

TEST_CASE ("StringByteStream::Consume () all on \"hello\"") {
  StringByteStream seq ("hello");
  seq.consume ();
  seq.consume ();
  seq.consume ();
  seq.consume ();
  seq.consume ();
  Byte b = seq.peek ();
  REQUIRE (b == END_BYTE);
}

TEST_CASE ("StringByteStream::path ()") {
  StringByteStream seq ("hello");
  REQUIRE (seq.path () == NULL);
}

TEST_CASE ("FileByteStream::Peek () on /dev/null") {
  FileByteStream seq ("/dev/null");
  Byte b = seq.peek ();
  REQUIRE (b == END_BYTE);
}

TEST_CASE ("FileByteStream::Consume () on /dev/null") {
  FileByteStream seq ("/dev/null");
  seq.consume ();
  Byte b = seq.peek ();
  REQUIRE (b == END_BYTE);
}

#define HELLO_PATH UNIT_TEST_DIR "/hello.txt"

TEST_CASE ("FileByteStream::Peek () on " HELLO_PATH) {
  FileByteStream seq (HELLO_PATH);
  Byte b = seq.peek ();
  REQUIRE (b == 'h');
}

TEST_CASE ("FileByteStream::Peek (1) on " HELLO_PATH) {
  FileByteStream seq (HELLO_PATH);
  Byte b = seq.peek (1);
  REQUIRE (b == 'e');
}

TEST_CASE ("FileByteStream::Peek (5) on " HELLO_PATH) {
  FileByteStream seq (HELLO_PATH);
  Byte b = seq.peek (5);
  REQUIRE (b == END_BYTE);
}

TEST_CASE ("FileByteStream::Consume () on " HELLO_PATH) {
  FileByteStream seq (HELLO_PATH);
  seq.consume ();
  Byte b = seq.peek ();
  REQUIRE (b == 'e');
}

TEST_CASE ("FileByteStream::Consume () all on " HELLO_PATH) {
  FileByteStream seq (HELLO_PATH);
  seq.consume ();
  seq.consume ();
  seq.consume ();
  seq.consume ();
  seq.consume ();
  Byte b = seq.peek ();
  REQUIRE (b == END_BYTE);
}

TEST_CASE ("FileByteStream::Peek () then FileByteStream::Consume () all on " HELLO_PATH) {
  FileByteStream seq (HELLO_PATH);
  seq.peek (1);
  seq.consume ();
  Byte b = seq.peek ();
  REQUIRE (b == 'e');
}

TEST_CASE ("FileByteStream::path () on " HELLO_PATH) {
  FileByteStream seq (HELLO_PATH);
  REQUIRE (std::string (seq.path ()) == std::string (HELLO_PATH));
}
