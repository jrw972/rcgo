#include "arch.hpp"

#include "tap.hpp"
#include "type.hpp"
#include "parameter_list.hpp"
#include "symbol.hpp"

using namespace arch;
using namespace type;

int
main (int argc, char** argv)
{
  Tap tap;
  util::Location loc;

  {
    unsigned int x = rand ();
    set_stack_alignment (x);
    tap.tassert ("set_stack_alignment", x == stack_alignment ());
  }

  {
    Struct s;
    s.append_field (NULL, false, "a", loc, named_int.get_pointer (), TagSet ());
    s.append_field (NULL, false, "b", loc, &named_bool, TagSet ());
    tap.tassert ("unit_size(Array)", unit_size (s.get_array (3)) == 16);
    tap.tassert ("unit_size(Slice)", unit_size (s.get_slice ()) == 16);
  }

  set_stack_alignment (8);

  {
    tap.tassert ("alignment (Bool)", alignment (Bool::instance ()) == 1);
    tap.tassert ("alignment (Uint8)", alignment (Uint8::instance ()) == 1);
    tap.tassert ("alignment (Uint16)", alignment (Uint16::instance ()) == 2);
    tap.tassert ("alignment (Uint32)", alignment (Uint32::instance ()) == 4);
    tap.tassert ("alignment (Uint64)", alignment (Uint64::instance ()) == 8);
    tap.tassert ("alignment (Int8)", alignment (Int8::instance ()) == 1);
    tap.tassert ("alignment (Int16)", alignment (Int16::instance ()) == 2);
    tap.tassert ("alignment (Int32)", alignment (Int32::instance ()) == 4);
    tap.tassert ("alignment (Int64)", alignment (Int64::instance ()) == 8);
    tap.tassert ("alignment (Float32)", alignment (Float32::instance ()) == 4);
    tap.tassert ("alignment (Float64)", alignment (Float64::instance ()) == 8);
    tap.tassert ("alignment (Complex64)", alignment (Complex64::instance ()) == 4);
    tap.tassert ("alignment (Complex128)", alignment (Complex128::instance ()) == 8);
    tap.tassert ("alignment (Uint)", alignment (Uint::instance ()) == sizeof (unsigned long));
    tap.tassert ("alignment (Int)", alignment (Int::instance ()) == sizeof (long));
    tap.tassert ("alignment (Uintptr)", alignment (Uintptr::instance ()) == sizeof (ptrdiff_t));
    tap.tassert ("alignment (String)", alignment (String::instance ()) == sizeof (void*));
    Struct s;
    tap.tassert ("alignment (Struct) empty", alignment (&s) == 0);
    s.append_field (NULL, false, "field", loc, &named_int, TagSet ());
    tap.tassert ("alignment (Struct)", alignment (&s) == alignment(&named_int));
    Component c (NULL, loc);
    tap.tassert ("alignment (Component) empty", alignment (&c) == sizeof (void*));
    c.append_field (NULL, false, "field", loc, &named_int, TagSet ());
    tap.tassert ("alignment (Component)", alignment (&c) == alignment(&named_int));
    tap.tassert ("alignment (Array)", alignment (c.get_array (3)) == alignment (&c));
    tap.tassert ("alignment (Pointer)", alignment (c.get_pointer ()) == sizeof (void*));
    tap.tassert ("alignment (Slice)", alignment (c.get_slice ()) == sizeof (void*));
    Function f (new decl::ParameterList (loc), new decl::ParameterList (loc));
    tap.tassert ("alignment (Function)", alignment (&f) == sizeof (void*));
    PushPort push (new decl::ParameterList (loc), new decl::ParameterList (loc));
    tap.tassert ("alignment (PushPort)", alignment (&push) == sizeof (void*));
    PullPort pull (new decl::ParameterList (loc), new decl::ParameterList (loc));
    tap.tassert ("alignment (PullPort)", alignment (&pull) == sizeof (void*));
    tap.tassert ("alignment (FileDescriptor)", alignment (FileDescriptor::instance ()) == sizeof (void*));
    tap.tassert ("alignment (NamedType)", alignment (&named_int) == alignment (Int::instance ()));
  }

  {
    tap.tassert ("size (Bool)", size (Bool::instance ()) == 1);
    tap.tassert ("size (Uint8)", size (Uint8::instance ()) == 1);
    tap.tassert ("size (Uint16)", size (Uint16::instance ()) == 2);
    tap.tassert ("size (Uint32)", size (Uint32::instance ()) == 4);
    tap.tassert ("size (Uint64)", size (Uint64::instance ()) == 8);
    tap.tassert ("size (Int8)", size (Int8::instance ()) == 1);
    tap.tassert ("size (Int16)", size (Int16::instance ()) == 2);
    tap.tassert ("size (Int32)", size (Int32::instance ()) == 4);
    tap.tassert ("size (Int64)", size (Int64::instance ()) == 8);
    tap.tassert ("size (Float32)", size (Float32::instance ()) == 4);
    tap.tassert ("size (Float64)", size (Float64::instance ()) == 8);
    tap.tassert ("size (Complex64)", size (Complex64::instance ()) == 8);
    tap.tassert ("size (Complex128)", size (Complex128::instance ()) == 16);
    tap.tassert ("size (Uint)", size (Uint::instance ()) == sizeof (unsigned long));
    tap.tassert ("size (Int)", size (Int::instance ()) == sizeof (long));
    tap.tassert ("size (Uintptr)", size (Uintptr::instance ()) == sizeof (ptrdiff_t));
    tap.tassert ("size (String)", size (String::instance ()) == sizeof (runtime::String));
    Struct s;
    tap.tassert ("size (Struct) empty", size (&s) == 0);
    s.append_field (NULL, false, "field", loc, &named_int, TagSet ());
    tap.tassert ("size (Struct)", size (&s) == size(&named_int));
    Component c (NULL, loc);
    tap.tassert ("size (Component) empty", size (&c) == sizeof (void*));
    c.append_field (NULL, false, "field", loc, &named_int, TagSet ());
    tap.tassert ("size (Component)", size (&c) == 16);
    tap.tassert ("size (Array)", size (c.get_array (3)) == 48);
    tap.tassert ("size (Pointer)", size (c.get_pointer ()) == sizeof (void*));
    tap.tassert ("size (Slice)", size (c.get_slice ()) == 24);
    Function f (new decl::ParameterList (loc), new decl::ParameterList (loc));
    tap.tassert ("size (Function)", size (&f) == sizeof (void*));
    PushPort push (new decl::ParameterList (loc), new decl::ParameterList (loc));
    tap.tassert ("size (PushPort)", size (&push) == sizeof (void*));
    PullPort pull (new decl::ParameterList (loc), new decl::ParameterList (loc));
    tap.tassert ("size (PullPort)", size (&pull) == 2 * sizeof (void*));
    tap.tassert ("size (FileDescriptor)", size (FileDescriptor::instance ()) == sizeof (void*));
    tap.tassert ("size (NamedType)", size (&named_int) == size (Int::instance ()));
  }

  {
    tap.tassert ("size_on_stack(Type)", size_on_stack (&named_bool) == stack_alignment ());

    decl::ParameterList list (loc);
    list.append (decl::Parameter::make (loc, "a", &named_int8, Immutable, Immutable));
    list.append (decl::Parameter::make (loc, "b", &named_int16, Immutable, Immutable));
    list.append (decl::Parameter::make (loc, "c", &named_int32, Immutable, Immutable));
    list.append (decl::Parameter::make (loc, "d", &named_int64, Immutable, Immutable));
    tap.tassert ("size_on_stack(ParameterList)", size_on_stack (&list) == 32);
  }

  {
    Struct s;
    s.append_field (NULL, false, "a", loc, &named_int8, TagSet ());
    s.append_field (NULL, false, "b", loc, &named_int16, TagSet ());
    s.append_field (NULL, false, "c", loc, &named_int32, TagSet ());
    s.append_field (NULL, false, "d", loc, &named_int64, TagSet ());
    tap.tassert ("offset",
                 offset (s.find_field ("a")) == 0 &&
                 offset (s.find_field ("b")) == 2 &&
                 offset (s.find_field ("c")) == 4 &&
                 offset (s.find_field ("d")) == 8);
  }

  tap.print_plan ();

  return 0;
}
