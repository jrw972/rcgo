#include "type.hpp"

#include "tap.hpp"
#include "semantic.hpp"

using namespace semantic;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    LeftShifter ls;
    tap.tassert ("LeftShifter::operator ()", ls (2, 1) == 4);
  }

  {
    RightShifter rs;
    tap.tassert ("RightShifter::operator ()", rs (2, 1) == 1);
  }

  tap.print_plan ();

  return 0;
}
