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

  node->EnterSymbol (new TypeSymbol ("int", node, &Type::NamedInt));
  node->EnterSymbol (new TypeSymbol ("int8", node, new Type::NamedType ("int8", Type::Int8::Instance ())));

  node->EnterSymbol (new TypeSymbol ("uint", node, new Type::NamedType ("uint", Type::Uint::Instance ())));

  Type::NamedType* uint8_type = new Type::NamedType ("uint8", Type::Uint8::Instance ());
  node->EnterSymbol (new TypeSymbol ("uint8", node, uint8_type));
  Type::NamedType* uint16_type = new Type::NamedType ("uint16", Type::Uint16::Instance ());
  node->EnterSymbol (new TypeSymbol ("uint16", node, uint16_type));
  node->EnterSymbol (new TypeSymbol ("uint32", node, new Type::NamedType ("uint32", Type::Uint32::Instance ())));

  Type::NamedType* uint64_type = new Type::NamedType ("uint64", Type::Uint64::Instance ());
  node->EnterSymbol (new TypeSymbol ("uint64", node, uint64_type));

  node->EnterSymbol (new TypeSymbol ("uint128", node, new Type::NamedType ("uint128", Type::Uint128::Instance ())));

  node->EnterSymbol (new TypeSymbol ("float64", node, &Type::NamedFloat64));

  node->EnterSymbol (new TypeSymbol ("string", node, &Type::NamedString));

  // Insert builtin-in functions.
  node->EnterSymbol (new runtime::New (node));
  node->EnterSymbol (new runtime::Move (node));
  node->EnterSymbol (new runtime::Merge (node));
  node->EnterSymbol (new runtime::Copy (node));
  node->EnterSymbol (new runtime::Println (node));

  /* I/O facilities. */
  Type::NamedType* fd_type = new Type::NamedType ("FileDescriptor", Type::FileDescriptor::Instance ());
  node->EnterSymbol (new TypeSymbol ("FileDescriptor", node, fd_type));
  node->EnterSymbol (new Readable (node, fd_type, &Type::NamedBool));
  node->EnterSymbol (new Read (node, fd_type, uint8_type));
  node->EnterSymbol (new Writable (node, fd_type, &Type::NamedBool));
  node->EnterSymbol (new TimerfdCreate (node, fd_type));
  node->EnterSymbol (new TimerfdSettime (node, fd_type, &Type::NamedInt, uint64_type));
  node->EnterSymbol (new UdpSocket (node, fd_type));
  node->EnterSymbol (new Sendto (node, fd_type, uint8_type, uint16_type));

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
