#include "type.hpp"

#include "tap.hpp"
#include "symbol_table.hpp"

using namespace type;
using namespace decl;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    SymbolTable st;
    tap.tassert ("SymbolTable::package ()", st.package () == NULL);
  }

  tap.print_plan ();

  return 0;
}
