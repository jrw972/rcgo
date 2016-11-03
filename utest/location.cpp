#include "arch.hpp"

#include "tap.hpp"
#include "location.hpp"

using namespace util;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    Location::static_file = "alpha";
    Location loc;
    tap.tassert ("Location::Location ()", loc.file () == "alpha" && loc.line () == 0);
  }

  {
    Location::static_file = "beta";
    Location loc (3);
    tap.tassert ("Location::Location (3)", loc.file () == "beta" && loc.line () == 3);
  }

  tap.print_plan ();

  return 0;
}
