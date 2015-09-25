#ifndef BuiltinFunction_hpp
#define BuiltinFunction_hpp

#include "Callable.hpp"
#include "Symbol.hpp"

/*
 * Base class for builtin functions.
 */
struct BuiltinFunction : public Callable, public Symbol
{
  BuiltinFunction (const std::string& id,
                   ast_t* dn,
                   const function_type_t* type)
    : Symbol (id, dn)
    , type_ (type)
    , value_ (typed_value_t::make_ref (typed_value_t (this)))
  { }

  // Symbol
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const { return "BuiltinFunction"; }

  virtual const type_t* type () const { return type_; }
  virtual typed_value_t value () const { return value_; }

protected:
  const function_type_t* const type_;
  typed_value_t const value_;
};

struct Readable : public BuiltinFunction
{
  Readable (ast_t* dn, const type_t* fd_type, const type_t* bool_type);
  virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const;
};

struct Read : public BuiltinFunction
{
  Read (ast_t* dn, const type_t* fd_type, const type_t* uint8_type);
  virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const;
};

struct TimerfdCreate : public BuiltinFunction
{
  TimerfdCreate (ast_t* dn, const type_t* fd_type);
  virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const;
};

struct TimerfdSettime : public BuiltinFunction
{
  TimerfdSettime (ast_t* dn, const type_t* fd_type, const type_t* int_type, const type_t* uint64_type);
  virtual void call (executor_base_t& exec, const ast_call_expr_t& node) const;
};

#endif /* BuiltinFunction_hpp */
