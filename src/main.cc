// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>

#include "src/compile.h"
#include "src/error_reporter.h"
#include "src/import_location.h"
#include "src/package.h"
#include "src/paths.h"

namespace rcgo {

int _main(int argc, char** argv) {
  struct ExitHandler : public TerminationHandlerI {
    void Terminate() override { exit(EXIT_FAILURE); }
  } exit_handler;

  Paths search_paths = GetSearchPaths();
  PackageCache package_cache;
  ErrorReporter error_reporter(std::cerr, 0, &exit_handler);
  ImportLocations import_locations;

  for (int idx = 1; idx < argc; ++idx) {
    import_locations.push_back(ImportLocation(argv[idx],
                                              Location::Make("(user)", idx)));
    Compile(&import_locations, search_paths, &package_cache, &error_reporter,
            std::cout);
    import_locations.pop_back();
  }

  return 0;
}

}  // namespace rcgo

int main(int argc, char** argv) {
  return rcgo::_main(argc, argv);
}
