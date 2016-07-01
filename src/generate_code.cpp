#include "generate_code.hpp"

#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "runtime.hpp"
#include "callable.hpp"
#include "symbol_visitor.hpp"
#include "field.hpp"
#include "semantic.hpp"
#include "bind.hpp"
#include "action.hpp"
#include "operation.hpp"

namespace  code
{
using namespace ast;
using namespace runtime;
using namespace type;
using namespace semantic;
using namespace decl;

static Operation* load (Node* node, Operation* op)
{
  assert (node->eval.expression_kind != UnknownExpressionKind);
  if (node->eval.expression_kind == VariableExpressionKind)
    {
      return new Load (op, node->eval.type);
    }
  return op;
}

struct CodeGenVisitor : public ast::DefaultNodeVisitor
{
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

  void visit (ast::Instance& node)
  {
    node.expression_list->accept (*this);
    node.operation = new MethodCall (node.symbol->initializer, new runtime::Instance (node.symbol), node.expression_list->operation);
  }

  void visit (Const& node)
  {
    // Do nothing.
  }

  void visit (ast::Initializer& node)
  {
    node.body->accept (*this);
    node.initializer->operation = new SetRestoreCurrentInstance (node.body->operation, node.initializer->memory_model.receiver_offset ());
  }

  void visit (ast::Getter& node)
  {
    node.body->accept (*this);
    node.getter->operation = new SetRestoreCurrentInstance (node.body->operation, node.getter->memory_model.receiver_offset ());
  }

  void visit (ast::Action& node)
  {
    node.precondition->accept (*this);
    Operation* p = node.precondition->operation;
    p = load (node.precondition, p);
    node.precondition->operation = new SetRestoreCurrentInstance (p, node.action->memory_model.receiver_offset ());
    node.body->accept (*this);
    node.body->operation = new SetRestoreCurrentInstance (node.body->operation, node.action->memory_model.receiver_offset ());
  }

  void visit (DimensionedAction& node)
  {
    node.precondition->accept (*this);
    Operation* p = node.precondition->operation;
    p = load (node.precondition, p);
    node.precondition->operation = new SetRestoreCurrentInstance (p, node.action->memory_model.receiver_offset ());
    node.body->accept (*this);
    node.body->operation = new SetRestoreCurrentInstance (node.body->operation, node.action->memory_model.receiver_offset ());
  }

  void visit (ast::Reaction& node)
  {
    node.body->accept (*this);
    node.reaction->operation = new SetRestoreCurrentInstance (node.body->operation, node.reaction->memory_model.receiver_offset ());
  }

  void visit (DimensionedReaction& node)
  {
    node.body->accept (*this);
    node.reaction->operation = new SetRestoreCurrentInstance (node.body->operation, node.reaction->memory_model.receiver_offset ());
  }

  void visit (ast::Bind& node)
  {
    node.body->accept (*this);
  }

  void visit (ast::Function& node)
  {
    node.body->accept (*this);
    node.function->operation = node.body->operation;
  }

  void visit (ast::Method& node)
  {
    node.body->accept (*this);
    node.method->operation = node.body->operation;
  }

  void visit (ListStatement& node)
  {
    node.visit_children (*this);
    ListOperation* op = new ListOperation ();
    for (List::ConstIterator pos = node.begin (), limit = node.end ();
         pos != limit;
         ++pos)
      {
        if ((*pos)->operation != NULL)
          {
            op->list.push_back ((*pos)->operation);
          }
      }
    node.operation = op;
  }

  void visit (ExpressionStatement& node)
  {
    node.visit_children (*this);
    node.operation = node.child->operation;
    // Clean up the stack if necessary.
    if (node.child->eval.expression_kind != VoidExpressionKind &&
        arch::size (node.child->eval.type) != 0)
      {
        node.operation = new Popn (node.operation, arch::size (node.child->eval.type));
      }
  }

