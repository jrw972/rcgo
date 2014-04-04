/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "SymbolTable.hpp"
#include "Symbol.hpp"
#include "Package.hpp"

using namespace rcgo;

TEST_CASE ("SymbolTable::find NULL") {
  SymbolTable st;
  REQUIRE (st.find ("not there") == NULL);
}

TEST_CASE ("SymbolTable::find") {
  Symbol* s = new ConstantSymbol ("my symbol", Location::builtin, NULL, NULL);
  SymbolTable st;
  st.insert (s);
  REQUIRE (st.find ("my symbol") == s);
}

TEST_CASE ("SymbolTable::begin and end") {
  Symbol* s = new ConstantSymbol ("my symbol", Location::builtin, NULL, NULL);
  SymbolTable st;
  st.insert (s);
  std::pair<const std::string, Symbol*> p = std::make_pair (s->identifier, s);
  REQUIRE (std::find (st.begin (), st.end (), p) != st.end ());
}

TEST_CASE ("Block::find_local_symbol") {
  Symbol* s = new ConstantSymbol ("my symbol", Location::builtin, NULL, NULL);
  SymbolTable st;
  Block b (st, NULL);
  b.insert (s);
  REQUIRE (b.find_local_symbol ("my symbol") == s);
}

TEST_CASE ("Block::find_global_symbol local") {
  Symbol* s = new ConstantSymbol ("my symbol", Location::builtin, NULL, NULL);
  SymbolTable st;
  Block b (st, NULL);
  b.insert (s);
  REQUIRE (b.find_global_symbol ("my symbol") == s);
}

TEST_CASE ("Block::find_global_symbol global") {
  Symbol* s1 = new ConstantSymbol ("my symbol", Location::builtin, NULL, NULL);
  SymbolTable st1;
  Block b1 (st1, NULL);
  b1.insert (s1);
  SymbolTable st2;
  Block b2 (st2, &b1);
  REQUIRE (b2.find_global_symbol ("my symbol") == s1);
}

TEST_CASE ("Block::find_global_symbol NULL") {
  SymbolTable st;
  Block b (st, NULL);
  REQUIRE (b.find_global_symbol ("my symbol") == NULL);
}

TEST_CASE ("Block::package") {
  Package p ("import path");
  SymbolTable st1;
  Block b1 (st1, NULL, &p);
  SymbolTable st2;
  Block b2 (st2, &b1);
  REQUIRE (b2.package () == &p);
}
