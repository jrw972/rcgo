#include "compute_receiver_access.hpp"

#include "AstVisitor.hpp"
#include "SymbolVisitor.hpp"
#include "action.hpp"
#include "reaction.hpp"

namespace semantic
{

  using namespace ast;

  namespace
  {

    void process_list (Node& node, Node* list)
    {
      for (Node::ConstIterator pos = list->Begin (), limit = list->End ();
           pos != limit;
           ++pos)
        {
          node.receiver_access = std::max (node.receiver_access, (*pos)->receiver_access);
        }
    }

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

      void visit (ast_getter_t& node)
      {
        node.body ()->Accept (*this);
        node.getter->immutable_phase_access = node.body ()->receiver_access;
      }

      void visit (ast_action_t& node)
      {
        node.precondition ()->Accept (*this);
        node.body ()->Accept (*this);
        node.action->precondition_access = node.precondition ()->receiver_access;
        node.action->immutable_phase_access = node.body ()->receiver_access;
      }

      void visit (ast_reaction_t& node)
      {
        node.body ()->Accept (*this);
        node.reaction->immutable_phase_access = node.body ()->receiver_access;
      }

      void visit (ast_bind_t& node)
      {
        // Do nothing.
      }

      void visit (ast_function_t& node)
      {
        // Do nothing.
      }

      void visit (ast_method_t& node)
      {
        // Do nothing.
      }

      void visit (ast_list_statement_t& node)
      {
        node.VisitChildren (*this);
        node.receiver_access = AccessNone;
        process_list (node, &node);
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
        process_list (node, node.expression_list ());
      }

      void visit (ast_assign_statement_t& node)
      {
        node.VisitChildren (*this);
        // Straight write.
        if (node.left ()->receiver_state)
          {
            node.receiver_access = AccessWrite;
            return;
          }
        // Check if a mutable pointer escapes.
        // Consevatively assume that is is written.
        if (node.right ()->receiver_state &&
            type_contains_pointer (node.right ()->type) &&
            node.right ()->dereference_mutability == MUTABLE)
          {
            node.receiver_access = AccessWrite;
            return;
          }
        node.receiver_access = std::max (node.left ()->receiver_access, node.right ()->receiver_access);
      }

      void visit (ast_return_statement_t& node)
      {
        node.VisitChildren (*this);
        node.receiver_access = node.child ()->receiver_access;
      }

      void visit (ast_activate_statement_t& node)
      {
        node.VisitChildren (*this);
        process_list (node, &node);
        node.mutable_phase_access = node.body ()->receiver_access;
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
            node.dereference_mutability == MUTABLE)
          {
            node.receiver_state = flag;
          }
        else
          {
            node.receiver_state = false;
          }
      }

      void visit (ast_identifier_expr_t& node)
      {
        node.receiver_state = false;
        node.receiver_access = AccessNone;
        ParameterSymbol* parameter = SymbolCast<ParameterSymbol> (node.symbol);
        if (parameter != NULL && (parameter->kind == ParameterSymbol::Receiver || parameter->kind == ParameterSymbol::ReceiverDuplicate))
          {
            node.receiver_state = true;
            node.receiver_access = AccessRead;
          }
      }

      void visit (ast_list_expr_t& node)
      {
        node.VisitChildren (*this);
        node.receiver_access = AccessNone;
        process_list (node, &node);
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

      void visit (ast_select_expr_t& node)
      {
        node.base ()->Accept (*this);
        node.receiver_state = node.base ()->receiver_state;
        node.receiver_access = node.base ()->receiver_access;
      }

      void visit (ast_push_port_call_expr_t& node)
      {
        node.args ()->Accept (*this);
        process_list (node, node.args ());
      }
    };
  }

  void compute_receiver_access (ast::Node* root)
  {
    Visitor visitor;
    root->Accept (visitor);
  }
}