  void visit (ReturnStatement& node)
  {
    node.visit_children (*this);
    Operation* c = node.child->operation;
    c = load (node.child, c);
    node.operation = new Return (c, node.return_symbol);
  }

  void visit (IfStatement& node)
  {
    node.visit_children (*this);
    if (node.condition->eval.value.present)
      {
        if (node.condition->eval.value.bool_value)
          {
            node.operation = node.true_branch->operation;
          }
        else
          {
            node.operation = node.false_branch->operation;
          }
      }
    else
      {
        Operation* c = node.condition->operation;
        c = load (node.condition, c);
        node.operation = new If (c, node.true_branch->operation, node.false_branch->operation);
      }
  }

  void visit (WhileStatement& node)
  {
    node.visit_children (*this);
    Operation* c = node.condition->operation;
    c = load (node.condition, c);
    node.operation = new While (c, node.body->operation);
  }

  void visit (ForIotaStatement& node)
  {
    node.body->accept (*this);
    node.operation = new ForIota (node.symbol, node.limit, node.body->operation);
  }

  void visit (VarStatement& node)
  {
    ListOperation* op = new ListOperation ();
    if (node.expression_list->empty ())
      {
        // Clear the variables.
        for (VarStatement::SymbolsType::const_iterator pos = node.symbols.begin (), limit = node.symbols.end ();
             pos != limit;
             ++pos)
          {
            Variable* symbol = *pos;
            op->list.push_back (new Clear (symbol->offset (), arch::size (symbol->type)));
          }
      }
    else
      {
        node.expression_list->accept (*this);
        // Initialize the variables.
        size_t idx = 0;
        for (List::ConstIterator pos = node.expression_list->begin (), limit = node.expression_list->end ();
             pos != limit;
             ++pos, ++idx)
          {
            Variable* symbol = node.symbols[idx];
            Operation* right = (*pos)->operation;
            right = load (*pos, right);
            op->list.push_back (new Assign (new Reference (symbol->offset ()), right, symbol->type));
          }
      }
    node.operation = op;
  }

  void visit (EmptyStatement& node)
  {
    node.operation = new Noop ();
  }

  void visit (AssignStatement& node)
  {
    node.visit_children (*this);
    Operation* left = node.left->operation;
    Operation* right = node.right->operation;
    right = load (node.right, right);
    node.operation = new Assign (left, right, node.left->eval.type);
  }

  void visit (AddAssignStatement& node)
  {
    node.visit_children (*this);
    Operation* left = node.left->operation;
    Operation* right = node.right->operation;
    right = load (node.right, right);
    node.operation = make_add_assign (left, right, node.left->eval.type);
  }

  void visit (IncrementDecrementStatement& node)
  {
    node.visit_children (*this);
    switch (node.kind)
      {
      case IncrementDecrementStatement::Increment:
        node.operation = make_increment (node.child->operation, node.child->eval.type);
        break;
      case IncrementDecrementStatement::Decrement:
        node.operation = make_decrement (node.child->operation, node.child->eval.type);
        break;
      }
  }

  void visit (ChangeStatement& node)
  {
    node.expr->accept (*this);
    node.body->accept (*this);
    Operation* root = node.expr->operation;
    root = load (node.expr, root);
    node.operation = new Change (root, node.root_symbol->offset (), node.body->operation);
  }

  void visit (ActivateStatement& node)
  {
    node.visit_children (*this);
    Operation* b = node.body->operation;
    // Add to the schedule.
    if (node.mutable_phase_access == AccessWrite ||
        (node.in_action && !node.expr_list->empty ()))
      {
        b = new Push (b);
      }
    b = new SetRestoreCurrentInstance (b, node.memory_model->receiver_offset ());
    node.operation = new Activate (node.expr_list->operation, b);
  }

