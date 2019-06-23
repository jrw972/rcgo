#ifndef SRC_GET_AND_CHECK_PACKAGE_NAME_H_
#define SRC_GET_AND_CHECK_PACKAGE_NAME_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <string>

#include "src/ast.h"
#include "src/error_reporter.h"

namespace rcgo {

// Checks that every source file has the same package name and returns
// a package name.
std::string GetAndCheckPackageName(const ast::SourceFiles& source_files,
                                   ErrorList* error_list);

Error PackageMismatch(
      const Location& a_location, const std::string& a_expected_package,
      const std::string& a_package);

}  // namespace rcgo

#endif  // SRC_GET_AND_CHECK_PACKAGE_NAME_H_
