#include "check_references.hpp"

#include <error.h>

#include "AstVisitor.hpp"
#include "SymbolVisitor.hpp"

namespace semantic
{

  using namespace ast;

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

    static void require_value_or_variable_list (const Node* node)
    {
      for (Node::ConstIterator pos = node->Begin (), limit = node->End ();
           pos != limit;
           ++pos)
        {
          require_value_or_variable (*pos);
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
        require_value_or_variable_list (node.args ());
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
            unimplemented;
          }

          void visit (const ParameterSymbol& symbol)
          {
            node.expression_kind = kVariable;
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
    };

  }

  void check_references (ast::Node* root)
  {
    ValueReferenceTypeVisitor visitor;
    root->Accept (visitor);
  }
}
