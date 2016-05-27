#ifndef RC_SRC_CALLABLE_HPP
#define RC_SRC_CALLABLE_HPP

#include "types.hpp"
#include "type.hpp"
#include "memory_model.hpp"
#include "symbol.hpp"
#include "expression_value.hpp"

namespace decl
{

/*
 * Base class for things that can be called.
 */

struct Callable
{
  virtual ~Callable () { }
  virtual void call (runtime::ExecutorBase& exec) const = 0;
  virtual const decl::ParameterList* parameter_list () const = 0;
  virtual const type::Type* callable_type () const = 0;
  virtual size_t return_size_on_stack () const = 0;
  virtual size_t receiver_size_on_stack () const = 0;
  virtual size_t parameters_size_on_stack () const = 0;
  virtual void check_types (ast::List* args) const;
  virtual void check_references (ast::List* args) const;
  virtual void check_mutability (ast::List* args) const;
  virtual void compute_receiver_access (const semantic::ExpressionValueList& args, ReceiverAccess& receiver_access, bool& flag) const;

  runtime::MemoryModel memory_model;
};

/*
 * TODO:  I debate whether or not the return symbols should be recorded here.
 */

struct Function : public Callable, public decl::Symbol
{
  // TODO:  Remove duplication with Symbol.
  Function (ast::Function* node, const type::Function* type);

  // Callable
  virtual void call (runtime::ExecutorBase& exec) const;
  virtual const type::Type* callable_type () const
  {
    return type;
  }

  // Symbol
  virtual void accept (decl::SymbolVisitor& visitor);
  virtual void accept (decl::ConstSymbolVisitor& visitor) const;
  virtual const type::Type* symbol_type () const
  {
    return type;
  }

  ast::Function* const node;

  virtual size_t return_size_on_stack () const
  {
    return util::align_up (type->GetReturnType ()->size (), arch::stack_alignment ());
  }
  virtual size_t receiver_size_on_stack () const
  {
    return 0;
  }
  virtual size_t parameters_size_on_stack () const;
  virtual const decl::ParameterList* parameter_list () const
  {
    return type->parameter_list;
  }
  ParameterSymbol* return_parameter () const
  {
    return type->GetReturnParameter ();
  }

  const type::Function* type;
};

struct Method : public Callable
{
  Method (ast::Method* n,
          const std::string& na,
          const type::Method* method_type_)
    : node (n)
    , name (na)
    , methodType (method_type_)
  { }

  virtual void call (runtime::ExecutorBase& exec) const;
  virtual const decl::ParameterList* parameter_list () const
  {
    return methodType->parameter_list;
  }
  decl::ParameterSymbol* return_parameter () const;
  decl::ParameterSymbol* receiver_parameter () const
  {
    return methodType->receiver_parameter;
  }

  virtual const type::Type* callable_type () const
  {
    return methodType;
  }

  virtual size_t return_size_on_stack () const
  {
    return util::align_up (methodType->return_type ()->size (), arch::stack_alignment ());
  }
  virtual size_t receiver_size_on_stack () const
  {
    return util::align_up (methodType->receiver_type ()->size (), arch::stack_alignment ());
  }
  virtual size_t parameters_size_on_stack () const;

  ast::Method* const node;
  std::string const name;
  const type::Method * const methodType;
};

struct Initializer : public Callable
{
  Initializer (ast::Initializer* n,
               const std::string& na,
               const type::Method* initializer_type_)
    : node (n)
    , name (na)
    , initializerType (initializer_type_)
  { }

  virtual void call (runtime::ExecutorBase& exec) const;
  virtual const type::Type* callable_type () const
  {
    return initializerType;
  }

  virtual size_t return_size_on_stack () const
  {
    return util::align_up (initializerType->return_type ()->size (), arch::stack_alignment ());
  }
  virtual size_t receiver_size_on_stack () const
  {
    return util::align_up (initializerType->receiver_type ()->size (), arch::stack_alignment ());
  }
  virtual size_t parameters_size_on_stack () const;
  virtual const decl::ParameterList* parameter_list () const
  {
    return initializerType->parameter_list;
  }
  decl::ParameterSymbol* return_parameter () const;
  decl::ParameterSymbol* receiver_parameter () const
  {
    return initializerType->receiver_parameter;
  }
  ast::Initializer* const node;
  std::string const name;
  const type::Method * const initializerType;
};

struct Getter : public Callable
{
  Getter (ast::Getter* n,
          const std::string& na,
          const type::Method* getter_type_)
    : node (n)
    , name (na)
    , getterType (getter_type_)
    , returnSize (getter_type_->return_type ()->size ())
  { }

  virtual void call (runtime::ExecutorBase& exec) const;

  virtual const type::Type* callable_type () const
  {
    return getterType;
  }

  virtual size_t return_size_on_stack () const
  {
    return util::align_up (getterType->return_type ()->size (), arch::stack_alignment ());
  }
  virtual size_t receiver_size_on_stack () const
  {
    return util::align_up (getterType->receiver_type ()->size (), arch::stack_alignment ());
  }
  virtual size_t parameters_size_on_stack () const;
  virtual const decl::ParameterList* parameter_list () const
  {
    return getterType->parameter_list;
  }

  decl::ParameterSymbol* return_parameter () const;
  decl::ParameterSymbol* receiver_parameter () const
  {
    return getterType->receiver_parameter;
  }

  ast::Getter* const node;
  std::string const name;
  const type::Method * const getterType;
  size_t const returnSize;
  ReceiverAccess immutable_phase_access;
};

}

#endif // RC_SRC_CALLABLE_HPP
