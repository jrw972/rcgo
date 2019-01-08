// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/import_location.h"

#include "src/catch.hpp"
#include "src/test_helpers.h"

namespace rcgo {
namespace test {

TEST_CASE("ImportLocation::ImportLocation ()") {
  ImportLocation il("url", location);
  REQUIRE(il.url == "url");
  REQUIRE(il.location == location);
}

}  // namespace test
}  // namespace rcgo
