#ifndef RC_SRC_AST_CAST_HPP
#define RC_SRC_AST_CAST_HPP

namespace ast
{

#include "ast.hpp"
#include "ast_visitor.hpp"

template<typename T>
T*
ast_cast (Node* node)
{
  if (node == NULL) return NULL;

  struct visitor : public DefaultVisitor
  {
    T* retval;

    visitor () : retval (NULL) { }

    void visit (T& type)
    {
      retval = &type;
    }
  };
  visitor v;
  node->accept (v);
  return v.retval;
}

}

#endif // RC_SRC_AST_CAST_HPP
