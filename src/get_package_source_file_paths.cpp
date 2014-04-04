/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Error.hpp"
#include "functions.hpp"
#include <errno.h>

namespace rcgo {

  Paths
  get_package_source_file_paths (const std::string& package_source_directory_path,
                                 DIR* package_source_directory,
                                 ErrorReporter& error_reporter)
  {
    Paths paths;

    // Build a list of file paths to consider for this package.
    for (;;) {
      struct dirent d;
      struct dirent* dp;
      int r = readdir_r (package_source_directory, &d, &dp);
      if (r != 0) {
        error_reporter << Perror (std::string ("Could not read ") + package_source_directory_path, errno); // NOT_COVERED
        continue; // NOT_COVERED
      }

      if (dp == NULL) {
        break;
      }
      // Look for an rcgo extension.
      std::string const n (d.d_name);
      size_t pos = n.find_last_of ('.');
      if (pos == std::string::npos) {
        continue;
      }
      std::string const extension (n.substr (pos));
      if (extension == ".rcgo") {
        paths.push_back (package_source_directory_path + "/" + n);
      }
    }

    // Sort the paths.  Not strictly necessary but useful for testing since the import process becomes deterministic.
    std::sort (paths.begin (), paths.end ());

    return paths;
  }

}
