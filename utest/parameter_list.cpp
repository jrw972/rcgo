#include "parameter_list.hpp"

#include "tap.hpp"
#include "location.hpp"
#include "symbol.hpp"
#include "arch.hpp"
#include "type.hpp"

#include <sstream>

using namespace type;
using namespace decl;
using namespace util;

int
main (int argc, char** argv)
{
  Tap tap;

  arch::set_stack_alignment (8);

  {
    Location loc;
    ParameterList sig (loc);
    tap.tassert ("ParameterList::ParameterList ()",
                 sig.location == loc &&
                 sig.size () == 0 &&
                 sig.begin () == sig.end () &&
                 sig.rbegin () == sig.rend () &&
                 sig.is_variadic () == false);
  }

  {
    Parameter* p = Parameter::make (util::Location (), "there", &named_int, Immutable, Immutable);
    ParameterList sig ((Location ()));
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
    ParameterList sig ((Location ()));
    std::string a = sig.to_string ();
    sig.append (Parameter::make (util::Location (), "there", &named_int, Immutable, Immutable));
    std::string b = sig.to_string ();
    sig.append (Parameter::make (util::Location (), "there", &named_int, Immutable, Immutable));
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
    ParameterList sig ((Location ()));
    ParameterList* sig2 = sig.set_variadic (true);
    tap.tassert ("ParameterList::set_variadic",
                 sig2 == &sig &&
                 sig.is_variadic ());
  }

  {
    ParameterList sig ((Location ()));
    sig.append (Parameter::make (util::Location (), "there", &named_int, Immutable, Immutable));
    tap.tassert ("ParameterList::is_foreign_safe () true",
                 sig.is_foreign_safe () == true);
    sig.append (Parameter::make (util::Location (), "there", named_int.get_pointer (), Immutable, Mutable));
    tap.tassert ("ParameterList::is_foreign_safe () false",
                 sig.is_foreign_safe () == false);
  }

  tap.print_plan ();

  return 0;
}
