#include "ast.hpp"
#include "Symbol.hpp"
#include <error.h>
#include "parameter.hpp"
#include "BuiltinFunction.hpp"

void
enter_symbols (ast_t * node)
{
  /* Insert types. */
  named_type_t* bool_type = new named_type_t ("bool", bool_type_t::instance ());
  node->EnterSymbol (new TypeSymbol ("bool", node, bool_type));

  named_type_t* int_type = new named_type_t ("int", int_type_t::instance ());
  node->EnterSymbol (new TypeSymbol ("int", node, int_type));
  node->EnterSymbol (new TypeSymbol ("int8", node, new named_type_t ("int8", int8_type_t::instance ())));

  node->EnterSymbol (new TypeSymbol ("uint", node, new named_type_t ("uint", uint_type_t::instance ())));

  named_type_t* uint8_type = new named_type_t ("uint8", uint8_type_t::instance ());
  node->EnterSymbol (new TypeSymbol ("uint8", node, uint8_type));
  named_type_t* uint16_type = new named_type_t ("uint16", uint16_type_t::instance ());
  node->EnterSymbol (new TypeSymbol ("uint16", node, uint16_type));
  node->EnterSymbol (new TypeSymbol ("uint32", node, new named_type_t ("uint32", uint32_type_t::instance ())));

  named_type_t* uint64_type = new named_type_t ("uint64", uint64_type_t::instance ());
  node->EnterSymbol (new TypeSymbol ("uint64", node, uint64_type));

  node->EnterSymbol (new TypeSymbol ("uint128", node, new named_type_t ("uint128", uint128_type_t::instance ())));

  node->EnterSymbol (new TypeSymbol ("float64", node, new named_type_t ("float64", float64_type_t::instance ())));

  /* I/O facilities. */
  named_type_t* fd_type = new named_type_t ("FileDescriptor", FileDescriptor_type_t::instance ());
  node->EnterSymbol (new TypeSymbol ("FileDescriptor", node, fd_type));
  node->EnterSymbol (new Readable (node, fd_type, bool_type));
  node->EnterSymbol (new Read (node, fd_type, uint8_type));
  node->EnterSymbol (new Writable (node, fd_type, bool_type));
  node->EnterSymbol (new TimerfdCreate (node, fd_type));
  node->EnterSymbol (new TimerfdSettime (node, fd_type, int_type, uint64_type));
  node->EnterSymbol (new UdpSocket (node, fd_type));
  node->EnterSymbol (new Sendto (node, fd_type, uint8_type, uint16_type));

  /* Insert zero constant. */
  node->EnterSymbol (new TypedConstantSymbol ("nil",
                     node,
                     typed_value_t::nil ()));

  /* Insert untyped boolean constants. */
  node->EnterSymbol (new TypedConstantSymbol ("true",
                     node,
                     typed_value_t (bool_type_t::instance (), true)));
  node->EnterSymbol (new TypedConstantSymbol ("false",
                     node,
                     typed_value_t (bool_type_t::instance (), false)));

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
      node.function_symbol = enter_undefined_symbol (new Function (node),
                             node);
    }

    static Symbol*
    enter_undefined_symbol (Symbol* s,
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
                         "%s is already defined in this scope", identifier.c_str ());
        }
      return s;
    }
  };

  visitor v;
  node->accept (v);
}
