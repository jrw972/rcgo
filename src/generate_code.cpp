#include "generate_code.hpp"

#include "AstVisitor.hpp"
#include "runtime.hpp"
#include "Callable.hpp"
#include "SymbolVisitor.hpp"
#include "field.hpp"
#include "semantic.hpp"
#include "reaction.hpp"
#include "bind.hpp"

namespace  code
{
  using namespace ast;
  using namespace runtime;
  using namespace Type;

  struct CodeGenVisitor : public ast::DefaultVisitor
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

    void visit (ast_instance_t& node)
    {
      node.expression_list ()->Accept (*this);
      node.operation = new MethodCall (node.symbol->initializer, new runtime::Instance (node.symbol), node.expression_list ()->operation);
    }

    void visit (ast_const_t& node)
    {
      // Do nothing.
    }

    void visit (ast_initializer_t& node)
    {
      node.body ()->Accept (*this);
      node.operation = new SetRestoreCurrentInstance (node.body ()->operation, node.initializer->memoryModel.ReceiverOffset ());
    }

    void visit (ast_getter_t& node)
    {
      node.body ()->Accept (*this);
      node.operation = new SetRestoreCurrentInstance (node.body ()->operation, node.getter->memoryModel.ReceiverOffset ());
    }

    void visit (ast_action_t& node)
    {
      node.precondition ()->Accept (*this);
      node.precondition ()->operation = new SetRestoreCurrentInstance (node.precondition ()->operation, node.action->memory_model.ReceiverOffset ());
      node.body ()->Accept (*this);
      node.body ()->operation = new SetRestoreCurrentInstance (node.body ()->operation, node.action->memory_model.ReceiverOffset ());
    }

    void visit (ast_reaction_t& node)
    {
      node.body ()->Accept (*this);
      node.operation = new SetRestoreCurrentInstance (node.body ()->operation, node.reaction->memory_model.ReceiverOffset ());
    }

    void visit (ast_bind_t& node)
    {
      node.body ()->Accept (*this);
    }

    void visit (ast_function_t& node)
    {
      node.body ()->Accept (*this);
      node.operation = node.body ()->operation;
    }

    void visit (ast_method_t& node)
    {
      node.body ()->Accept (*this);
      node.operation = node.body ()->operation;
    }

