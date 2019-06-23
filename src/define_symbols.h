#ifndef SRC_DEFINE_SYMBOLS_H_
#define SRC_DEFINE_SYMBOLS_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/block.h"
#include "src/error_reporter.h"
#include "src/type.h"

namespace rcgo {

// Defines the package-level symbols of source_file using the symbol
// table of file_block including types, constants, variables,
// functions, and methods.
void DefineSymbols(ast::Node* source_file, Block* file_block,
                   type::Factory* type_factory, ErrorList* error_list);

}  // namespace rcgo

#endif  // SRC_DEFINE_SYMBOLS_H_
