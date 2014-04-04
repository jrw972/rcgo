#ifndef RCGO_SRC_IMPORT_LOCATION_HPP
#define RCGO_SRC_IMPORT_LOCATION_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Location.hpp"
#include <vector>

namespace rcgo {

  struct ImportLocation { // NOT_COVERED
    std::string import_path;
    Location location;

    ImportLocation (const std::string& a_import_path,
                    const Location& a_location);
  };

  typedef std::vector<ImportLocation> ImportLocations;

}

#endif // RCGO_SRC_IMPORT_LOCATION_HPP
