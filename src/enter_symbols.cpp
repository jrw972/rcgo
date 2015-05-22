#include "ast.hpp"
#include "symbol.hpp"
#include <error.h>

void
enter_symbols (ast_t * node)
{
  /* Construct the top-level table. */
  symtab_t *symtab = symtab_get_root (node->symtab);

  /* Insert types. */
  {
    symtab->enter (symbol_make_type ("bool", new named_type_t ("bool", bool_type_t::instance ()), node));

    symtab->enter (symbol_make_type ("int", new named_type_t ("int", int_type_t::instance ()), node));

    symtab->enter (symbol_make_type ("int8", new named_type_t ("int8", int8_type_t::instance ()), node));

    symtab->enter (symbol_make_type ("uint", new named_type_t ("uint", uint_type_t::instance ()), node));

    symtab->enter (symbol_make_type ("uint8", new named_type_t ("uint8", uint8_type_t::instance ()), node));

    symtab->enter (symbol_make_type ("uint32", new named_type_t ("uint32", uint32_type_t::instance ()), node));

    symtab->enter (symbol_make_type ("uint64", new named_type_t ("uint64", uint32_type_t::instance ()), node));

    symtab->enter (symbol_make_type ("uint128", new named_type_t ("uint128", uint128_type_t::instance ()), node));

    symtab->enter (symbol_make_type ("float64", new named_type_t ("float64", float64_type_t::instance ()), node));

    symtab->enter (symbol_make_type ("string", new named_type_t ("string", string_type_t::instance ()), node));
  }

  /* Insert zero constant. */
  symtab->enter (symbol_make_typed_constant ("nil",
                                             typed_value_t::nil (),
                                             node));

  /* Insert untyped boolean constants. */
  symtab->enter (symbol_make_typed_constant ("true",
                                             typed_value_t (bool_type_t::instance (), true),
                                             node));
  symtab->enter (symbol_make_typed_constant ("false",
                                             typed_value_t (bool_type_t::instance (), false),
                                             node));

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
      enter_undefined_symbol (node.symbol, node.identifier (),
			      SymbolInstance, node.symtab);
    }

    void visit (ast_type_definition_t& node)
    {
      enter_undefined_symbol (node.symbol, node.identifier (),
			      SymbolType, node.symtab);
    }

    void visit (ast_function_t& node)
    {
      enter_undefined_symbol (node.function_symbol, node.identifier (),
                              SymbolFunction, symtab_parent (node.symtab));
    }

    static void
    enter_undefined_symbol (symbol_holder& node,
                            ast_t * identifier_node, SymbolKind kind,
                            symtab_t* symtab)
    {
      const std::string& identifier = ast_get_identifier (identifier_node);
      symbol_t *symbol = symtab->find_current (identifier);
      if (symbol == NULL)
        {
          symbol = symbol_make_undefined (identifier, kind, identifier_node);
          symtab->enter (symbol);
          node.symbol (symbol);
        }
      else
        {
          error_at_line (-1, 0, identifier_node->location.file,
                         identifier_node->location.line,
                         "%s is already defined in this scope", identifier.c_str ());
        }
    }
  };

  visitor v;
  node->accept (v);
}
