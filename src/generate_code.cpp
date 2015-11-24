#include "generate_code.hpp"
#include "AstVisitor.hpp"
#include "runtime.hpp"
#include "Callable.hpp"

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

    void visit (ast_initializer_t& node)
    {
      node.body ()->Accept (*this);
      node.operation = new SetRestoreCurrentInstance (node.body ()->operation, node.initializer->memoryModel.ReceiverOffset ());
    }

    void visit (ast_instance_t& node)
    {
      node.expression_list ()->Accept (*this);
      ListOperation* list = static_cast<ListOperation*> (node.expression_list ()->operation);
      // Prepend the receiver.
      list->list.insert (list->list.begin (), new runtime::Instance (node.symbol));
      node.operation = new CallableOperation (node.symbol->initializer, node.expression_list ()->operation);
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

  };

  void generate_code (ast::Node* root)
  {
    CodeGenVisitor visitor;
    root->Accept (visitor);
  }

}
