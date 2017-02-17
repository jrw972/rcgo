#include "builtin_function.hpp"

#include <error.h>
#include <errno.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netdb.h>
#include <unistd.h>

#include <sstream>

#include "symbol.hpp"
#include "executor_base.hpp"
#include "type.hpp"
#include "allocate.hpp"
#include "identifier.hpp"

namespace runtime
{
using namespace semantic;
using namespace decl;
using namespace type;
using namespace code;

BuiltinFunction::BuiltinFunction (const source::Identifier& identifier,
                                  const type::Function* a_type)
  : FunctionBase (identifier, a_type)
{
  allocate_parameters (memory_model, type->parameter_list);
  allocate_parameters (memory_model, type->return_parameter_list);
}

Readable::Readable (const source::Location& loc)
  : BuiltinFunction (source::Identifier ("readable", loc),
                     new type::Function ((new decl::ParameterList (loc))
                                         ->append (Parameter::make (source::Identifier ("fd", loc), Immutable, Foreign, &type::named_file_descriptor)),
                                         (new decl::ParameterList (loc))->append (Parameter::make_return (source::Identifier ("", loc), Immutable, &type::named_bool))))
{ }

void
Readable::call (runtime::ExecutorBase& exec) const
{
  runtime::FileDescriptor** fd = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type->parameter_list->at (0)->offset ()));
  bool* r = static_cast<bool*> (exec.stack ().get_address (type->return_parameter_list->at (0)->offset ()));

  struct pollfd pfd;
  pfd.fd = (*fd)->fd ();
  pfd.events = POLLIN;

  int s = poll (&pfd, 1, 0);

  if (s < 0)
    {
      error (EXIT_FAILURE, errno, "poll");
    }

  exec.checked_for_readability (*fd);

  *r = (pfd.revents & POLLIN) != 0;
}

Read::Read (const source::Location& loc)
  : BuiltinFunction (source::Identifier ("read", loc),
                     new type::Function ((new decl::ParameterList (loc))
                                         ->append (Parameter::make (source::Identifier ("fd", loc), Immutable, Mutable, &type::named_file_descriptor))
                                         ->append (Parameter::make (source::Identifier ("buf", loc),Immutable, Mutable, type::named_byte.get_slice ())),
                                         (new decl::ParameterList (loc))->append (Parameter::make_return (source::Identifier ("", loc), Immutable, Int::instance ()))))
{ }

void
Read::call (runtime::ExecutorBase& exec) const
{
  runtime::FileDescriptor** fd = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type->parameter_list->at (0)->offset ()));
  runtime::Slice* buf = static_cast<runtime::Slice*> (exec.stack ().get_address (type->parameter_list->at (1)->offset ()));
  long* r = static_cast<long*> (exec.stack ().get_address (type->return_parameter_list->at (0)->offset ()));
  *r = read ((*fd)->fd (), buf->ptr, buf->length);
}

Writable::Writable (const source::Location& loc)
  : BuiltinFunction (source::Identifier ("writable", loc),
                     new type::Function ((new decl::ParameterList (loc))
                                         ->append (Parameter::make (source::Identifier ("fd", loc),Immutable, Foreign, &type::named_file_descriptor)),
                                         (new decl::ParameterList (loc))->append (Parameter::make_return (source::Identifier ("", loc), Immutable, &type::named_bool))))
{ }

void
Writable::call (runtime::ExecutorBase& exec) const
{
  runtime::FileDescriptor** fd = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type->parameter_list->at (0)->offset ()));
  bool* r = static_cast<bool*> (exec.stack ().get_address (type->return_parameter_list->at (0)->offset ()));

  struct pollfd pfd;
  pfd.fd = (*fd)->fd ();
  pfd.events = POLLOUT;

  int s = poll (&pfd, 1, 0);

  if (s < 0)
    {
      error (EXIT_FAILURE, errno, "poll");
    }

  exec.checked_for_writability (*fd);

  *r = (pfd.revents & POLLOUT) != 0;
}

ClockGettime::ClockGettime (const source::Location& loc)
  : BuiltinFunction (source::Identifier ("clock_gettime", loc),
                     new type::Function ((new decl::ParameterList (loc))
                                         ->append (Parameter::make (source::Identifier ("tp", loc), Immutable, Foreign, type::named_timespec.get_pointer ())),
                                         (new decl::ParameterList (loc))->append (Parameter::make_return (source::Identifier ("", loc), Immutable, &type::named_int))))
{ }

void
ClockGettime::call (runtime::ExecutorBase& exec) const
{
  struct timespec* ts = *static_cast< struct timespec**> (exec.stack ().get_address (type->parameter_list->at (0)->offset ()));
  long* r = static_cast<long*> (exec.stack ().get_address (type->return_parameter_list->at (0)->offset ()));
  *r = clock_gettime (CLOCK_REALTIME, ts);
}

TimerfdCreate::TimerfdCreate (const source::Location& loc)
  : BuiltinFunction (source::Identifier ("timerfd_create", loc),
                     new type::Function (new decl::ParameterList (loc),
                                         (new decl::ParameterList (loc))->append (Parameter::make_return (source::Identifier ("", loc), Mutable, &type::named_file_descriptor))))
{ }

