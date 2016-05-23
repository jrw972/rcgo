#include "error_reporter.hpp"

#include "location.hpp"
#include "type.hpp"

namespace util
{

ErrorReporter::ErrorReporter (size_t limit, std::ostream& out)
  : limit_ (limit)
  , out_ (out)
{ }

std::ostream&
operator<< (std::ostream& out,
            const Location& loc)
{
  return out << loc.file << ':' << loc.line;
}

std::ostream&
operator<< (std::ostream& out,
            const ErrorCode& ec)
{
  return out << '(' << 'E' << int(ec) << ')';
}

ErrorCode
ErrorReporter::func_expects_count (const Location& loc,
                                   const std::string& func,
                                   size_t expect,
                                   size_t given)
{
  out_ << loc << ": function " << func << " expects " << expect << " arguments but given " << given << " arguments " << Func_Expects_Count << '\n';
  return bump (Func_Expects_Count);
}

ErrorCode
ErrorReporter::func_expects_arg (const Location& loc,
                                 const std::string& func,
                                 size_t idx,
                                 const type::Type* expect,
                                 const type::Type* given)
{
  out_ << loc << ": function " << func << " expects argument " << idx << " to be of type " << expect->to_string () << " but given " << given->to_string () << ' ' << Func_Expects_Arg << '\n';
  return bump (Func_Expects_Arg);
}

ErrorCode
ErrorReporter::cannot_be_applied (const Location& loc,
                                  const std::string& op,
                                  const type::Type* type)
{
  out_ << loc << ": " << op << " cannot be applied to " << type->to_string () << ' ' << Cannot_Be_Applied << '\n';
  return bump (Cannot_Be_Applied);
}

ErrorCode
ErrorReporter::cannot_be_applied (const Location& loc,
                                  const std::string& op,
                                  const type::Type* left,
                                  const type::Type* right)
{
  out_ << loc << ": " << op << " cannot be applied to " << left->to_string () << " and " << right->to_string () << ' ' << Cannot_Be_Applied << '\n';
  return bump (Cannot_Be_Applied);
}

ErrorCode
ErrorReporter::undefined (const Location& loc,
                          const std::string& id)
{
  out_ << loc << ": " << id << " is not defined in this scope " << Undefined << '\n';
  return bump (Undefined);
}

ErrorCode
ErrorReporter::hidden (const Location& loc,
                       const std::string& id)
{
  out_ << loc << ": " << id << " is hidden in this scope " << Hidden << '\n';
  return bump (Hidden);
}

ErrorCode
ErrorReporter::requires_value_or_variable (const Location& loc)
{
  out_ << loc << ": required a value or variable " << Requires_Value_Or_Variable << '\n';
  return bump (Requires_Value_Or_Variable);
}

ErrorCode
ErrorReporter::requires_type (const Location& loc)
{
  out_ << loc << ": required a type " << Requires_Type << '\n';
  return bump (Requires_Type);
}

ErrorCode
ErrorReporter::leaks_pointers (const Location& loc)
{
  out_ << loc << ": operation leaks pointers " << Leaks_Pointers << '\n';
  return bump (Leaks_Pointers);
}

ErrorCode
ErrorReporter::bump (ErrorCode code)
{
  list_.push_back (code);
  if (limit_ > 0 && count () == limit_)
    {
      exit (EXIT_FAILURE);
    }
  return code;
}
}
