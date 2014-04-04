/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ImportLocation.hpp"
#include "Package.hpp"
#include "ErrorReporter.hpp"
#include "functions.hpp"
#include <iostream>

using namespace rcgo;

int
main (int argc,
      char** argv)
{
  struct ExitHandler : public TerminationHandlerI {
    void terminate ()
    {
      exit (EXIT_FAILURE);
    }
  } th;

  Paths search_paths = get_search_paths ();
  PackageCache package_factory;
  ErrorReporter error_reporter (std::cerr, 0, th);
  ImportLocations import_locations;

  for (int idx = 1; idx < argc; ++idx) {
    import_locations.push_back (ImportLocation (argv[idx], Location::make ("(user)", idx)));
    compile (import_locations, search_paths, package_factory, error_reporter, std::cout);
    import_locations.pop_back ();
  }

  return 0;
}
