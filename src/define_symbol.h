#ifndef SRC_DEFINE_SYMBOL_H_
#define SRC_DEFINE_SYMBOL_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/block.h"
#include "src/error_reporter.h"
#include "src/symbol.h"
#include "src/type.h"

namespace rcgo {

// Process the definition of symbol.  Returns true if the symbol is
// defined after processing.
bool DefineSymbol(symbol::Symbol* symbol, Block* file_block,
                  type::Factory* type_factory, ErrorReporter* error_reporter);

}  // namespace rcgo

#endif  // SRC_DEFINE_SYMBOL_H_
