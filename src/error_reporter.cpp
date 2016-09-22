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
  out_ << loc << ": function " << func << " expects argument " << idx << " to be of type " << expect->to_error_string () << " but given " << given->to_error_string () << ' ' << Func_Expects_Arg << '\n';
  return bump (Func_Expects_Arg);
}

ErrorCode
ErrorReporter::cannot_be_applied (const Location& loc,
                                  const std::string& op,
                                  const type::Type* type)
{
  out_ << loc << ": " << op << " cannot be applied to " << type->to_error_string () << ' ' << Cannot_Be_Applied << '\n';
  return bump (Cannot_Be_Applied);
}

ErrorCode
ErrorReporter::cannot_be_applied (const Location& loc,
                                  const std::string& op,
                                  const type::Type* left,
                                  const type::Type* right)
{
  out_ << loc << ": " << op << " cannot be applied to " << left->to_error_string () << " and " << right->to_error_string () << ' ' << Cannot_Be_Applied << '\n';
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
ErrorReporter::hidden_symbol (const Location& loc,
                              const std::string& id)
{
  out_ << loc << ": " << id << " is hidden in this scope " << Hidden_Symbol << '\n';
  return bump (Hidden_Symbol);
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
ErrorReporter::signature_is_not_foreign_safe (const Location& loc)
{
  out_ << loc << ": signature is not foreign safe " << Signature_Is_Not_Foreign_Safe << '\n';
  return bump (Signature_Is_Not_Foreign_Safe);
}

ErrorCode
ErrorReporter::expected_n_expressions (const Location& loc,
                                       size_t expected,
                                       size_t received)
{
  out_ << loc << ": expected " << expected << " expressions but received " << received << ' ' << Expected_N_Expressions << '\n';
  return bump (Expected_N_Expressions);
}

ErrorCode
ErrorReporter::cannot_convert (const Location& loc,
                               const type::Type* from,
                               const type::Type* to)
{
  out_ << loc << ": cannot convert expression of type " << from->to_error_string () << " to " << to->to_error_string () << ' ' << Cannot_Convert << '\n';
  return bump (Cannot_Convert);
}

ErrorCode
ErrorReporter::expression_is_not_constant (const Location& loc)
{
  out_ << loc << ": expression is not constant " << Expression_Is_Not_Constant << '\n';
  return bump (Expression_Is_Not_Constant);
}

ErrorCode
ErrorReporter::already_declared (const Location& loc,
                                 const std::string& id,
                                 const Location& previous_loc)
{
  out_ << loc << ": " << id << " was already declared in this scope (previous declaration at " << previous_loc << ") " << Already_Declared << '\n';
  return bump (Already_Declared);
}

ErrorCode
ErrorReporter::defined_recursively (const Location& loc,
                                    const std::string& id)
{
  out_ << loc << ": " << id << " is defined recursively " << Defined_Recursively << '\n';
  return bump (Defined_Recursively);
}

const ErrorReporter::ListType& ErrorReporter::list () const
{
  return list_;
}

size_t ErrorReporter::count () const
{
  return list_.size ();
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
