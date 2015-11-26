#include "generate_code.hpp"

#include "AstVisitor.hpp"
#include "runtime.hpp"
#include "Callable.hpp"
#include "SymbolVisitor.hpp"
#include "field.hpp"

namespace  code
{
  using namespace ast;
  using namespace runtime;

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
      node.operation = new Assign (left, right);
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
          op->list.push_back ((*pos)->operation);
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
        Operation* op;
        Visitor () : op (NULL) { }
        void defaultAction (const Symbol& s)
        {
          symbol_not_reached (s);
        }

        void visit (const VariableSymbol& s)
        {
          op = new Reference (s.offset ());
        }
      };
      Visitor v;
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
  };

  void generate_code (ast::Node* root)
  {
    CodeGenVisitor visitor;
    root->Accept (visitor);
  }

}
