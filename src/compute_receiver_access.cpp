#include "compute_receiver_access.hpp"

#include "ast.hpp"
#include "ast_visitor.hpp"
#include "symbol_visitor.hpp"
#include "action.hpp"
#include "reaction.hpp"

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
      node.receiver_access = std::max (node.receiver_access, (*pos)->receiver_access);
    }
}

struct Visitor : public ast::DefaultVisitor
{
  void default_action (Node& node)
  {
    AST_NOT_REACHED (node);
  }

  void visit (AddressOfExpr& node)
  {
    node.child->accept (*this);
    node.receiver_state = node.child->receiver_state;
    node.receiver_access = node.child->receiver_access;
  }

  void visit (ConversionExpr& node)
  {
    node.expr->accept (*this);
    node.receiver_state = node.expr->receiver_state;
    node.receiver_access = node.expr->receiver_access;
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
    node.getter->immutable_phase_access = node.body->receiver_access;
  }

  void visit (ast::Action& node)
  {
    node.precondition->accept (*this);
    node.body->accept (*this);
    node.action->precondition_access = node.precondition->receiver_access;
    node.action->immutable_phase_access = node.body->receiver_access;
  }

  void visit (DimensionedAction& node)
  {
    node.precondition->accept (*this);
    node.body->accept (*this);
    node.action->precondition_access = node.precondition->receiver_access;
    node.action->immutable_phase_access = node.body->receiver_access;
  }

  void visit (ast::Reaction& node)
  {
    node.body->accept (*this);
    node.reaction->immutable_phase_access = node.body->receiver_access;
  }

  void visit (DimensionedReaction& node)
  {
    node.body->accept (*this);
    node.reaction->immutable_phase_access = node.body->receiver_access;
  }

