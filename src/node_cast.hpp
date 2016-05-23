#ifndef RC_SRC_NODE_CAST_HPP
#define RC_SRC_NODE_CAST_HPP

#include "node.hpp"
#include "node_visitor.hpp"

namespace ast
{

template<typename T>
T*
node_cast (Node* node)
{
  if (node == NULL) return NULL;

  struct visitor : public DefaultNodeVisitor
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

#endif // RC_SRC_NODE_CAST_HPP
