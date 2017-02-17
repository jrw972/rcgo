#include "node_visitor.hpp"
#include "node.hpp"
#include "scope.hpp"
#include "error_reporter.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "node_cast.hpp"
#include "callable.hpp"
#include "symbol_cast.hpp"
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

struct visitor : public DefaultNodeVisitor
{
  visitor (ErrorReporter& a_er,
           SymbolTable& a_symbol_table)
    : er (a_er)
    , symbol_table (a_symbol_table)
  { }

  ErrorReporter& er;
  SymbolTable& symbol_table;

  NamedType*
  process_receiver (ast::Receiver* node)
  {
    decl::Symbol* symbol = symbol_table.retrieve_symbol (node->type_identifier);
    if (symbol == NULL)
      {
        er.not_declared (node->type_identifier);
        return NULL;
      }

    NamedType* type = decl::symbol_cast<NamedType> (symbol);

    if (type == NULL)
      {
        er.expected_a_type (node->type_identifier.location ());
        return NULL;
      }

    return type;
  }

  void visit (MethodDecl& node)
  {
    NamedType* type = process_receiver (node.receiver);
    if (type == NULL)
      {
        return;
      }
    decl::Method* method = new decl::Method (&node, type);
    type->insert_method (method);
    node.method = method;
  }

  void visit (InitializerDecl& node)
  {
    NamedType* type = process_receiver (node.receiver);
    if (type == NULL)
      {
        return;
      }
    decl::Initializer* initializer = new decl::Initializer (&node, type);
    type->insert_initializer (initializer);
    node.initializer = initializer;
  }

  void visit (GetterDecl& node)
  {
    NamedType* type = process_receiver (node.receiver);
    if (type == NULL)
      {
        return;
      }
    decl::Getter* getter = new decl::Getter (&node, type);
    type->insert_getter (getter);
    node.getter = getter;
  }

  void visit (ActionDecl& node)
  {
    NamedType* type = process_receiver (node.receiver);
    if (type == NULL)
      {
        return;
      }
    decl::Action* action = new decl::Action (&node, type);
    type->insert_action (action);
    node.action = action;
  }

  void visit (ReactionDecl& node)
  {
    NamedType* type = process_receiver (node.receiver);
    if (type == NULL)
      {
        return;
      }
    decl::Reaction* reaction = new decl::Reaction (&node, type);
    type->insert_reaction (reaction);
    node.reaction = reaction;
  }

  void visit (BinderDecl& node)
  {
    NamedType* type = process_receiver (node.receiver);
    if (type == NULL)
      {
        return;
      }
    decl::Bind* bind = new decl::Bind (&node, type);
    type->insert_bind (bind);
    node.bind = bind;
  }
};
}

void enter_method_identifiers (ast::Node* root, util::ErrorReporter& er, decl::SymbolTable& symbol_table)
{
  visitor v (er, symbol_table);
  root->accept (v);
}

}
