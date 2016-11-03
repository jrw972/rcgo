#include "node_visitor.hpp"
#include "node.hpp"
#include "scope.hpp"
#include "error_reporter.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "node_cast.hpp"
#include "callable.hpp"
#include "symbol_table.hpp"

namespace semantic
{
namespace
{
using namespace ast;
using namespace decl;
using namespace util;
using namespace type;

struct visitor : public DefaultNodeVisitor
{
  visitor (ErrorReporter& a_er,
           SymbolTable& a_symbol_table)
    : er (a_er)
    , symbol_table (a_symbol_table)
  { }

  ErrorReporter& er;
  SymbolTable& symbol_table;

  void check_already_declared (const Identifier* node)
  {
    if (symbol_table.is_declared_locally (node->identifier))
      {
        Symbol* s = symbol_table.retrieve_symbol (node->identifier);
        er.already_declared (node->location, node->identifier, s->location);
      }
  }

  void visit (SourceFile& node)
  {
    node.top_level_decl_list->accept (*this);
  }

  void visit (TopLevelDeclList& node)
  {
    node.visit_children (*this);
  }

  void visit (ConstDecl& node)
  {
    ast::List* identifier_list = node.identifiers;
    ast::List* expression_list = node.expressions;

    if (identifier_list->size () != expression_list->size ())
      {
        er.expected_n_expressions (node.location, identifier_list->size (), expression_list->size ());
      }

    const size_t limit = std::min (identifier_list->size (), expression_list->size ());

    // Enter each symbol.
    size_t idx = 0;
    for (List::ConstIterator id_pos = identifier_list->begin (),
         init_pos = expression_list->begin ();
         idx != limit;
         ++id_pos, ++init_pos, ++idx)
      {
        const Identifier* id = node_cast<Identifier> (*id_pos);
        check_already_declared (id);
        Constant* symbol = new Constant (id->identifier, id->location, node.type, *init_pos);
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

void enter_identifiers (ast::Node* root, util::ErrorReporter& er, decl::SymbolTable& symbol_table, bool exported_symbols)
{
  visitor v (er, symbol_table);
  root->accept (v);
}

}
