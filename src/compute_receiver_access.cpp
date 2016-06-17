#include "compute_receiver_access.hpp"

#include "node.hpp"
#include "node_visitor.hpp"
#include "symbol_visitor.hpp"
#include "symbol_cast.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "parameter_list.hpp"
#include "semantic.hpp"

namespace semantic
{

using namespace ast;
using namespace decl;

namespace
{

void process_list (Node& node, List* list)
{
  for (List::ConstIterator pos = list->begin (), limit = list->end ();
       pos != limit;
       ++pos)
    {
      node.eval.receiver_access = std::max (node.eval.receiver_access, (*pos)->eval.receiver_access);
    }
}

struct Visitor : public ast::DefaultNodeVisitor
{
  void default_action (Node& node)
  {
    AST_NOT_REACHED (node);
  }

  void visit (AddressOfExpr& node)
  {
    node.child->accept (*this);
    node.eval.receiver_state = node.child->eval.receiver_state;
    node.eval.receiver_access = node.child->eval.receiver_access;
  }

  void visit (ConversionExpr& node)
  {
    node.expr->accept (*this);
    node.eval.receiver_state = node.expr->eval.receiver_state;
    node.eval.receiver_access = node.expr->eval.receiver_access;
  }

  void visit (SourceFile& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::Type& node)
  {
    // Do nothing.
  }

  void visit (Const& node)
  {
    // Do nothing.
  }

  void visit (ast::Initializer& node)
  {
    // Do nothing.
  }

  void visit (ast::Getter& node)
  {
    node.body->accept (*this);
    node.getter->immutable_phase_access = node.body->eval.receiver_access;
  }

  void visit (ast::Action& node)
  {
    node.precondition->accept (*this);
    node.body->accept (*this);
    node.action->precondition_access = node.precondition->eval.receiver_access;
    node.action->immutable_phase_access = node.body->eval.receiver_access;
  }

  void visit (DimensionedAction& node)
  {
    node.precondition->accept (*this);
    node.body->accept (*this);
    node.action->precondition_access = node.precondition->eval.receiver_access;
    node.action->immutable_phase_access = node.body->eval.receiver_access;
  }

  void visit (ast::Reaction& node)
  {
    node.body->accept (*this);
    node.reaction->immutable_phase_access = node.body->eval.receiver_access;
  }

  void visit (DimensionedReaction& node)
  {
    node.body->accept (*this);
    node.reaction->immutable_phase_access = node.body->eval.receiver_access;
  }

  void visit (ast::Bind& node)
  {
    // Do nothing.
  }

  void visit (ast::Function& node)
  {
    // Do nothing.
  }

  void visit (ast::Method& node)
  {
    // Do nothing.
  }

  void visit (ListStatement& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = AccessNone;
    process_list (node, &node);
  }

  void visit (ExpressionStatement& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.child->eval.receiver_access;
  }

  void visit (VarStatement& node)
  {
    node.expression_list->accept (*this);
    node.eval.receiver_access = AccessNone;
    process_list (node, node.expression_list);
  }

  void visit (EmptyStatement& node)
  {
    node.eval.receiver_access = AccessNone;
  }

  void visit (WhileStatement& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.body->eval.receiver_access;
  }

  void visit (IfStatement& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.statement->eval.receiver_access;
    node.eval.receiver_access = std::max (node.eval.receiver_access, node.true_branch->eval.receiver_access);
    node.eval.receiver_access = std::max (node.eval.receiver_access, node.false_branch->eval.receiver_access);
  }

  void visit (AddAssignStatement& node)
  {
    node.visit_children (*this);
    // Straight write.
    if (node.left->eval.receiver_state)
      {
        node.eval.receiver_access = AccessWrite;
        return;
      }
    node.eval.receiver_access = std::max (node.left->eval.receiver_access, node.right->eval.receiver_access);
  }

  void visit (AssignStatement& node)
  {
    node.visit_children (*this);
    // Straight write.
    if (node.left->eval.receiver_state)
      {
        node.eval.receiver_access = AccessWrite;
        return;
      }
    // Check if a mutable pointer escapes.
    // Consevatively assume that is is written.
    if (node.right->eval.receiver_state &&
        node.right->eval.type->contains_pointer () &&
        node.right->eval.indirection_mutability == Mutable)
      {
        node.eval.receiver_access = AccessWrite;
        return;
      }
    node.eval.receiver_access = std::max (node.left->eval.receiver_access, node.right->eval.receiver_access);
  }

  void visit (IncrementDecrementStatement& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.child->eval.receiver_access;
    if (node.child->eval.receiver_state)
      {
        node.eval.receiver_access = AccessWrite;
      }
  }

  void visit (ReturnStatement& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.child->eval.receiver_access;
  }

  void visit (ActivateStatement& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.expr_list->eval.receiver_access;
    node.mutable_phase_access = node.body->eval.receiver_access;
  }

  void visit (ChangeStatement& node)
  {
    node.expr->accept (*this);
    node.body->accept (*this);
    node.eval.receiver_access = std::max (node.expr->eval.receiver_access, node.body->eval.receiver_access);
  }

