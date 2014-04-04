/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "Location.hpp"
#include <utility>

using namespace rcgo;
using namespace std::rel_ops;

TEST_CASE ("Location::make") {
  Location location = Location::make ("my file", 10);
  REQUIRE (std::string (location.path) == std::string ("my file"));
  REQUIRE (location.line == 10);
}

TEST_CASE ("Location::operator==") {
  Location location1 = Location::make ("my file", 10);
  Location location2 = Location::make ("my file", 10);
  Location location3 = Location::make ("my file", 11);
  REQUIRE (location1 == location2);
  REQUIRE (location1 != location3);
}

TEST_CASE ("Location::operator<<") {
  Location location = Location::make ("my file", 10);
  std::stringstream ss;
  ss << location;
  REQUIRE (ss.str () == "my file:10");
}
