// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/insert_symbol.h"

namespace rcgo {

void InsertSymbol(MutableBlock* block, symbol::Symbol* symbol,
                  ErrorReporter* error_reporter) {
  symbol::Symbol* s = block->FindLocalSymbol(symbol->identifier);
  if (s == NULL) {
    block->Insert(symbol);
  } else {
    error_reporter->Insert(DuplicateSymbol(symbol, s));
  }
}

}  // namespace rcgo