    void visit (ast_list_statement_t& node)
    {
      node.VisitChildren (*this);
      ListOperation* op = new ListOperation ();
      for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
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

    void visit (ast_expression_statement_t& node)
    {
      node.VisitChildren (*this);
      // TODO:  Add cleanup code so that something isn't left on the stack.
      node.operation = node.child ()->operation;
    }

    void visit (ast_return_statement_t& node)
    {
      node.VisitChildren (*this);
      Operation* c = node.child ()->operation;
      if (node.child ()->expression_kind == kVariable)
        {
          c = new Load (c, node.child ()->type);
        }
      node.operation = new Return (c, node.return_symbol);
    }

    void visit (ast_if_statement_t& node)
    {
      node.VisitChildren (*this);
      if (node.condition ()->value.present)
        {
          if (node.condition ()->value.ref (*Bool::Instance ()))
            {
              node.operation = node.true_branch ()->operation;
            }
          else
            {
              node.operation = node.false_branch ()->operation;
            }
        }
      else
        {
          Operation* c = node.condition ()->operation;
          if (node.condition ()->expression_kind == kVariable)
            {
              c = new Load (c, node.condition ()->type);
            }
          node.operation = new If (c, node.true_branch ()->operation, node.false_branch ()->operation);
        }
    }

    void visit (ast_var_statement_t& node)
    {
      ListOperation* op = new ListOperation ();
      if (node.expression_list ()->Empty ())
        {
          // Clear the variables.
          for (ast_var_statement_t::SymbolsType::const_iterator pos = node.symbols.begin (), limit = node.symbols.end ();
               pos != limit;
               ++pos)
            {
              VariableSymbol* symbol = *pos;
              op->list.push_back (new Clear (symbol->offset (), symbol->type->Size ()));
            }
        }
      else
        {
          node.expression_list ()->Accept (*this);
          // Initialize the variables.
          size_t idx = 0;
          for (Node::ConstIterator pos = node.expression_list ()->Begin (), limit = node.expression_list ()->End ();
               pos != limit;
               ++pos, ++idx)
            {
              VariableSymbol* symbol = node.symbols[idx];
              Operation* right = (*pos)->operation;
              if ((*pos)->expression_kind == kVariable)
                {
                  right = new Load (right, (*pos)->type);
                }
              op->list.push_back (new Assign (new Reference (symbol->offset ()), right, symbol->type));
            }
        }
      node.operation = op;
    }

    void visit (ast_assign_statement_t& node)
    {
      node.VisitChildren (*this);
      Operation* left = node.left ()->operation;
      Operation* right = node.right ()->operation;
      if (node.right ()->expression_kind == kVariable)
        {
          right = new Load (right, node.right ()->type);
        }
      node.operation = new Assign (left, right, node.left ()->type);
    }

    void visit (ast_increment_statement_t& node)
    {
      node.VisitChildren (*this);
      node.operation = make_increment (node.child ()->operation, node.child ()->type);
    }

    void visit (ast_change_statement_t& node)
    {
      node.expr ()->Accept (*this);
      node.body ()->Accept (*this);
      Operation* root = node.expr ()->operation;
      if (node.expr ()->expression_kind == kVariable)
        {
          root = new Load (root, node.expr ()->type);
        }
      node.operation = new Change (root, node.root_symbol->offset (), node.body ()->operation);
    }

    void visit (ast_activate_statement_t& node)
    {
      node.VisitChildren (*this);
      Operation* b = node.body ()->operation;
      b = new SetRestoreCurrentInstance (b, node.memoryModel->ReceiverOffset ());
      // Add to the schedule.
      if (node.mutable_phase_access == AccessWrite)
        {
          b = new Push (b);
        }
      node.operation = new Activate (node.expr_list ()->operation, b);
    }

    void visit (ast_bind_push_port_statement_t& node)
    {
      node.VisitChildren (*this);
    }

    void visit (ast_bind_pull_port_statement_t& node)
    {
      node.VisitChildren (*this);
    }

    void visit (ast_call_expr_t& node)
    {
      node.args ()->Accept (*this);

      if (node.expr ()->expression_kind == kType)
        {
          // Conversion.
          node.operation = node.args ()->At (0)->operation;

          if (node.string_duplication)
            {
              node.operation = new ConvertStringToSliceOfBytes (node.operation);
            }
          return;
        }
      else if (node.callable != NULL)
        {
          if (node.function_type)
            {
              node.operation = new FunctionCall (node.callable, node.args ()->operation);
            }
          else if (node.method_type)
            {
              node.expr ()->Accept (*this);
              if (type_dereference (node.expr ()->At (0)->type))
                {
                  if (type_dereference (node.method_type->receiver_type ()))
                    {
                      if (node.expr ()->At (0)->expression_kind == kVariable)
                        {
                          // Got a pointer.  Expecting a pointer.  Load the pointer.
                          node.operation = new MethodCall (node.callable, new Load (node.expr ()->At (0)->operation, node.expr ()->At (0)->type), node.args ()->operation);
                        }
                      else
                        {
                          // Got a pointer.  Expecting a pointer.  Pointer is alreay loaded.
                          node.operation = new MethodCall (node.callable, node.expr ()->At (0)->operation, node.args ()->operation);
                        }
                    }
                  else
                    {
                      // Got a pointer.  Expecting a value.  Load the variable and then the pointer.
                      if (node.expr ()->At (0)->expression_kind == kVariable)
                        {
                          node.operation = new MethodCall (node.callable, new Load (new Load (node.expr ()->At (0)->operation, node.expr ()->At (0)->type), node.method_type->receiver_type ()), node.args ()->operation);
                        }
                      else
                        {
                          unimplemented;
                        }
                    }
                }
              else
                {
                  if (type_dereference (node.method_type->receiver_type ()))
                    {
                      if (node.expr ()->At (0)->expression_kind == kVariable)
                        {
                          // Got a value.  Expected a pointer.  Use variable as pointer.
                          node.operation = new MethodCall (node.callable, node.expr ()->At (0)->operation, node.args ()->operation);
                        }
                      else
                        {
                          unimplemented;
                        }
                    }
                  else
                    {
                      if (node.expr ()->At (0)->expression_kind == kVariable)
                        {
                          // Got a value.  Expected a value.  Load the variable.
                          node.operation = new MethodCall (node.callable, new Load (node.expr ()->At (0)->operation, node.expr ()->At (0)->type), node.args ()->operation);
                        }
                      else
                        {
                          unimplemented;
                        }
                    }
                }
            }
          else
            {
              not_reached;
            }
        }
      else
        {
          node.expr ()->Accept (*this);
          if (node.function_type)
            {
              node.expr ()->Accept (*this);
              Operation* r = node.expr ()->operation;
              if (node.expr ()->expression_kind == kVariable)
                {
                  r = new Load (r, node.expr ()->type);
                }
              node.operation = new DynamicFunctionCall (node.function_type, r, node.args ()->operation);
            }
          else if (node.method_type)
            {
              unimplemented;
            }
          else
            {
              not_reached;
            }
        }
    }

    void visit (ast_list_expr_t& node)
    {
      node.VisitChildren (*this);
      ListOperation* op = new ListOperation ();
      for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
           pos != limit;
           ++pos)
        {
          if ((*pos)->operation != NULL)
            {
              if ((*pos)->expression_kind == kVariable)
                {
                  op->list.push_back (new Load ((*pos)->operation, (*pos)->type));
                }
              else
                {
                  op->list.push_back ((*pos)->operation);
                }
            }
        }
      node.operation = op;
    }

