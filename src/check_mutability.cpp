#include "check_mutability.hpp"

#include <error.h>

#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolVisitor.hpp"

namespace semantic
{

  using namespace ast;

  namespace
  {
    void check_mutability_arguments (ast::Node* node, const Type::Signature* signature)
    {
      ast_list_expr_t* args = ast_cast<ast_list_expr_t> (node);

      size_t i = 0;
      for (Node::ConstIterator pos = args->Begin (), limit = args->End ();
           pos != limit;
           ++pos, ++i)
        {
          const Type::Type* arg = (*pos)->type;
          if (type_contains_pointer (arg))
            {
              if (signature->At (i)->dereference_mutability < (*pos)->dereference_mutability)
                {
                  error_at_line (-1, 0, (*pos)->location.File.c_str (), (*pos)->location.Line,
                                 "argument %zd casts away +const or +foreign (E85)", i + 1);
                }
            }
        }
    }

    // Compute and check the intrinsic and dereference mutability of the expression.
    struct MutabilityVisitor : public ast::DefaultVisitor
    {
      void default_action (Node& node)
      {
        ast_not_reached (node);
      }

      void visit (ast_call_expr_t& node)
      {
        node.VisitChildren (*this);
        check_mutability_arguments (node.args (), node.signature);
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = node.return_parameter->dereference_mutability;
      }

      void visit (ast_list_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_literal_expr_t& node)
      {
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = IMMUTABLE;
      }

      void visit (ast_identifier_expr_t& node)
      {
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
            node.intrinsic_mutability = IMMUTABLE;
            node.dereference_mutability = IMMUTABLE;
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

          void visit (const ConstantSymbol& symbol)
          {
            node.intrinsic_mutability = IMMUTABLE;
            node.dereference_mutability = IMMUTABLE;
          }

          void visit (const VariableSymbol& symbol)
          {
            node.intrinsic_mutability = symbol.intrinsic_mutability;
            node.dereference_mutability = symbol.dereference_mutability;
          }

          void visit (const HiddenSymbol& symbol)
          {
            unimplemented;
          }
        };
        visitor v (node);
        node.symbol->accept (v);
      }

      void visit (SourceFile& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast::Type& node)
      {
        // Do nothing.
      }

      void visit (ast_instance_t& node)
      {
        node.expression_list ()->Accept (*this);
        check_mutability_arguments (node.expression_list (), node.symbol->initializer->initializerType->signature);
      }

      void visit (ast_initializer_t& node)
      {
        node.body ()->Accept (*this);
      }

      void visit (ast_function_t& node)
      {
        node.body ()->Accept (*this);
      }

      void visit (ast_const_t& node)
      {
        // Do nothing.
      }

      void visit (ast_list_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_expression_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_return_statement_t& node)
      {
        node.VisitChildren (*this);

        if (type_contains_pointer (node.child ()->type) &&
            node.return_symbol->dereference_mutability < node.child ()->dereference_mutability)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "return casts away +const or +foreign (E149)");
          }
      }

      void visit (ast_if_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_var_statement_t& node)
      {
        if (!node.expression_list ()->Empty ())
          {
            node.expression_list ()->Accept (*this);
            size_t idx = 0;
            for (Node::ConstIterator pos = node.expression_list ()->Begin (), limit = node.expression_list ()->End ();
                 pos != limit;
                 ++pos, ++idx)
              {
                Node* n = *pos;
                VariableSymbol* symbol = node.symbols[idx];

                if (type_contains_pointer (n->type) &&
                    symbol->dereference_mutability < n->dereference_mutability)
                  {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "assignment casts away +const or +foreign (E149)");
                  }
              }
          }
      }

      void visit (ast_assign_statement_t& node)
      {
        node.VisitChildren (*this);
        if (node.left ()->intrinsic_mutability != MUTABLE)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "target of assignment is not mutable (E86)");
          }

        if (type_contains_pointer (node.right ()->type) &&
            node.left ()->dereference_mutability < node.right ()->dereference_mutability)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "assignment casts away +const or +foreign (E149)");
          }
      }

      void visit (ast_change_statement_t& node)
      {
        node.expr ()->Accept (*this);
        node.root_symbol->dereference_mutability = node.expr ()->dereference_mutability;
        node.body ()->Accept (*this);
      }

      void visit (ast_dereference_expr_t& node)
      {
        node.VisitChildren (*this);
        node.intrinsic_mutability = node.child ()->dereference_mutability;
        node.dereference_mutability = node.child ()->dereference_mutability;
      }

      void visit (ast_address_of_expr_t& node)
      {
        node.VisitChildren (*this);
        node.intrinsic_mutability = node.child ()->intrinsic_mutability;
        node.dereference_mutability = std::max (node.child ()->intrinsic_mutability, node.child ()->dereference_mutability);
      }

      void visit (ast_select_expr_t& node)
      {
        node.base ()->Accept (*this);

        if (type_dereference (node.base ()->type))
          {
            node.intrinsic_mutability = node.base ()->dereference_mutability;
            node.dereference_mutability = node.base ()->dereference_mutability;
          }
        else
          {
            node.intrinsic_mutability = node.base ()->intrinsic_mutability;
            node.dereference_mutability = node.base ()->dereference_mutability;
          }
      }

      void visit (ast_index_expr_t& node)
      {
        node.VisitChildren (*this);
        if (node.array_type != NULL)
          {
            node.intrinsic_mutability = node.base ()->intrinsic_mutability;
            node.dereference_mutability = node.base ()->dereference_mutability;
            return;
          }
        not_reached;
      }

      void visit (ast_unary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = IMMUTABLE;
      }

      void visit (ast_binary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
        node.intrinsic_mutability = IMMUTABLE;
        node.dereference_mutability = IMMUTABLE;
      }

      void visit (TypeExpression& node)
      {
        // Do nothing.
      }
    };

  }

  void check_mutability (ast::Node* root)
  {
    MutabilityVisitor visitor;
    root->Accept (visitor);
  }
}
