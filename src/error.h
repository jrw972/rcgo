#ifndef SRC_ERROR_H_
#define SRC_ERROR_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "src/location.h"

namespace rcgo {

struct Error {
  Location location;
  std::stringstream message;

  Error() {}
  explicit Error(Location const& a_location): location(a_location) {}

  bool operator==(Error const& other) const {
    if (this->location != other.location) {
      std::cerr << this->location << " != " << other.location << std::endl;
    }
    if (this->message.str() != other.message.str()) {
      std::cerr << this->message.str() << " != " << other.message.str() << std::endl;
    }

    return this->location == other.location &&
        this->message.str() == other.message.str();
  }
};

std::ostream& operator<< (std::ostream& out, Error const& error);

typedef std::vector<Error> ErrorList;

Error Perror(const std::string& prefix, int err);
Error NotDeclared(Location const& location, std::string const& identifier);

}  // namespace rcgo

#endif  // SRC_ERROR_H_
