#include "type.hpp"

#include "tap.hpp"
#include "reaction.hpp"

using namespace type;
using namespace decl;

int
main (int argc, char** argv)
{
  Tap tap;

  {
    std::stringstream ss;
    ss << named_int;
    tap.tassert ("operator<<", ss.str () == named_int.to_string ());
  }

  {
    const Pointer* p1 = named_int.get_pointer ();
    const Pointer* p2 = named_int.get_pointer ();
    tap.tassert ("Type::get_pointer", p1 != NULL && p1 == p2);
  }

  {
    const Slice* p1 = named_int.get_slice ();
    const Slice* p2 = named_int.get_slice ();
    tap.tassert ("Type::get_slice", p1 != NULL && p1 == p2);
  }

  {
    const Array* p1 = named_int.get_array (3);
    const Array* p2 = named_int.get_array (3);
    tap.tassert ("Type::get_array", p1 != NULL && p1 == p2);
  }

  {
    const Heap* p1 = named_int.get_heap ();
    const Heap* p2 = named_int.get_heap ();
    tap.tassert ("Type::get_heap", p1 != NULL && p1 == p2);
  }

  {
    const Array* p1 = named_int.get_array (2);
    std::string s = p1->to_string ();
    tap.tassert ("Array::to_string", s == "[2]int");
  }

  {
    NamedType foo ("foo", new Component ());
    Reaction* r1 = foo.get_reaction ("r");
    Reaction* r = new Reaction (NULL, NULL, NULL, "r", NULL);
    foo.insert_reaction (r);
    Reaction* r2 = foo.get_reaction ("r");
    tap.tassert ("NamedType::get_reaction", r1 == NULL && r2 == r);
  }

  tap.print_plan ();

  return 0;
}
