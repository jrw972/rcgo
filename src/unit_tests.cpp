/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

namespace {
  using namespace Catch;

  struct AutomakeReporter : StreamingReporterBase {
    AutomakeReporter( ReporterConfig const& _config )
      :   StreamingReporterBase( _config )
    {}

    virtual ~AutomakeReporter() CATCH_OVERRIDE {}
    static std::string getDescription() {
      return "Reports test results in the format of Automake .trs files";
    }

    virtual void assertionStarting( AssertionInfo const& ) CATCH_OVERRIDE { }

    virtual bool assertionEnded( AssertionStats const& _assertionStats ) CATCH_OVERRIDE { return true; }

    virtual void testCaseEnded( TestCaseStats const& _testCaseStats ) CATCH_OVERRIDE {
      // Possible values to emit are PASS, XFAIL, SKIP, FAIL, XPASS and ERROR.
      stream << ":test-result: ";
      if (_testCaseStats.totals.assertions.allPassed ())
        {
          stream << "PASS";
        }
      else if (_testCaseStats.totals.assertions.allOk ())
        {
          stream << "XFAIL";
        }
      else
        {
          stream << "FAIL";
        }

      std::string name = _testCaseStats.testInfo.name;

      size_t index = 0;
      for (;;)
        {
          index = name.find ("\n", index);
          if (index == std::string::npos)
            {
              break;
            }
          name.replace (index, 2, "\\n");
          index += 2;
        }

      stream << ' ' << name << '\n';
      StreamingReporterBase::testCaseEnded( _testCaseStats );
    }

    virtual void skipTest( TestCaseInfo const& testInfo ) CATCH_OVERRIDE {
      stream << ":test-result: SKIP " << testInfo.name << '\n';
    }

  };

  REGISTER_REPORTER( "automake", AutomakeReporter)
}