  void visit (Bind& node)
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
    node.receiver_access = AccessNone;
    process_list (node, &node);
  }

  void visit (ExpressionStatement& node)
  {
    node.visit_children (*this);
    node.receiver_access = node.child->receiver_access;
  }

  void visit (VarStatement& node)
  {
    node.expression_list->accept (*this);
    node.receiver_access = AccessNone;
    process_list (node, node.expression_list);
  }

  void visit (EmptyStatement& node)
  {
    node.receiver_access = AccessNone;
  }

  void visit (IfStatement& node)
  {
    node.visit_children (*this);
    node.receiver_access = node.statement->receiver_access;
    node.receiver_access = std::max (node.receiver_access, node.true_branch->receiver_access);
    node.receiver_access = std::max (node.receiver_access, node.false_branch->receiver_access);
  }

  void visit (AssignStatement& node)
  {
    node.visit_children (*this);
    // Straight write.
    if (node.left->receiver_state)
      {
        node.receiver_access = AccessWrite;
        return;
      }
    // Check if a mutable pointer escapes.
    // Consevatively assume that is is written.
    if (node.right->receiver_state &&
        type_contains_pointer (node.right->type) &&
        node.right->indirection_mutability == Mutable)
      {
        node.receiver_access = AccessWrite;
        return;
      }
    node.receiver_access = std::max (node.left->receiver_access, node.right->receiver_access);
  }

  void visit (IncrementStatement& node)
  {
    node.visit_children (*this);
    node.receiver_access = node.child->receiver_access;
    if (node.child->receiver_state)
      {
        node.receiver_access = AccessWrite;
      }
  }

  void visit (ReturnStatement& node)
  {
    node.visit_children (*this);
    node.receiver_access = node.child->receiver_access;
  }

  void visit (ActivateStatement& node)
  {
    node.visit_children (*this);
    node.receiver_access = node.expr_list->receiver_access;
    node.mutable_phase_access = node.body->receiver_access;
  }

  void visit (ChangeStatement& node)
  {
    node.expr->accept (*this);
    node.body->accept (*this);
    node.receiver_access = std::max (node.expr->receiver_access, node.body->receiver_access);
  }

  void visit (CallExpr& node)
  {
    if (node.expr->expression_kind == kType)
      {
        // Conversion.
        node.args->accept (*this);
        node.receiver_access = node.args->at (0)->receiver_access;
        node.receiver_state = node.args->at (0)->receiver_state;
        return;
      }

    node.visit_children (*this);

    // Check if a mutable pointer goes into a function.
    bool flag = false;
    if (node.callable != NULL)
      {
        node.callable->compute_receiver_access (node.args, node.receiver_access, flag);
      }
    else
      {
        compute_receiver_access_arguments (node.args, node.signature, node.receiver_access, flag);
      }

    // Extend the check to the receiver if invoking a method.
    node.receiver_access = std::max (node.receiver_access, node.expr->receiver_access);
    if (node.method_type != NULL &&
        node.expr->receiver_state &&
        type_contains_pointer (node.method_type->receiver_type ()) &&
        node.method_type->receiver_parameter->dereference_mutability == Mutable)
      {
        node.receiver_access = AccessWrite;
      }

    // Check if a mutable pointer containing receiver state is returned.
    if (type_contains_pointer (node.type) &&
        node.indirection_mutability == Mutable)
      {
        node.receiver_state = flag;
      }
    else
      {
        node.receiver_state = false;
      }
  }

  void visit (IdentifierExpr& node)
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

  void visit (ListExpr& node)
  {
    node.visit_children (*this);
    node.receiver_access = AccessNone;
    process_list (node, &node);
  }

  void visit (LiteralExpr& node)
  {
    node.receiver_state = false;
    node.receiver_access = AccessNone;
  }

  void visit (UnaryArithmeticExpr& node)
  {
    node.visit_children (*this);
    node.receiver_state = false;
    node.receiver_access = node.child->receiver_access;
  }

  void visit (BinaryArithmeticExpr& node)
  {
    node.visit_children (*this);
    node.receiver_state = false;
    node.receiver_access = std::max (node.left->receiver_access,
                                     node.right->receiver_access);
  }

  void visit (Instance& node)
  {
    node.expression_list->accept (*this);
  }

  void visit (SelectExpr& node)
  {
    node.base->accept (*this);
    node.receiver_state = node.base->receiver_state;
    node.receiver_access = node.base->receiver_access;
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
    node.receiver_state = node.child->receiver_state;
    node.receiver_access = node.child->receiver_access;
  }

  void visit (IndexExpr& node)
  {
    node.visit_children (*this);
    node.receiver_state = node.base->receiver_state;
    node.receiver_access = std::max (node.base->receiver_access, node.index->receiver_access);
  }

  void visit (SliceExpr& node)
  {
    node.base->accept (*this);
    node.receiver_state = node.base->receiver_state;
    node.receiver_access = node.base->receiver_access;
    if (node.low_present)
      {
        node.low->accept (*this);
        node.receiver_access = std::max (node.receiver_access, node.low->receiver_access);
      }
    if (node.high_present)
      {
        node.high->accept (*this);
        node.receiver_access = std::max (node.receiver_access, node.high->receiver_access);
      }
    if (node.max_present)
      {
        node.max->accept (*this);
        node.receiver_access = std::max (node.receiver_access, node.max->receiver_access);
      }
  }
};
}

void compute_receiver_access_arguments (List* args, const type::Signature* signature, ReceiverAccess& receiver_access, bool& flag)
{
  // Check if a mutable pointer escapes.
  receiver_access = AccessNone;
  flag = false;
  size_t i = 0;
  for (List::ConstIterator pos = args->begin (),
       limit = args->end ();
       pos != limit;
       ++pos)
    {
      Node* arg = *pos;
      ParameterSymbol* param = signature->At (i);
      receiver_access = std::max (receiver_access, arg->receiver_access);
      if (arg->receiver_state &&
          type_contains_pointer (param->type) &&
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
