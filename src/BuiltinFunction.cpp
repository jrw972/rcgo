#include "BuiltinFunction.hpp"
#include "SymbolVisitor.hpp"
#include "executor_base.hpp"
#include "heap.hpp"
#include "ast.hpp"
#include "runtime.hpp"

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

Readable::Readable (ast_t* dn, const type_t* fd_type, const type_t* bool_type)
  : BuiltinFunction ("readable",
                     dn,
                     new function_type_t ((new signature_type_t ())
                                          ->append (new parameter_t (dn, "fd", typed_value_t::make_value (fd_type, typed_value_t::STACK, MUTABLE, IMMUTABLE), false)),
                                          new parameter_t (dn, "0return", typed_value_t::make_value (bool_type, typed_value_t::STACK, MUTABLE, MUTABLE), false)))
{ }

void
Readable::call (executor_base_t& exec, const ast_call_expr_t& node) const
{
  ast_t::const_iterator pos = node.args ()->begin ();
  runtime::evaluate_expr (exec, *pos);
  FileDescriptor* fd = static_cast<FileDescriptor*> (stack_frame_pop_pointer (exec.stack ()));

  struct pollfd pfd;
  pfd.fd = fd->fd ();
  pfd.events = POLLIN;

  int r = poll (&pfd, 1, 0);

  if (r < 0)
    {
      error (EXIT_FAILURE, errno, "poll");
    }

  exec.checkedForReadability (fd);

  stack_frame_push_tv (exec.stack (), typed_value_t (bool_type_t::instance (), pfd.revents & POLLIN));
}

Read::Read (ast_t* dn, const type_t* fd_type, const type_t* uint8_type)
  : BuiltinFunction ("read",
                     dn,
                     new function_type_t ((new signature_type_t ())
                                          ->append (new parameter_t (dn, "fd", typed_value_t::make_value (fd_type, typed_value_t::STACK, MUTABLE, MUTABLE), false))
                                          ->append (new parameter_t (dn, "buf", typed_value_t::make_value (uint8_type->getSliceType (), typed_value_t::STACK, MUTABLE, MUTABLE), false)),
                                          new parameter_t (dn, "0return", typed_value_t::make_value (int_type_t::instance (), typed_value_t::STACK, MUTABLE, MUTABLE), false)))
{ }

void
Read::call (executor_base_t& exec, const ast_call_expr_t& node) const
{
  typed_value_t buf_tv = type_->getParameter ("buf")->value;

  ast_t::const_iterator pos = node.args ()->begin ();
  runtime::evaluate_expr (exec, *pos++);
  FileDescriptor* fd = static_cast<FileDescriptor*> (stack_frame_pop_pointer (exec.stack ()));
  runtime::evaluate_expr (exec, *pos++);
  stack_frame_pop_tv (exec.stack (), buf_tv);
  slice_type_t::ValueType slice = buf_tv.slice_value ();
  int r = read (fd->fd (), slice.ptr, slice.length);
  typed_value_t retval (int_type_t::instance (), r);
  stack_frame_push_tv (exec.stack (), retval);
}

Writable::Writable (ast_t* dn, const type_t* fd_type, const type_t* bool_type)
  : BuiltinFunction ("writable",
                     dn,
                     new function_type_t ((new signature_type_t ())
                                          ->append (new parameter_t (dn, "fd", typed_value_t::make_value (fd_type, typed_value_t::STACK, MUTABLE, IMMUTABLE), false)),
                                          new parameter_t (dn, "0return", typed_value_t::make_value (bool_type, typed_value_t::STACK, MUTABLE, MUTABLE), false)))
{ }

void
Writable::call (executor_base_t& exec, const ast_call_expr_t& node) const
{
  ast_t::const_iterator pos = node.args ()->begin ();
  runtime::evaluate_expr (exec, *pos);
  FileDescriptor* fd = static_cast<FileDescriptor*> (stack_frame_pop_pointer (exec.stack ()));

  struct pollfd pfd;
  pfd.fd = fd->fd ();
  pfd.events = POLLOUT;

  int r = poll (&pfd, 1, 0);

  if (r < 0)
    {
      error (EXIT_FAILURE, errno, "poll");
    }

  exec.checkedForWritability (fd);

  stack_frame_push_tv (exec.stack (), typed_value_t (bool_type_t::instance (), pfd.revents & POLLOUT));
}

TimerfdCreate::TimerfdCreate (ast_t* dn, const type_t* fd_type)
  : BuiltinFunction ("timerfd_create",
                     dn,
                     new function_type_t (new signature_type_t (),
                                          new parameter_t (dn, "0return", typed_value_t::make_value (fd_type, typed_value_t::STACK, MUTABLE, MUTABLE), false)))
{ }

void
TimerfdCreate::call (executor_base_t& exec, const ast_call_expr_t& node) const
{
  int fd = timerfd_create (CLOCK_MONOTONIC, TFD_NONBLOCK);
  if (fd != -1)
    {
      FileDescriptor* thefd = exec.allocateFileDescriptor (fd);
      stack_frame_push_pointer (exec.stack (), thefd);
    }
  else
    {
      stack_frame_push_pointer (exec.stack (), NULL);
    }
}

