#ifndef RC_SRC_BUILTIN_FUNCTION_HPP
#define RC_SRC_BUILTIN_FUNCTION_HPP

#include "callable.hpp"
#include "symbol.hpp"

namespace decl
{

// Base class for builtin functions.
struct BuiltinFunction : public decl::Callable, public decl::Symbol
{
  BuiltinFunction (const std::string& id,
                   const util::Location& loc,
                   const type::Function* type);

  // Symbol
  virtual void accept (decl::SymbolVisitor& visitor);
  virtual void accept (decl::ConstSymbolVisitor& visitor) const;
  virtual const type::Type* symbol_type () const;

  // Callable
  virtual const type::Type* callable_type () const;
  virtual const decl::ParameterList* parameter_list () const;
  virtual size_t return_size_on_stack () const;
  virtual size_t receiver_size_on_stack () const;
  virtual size_t parameters_size_on_stack () const;

  const type::Function* const type;
};

}

#endif // RC_SRC_BUILTIN_FUNCTION_HPP
