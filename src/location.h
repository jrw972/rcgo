#ifndef SRC_LOCATION_H_
#define SRC_LOCATION_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ostream>
#include <vector>

namespace rcgo {

struct Location {
  const char* path;
  unsigned int line;

  Location();
  Location(const char* a_path, unsigned int a_line);
  static Location builtin;  // TODO(jrw972): Remove this.

  bool operator== (const Location& other) const {
    return this->path == other.path && this->line == other.line;
  }

  bool operator!= (const Location& other) const {
    return !(*this == other);
  }
};

typedef std::vector<Location> LocationList;

inline std::ostream& operator<< (std::ostream& out, const Location& location) {
  return out << (location.path ? location.path : "undefined") << ':'
             << location.line;
}

}  // namespace rcgo

#endif  // SRC_LOCATION_H_
