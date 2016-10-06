#include "process_top_level_declarations.hpp"
#include "node_visitor.hpp"
#include "node.hpp"
#include "type.hpp"
#include "semantic.hpp"
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
  Scope* file_scope;

  visitor (ErrorReporter& a_er,
           Scope* a_file_scope)
    : er (a_er)
    , file_scope (a_file_scope)
  { }

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
    for (ConstDecl::SymbolsType::const_iterator pos = node.symbols.begin (),
         limit = node.symbols.end ();
         pos != limit;
         ++pos)
      {
        (*pos)->process_declaration (er, file_scope);
      }
  }

  void visit (TypeDecl& node)
  {
    if (!node.symbol) return;
    node.symbol->process_declaration (er, file_scope);
  }

  void visit (FunctionDecl& node)
  {
    if (!node.symbol) return;
    node.symbol->process_declaration (er, file_scope);
  }

  void visit (InstanceDecl& node)
  {
    if (!node.symbol) return;
    node.symbol->process_declaration (er, file_scope);
  }
};

}

void process_top_level_declarations (ast::Node* root, util::ErrorReporter& er, decl::Scope* file_scope)
{
  visitor v (er, file_scope);
  root->accept (v);
}
}