TimerfdSettime::TimerfdSettime (ast_t* dn, const type_t* fd_type, const type_t* int_type, const type_t* uint64_type)
  : BuiltinFunction ("timerfd_settime",
                     dn,
                     new function_type_t ((new signature_type_t ())
                                          ->append (new parameter_t (dn, "fd", typed_value_t::make_value (fd_type, typed_value_t::STACK, MUTABLE, MUTABLE), false))
                                          ->append (new parameter_t (dn, "s", typed_value_t::make_value (uint64_type, typed_value_t::STACK, MUTABLE, MUTABLE), false)),
                                          new parameter_t (dn, "0return", typed_value_t::make_value (int_type, typed_value_t::STACK, MUTABLE, MUTABLE), false)))
{ }

void
TimerfdSettime::call (executor_base_t& exec, const ast_call_expr_t& node) const
{
  ast_t::const_iterator pos = node.args ()->begin ();
  runtime::evaluate_expr (exec, *pos);
  FileDescriptor* fd = static_cast<FileDescriptor*> (stack_frame_pop_pointer (exec.stack ()));
  ++pos;
  runtime::evaluate_expr (exec, *pos);
  typed_value_t tv (uint64_type_t::instance (), 0);
  stack_frame_pop_tv (exec.stack (), tv);

  struct itimerspec spec;
  spec.it_interval.tv_sec = tv.value.ref (*uint64_type_t::instance ());
  spec.it_interval.tv_nsec = 0;
  spec.it_value.tv_sec = tv.value.ref (*uint64_type_t::instance ());
  spec.it_value.tv_nsec = 0;
  int retval = timerfd_settime (fd->fd (), 0, &spec, NULL);

  stack_frame_push_tv (exec.stack (), typed_value_t (int_type_t::instance (), retval));
}

UdpSocket::UdpSocket (ast_t* dn, const type_t* fd_type)
  : BuiltinFunction ("udp_socket",
                     dn,
                     new function_type_t (new signature_type_t (),
                                          new parameter_t (dn, "0return", typed_value_t::make_value (fd_type, typed_value_t::STACK, MUTABLE, MUTABLE), false)))
{ }

void
UdpSocket::call (executor_base_t& exec, const ast_call_expr_t& node) const
{
  int fd = socket (AF_INET, SOCK_DGRAM, 0);
  if (fd == -1)
    {
      stack_frame_push_pointer (exec.stack (), NULL);
      return;
    }

  int s = fcntl (fd, F_SETFL, O_NONBLOCK);
  if (s == -1)
    {
      stack_frame_push_pointer (exec.stack (), NULL);
      return;
    }

  FileDescriptor* thefd = exec.allocateFileDescriptor (fd);
  stack_frame_push_pointer (exec.stack (), thefd);
}

Sendto::Sendto (ast_t* dn, const type_t* fd_type, const type_t* uint8_type, const type_t* uint16_type)
  : BuiltinFunction ("sendto",
                     dn,
                     new function_type_t ((new signature_type_t ())
                                          ->append (new parameter_t (dn, "fd", typed_value_t::make_value (fd_type, typed_value_t::STACK, MUTABLE, MUTABLE), false))
                                          ->append (new parameter_t (dn, "host", typed_value_t::make_value (uint8_type->getSliceType (), typed_value_t::STACK, MUTABLE, IMMUTABLE), false))
                                          ->append (new parameter_t (dn, "port", typed_value_t::make_value (uint16_type, typed_value_t::STACK, MUTABLE, IMMUTABLE), false))
                                          ->append (new parameter_t (dn, "buf", typed_value_t::make_value (uint8_type->getSliceType (), typed_value_t::STACK, MUTABLE, IMMUTABLE), false)),
                                          new parameter_t (dn, "0return", typed_value_t::make_value (int_type_t::instance (), typed_value_t::STACK, MUTABLE, MUTABLE), false)))
{ }

void
Sendto::call (executor_base_t& exec, const ast_call_expr_t& node) const
{
  typed_value_t host_tv = type_->getParameter ("host")->value;
  typed_value_t port_tv = type_->getParameter ("port")->value;
  typed_value_t buf_tv = type_->getParameter ("buf")->value;

  ast_t::const_iterator pos = node.args ()->begin ();
  runtime::evaluate_expr (exec, *pos++);
  FileDescriptor* fd = static_cast<FileDescriptor*> (stack_frame_pop_pointer (exec.stack ()));
  runtime::evaluate_expr (exec, *pos++);
  stack_frame_pop_tv (exec.stack (), host_tv);
  runtime::evaluate_expr (exec, *pos++);
  stack_frame_pop_tv (exec.stack (), port_tv);
  runtime::evaluate_expr (exec, *pos++);
  stack_frame_pop_tv (exec.stack (), buf_tv);

  slice_type_t::ValueType host_slice = host_tv.slice_value ();
  std::string host (static_cast<const char*> (host_slice.ptr), host_slice.length);
  std::stringstream port;
  port << port_tv.value.ref (*uint16_type_t::instance ());
  slice_type_t::ValueType buf_slice = buf_tv.slice_value ();

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
