#include "ast.hpp"
#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include <error.h>
#include "BuiltinFunction.hpp"
#include "Template.hpp"
#include "runtime.hpp"
using namespace ast;

void
enter_symbols (Node * node)
{
  /* Insert types. */
  node->EnterSymbol (new TypeSymbol ("bool", node, &Type::NamedBool));

  node->EnterSymbol (new TypeSymbol ("uint8", node, &Type::NamedUint8));
  node->EnterSymbol (new TypeSymbol ("uint16", node, &Type::NamedUint16));
  node->EnterSymbol (new TypeSymbol ("uint32", node, &Type::NamedUint32));
  node->EnterSymbol (new TypeSymbol ("uint64", node, &Type::NamedUint64));

  node->EnterSymbol (new TypeSymbol ("int8", node, &Type::NamedInt8));
  node->EnterSymbol (new TypeSymbol ("int16", node, &Type::NamedInt16));
  node->EnterSymbol (new TypeSymbol ("int32", node, &Type::NamedInt32));
  node->EnterSymbol (new TypeSymbol ("int64", node, &Type::NamedInt64));

  node->EnterSymbol (new TypeSymbol ("float32", node, &Type::NamedFloat32));
  node->EnterSymbol (new TypeSymbol ("float64", node, &Type::NamedFloat64));

  node->EnterSymbol (new TypeSymbol ("complex64", node, &Type::NamedComplex64));
  node->EnterSymbol (new TypeSymbol ("complex128", node, &Type::NamedComplex128));

  node->EnterSymbol (new TypeSymbol ("byte", node, &Type::NamedByte));
  node->EnterSymbol (new TypeSymbol ("rune", node, &Type::NamedRune));

  node->EnterSymbol (new TypeSymbol ("uint", node, &Type::NamedUint));
  node->EnterSymbol (new TypeSymbol ("int", node, &Type::NamedInt));
  node->EnterSymbol (new TypeSymbol ("uintptr", node, &Type::NamedUintptr));

  node->EnterSymbol (new TypeSymbol ("string", node, &Type::NamedString));

  // Insert builtin-in functions.
  node->EnterSymbol (new runtime::New (node));
  node->EnterSymbol (new runtime::Move (node));
  node->EnterSymbol (new runtime::Merge (node));
  node->EnterSymbol (new runtime::Copy (node));
  node->EnterSymbol (new runtime::Println (node));

  /* I/O facilities. */
  node->EnterSymbol (new TypeSymbol ("FileDescriptor", node, &Type::NamedFileDescriptor));
  node->EnterSymbol (new Readable (node));
  node->EnterSymbol (new Read (node));
  node->EnterSymbol (new Writable (node));
  node->EnterSymbol (new TimerfdCreate (node));
  node->EnterSymbol (new TimerfdSettime (node));
  node->EnterSymbol (new UdpSocket (node));
  node->EnterSymbol (new Sendto (node));

  /* Insert zero constant. */
  node->EnterSymbol (new ConstantSymbol ("nil",
                                         node,
                                         Type::Nil::Instance (),
                                         value_t ()));

  /* Insert untyped boolean constants. */
  node->EnterSymbol (new ConstantSymbol ("true",
                                         node,
                                         Type::Boolean::Instance (),
                                         value_t (Type::Boolean::Instance (), true)));
  node->EnterSymbol (new ConstantSymbol ("false",
                                         node,
                                         Type::Boolean::Instance (),
                                         value_t (Type::Boolean::Instance (), false)));
}
