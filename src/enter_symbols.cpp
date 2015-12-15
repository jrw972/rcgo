#include <error.h>

#include "ast.hpp"
#include "ast_visitor.hpp"
#include "symbol.hpp"
#include "builtin_function.hpp"
#include "template.hpp"
#include "runtime.hpp"

namespace semantic
{

using namespace ast;
using namespace semantic;
using namespace decl;

void
enter_symbols (Node * node)
{
  /* Insert types. */
  node->EnterSymbol (new TypeSymbol ("bool", node, &type::NamedBool));

  node->EnterSymbol (new TypeSymbol ("uint8", node, &type::NamedUint8));
  node->EnterSymbol (new TypeSymbol ("uint16", node, &type::NamedUint16));
  node->EnterSymbol (new TypeSymbol ("uint32", node, &type::NamedUint32));
  node->EnterSymbol (new TypeSymbol ("uint64", node, &type::NamedUint64));

  node->EnterSymbol (new TypeSymbol ("int8", node, &type::NamedInt8));
  node->EnterSymbol (new TypeSymbol ("int16", node, &type::NamedInt16));
  node->EnterSymbol (new TypeSymbol ("int32", node, &type::NamedInt32));
  node->EnterSymbol (new TypeSymbol ("int64", node, &type::NamedInt64));

  node->EnterSymbol (new TypeSymbol ("float32", node, &type::NamedFloat32));
  node->EnterSymbol (new TypeSymbol ("float64", node, &type::NamedFloat64));

  node->EnterSymbol (new TypeSymbol ("complex64", node, &type::NamedComplex64));
  node->EnterSymbol (new TypeSymbol ("complex128", node, &type::NamedComplex128));

  node->EnterSymbol (new TypeSymbol ("byte", node, &type::NamedByte));
  node->EnterSymbol (new TypeSymbol ("rune", node, &type::NamedRune));

  node->EnterSymbol (new TypeSymbol ("uint", node, &type::NamedUint));
  node->EnterSymbol (new TypeSymbol ("int", node, &type::NamedInt));
  node->EnterSymbol (new TypeSymbol ("uintptr", node, &type::NamedUintptr));

  node->EnterSymbol (new TypeSymbol ("string", node, &type::NamedString));

  // Insert builtin-in functions.
  node->EnterSymbol (new runtime::New (node));
  node->EnterSymbol (new runtime::Move (node));
  node->EnterSymbol (new runtime::Merge (node));
  node->EnterSymbol (new runtime::Copy (node));
  node->EnterSymbol (new runtime::Println (node));

  /* I/O facilities. */
  node->EnterSymbol (new TypeSymbol ("FileDescriptor", node, &type::NamedFileDescriptor));
  node->EnterSymbol (new Readable (node));
  node->EnterSymbol (new Read (node));
  node->EnterSymbol (new Writable (node));
  node->EnterSymbol (new TimerfdCreate (node));
  node->EnterSymbol (new TimerfdSettime (node));
  node->EnterSymbol (new UdpSocket (node));
  node->EnterSymbol (new Sendto (node));

  /* Insert zero constant. */
  value_t v;
  v.present = true;
  node->EnterSymbol (new ConstantSymbol ("nil",
                                         node,
                                         type::Nil::Instance (),
                                         v));

  /* Insert untyped boolean constants. */
  node->EnterSymbol (new ConstantSymbol ("true",
                                         node,
                                         type::Boolean::Instance (),
                                         value_t (type::Boolean::Instance (), true)));
  node->EnterSymbol (new ConstantSymbol ("false",
                                         node,
                                         type::Boolean::Instance (),
                                         value_t (type::Boolean::Instance (), false)));
}

}
