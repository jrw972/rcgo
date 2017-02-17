#include "decl.hpp"

#include "node_visitor.hpp"
#include "node.hpp"
#include "symbol_table.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "callable.hpp"
#include "identifier.hpp"
#include "error_reporter.hpp"

namespace decl
{

  namespace
{

  using namespace ast;
  using namespace decl;
  using namespace source;
  using namespace util;

  struct lookup_visitor : public DefaultNodeVisitor
  {
    ErrorReporter& er;
    const SymbolTable& symbol_table;

    lookup_visitor (ErrorReporter& a_er,
                    const SymbolTable& a_symbol_table)
      : er (a_er)
      , symbol_table (a_symbol_table)
    { }

    void default_action (Node& node)
    {
      NODE_NOT_REACHED (node);
    }

    void visit (FieldList& node)
    {
      node.visit_children (*this);
    }

    void visit (Receiver& node)
    {
      node.type_identifier_symbol = symbol_table.retrieve_symbol (node.type_identifier);
      if (node.type_identifier_symbol == NULL)
        {
          er.not_declared (node.type_identifier);
        }
    }

    void visit (ast::ParameterList& node)
    {
      node.visit_children (*this);
    }

    void visit (IdentifierExpression& node)
    {
      node.symbol = symbol_table.retrieve_symbol (node.identifier);
      if (node.symbol == NULL)
        {
          er.not_declared (node.identifier);
        }
    }

    void visit (ExpressionList& node)
    {
      node.visit_children (*this);
    }
  };

  struct generate_and_lookup_visitor : public DefaultNodeVisitor
  {

    ErrorReporter& er;
    SymbolTable& symbol_table;
    lookup_visitor lookup;

    generate_and_lookup_visitor (ErrorReporter& a_er,
             SymbolTable& a_symbol_table)
      : er (a_er)
      , symbol_table (a_symbol_table)
      , lookup (a_er, a_symbol_table)
    { }

    void default_action (Node& node)
    {
      node.visit_children (*this);
    }

    // Declarations.
    void visit (ConstDecl& node)
    {
      UNIMPLEMENTED;
    }

    void visit (TypeDecl& node)
    {
      if (node.symbol == NULL) {
        node.symbol = new type::NamedType (&node);
        if (!symbol_table.check_is_declared_locally (er, node.identifier))
          {
            symbol_table.enter_symbol (node.symbol);
          }
      }
      node.type->accept (lookup);
    }

    void visit (FunctionDecl& node)
    {
      UNIMPLEMENTED;
    }

    void visit (MethodDecl& node)
    {
      UNIMPLEMENTED;
    }

    void visit (InitializerDecl& node)
    {
      node.receiver->accept (lookup);
      node.parameters->accept (lookup);
      node.return_parameters->accept (lookup);
      symbol_table.open_scope ();
      node.receiver->accept (*this);
      node.parameters->accept (*this);
      node.return_parameters->accept (*this);
      node.body->accept (*this);
      symbol_table.close_scope ();
    }

    void visit (GetterDecl& node)
    {
      UNIMPLEMENTED;
    }

    void visit (ActionDecl& node)
    {
      UNIMPLEMENTED;
    }

    void visit (ReactionDecl& node)
    {
      UNIMPLEMENTED;
    }

    void visit (BinderDecl& node)
    {
      UNIMPLEMENTED;
    }

    void visit (InstanceDecl& node)
    {
      node.arguments->accept (lookup);
    }

    void visit (VarDecl& node)
    {
      UNIMPLEMENTED;
    }

    void visit (Receiver& node)
    {
      node.identifier_symbol = Parameter::make_receiver (node.identifier, node.mutability, node.indirection_mutability);
      if (!symbol_table.check_is_declared_locally (er, node.identifier))
        {
          symbol_table.enter_symbol (node.identifier_symbol);
        }
    }

    void visit (ParameterList& node)
    {
      UNIMPLEMENTED;
    }

    // Uses.
    void visit (StatementList& node)
    {
      if (!node.is_body)
        {
          symbol_table.open_scope ();
        }
      node.visit_children (*this);
      if (!node.is_body)
        {
          symbol_table.close_scope ();
        }
    }

    void visit (IdentifierType& node)
    {
      UNIMPLEMENTED;
    }

    void visit (IdentifierExpression& node)
    {
      node.accept (lookup);
    }
  };

}

void
generate_and_lookup_symbols (util::ErrorReporter& er,
                             SymbolTable& symbol_table,
                             ast::Node* node)
{
  generate_and_lookup_visitor v (er, symbol_table);
  node->accept (v);
}

}
