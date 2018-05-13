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
  Paths sp;
  sp.push_back(UNIT_TEST_DIR "/src");
  PackageCache pc;
  ErrorReporter er0(ss, 0, &abort_handler);
  ErrorReporter er1(ss, 1, &abort_handler);
  ImportLocations il;

  SECTION("a simple package") {
    il.push_back(ImportLocation("import_alpha", location));
    Package* p = Compile(&il, sp, &pc, &er1, ss);
    REQUIRE(p->IsChanged());
    REQUIRE(p->import_path == "import_alpha");
    REQUIRE(p->name() == "import_alpha");
    Package* p2 = Compile(&il, sp, &pc, &er1, ss);
    REQUIRE(p2 == p);
  }

  SECTION("a package that imports another package") {
    il.push_back(ImportLocation("import_beta", location));
    Package* p = Compile(&il, sp, &pc, &er0, ss);
    Package* p2 = pc.Find("import_alpha");
    REQUIRE(p2 != NULL);
  }

  SECTION("a package that does not exist") {
    il.push_back(
        ImportLocation("does_not_exist", location));
    Package* p = Compile(&il, sp, &pc, &er0, ss);
    REQUIRE(p->IsError());
    REQUIRE(er0.At(0) == std::string(CouldNotFindPackage(
        ImportLocation("does_not_exist", location))));
  }

  SECTION("a package with conflicting package names") {
    il.push_back(ImportLocation("package_mismatch", location));
    Package* p = Compile(&il, sp, &pc, &er0, ss);
    REQUIRE(p->IsError());
    REQUIRE(er0.At(0) == std::string(PackageMismatch(
        Location::Make(UNIT_TEST_DIR "/src/package_mismatch/b.rcgo", 1),
        "package_mismatch", "not_package_mismatch")));
  }

#define RECURSIVE_IMPORT_ALPHA UNIT_TEST_DIR \
      "/src/recursive_import_alpha/recursive_import_alpha.rcgo"
#define RECURSIVE_IMPORT_BETA UNIT_TEST_DIR \
      "/src/recursive_import_beta/recursive_import_beta.rcgo"

  SECTION("a package that recursively imports itself") {
    il.push_back(
        ImportLocation("recursive_import_alpha", location));
    Package* p = Compile(&il, sp, &pc, &er0, ss);
    {
      const Location loc = Location::Make(RECURSIVE_IMPORT_ALPHA, 3);
      il.push_back(ImportLocation("recursive_import_beta", loc));
    }
    {
      const Location loc = Location::Make(RECURSIVE_IMPORT_BETA, 3);
      il.push_back(ImportLocation("recursive_import_alpha", loc));
    }

    REQUIRE(p->IsError());
    REQUIRE(er0.At(0) == std::string(RecursiveImport(il)));
  }

  SECTION("a package that has no files") {
    il.push_back(ImportLocation("no_files", location));
    Package* p = Compile(&il, sp, &pc, &er0, ss);
    REQUIRE(p->IsError());
    REQUIRE(er0.At(0) == std::string(NoFiles(
        ImportLocation("no_files", location), UNIT_TEST_DIR "/src/no_files")));
  }
}

}  // namespace test
}  // namespace rcgo
