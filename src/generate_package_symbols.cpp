#include "decl.hpp"

#include "node_visitor.hpp"
#include "node.hpp"
#include "symbol_table.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "callable.hpp"

namespace decl
{

  namespace
{

  using namespace ast;
  using namespace decl;
  using namespace source;
  using namespace util;

  struct visitor : public DefaultNodeVisitor
  {

    ErrorReporter& er;
    SymbolTable& symbol_table;

    visitor (ErrorReporter& a_er,
             SymbolTable& a_symbol_table)
      : er (a_er)
      , symbol_table (a_symbol_table)
    { }

    void visit (TopLevelDeclList& node)
    {
      node.visit_children (*this);
    }

    void visit (ConstDecl& node)
    {
      ast::IdentifierList* identifier_list = node.identifiers;

      // Enter each symbol.
      for (ast::IdentifierList::const_iterator id_pos = identifier_list->begin (),
             id_limit = identifier_list->end ();
           id_pos != id_limit;
           ++id_pos)
        {
          const Identifier& id = *id_pos;
          decl::Constant* symbol = new decl::Constant (id, node.type, NULL);
          node.symbols.push_back (symbol);
          if (!symbol_table.check_is_declared_locally (er, id))
            {
              symbol_table.enter_symbol (symbol);
            }
        }
    }

    void visit (TypeDecl& node)
    {
      node.symbol = new type::NamedType (&node);
      if (!symbol_table.check_is_declared_locally (er, node.identifier))
        {
          symbol_table.enter_symbol (node.symbol);
        }
    }

    void visit (FunctionDecl& node)
    {
      node.symbol = new decl::Function (&node);
      if (!symbol_table.check_is_declared_locally (er, node.identifier))
        {
          symbol_table.enter_symbol (node.symbol);
        }
    }

    void visit (InstanceDecl& node)
    {
      node.symbol = new decl::Instance (&node);
      if (!symbol_table.check_is_declared_locally (er, node.identifier))
        {
          symbol_table.enter_symbol (node.symbol);
        }
    }
  };

}

void
generate_package_symbols (util::ErrorReporter& er,
                          SymbolTable& symbol_table,
                          ast::Node* node)
{
  visitor v (er, symbol_table);
  node->accept (v);
}

}
