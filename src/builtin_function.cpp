#include "builtin_function.hpp"

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

#include "symbol_visitor.hpp"
#include "executor_base.hpp"
#include "heap.hpp"
#include "ast.hpp"
#include "runtime.hpp"
#include "semantic.hpp"

namespace decl
{

using namespace type;
using namespace ast;
using namespace semantic;

BuiltinFunction::BuiltinFunction (const std::string& id,
                                  ast::Node* dn,
                                  const type::Function* type)
  : Symbol (id, dn)
  , type_ (type)
{
  allocate_parameter (memory_model_, type_->GetSignature ()->Begin (), type_->GetSignature ()->End ());
  allocate_symbol (memory_model_, type_->GetReturnParameter ());
}

void
BuiltinFunction::accept (decl::SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
BuiltinFunction::accept (decl::ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

Readable::Readable (ast::Node* dn)
  : BuiltinFunction ("readable",
                     dn,
                     new type::Function (type::Function::FUNCTION, (new Signature ())
                                         ->Append (ParameterSymbol::make (dn, "fd", &type::NamedFileDescriptor, IMMUTABLE, FOREIGN)),
                                         ParameterSymbol::makeReturn (dn, ReturnSymbol, &type::NamedBool, IMMUTABLE)))
{ }

void
Readable::call (runtime::executor_base_t& exec) const
{
  runtime::FileDescriptor** fd = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type_->GetSignature ()->At (0)->offset ()));
  Bool::ValueType* r = static_cast<Bool::ValueType*> (exec.stack ().get_address (type_->GetReturnParameter ()->offset ()));

  struct pollfd pfd;
  pfd.fd = (*fd)->fd ();
  pfd.events = POLLIN;

  int s = poll (&pfd, 1, 0);

  if (s < 0)
    {
      error (EXIT_FAILURE, errno, "poll");
    }

  exec.checkedForReadability (*fd);

  *r = (pfd.revents & POLLIN) != 0;
}

Read::Read (ast::Node* dn)
  : BuiltinFunction ("read",
                     dn,
                     new type::Function (type::Function::FUNCTION, (new Signature ())
                                         ->Append (ParameterSymbol::make (dn, "fd", &type::NamedFileDescriptor, IMMUTABLE, MUTABLE))
                                         ->Append (ParameterSymbol::make (dn, "buf", type::NamedByte.GetSlice (), IMMUTABLE, MUTABLE)),
                                         ParameterSymbol::makeReturn (dn, ReturnSymbol, Int::Instance (), IMMUTABLE)))
{ }

void
Read::call (runtime::executor_base_t& exec) const
{
  runtime::FileDescriptor** fd = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type_->GetSignature ()->At (0)->offset ()));
  Slice::ValueType* buf = static_cast<Slice::ValueType*> (exec.stack ().get_address (type_->GetSignature ()->At (1)->offset ()));
  Int::ValueType* r = static_cast<Int::ValueType*> (exec.stack ().get_address (type_->GetReturnParameter ()->offset ()));
  *r = read ((*fd)->fd (), buf->ptr, buf->length);
}

Writable::Writable (ast::Node* dn)
  : BuiltinFunction ("writable",
                     dn,
                     new type::Function (type::Function::FUNCTION, (new Signature ())
                                         ->Append (ParameterSymbol::make (dn, "fd", &type::NamedFileDescriptor, IMMUTABLE, FOREIGN)),
                                         ParameterSymbol::makeReturn (dn, ReturnSymbol, &type::NamedBool, IMMUTABLE)))
{ }

void
Writable::call (runtime::executor_base_t& exec) const
{
  runtime::FileDescriptor** fd = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type_->GetSignature ()->At (0)->offset ()));
  Bool::ValueType* r = static_cast<Bool::ValueType*> (exec.stack ().get_address (type_->GetReturnParameter ()->offset ()));

  struct pollfd pfd;
  pfd.fd = (*fd)->fd ();
  pfd.events = POLLOUT;

  int s = poll (&pfd, 1, 0);

  if (s < 0)
    {
      error (EXIT_FAILURE, errno, "poll");
    }

  exec.checkedForWritability (*fd);

  *r = (pfd.revents & POLLOUT) != 0;
}

TimerfdCreate::TimerfdCreate (ast::Node* dn)
  : BuiltinFunction ("timerfd_create",
                     dn,
                     new type::Function (type::Function::FUNCTION, new Signature (),
                                         ParameterSymbol::makeReturn (dn, ReturnSymbol, &type::NamedFileDescriptor, MUTABLE)))
{ }

