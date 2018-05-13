// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

namespace Catch {

// using namespace Catch;

struct AutomakeReporter : StreamingReporterBase {
  explicit AutomakeReporter(ReporterConfig const& config)
      : StreamingReporterBase(config) {}

  virtual ~AutomakeReporter() CATCH_OVERRIDE {}

  static std::string getDescription() {
    return "Reports test results in the format of Automake .trs files";
  }

  void assertionStarting(AssertionInfo const&) CATCH_OVERRIDE {}

  bool assertionEnded(AssertionStats const&) CATCH_OVERRIDE {
    return true;
  }

  void testCaseEnded(TestCaseStats const& test_case_stats) CATCH_OVERRIDE {
    // Possible values to emit are PASS, XFAIL, SKIP, FAIL, XPASS and ERROR.
    stream << ":test-result: ";
    if (test_case_stats.totals.assertions.allPassed()) {
      stream << "PASS";
    } else if (test_case_stats.totals.assertions.allOk()) {
      stream << "XFAIL";
    } else {
      stream << "FAIL";
    }

    std::string name = test_case_stats.testInfo.name;

    size_t index = 0;
    for (;;) {
      index = name.find("\n", index);
      if (index == std::string::npos) {
        break;
      }
      name.replace(index, 2, "\\n");
      index += 2;
    }

    stream << ' ' << name << '\n';
    StreamingReporterBase::testCaseEnded(test_case_stats);
  }

  void skipTest(TestCaseInfo const& testInfo) CATCH_OVERRIDE {
    stream << ":test-result: SKIP " << testInfo.name << '\n';
  }
};

REGISTER_REPORTER("automake", AutomakeReporter)

}  // namespace Catch
