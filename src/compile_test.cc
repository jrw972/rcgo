// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/compile.h"

#include <string>

#include "src/catch.hpp"
#include "src/error.h"
#include "src/error_reporter.h"
#include "src/import_location.h"
#include "src/package.h"
#include "src/test_helpers.h"

namespace rcgo {
namespace test {

TEST_CASE("compile()") {
  std::stringstream ss;
  ErrorReporter er0(ss, 0, &abort_handler);
  ErrorReporter er1(ss, 1, &abort_handler);
  Paths paths;

  SECTION("a simple package") {
    Package* p = Compile(UNIT_TEST_DIR "/src/import_alpha", &paths, &er1, ss);
    REQUIRE(p != NULL);
    REQUIRE(p->name() == "import_alpha");
  }

  SECTION("a package that imports another package") {
    Package* p = Compile(UNIT_TEST_DIR "/src/import_beta", &paths, &er0, ss);
    Package::ImportsType::const_iterator pos = p->imports.find("import_alpha");
    REQUIRE(pos != p->imports.end());
  }

  SECTION("a package with conflicting package names") {
    Package* p =
        Compile(UNIT_TEST_DIR "/src/package_mismatch", &paths, &er0, ss);
    REQUIRE(er0.At(0) == std::string(PackageMismatch(
        Location::Make(UNIT_TEST_DIR "/src/package_mismatch/b.rcgo", 1),
        "package_mismatch", "not_package_mismatch")));
  }

#define RECURSIVE_IMPORT_ALPHA UNIT_TEST_DIR \
      "/src/recursive_import_alpha"
#define RECURSIVE_IMPORT_BETA UNIT_TEST_DIR \
      "/src/recursive_import_beta"

  SECTION("a package that recursively imports itself") {
    Package* p = Compile(RECURSIVE_IMPORT_ALPHA, &paths, &er0, ss);
    paths.push_back(RECURSIVE_IMPORT_ALPHA);
    paths.push_back(RECURSIVE_IMPORT_BETA);

    REQUIRE(er0.At(0) ==
            std::string(RecursiveImport(paths, RECURSIVE_IMPORT_ALPHA)));
  }

  SECTION("a package that has no files") {
    Package* p = Compile(UNIT_TEST_DIR "/src/no_files", &paths, &er0, ss);
    REQUIRE(er0.At(0) == std::string(NoFiles(UNIT_TEST_DIR "/src/no_files")));
  }
}

}  // namespace test
}  // namespace rcgo
