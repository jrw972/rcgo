/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "ErrorReporter.hpp"
#include "Error.hpp"
#include "spies.hpp"
#include "functions.hpp"

using namespace rcgo;

struct Handler : public TerminationHandlerI {
  bool TerminateWasCalled;

  Handler ()
    : TerminateWasCalled (false)
  { }

  void terminate ()
  {
    TerminateWasCalled = true;
  }
};

struct MyError : public Error {
  void print (std::ostream& out) const
  {
    out << "test";
  }
};

TEST_CASE ("ErrorReporter::insert ()") {
  std::stringstream ss;
  Handler h;
  ErrorReporter er (ss, 1, h);
  REQUIRE (er.empty ());
  REQUIRE (er.count () == 0);
  er << MyError ();
  REQUIRE (er.at (0) == "test");
  REQUIRE (!er.empty ());
  REQUIRE (er.count () == 1);
  REQUIRE (h.TerminateWasCalled);
}
