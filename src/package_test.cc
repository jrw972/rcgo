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
  Package p("import_path");
  REQUIRE(p.import_path == "import_path");
  REQUIRE(p.IsNew());
}

TEST_CASE("Package::Changed()") {
  Package p("import_path");
  p.Changed();
  REQUIRE(p.IsChanged());
}

TEST_CASE("Package::NotChanged()") {
  Package p("import_path");
  p.NotChanged();
  REQUIRE(p.IsNotChanged());
}

TEST_CASE("Package::Error()") {
  Package p("import_path");
  p.Error();
  REQUIRE(p.IsError());
}

TEST_CASE("PackageCache::Insert()") {
  PackageCache pc;
  REQUIRE(pc.Find("import_path") == NULL);
  Package* p = new Package("import_path");
  pc.Insert(p);
  REQUIRE(pc.Find("import_path") == p);
}

TEST_CASE("Package::name()") {
  Package p("import_path");
  p.name("identifier");
  REQUIRE(p.name() == "identifier");
}

}  // namespace test
}  // namespace rcgo
