// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/error_reporter.h"

#include "src/catch.hpp"
#include "src/error.h"
#include "src/test_helpers.h"

namespace rcgo {
namespace test {

struct Handler : public TerminationHandlerI {
  bool terminate_was_called;

  Handler() : terminate_was_called(false) {}

  void Terminate() override { terminate_was_called = true; }
};

struct MyError : public Error {
  void Print(std::ostream& out) const override { out << "test"; }
};

TEST_CASE("ErrorReporter::insert ()") {
  std::stringstream ss;
  Handler h;
  ErrorReporter er(ss, 1, &h);
  REQUIRE(er.Empty());
  REQUIRE(er.Count() == 0);
  er.Insert(MyError());
  REQUIRE(er.At(0) == "test");
  REQUIRE(!er.Empty());
  REQUIRE(er.Count() == 1);
  REQUIRE(h.terminate_was_called);
}

}  // namespace test
}  // namespace rcgo
