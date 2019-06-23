#ifndef SRC_PATHS_H_
#define SRC_PATHS_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <dirent.h>

#include <string>

#include "src/error_reporter.h"
#include "src/import_location.h"

namespace rcgo {

// Returns a list paths to source files in
// package_source_directory_path/package_source_directory.
Paths GetPackageSourceFilePaths(
    const std::string& package_source_directory_path,
    DIR* package_source_directory, ErrorList* error_list);

}  // namespace rcgo

#endif  // SRC_PATHS_H_
