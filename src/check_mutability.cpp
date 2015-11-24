#include "check_mutability.hpp"

#include <error.h>

#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolVisitor.hpp"

namespace semantic {

  using namespace ast;

  namespace {
  void check_mutability_arguments (ast::Node* node, const Type::Signature* signature)
  {
    ast_list_expr_t* args = ast_cast<ast_list_expr_t> (node);

    size_t i = 0;
    for (Node::ConstIterator pos = args->Begin (), limit = args->End ();
         pos != limit;
         ++pos, ++i) {
      const Type::Type* arg = (*pos)->type;
      if (type_contains_pointer (arg)) {
        if (signature->At (i)->dereference_mutability < (*pos)->dereference_mutability) {
          error_at_line (-1, 0, (*pos)->location.File.c_str (), (*pos)->location.Line,
                         "call casts aways +const or +foreign (E30)");
        }
      }
    }
  }

  // Compute and check the intrinsic and dereference mutability of the expression.
  struct MutabilityVisitor : public ast::DefaultVisitor {
    void default_action (Node& node) {
      ast_not_reached (node);
    }

    void visit (ast_call_expr_t& node) {
      node.VisitChildren (*this);
      check_mutability_arguments (node.args (), node.signature);
      node.intrinsic_mutability = IMMUTABLE;
      node.dereference_mutability = node.return_parameter->dereference_mutability;
    }

    void visit (ast_list_expr_t& node) {
      node.VisitChildren (*this);
    }

    void visit (ast_literal_expr_t& node) {
      node.intrinsic_mutability = IMMUTABLE;
      node.dereference_mutability = IMMUTABLE;
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
          node.intrinsic_mutability = IMMUTABLE;
          node.dereference_mutability = IMMUTABLE;
        }

        void visit (const ::Function& symbol)
        {
          unimplemented;
        }

        void visit (const ParameterSymbol& symbol)
        {
          node.intrinsic_mutability = symbol.intrinsic_mutability;
          node.dereference_mutability = symbol.dereference_mutability;
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

    void visit (ast_list_statement_t& node) {
      node.VisitChildren (*this);
    }

    void visit (ast_expression_statement_t& node) {
      node.VisitChildren (*this);
    }

    void visit (ast_instance_t& node) {
      node.expression_list ()->Accept (*this);
      check_mutability_arguments (node.expression_list (), node.symbol->initializer->initializerType->signature);
    }
  };

  }

  void check_mutability (ast::Node* root)
  {
    MutabilityVisitor visitor;
    root->Accept (visitor);
  }
}
