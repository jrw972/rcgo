#include "generate_code.hpp"

#include "AstVisitor.hpp"
#include "runtime.hpp"
#include "Callable.hpp"
#include "SymbolVisitor.hpp"
#include "field.hpp"
#include "semantic.hpp"

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
      ListOperation* list = static_cast<ListOperation*> (node.expression_list ()->operation);
      // Prepend the receiver.
      list->list.insert (list->list.begin (), new runtime::Instance (node.symbol));
      node.operation = new CallableOperation (node.symbol->initializer, node.expression_list ()->operation);
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

    void visit (ast_function_t& node)
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
      node.operation = new ReturnJ (node.child ()->operation);
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

    void visit (ast_call_expr_t& node)
    {
      node.args ()->Accept (*this);

      if (node.callable != NULL)
        {
          node.operation = new CallableOperation (node.callable, node.args ()->operation);
        }
      else
        {
          unimplemented;
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
                  unimplemented;
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
          unimplemented;
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
              node.operation = new Index (node.base ()->operation, index_op);
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
  };

  void generate_code (ast::Node* root)
  {
    CodeGenVisitor visitor;
    root->Accept (visitor);
  }

}
