#ifndef RCGO_SRC_ERROR_REPORTER_HPP
#define RCGO_SRC_ERROR_REPORTER_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

namespace rcgo {

  struct TerminationHandlerI {
    TerminationHandlerI ();
    virtual ~TerminationHandlerI ();
    virtual void terminate () = 0;
  };

  struct ErrorReporter {
    ErrorReporter (std::ostream& a_out, size_t a_limit, TerminationHandlerI& a_termination_handler);
    void insert (const Error& error);
    const std::string& at (size_t idx) const;
    bool empty () const;
    size_t count () const;

  private:
    std::ostream& m_out;
    size_t const m_limit;
    TerminationHandlerI& m_termination_handler;
    std::vector<std::string> m_errors;
  };

}

#endif // RCGO_SRC_ERROR_REPORTER_HPP
