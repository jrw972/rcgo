#include "type.hpp"

#include "tap.hpp"
#include "action.hpp"
#include "runtime.hpp"
#include "node.hpp"
#include "type.hpp"
#include "symbol.hpp"

using namespace decl;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    util::Location loc;
    Parameter* r = Parameter::make_receiver (loc, "", &type::named_int, Immutable, Immutable);
    ast::Node* p = new ast::EmptyStatement (1);
    ast::Node* n = new ast::EmptyStatement (1);
    Action a (r, p, n, "a");
    tap.tassert ("Action::Action", a.receiver_parameter == r && a.precondition == p && a.body == n && a.name == "a" && a.has_dimension () == false);
  }

  {
    util::Location loc;
    Parameter* r = Parameter::make_receiver (loc, "", &type::named_int, Immutable, Immutable);
    ast::Node* p = new ast::EmptyStatement (1);
    ast::Node* n = new ast::EmptyStatement (1);
    Parameter* i = Parameter::make (loc, "", &type::named_int, Immutable, Immutable);
    Action a (r, p, n, "a", i, 3);
    tap.tassert ("Action::Action", a.receiver_parameter == r && a.body == n && a.name == "a" && a.iota_parameter == i && a.dimension == 3 && a.has_dimension () == true);
  }

  tap.print_plan ();

  return 0;
}
