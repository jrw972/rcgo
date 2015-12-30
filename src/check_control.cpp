#include "check_control.hpp"

#include <error.h>

#include "ast.hpp"
#include "ast_visitor.hpp"

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
    AST_NOT_REACHED (node);
  }

  void visit (SourceFile& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::Type& node)
  {
    // Do nothing.
  }

  void visit (Instance& node)
  {
    node.expression_list->accept (*this);
  }

  void visit (Const& node)
  {
    // Do nothing.
  }

  void visit (ast::Initializer& node)
  {
    Visitor v (*this);
    v.context = Initializer;
    node.body->accept (v);
  }

  void visit (ast::Getter& node)
  {
    Visitor v (*this);
    v.context = Getter;
    node.body->accept (v);
  }

  void visit (ast::Action& node)
  {
    Visitor v (*this);
    v.context = Action;
    node.precondition->accept (v);
    node.body->accept (v);
  }

  void visit (DimensionedAction& node)
  {
    Visitor v (*this);
    v.context = Action;
    node.precondition->accept (v);
    node.body->accept (v);
  }

  void visit (ast::Reaction& node)
  {
    Visitor v (*this);
    v.context = Reaction;
    node.body->accept (v);
  }

  void visit (DimensionedReaction& node)
  {
    Visitor v (*this);
    v.context = Reaction;
    node.body->accept (v);
  }

  void visit (Bind& node)
  {
    // Do nothing.
  }

  void visit (Function& node)
  {
    node.body->accept (*this);
  }

  void visit (Method& node)
  {
    node.body->accept (*this);
  }

  void visit (ListStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (ExpressionStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (ReturnStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (IfStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (WhileStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (ForIotaStatement& node)
  {
    node.body->accept (*this);
  }

  void visit (VarStatement& node)
  {
    node.expression_list->accept (*this);
  }

  void visit (EmptyStatement& node)
  {
    // Do nothing.
  }

  void visit (AssignStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (AddAssignStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (IncrementStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (ChangeStatement& node)
  {
    node.expr->accept (*this);
    node.body->accept (*this);
  }

  void visit (ActivateStatement& node)
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

    node.expr_list->accept (*this);
    Visitor v (*this);
    v.in_mutable_phase = true;
    node.body->accept (v);
    node.in_action = context == Action;
  }

  void visit (CallExpr& node)
  {
    node.visit_children (*this);
    if (node.function_type)
      {
        switch (node.function_type->function_kind)
          {
          case type::Function::FUNCTION:
            // No restrictions on caller.
            break;

          case type::Function::PUSH_PORT:
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "push ports cannot be called (E202)");
            break;

          case type::Function::PULL_PORT:
            // Must be called from either a getter, an action, or reaction.
            if (!(context == Getter ||
                  context == Action ||
                  context == Reaction))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "pull ports may only be called from a getter, an action, or a reaction (E201)");
              }
            if (in_mutable_phase)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "cannot call pull port in mutable section (E198)");
              }
            break;
          }
        return;
      }

    if (node.method_type)
      {
        switch (node.method_type->method_kind)
          {
          case type::Method::METHOD:
            // No restrictions on caller.
            break;
          case type::Method::INITIALIZER:
            // Caller must be an initializer.
            if (context != Initializer)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "initializers may only be called from initializers (E197)");
              }
            break;
          case type::Method::GETTER:
          {
            // Must be called from either a getter, action, reaction, or initializer.
            if (!(context == Getter ||
                  context == Action ||
                  context == Reaction ||
                  context == Initializer))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "getters may only be called from a getter, an action, a reaction, or an initializer (E196)");
              }
            if (in_mutable_phase)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "cannot call getter in mutable section (E34)");
              }
          }
          break;
          case type::Method::REACTION:
          {
            UNIMPLEMENTED;
          }
          break;
          }
        return;
      }

    NOT_REACHED;
  }

  void visit (ConversionExpr& node)
  {
    node.expr->accept (*this);
  }

  void visit (IdentifierExpr& node)
  {
    // Do nothing.
  }

  void visit (ListExpr& node)
  {
    node.visit_children (*this);
  }

  void visit (LiteralExpr& node)
  {
    // Do nothing.
  }

  void visit (DereferenceExpr& node)
  {
    node.visit_children (*this);
  }

  void visit (AddressOfExpr& node)
  {
    node.visit_children (*this);
  }

  void visit (SelectExpr& node)
  {
    node.base->accept (*this);
  }

  void visit (IndexExpr& node)
  {
    node.visit_children (*this);
  }

  void visit (SliceExpr& node)
  {
    node.visit_children (*this);
  }

  void visit (UnaryArithmeticExpr& node)
  {
    node.visit_children (*this);
  }

  void visit (BinaryArithmeticExpr& node)
  {
    node.visit_children (*this);
  }

  void visit (TypeExpression& node)
  {
    // Do nothing.
  }

  void visit (PushPortCallExpr& node)
  {
    node.args->accept (*this);
  }

  void visit (IndexedPushPortCallExpr& node)
  {
    node.index->accept (*this);
    node.args->accept (*this);
  }

  void visit (CompositeLiteral& node)
  {
    node.literal_value->accept (*this);
  }

  void visit (ElementList& node)
  {
    node.visit_children (*this);
  }
};
}

void check_control (ast::Node* root)
{
  Visitor v;
  root->accept (v);
}
}
