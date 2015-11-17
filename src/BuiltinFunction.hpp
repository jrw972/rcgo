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
                   Ast::Node* dn,
                   const Type::Function* type)
    : Symbol (id, dn)
    , type_ (type)
    , value_ (typed_value_t::make_ref (typed_value_t (this)))
  { }

  // Symbol
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const
  {
    return "BuiltinFunction";
  }

  virtual const Type::Type* type () const
  {
    return type_;
  }
  virtual typed_value_t value () const
  {
    return value_;
  }

protected:
  const Type::Function* const type_;
  typed_value_t const value_;
};

struct Readable : public BuiltinFunction
{
  Readable (Ast::Node* dn);
  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const Ast::ast_call_expr_t& node) const;
};

struct Read : public BuiltinFunction
{
  Read (Ast::Node* dn);
  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const Ast::ast_call_expr_t& node) const;
};

struct Writable : public BuiltinFunction
{
  Writable (Ast::Node* dn);
  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const Ast::ast_call_expr_t& node) const;
};

struct TimerfdCreate : public BuiltinFunction
{
  TimerfdCreate (Ast::Node* dn);
  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const Ast::ast_call_expr_t& node) const;
};

struct TimerfdSettime : public BuiltinFunction
{
  TimerfdSettime (Ast::Node* dn);
  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const Ast::ast_call_expr_t& node) const;
};

struct UdpSocket : public BuiltinFunction
{
  UdpSocket (Ast::Node* dn);
  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const Ast::ast_call_expr_t& node) const;
};

struct Sendto : public BuiltinFunction
{
  Sendto (Ast::Node* dn);
  virtual void call (executor_base_t& exec, const MemoryModel& memoryModel, const Ast::ast_call_expr_t& node) const;
};

#endif /* BuiltinFunction_hpp */