void
TimerfdCreate::call (runtime::ExecutorBase& exec) const
{
  runtime::FileDescriptor** ret = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type->return_parameter_list->at (0)->offset ()));
  int fd = timerfd_create (CLOCK_MONOTONIC, TFD_NONBLOCK);
  if (fd != -1)
    {
      *ret = exec.allocate_file_descriptor (fd);
    }
  else
    {
      *ret = NULL;
    }
}

TimerfdSettime::TimerfdSettime (const source::Location& loc)
  : BuiltinFunction (source::Identifier ("timerfd_settime", loc),
                     new type::Function ((new decl::ParameterList (loc))
                                         ->append (Parameter::make (source::Identifier ("fd", loc), Immutable, Mutable,  &type::named_file_descriptor))
                                         ->append (Parameter::make (source::Identifier ("s", loc), Immutable, Immutable, &type::named_uint64)),
                                         (new decl::ParameterList (loc))->append (Parameter::make_return (source::Identifier ("", loc), Immutable, &type::named_int))))
{ }

void
TimerfdSettime::call (runtime::ExecutorBase& exec) const
{
  runtime::FileDescriptor** fd = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type->parameter_list->at (0)->offset ()));
  uint64_t* v = static_cast<uint64_t*> (exec.stack ().get_address (type->parameter_list->at (1)->offset ()));
  long* r = static_cast<long*> (exec.stack ().get_address (type->return_parameter_list->at (0)->offset ()));

  struct itimerspec spec;
  spec.it_interval.tv_sec = *v;
  spec.it_interval.tv_nsec = 0;
  spec.it_value.tv_sec = *v;
  spec.it_value.tv_nsec = 0;
  *r = timerfd_settime ((*fd)->fd (), 0, &spec, NULL);
}

UdpSocket::UdpSocket (const source::Location& loc)
  : BuiltinFunction (source::Identifier ("udp_socket", loc),
                     new type::Function (new decl::ParameterList (loc),
                                         (new decl::ParameterList (loc))->append (Parameter::make_return (source::Identifier ("", loc), Mutable, &type::named_file_descriptor))))
{ }

void
UdpSocket::call (runtime::ExecutorBase& exec) const
{
  runtime::FileDescriptor** ret = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type->return_parameter_list->at (0)->offset ()));

  int fd = socket (AF_INET6, SOCK_DGRAM, 0);
  if (fd == -1)
    {
      *ret = NULL;
      return;
    }

  int s = fcntl (fd, F_SETFL, O_NONBLOCK);
  if (s == -1)
    {
      *ret = NULL;
      return;
    }

  *ret = exec.allocate_file_descriptor (fd);
}

Sendto::Sendto (const source::Location& loc)
  : BuiltinFunction (source::Identifier ("sendto", loc),
                     new type::Function ((new decl::ParameterList (loc))
                                         ->append (Parameter::make (source::Identifier ("fd", loc), Immutable, Mutable, &type::named_file_descriptor))
                                         ->append (Parameter::make (source::Identifier ("host", loc), Immutable, Foreign, &type::named_string))
                                         ->append (Parameter::make (source::Identifier ("port", loc), Immutable, Immutable, &type::named_uint16))
                                         ->append (Parameter::make (source::Identifier ("buf", loc), Immutable, Foreign, type::named_byte.get_slice ())),
                                         (new decl::ParameterList (loc))->append (Parameter::make_return (source::Identifier ("", loc), Immutable, Int::instance ()))))
{ }

void
Sendto::call (runtime::ExecutorBase& exec) const
{
  runtime::FileDescriptor** fd = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type->parameter_list->at (0)->offset ()));
  runtime::String* host = static_cast<runtime::String*> (exec.stack ().get_address (type->parameter_list->at (1)->offset ()));
  uint16_t* port = static_cast<uint16_t*> (exec.stack ().get_address (type->parameter_list->at (2)->offset ()));
  runtime::Slice* buf = static_cast<runtime::Slice*> (exec.stack ().get_address (type->parameter_list->at (3)->offset ()));
  long* ret = static_cast<long*> (exec.stack ().get_address (type->return_parameter_list->at (0)->offset ()));

  std::string host2 (static_cast<const char*> (host->ptr), host->length);
  std::stringstream port2;
  port2 << *port;

  struct addrinfo* info = NULL;
  struct addrinfo hints;
  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG | AI_NUMERICSERV;
  int r = getaddrinfo (host2.c_str (), port2.str ().c_str (), &hints, &info);
  if (r != 0)
    {
      UNIMPLEMENTED;
    }
  ssize_t s = sendto ((*fd)->fd (), buf->ptr, buf->length, 0, info->ai_addr, info->ai_addrlen);
  if (s != static_cast<ssize_t> (buf->length))
    {
      UNIMPLEMENTED;
    }

  freeaddrinfo (info);
  *ret = 0;
}

};
