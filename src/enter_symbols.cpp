#include "ast.hpp"
#include "Symbol.hpp"
#include <error.h>
#include "parameter.hpp"
#include "BuiltinFunction.hpp"
#include "Template.hpp"
#include "runtime.hpp"

template <typename T>
static T*
enter_undefined_symbol (T* s,
                        ast_t& a)
{
  ast_t* symtab = a.parent ();
  const std::string& identifier = s->identifier;
  Symbol *symbol = symtab->FindSymbolCurrent (identifier);
  if (symbol == NULL)
    {
      symtab->EnterSymbol (s);
    }
  else
    {
      error_at_line (-1, 0, s->definingNode->location.File.c_str (),
                     s->definingNode->location.Line,
                     "%s is already defined in this scope (E73)", identifier.c_str ());
    }
  return s;
}

void
enter_symbols (ast_t * node)
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
  node->EnterSymbol (new TypedConstantSymbol ("nil",
                     node,
                     typed_value_t::nil ()));

  /* Insert untyped boolean constants. */
  node->EnterSymbol (new TypedConstantSymbol ("true",
                     node,
                     typed_value_t (Type::Boolean::Instance (), true)));
  node->EnterSymbol (new TypedConstantSymbol ("false",
                     node,
                     typed_value_t (Type::Boolean::Instance (), false)));

  struct visitor : public ast_visitor_t
  {
    void default_action (ast_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          (*pos)->accept (*this);
        }
    }

    void visit (ast_instance_t& node)
    {
      node.symbol =
        enter_undefined_symbol (new InstanceSymbol (ast_get_identifier (node.identifier ()), node.identifier ()),
                                node);
    }

    void visit (ast_type_definition_t& node)
    {
      node.symbol =
        enter_undefined_symbol (new TypeSymbol (ast_get_identifier (node.identifier ()), node.identifier ()),
                                node);
    }

    void visit (ast_function_t& node)
    {
      node.function = enter_undefined_symbol (new Function (node),
                                              node);
    }
  };

  visitor v;
  node->accept (v);
}
