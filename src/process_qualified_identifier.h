#ifndef SRC_PROCESS_QUALIFIED_IDENTIFIER_H_
#define SRC_PROCESS_QUALIFIED_IDENTIFIER_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/error_reporter.h"
#include "src/symbol_table.h"

namespace rcgo {

// Returns the symbol for package.name or name.
Symbol* ProcessQualifiedIdentifier(ast::Node* ast, const Block& block,
                                   ErrorReporter* error_reporter);

}

#endif  // SRC_PROCESS_QUALIFIED_IDENTIFIER_H_
