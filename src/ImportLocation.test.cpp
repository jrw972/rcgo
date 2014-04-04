/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "ImportLocation.hpp"
#include "spies.hpp"

using namespace rcgo;

TEST_CASE ("ImportLocation::ImportLocation ()") {
  ImportLocation il ("path", location);
  REQUIRE (il.import_path == "path");
  REQUIRE (il.location == location);
}
