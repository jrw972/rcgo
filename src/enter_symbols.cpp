#include <error.h>

#include "ast.hpp"
#include "ast_visitor.hpp"
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

void
enter_symbols (SymbolTable& symtab)
{
  util::Location loc;
  // Insert types.
  symtab.enter_symbol (new TypeSymbol ("bool", loc, &type::NamedBool));

  symtab.enter_symbol (new TypeSymbol ("uint8", loc, &type::NamedUint8));
  symtab.enter_symbol (new TypeSymbol ("uint16", loc, &type::NamedUint16));
  symtab.enter_symbol (new TypeSymbol ("uint32", loc, &type::NamedUint32));
  symtab.enter_symbol (new TypeSymbol ("uint64", loc, &type::NamedUint64));

  symtab.enter_symbol (new TypeSymbol ("int8", loc, &type::NamedInt8));
  symtab.enter_symbol (new TypeSymbol ("int16", loc, &type::NamedInt16));
  symtab.enter_symbol (new TypeSymbol ("int32", loc, &type::NamedInt32));
  symtab.enter_symbol (new TypeSymbol ("int64", loc, &type::NamedInt64));

  symtab.enter_symbol (new TypeSymbol ("float32", loc, &type::NamedFloat32));
  symtab.enter_symbol (new TypeSymbol ("float64", loc, &type::NamedFloat64));

  symtab.enter_symbol (new TypeSymbol ("complex64", loc, &type::NamedComplex64));
  symtab.enter_symbol (new TypeSymbol ("complex128", loc, &type::NamedComplex128));

  symtab.enter_symbol (new TypeSymbol ("byte", loc, &type::NamedByte));
  symtab.enter_symbol (new TypeSymbol ("rune", loc, &type::NamedRune));

  symtab.enter_symbol (new TypeSymbol ("uint", loc, &type::NamedUint));
  symtab.enter_symbol (new TypeSymbol ("int", loc, &type::NamedInt));
  symtab.enter_symbol (new TypeSymbol ("uintptr", loc, &type::NamedUintptr));

  symtab.enter_symbol (new TypeSymbol ("string", loc, &type::NamedString));

  // Insert builtin-in functions.
  symtab.enter_symbol (new runtime::New (loc));
  symtab.enter_symbol (new runtime::Move (loc));
  symtab.enter_symbol (new runtime::Merge (loc));
  symtab.enter_symbol (new runtime::Copy (loc));
  symtab.enter_symbol (new runtime::Println (loc));

  // I/O facilities.
  symtab.enter_symbol (new TypeSymbol ("FileDescriptor", loc, &type::NamedFileDescriptor));
  symtab.enter_symbol (new Readable (loc));
  symtab.enter_symbol (new Read (loc));
  symtab.enter_symbol (new Writable (loc));
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
