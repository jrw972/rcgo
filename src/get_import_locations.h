#ifndef SRC_GET_IMPORT_LOCATIONS_H_
#define SRC_GET_IMPORT_LOCATIONS_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/ast.h"
#include "src/import_location.h"

namespace rcgo {

// Extracts and returns the import locations in a set of source files.
ImportLocations GetImportLocations(const ast::SourceFiles& source_files);

}  // namespace rcgo

#endif  // SRC_GET_IMPORT_LOCATIONS_H_
