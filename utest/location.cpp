#include "arch.hpp"

#include "tap.hpp"
#include "location.hpp"

using namespace source;

int
main (int argc, char** argv)
{
  Tap tap;

  tap.print_plan ();

  return 0;
}
