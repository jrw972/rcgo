// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/package.h"

#include "src/ast.h"
#include "src/catch.hpp"

namespace rcgo {
namespace test {

TEST_CASE("Package::Package()") {
  Package p("import_path", YAML::Node());
  REQUIRE(p.path == "import_path");
}

TEST_CASE("Package::name()") {
  Package p("import_path", YAML::Node());
  p.name("identifier");
  REQUIRE(p.name() == "identifier");
}

}  // namespace test
}  // namespace rcgo
