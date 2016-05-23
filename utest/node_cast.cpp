#include "type.hpp"

#include "tap.hpp"
#include "node_cast.hpp"

using namespace ast;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    Node* n = new Identifier (1, "id");
    tap.tassert ("ast::node_cast", node_cast<Identifier> (n) != NULL);
    tap.tassert ("ast::node_cast (null)", node_cast<Reaction> (n) == NULL);
  }

  tap.print_plan ();

  return 0;
}
