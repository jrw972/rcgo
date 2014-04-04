/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Location.hpp"
#include <set>

namespace rcgo {

  Location Location::make (const char* path, unsigned int line)
  {
    // Cache all paths.
    static std::set<std::string> strings;

    Location loc;
    loc.path = path;
    loc.line = line;

    if (path != NULL)
      {
        std::string s (path);
        std::pair<std::set<std::string>::iterator, bool> pos = strings.insert (s);
        loc.path = pos.first->c_str ();
      }
    return loc;
  }

  bool Location::operator== (const Location& other) const
  {
    return this->path == other.path && this->line == other.line;
  }

  std::ostream& operator<< (std::ostream& out, const Location& location)
  {
    out << (location.path ? location.path : "undefined") << ':' << location.line;
    return out;
  }

  Location Location::builtin = make ("(builtin)", 1);
}
