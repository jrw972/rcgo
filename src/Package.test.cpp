/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "Package.hpp"
#include "Ast.hpp"

using namespace rcgo;

TEST_CASE ("Package::Package ()") {
  Package p ("import_path");
  REQUIRE (p.import_path == "import_path");
  REQUIRE (p.is_new ());
}

TEST_CASE ("Package::changed ()") {
  Package p ("import_path");
  p.changed ();
  REQUIRE (p.is_changed ());
}

TEST_CASE ("Package::not_changed ()") {
  Package p ("import_path");
  p.not_changed ();
  REQUIRE (p.is_not_changed ());
}

TEST_CASE ("Package::error ()") {
  Package p ("import_path");
  p.error ();
  REQUIRE (p.is_error ());
}

TEST_CASE ("PackageCache::insert ()") {
  PackageCache pc;
  REQUIRE (pc.find ("import_path") == NULL);
  Package* p = new Package ("import_path");
  pc.insert (p);
  REQUIRE (pc.find ("import_path") == p);
}

TEST_CASE ("Package::identifier ()") {
  Package p ("import_path");
  p.name ("identifier");
  REQUIRE (p.name () == "identifier");
}
