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
  Readable (const util::Location& loc);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct Read : public BuiltinFunction
{
  Read (const util::Location& loc);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct Writable : public BuiltinFunction
{
  Writable (const util::Location& loc);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct ClockGettime : public BuiltinFunction
{
  ClockGettime (const util::Location& loc);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct TimerfdCreate : public BuiltinFunction
{
  TimerfdCreate (const util::Location& loc);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct TimerfdSettime : public BuiltinFunction
{
  TimerfdSettime (const util::Location& loc);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct UdpSocket : public BuiltinFunction
{
  UdpSocket (const util::Location& loc);
  virtual void call (runtime::executor_base_t& exec) const;
};

struct Sendto : public BuiltinFunction
{
  Sendto (const util::Location& loc);
  virtual void call (runtime::executor_base_t& exec) const;
};

}

#endif // RC_SRC_BUILTIN_FUNCTION_HPP
