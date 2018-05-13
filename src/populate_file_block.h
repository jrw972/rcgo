#ifndef SRC_POPULATE_FILE_BLOCK_H_
#define SRC_POPULATE_FILE_BLOCK_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/ast.h"
#include "src/error_reporter.h"
#include "src/package.h"
#include "src/symbol_table.h"

namespace rcgo {

// Populates file_block by processing the import declarations in source_file.
void PopulateFileBlock(ast::Node* source_file, Block* file_block,
                       PackageCache* package_cache,
                       ErrorReporter* error_reporter);

}  // namespace rcgo

#endif  // SRC_POPULATE_FILE_BLOCK_H_
