// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/symbol_table.h"

#include <utility>

#include "src/catch.hpp"
#include "src/package.h"
#include "src/symbol.h"

namespace rcgo {
namespace test {

TEST_CASE("SymbolTable::Find NULL") {
  SymbolTable st;
  REQUIRE(st.Find("not there") == NULL);
}

TEST_CASE("SymbolTable::Find") {
  Symbol* s = new ConstantSymbol("my symbol", Location::builtin, NULL, NULL);
  SymbolTable st;
  st.Insert(s);
  REQUIRE(st.Find("my symbol") == s);
}

TEST_CASE("SymbolTable::Begin and End") {
  Symbol* s = new ConstantSymbol("my symbol", Location::builtin, NULL, NULL);
  SymbolTable st;
  st.Insert(s);
  std::pair<const std::string, Symbol*> p = std::make_pair(s->identifier, s);
  REQUIRE(std::find(st.Begin(), st.End(), p) != st.End());
}

TEST_CASE("Block::FindLocalSymbol") {
  Symbol* s = new ConstantSymbol("my symbol", Location::builtin, NULL, NULL);
  SymbolTable st;
  Block b(&st, NULL);
  b.Insert(s);
  REQUIRE(b.FindLocalSymbol("my symbol") == s);
}

TEST_CASE("Block::FindGlobalSymbol local") {
  Symbol* s = new ConstantSymbol("my symbol", Location::builtin, NULL, NULL);
  SymbolTable st;
  Block b(&st, NULL);
  b.Insert(s);
  REQUIRE(b.FindGlobalSymbol("my symbol") == s);
}

TEST_CASE("Block::FindGlobalSymbol global") {
  Symbol* s1 = new ConstantSymbol("my symbol", Location::builtin, NULL, NULL);
  SymbolTable st1;
  Block b1(&st1, NULL);
  b1.Insert(s1);
  SymbolTable st2;
  Block b2(&st2, &b1);
  REQUIRE(b2.FindGlobalSymbol("my symbol") == s1);
}

TEST_CASE("Block::FindGlobalSymbol NULL") {
  SymbolTable st;
  Block b(&st, NULL);
  REQUIRE(b.FindGlobalSymbol("my symbol") == NULL);
}

TEST_CASE("Block::GetPackage") {
  Package p("import path");
  SymbolTable st1;
  Block b1(&st1, NULL, &p);
  SymbolTable st2;
  Block b2(&st2, &b1);
  REQUIRE(b2.GetPackage() == &p);
}

}  // namespace test
}  // namespace rcgo
