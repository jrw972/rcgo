#ifndef SRC_PATHS_H_
#define SRC_PATHS_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <dirent.h>

#include <string>
#include <vector>

#include "src/error_reporter.h"
#include "src/import_location.h"

namespace rcgo {

typedef std::vector<std::string> Paths;

// Returns the list of paths to search for import declarations.
Paths GetSearchPaths();

// Finds the path to the import path in import_location using
// search_paths.  Returns true on success and populates
// package_source_directory_path and package_source_directory.
bool FindPackageSourceDirectory(const ImportLocation& import_location,
                                const Paths& search_paths,
                                std::string* package_source_directory_path,
                                DIR** package_source_directory,
                                ErrorReporter* error_reporter);

// Returns a list paths to source files in
// package_source_directory_path/package_source_directory.
Paths GetPackageSourceFilePaths(
    const std::string& package_source_directory_path,
    DIR* package_source_directory, ErrorReporter* error_reporter);

}  // namespace rcgo

#endif  // SRC_PATHS_H_
