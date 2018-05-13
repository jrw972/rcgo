// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/byte_stream.h"

#include "src/catch.hpp"

namespace rcgo {
namespace test {

TEST_CASE("StringByteStream::Peek() on \"\"") {
  StringByteStream seq("");
  Byte b = seq.Peek();
  REQUIRE(b == kEndByte);
}

TEST_CASE("StringByteStream::Consume() on \"\"") {
  StringByteStream seq("");
  seq.Consume();
  Byte b = seq.Peek();
  REQUIRE(b == kEndByte);
}

TEST_CASE("StringByteStream::Peek() on \"hello\"") {
  StringByteStream seq("hello");
  Byte b = seq.Peek();
  REQUIRE(b == 'h');
}

TEST_CASE("StringByteStream::Peek(1) on \"hello\"") {
  StringByteStream seq("hello");
  Byte b = seq.Peek(1);
  REQUIRE(b == 'e');
}

TEST_CASE("StringByteStream::Peek(5) on \"hello\"") {
  StringByteStream seq("hello");
  Byte b = seq.Peek(5);
  REQUIRE(b == kEndByte);
}

TEST_CASE("StringByteStream::Consume() on \"hello\"") {
  StringByteStream seq("hello");
  seq.Consume();
  Byte b = seq.Peek();
  REQUIRE(b == 'e');
}

TEST_CASE("StringByteStream::Consume() all on \"hello\"") {
  StringByteStream seq("hello");
  seq.Consume();
  seq.Consume();
  seq.Consume();
  seq.Consume();
  seq.Consume();
  Byte b = seq.Peek();
  REQUIRE(b == kEndByte);
}

TEST_CASE("StringByteStream::path()") {
  StringByteStream seq("hello");
  REQUIRE(seq.Path() == NULL);
}

TEST_CASE("FileByteStream::Peek() on /dev/null") {
  FileByteStream seq("/dev/null");
  Byte b = seq.Peek();
  REQUIRE(b == kEndByte);
}

TEST_CASE("FileByteStream::Consume() on /dev/null") {
  FileByteStream seq("/dev/null");
  seq.Consume();
  Byte b = seq.Peek();
  REQUIRE(b == kEndByte);
}

#define HELLO_PATH UNIT_TEST_DIR "/hello.txt"

TEST_CASE("FileByteStream::Peek() on " HELLO_PATH) {
  FileByteStream seq(HELLO_PATH);
  Byte b = seq.Peek();
  REQUIRE(b == 'h');
}

TEST_CASE("FileByteStream::Peek(1) on " HELLO_PATH) {
  FileByteStream seq(HELLO_PATH);
  Byte b = seq.Peek(1);
  REQUIRE(b == 'e');
}

TEST_CASE("FileByteStream::Peek(5) on " HELLO_PATH) {
  FileByteStream seq(HELLO_PATH);
  Byte b = seq.Peek(5);
  REQUIRE(b == kEndByte);
}

TEST_CASE("FileByteStream::Consume() on " HELLO_PATH) {
  FileByteStream seq(HELLO_PATH);
  seq.Consume();
  Byte b = seq.Peek();
  REQUIRE(b == 'e');
}

TEST_CASE("FileByteStream::Consume() all on " HELLO_PATH) {
  FileByteStream seq(HELLO_PATH);
  seq.Consume();
  seq.Consume();
  seq.Consume();
  seq.Consume();
  seq.Consume();
  Byte b = seq.Peek();
  REQUIRE(b == kEndByte);
}

TEST_CASE("FileByteStream::Peek() then FileByteStream::Consume() all on "
          HELLO_PATH) {
  FileByteStream seq(HELLO_PATH);
  seq.Peek(1);
  seq.Consume();
  Byte b = seq.Peek();
  REQUIRE(b == 'e');
}

TEST_CASE("FileByteStream::path() on " HELLO_PATH) {
  FileByteStream seq(HELLO_PATH);
  REQUIRE(std::string(seq.Path()) == std::string(HELLO_PATH));
}

}  // namespace test
}  // namespace rcgo
