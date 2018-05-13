#ifndef SRC_IMPORT_LOCATION_H_
#define SRC_IMPORT_LOCATION_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <vector>
#include <string>

#include "src/location.h"

namespace rcgo {

struct ImportLocation {  // NOT_COVERED
  std::string import_path;
  Location location;

  ImportLocation(const std::string& a_import_path, const Location& a_location)
      : import_path(a_import_path) , location(a_location) {}
};

typedef std::vector<ImportLocation> ImportLocations;
}  // namespace rcgo

#endif  // SRC_IMPORT_LOCATION_H_
