// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/error_reporter.h"

#include <sstream>

namespace rcgo {

// void ErrorReporter::Insert(const Error& error) {
//   std::stringstream str;
//   error.Print(str);
//   m_out << str.str();
//   m_errors.push_back(str.str());
//   if (m_limit != 0 && m_errors.size() >= m_limit) {
//     m_out.flush();
//     m_termination_handler->Terminate();
//   }
// }

void ReportErrors(std::ostream& out, ErrorList const& error_list) {
  for (auto const& error : error_list) {
    if (error.location != Location()) {
      out << error.location << ": " << error.message.str();
    } else {
      out << error.message.str();
    }
  }
  out.flush();
}

}  // namespace rcgo
