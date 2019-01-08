// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/paths.h"

#include <errno.h>

#include <algorithm>
#include <string>

#include "src/error.h"

namespace rcgo {

Paths GetPackageSourceFilePaths(
    const std::string& package_source_directory_path,
    DIR* package_source_directory, ErrorReporter* error_reporter) {
  Paths paths;

  // Build a list of file paths to consider for this package.
  for (;;) {
    errno = 0;
    struct dirent* dp = readdir(package_source_directory);
    if (dp == nullptr) {
      if (errno != 0) {
        Perror e(std::string("Could not read ") + package_source_directory_path,
                 errno);
        error_reporter->Insert(e);  // NOT_COVERED
        continue;  // NOT_COVERED
      }
      break;
    }
    // Look for an rcgo extension.
    std::string const n(dp->d_name);
    size_t pos = n.find_last_of('.');
    if (pos == std::string::npos) {
      continue;
    }
    std::string const extension(n.substr(pos));
    if (extension == ".rcgo") {
      paths.push_back(package_source_directory_path + "/" + n);
    }
  }

  // Sort the paths.  Not strictly necessary but useful for testing
  // since the import process becomes deterministic.
  std::sort(paths.begin(), paths.end());

  return paths;
}

}  // namespace rcgo
