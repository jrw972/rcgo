/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ImportLocation.hpp"
#include <sys/types.h>
#include <errno.h>
#include "Error.hpp"
#include "functions.hpp"

namespace rcgo {

  bool
  find_package_source_directory (const ImportLocation& import_location,
                                 const Paths& search_paths,
                                 ErrorReporter& error_reporter,
                                 std::string& package_source_directory_path,
                                 DIR*& package_source_directory)
  {
    // Search for the import path in the search paths.
    package_source_directory = NULL;
    for (Paths::const_iterator pos = search_paths.begin (),
           limit = search_paths.end ();
         pos != limit;
         ++pos) {
      package_source_directory_path = *pos + "/" + import_location.import_path;
      package_source_directory = opendir (package_source_directory_path.c_str ());
      if (package_source_directory != NULL) {
        break;
      } else if (ENOENT == errno) {
        // Directory does not exist.
      } else {
        error_reporter << Perror (std::string ("Could not open ") + package_source_directory_path, errno); // NOT_COVERED
        return false;
      }
    }

    // Not found.
    if (package_source_directory == NULL) {
      error_reporter << CouldNotFindPackage (import_location);
      return false;
    }

    return true;
  }

}
