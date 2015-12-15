#ifndef rc_src_callable_hpp
#define rc_src_callable_hpp

#include "types.hpp"
#include "type.hpp"
#include "memory_model.hpp"
#include "symbol.hpp"

/*
 * Base class for things that can be called.
 */

class Callable
{
public:
  virtual ~Callable () { }
  virtual void call (executor_base_t& exec) const = 0;
  virtual const Type::Signature* signature () const = 0;
  virtual const Type::Type* type () const = 0;
  virtual size_t return_size () const = 0;
  virtual size_t receiver_size () const = 0;
  virtual size_t arguments_size () const = 0;
  virtual size_t locals_size () const = 0;
  virtual void check_types (ast::Node* args) const;
  virtual void check_references (ast::Node* args) const;
  virtual void check_mutability (ast::Node* args) const;
  virtual void compute_receiver_access (ast::Node* args, ReceiverAccess& receiver_access, bool& flag) const;
};
/*
 * TODO:  I debate whether or not the return symbols should be recorded here.
 */

struct Function : public Callable, public Symbol
{
  // TODO:  Remove duplication with Symbol.
  Function (ast::ast_function_t& node, const Type::Function* type);

  // Callable
  virtual void call (executor_base_t& exec) const;
  virtual const Type::Type* type () const
  {
    return functionType_;
  }

  // Symbol
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const
  {
    return "Function";
  }

  ast::ast_function_t& node;
  MemoryModel memoryModel;

  virtual size_t return_size () const
  {
    return functionType_->GetReturnType ()->Size ();
  }
  virtual size_t receiver_size () const
  {
    return 0;
  }
  virtual size_t arguments_size () const
  {
    return functionType_->GetSignature ()->Size ();
  }
  virtual size_t locals_size () const
  {
    return memoryModel.LocalsSize ();
  }
  virtual const Type::Signature* signature () const
  {
    return functionType_->GetSignature ();
  }

private:
  const Type::Function* functionType_;
};

struct Method : public Callable
{
  Method (ast::ast_method_t* n,
          const std::string& na,
          const Type::Method* method_type_)
    : node (n)
    , name (na)
    , methodType (method_type_)
    , returnSize (method_type_->return_type ()->Size ())
  { }

  virtual void call (executor_base_t& exec) const;
  virtual const Type::Signature* signature () const
  {
    return methodType->signature;
  }

  virtual const Type::Type* type () const
  {
    return methodType;
  }

  virtual size_t return_size () const
  {
    return methodType->return_type ()->Size ();
  }
  virtual size_t receiver_size () const
  {
    return methodType->receiver_type ()->Size ();
  }
  virtual size_t arguments_size () const
  {
    return methodType->signature->Size ();
  }
  virtual size_t locals_size () const
  {
    return memoryModel.LocalsSize ();
  }

  ast::ast_method_t* const node;
  std::string const name;
  const Type::Method * const methodType;
  size_t const returnSize;
  MemoryModel memoryModel;
};

struct Initializer : public Callable
{
  Initializer (ast::ast_initializer_t* n,
               const std::string& na,
               const Type::Method* initializer_type_)
    : node (n)
    , name (na)
    , initializerType (initializer_type_)
    , returnSize (initializer_type_->return_type ()->Size ())
  { }

  virtual void call (executor_base_t& exec) const;
  virtual const Type::Type* type () const
  {
    return initializerType;
  }

  virtual size_t return_size () const
  {
    return returnSize;
  }
  virtual size_t receiver_size () const
  {
    return initializerType->receiver_type ()->Size ();
  }
  virtual size_t arguments_size () const
  {
    return initializerType->signature->Size ();
  }
  virtual size_t locals_size () const
  {
    return memoryModel.LocalsSize ();
  }
  virtual const Type::Signature* signature () const
  {
    return initializerType->signature;
  }

  ast::ast_initializer_t* const node;
  std::string const name;
  const Type::Method * const initializerType;
  size_t const returnSize;
  MemoryModel memoryModel;
};

struct Getter : public Callable
{
  Getter (ast::ast_getter_t* n,
          const std::string& na,
          const Type::Method* getter_type_)
    : node (n)
    , name (na)
    , getterType (getter_type_)
    , returnSize (getter_type_->return_type ()->Size ())
  { }

  virtual void call (executor_base_t& exec) const;
  void call (executor_base_t& exec, const ast::ast_call_expr_t& node, component_t* thisPtr) const;
  virtual const Type::Type* type () const
  {
    return getterType;
  }

  virtual size_t return_size () const
  {
    return getterType->return_type ()->Size ();
  }
  virtual size_t receiver_size () const
  {
    return getterType->receiver_type ()->Size ();
  }
  virtual size_t arguments_size () const
  {
    return getterType->signature->Size ();
  }
  virtual size_t locals_size () const
  {
    return memoryModel.LocalsSize ();
  }
  virtual const Type::Signature* signature () const
  {
    return getterType->signature;
  }

  ast::ast_getter_t* const node;
  std::string const name;
  const Type::Method * const getterType;
  size_t const returnSize;
  ReceiverAccess immutable_phase_access;
  MemoryModel memoryModel;
};

#endif // rc_src_callable_hpp
