#include "error_reporter.hpp"

#include "location.hpp"
#include "type.hpp"
#include "identifier.hpp"

namespace util
{

ErrorReporter::ErrorReporter (size_t limit, std::ostream& out)
  : limit_ (limit)
  , out_ (out)
{ }

std::ostream&
operator<< (std::ostream& out,
            const source::Location& loc)
{
  return out << loc.file () << ':' << loc.line ();
}

std::ostream&
operator<< (std::ostream& out,
            const ErrorCode& ec)
{
  return out << '(' << 'E' << int(ec) << ')';
}

ErrorCode
ErrorReporter::func_expects_count (const source::Location& loc,
                                   const std::string& func,
                                   size_t expect,
                                   size_t given)
{
  out_ << loc << ": function " << func << " expects " << expect << " arguments but given " << given << " arguments " << Func_Expects_Count << '\n';
  return bump (Func_Expects_Count);
}

ErrorCode
ErrorReporter::func_expects_arg (const source::Location& loc,
                                 const std::string& func,
                                 size_t idx,
                                 const type::Type* expect,
                                 const type::Type* given)
{
  out_ << loc << ": function " << func << " expects argument " << idx << " to be of type " << *expect << " but given " << *given << ' ' << Func_Expects_Arg << '\n';
  return bump (Func_Expects_Arg);
}

ErrorCode
ErrorReporter::cannot_be_applied (const source::Location& loc,
                                  const std::string& op,
                                  const type::Type* type)
{
  out_ << loc << ": " << op << " cannot be applied to " << *type << ' ' << Cannot_Be_Applied << '\n';
  return bump (Cannot_Be_Applied);
}

ErrorCode
ErrorReporter::cannot_be_applied (const source::Location& loc,
                                  const std::string& op,
                                  const type::Type* left,
                                  const type::Type* right)
{
  out_ << loc << ": " << op << " cannot be applied to " << *left << " and " << *right << ' ' << Cannot_Be_Applied << '\n';
  return bump (Cannot_Be_Applied);
}

ErrorCode
ErrorReporter::not_declared (const source::Identifier& identifier)
{
  out_ << identifier.location () << ": " << identifier.identifier () << " is not declared in this scope " << Not_Declared << '\n';
  return bump (Not_Declared);
}

ErrorCode
ErrorReporter::hidden_symbol (const source::Location& loc,
                              const std::string& id)
{
  out_ << loc << ": " << id << " is hidden in this scope " << Hidden_Symbol << '\n';
  return bump (Hidden_Symbol);
}

ErrorCode
ErrorReporter::expected_an_rvalue (const source::Location& loc)
{
  out_ << loc << ": expected an rvalue " << Expected_An_Rvalue << '\n';
  return bump (Expected_An_Rvalue);
}

ErrorCode
ErrorReporter::expected_a_type (const source::Location& loc)
{
  out_ << loc << ": expected a type " << Expected_A_Type << '\n';
  return bump (Expected_A_Type);
}

ErrorCode
ErrorReporter::leaks_pointers (const source::Location& loc)
{
  out_ << loc << ": operation leaks pointers " << Leaks_Pointers << '\n';
  return bump (Leaks_Pointers);
}

ErrorCode
ErrorReporter::parameter_is_not_foreign_safe (const source::Location& loc)
{
  out_ << loc << ": parameter is not foreign safe " << Parameter_Is_Not_Foreign_Safe << '\n';
  return bump (Parameter_Is_Not_Foreign_Safe);
}

ErrorCode
ErrorReporter::expected_n_expressions (const source::Location& loc,
                                       size_t expected,
                                       size_t received)
{
  out_ << loc << ": expected " << expected << " expressions but received " << received << ' ' << Expected_N_Expressions << '\n';
  return bump (Expected_N_Expressions);
}

ErrorCode
ErrorReporter::cannot_convert (const source::Location& loc,
                               const type::Type* from,
                               const type::Type* to)
{
  out_ << loc << ": cannot convert expression of type " << *from << " to " << *to << ' ' << Cannot_Convert << '\n';
  return bump (Cannot_Convert);
}

ErrorCode
ErrorReporter::expression_is_not_constant (const source::Location& loc)
{
  out_ << loc << ": expression is not constant " << Expression_Is_Not_Constant << '\n';
  return bump (Expression_Is_Not_Constant);
}

ErrorCode
ErrorReporter::already_declared (const source::Location& loc,
                                 const std::string& id,
                                 const source::Location& previous_loc)
{
  out_ << loc << ": " << id << " was already declared in this scope (previous declaration at " << previous_loc << ") " << Already_Declared << '\n';
  return bump (Already_Declared);
}

ErrorCode
ErrorReporter::defined_recursively (const source::Location& loc,
                                    const std::string& id)
{
  out_ << loc << ": " << id << " is defined recursively " << Defined_Recursively << '\n';
  return bump (Defined_Recursively);
}

ErrorCode
ErrorReporter::non_integer_array_dimension (const source::Location& loc,
    const type::Type* type)
{
  out_ << loc << ": cannot convert value of type " << *type << " to integer for array dimension " << Non_Integer_Array_Dimension << '\n';
  return bump (Non_Integer_Array_Dimension);
}

ErrorCode
ErrorReporter::negative_array_dimension (const source::Location& loc,
    long dim)
{
  out_ << loc << ": array dimension (" << dim << ") is negative " << Negative_Array_Dimension << '\n';
  return bump (Negative_Array_Dimension);
}

ErrorCode
ErrorReporter::not_defined (const source::Location& loc,
                            const std::string& id)
{
  out_ << loc << ": " << id << " is not defined " << Not_Defined << '\n';
  return bump (Not_Defined);
}

ErrorCode
ErrorReporter::expected_a_component (const source::Location& loc)
{
  out_ << loc << ": expected a component " << Expected_A_Component << '\n';
  return bump (Expected_A_Component);
}

ErrorCode
ErrorReporter::expected_a_pointer (const source::Location& loc)
{
  out_ << loc << ": expected a pointer " << Expected_A_Pointer << '\n';
  return bump (Expected_A_Pointer);
}

ErrorCode
ErrorReporter::expected_immutable_indirection_mutability (const source::Location& loc)
{
  out_ << loc << ": expected immutable indirection mutability ($const or $foreign) " << Expected_Immutable_Indirection_Mutability << '\n';
  return bump (Expected_Immutable_Indirection_Mutability);
}

ErrorCode
ErrorReporter::length_exceeds_capacity (const source::Location& loc,
                                        long len,
                                        long cap)
{
  out_ << loc << ": length (" << len << ") exceeds capacity (" << cap << ")" << Length_Exceeds_Capacity << '\n';
  return bump (Length_Exceeds_Capacity);
}

ErrorCode
ErrorReporter::destination_is_not_mutable (const source::Location& loc)
{
  out_ << loc << ": destination is not mutable " << Destination_Is_Not_Mutable << '\n';
  return bump (Destination_Is_Not_Mutable);
}

ErrorCode
ErrorReporter::assignment_leaks_immutable_pointers (const source::Location& loc)
{
  out_ << loc << ": assignment leaks immutable pointers " << Assignment_Leaks_Immutable_Pointers << '\n';
  return bump (Assignment_Leaks_Immutable_Pointers);
}

const ErrorReporter::ListType& ErrorReporter::list () const
{
  return list_;
}

ErrorCode
ErrorReporter::bump (ErrorCode code)
{
  list_.push_back (code);
  if (limit_ > 0 && list_.size () == limit_)
    {
      exit (EXIT_FAILURE);
    }
  return code;
}
}
