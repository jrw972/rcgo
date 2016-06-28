#include "type.hpp"

#include "tap.hpp"
#include "symbol_cast.hpp"

using namespace type;
using namespace decl;
using namespace util;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    Symbol* s = new Variable ("id", Location (), &named_bool, Mutable, Mutable);
    tap.tassert ("decl::symbol_cast", symbol_cast<Variable> (s) != NULL);
    tap.tassert ("decl::symbol_cast (null)", symbol_cast<Parameter> (s) == NULL);
  }

  {
    const Symbol* s = new Variable ("id", Location (), &named_bool, Mutable, Mutable);
    tap.tassert ("decl::symbol_cast const", symbol_cast<Variable> (s) != NULL);
    tap.tassert ("decl::symbol_cast const (null)", symbol_cast<Parameter> (s) == NULL);
  }

  tap.print_plan ();

  return 0;
}
