#include "check_control.hpp"

#include <error.h>

#include "AstVisitor.hpp"

namespace semantic
{

  using namespace ast;

  namespace
  {
    struct Visitor : public ast::DefaultVisitor
    {
      enum Context
      {
        Other,
        Action,
        Reaction,
        Initializer,
        Getter,
      };

      Context context;
      bool in_mutable_phase;

      Visitor () : context (Other), in_mutable_phase (false) { }

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

      void visit (ast_instance_t& node)
      {
        node.expression_list ()->Accept (*this);
      }

      void visit (ast_const_t& node)
      {
        // Do nothing.
      }

      void visit (ast_initializer_t& node)
      {
        Visitor v (*this);
        v.context = Initializer;
        node.body ()->Accept (v);
      }

      void visit (ast_action_t& node)
      {
        Visitor v (*this);
        v.context = Action;
        node.precondition ()->Accept (v);
        node.body ()->Accept (v);
      }

      void visit (ast_reaction_t& node)
      {
        Visitor v (*this);
        v.context = Reaction;
        node.body ()->Accept (v);
      }

      void visit (ast_bind_t& node)
      {
        // Do nothing.
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
      }

      void visit (ast_return_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_if_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_var_statement_t& node)
      {
        node.expression_list ()->Accept (*this);
      }

      void visit (ast_assign_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_increment_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_change_statement_t& node)
      {
        node.expr ()->Accept (*this);
        node.body ()->Accept (*this);
      }

      void visit (ast_activate_statement_t& node)
      {
        if (!(context == Action ||
              context == Reaction))
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "activation outside of action or reaction (E53)");
          }

        if (in_mutable_phase)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "activations within activations are not allowed (E54)");
          }

        node.expr_list ()->Accept (*this);
        Visitor v (*this);
        v.in_mutable_phase = true;
        node.body ()->Accept (v);
      }

      void visit (ast_call_expr_t& node)
      {
        node.VisitChildren (*this);
        if (node.function_type)
          {
            switch (node.function_type->function_kind)
              {
              case Type::Function::FUNCTION:
                // No restrictions on caller.
                break;

              case Type::Function::PUSH_PORT:
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "push ports cannot be called (E28)");
                break;

              case Type::Function::PULL_PORT:
                // Must be called from either a getter, an action, or reaction.
                if (!(context == Getter ||
                      context == Action ||
                      context == Reaction))
                  {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "pull ports may only be called from a getter, an action, or a reaction (E29)");
                  }

                unimplemented;
                //TypeCheckCall (node, type.GetSignature (), type.GetReturnParameter ()->value, node.args (), tvlist);
                unimplemented;
                // if (node.GetInMutableSection ())
                //   {
                //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                //                    "cannot call pull port in mutable section (E30)");
                //   }
                break;
              }
          }
        else if (node.method_type)
          {
            switch (node.method_type->method_kind)
              {
              case Type::Method::METHOD:
                // No restrictions on caller.
                break;
              case Type::Method::INITIALIZER:
                // Caller must be an initializer.
                if (context != Initializer)
                  {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "initializers may only be called from initializers (E31)");
                  }
                break;
              case Type::Method::GETTER:
              {
                // Must be called from either a getter, action, reaction, or initializer.
                if (!(context == Getter ||
                      context == Action ||
                      context == Reaction ||
                      context == Initializer))
                  {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "getters may only be called from a getter, an action, a reaction, or an initializer (E32)");
                  }
                unimplemented;
                // if (node.GetInMutableSection ())
                //   {
                //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                //                    "cannot call getter in mutable section (E33)");
                //   }
              }
              break;
              case Type::Method::REACTION:
              {
                unimplemented;
              }
              break;
              }
          }
        else
          {
            not_reached;
          }
      }

      void visit (ast_identifier_expr_t& node)
      {
        // Do nothing.
      }

      void visit (ast_list_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_literal_expr_t& node)
      {
        // Do nothing.
      }

      void visit (ast_dereference_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_address_of_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_select_expr_t& node)
      {
        node.base ()->Accept (*this);
      }

      void visit (ast_index_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_unary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_binary_arithmetic_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (TypeExpression& node)
      {
        // Do nothing.
      }

      void visit (ast_push_port_call_expr_t& node)
      {
        node.args ()->Accept (*this);
      }
    };
  }

  void check_control (ast::Node* root)
  {
    Visitor v;
    root->Accept (v);
  }
}
