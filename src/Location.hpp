#ifndef RCGO_SRC_LOCATION_HPP
#define RCGO_SRC_LOCATION_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ostream>

namespace rcgo {

  struct Location {
    const char* path;
    unsigned int line;

    static Location make (const char* a_path, unsigned int a_line);
    static Location builtin;

    bool operator== (const Location& other) const;
  };

  std::ostream& operator<< (std::ostream& a_out, const Location& a_location);

}

#endif // RCGO_SRC_UTF8_HPP
