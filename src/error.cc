// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/error.h"

#include <cstring>

namespace rcgo {

std::ostream& operator<< (std::ostream& out, Error const& error) {
  if (error.location != Location()) {
    out << error.location << ": " << error.message.str();
  } else {
    out << error.message.str();
  }
  return out;
}

Error Perror(const std::string& prefix, int err) {
  char buf[256];
  strerror_r(err, buf, 256);
  Error error;
  error.message << prefix << ": " << buf << std::endl;
  return error;
}

Error NotDeclared(Location const& a_location, std::string const& a_identifier) {
  Error error(a_location);;
  error.message << "error: " << a_identifier << " was not declared" << std::endl;
  return error;
}

}  // namespace rcgo
