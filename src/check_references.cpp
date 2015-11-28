#include "check_references.hpp"

#include <error.h>

#include "AstVisitor.hpp"
#include "SymbolVisitor.hpp"

namespace semantic
{

  using namespace ast;
  using namespace Type;

  namespace
  {
    static void require_value_or_variable (const Node* node)
    {
      if (!(node->expression_kind == kValue ||
            node->expression_kind == kVariable))
        {
          error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                         "required a value (E78)");
        }
    }

    static void require_variable (const Node* node)
    {
      if (!(node->expression_kind == kVariable))
        {
          error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                         "required a variable (E2)");
        }
    }

    // static void require_variable_list (const Node* node) {
    //   for (Node::ConstIterator pos = node->Begin (), limit = node->End ();
    //        pos != limit;
    //        ++pos) {
    //     require_variable (*pos);
    //   }
    // }

    // Determine whether an expression represents a value, reference, or type.
    struct ValueReferenceTypeVisitor : public ast::DefaultVisitor
    {
      void default_action (Node& node)
      {
        ast_not_reached (node);
      }

      void visit (ast_call_expr_t& node)
      {
        node.VisitChildren (*this);
        require_value_or_variable (node.expr ());
        node.callable->check_references (node.args ());
        node.expression_kind = kValue;
      }

      void visit (ast_list_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_literal_expr_t& node)
      {
        node.expression_kind = kValue;
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
            node.expression_kind = kValue;
          }

          void visit (const ::Function& symbol)
          {
            node.expression_kind = kValue;
          }

          void visit (const ParameterSymbol& symbol)
          {
            node.expression_kind = kVariable;
          }

          void visit (const TypeSymbol& symbol)
          {
            unimplemented;
          }

          void visit (const ConstantSymbol& symbol)
          {
            node.expression_kind = kValue;
          }

          void visit (const VariableSymbol& symbol)
          {
            node.expression_kind = kVariable;
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
        require_value_or_variable_list (node.expression_list ());
      }

      void visit (ast_initializer_t& node)
      {
        node.body ()->Accept (*this);
      }

      void visit (ast_function_t& node)
      {
        node.body ()->Accept (*this);
      }

      void visit (ast_list_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_expression_statement_t& node)
      {
        node.VisitChildren (*this);
        require_value_or_variable (node.child ());
      }

      void visit (ast_return_statement_t& node)
      {
        node.VisitChildren (*this);
        require_value_or_variable (node.child ());
      }

      void visit (ast_if_statement_t& node)
      {
        node.VisitChildren (*this);
        require_value_or_variable (node.condition ());
      }

      void visit (ast_change_statement_t& node)
      {
        node.expr ()->Accept (*this);
        require_value_or_variable (node.expr ());
        node.body ()->Accept (*this);
      }

      void visit (ast_const_t& node)
      {
        // Do nothing.
      }

      void visit (ast_var_statement_t& node)
      {
        node.expression_list ()->Accept (*this);
        require_value_or_variable_list (node.expression_list ());
      }

      void visit (ast_assign_statement_t& node)
      {
        node.VisitChildren (*this);
        require_variable (node.left ());
        require_value_or_variable (node.right ());
      }

      void visit (ast_increment_statement_t& node)
      {
        node.VisitChildren (*this);
        require_variable (node.child ());
      }

      void visit (ast_dereference_expr_t& node)
      {
        node.VisitChildren (*this);
        require_value_or_variable (node.child ());
        node.expression_kind = kVariable;
      }

      void visit (ast_address_of_expr_t& node)
      {
        node.VisitChildren (*this);
        require_variable (node.child ());
        node.expression_kind = kValue;
      }

      void visit (ast_select_expr_t& node)
      {
        node.base ()->Accept (*this);
        require_value_or_variable (node.base ());

        if (type_dereference (node.base ()->type))
          {
            // Selecting through pointer always yields a variable.
            node.expression_kind = kVariable;
          }
        else
          {
            // Otherwise, use the base kind.
            node.expression_kind = node.base ()->expression_kind;
          }
      }

      void visit (ast_index_expr_t& node)
      {
        node.VisitChildren (*this);
        require_value_or_variable (node.base ());
        require_value_or_variable (node.index ());

        if (node.array_type != NULL)
          {
            node.expression_kind = node.base ()->expression_kind;
            return;
          }
        not_reached;
      }

      void visit (ast_unary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
        require_value_or_variable (node.child ());
        node.expression_kind = kValue;
      }

      void visit (ast_binary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
        require_value_or_variable (node.left ());
        require_value_or_variable (node.left ());
        node.expression_kind = kValue;
      }

      void visit (TypeExpression& node)
      {
        node.expression_kind = kType;
      }
    };
  }

  void require_type (const Node* node)
  {
    if (!(node->expression_kind == kType))
      {
        error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                       "required a type (E2)");
      }
  }

  void require_value_or_variable_list (const Node* node)
  {
    for (Node::ConstIterator pos = node->Begin (), limit = node->End ();
         pos != limit;
         ++pos)
      {
        require_value_or_variable (*pos);
      }
  }

  void check_references (ast::Node* root)
  {
    ValueReferenceTypeVisitor visitor;
    root->Accept (visitor);
  }
}
