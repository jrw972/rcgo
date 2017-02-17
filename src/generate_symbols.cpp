#include "node_visitor.hpp"
#include "node.hpp"
#include "scope.hpp"
#include "error_reporter.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "node_cast.hpp"
#include "callable.hpp"
#include "symbol_table.hpp"
#include "identifier.hpp"

namespace semantic
{
namespace
{
using namespace ast;
using namespace decl;
using namespace util;
using namespace type;
using namespace source;

struct visitor : public DefaultNodeVisitor
{
  visitor (ErrorReporter& a_er,
           SymbolTable& a_symbol_table)
    : er (a_er)
    , symbol_table (a_symbol_table)
  { }

  ErrorReporter& er;
  SymbolTable& symbol_table;

  void check_already_declared (const Identifier& identifier)
  {
    if (symbol_table.is_declared_locally (identifier.identifier ()))
      {
        Symbol* s = symbol_table.retrieve_symbol (identifier);
        er.already_declared (identifier.location (), identifier.identifier (), s->identifier.location ());
      }
  }

  void visit (ConstDecl& node)
  {
    ast::IdentifierList* identifier_list = node.identifiers;
    ast::List* expression_list = node.expressions;

    if (identifier_list->size () != expression_list->size ())
      {
        er.expected_n_expressions (node.location, identifier_list->size (), expression_list->size ());
      }

    const size_t limit = std::min (identifier_list->size (), expression_list->size ());

    // Enter each symbol.
    size_t idx = 0;
    IdentifierList::const_iterator id_pos;
    List::ConstIterator init_pos;
    for (id_pos = identifier_list->begin (),
         init_pos = expression_list->begin ();
         idx != limit;
         ++id_pos, ++init_pos, ++idx)
      {
        const Identifier& id = *id_pos;
        check_already_declared (id);
        Constant* symbol = new Constant (id, node.type, *init_pos);
        symbol_table.enter_symbol (symbol);
        node.symbols.push_back (symbol);
      }
  }

  void visit (TypeDecl& node)
  {
    check_already_declared (node.identifier);
    node.symbol = new NamedType (&node);
    symbol_table.enter_symbol (node.symbol);
  }

  void visit (FunctionDecl& node)
  {
    check_already_declared (node.identifier);
    node.symbol = new decl::Function (&node);
    symbol_table.enter_symbol (node.symbol);
  }

  void visit (InstanceDecl& node)
  {
    check_already_declared (node.identifier);
    node.symbol = new decl::Instance (&node);
    symbol_table.enter_symbol (node.symbol);
  }
};
}

void generate_symbols (ast::Node* root,
                       util::ErrorReporter& er,
                       decl::SymbolTable& symbol_table)
{
  visitor v (er, symbol_table);
  root->accept (v);
}

}
