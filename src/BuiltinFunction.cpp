#include "BuiltinFunction.hpp"
#include "SymbolVisitor.hpp"
#include "executor_base.hpp"
#include "heap.hpp"
#include "ast.hpp"
#include "runtime.hpp"

#include <sys/timerfd.h>
#include <poll.h>
#include <errno.h>

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

  if (r < 0) {
    error (EXIT_FAILURE, errno, "poll");
  }

  exec.checkedForReadability (fd);

  stack_frame_push_tv (exec.stack (), typed_value_t (bool_type_t::instance (), pfd.revents & POLLIN));
}

Read::Read (ast_t* dn, const type_t* fd_type, const type_t* uint8_type)
  : BuiltinFunction ("read",
                     dn,
                     new function_type_t ((new signature_type_t ())
                                          ->append (new parameter_t (dn, "fd", typed_value_t::make_value (fd_type, typed_value_t::STACK, MUTABLE, IMMUTABLE), false))
                                          ->append (new parameter_t (dn, "buf", typed_value_t::make_value (uint8_type->getSliceType (), typed_value_t::STACK, MUTABLE, IMMUTABLE), false)),
                                          new parameter_t (dn, "0return", typed_value_t::make_value (void_type_t::instance (), typed_value_t::STACK, MUTABLE, MUTABLE), false)))
{ }

void
Read::call (executor_base_t& exec, const ast_call_expr_t& node) const
{
  ast_t::const_iterator pos = node.args ()->begin ();
  runtime::evaluate_expr (exec, *pos++);
  FileDescriptor* fd = static_cast<FileDescriptor*> (stack_frame_pop_pointer (exec.stack ()));

  runtime::evaluate_expr (exec, *pos++);

  std::cout << "reading fd=" << fd->fd () << '\n';

  // struct pollfd pfd;
  // pfd.fd = fd->fd ();
  // pfd.events = POLLIN;

  // int r = poll (&pfd, 1, 0);

  // if (r < 0) {
  //   error (EXIT_FAILURE, errno, "poll");
  // }

  // exec.checkedForReadability (fd);

  // stack_frame_push_tv (exec.stack (), typed_value_t (bool_type_t::instance (), pfd.revents & POLLIN));
  unimplemented;
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
  if (fd != -1) {
    FileDescriptor* thefd = exec.allocateFileDescriptor (fd);
    stack_frame_push_pointer (exec.stack (), thefd);
  } else {
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
