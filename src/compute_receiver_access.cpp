#include "compute_receiver_access.hpp"

#include "node.hpp"
#include "node_visitor.hpp"
#include "symbol_visitor.hpp"
#include "symbol_cast.hpp"
#include "action.hpp"
#include "parameter_list.hpp"
#include "semantic.hpp"
#include "callable.hpp"

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

  void visit (AddressOf& node)
  {
    node.child->accept (*this);
    node.eval.receiver_state = node.child->eval.receiver_state;
    node.eval.receiver_access = node.child->eval.receiver_access;
  }

  void visit (Conversion& node)
  {
    node.argument->accept (*this);
    node.eval.receiver_state = node.argument->eval.receiver_state;
    node.eval.receiver_access = node.argument->eval.receiver_access;
  }

  void visit (SourceFile& node)
  {
    node.top_level_decl_list->accept (*this);
  }

  void visit (TopLevelDeclList& node)
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

  void visit (StatementList& node)
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

  void visit (Var& node)
  {
    node.expressions->accept (*this);
    node.eval.receiver_access = AccessNone;
    process_list (node, node.expressions);
  }

  void visit (EmptyStatement& node)
  {
    node.eval.receiver_access = AccessNone;
  }

  void visit (While& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.body->eval.receiver_access;
  }

  void visit (If& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.before->eval.receiver_access;
    node.eval.receiver_access = std::max (node.eval.receiver_access, node.true_body->eval.receiver_access);
    node.eval.receiver_access = std::max (node.eval.receiver_access, node.false_body->eval.receiver_access);
  }

  void visit (AddAssign& node)
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

  void visit (Assign& node)
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

  void visit (IncrementDecrement& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.child->eval.receiver_access;
    if (node.child->eval.receiver_state)
      {
        node.eval.receiver_access = AccessWrite;
      }
  }

  void visit (Return& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.child->eval.receiver_access;
  }

  void visit (Activate& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = node.arguments->eval.receiver_access;
    node.mutable_phase_access = node.body->eval.receiver_access;
  }

  void visit (Change& node)
  {
    node.argument->accept (*this);
    node.body->accept (*this);
    node.eval.receiver_access = std::max (node.argument->eval.receiver_access, node.body->eval.receiver_access);
  }

  void visit (Call& node)
  {
    assert (node.expression->eval.expression_kind != UnknownExpressionKind);
    if (node.expression->eval.expression_kind == TypeExpressionKind)
      {
        // Conversion.
        node.arguments->accept (*this);
        node.eval.receiver_access = node.arguments->at (0)->eval.receiver_access;
        node.eval.receiver_state = node.arguments->at (0)->eval.receiver_state;
        return;
      }

    node.visit_children (*this);

    ExpressionValueList evals = semantic::collect_evals (node.arguments);

    // Check if a mutable pointer goes into a function.
    bool flag = false;
    if (node.polymorphic_function != NULL)
      {
        node.polymorphic_function->compute_receiver_access (evals, node.eval.receiver_access, flag);
      }
    else if (node.callable != NULL)
      {
        node.callable->compute_receiver_access (evals, node.eval.receiver_access, flag);
      }
    else
      {
        compute_receiver_access_arguments (evals, node.parameters, node.eval.receiver_access, flag);
      }

    // Extend the check to the receiver if invoking a method.
    node.eval.receiver_access = std::max (node.eval.receiver_access, node.expression->eval.receiver_access);
    if (node.method_type != NULL &&
        node.expression->eval.receiver_state &&
        node.method_type->receiver_parameter->type->contains_pointer () &&
        node.method_type->receiver_parameter->indirection_mutability == Mutable)
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

  void visit (IdentifierExpression& node)
  {
    node.eval.receiver_state = false;
    node.eval.receiver_access = AccessNone;
    Parameter* parameter = symbol_cast<Parameter> (node.symbol);
    if (parameter != NULL && (parameter->kind == Parameter::Receiver || parameter->kind == Parameter::Receiver_Duplicate))
      {
        node.eval.receiver_state = true;
        node.eval.receiver_access = AccessRead;
      }
  }

  void visit (ExpressionList& node)
  {
    node.visit_children (*this);
    node.eval.receiver_access = AccessNone;
    process_list (node, &node);
  }

  void visit (Literal& node)
  {
    node.eval.receiver_state = false;
    node.eval.receiver_access = AccessNone;
  }

  void visit (ast::UnaryArithmetic& node)
  {
    node.visit_children (*this);
    node.eval.receiver_state = false;
    node.eval.receiver_access = node.child->eval.receiver_access;
  }

  void visit (ast::BinaryArithmetic& node)
  {
    node.visit_children (*this);
    node.eval.receiver_state = false;
    node.eval.receiver_access = std::max (node.left->eval.receiver_access,
                                          node.right->eval.receiver_access);
  }

  void visit (ast::Instance& node)
  {
    node.arguments->accept (*this);
  }

  void visit (Select& node)
  {
    node.base->accept (*this);
    node.eval.receiver_state = node.base->eval.receiver_state;
    node.eval.receiver_access = node.base->eval.receiver_access;
  }

  void visit (PushPortCall& node)
  {
    node.arguments->accept (*this);
    process_list (node, node.arguments);
  }

  void visit (IndexedPushPortCall& node)
  {
    node.index->accept (*this);
    node.arguments->accept (*this);
    process_list (node, node.arguments);
  }

  void visit (TypeExpression& node)
  {
    // Do nothing.
  }

  void visit (Dereference& node)
  {
    node.visit_children (*this);
    node.eval.receiver_state = node.child->eval.receiver_state;
    node.eval.receiver_access = node.child->eval.receiver_access;
  }

  void visit (Index& node)
  {
    node.visit_children (*this);
    node.eval.receiver_state = node.base->eval.receiver_state;
    node.eval.receiver_access = std::max (node.base->eval.receiver_access, node.index->eval.receiver_access);
  }

  void visit (IndexSlice& node)
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
      Parameter* param = signature->at (i);
      receiver_access = std::max (receiver_access, pos->receiver_access);
      if (pos->receiver_state &&
          param->type->contains_pointer () &&
          param->indirection_mutability == Mutable)
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
