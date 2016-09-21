#include "enter_predeclared_identifiers.hpp"

#include "builtin_function.hpp"
#include "scope.hpp"
#include "semantic.hpp"

namespace semantic
{

using namespace ast;
using namespace semantic;
using namespace decl;
using namespace runtime;

void
enter_predeclared_identifiers (Scope* scope)
{
  // TODO:  Do not use dynamic allocation.
  // Use statically allocated instances where possible.

  util::Location loc ("<builtin>");
  // Insert types.
  scope->enter_symbol (&type::named_bool);

  scope->enter_symbol (&type::named_uint8);
  scope->enter_symbol (&type::named_uint16);
  scope->enter_symbol (&type::named_uint32);
  scope->enter_symbol (&type::named_uint64);

  scope->enter_symbol (&type::named_int8);
  scope->enter_symbol (&type::named_int16);
  scope->enter_symbol (&type::named_int32);
  scope->enter_symbol (&type::named_int64);

  scope->enter_symbol (&type::named_float32);
  scope->enter_symbol (&type::named_float64);

  scope->enter_symbol (&type::named_complex64);
  scope->enter_symbol (&type::named_complex128);

  scope->enter_symbol (&type::named_byte);
  scope->enter_symbol (&type::named_rune);

  scope->enter_symbol (&type::named_uint);
  scope->enter_symbol (&type::named_int);
  scope->enter_symbol (&type::named_uintptr);

  scope->enter_symbol (&type::named_string);

  // Insert functions.
  scope->enter_symbol (new New (loc));
  scope->enter_symbol (new Move (loc));
  scope->enter_symbol (new Merge (loc));
  scope->enter_symbol (new Len (loc));
  scope->enter_symbol (new Append (loc));
  scope->enter_symbol (new Copy (loc));
  scope->enter_symbol (new Println (loc));

  // I/O facilities.
  scope->enter_symbol (&type::named_file_descriptor);
  scope->enter_symbol (&type::named_timespec);
  scope->enter_symbol (new Readable (loc));
  scope->enter_symbol (new Read (loc));
  scope->enter_symbol (new Writable (loc));
  scope->enter_symbol (new ClockGettime (loc));
  scope->enter_symbol (new TimerfdCreate (loc));
  scope->enter_symbol (new TimerfdSettime (loc));
  scope->enter_symbol (new UdpSocket (loc));
  scope->enter_symbol (new Sendto (loc));

  // Insert zero constant.
  Value v;
  v.present = true;
  scope->enter_symbol (new Constant ("nil",
                                     loc,
                                     type::UntypedNil::instance (),
                                     v));

  // Insert untyped boolean constants.
  v.bool_value = true;
  scope->enter_symbol (new Constant ("true",
                                     loc,
                                     type::UntypedBoolean::instance (),
                                     v));
  v.bool_value = false;
  scope->enter_symbol (new Constant ("false",
                                     loc,
                                     type::UntypedBoolean::instance (),
                                     v));
}

}
