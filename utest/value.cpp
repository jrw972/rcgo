#include "tap.hpp"
#include "value.hpp"

using namespace semantic;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    UntypedComplex uc = UntypedComplex::make (3.0, 4.0);
    tap.tassert ("UntypedComplex::make",
                 uc.real == 3.0 && uc.imag == 4.0);
    tap.tassert ("UntypedComplex::operator==",
                 uc == uc);
    uc = 2.0;
    tap.tassert ("UntypedComplex::operator=(double)",
                 uc.real == 2.0 && uc.imag == 0.0);
  }


  tap.print_plan ();

  return 0;
}
