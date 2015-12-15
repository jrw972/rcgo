#ifndef rc_src_builtin_function_hpp
#define rc_src_builtin_function_hpp

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
                   ast::Node* dn,
                   const type::Function* type);

  // Symbol
  virtual void accept (decl::SymbolVisitor& visitor);
  virtual void accept (decl::ConstSymbolVisitor& visitor) const;
  virtual const char* kindString () const
  {
    return "BuiltinFunction";
  }

  virtual const type::Type* type () const
  {
    return type_;
  }
  virtual const type::Signature* signature () const
  {
    return type_->GetSignature ();
  }
  virtual size_t return_size () const
  {
    return type_->GetReturnType ()->Size ();
  }
  virtual size_t receiver_size () const
  {
    return 0;
  }
  virtual size_t locals_size () const
  {
    return 0;
  }
  virtual size_t arguments_size () const
  {
    return type_->GetSignature ()->Size ();
  }

protected:
  const type::Function* const type_;
  runtime::MemoryModel memory_model_;
};

struct Readable : public BuiltinFunction
{
  Readable (ast::Node* dn);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct Read : public BuiltinFunction
{
  Read (ast::Node* dn);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct Writable : public BuiltinFunction
{
  Writable (ast::Node* dn);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct TimerfdCreate : public BuiltinFunction
{
  TimerfdCreate (ast::Node* dn);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct TimerfdSettime : public BuiltinFunction
{
  TimerfdSettime (ast::Node* dn);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct UdpSocket : public BuiltinFunction
{
  UdpSocket (ast::Node* dn);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct Sendto : public BuiltinFunction
{
  Sendto (ast::Node* dn);
  virtual void call (runtime::executor_base_t& exec) const;
};

}

#endif // rc_src_builtin_function_hpp