    void visit (ast_literal_expr_t& node)
    {
      node.operation = make_literal (node.type, node.value);
    }

    void visit (ast_identifier_expr_t& node)
    {
      if (node.value.present)
        {
          node.operation = make_literal (node.type, node.value);
          return;
        }

      struct Visitor : public ConstSymbolVisitor
      {
        ast_identifier_expr_t& node;
        Operation* op;
        Visitor (ast_identifier_expr_t& n) : node (n), op (NULL) { }
        void defaultAction (const Symbol& s)
        {
          symbol_not_reached (s);
        }

        void visit (const ParameterSymbol& s)
        {
          op = new Reference (s.offset ());
        }

        void visit (const VariableSymbol& s)
        {
          op = new Reference (s.offset ());
        }

        void visit (const TypeSymbol& s)
        {
          op = new Noop ();
        }
      };
      Visitor v (node);
      node.symbol->accept (v);
      node.operation = v.op;
    }

    void visit (ast_dereference_expr_t& node)
    {
      node.VisitChildren (*this);
      node.operation = node.child ()->operation;
      if (node.child ()->expression_kind == kVariable)
        {
          node.operation = new Load (node.operation, node.child ()->type);
        }
    }

    void visit (ast_address_of_expr_t& node)
    {
      node.VisitChildren (*this);
      node.operation = node.child ()->operation;
    }

    void visit (ast_select_expr_t& node)
    {
      node.base ()->Accept (*this);

      if (node.field != NULL)
        {
          if (type_dereference (node.base ()->type))
            {
              if (node.base ()->expression_kind == kVariable)
                {
                  node.operation = new Select (new Load (node.base ()->operation, node.base ()->type), node.field->offset);
                }
              else
                {
                  unimplemented;
                }
            }
          else
            {
              if (node.base ()->expression_kind == kVariable)
                {
                  node.operation = new Select (node.base ()->operation, node.field->offset);
                }
              else
                {
                  unimplemented;
                }
            }
        }
      else
        {
          assert (node.callable != NULL);
          value_t v;
          v.present = true;
          v.pointer_value_ = const_cast<void*> (static_cast<const void*> (node.callable));
          node.operation = make_literal (Int::Instance ()->GetPointer (), v);
        }
    }

    void visit (ast_index_expr_t& node)
    {
      node.VisitChildren (*this);

      if (node.array_type != NULL)
        {
          Operation* index_op = node.index ()->operation;
          if (node.index ()->expression_kind == kVariable)
            {
              index_op = new Load (index_op, node.index ()->type);
            }

          index_op = MakeConvertToInt (index_op, node.index ()->type);

          if (node.base ()->expression_kind == kVariable)
            {
              node.operation = new IndexArray (node.location, node.base ()->operation, index_op, node.array_type);
            }
          else
            {
              unimplemented;
            }

          return;
        }

      if (node.slice_type != NULL)
        {
          Operation* index_op = node.index ()->operation;
          if (node.index ()->expression_kind == kVariable)
            {
              index_op = new Load (index_op, node.index ()->type);
            }

          index_op = MakeConvertToInt (index_op, node.index ()->type);

          if (node.base ()->expression_kind == kVariable)
            {
              node.operation = new IndexSlice (node.location, new Load (node.base ()->operation, node.slice_type), index_op, node.slice_type);
            }
          else
            {
              unimplemented;
            }

          return;
        }

      not_reached;
    }


