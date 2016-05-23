#include <error.h>

#include "node.hpp"
#include "node_visitor.hpp"
#include "symbol.hpp"
#include "builtin_function.hpp"
#include "template.hpp"
#include "runtime.hpp"
#include "symbol_table.hpp"

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
  // Eliminate TypeSymbol.  Let a NamedType by a Symbol.
  // Use statically allocated instances where possible.

  util::Location loc;
  // TODO:  Are named types symbols?
  // Insert types.
  symtab.enter_symbol (new TypeSymbol ("bool", loc, &type::named_bool));

  symtab.enter_symbol (new TypeSymbol ("uint8", loc, &type::named_uint8));
  symtab.enter_symbol (new TypeSymbol ("uint16", loc, &type::named_uint16));
  symtab.enter_symbol (new TypeSymbol ("uint32", loc, &type::named_uint32));
  symtab.enter_symbol (new TypeSymbol ("uint64", loc, &type::named_uint64));

  symtab.enter_symbol (new TypeSymbol ("int8", loc, &type::named_int8));
  symtab.enter_symbol (new TypeSymbol ("int16", loc, &type::named_int16));
  symtab.enter_symbol (new TypeSymbol ("int32", loc, &type::named_int32));
  symtab.enter_symbol (new TypeSymbol ("int64", loc, &type::named_int64));

  symtab.enter_symbol (new TypeSymbol ("float32", loc, &type::named_float32));
  symtab.enter_symbol (new TypeSymbol ("float64", loc, &type::named_float64));

  symtab.enter_symbol (new TypeSymbol ("complex64", loc, &type::named_complex64));
  symtab.enter_symbol (new TypeSymbol ("complex128", loc, &type::named_complex128));

  symtab.enter_symbol (new TypeSymbol ("byte", loc, &type::named_byte));
  symtab.enter_symbol (new TypeSymbol ("rune", loc, &type::named_rune));

  symtab.enter_symbol (new TypeSymbol ("uint", loc, &type::named_uint));
  symtab.enter_symbol (new TypeSymbol ("int", loc, &type::named_int));
  symtab.enter_symbol (new TypeSymbol ("uintptr", loc, &type::named_uintptr));

  symtab.enter_symbol (new TypeSymbol ("string", loc, &type::named_string));

  // Insert builtin-in functions.
  symtab.enter_symbol (new New (loc));
  symtab.enter_symbol (new Move (loc));
  symtab.enter_symbol (new Merge (loc));
  symtab.enter_symbol (new Len (loc));
  symtab.enter_symbol (new Append (loc));
  symtab.enter_symbol (new Copy (loc));
  symtab.enter_symbol (new Println (loc));

  // I/O facilities.
  symtab.enter_symbol (new TypeSymbol ("FileDescriptor", loc, &type::named_file_descriptor));
  symtab.enter_symbol (new TypeSymbol ("timespec", loc, &type::named_timespec));
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
  symtab.enter_symbol (new ConstantSymbol ("nil",
                       loc,
                       type::Nil::Instance (),
                       v));

  // Insert untyped boolean constants.
  v.bool_value = true;
  symtab.enter_symbol (new ConstantSymbol ("true",
                       loc,
                       type::Boolean::Instance (),
                       v));
  v.bool_value = false;
  symtab.enter_symbol (new ConstantSymbol ("false",
                       loc,
                       type::Boolean::Instance (),
                       v));
}

}
