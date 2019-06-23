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
#include "src/get_and_check_package_name.h"
#include "src/import_location.h"
#include "src/package.h"
#include "src/test_helpers.h"

namespace rcgo {
namespace test {

TEST_CASE("compile()") {
  std::stringstream ss;
  ErrorList el;
  Paths paths;

  SECTION("a simple package") {
    Package* p = Compile(UNIT_TEST_DIR "/src/import_alpha", &paths, &el, ss);
    REQUIRE(p != NULL);
    REQUIRE(p->name() == "import_alpha");
    REQUIRE(el.empty());
  }

  SECTION("a package that imports another package") {
    Package* p = Compile(UNIT_TEST_DIR "/src/import_beta", &paths, &el, ss);
    Package::ImportsType::const_iterator pos = p->imports.find("import_alpha");
    REQUIRE(pos != p->imports.end());
    REQUIRE(el.empty());
  }

  SECTION("a package with conflicting package names") {
    Package* p =
        Compile(UNIT_TEST_DIR "/src/package_mismatch", &paths, &el, ss);
    REQUIRE(el.size() == 1);
    REQUIRE(el.at(0) == PackageMismatch(
        Location(UNIT_TEST_DIR "/src/package_mismatch/b.rcgo", 1),
        "package_mismatch", "not_package_mismatch"));
  }

#define RECURSIVE_IMPORT_ALPHA UNIT_TEST_DIR \
      "/src/recursive_import_alpha"
#define RECURSIVE_IMPORT_BETA UNIT_TEST_DIR \
      "/src/recursive_import_beta"

  SECTION("a package that recursively imports itself") {
    Package* p = Compile(RECURSIVE_IMPORT_ALPHA, &paths, &el, ss);
    paths.push_back(RECURSIVE_IMPORT_ALPHA);
    paths.push_back(RECURSIVE_IMPORT_BETA);
    REQUIRE(el.size() == 1);
    REQUIRE(el.at(0) == RecursiveImport(paths, RECURSIVE_IMPORT_ALPHA));
  }

  SECTION("a package that has no files") {
    Package* p = Compile(UNIT_TEST_DIR "/src/no_files", &paths, &el, ss);
    REQUIRE(el.size() == 1);
    REQUIRE(el.at(0) == NoFiles(UNIT_TEST_DIR "/src/no_files"));
  }
}

}  // namespace test
}  // namespace rcgo
