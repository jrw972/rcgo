// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/enter_predeclared_symbols.h"

#include "src/catch.hpp"

namespace rcgo {
namespace test {

TEST_CASE("EnterPredeclaredSymbols()") {
  type::Factory tf;
  SymbolTable st;
  EnterPredeclaredSymbols(&tf, &st);
  // TODO(jrw972):  Test something.
}

}  // namespace test
}  // namespace rcgo
