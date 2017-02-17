#include "parameter_list.hpp"

#include "tap.hpp"
#include "location.hpp"
#include "symbol.hpp"
#include "arch.hpp"
#include "type.hpp"
#include "identifier.hpp"

#include <sstream>

using namespace type;
using namespace decl;
using namespace source;

int
main (int argc, char** argv)
{
  Tap tap;

  arch::set_stack_alignment (8);

  const Location& loc = source::Location::builtin;

  {
    ParameterList sig (loc);
    tap.tassert ("ParameterList::ParameterList ()",
                 sig.location == loc &&
                 sig.size () == 0 &&
                 sig.begin () == sig.end () &&
                 sig.rbegin () == sig.rend () &&
                 sig.is_variadic () == false);
  }

  {
    Parameter* p = Parameter::make (Identifier ("there", loc), Immutable, Immutable);
    ParameterList sig (loc);
    ParameterList* sig2 = sig.append (p);

    tap.tassert ("ParameterList::append (Parameter*)",
                 sig2 == &sig &&
                 sig.size () == 1 &&
                 sig.at (0) == p &&
                 *sig.begin () == p &&
                 sig.find ("there") == p &&
                 sig.find ("not there") == NULL);
  }

  {
    ParameterList sig (loc);
    std::string a = sig.to_string ();
    sig.append (Parameter::make (Identifier ("there", loc), Immutable, Immutable, &named_int));
    std::string b = sig.to_string ();
    sig.append (Parameter::make (Identifier ("there", loc), Immutable, Immutable, &named_int));
    std::string c = sig.to_string ();

    std::stringstream str;
    str << sig;

    tap.tassert ("ParameterList::to_string",
                 a == "()" &&
                 b == "(int)" &&
                 c == "(int, int)" &&
                 str.str () == c);
  }

  {
    ParameterList sig (loc);
    ParameterList* sig2 = sig.set_variadic (true);
    tap.tassert ("ParameterList::set_variadic",
                 sig2 == &sig &&
                 sig.is_variadic ());
  }

  tap.print_plan ();

  return 0;
}
