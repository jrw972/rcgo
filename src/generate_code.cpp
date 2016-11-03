#include "generate_code.hpp"

#include "node.hpp"
#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "runtime.hpp"
#include "callable.hpp"
#include "symbol_visitor.hpp"
#include "operation.hpp"
#include "polymorphic_function.hpp"

namespace  code
{
using namespace ast;
using namespace runtime;
using namespace type;
using namespace semantic;
using namespace decl;

static Operation* load (Node* node, Operation* op)
{
  assert (node->eval.kind != ExpressionValue::Unknown);
  if (node->eval.kind == ExpressionValue::Variable)
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
    node.top_level_decl_list->accept (*this);
  }

  void visit (TopLevelDeclList& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::TypeDecl& node)
  {
    // Do nothing.
  }

  void visit (ast::InstanceDecl& node)
  {
    node.arguments->accept (*this);
    node.operation = new MethodCall (node.symbol->initializer (), new runtime::Instance (node.symbol), node.arguments->operation);
  }

  void visit (ConstDecl& node)
  {
    // Do nothing.
  }

  void visit (ast::InitDecl& node)
  {
    node.body->accept (*this);
    node.initializer->operation = new SetRestoreCurrentInstance (node.body->operation, node.initializer->memory_model.receiver_offset ());
  }

  void visit (ast::GetterDecl& node)
  {
    node.body->accept (*this);
    node.getter->operation = new SetRestoreCurrentInstance (node.body->operation, node.getter->memory_model.receiver_offset ());
  }

  void visit (ast::ActionDecl& node)
  {
    node.precondition->accept (*this);
    Operation* p = node.precondition->operation;
    p = load (node.precondition, p);
    node.precondition->operation = new SetRestoreCurrentInstance (p, node.action->memory_model.receiver_offset ());
    node.body->accept (*this);
    node.body->operation = new SetRestoreCurrentInstance (node.body->operation, node.action->memory_model.receiver_offset ());
  }

  void visit (ast::ReactionDecl& node)
  {
    node.body->accept (*this);
    node.reaction->operation = new SetRestoreCurrentInstance (node.body->operation, node.reaction->memory_model.receiver_offset ());
  }

  void visit (ast::BindDecl& node)
  {
    node.body->accept (*this);
  }

  void visit (ast::FunctionDecl& node)
  {
    node.body->accept (*this);
    node.symbol->operation = node.body->operation;
  }

  void visit (ast::MethodDecl& node)
  {
    node.body->accept (*this);
    node.method->operation = node.body->operation;
  }

