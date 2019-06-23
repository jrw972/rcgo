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

// Compiles the package that is last in import_locations.
Package* Compile(
    const Path& path, Paths* path_list, ErrorList* error_list,
    std::ostream& out);

Error RecursiveImport(const Paths& path_list, const Path& path);
Error NoFiles(std::string const& package_source_directory_path);

}  // namespace rcgo

#endif  // SRC_COMPILE_H_
