#include <error.h>

#include "semantic.hpp"
#include "ast.hpp"
#include "symbol.hpp"
#include "semantic.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "field.hpp"
#include "callable.hpp"
#include "ast_visitor.hpp"

namespace semantic
{

using namespace type;
using namespace ast;
using namespace decl;

static void
type_check_statement (Node * node)
{
  struct visitor : public ast::DefaultVisitor
  {
    void default_action (Node& node)
    {
      AST_NOT_REACHED (node);
    }

    void visit (ast_bind_pull_port_statement_t& node)
    {
      UNIMPLEMENTED;
      // typed_Value pull_port_tv = CheckExpectReference (node.left ());
      // typed_Value getter_tv = CheckAndImplicitlyDereference (node.right_ref ());

      // const Type::Function* pull_port_type = type_cast<Type::Function> (pull_port_tv.type);

      // if (pull_port_type == NULL || pull_port_type->kind != Type::Function::PULL_PORT)
      //   {
      //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                    "target of bind is not a pull port (E42)");
      //   }

      // const Type::Method* getter_type = type_cast<Type::Method> (getter_tv.type);

      // if (getter_type == NULL)
      //   {
      //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                    "source of bind is not a getter (E43)");
      //   }

      // Type::Function g (Type::Function::FUNCTION, getter_type->signature, getter_type->return_parameter);
      // if (!type_is_equal (pull_port_type, &g))
      //   {
      //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
      //                    "cannot bind %s to %s (E44)", pull_port_type->ToString ().c_str (), getter_type->ToString ().c_str ());
      //   }
    }

    static void arithmetic_assign (ast_binary_t* node, const char* symbol)
    {
      UNIMPLEMENTED;
      // typed_Value left_tv = check_assignment_target (node->left ());
      // typed_Value right_tv = CheckAndImplicitlyDereference (node->right_ref ());
      // if (!type_is_equal (left_tv.type, right_tv.type))
      //   {
      //     error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
      //                    "incompatible types (%s) %s (%s) (E46)", left_tv.type->ToString ().c_str (), symbol, right_tv.type->ToString ().c_str ());
      //   }

      // struct visitor : public Type::DefaultVisitor
      // {
      //   ast::Node* node;
      //   const char* symbol;

      //   visitor (ast::Node* n, const char* s) : node (n), symbol (s) { }

      //   void visit (const NamedType& type)
      //   {
      //     type.UnderlyingType ()->Accept (*this);
      //   }

      //   void visit (const Int& type)
      //   {
      //     // Okay.
      //   }

      //   void visit (const Uint& type)
      //   {
      //     // Okay.
      //   }

      //   void default_action (const Type::Type& type)
      //   {
      //     error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
      //                    "incompatible types (%s) %s (%s) (E47)", type.ToString ().c_str (), symbol, type.ToString ().c_str ());
      //   }
      // };
      // visitor v (node, symbol);
      // left_tv.type->Accept (v);
    }

    void visit (ast_subtract_assign_statement_t& node)
    {
      arithmetic_assign (&node, "-=");
    }

    void visit (ast_list_statement_t& node)
    {
      for (Node::ConstIterator pos = node.Begin (), limit = node.End ();
           pos != limit;
           ++pos)
        {
          type_check_statement (*pos);
        }
    }

    void visit (ast_decrement_statement_t& node)
    {
      UNIMPLEMENTED;
    }
  };

  visitor v;
  node->Accept (v);
}

// TODO: Replace node with its symbol table.
void
enter_signature (Node& node, const Signature * type)
{
  for (Signature::ParametersType::const_iterator pos = type->Begin (), limit = type->End ();
       pos != limit; ++pos)
    {
      ParameterSymbol* x = *pos;
      // Check if the symbol is defined locally.
      const std::string& identifier = x->identifier;
      Symbol* s = node.FindLocalSymbol (identifier);
      if (s == NULL)
        {
          node.EnterSymbol (x);
        }
      else
        {
          error_at_line (-1, 0, x->definingNode->location.File.c_str (), x->definingNode->location.Line,
                         "%s is already defined in this scope (E55)",
                         identifier.c_str ());
        }
    }
}

}
