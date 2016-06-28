#include "bind.hpp"

#include "tap.hpp"
#include "node.hpp"
#include "symbol.hpp"
#include "type.hpp"

using namespace decl;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    ast::Identifier id (1, "id");
    util::Location loc (1);
    Parameter* param = Parameter::make (loc, "theparam", &type::named_string, Immutable, Immutable);

    Bind b (&id, "thename", param);
    tap.tassert ("Bind::Bind", b.node == &id && b.name == "thename" && b.receiver_parameter == param);
  }

  tap.print_plan ();

  return 0;
}
