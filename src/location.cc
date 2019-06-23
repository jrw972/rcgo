// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/location.h"

#include <set>
#include <string>
#include <utility>

namespace rcgo {

Location::Location()
    : path(nullptr), line(0) {}

Location::Location(const char* path, unsigned int line) {
  // Cache all paths.
  static std::set<std::string> strings;

  this->path = path;
  this->line = line;

  if (path != nullptr) {
    std::string s(path);
    std::pair<std::set<std::string>::iterator, bool> pos = strings.insert(s);
    this->path = pos.first->c_str();
  }
}

Location Location::builtin("(builtin)", 1);

}  // namespace rcgo
