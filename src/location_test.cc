// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/location.h"

#include <string>
#include <utility>

#include "src/catch.hpp"

namespace rcgo {
using std::rel_ops::operator!=;
namespace test {

TEST_CASE("Location::make") {
  Location location = Location::Make("my file", 10);
  REQUIRE(std::string(location.path) == std::string("my file"));
  REQUIRE(location.line == 10);
}

TEST_CASE("Location::operator==") {
  Location location1 = Location::Make("my file", 10);
  Location location2 = Location::Make("my file", 10);
  Location location3 = Location::Make("my file", 11);
  REQUIRE(location1 == location2);
  REQUIRE(location1 != location3);
}

TEST_CASE("Location::operator<<") {
  Location location = Location::Make("my file", 10);
  std::stringstream ss;
  ss << location;
  REQUIRE(ss.str() == "my file:10");
}

}  // namespace test
}  // namespace rcgo
