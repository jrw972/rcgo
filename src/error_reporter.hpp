#ifndef RC_SRC_ERROR_REPORTER_HPP
#define RC_SRC_ERROR_REPORTER_HPP

#include <iostream>

#include "types.hpp"

namespace util
{

struct ErrorReporter
{
  ErrorReporter (int limit = 0, std::ostream& out = std::cerr);

  int func_expects_count (const Location& loc,
                          const std::string& func,
                          size_t expect,
                          size_t given);
  int func_expects_arg (const Location& loc,
                        const std::string& func,
                        size_t idx,
                        const type::Type* expect,
                        const type::Type* given);
  int cannot_be_applied (const Location& loc,
                         const std::string& op,
                         const type::Type* type);

  int count () const
  {
    return count_;
  }
private:
  void bump ();
  int const limit_;
  std::ostream& out_;
  int count_;
};

}

#endif // RC_SRC_ERROR_REPORTER_HPP
