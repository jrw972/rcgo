#include "BuiltinFunction.hpp"
#include "SymbolVisitor.hpp"
#include "executor_base.hpp"
#include "heap.hpp"
#include "ast.hpp"
#include "runtime.hpp"

#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netdb.h>

using namespace Type;
using namespace ast;

void
BuiltinFunction::accept (SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
BuiltinFunction::accept (ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

Readable::Readable (ast::Node* dn)
  : BuiltinFunction ("readable",
                     dn,
                     new Type::Function (Type::Function::FUNCTION, (new Signature ())
                                         ->Append (new parameter_t (dn, "fd", typed_value_t::make_value (&Type::NamedFileDescriptor, MUTABLE, IMMUTABLE, false), false)),
                                         new parameter_t (dn, ReturnSymbol, typed_value_t::make_value (&Type::NamedBool, MUTABLE, MUTABLE, false), false)))
{ }

void
Readable::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  Node::ConstIterator pos = node.args ()->Begin ();
  runtime::evaluate_expression (exec, memoryModel, *pos);
  ::FileDescriptor* fd = static_cast< ::FileDescriptor*> (exec.stack ().pop_pointer ());

  struct pollfd pfd;
  pfd.fd = fd->fd ();
  pfd.events = POLLIN;

  int r = poll (&pfd, 1, 0);

  if (r < 0)
    {
      error (EXIT_FAILURE, errno, "poll");
    }

  exec.checkedForReadability (fd);

  exec.stack ().push (Bool::ValueType (pfd.revents & POLLIN));
}

Read::Read (ast::Node* dn)
  : BuiltinFunction ("read",
                     dn,
                     new Type::Function (Type::Function::FUNCTION, (new Signature ())
                                         ->Append (new parameter_t (dn, "fd", typed_value_t::make_value (&Type::NamedFileDescriptor, MUTABLE, MUTABLE, false), false))
                                         ->Append (new parameter_t (dn, "buf", typed_value_t::make_value (Type::NamedByte.GetSlice (), MUTABLE, MUTABLE, false), false)),
                                         new parameter_t (dn, ReturnSymbol, typed_value_t::make_value (Int::Instance (), MUTABLE, MUTABLE, false), false)))
{ }

void
Read::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  Node::ConstIterator pos = node.args ()->Begin ();
  runtime::evaluate_expression (exec, memoryModel, *pos++);
  ::FileDescriptor* fd = static_cast< ::FileDescriptor*> (exec.stack ().pop_pointer ());
  runtime::evaluate_expression (exec, memoryModel, *pos++);
  Slice::ValueType slice;
  exec.stack ().pop (slice);
  int r = read (fd->fd (), slice.ptr, slice.length);
  exec.stack ().push (Int::ValueType (r));
}

Writable::Writable (ast::Node* dn)
  : BuiltinFunction ("writable",
                     dn,
                     new Type::Function (Type::Function::FUNCTION, (new Signature ())
                                         ->Append (new parameter_t (dn, "fd", typed_value_t::make_value (&Type::NamedFileDescriptor, MUTABLE, IMMUTABLE, false), false)),
                                         new parameter_t (dn, ReturnSymbol, typed_value_t::make_value (&Type::NamedBool, MUTABLE, MUTABLE, false), false)))
{ }

void
Writable::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  Node::ConstIterator pos = node.args ()->Begin ();
  runtime::evaluate_expression (exec, memoryModel, *pos);
  ::FileDescriptor* fd = static_cast< ::FileDescriptor*> (exec.stack ().pop_pointer ());

  struct pollfd pfd;
  pfd.fd = fd->fd ();
  pfd.events = POLLOUT;

  int r = poll (&pfd, 1, 0);

  if (r < 0)
    {
      error (EXIT_FAILURE, errno, "poll");
    }

  exec.checkedForWritability (fd);

  exec.stack ().push (Bool::ValueType (pfd.revents & POLLOUT));
}

TimerfdCreate::TimerfdCreate (ast::Node* dn)
  : BuiltinFunction ("timerfd_create",
                     dn,
                     new Type::Function (Type::Function::FUNCTION, new Signature (),
                                         new parameter_t (dn, ReturnSymbol, typed_value_t::make_value (&Type::NamedFileDescriptor, MUTABLE, MUTABLE, false), false)))
{ }

void
TimerfdCreate::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  int fd = timerfd_create (CLOCK_MONOTONIC, TFD_NONBLOCK);
  if (fd != -1)
    {
      ::FileDescriptor* thefd = exec.allocateFileDescriptor (fd);
      exec.stack ().push_pointer (thefd);
    }
  else
    {
      exec.stack ().push_pointer (NULL);
    }
}

