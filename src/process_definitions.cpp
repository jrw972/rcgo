#include <error.h>

#include "node.hpp"
#include "symbol.hpp"
#include "callable.hpp"
#include "node_visitor.hpp"

namespace semantic
{

using namespace type;
using namespace ast;
using namespace decl;

static void
type_check_statement (Node * node)
{
  struct visitor : public ast::DefaultNodeVisitor
  {
    void default_action (Node& node)
    {
      AST_NOT_REACHED (node);
    }

    void visit (BindPullPort& node)
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

    void visit (StatementList& node)
    {
      for (List::ConstIterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          type_check_statement (*pos);
        }
    }
  };

  visitor v;
  node->accept (v);
}

}
