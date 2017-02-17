#include "type.hpp"

#include "tap.hpp"
#include "symbol_cast.hpp"
#include "identifier.hpp"

using namespace type;
using namespace decl;
using namespace source;

int
main (int argc, char** argv)
{
  Tap tap;

  const Location& loc = source::Location::builtin;

  {
    Symbol* s = new Variable (Identifier ("id", loc), /*&named_bool,*/ Mutable, Mutable);
    tap.tassert ("decl::symbol_cast", symbol_cast<Variable> (s) != NULL);
    tap.tassert ("decl::symbol_cast (null)", symbol_cast<Parameter> (s) == NULL);
  }

  {
    const Symbol* s = new Variable (Identifier ("id", loc), /*&named_bool,*/ Mutable, Mutable);
    tap.tassert ("decl::symbol_cast const", symbol_cast<Variable> (s) != NULL);
    tap.tassert ("decl::symbol_cast const (null)", symbol_cast<Parameter> (s) == NULL);
  }

  tap.print_plan ();

  return 0;
}
