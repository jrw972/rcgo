/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "ErrorReporter.hpp"

#include "Symbol.hpp"
#include "Error.hpp"
#include <sstream>

namespace rcgo {

  TerminationHandlerI::TerminationHandlerI ()
  { }

  TerminationHandlerI::~TerminationHandlerI ()
  { }

  ErrorReporter::ErrorReporter (std::ostream& aOut, size_t aLimit, TerminationHandlerI& aTerminationHandler)
    : m_out (aOut)
    , m_limit (aLimit)
    , m_termination_handler (aTerminationHandler)
  { }

  void ErrorReporter::insert (const Error& error)
  {
    std::stringstream str;
    error.print (str);
    m_out << str.str ();
    m_errors.push_back (str.str ());
    if (m_limit != 0 && m_errors.size () >= m_limit)
      {
        m_out.flush ();
        m_termination_handler.terminate ();
      }
  }

  const std::string& ErrorReporter::at (size_t idx) const
  {
    return m_errors.at (idx);
  }

  bool ErrorReporter::empty () const
  {
    return m_errors.empty ();
  }

  size_t ErrorReporter::count () const
  {
    return m_errors.size ();
  }

  ErrorReporter& operator<< (ErrorReporter& error_reporter,
                             const Error& error)
  {
    error_reporter.insert (error);
    return error_reporter;
  }

}
