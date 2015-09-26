#include "ast.hpp"
#include "Symbol.hpp"
#include <error.h>
#include "parameter.hpp"

void
enter_symbols (ast_t * node)
{
  /* Construct the top-level table. */
  symtab_t *symtab = symtab_get_root (node->symtab);

  /* Insert types. */
  named_type_t* bool_type = new named_type_t ("bool", bool_type_t::instance ());
  symtab->enter (new TypeSymbol ("bool", node, bool_type));

  named_type_t* int_type = new named_type_t ("int", int_type_t::instance ());
  symtab->enter (new TypeSymbol ("int", node, int_type));

  symtab->enter (new TypeSymbol ("int8", node, new named_type_t ("int8", int8_type_t::instance ())));

  symtab->enter (new TypeSymbol ("uint", node, new named_type_t ("uint", uint_type_t::instance ())));

  named_type_t* uint8_type = new named_type_t ("uint8", uint8_type_t::instance ());
  symtab->enter (new TypeSymbol ("uint8", node, uint8_type));

  symtab->enter (new TypeSymbol ("uint32", node, new named_type_t ("uint32", uint32_type_t::instance ())));

  named_type_t* uint64_type = new named_type_t ("uint64", uint64_type_t::instance ());
  symtab->enter (new TypeSymbol ("uint64", node, uint64_type));

  symtab->enter (new TypeSymbol ("uint128", node, new named_type_t ("uint128", uint128_type_t::instance ())));

  symtab->enter (new TypeSymbol ("float64", node, new named_type_t ("float64", float64_type_t::instance ())));

  symtab->enter (new TypeSymbol ("string", node, new named_type_t ("string", string_type_t::instance ())));

  /* I/O facilities. */
  named_type_t* fd_type = new named_type_t ("FileDescriptor", FileDescriptor_type_t::instance ());
  symtab->enter (new TypeSymbol ("FileDescriptor", node, fd_type));
  symtab->enter (new Readable (node, fd_type, bool_type));
  symtab->enter (new Read (node, fd_type, uint8_type));
  symtab->enter (new TimerfdCreate (node, fd_type));
  symtab->enter (new TimerfdSettime (node, fd_type, int_type, uint64_type));

  /* Insert zero constant. */
  symtab->enter (new TypedConstantSymbol ("nil",
                                          node,
                                          typed_value_t::nil ()));

  /* Insert untyped boolean constants. */
  symtab->enter (new TypedConstantSymbol ("true",
                                          node,
                                          typed_value_t (bool_type_t::instance (), true)));
  symtab->enter (new TypedConstantSymbol ("false",
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
      enter_undefined_symbol (node.symbol,
                              new InstanceSymbol (ast_get_identifier (node.identifier ()), node.identifier ()),
			      node.symtab);
    }

    void visit (ast_type_definition_t& node)
    {
      enter_undefined_symbol (node.symbol,
                              new TypeSymbol (ast_get_identifier (node.identifier ()), node.identifier ()),
			      node.symtab);
    }

    void visit (ast_function_t& node)
    {
      enter_undefined_symbol (node.function_symbol,
                              new Function (node),
                              symtab_parent (node.symtab));
    }

    static void
    enter_undefined_symbol (symbol_holder& node,
                            Symbol* s,
                            symtab_t* symtab)
    {
      const std::string& identifier = s->identifier;
      Symbol *symbol = symtab->find_current (identifier);
      if (symbol == NULL)
        {
          symbol = s;
          symtab->enter (symbol);
          node.symbol (symbol);
        }
      else
        {
           error_at_line (-1, 0, s->definingNode->location.File.c_str (),
                         s->definingNode->location.Line,
                         "%s is already defined in this scope", identifier.c_str ());
        }
    }
  };

  visitor v;
  node->accept (v);
}