  void visit (StatementList& node)
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
    if (node.child->eval.kind != ExpressionValue::Void &&
        arch::size (node.child->eval.type) != 0)
      {
        node.operation = new Popn (node.operation, arch::size (node.child->eval.type));
      }
  }

  void visit (ast::Return& node)
  {
    node.visit_children (*this);
    Operation* c = node.child->operation;
    c = load (node.child, c);
    node.operation = new runtime::Return (c, node.return_symbol);
  }

  void visit (ast::If& node)
  {
    node.visit_children (*this);
    if (node.condition->eval.is_constant ())
      {
        if (node.condition->eval.value.bool_value)
          {
            node.operation = node.true_body->operation;
          }
        else
          {
            node.operation = node.false_body->operation;
          }
      }
    else
      {
        Operation* c = node.condition->operation;
        c = load (node.condition, c);
        node.operation = new runtime::If (c, node.true_body->operation, node.false_body->operation);
      }
  }

  void visit (ast::While& node)
  {
    node.visit_children (*this);
    Operation* c = node.condition->operation;
    c = load (node.condition, c);
    node.operation = new runtime::While (c, node.body->operation);
  }

  void visit (ast::ForIota& node)
  {
    node.body->accept (*this);
    node.operation = new runtime::ForIota (node.symbol, node.limit_value, node.body->operation);
  }

  void visit (Var& node)
  {
    ListOperation* op = new ListOperation ();
    if (node.expressions->empty ())
      {
        // Clear the variables.
        for (Var::SymbolsType::const_iterator pos = node.symbols.begin (), limit = node.symbols.end ();
             pos != limit;
             ++pos)
          {
            Variable* symbol = *pos;
            op->list.push_back (new Clear (symbol->offset (), arch::size (symbol->type)));
          }
      }
    else
      {
        node.expressions->accept (*this);
        // Initialize the variables.
        size_t idx = 0;
        for (List::ConstIterator pos = node.expressions->begin (), limit = node.expressions->end ();
             pos != limit;
             ++pos, ++idx)
          {
            Variable* symbol = node.symbols[idx];
            Operation* right = (*pos)->operation;
            right = load (*pos, right);
            op->list.push_back (new runtime::Assign (new Reference (symbol->offset ()), right, symbol->type));
          }
      }
    node.operation = op;
  }

  void visit (EmptyStatement& node)
  {
    node.operation = new Noop ();
  }

  void visit (ast::Assign& node)
  {
    node.visit_children (*this);
    Operation* left = node.left->operation;
    Operation* right = node.right->operation;
    right = load (node.right, right);
    node.operation = new runtime::Assign (left, right, node.left->eval.type);
  }

  void visit (AddAssign& node)
  {
    node.visit_children (*this);
    Operation* left = node.left->operation;
    Operation* right = node.right->operation;
    right = load (node.right, right);
    node.operation = make_add_assign (left, right, node.left->eval.type);
  }

  void visit (IncrementDecrement& node)
  {
    node.visit_children (*this);
    switch (node.kind)
      {
      case IncrementDecrement::Increment:
        node.operation = make_increment (node.child->operation, node.child->eval.type);
        break;
      case IncrementDecrement::Decrement:
        node.operation = make_decrement (node.child->operation, node.child->eval.type);
        break;
      }
  }

  void visit (ast::Change& node)
  {
    node.argument->accept (*this);
    node.body->accept (*this);
    Operation* root = node.argument->operation;
    root = load (node.argument, root);
    node.operation = new runtime::Change (root, node.root_symbol->offset (), node.body->operation);
  }

  void visit (ast::Activate& node)
  {
    node.visit_children (*this);
    Operation* b = node.body->operation;
    // Add to the schedule.
    if (node.mutable_phase_access == AccessWrite ||
        (node.in_action && !node.arguments->empty ()))
      {
        b = new Push (b);
      }
    b = new SetRestoreCurrentInstance (b, node.memory_model->receiver_offset ());
    node.operation = new runtime::Activate (node.arguments->operation, b);
  }

  void visit (BindPushPort& node)
  {
    node.visit_children (*this);
  }

  void visit (BindPushPortParameter& node)
  {
    node.visit_children (*this);
  }

  void visit (BindPullPort& node)
  {
    node.visit_children (*this);

    ast::Node* n = node_cast<ast::Select> (node.right)->base;
    if (n->eval.kind == ExpressionValue::Variable &&
        n->eval.type->to_pointer ())
      {
        n->operation = load (n, n->operation);
      }
  }

  void visit (Call& node)
  {
    if (node.eval.is_constant ())
      {
        node.operation = make_literal (node.eval.type, node.eval.value);
        return;
      }

    assert (node.expression->eval.kind != ExpressionValue::Unknown);
    if (node.expression->eval.kind == ExpressionValue::Type)
      {
        node.arguments->at (0)->accept (*this);
        Operation* o = node.arguments->at (0)->operation;
        o = load (node.arguments->at (0), o);
        node.operation = make_conversion (o, node.arguments->at (0)->eval.type, node.eval.type);
        return;
      }

    node.arguments->accept (*this);

    if (node.expression->eval.is_polymorphic_function ())
      {
        ExpressionValueList arguments;
        List* args = node.arguments;
        for (List::ConstIterator pos = args->begin (), limit = args->end ();
             pos != limit;
             ++pos)
          {
            arguments.push_back ((*pos)->eval);
          }

        node.operation = node.expression->eval.polymorphic_function->generate_code (node.eval, arguments, static_cast<ListOperation*> (node.arguments->operation));
      }
    else if (node.callable != NULL)
      {
        if (node.function_type)
          {
            node.operation = new FunctionCall (node.callable, node.arguments->operation);
          }
        else if (node.method_type || node.initializer_type || node.getter_type || node.reaction_type)
          {
            const type::MethodBase* mb = node.method_type;
            mb = mb ? mb : node.initializer_type;
            mb = mb ? mb : node.getter_type;
            mb = mb ? mb : node.reaction_type;

            node.expression->accept (*this);
            Node* sb = node_cast<ast::Select> (node.expression)->base;
            if (sb->eval.type->underlying_type ()->to_pointer ())
              {
                if (mb->receiver_parameter->type->underlying_type ()->to_pointer ())
                  {
                    assert (sb->eval.kind != ExpressionValue::Unknown);
                    if (sb->eval.kind == ExpressionValue::Variable)
                      {
                        // Got a pointer.  Expecting a pointer.  Load the pointer.
                        node.operation = new MethodCall (node.callable, new Load (sb->operation, sb->eval.type), node.arguments->operation);
                      }
                    else
                      {
                        // Got a pointer.  Expecting a pointer.  Pointer is alreay loaded.
                        node.operation = new MethodCall (node.callable, sb->operation, node.arguments->operation);
                      }
                  }
                else
                  {
                    // Got a pointer.  Expecting a value.  Load the variable and then the pointer.
                    assert (sb->eval.kind != ExpressionValue::Unknown);
                    if (sb->eval.kind == ExpressionValue::Variable)
                      {
                        node.operation = new MethodCall (node.callable, new Load (new Load (sb->operation, sb->eval.type), mb->receiver_parameter->type), node.arguments->operation);
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
                    assert (sb->eval.kind != ExpressionValue::Unknown);
                    if (sb->eval.kind == ExpressionValue::Variable)
                      {
                        // Got a value.  Expected a pointer.  Use variable as pointer.
                        node.operation = new MethodCall (node.callable, sb->operation, node.arguments->operation);
                      }
                    else
                      {
                        UNIMPLEMENTED;
                      }
                  }
                else
                  {
                    assert (sb->eval.kind != ExpressionValue::Unknown);
                    if (sb->eval.kind == ExpressionValue::Variable)
                      {
                        // Got a value.  Expected a value.  Load the variable.
                        node.operation = new MethodCall (node.callable, new Load (sb->operation, sb->eval.type), node.arguments->operation);
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
            node.expression->accept (*this);
            Operation* r = node.expression->operation;
            r = load (node.expression, r);
            node.operation = new DynamicPullPortCall (node.pull_port_type, r, node.arguments->operation);
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

  void visit (Conversion& node)
  {
    node.argument->accept (*this);
    Operation* o = node.argument->operation;
    o = load (node.argument, o);
    node.operation = make_conversion (o, node.argument->eval.type, node.eval.type);
  }

  void visit (ExpressionList& node)
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

  void visit (ast::Literal& node)
  {
    node.operation = make_literal (node.eval.type, node.eval.value);
  }

  void visit (IdentifierExpression& node)
  {
    if (node.eval.is_constant ())
      {
        node.operation = make_literal (node.eval.type, node.eval.value);
        return;
      }

    struct Visitor : public ConstSymbolVisitor
    {
      IdentifierExpression& node;
      Operation* op;
      Visitor (IdentifierExpression& n) : node (n), op (NULL) { }
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

  void visit (Dereference& node)
  {
    node.visit_children (*this);
    node.operation = load (node.child, node.child->operation);
  }

  void visit (AddressOf& node)
  {
    node.visit_children (*this);
    node.operation = node.child->operation;
  }

  void visit (ast::Select& node)
  {
    node.base->accept (*this);

    if (node.field != NULL)
      {
        if (node.base->eval.type->underlying_type ()->to_pointer ())
          {
            assert (node.base->eval.kind != ExpressionValue::Unknown);
            if (node.base->eval.kind == ExpressionValue::Variable)
              {
                node.operation = new runtime::Select (new Load (node.base->operation, node.base->eval.type), arch::offset (node.field));
              }
            else
              {
                UNIMPLEMENTED;
              }
          }
        else
          {
            assert (node.base->eval.kind != ExpressionValue::Unknown);
            if (node.base->eval.kind == ExpressionValue::Variable)
              {
                node.operation = new runtime::Select (node.base->operation, arch::offset (node.field));
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
        node.operation = new CallableLiteral (node.callable);
      }
  }

  void visit (Index& node)
  {
    node.visit_children (*this);

    if (node.array_type != NULL)
      {
        Operation* index_op = node.index->operation;
        index_op = load (node.index, index_op);
        index_op = MakeConvertToInt (index_op, node.index->eval.type);
        assert (node.base->eval.kind != ExpressionValue::Unknown);
        if (node.base->eval.kind == ExpressionValue::Variable)
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
        assert (node.base->eval.kind != ExpressionValue::Unknown);
        if (node.base->eval.kind == ExpressionValue::Variable)
          {
            node.operation = new runtime::IndexSlice (node.location, new Load (node.base->operation, node.slice_type), index_op, node.slice_type);
          }
        else
          {
            UNIMPLEMENTED;
          }

        return;
      }

    NOT_REACHED;
  }

  void visit (ast::IndexSlice& node)
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
        assert (node.base->eval.kind != ExpressionValue::Unknown);
        if (node.base->eval.kind == ExpressionValue::Variable)
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

  // void visit (ast::BinaryArithmetic& node)
  // {
  //   if (node.eval.is_constant ())
  //     {
  //       node.operation = make_literal (node.eval.type, node.eval.value);
  //     }
  //   else
  //     {
  //       ExpressionValueList arg_vals;
  //       arg_vals.push_back (node.left->eval);
  //       arg_vals.push_back (node.right->eval);
  //       node.visit_children (*this);
  //       Operation* left = node.left->operation;
  //       left = load (node.left, left);
  //       Operation* right = node.right->operation;
  //       right = load (node.right, right);
  //       node.operation = node.eval.polymorphic_function->generate_code (node.eval, arg_vals, new ListOperation (left, right));
  //     }
  // }

  void visit (TypeExpression& node)
  {
    //Do nothing.
  }

  void visit (ast::PushPortCall& node)
  {
    node.arguments->accept (*this);
    node.operation = new runtime::PushPortCall (node.push_port_type, node.receiver_parameter->offset (), arch::offset (node.field), node.arguments->operation);
  }

  void visit (ast::IndexedPushPortCall& node)
  {
    node.index->accept (*this);
    Operation* i = node.index->operation;
    i = load (node.index, i);
    node.arguments->accept (*this);
    node.operation = new runtime::IndexedPushPortCall (node.push_port_type, node.receiver_parameter->offset (), arch::offset (node.field), i, node.arguments->operation, node.array_type);
  }

};

void generate_code (ast::Node* root)
{
  CodeGenVisitor visitor;
  root->accept (visitor);
}

}
