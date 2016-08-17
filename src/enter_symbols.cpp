#include "semantic.hpp"

#include "builtin_function.hpp"

namespace semantic
{

using namespace ast;
using namespace semantic;
using namespace decl;
using namespace runtime;

void
enter_symbols (SymbolTable& symtab)
{
  // TODO:  Do not use dynamic allocation.
  // Use statically allocated instances where possible.

  util::Location loc ("<builtin>");
  // Insert types.
  symtab.enter_symbol (&type::named_bool);

  symtab.enter_symbol (&type::named_uint8);
  symtab.enter_symbol (&type::named_uint16);
  symtab.enter_symbol (&type::named_uint32);
  symtab.enter_symbol (&type::named_uint64);

  symtab.enter_symbol (&type::named_int8);
  symtab.enter_symbol (&type::named_int16);
  symtab.enter_symbol (&type::named_int32);
  symtab.enter_symbol (&type::named_int64);

  symtab.enter_symbol (&type::named_float32);
  symtab.enter_symbol (&type::named_float64);

  symtab.enter_symbol (&type::named_complex64);
  symtab.enter_symbol (&type::named_complex128);

  symtab.enter_symbol (&type::named_byte);
  symtab.enter_symbol (&type::named_rune);

  symtab.enter_symbol (&type::named_uint);
  symtab.enter_symbol (&type::named_int);
  symtab.enter_symbol (&type::named_uintptr);

  symtab.enter_symbol (&type::named_string);

  // Insert functions.
  symtab.enter_symbol (new New (loc));
  symtab.enter_symbol (new Move (loc));
  symtab.enter_symbol (new Merge (loc));
  symtab.enter_symbol (new Len (loc));
  symtab.enter_symbol (new Append (loc));
  symtab.enter_symbol (new Copy (loc));
  symtab.enter_symbol (new Println (loc));

  // I/O facilities.
  symtab.enter_symbol (&type::named_file_descriptor);
  symtab.enter_symbol (&type::named_timespec);
  symtab.enter_symbol (new Readable (loc));
  symtab.enter_symbol (new Read (loc));
  symtab.enter_symbol (new Writable (loc));
  symtab.enter_symbol (new ClockGettime (loc));
  symtab.enter_symbol (new TimerfdCreate (loc));
  symtab.enter_symbol (new TimerfdSettime (loc));
  symtab.enter_symbol (new UdpSocket (loc));
  symtab.enter_symbol (new Sendto (loc));

  // Insert zero constant.
  Value v;
  v.present = true;
  symtab.enter_symbol (new Constant ("nil",
                                     loc,
                                     type::UntypedNil::instance (),
                                     v));

  // Insert untyped boolean constants.
  v.bool_value = true;
  symtab.enter_symbol (new Constant ("true",
                                     loc,
                                     type::UntypedBoolean::instance (),
                                     v));
  v.bool_value = false;
  symtab.enter_symbol (new Constant ("false",
                                     loc,
                                     type::UntypedBoolean::instance (),
                                     v));
}

}
