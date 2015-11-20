#ifndef Callable_hpp
#define Callable_hpp

#include "types.hpp"
#include "Type.hpp"
#include "MemoryModel.hpp"
#include "Symbol.hpp"

/*
 * Base class for things that can be called.
 */

class Callable
{
public:
  virtual ~Callable () { }
  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const ast::ast_call_expr_t& node) const = 0;
  virtual const Type::Type* type () const = 0;
};

/*
 * TODO:  I debate whether or not the return symbols should be recorded here.
 */

struct Function : public Callable, public Symbol
{
  // TODO:  Remove duplication with Symbol.
  Function (ast::ast_function_t& node);

  // Callable
  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const ast::ast_call_expr_t& node) const;
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

  void set (const Type::Function* functionType,
            const Symbol* returnSymbol);

  const Symbol* returnSymbol () const
  {
    return returnSymbol_;
  }
  typed_value_t value () const
  {
    return value_;
  }

private:
  const Type::Function* functionType_;
  const Symbol* returnSymbol_;
  size_t returnSize_;
  typed_value_t value_;
};

struct Method : public Callable
{
  Method (ast::ast_method_t* n,
          const std::string& na,
          const Type::Method* method_type_,
          const Symbol* return_symbol_)
    : node (n)
    , name (na)
    , methodType (method_type_)
    , returnSymbol (return_symbol_)
    , returnSize (method_type_->return_type ()->Size ())
  { }

  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const ast::ast_call_expr_t& node) const;
  virtual const Type::Type* type () const
  {
    return methodType;
  }

  ast::ast_method_t* const node;
  std::string const name;
  const Type::Method * const methodType;
  const Symbol* const returnSymbol;
  size_t const returnSize;
  MemoryModel memoryModel;
};

struct Initializer : public Callable
{
  Initializer (ast::ast_initializer_t* n,
               const std::string& na,
               const Type::Method* initializer_type_,
               const Symbol* return_symbol_)
    : node (n)
    , name (na)
    , initializerType (initializer_type_)
    , returnSymbol (return_symbol_)
    , returnSize (initializer_type_->return_type ()->Size ())
  { }

  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const ast::ast_call_expr_t& node) const;
  void call (executor_base_t& exec, component_t* thisPtr, const ast::Node* args) const;
  virtual const Type::Type* type () const
  {
    return initializerType;
  }

  ast::ast_initializer_t* const node;
  std::string const name;
  const Type::Method * const initializerType;
  const Symbol* const returnSymbol;
  size_t const returnSize;
  MemoryModel memoryModel;
};

struct Getter : public Callable
{
  Getter (ast::ast_getter_t* n,
          const std::string& na,
          const Type::Method* getter_type_,
          const Symbol* return_symbol_)
    : node (n)
    , name (na)
    , getterType (getter_type_)
    , returnSymbol (return_symbol_)
    , returnSize (getter_type_->return_type ()->Size ())
  { }

  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const ast::ast_call_expr_t& node) const;
  void call (executor_base_t& exec, const ast::ast_call_expr_t& node, component_t* thisPtr) const;
  virtual const Type::Type* type () const
  {
    return getterType;
  }

  ast::ast_getter_t* const node;
  std::string const name;
  const Type::Method * const getterType;
  const Symbol* const returnSymbol;
  size_t const returnSize;
  ReceiverAccess immutable_phase_access;
  MemoryModel memoryModel;
};

#endif /* Callable_hpp */
