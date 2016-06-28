#ifndef RC_SRC_BUILTIN_FUNCTION_HPP
#define RC_SRC_BUILTIN_FUNCTION_HPP

#include "types.hpp"
#include "callable.hpp"

namespace runtime
{

// Base class for builtin functions.
struct BuiltinFunction : public decl::FunctionBase
{
  BuiltinFunction (const std::string& id,
                   const util::Location& loc,
                   const type::Function* type);
};

struct Readable : public BuiltinFunction
{
  Readable (const util::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct Read : public BuiltinFunction
{
  Read (const util::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct Writable : public BuiltinFunction
{
  Writable (const util::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct ClockGettime : public BuiltinFunction
{
  ClockGettime (const util::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct TimerfdCreate : public BuiltinFunction
{
  TimerfdCreate (const util::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct TimerfdSettime : public BuiltinFunction
{
  TimerfdSettime (const util::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct UdpSocket : public BuiltinFunction
{
  UdpSocket (const util::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

struct Sendto : public BuiltinFunction
{
  Sendto (const util::Location& loc);
  virtual void call (ExecutorBase& exec) const;
};

};

#endif // RC_SRC_BUILTIN_FUNCTION_HPP
