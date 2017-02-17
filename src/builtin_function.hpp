#ifndef RC_SRC_BUILTIN_FUNCTION_HPP
#define RC_SRC_BUILTIN_FUNCTION_HPP

#include "types.hpp"
#include "callable.hpp"

namespace runtime
{

// Base class for builtin functions.
struct BuiltinFunction : public decl::FunctionBase
{
  BuiltinFunction (const source::Identifier& identifier,
                   const type::Function* type);
};

struct Readable : public BuiltinFunction
{
  Readable (const source::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct Read : public BuiltinFunction
{
  Read (const source::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct Writable : public BuiltinFunction
{
  Writable (const source::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct ClockGettime : public BuiltinFunction
{
  ClockGettime (const source::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct TimerfdCreate : public BuiltinFunction
{
  TimerfdCreate (const source::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct TimerfdSettime : public BuiltinFunction
{
  TimerfdSettime (const source::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct UdpSocket : public BuiltinFunction
{
  UdpSocket (const source::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct Sendto : public BuiltinFunction
{
  Sendto (const source::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

};

#endif // RC_SRC_BUILTIN_FUNCTION_HPP
