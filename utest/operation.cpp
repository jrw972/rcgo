#include "type.hpp"

#include "tap.hpp"
#include "operation.hpp"

using namespace semantic;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    runtime::LeftShifter ls;
    tap.tassert ("LeftShifter::operator ()", ls (2, 1) == 4);
  }

  {
    runtime::RightShifter rs;
    tap.tassert ("RightShifter::operator ()", rs (2, 1) == 1);
  }

  tap.print_plan ();

  return 0;
}
