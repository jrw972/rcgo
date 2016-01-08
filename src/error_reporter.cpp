#include "error_reporter.hpp"

#include "location.hpp"
#include "type.hpp"

namespace util
{

ErrorReporter::ErrorReporter (int limit, std::ostream& out)
  : limit_ (limit)
  , out_ (out)
  , count_ (0)
{ }

enum ErrorCode
{
  Func_Expects_Count = 1,
  Func_Expects_Arg = 2,
  Cannot_Be_Applied = 3,
};

std::ostream&
operator<< (std::ostream& out,
            const Location& loc)
{
  return out << loc.File << ':' << loc.Line;
}

std::ostream&
operator<< (std::ostream& out,
            const ErrorCode& ec)
{
  return out << '(' << 'E' << int(ec) << ')';
}

int
ErrorReporter::func_expects_count (const Location& loc,
                                   const std::string& func,
                                   size_t expect,
                                   size_t given)
{
  out_ << loc << ": function " << func << " expects " << expect << " arguments but given " << given << " arguments " << Func_Expects_Count << '\n';
  bump ();
  return Func_Expects_Count;
}

int
ErrorReporter::func_expects_arg (const Location& loc,
                                 const std::string& func,
                                 size_t idx,
                                 const type::Type* expect,
                                 const type::Type* given)
{
  out_ << loc << ": function " << func << " expects argument " << idx << " to be of type " << expect->ToString () << " but given " << given->ToString () << ' ' << Func_Expects_Arg << '\n';
  bump ();
  return Func_Expects_Arg;
}

int
ErrorReporter::cannot_be_applied (const Location& loc,
                                  const std::string& op,
                                  const type::Type* type)
{
  out_ << loc << ": " << op << " cannot be applied to an expression of type " << type->ToString () << Cannot_Be_Applied << '\n';
  bump ();
  return Cannot_Be_Applied;
}

void
ErrorReporter::bump ()
{
  ++count_;
  if (limit_ > 0 && count_ == limit_)
    {
      exit (EXIT_FAILURE);
    }
}
}
