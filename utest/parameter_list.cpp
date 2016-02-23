#include "parameter_list.hpp"

#include "tap.hpp"
#include "location.hpp"
#include "symbol.hpp"

using namespace type;
using namespace decl;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    util::Location loc;
    ParameterSymbol* p = ParameterSymbol::make (loc, "there", &named_int, Immutable, Immutable);
    ParameterList sig;
    sig.append (p);
    tap.tassert ("ParameterList::find (NULL)", sig.find ("not there") == NULL && sig.find ("there") == p);
  }

  tap.print_plan ();

  return 0;
}
