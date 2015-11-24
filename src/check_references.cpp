#include "check_references.hpp"

#include "AstVisitor.hpp"
#include "SymbolVisitor.hpp"

namespace semantic {

  using namespace ast;

  namespace {
  // Determine whether an expression represents a value, reference, or type.
  struct ValueReferenceTypeVisitor : public ast::DefaultVisitor {
    void default_action (Node& node) {
      ast_not_reached (node);
    }

    void visit (ast_call_expr_t& node) {
      node.VisitChildren (*this);
      node.expression_kind = kValue;
    }

    void visit (ast_list_expr_t& node) {
      node.VisitChildren (*this);
    }

    void visit (ast_literal_expr_t& node) {
      node.expression_kind = kValue;
    }

    void visit (ast_identifier_expr_t& node) {
      struct visitor : public ConstSymbolVisitor
      {
        ast_identifier_expr_t& node;

        visitor (ast_identifier_expr_t& n)
          : node (n)
        { }

        void defaultAction (const Symbol& symbol)
        {
          not_reached;
        }

        void visit (const BuiltinFunction& symbol)
        {
          unimplemented;
        }

        void visit (const ::Template& symbol)
        {
          node.expression_kind = kValue;
        }

        void visit (const ::Function& symbol)
        {
          unimplemented;
        }

        void visit (const ParameterSymbol& symbol)
        {
          node.expression_kind = kReference;
        }

        void visit (const TypeSymbol& symbol)
        {
          unimplemented;
        }

        void visit (const TypedConstantSymbol& symbol)
        {
          unimplemented;
        }

        void visit (const VariableSymbol& symbol)
        {
          unimplemented;
        }

        void visit (const HiddenSymbol& symbol)
        {
          unimplemented;
        }
      };
      visitor v (node);
      node.symbol->accept (v);
    }

    void visit (SourceFile& node) {
      node.VisitChildren (*this);
    }

    void visit (ast::Type& node) {
      // Do nothing.
    }

    void visit (ast_initializer_t& node) {
      node.body ()->Accept (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      node.VisitChildren (*this);
    }

    void visit (ast_expression_statement_t& node)
    {
      node.VisitChildren (*this);
    }

    void visit (ast_instance_t& node)
    {
      node.expression_list ()->Accept (*this);
    }
  };

  }

  void check_references (ast::Node* root)
  {
    ValueReferenceTypeVisitor visitor;
    root->Accept (visitor);
  }
}