void
TimerfdCreate::call (runtime::executor_base_t& exec) const
{
  runtime::FileDescriptor** ret = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type_->GetReturnParameter ()->offset ()));
  int fd = timerfd_create (CLOCK_MONOTONIC, TFD_NONBLOCK);
  if (fd != -1)
    {
      *ret = exec.allocateFileDescriptor (fd);
    }
  else
    {
      *ret = NULL;
    }
}

TimerfdSettime::TimerfdSettime (ast::Node* dn)
  : BuiltinFunction ("timerfd_settime",
                     dn,
                     new type::Function (type::Function::FUNCTION, (new Signature ())
                                         ->Append (ParameterSymbol::make (dn, "fd", &type::NamedFileDescriptor, IMMUTABLE, MUTABLE))
                                         ->Append (ParameterSymbol::make (dn, "s", &type::NamedUint64, IMMUTABLE, IMMUTABLE)),
                                         ParameterSymbol::makeReturn (dn, ReturnSymbol, &type::NamedInt, IMMUTABLE)))
{ }

void
TimerfdSettime::call (runtime::executor_base_t& exec) const
{
  runtime::FileDescriptor** fd = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type_->GetSignature ()->At (0)->offset ()));
  Uint64::ValueType* v = static_cast<Uint64::ValueType*> (exec.stack ().get_address (type_->GetSignature ()->At (1)->offset ()));
  Int::ValueType* r = static_cast<Int::ValueType*> (exec.stack ().get_address (type_->GetReturnParameter ()->offset ()));

  struct itimerspec spec;
  spec.it_interval.tv_sec = *v;
  spec.it_interval.tv_nsec = 0;
  spec.it_value.tv_sec = *v;
  spec.it_value.tv_nsec = 0;
  *r = timerfd_settime ((*fd)->fd (), 0, &spec, NULL);
}

UdpSocket::UdpSocket (ast::Node* dn)
  : BuiltinFunction ("udp_socket",
                     dn,
                     new type::Function (type::Function::FUNCTION, new Signature (),
                                         ParameterSymbol::makeReturn (dn, ReturnSymbol, &type::NamedFileDescriptor, MUTABLE)))
{ }

void
UdpSocket::call (runtime::executor_base_t& exec) const
{
  runtime::FileDescriptor** ret = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type_->GetReturnParameter ()->offset ()));

  int fd = socket (AF_INET, SOCK_DGRAM, 0);
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

  *ret = exec.allocateFileDescriptor (fd);
}

Sendto::Sendto (ast::Node* dn)
  : BuiltinFunction ("sendto",
                     dn,
                     new type::Function (type::Function::FUNCTION, (new Signature ())
                                         ->Append (ParameterSymbol::make (dn, "fd", &type::NamedFileDescriptor, IMMUTABLE, MUTABLE))
                                         ->Append (ParameterSymbol::make (dn, "host", &type::NamedString, IMMUTABLE, FOREIGN))
                                         ->Append (ParameterSymbol::make (dn, "port", &type::NamedUint16, IMMUTABLE, IMMUTABLE))
                                         ->Append (ParameterSymbol::make (dn, "buf", type::NamedByte.GetSlice (), IMMUTABLE, FOREIGN)),
                                         ParameterSymbol::makeReturn (dn, ReturnSymbol, Int::Instance (), IMMUTABLE)))
{ }

void
Sendto::call (runtime::executor_base_t& exec) const
{
  runtime::FileDescriptor** fd = static_cast< runtime::FileDescriptor**> (exec.stack ().get_address (type_->GetSignature ()->At (0)->offset ()));
  StringU::ValueType* host = static_cast<StringU::ValueType*> (exec.stack ().get_address (type_->GetSignature ()->At (1)->offset ()));
  Uint16::ValueType* port = static_cast<Uint16::ValueType*> (exec.stack ().get_address (type_->GetSignature ()->At (2)->offset ()));
  Slice::ValueType* buf = static_cast<Slice::ValueType*> (exec.stack ().get_address (type_->GetSignature ()->At (3)->offset ()));
  Int::ValueType* ret = static_cast<Int::ValueType*> (exec.stack ().get_address (type_->GetReturnParameter ()->offset ()));

  std::string host2 (static_cast<const char*> (host->ptr), host->length);
  std::stringstream port2;
  port2 << *port;

  struct addrinfo* info;
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

}