  void visit (BindPushPortStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (BindPushPortParamStatement& node)
  {
    node.visit_children (*this);
  }

  void visit (BindPullPortStatement& node)
  {
    node.visit_children (*this);

    ast::Node* n = node_cast<SelectExpr> (node.right)->base;
    if (n->eval.expression_kind == VariableExpressionKind &&
        n->eval.type->to_pointer ())
      {
        n->operation = load (n, n->operation);
      }
  }

  void visit (CallExpr& node)
  {
    assert (node.expr->eval.expression_kind != UnknownExpressionKind);
    if (node.expr->eval.expression_kind == TypeExpressionKind)
      {
        node.args->at (0)->accept (*this);
        Operation* o = node.args->at (0)->operation;
        o = load (node.args->at (0), o);
        node.operation = make_conversion (o, node.args->at (0)->eval.type, node.eval.type);
        return;
      }

    node.args->accept (*this);

    if (node.temp != NULL)
      {
        ExpressionValueList arguments;
        List* args = node.args;
        for (List::ConstIterator pos = args->begin (), limit = args->end ();
             pos != limit;
             ++pos)
          {
            arguments.push_back ((*pos)->eval);
          }

        node.operation = node.temp->generate_code (node.eval, arguments, static_cast<ListOperation*> (node.args->operation));
      }
    else if (node.callable != NULL)
      {
        if (node.function_type)
          {
            node.operation = new FunctionCall (node.callable, node.args->operation);
          }
        else if (node.method_type || node.initializer_type || node.getter_type || node.reaction_type)
          {
            const type::MethodBase* mb = node.method_type;
            mb = mb ? mb : node.initializer_type;
            mb = mb ? mb : node.getter_type;
            mb = mb ? mb : node.reaction_type;

            node.expr->accept (*this);
            Node* sb = node_cast<SelectExpr> (node.expr)->base;
            if (sb->eval.type->underlying_type ()->to_pointer ())
              {
                if (mb->receiver_parameter->type->underlying_type ()->to_pointer ())
                  {
                    assert (sb->eval.expression_kind != UnknownExpressionKind);
                    if (sb->eval.expression_kind == VariableExpressionKind)
                      {
                        // Got a pointer.  Expecting a pointer.  Load the pointer.
                        node.operation = new MethodCall (node.callable, new Load (sb->operation, sb->eval.type), node.args->operation);
                      }
                    else
                      {
                        // Got a pointer.  Expecting a pointer.  Pointer is alreay loaded.
                        node.operation = new MethodCall (node.callable, sb->operation, node.args->operation);
                      }
                  }
                else
                  {
                    // Got a pointer.  Expecting a value.  Load the variable and then the pointer.
                    assert (sb->eval.expression_kind != UnknownExpressionKind);
                    if (sb->eval.expression_kind == VariableExpressionKind)
                      {
                        node.operation = new MethodCall (node.callable, new Load (new Load (sb->operation, sb->eval.type), mb->receiver_parameter->type), node.args->operation);
                      }
                    else
                      {
                        UNIMPLEMENTED;
                      }
                  }
              }
            else
              {
                if (mb->receiver_parameter->type->underlying_type ()->to_pointer ())
                  {
                    assert (sb->eval.expression_kind != UnknownExpressionKind);
                    if (sb->eval.expression_kind == VariableExpressionKind)
                      {
                        // Got a value.  Expected a pointer.  Use variable as pointer.
                        node.operation = new MethodCall (node.callable, sb->operation, node.args->operation);
                      }
                    else
                      {
                        UNIMPLEMENTED;
                      }
                  }
                else
                  {
                    assert (sb->eval.expression_kind != UnknownExpressionKind);
                    if (sb->eval.expression_kind == VariableExpressionKind)
                      {
                        // Got a value.  Expected a value.  Load the variable.
                        node.operation = new MethodCall (node.callable, new Load (sb->operation, sb->eval.type), node.args->operation);
                      }
                    else
                      {
                        UNIMPLEMENTED;
                      }
                  }
              }
          }
        else
          {
            NOT_REACHED;
          }
      }
    else
      {
        if (node.pull_port_type)
          {
            node.expr->accept (*this);
            Operation* r = node.expr->operation;
            r = load (node.expr, r);
            node.operation = new DynamicPullPortCall (node.pull_port_type, r, node.args->operation);
          }
        else if (node.method_type)
          {
            UNIMPLEMENTED;
          }
        else
          {
            NOT_REACHED;
          }
      }
  }

  void visit (ConversionExpr& node)
  {
    node.expr->accept (*this);
    Operation* o = node.expr->operation;
    o = load (node.expr, o);
    node.operation = make_conversion (o, node.expr->eval.type, node.eval.type);
  }

  void visit (ListExpr& node)
  {
    node.visit_children (*this);
    ListOperation* op = new ListOperation ();
    for (List::ConstIterator pos = node.begin (), limit = node.end ();
         pos != limit;
         ++pos)
      {
        Node* n = *pos;
        Operation* o = n->operation;
        if (o != NULL)
          {
            op->list.push_back (load (n, o));
          }
      }
    node.operation = op;
  }

  void visit (LiteralExpr& node)
  {
    node.operation = make_literal (node.eval.type, node.eval.value);
  }

  void visit (IdentifierExpr& node)
  {
    if (node.eval.value.present)
      {
        node.operation = make_literal (node.eval.type, node.eval.value);
        return;
      }

    struct Visitor : public ConstSymbolVisitor
    {
      IdentifierExpr& node;
      Operation* op;
      Visitor (IdentifierExpr& n) : node (n), op (NULL) { }
      void default_action (const Symbol& s)
      {
        NOT_REACHED;
      }

      void visit (const Parameter& s)
      {
        op = new Reference (s.offset ());
      }

      void visit (const Variable& s)
      {
        op = new Reference (s.offset ());
      }

      void visit (const NamedType& s)
      {
        op = new Noop ();
      }
    };
    Visitor v (node);
    node.symbol->accept (v);
    node.operation = v.op;
  }

  void visit (DereferenceExpr& node)
  {
    node.visit_children (*this);
    node.operation = load (node.child, node.child->operation);
  }

  void visit (AddressOfExpr& node)
  {
    node.visit_children (*this);
    node.operation = node.child->operation;
  }

  void visit (SelectExpr& node)
  {
    node.base->accept (*this);

    if (node.field != NULL)
      {
        if (node.base->eval.type->underlying_type ()->to_pointer ())
          {
            assert (node.base->eval.expression_kind != UnknownExpressionKind);
            if (node.base->eval.expression_kind == VariableExpressionKind)
              {
                node.operation = new Select (new Load (node.base->operation, node.base->eval.type), arch::offset (node.field));
              }
            else
              {
                UNIMPLEMENTED;
              }
          }
        else
          {
            assert (node.base->eval.expression_kind != UnknownExpressionKind);
            if (node.base->eval.expression_kind == VariableExpressionKind)
              {
                node.operation = new Select (node.base->operation, arch::offset (node.field));
              }
            else
              {
                UNIMPLEMENTED;
              }
          }
      }
    else
      {
        assert (node.callable != NULL);
        Value v;
        v.present = true;
        v.pointer_value = const_cast<void*> (static_cast<const void*> (node.callable));
        node.operation = make_literal (Int::instance ()->get_pointer (), v);
      }
  }

  void visit (IndexExpr& node)
  {
    node.visit_children (*this);

    if (node.array_type != NULL)
      {
        Operation* index_op = node.index->operation;
        index_op = load (node.index, index_op);
        index_op = MakeConvertToInt (index_op, node.index->eval.type);
        assert (node.base->eval.expression_kind != UnknownExpressionKind);
        if (node.base->eval.expression_kind == VariableExpressionKind)
          {
            node.operation = new IndexArray (node.location, node.base->operation, index_op, node.array_type);
          }
        else
          {
            UNIMPLEMENTED;
          }

        return;
      }

    if (node.slice_type != NULL)
      {
        Operation* index_op = node.index->operation;
        index_op = load (node.index, index_op);
        index_op = MakeConvertToInt (index_op, node.index->eval.type);
        assert (node.base->eval.expression_kind != UnknownExpressionKind);
        if (node.base->eval.expression_kind == VariableExpressionKind)
          {
            node.operation = new IndexSlice (node.location, new Load (node.base->operation, node.slice_type), index_op, node.slice_type);
          }
        else
          {
            UNIMPLEMENTED;
          }

        return;
      }

    NOT_REACHED;
  }

  void visit (SliceExpr& node)
  {
    node.base->accept (*this);

    Operation* low = NULL;
    if (node.low_present)
      {
        node.low->accept (*this);
        low = node.low->operation;
        low = load (node.low, low);
        low = MakeConvertToInt (low, node.low->eval.type);
      }

    Operation* high = NULL;
    if (node.high_present)
      {
        node.high->accept (*this);
        high = node.high->operation;
        high = load (node.high, high);
        high = MakeConvertToInt (high, node.high->eval.type);
      }

    Operation* max = NULL;
    if (node.max_present)
      {
        node.max->accept (*this);
        max = node.max->operation;
        max = load (node.max, max);
        max = MakeConvertToInt (max, node.max->eval.type);
      }

    if (node.array_type != NULL)
      {
        assert (node.base->eval.expression_kind != UnknownExpressionKind);
        if (node.base->eval.expression_kind == VariableExpressionKind)
          {
            node.operation = new SliceArray (node.location, node.base->operation, low, high, max, node.array_type);
            return;
          }
        else
          {
            UNIMPLEMENTED;
            return;
          }
      }

    if (node.slice_type != NULL)
      {
        Operation* base = node.base->operation;
        base = load (node.base, base);
        node.operation = new SliceSlice (node.location, base, low, high, max, node.slice_type);
        return;
      }

    NOT_REACHED;
  }

  void visit (UnaryArithmeticExpr& node)
  {
    if (node.eval.value.present)
      {
        node.operation = make_literal (node.eval.type, node.eval.value);
      }
    else
      {
        ExpressionValueList arg_vals;
        arg_vals.push_back (node.child->eval);

        node.visit_children (*this);
        Operation* c = node.child->operation;
        c = load (node.child, c);
        node.operation = node.temp->generate_code (node.eval, arg_vals, new ListOperation (c));

        // switch (node.arithmetic)
        //   {
        //   case ::Posate:
        //     UNIMPLEMENTED;
        //   case ::Negate:
        //     node.operation = make_unary<Negater> (node.eval.type, c);
        //     break;
        //   case ::Complement:
        //     UNIMPLEMENTED;
        //   }
      }
  }

  void visit (BinaryArithmeticExpr& node)
  {
    if (node.eval.value.present)
      {
        node.operation = make_literal (node.eval.type, node.eval.value);
      }
    else
      {
        ExpressionValueList arg_vals;
        arg_vals.push_back (node.left->eval);
        arg_vals.push_back (node.right->eval);
        node.visit_children (*this);
        Operation* left = node.left->operation;
        left = load (node.left, left);
        Operation* right = node.right->operation;
        right = load (node.right, right);
        node.operation = node.temp->generate_code (node.eval, arg_vals, new ListOperation (left, right));
      }
  }

  void visit (TypeExpression& node)
  {
    //Do nothing.
  }

  void visit (PushPortCallExpr& node)
  {
    node.args->accept (*this);
    node.operation = new PushPortCall (node.push_port_type, node.receiver_parameter->offset (), arch::offset (node.field), node.args->operation);
  }

  void visit (IndexedPushPortCallExpr& node)
  {
    node.index->accept (*this);
    Operation* i = node.index->operation;
    i = load (node.index, i);
    node.args->accept (*this);
    node.operation = new IndexedPushPortCall (node.push_port_type, node.receiver_parameter->offset (), arch::offset (node.field), i, node.args->operation, node.array_type);
  }

};

void generate_code (ast::Node* root)
{
  CodeGenVisitor visitor;
  root->accept (visitor);
}

}
