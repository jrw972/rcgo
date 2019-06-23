#ifndef SRC_INSERT_SYMBOL_H_
#define SRC_INSERT_SYMBOL_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/block.h"
#include "src/error_reporter.h"

namespace rcgo {

// Inserts the symbol into the symbol table of block (if not already declared).
void InsertSymbol(MutableBlock* block, symbol::Symbol* symbol,
                  ErrorList* error_list);

}  // namespace rcgo

#endif  // SRC_INSERT_SYMBOL_H_
