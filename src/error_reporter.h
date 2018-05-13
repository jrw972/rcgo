#ifndef SRC_ERROR_REPORTER_H_
#define SRC_ERROR_REPORTER_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <string>
#include <vector>

#include "src/error.h"

namespace rcgo {

struct TerminationHandlerI {
  virtual ~TerminationHandlerI() {}
  virtual void Terminate() = 0;
};

struct ErrorReporter {
  ErrorReporter(std::ostream& a_out, size_t a_limit,
                TerminationHandlerI* a_termination_handler)
      : m_out(a_out), m_limit(a_limit),
        m_termination_handler(a_termination_handler) {}

  void Insert(const Error& error);
  const std::string& At(size_t idx) const { return m_errors.at(idx); }
  bool Empty() const { return m_errors.empty(); }
  size_t Count() const { return m_errors.size(); }

 private:
  std::ostream& m_out;
  size_t const m_limit;
  TerminationHandlerI* m_termination_handler;
  std::vector<std::string> m_errors;
};

}  // namespace rcgo

#endif  // SRC_ERROR_REPORTER_H_
