// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/block.h"

#include <utility>

#include "src/catch.hpp"
#include "src/package.h"
#include "src/symbol.h"

namespace rcgo {
namespace test {

TEST_CASE("symbol::Table::Find nullptr") {
  symbol::Table st(nullptr);
  REQUIRE(st.Find("not there") == nullptr);
}

TEST_CASE("symbol::Table::Find") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  symbol::Symbol* s = blk.MakeConstant("my symbol", Location::builtin, nullptr);
  st.Insert(s);
  REQUIRE(st.Find("my symbol") == s);
}

TEST_CASE("symbol::Table::Begin and End") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  symbol::Symbol* s = blk.MakeConstant("my symbol", Location::builtin, nullptr);
  st.Insert(s);
  std::pair<const std::string, symbol::Symbol*> p = std::make_pair(s->identifier, s);
  REQUIRE(std::find(st.Begin(), st.End(), p) != st.End());
}

TEST_CASE("Block::FindLocalSymbol") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  symbol::Symbol* s = blk.MakeConstant("my symbol", Location::builtin, nullptr);
  MutableBlock b(&st, nullptr);
  b.Insert(s);
  REQUIRE(b.FindLocalSymbol("my symbol") == s);
}

TEST_CASE("Block::FindGlobalSymbol local") {
  symbol::Table st(nullptr);
  MutableBlock blk(&st, nullptr);
  symbol::Symbol* s = blk.MakeConstant("my symbol", Location::builtin, nullptr);
  MutableBlock b(&st, nullptr);
  b.Insert(s);
  REQUIRE(b.FindGlobalSymbol("my symbol") == s);
}

TEST_CASE("Block::FindGlobalSymbol global") {
  symbol::Table st1(nullptr);
  MutableBlock b1(&st1, nullptr);
  symbol::Symbol* s1 = b1.MakeConstant("my symbol", Location::builtin, nullptr);
  b1.Insert(s1);
  symbol::Table st2(st1.package);
  MutableBlock b2(&st2, &b1);
  REQUIRE(b2.FindGlobalSymbol("my symbol") == s1);
}

TEST_CASE("Block::FindGlobalSymbol nullptr") {
  symbol::Table st(nullptr);
  MutableBlock b(&st, nullptr);
  REQUIRE(b.FindGlobalSymbol("my symbol") == nullptr);
}

TEST_CASE("Block::Block") {
  Package p("import path", YAML::Node());
  symbol::Table st1(&p);
  MutableBlock b1(&st1, nullptr);
  REQUIRE(b1.package() == &p);
}

}  // namespace test
}  // namespace rcgo
