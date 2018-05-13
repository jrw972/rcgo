#ifndef SRC_COMPILE_H_
#define SRC_COMPILE_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/package.h"
#include "src/paths.h"

namespace rcgo {

// Compiles the package that is the last in import_locations.
Package* Compile(ImportLocations* import_locations, const Paths& search_paths,
                 PackageCache* package_cache, ErrorReporter* error_reporter,
                 std::ostream& out);

}  // namespace rcgo

#endif  // SRC_COMPILE_H_