TimerfdSettime::TimerfdSettime (ast::Node* dn)
  : BuiltinFunction ("timerfd_settime",
                     dn,
                     new Type::Function (Type::Function::FUNCTION, (new Signature ())
                                         ->Append (new parameter_t (dn, "fd", typed_value_t::make_value (&Type::NamedFileDescriptor, MUTABLE, MUTABLE, false), false))
                                         ->Append (new parameter_t (dn, "s", typed_value_t::make_value (&Type::NamedUint64, MUTABLE, MUTABLE, false), false)),
                                         new parameter_t (dn, ReturnSymbol, typed_value_t::make_value (&Type::NamedInt, MUTABLE, MUTABLE, false), false)))
{ }

void
TimerfdSettime::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  Node::ConstIterator pos = node.args ()->Begin ();
  runtime::evaluate_expression (exec, memoryModel, *pos);
  ::FileDescriptor* fd = static_cast< ::FileDescriptor*> (exec.stack ().pop_pointer ());
  ++pos;
  runtime::evaluate_expression (exec, memoryModel, *pos);
  Uint64::ValueType v;
  exec.stack ().pop (v);

  struct itimerspec spec;
  spec.it_interval.tv_sec = v;
  spec.it_interval.tv_nsec = 0;
  spec.it_value.tv_sec = v;
  spec.it_value.tv_nsec = 0;
  int retval = timerfd_settime (fd->fd (), 0, &spec, NULL);

  exec.stack ().push (Int::ValueType (retval));
}

UdpSocket::UdpSocket (ast::Node* dn)
  : BuiltinFunction ("udp_socket",
                     dn,
                     new Type::Function (Type::Function::FUNCTION, new Signature (),
                                         new parameter_t (dn, ReturnSymbol, typed_value_t::make_value (&Type::NamedFileDescriptor, MUTABLE, MUTABLE, false), false)))
{ }

void
UdpSocket::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  int fd = socket (AF_INET, SOCK_DGRAM, 0);
  if (fd == -1)
    {
      exec.stack ().push_pointer (NULL);
      return;
    }

  int s = fcntl (fd, F_SETFL, O_NONBLOCK);
  if (s == -1)
    {
      exec.stack ().push_pointer (NULL);
      return;
    }

  ::FileDescriptor* thefd = exec.allocateFileDescriptor (fd);
  exec.stack ().push_pointer (thefd);
}

Sendto::Sendto (ast::Node* dn)
  : BuiltinFunction ("sendto",
                     dn,
                     new Type::Function (Type::Function::FUNCTION, (new Signature ())
                                         ->Append (new parameter_t (dn, "fd", typed_value_t::make_value (&Type::NamedFileDescriptor, MUTABLE, MUTABLE, false), false))
                                         ->Append (new parameter_t (dn, "host", typed_value_t::make_value (&Type::NamedString, MUTABLE, IMMUTABLE, false), false))
                                         ->Append (new parameter_t (dn, "port", typed_value_t::make_value (&Type::NamedUint16, MUTABLE, IMMUTABLE, false), false))
                                         ->Append (new parameter_t (dn, "buf", typed_value_t::make_value (Type::NamedByte.GetSlice (), MUTABLE, IMMUTABLE, false), false)),
                                         new parameter_t (dn, ReturnSymbol, typed_value_t::make_value (Int::Instance (), MUTABLE, MUTABLE, false), false)))
{ }

void
Sendto::call (executor_base_t& exec, const MemoryModel& memoryModel, const ast_call_expr_t& node) const
{
  ::FileDescriptor* fd;
  StringU::ValueType host_string;
  Uint16::ValueType port_value;
  Slice::ValueType buf_slice;

  Node::ConstIterator pos = node.args ()->Begin ();
  runtime::evaluate_expression (exec, memoryModel, *pos++);
  fd = static_cast< ::FileDescriptor*> (exec.stack ().pop_pointer ());
  runtime::evaluate_expression (exec, memoryModel, *pos++);
  exec.stack ().pop (host_string);
  runtime::evaluate_expression (exec, memoryModel, *pos++);
  exec.stack ().pop (port_value);
  runtime::evaluate_expression (exec, memoryModel, *pos++);
  exec.stack ().pop (buf_slice);

  std::string host (static_cast<const char*> (host_string.ptr), host_string.length);
  std::stringstream port;
  port << port_value;

  struct addrinfo* info;
  struct addrinfo hints;
  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG | AI_NUMERICSERV;
  int r = getaddrinfo (host.c_str (), port.str ().c_str (), &hints, &info);
  if (r != 0)
    {
      unimplemented;
    }

  ssize_t s = sendto (fd->fd (), buf_slice.ptr, buf_slice.length, 0, info->ai_addr, info->ai_addrlen);
  if (s != static_cast<ssize_t> (buf_slice.length))
    {
      unimplemented;
    }

  freeaddrinfo (info);
}
