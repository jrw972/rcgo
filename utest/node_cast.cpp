#include "type.hpp"

#include "tap.hpp"
#include "node_cast.hpp"

using namespace ast;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    Node* n = new EmptyType (source::Location::builtin);
    tap.tassert ("ast::node_cast", node_cast<EmptyType> (n) != NULL);
    tap.tassert ("ast::node_cast (null)", node_cast<ReactionDecl> (n) == NULL);
  }

  tap.print_plan ();

  return 0;
}