    void visit (ast_unary_arithmetic_expr_t& node)
    {
      if (node.value.present)
        {
          node.operation = make_literal (node.type, node.value);
        }
      else
        {
          node.VisitChildren (*this);
          Operation* c = node.child ()->operation;
          if (node.child ()->expression_kind == kVariable)
            {
              c = new Load (c, node.child ()->type);
            }
          switch (node.arithmetic)
            {
            case LogicNot:
              node.operation = make_unary<LogicNotter> (node.type, c);
              break;
            case Negate:
              node.operation = make_unary<Negater> (node.type, c);
              break;
            }
        }
    }

    void visit (ast_binary_arithmetic_expr_t& node)
    {
      if (node.value.present)
        {
          node.operation = make_literal (node.type, node.value);
        }
      else
        {
          node.VisitChildren (*this);
          Operation* left = node.left ()->operation;
          if (node.left ()->expression_kind == kVariable)
            {
              left = new Load (left, node.left ()->type);
            }
          Operation* right = node.right ()->operation;
          if (node.right ()->expression_kind == kVariable)
            {
              right = new Load (right, node.right ()->type);
            }

          switch (node.arithmetic)
            {
            case Multiply:
              node.operation = make_binary_arithmetic<Multiplier> (node.type, left, right);
              break;
            case Divide:
              node.operation = make_binary_arithmetic<Divider> (node.type, left, right);
              break;
            case Modulus:
              node.operation = make_binary_integral<Modulizer> (node.type, left, right);
              break;
            case LeftShift:
              node.operation = make_shift<LeftShifter> (node.type, left, MakeConvertToUint (right, node.right ()->type));
              break;
            case RightShift:
              node.operation = make_shift<RightShifter> (node.type, left, MakeConvertToUint (right, node.right ()->type));
              break;
            case BitAnd:
              node.operation = make_binary_integral<BitAnder> (node.type, left, right);
              break;
            case BitAndNot:
              node.operation = make_binary_integral<BitAndNotter> (node.type, left, right);
              break;
            case Add:
              node.operation = make_binary_arithmetic<Adder> (node.type, left, right);
              break;
            case Subtract:
              node.operation = make_binary_arithmetic<Subtracter> (node.type, left, right);
              break;
            case BitOr:
              node.operation = make_binary_integral<BitOrer> (node.type, left, right);
              break;
            case BitXor:
              node.operation = make_binary_integral<BitXorer> (node.type, left, right);
              break;
            case Equal:
              node.operation = make_binary_arithmetic<Equalizer> (node.left ()->type, left, right);
              break;
            case NotEqual:
              node.operation = make_binary_arithmetic<NotEqualizer> (node.left ()->type, left, right);
              break;
            case LessThan:
              node.operation = make_binary_arithmetic<LessThaner> (node.left ()->type, left, right);
              break;
            case LessEqual:
              node.operation = make_binary_arithmetic<LessEqualizer> (node.left ()->type, left, right);
              break;
            case MoreThan:
              node.operation = make_binary_arithmetic<MoreThaner> (node.left ()->type, left, right);
              break;
            case MoreEqual:
              node.operation = make_binary_arithmetic<MoreEqualizer> (node.left ()->type, left, right);
              break;
            case ::LogicOr:
              node.operation = new runtime::LogicOr (left, right);
              break;
            case ::LogicAnd:
              node.operation = new runtime::LogicAnd (left, right);
              break;
            }
        }
    }

    void visit (TypeExpression& node)
    {
      //Do nothing.
    }

    void visit (ast_push_port_call_expr_t& node)
    {
      node.args ()->Accept (*this);
      node.operation = new PushPortCall (node.receiver_parameter->offset (), node.field->offset, node.args ()->operation);
    }
  };

  void generate_code (ast::Node* root)
  {
    CodeGenVisitor visitor;
    root->Accept (visitor);
  }

}
