#ifndef RC_SRC_BUILTIN_FUNCTION_HPP
#define RC_SRC_BUILTIN_FUNCTION_HPP

#include "callable.hpp"
#include "symbol.hpp"

namespace decl
{

/*
 * Base class for builtin functions.
 */
struct BuiltinFunction : public decl::Callable, public decl::Symbol
{
  BuiltinFunction (const std::string& id,
                   const util::Location& loc,
                   const type::Function* type);

  // Symbol
  virtual void accept (decl::SymbolVisitor& visitor);
  virtual void accept (decl::ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const
  {
    return "BuiltinFunction";
  }

  const type::Function* const type;

  virtual const type::Type* symbol_type () const
  {
    return type;
  }
  virtual const type::Type* callable_type () const
  {
    return type;
  }
  virtual const decl::ParameterList* signature () const
  {
    return type->parameter_list;
  }
  virtual size_t return_size () const
  {
    return type->GetReturnType ()->Size ();
  }
  virtual size_t receiver_size () const
  {
    return 0;
  }
  virtual size_t locals_size () const
  {
    return 0;
  }
  virtual size_t arguments_size () const;

protected:
  runtime::MemoryModel memory_model_;
};

}

#endif // RC_SRC_BUILTIN_FUNCTION_HPP
