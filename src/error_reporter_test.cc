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
};

}  // namespace test
}  // namespace rcgo
