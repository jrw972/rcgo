#ifndef Callable_hpp
#define Callable_hpp

#include "types.hpp"
#include "type.hpp"
#include "memory_model.hpp"
#include "Symbol.hpp"

/*
 * Base class for things that can be called.
 */

class Callable {
public:
  virtual ~Callable () { }
  virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const = 0;
  virtual const type_t* type () const = 0;
};

/*
 * TODO:  I debate whether or not the return symbols should be recorded here.
 */

struct Function : public Callable, public Symbol
{
  // TODO:  Remove duplication with Symbol.
  Function (ast_function_t& node);

  // Callable
  virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const;
  virtual const type_t* type () const { return functionType_; }

  // Symbol
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const { return "Function"; }

  ast_function_t& node;
  memory_model_t memoryModel;

  void set (const function_type_t* functionType,
            const Symbol* returnSymbol);

  const Symbol* returnSymbol () const { return returnSymbol_; }
  typed_value_t value () const { return value_; }

private:
  const function_type_t* functionType_;
  const Symbol* returnSymbol_;
  size_t returnSize_;
  typed_value_t value_;
};

struct Method : public Callable
{
  Method (ast_method_t* n,
          const std::string& na,
          const method_type_t* method_type_,
          const Symbol* return_symbol_)
    : node (n)
    , name (na)
    , methodType (method_type_)
    , returnSymbol (return_symbol_)
    , returnSize (method_type_->return_type ()->size ())
  { }

  virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const;
  virtual const type_t* type () const { return methodType; }

  ast_method_t* const node;
  std::string const name;
  const method_type_t * const methodType;
  const Symbol* const returnSymbol;
  size_t const returnSize;
  memory_model_t memoryModel;
};

struct Initializer : public Callable
{
  Initializer (ast_initializer_t* n,
               const std::string& na,
               const initializer_type_t* initializer_type_)
    : node (n)
    , name (na)
    , initializerType (initializer_type_)
  { }

  virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const;
  void call (executor_base_t& exec, const ast_call_expr_t& node, component_t* thisPtr) const;
  virtual const type_t* type () const { return initializerType; }

  ast_initializer_t* const node;
  std::string const name;
  const initializer_type_t * const initializerType;
  memory_model_t memoryModel;
};

struct Getter : public Callable
{
  Getter (ast_getter_t* n,
          const std::string& na,
          const getter_type_t* getter_type_,
          const Symbol* return_symbol_)
    : node (n)
    , name (na)
    , getterType (getter_type_)
    , returnSymbol (return_symbol_)
    , returnSize (getter_type_->return_type ()->size ())
  { }

  virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const;
  void call (executor_base_t& exec, const ast_call_expr_t& node, component_t* thisPtr) const;
  virtual const type_t* type () const { return getterType; }

  ast_getter_t* const node;
  std::string const name;
  const getter_type_t * const getterType;
  const Symbol* const returnSymbol;
  size_t const returnSize;
  memory_model_t memoryModel;
};

#endif /* Callable_hpp */
