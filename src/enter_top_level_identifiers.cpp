#include "node_visitor.hpp"
#include "node.hpp"
#include "scope.hpp"
#include "error_reporter.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "node_cast.hpp"
#include "callable.hpp"
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
           Scope* a_package_scope,
           Scope* a_file_scope)
    : er (a_er)
    , package_scope (a_package_scope)
    , file_scope (a_file_scope)
  { }

  ErrorReporter& er;
  Scope* package_scope;
  Scope* file_scope;

  void check_already_declared (const std::string& identifier, const source::Location& location)
  {
    Symbol* s;

    s = package_scope->find_local_symbol (identifier);
    if (s)
      {
        er.already_declared (location, identifier, s->identifier.location ());
        return;
      }

    s = file_scope->find_local_symbol (identifier);
    if (s)
      {
        er.already_declared (location, identifier, s->identifier.location ());
        return;
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
    ast::List::ConstIterator init_pos;
    for (id_pos = identifier_list->begin (),
         init_pos = expression_list->begin ();
         idx != limit;
         ++id_pos, ++init_pos, ++idx)
      {
        const Identifier& id = *id_pos;
        check_already_declared (id.identifier (), id.location ());
        Constant* symbol = new Constant (id, node.type, *init_pos);
        package_scope->enter_symbol (symbol);
        node.symbols.push_back (symbol);
      }
  }

  void visit (TypeDecl& node)
  {
    check_already_declared (node.identifier.identifier (), node.identifier.location ());
    node.symbol = new NamedType (&node);
    package_scope->enter_symbol (node.symbol);
  }

  void visit (FunctionDecl& node)
  {
    check_already_declared (node.identifier.identifier (), node.identifier.location ());
    node.symbol = new decl::Function (&node);
    package_scope->enter_symbol (node.symbol);
  }

  void visit (InstanceDecl& node)
  {
    check_already_declared (node.identifier.identifier (), node.identifier.location ());
    node.symbol = new decl::Instance (&node);
    package_scope->enter_symbol (node.symbol);
  }
};
}

void enter_file_identifiers (ast::Node* root, util::ErrorReporter& er, decl::Scope* package_scope, decl::Scope* file_scope)
{
  visitor v (er, package_scope, file_scope);
  root->accept (v);
}

}
