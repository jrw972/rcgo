/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "ImportLocation.hpp"
#include "Package.hpp"
#include "ErrorReporter.hpp"
#include "Error.hpp"
#include "spies.hpp"
#include "functions.hpp"

using namespace rcgo;

TEST_CASE ("compile ()") {
  std::stringstream ss;
  Paths sp;
  sp.push_back (UNIT_TEST_DIR "/src");
  PackageCache pc;
  ErrorReporter er0 (ss, 0, abortHandler);
  ErrorReporter er1 (ss, 1, abortHandler);
  ImportLocations import_locations;

  SECTION ("a simple package") {
    import_locations.push_back (ImportLocation ("import_alpha", location));
    Package* p = compile (import_locations, sp, pc, er1, ss);
    REQUIRE (p->is_changed ());
    REQUIRE (p->import_path == "import_alpha");
    REQUIRE (p->name () == "import_alpha");
    Package* p2 = compile (import_locations, sp, pc, er1, ss);
    REQUIRE (p2 == p);
  }

  SECTION ("a package that imports another package") {
    import_locations.push_back (ImportLocation ("import_beta", location));
    Package* p = compile (import_locations, sp, pc, er0, ss);
    Package* p2 = pc.find ("import_alpha");
    REQUIRE (p2 != NULL);
  }

  SECTION ("a package that does not exist") {
    import_locations.push_back (ImportLocation ("does_not_exist", location));
    Package* p = compile (import_locations, sp, pc, er0, ss);
    REQUIRE (p->is_error ());
    REQUIRE (er0.at (0) == std::string (CouldNotFindPackage (ImportLocation ("does_not_exist", location))));
  }

  SECTION ("a package with conflicting package names") {
    import_locations.push_back (ImportLocation ("package_mismatch", location));
    Package* p = compile (import_locations, sp, pc, er0, ss);
    REQUIRE (p->is_error ());
    REQUIRE (er0.at (0) == std::string (PackageMismatch (Location::make (UNIT_TEST_DIR "/src/package_mismatch/b.rcgo", 1), "package_mismatch", "not_package_mismatch")));
  }

  SECTION ("a package that recursively imports itself") {
    import_locations.push_back (ImportLocation ("recursive_import_alpha", location));
    Package* p = compile (import_locations, sp, pc, er0, ss);

    import_locations.push_back (ImportLocation ("recursive_import_beta", Location::make (UNIT_TEST_DIR "/src/recursive_import_alpha/recursive_import_alpha.rcgo", 3)));
    import_locations.push_back (ImportLocation ("recursive_import_alpha", Location::make (UNIT_TEST_DIR "/src/recursive_import_beta/recursive_import_beta.rcgo", 3)));

    REQUIRE (p->is_error ());
    REQUIRE (er0.at (0) == std::string (RecursiveImport (import_locations)));
  }

  SECTION ("a package that has no files") {
    import_locations.push_back (ImportLocation ("no_files", location));
    Package* p = compile (import_locations, sp, pc, er0, ss);
    REQUIRE (p->is_error ());
    REQUIRE (er0.at (0) == std::string (NoFiles (ImportLocation ("no_files", location), UNIT_TEST_DIR "/src/no_files")));
  }
}
