#include "compute_receiver_access.hpp"

#include "AstVisitor.hpp"
#include "SymbolVisitor.hpp"

namespace semantic
{

  using namespace ast;

  namespace
  {
    struct Visitor : public ast::DefaultVisitor
    {
      void default_action (Node& node)
      {
        ast_not_reached (node);
      }

      void visit (SourceFile& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast::Type& node)
      {
        // Do nothing.
      }

      void visit (ast_initializer_t& node)
      {
        // Do nothing.
      }

      void visit (ast_function_t& node)
      {
        // Do nothing.
      }

      void visit (ast_list_statement_t& node)
      {
        node.VisitChildren (*this);
        node.receiver_access = AccessNone;
        for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
             pos != limit;
             ++pos)
          {
            node.receiver_access = std::max (node.receiver_access, (*pos)->receiver_access);
          }
      }

      void visit (ast_expression_statement_t& node)
      {
        node.VisitChildren (*this);
        node.receiver_access = node.child ()->receiver_access;
      }

      void visit (ast_var_statement_t& node)
      {
        node.expression_list ()->Accept (*this);
        node.receiver_access = AccessNone;
        for (Node::ConstIterator pos = node.expression_list ()->Begin (), limit = node.expression_list ()->End ();
             pos != limit;
             ++pos)
          {
            node.receiver_access = std::max (node.receiver_access, (*pos)->receiver_access);
          }
      }

      void visit (ast_assign_statement_t& node)
      {
        unimplemented;
        node.VisitChildren (*this);
        if (node.left ()->receiver_access != AccessNone) {
          node.receiver_access = AccessWrite;
          return;
        }
        // Check if a mutable pointer escapes.
        if (node.right ()->receiver_access != AccessNone &&
            type_contains_pointer (node.right ()->type) &&
            node.right ()->dereference_mutability == MUTABLE)
          {
            node.receiver_access = AccessWrite;
            return;
          }
      }

      void visit (ast_call_expr_t& node)
      {
        node.VisitChildren (*this);
        node.receiver_access = node.expr ()->receiver_access;

        // Check if a mutable pointer escapes.
        bool flag = false;
        size_t i = 0;
        for (Node::ConstIterator pos = node.args ()->Begin (),
             limit = node.args ()->End ();
             pos != limit;
             ++pos)
          {
            Node* arg = *pos;
            ParameterSymbol* param = node.signature->At (i);
            node.receiver_access = std::max (node.receiver_access, arg->receiver_access);
            if (arg->receiver_state &&
                type_contains_pointer (param->type) &&
                param->dereference_mutability == MUTABLE)
              {
                node.receiver_access = std::max (node.receiver_access, AccessWrite);
                flag = true;
              }
          }

        if (type_contains_pointer (node.type) &&
            node.dereference_mutability == MUTABLE) {
          node.receiver_state = flag;
        } else {
          node.receiver_state = false;
        }
      }

      void visit (ast_identifier_expr_t& node)
      {
        node.receiver_state = false;
        node.receiver_access = AccessNone;
        ParameterSymbol* parameter = SymbolCast<ParameterSymbol> (node.symbol);
        if (parameter != NULL)
          {
            unimplemented;
          }
      }

      void visit (ast_list_expr_t& node)
      {
        node.VisitChildren (*this);
        node.receiver_access = AccessNone;
        for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
             pos != limit;
             ++pos)
          {
            node.receiver_access = std::max (node.receiver_access, (*pos)->receiver_access);
          }
      }

      void visit (ast_literal_expr_t& node)
      {
        node.receiver_state = false;
        node.receiver_access = AccessNone;
      }

      void visit (ast_unary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
        node.receiver_state = false;
        node.receiver_access = node.child ()->receiver_access;
      }

      void visit (ast_binary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
        node.receiver_state = false;
        node.receiver_access = std::max (node.left ()->receiver_access,
                                         node.right ()->receiver_access);
      }

      void visit (ast_instance_t& node)
      {
        node.expression_list ()->Accept (*this);
      }
    };
  }

  void compute_receiver_access (ast::Node* root)
  {
    Visitor visitor;
    root->Accept (visitor);
  }
}
