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
                   ast::Node* dn,
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
  Readable (ast::Node* dn);
  virtual void call (executor_base_t& exec) const;
  virtual size_t return_size () const
  {
    unimplemented;
  }
  virtual size_t arguments_size () const
  {
    unimplemented;
  }
  virtual size_t locals_size () const
  {
    unimplemented;
  }
  virtual const Type::Signature* signature () const
  {
    unimplemented;
  }
  virtual void check_types (ast::Node* args) const
  {
    unimplemented;
  }
};

struct Read : public BuiltinFunction
{
  Read (ast::Node* dn);
  virtual void call (executor_base_t& exec) const;
  virtual size_t return_size () const
  {
    unimplemented;
  }
  virtual size_t arguments_size () const
  {
    unimplemented;
  }
  virtual size_t locals_size () const
  {
    unimplemented;
  }
  virtual const Type::Signature* signature () const
  {
    unimplemented;
  }
  virtual void check_types (ast::Node* args) const
  {
    unimplemented;
  }
};

struct Writable : public BuiltinFunction
{
  Writable (ast::Node* dn);
  virtual void call (executor_base_t& exec) const;
  virtual size_t return_size () const
  {
    unimplemented;
  }
  virtual size_t arguments_size () const
  {
    unimplemented;
  }
  virtual size_t locals_size () const
  {
    unimplemented;
  }
  virtual const Type::Signature* signature () const
  {
    unimplemented;
  }
  virtual void check_types (ast::Node* args) const
  {
    unimplemented;
  }
};

struct TimerfdCreate : public BuiltinFunction
{
  TimerfdCreate (ast::Node* dn);
  virtual void call (executor_base_t& exec) const;
  virtual size_t return_size () const
  {
    unimplemented;
  }
  virtual size_t arguments_size () const
  {
    unimplemented;
  }
  virtual size_t locals_size () const
  {
    unimplemented;
  }
  virtual const Type::Signature* signature () const
  {
    unimplemented;
  }
  virtual void check_types (ast::Node* args) const
  {
    unimplemented;
  }
};

struct TimerfdSettime : public BuiltinFunction
{
  TimerfdSettime (ast::Node* dn);
  virtual void call (executor_base_t& exec) const;
  virtual size_t return_size () const
  {
    unimplemented;
  }
  virtual size_t arguments_size () const
  {
    unimplemented;
  }
  virtual size_t locals_size () const
  {
    unimplemented;
  }
  virtual const Type::Signature* signature () const
  {
    unimplemented;
  }
  virtual void check_types (ast::Node* args) const
  {
    unimplemented;
  }
};

struct UdpSocket : public BuiltinFunction
{
  UdpSocket (ast::Node* dn);
  virtual void call (executor_base_t& exec) const;
  virtual size_t return_size () const
  {
    unimplemented;
  }
  virtual size_t arguments_size () const
  {
    unimplemented;
  }
  virtual size_t locals_size () const
  {
    unimplemented;
  }
  virtual const Type::Signature* signature () const
  {
    unimplemented;
  }
  virtual void check_types (ast::Node* args) const
  {
    unimplemented;
  }
};

struct Sendto : public BuiltinFunction
{
  Sendto (ast::Node* dn);
  virtual void call (executor_base_t& exec) const;
  virtual size_t return_size () const
  {
    unimplemented;
  }
  virtual size_t arguments_size () const
  {
    unimplemented;
  }
  virtual size_t locals_size () const
  {
    unimplemented;
  }
  virtual const Type::Signature* signature () const
  {
    unimplemented;
  }
  virtual void check_types (ast::Node* args) const
  {
    unimplemented;
  }
};

#endif /* BuiltinFunction_hpp */
