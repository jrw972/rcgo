#include "enter_top_level_identifiers.hpp"
#include "node_visitor.hpp"
#include "node.hpp"
#include "scope.hpp"
#include "error_reporter.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "node_cast.hpp"
#include "callable.hpp"

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
           Scope* a_package_scope,
           Scope* a_file_scope)
    : er (a_er)
    , package_scope (a_package_scope)
    , file_scope (a_file_scope)
  { }

  ErrorReporter& er;
  Scope* package_scope;
  Scope* file_scope;

  bool already_declared (const Identifier* node)
  {
    Symbol* s;

    s = package_scope->find_local_symbol (node->identifier);
    if (s)
      {
        er.already_declared (node->location, node->identifier, s->location);
        return true;
      }

    s = file_scope->find_local_symbol (node->identifier);
    if (s)
      {
        er.already_declared (node->location, node->identifier, s->location);
        return true;
      }

    return false;
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
        if (!already_declared (id))
          {
            Constant* symbol = new Constant (id->identifier, id->location, node.type, *init_pos);
            package_scope->enter_symbol (symbol);
            node.symbols.push_back (symbol);
          }
      }
  }

  void visit (TypeDecl& node)
  {
    const Identifier* id = node.identifier;
    if (already_declared (id))
      {
        return;
      }
    node.symbol = new NamedType (id->identifier, id->location, &node);
    package_scope->enter_symbol (node.symbol);
  }

  void visit (FunctionDecl& node)
  {
    const Identifier* id = node.identifier;
    if (already_declared (id))
      {
        return;
      }
    node.symbol = new decl::Function (id->identifier, id->location);
    package_scope->enter_symbol (node.symbol);
  }

  void visit (InstanceDecl& node)
  {
    const Identifier* id = node.identifier;
    if (already_declared (id))
      {
        return;
      }
    node.symbol = new decl::Instance (id->identifier, id->location);
    package_scope->enter_symbol (node.symbol);
  }
};
}

void enter_top_level_identifiers (ast::Node* root, util::ErrorReporter& er, decl::Scope* package_scope, decl::Scope* file_scope)
{
  visitor v (er, package_scope, file_scope);
  root->accept (v);
}

}
