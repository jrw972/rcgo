// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/paths.h"

#include <errno.h>

#include <algorithm>

#include "src/error.h"

namespace rcgo {

Paths GetSearchPaths() {
  Paths search_paths;
  const char* c;
  c = getenv("RCGOROOT");
  if (c != NULL) {
    search_paths.push_back(std::string(c) + "/src");
  }
  c = getenv("RCGOPATH");
  if (c != NULL) {
    search_paths.push_back(std::string(c) + "/src");
  }
  return search_paths;
}

bool FindPackageSourceDirectory(
    const ImportLocation& import_location, const Paths& search_paths,
    std::string* package_source_directory_path, DIR** package_source_directory,
    ErrorReporter* error_reporter) {
  // Search for the import path in the search paths.
  *package_source_directory = NULL;
  for (Paths::const_iterator pos = search_paths.begin(),
           limit = search_paths.end();
       pos != limit;
       ++pos) {
    *package_source_directory_path = *pos + "/" + import_location.import_path;
    *package_source_directory = opendir(package_source_directory_path->c_str());
    if (*package_source_directory != NULL) {
      break;
    } else if (ENOENT == errno) {
      // Directory does not exist.
    } else {
      Perror e(std::string("Could not open ") + *package_source_directory_path,
               errno);
      error_reporter->Insert(e);  // NOT_COVERED
      return false;
    }
  }

  // Not found.
  if (*package_source_directory == NULL) {
    error_reporter->Insert(CouldNotFindPackage(import_location));
    return false;
  }

  return true;
}

Paths GetPackageSourceFilePaths(
    const std::string& package_source_directory_path,
    DIR* package_source_directory, ErrorReporter* error_reporter) {
  Paths paths;

  // Build a list of file paths to consider for this package.
  for (;;) {
    struct dirent d;
    struct dirent* dp;
    int r = readdir_r(package_source_directory, &d, &dp);
    if (r != 0) {
      Perror e(std::string("Could not read ") + package_source_directory_path,
               errno);
      error_reporter->Insert(e);  // NOT_COVERED
      continue;  // NOT_COVERED
    }

    if (dp == NULL) {
      break;
    }
    // Look for an rcgo extension.
    std::string const n(d.d_name);
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
