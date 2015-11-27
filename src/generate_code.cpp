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
          op->list.push_back ((*pos)->operation);
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
      if (node.condition ()->value.present) {
        if (node.condition ()->value.ref (*Bool::Instance ())) {
          node.operation = node.true_branch ()->operation;
        } else {
          node.operation = node.false_branch ()->operation;
        }
      } else {
        Operation* c = node.condition ()->operation;
        if (node.condition ()->expression_kind == kVariable) {
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
          // Initialize the variables.
          unimplemented;
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
      node.operation = new Assign (left, right, node.left ()->type->Size ());
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
          if ((*pos)->expression_kind == kVariable) {
            op->list.push_back (new Load ((*pos)->operation, (*pos)->type));
          } else {
            op->list.push_back ((*pos)->operation);
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
      struct Visitor : public ConstSymbolVisitor
      {
        ast_identifier_expr_t& node;
        Operation* op;
        Visitor (ast_identifier_expr_t& n) : node (n), op (NULL) { }
        void defaultAction (const Symbol& s)
        {
          symbol_not_reached (s);
        }

        void visit (const ConstantSymbol& s)
        {
          op = make_literal (node.type, node.value);
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
      if (node.array_type != NULL) {

        Operation* index_op = node.index ()->operation;
        if (node.index ()->expression_kind == kVariable) {
          index_op = new Load (index_op, node.index ()->type);
        }

        index_op = MakeConvertToInt (index_op, node.index ()->type);

        if (node.base ()->expression_kind == kVariable) {
          node.operation = new Index (node.base ()->operation, index_op);
        } else {
          unimplemented;
        }

        return;
      }

      not_reached;
    }


    void visit (ast_unary_arithmetic_expr_t& node)
    {
      if (node.value.present) {
        unimplemented;
      } else {
        node.VisitChildren (*this);
        Operation* c = node.child ()->operation;
        if (node.child ()->expression_kind == kVariable) {
          c = new Load (c, node.child ()->type);
        }
        switch (node.arithmetic) {
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
      if (node.value.present) {
        node.operation = make_literal (node.type, node.value);
      } else {
        node.VisitChildren (*this);
        Operation* left = node.left ()->operation;
        if (node.left ()->expression_kind == kVariable) {
          left = new Load (left, node.left ()->type);
        }
        Operation* right = node.right ()->operation;
        if (node.right ()->expression_kind == kVariable) {
          right = new Load (right, node.right ()->type);
        }

        switch (node.arithmetic) {
        case Multiply:
          unimplemented;
        case Divide:
          unimplemented;
        case Modulus:
          unimplemented;
        case LeftShift:
          unimplemented;
        case RightShift:
          unimplemented;
        case BitAnd:
          unimplemented;
        case BitAndNot:
          unimplemented;
        case Add:
          unimplemented;
        case Subtract:
          unimplemented;
        case BitOr:
          unimplemented;
        case BitXor:
          unimplemented;
        case Equal:
          node.operation = make_binary<Equalizer> (node.type, left, right);
          break;
        case NotEqual:
          node.operation = make_binary<NotEqualizer> (node.type, left, right);
          break;
        case LessThan:
          unimplemented;
        case LessEqual:
          unimplemented;
        case MoreThan:
          unimplemented;
        case MoreEqual:
          unimplemented;
        case ::LogicOr:
          node.operation = new runtime::LogicOr (left, right);
          break;
        case ::LogicAnd:
          node.operation = new runtime::LogicAnd (left, right);
          break;
        }
      }
    }
  };

  void generate_code (ast::Node* root)
  {
    CodeGenVisitor visitor;
    root->Accept (visitor);
  }

}