  void visit (CallExpr& node)
  {
    assert (node.expr->eval.expression_kind != UnknownExpressionKind);
    if (node.expr->eval.expression_kind == TypeExpressionKind)
      {
        // Conversion.
        node.args->accept (*this);
        node.eval.receiver_access = node.args->at (0)->eval.receiver_access;
        node.eval.receiver_state = node.args->at (0)->eval.receiver_state;
        return;
      }

    node.visit_children (*this);

    ExpressionValueList evals = semantic::collect_evals (node.args);

    // Check if a mutable pointer goes into a function.
    bool flag = false;
    if (node.temp != NULL)
      {
        node.temp->compute_receiver_access (evals, node.eval.receiver_access, flag);
      }
    else if (node.callable != NULL)
      {
        node.callable->compute_receiver_access (evals, node.eval.receiver_access, flag);
      }
    else
      {
        compute_receiver_access_arguments (evals, node.parameter_list, node.eval.receiver_access, flag);
      }

    // Extend the check to the receiver if invoking a method.
    node.eval.receiver_access = std::max (node.eval.receiver_access, node.expr->eval.receiver_access);
    if (node.method_type != NULL &&
        node.expr->eval.receiver_state &&
        node.method_type->receiver_parameter->type->contains_pointer () &&
        node.method_type->receiver_parameter->dereference_mutability == Mutable)
      {
        node.eval.receiver_access = AccessWrite;
      }

    // Check if a mutable pointer containing receiver state is returned.
    if (node.eval.expression_kind != VoidExpressionKind &&
        node.eval.type->contains_pointer () &&
        node.eval.indirection_mutability == Mutable)
      {
        node.eval.receiver_state = flag;
      }
    else
      {
        node.eval.receiver_state = false;
      }
  }

  void visit (IdentifierExpr& node)
  {
    node.eval.receiver_state = false;
    node.eval.receiver_access = AccessNone;
    ParameterSymbol* parameter = symbol_cast<ParameterSymbol> (node.symbol);
    if (parameter != NULL && (parameter->kind == ParameterSymbol::Receiver || parameter->kind == ParameterSymbol::ReceiverDuplicate))
      {
        node.eval.receiver_state = true;
        node.eval.receiver_access = AccessRead;
      }
  }

  void visit (ListExpr& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = AccessNone;
    process_list (node, &node);
  }

  void visit (LiteralExpr& node)
  {
    node.eval.receiver_state = false;
    node.eval.receiver_access = AccessNone;
  }

  void visit (UnaryArithmeticExpr& node)
  {
    node.visit_children (*this);
    node.eval.receiver_state = false;
    node.eval.receiver_access = node.child->eval.receiver_access;
  }

  void visit (BinaryArithmeticExpr& node)
  {
    node.visit_children (*this);
    node.eval.receiver_state = false;
    node.eval.receiver_access = std::max (node.left->eval.receiver_access,
                                          node.right->eval.receiver_access);
  }

  void visit (Instance& node)
  {
    node.expression_list->accept (*this);
  }

  void visit (SelectExpr& node)
  {
    node.base->accept (*this);
    node.eval.receiver_state = node.base->eval.receiver_state;
    node.eval.receiver_access = node.base->eval.receiver_access;
  }

  void visit (PushPortCallExpr& node)
  {
    node.args->accept (*this);
    process_list (node, node.args);
  }

  void visit (IndexedPushPortCallExpr& node)
  {
    node.index->accept (*this);
    node.args->accept (*this);
    process_list (node, node.args);
  }

  void visit (TypeExpression& node)
  {
    // Do nothing.
  }

  void visit (DereferenceExpr& node)
  {
    node.visit_children (*this);
    node.eval.receiver_state = node.child->eval.receiver_state;
    node.eval.receiver_access = node.child->eval.receiver_access;
  }

  void visit (IndexExpr& node)
  {
    node.visit_children (*this);
    node.eval.receiver_state = node.base->eval.receiver_state;
    node.eval.receiver_access = std::max (node.base->eval.receiver_access, node.index->eval.receiver_access);
  }

  void visit (SliceExpr& node)
  {
    node.base->accept (*this);
    node.eval.receiver_state = node.base->eval.receiver_state;
    node.eval.receiver_access = node.base->eval.receiver_access;
    if (node.low_present)
      {
        node.low->accept (*this);
        node.eval.receiver_access = std::max (node.eval.receiver_access, node.low->eval.receiver_access);
      }
    if (node.high_present)
      {
        node.high->accept (*this);
        node.eval.receiver_access = std::max (node.eval.receiver_access, node.high->eval.receiver_access);
      }
    if (node.max_present)
      {
        node.max->accept (*this);
        node.eval.receiver_access = std::max (node.eval.receiver_access, node.max->eval.receiver_access);
      }
  }
};
}

void
compute_receiver_access_arguments (const ExpressionValueList& args,
                                   const decl::ParameterList* signature,
                                   ReceiverAccess& receiver_access,
                                   bool& flag)
{
  // Check if a mutable pointer escapes.
  receiver_access = AccessNone;
  flag = false;
  size_t i = 0;
  for (ExpressionValueList::const_iterator pos = args.begin (),
       limit = args.end ();
       pos != limit;
       ++pos, ++i)
    {
      ParameterSymbol* param = signature->at (i);
      receiver_access = std::max (receiver_access, pos->receiver_access);
      if (pos->receiver_state &&
          param->type->contains_pointer () &&
          param->dereference_mutability == Mutable)
        {
          receiver_access = std::max (receiver_access, AccessWrite);
          flag = true;
        }
    }
}

void compute_receiver_access (ast::Node* root)
{
  Visitor visitor;
  root->accept (visitor);
}
}
