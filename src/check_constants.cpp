#include "check_constants.hpp"

#include <error.h>

#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include "Callable.hpp"

namespace semantic
{

  using namespace ast;

  namespace
  {

    static void check_constants_arguments (ast::Node* node, const Type::Signature* signature)
    {
      // Convert and check untyped args.
      ast_list_expr_t* args = ast_cast<ast_list_expr_t> (node);

      size_t i = 0;
      for (Node::ConstIterator pos = args->Begin (), limit = args->End ();
           pos != limit;
           ++pos, ++i)
        {
          Node* arg = (*pos);
          if (arg->type->IsUntyped ())
            {
              const Type::Type* arg_type = arg->type;
              const Type::Type* param_type = signature->At (i)->type;
              if (arg->value.representable (arg->type, param_type))
                {
                  arg->value.convert (arg->type, param_type);
                  arg->type = param_type;
                }
              else
                {
                  error_at_line (-1, 0, arg->location.File.c_str (), arg->location.Line,
                                 "cannot assign %s to %s in call(E30)", arg_type->ToString ().c_str (), param_type->ToString ().c_str ());
                }
            }
        }
    }

    // Convert untyped constants to typed (named) constants.
    // Perform additional checks given the value.
    struct ConstantExpressionVisitor : public ast::DefaultVisitor
    {
      void default_action (Node& node)
      {
        ast_not_reached (node);
      }

      void visit (ast_call_expr_t& node)
      {
        node.expr ()->Accept (*this);
        check_constants_arguments (node.args (), node.signature);
      }

      void visit (ast_list_expr_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_literal_expr_t& node)
      {
        // Do nothing.
      }

      void visit (ast_identifier_expr_t& node)
      {
        // Do nothing.
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
      }

      void visit (ast_list_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_expression_statement_t& node)
      {
        node.VisitChildren (*this);
      }

      void visit (ast_instance_t& node)
      {
        node.expression_list ()->Accept (*this);
        check_constants_arguments (node.expression_list (), node.symbol->initializer->initializerType->signature);
      }

    };

  }

  void check_constants (ast::Node* root)
  {
    ConstantExpressionVisitor visitor;
    root->Accept (visitor);
  }
}
