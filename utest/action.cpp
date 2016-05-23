#include "type.hpp"

#include "tap.hpp"
#include "action.hpp"
#include "runtime.hpp"
#include "node.hpp"
#include "type.hpp"

using namespace decl;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    util::Location loc;
    ParameterSymbol* p = ParameterSymbol::makeReceiver (loc, "", &type::named_int, Immutable, Immutable);
    ast::Node* n = new ast::EmptyStatement (1);
    Action a (p, n, "a");
    tap.tassert ("Action::Action", a.receiver_parameter == p && a.body == n && a.name == "a");
  }

  {
    util::Location loc;
    ParameterSymbol* p = ParameterSymbol::makeReceiver (loc, "", &type::named_int, Immutable, Immutable);
    ast::Node* n = new ast::EmptyStatement (1);
    ParameterSymbol* i = ParameterSymbol::make (loc, "", &type::named_int, Immutable, Immutable);
    Action a (p, n, "a", i, 3);
    tap.tassert ("Action::Action", a.receiver_parameter == p && a.body == n && a.name == "a" && a.iota_parameter == i && a.dimension == 3);
  }

  tap.print_plan ();

  return 0;
}
