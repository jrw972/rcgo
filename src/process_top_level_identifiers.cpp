#include "node_visitor.hpp"
#include "node.hpp"
#include "type.hpp"
#include "callable.hpp"
#include "node_cast.hpp"
#include "process_type.hpp"

namespace semantic
{
namespace
{

using namespace ast;
using namespace util;
using namespace decl;
using namespace type;

struct visitor : public DefaultNodeVisitor
{

  ErrorReporter& er;
  SymbolTable& symbol_table;

  visitor (ErrorReporter& a_er,
           SymbolTable& a_symbol_table)
    : er (a_er)
    , symbol_table (a_symbol_table)
  { }

  void visit (ConstDecl& node)
  {
    for (ConstDecl::SymbolsType::const_iterator pos = node.symbols.begin (),
         limit = node.symbols.end ();
         pos != limit;
         ++pos)
      {
        (*pos)->process_declaration (er, symbol_table);
      }
  }

  void visit (TypeDecl& node)
  {
    assert (node.symbol);
    node.symbol->process_declaration (er, symbol_table);
  }

  void visit (FunctionDecl& node)
  {
    assert (node.symbol);
    node.symbol->process_declaration (er, symbol_table);
  }

  void visit (InstanceDecl& node)
  {
    assert (node.symbol);
    node.symbol->process_declaration (er, symbol_table);
  }
};

}

void process_top_level_declarations (ast::Node* root, util::ErrorReporter& er, decl::SymbolTable& symbol_table)
{
  visitor v (er, symbol_table);
  root->accept (v);
}
}
