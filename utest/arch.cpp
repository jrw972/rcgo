#include "arch.hpp"

#include "tap.hpp"

using namespace arch;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    unsigned int x = rand ();
    set_stack_alignment (x);
    tap.tassert ("set_stack_alignment", x == stack_alignment ());
  }

  tap.print_plan ();

  return 0;
}
