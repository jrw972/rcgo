#include "type.hpp"

#include "tap.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "bind.hpp"
#include "package.hpp"

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
    NamedType foo ("foo", new Component (NULL));
    Action* r1 = foo.get_action ("r");
    Action* r = new Action (NULL, NULL, "r");
    foo.insert_action (r);
    Action* r2 = foo.get_action ("r");
    tap.tassert ("NamedType::get_action", r1 == NULL && r2 == r);
  }

  {
    NamedType foo ("foo", new Component (NULL));
    Reaction* r1 = foo.get_reaction ("r");
    Reaction* r = new Reaction (NULL, NULL, "r", NULL);
    foo.insert_reaction (r);
    Reaction* r2 = foo.get_reaction ("r");
    tap.tassert ("NamedType::get_reaction", r1 == NULL && r2 == r);
  }

  {
    NamedType foo ("foo", new Component (NULL));
    Bind* r1 = foo.get_bind ("r");
    Bind* r = new Bind (NULL, "r", NULL);
    foo.insert_bind (r);
    Bind* r2 = foo.get_bind ("r");
    tap.tassert ("NamedType::get_bind", r1 == NULL && r2 == r);
  }

  {
    util::Location loc;
    NamedType foo ("foo", new Component (NULL));
    decl::Method* r1 = foo.get_method ("r");
    decl::Method* r = new decl::Method (NULL, "r", new type::Method (type::Method::METHOD, &foo, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), new Signature (), ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable)));
    foo.insert_method (r);
    decl::Method* r2 = foo.get_method ("r");
    tap.tassert ("NamedType::get_method", r1 == NULL && r2 == r);
  }

  {
    util::Location loc;
    NamedType foo ("foo", new Component (NULL));
    Initializer* r1 = foo.get_initializer ("r");
    Initializer* r = new Initializer (NULL, "r", new type::Method (type::Method::INITIALIZER, &foo, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), new Signature (), ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable)));
    foo.insert_initializer (r);
    Initializer* r2 = foo.get_initializer ("r");
    tap.tassert ("NamedType::get_initializer", r1 == NULL && r2 == r);
  }

  {
    util::Location loc;
    NamedType foo ("foo", new Component (NULL));
    Getter* r1 = foo.get_getter ("r");
    Getter* r = new Getter (NULL, "r", new type::Method (type::Method::GETTER, &foo, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), new Signature (), ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable)));
    foo.insert_getter (r);
    Getter* r2 = foo.get_getter ("r");
    tap.tassert ("NamedType::get_getter", r1 == NULL && r2 == r);
  }

  {
    Struct s;
    s.append_field (NULL, false, "r", &named_int, TagSet ());
    tap.tassert ("Struct::to_string", s.to_string () == "{r int;}");
  }

  {
    Struct s;
    Field* f1 = s.get_field ("r");
    s.append_field (NULL, false, "r", &named_int, TagSet ());
    Field* f2 = s.get_field ("r");
    tap.tassert ("Struct::get_field", f1 == NULL && f2 != NULL);
  }

  {
    util::Location loc;

    Struct s;
    s.append_field (NULL, false, "field", &named_int, TagSet ());

    NamedType nt ("foo", &s);

    decl::Method* method = new decl::Method (NULL, "method", new type::Method (type::Method::METHOD, &nt, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), new Signature (), ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable)));
    Initializer* initializer = new Initializer (NULL, "initializer", new type::Method (type::Method::INITIALIZER, &nt, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), new Signature (), ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable)));
    Getter* getter = new Getter (NULL, "getter", new type::Method (type::Method::GETTER, &nt, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), new Signature (), ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable)));
    Action* action = new Action (NULL, NULL, "action");
    Reaction* reaction = new Reaction (NULL, NULL, "reaction", NULL);
    Bind* bind = new Bind (NULL, "bind", NULL);

    nt.insert_method (method);
    nt.insert_initializer (initializer);
    nt.insert_getter (getter);
    nt.insert_action (action);
    nt.insert_reaction (reaction);
    nt.insert_bind (bind);

    {
      const Type* t = nt.select ("field");
      tap.tassert ("Type::select (field)", t == &named_int);
    }
    {
      const Type* t = nt.select ("method");
      tap.tassert ("Type::select (method)", t == method->type ());
    }
    {
      const Type* t = nt.select ("initializer");
      tap.tassert ("Type::select (initializer)", t == initializer->type ());
    }
    {
      const Type* t = nt.select ("getter");
      tap.tassert ("Type::select (getter)", t == getter->type ());
    }
    {
      const Type* t = nt.select ("action");
      tap.tassert ("Type::select (action)", t == Void::Instance ());
    }
    {
      const Type* t = nt.select ("reaction");
      tap.tassert ("Type::select (reaction)", t == reaction->type ());
    }
    {
      const Type* t = nt.select ("bind");
      tap.tassert ("Type::select (bind)", t == Void::Instance ());
    }
    {
      const Type* t = nt.select ("not there");
      tap.tassert ("Type::select (NULL)", t == NULL);
    }
    {
      util::Location loc;
      ParameterSymbol* p = ParameterSymbol::make (loc, "there", &named_int, Immutable, Immutable);
      Signature sig;
      sig.Append (p);
      tap.tassert ("Signature::find (NULL)", sig.find ("not there") == NULL && sig.find ("there") == p);
    }
  }
  {
    const Type* t = named_int.get_heap ()->get_pointer ();
    tap.tassert ("Type::move", t->move () == t);
  }
  {
    tap.tassert ("Type::move", named_int.move () == NULL);
  }
  {
    const Type* t = named_int.get_heap ()->get_pointer ();
    tap.tassert ("Type::merge", t->merge_change () == named_int.get_pointer ());
  }
  {
    tap.tassert ("Type::merge", named_int.merge_change () == NULL);
  }
  {
    tap.tassert ("type::are_identical - two named types true", are_identical (&named_int, &named_int));
  }
  {
    tap.tassert ("type::are_identical - two named types false", !are_identical (&named_int, &named_float32));
  }
  {
    tap.tassert ("type::are_identical - named, unnamed", !are_identical (&named_int, named_int.UnderlyingType ()));
  }
  {
    tap.tassert ("type::are_identical - array same type same length", are_identical (named_int.get_array (3), named_int.get_array (3)));
  }
  {
    tap.tassert ("type::are_identical - array same type different length", !are_identical (named_int.get_array (3), named_int.get_array (4)));
  }
  {
    tap.tassert ("type::are_identical - array different type same length", !are_identical (named_int.get_array (3), named_float32.get_array (3)));
  }
  {
    tap.tassert ("type::are_identical - array different type different length", !are_identical (named_int.get_array (3), named_float32.get_array (4)));
  }
  {
    tap.tassert ("type::are_identical - slice same type", are_identical (named_int.get_slice (), named_int.get_slice ()));
  }
  {
    tap.tassert ("type::are_identical - slice different type", !are_identical (named_int.get_slice (), named_float32.get_slice ()));
  }

  {
    Struct* s1 = (new Struct ())->append_field (NULL, false, "x", &named_int, TagSet ());
    Struct* s2 = (new Struct ())->append_field (NULL, false, "x", &named_int, TagSet ());
    tap.tassert ("type::are_identical - struct same", are_identical (s1, s2));
  }
  {
    Struct* s1 = (new Struct ())->append_field (NULL, true, "T", &named_int, TagSet ());
    Struct* s2 = (new Struct ())->append_field (NULL, true, "T", &named_int, TagSet ());
    tap.tassert ("type::are_identical - struct same anonymous field", are_identical (s1, s2));
  }
  {
    Struct* s1 = (new Struct ())->append_field (NULL, false, "T", &named_int, TagSet ());
    Struct* s2 = (new Struct ())->append_field (NULL, true, "T", &named_int, TagSet ());
    tap.tassert ("type::are_identical - struct different anonymous field", !are_identical (s1, s2));
  }
  {
    Struct* s1 = (new Struct ())->append_field (NULL, false, "x", &named_int, TagSet ());
    Struct* s2 = (new Struct ())->append_field (NULL, false, "y", &named_int, TagSet ());
    tap.tassert ("type::are_identical - struct different field name", !are_identical (s1, s2));
  }
  {
    Struct* s1 = (new Struct ())->append_field (NULL, false, "x", &named_int, TagSet ());
    Struct* s2 = (new Struct ())->append_field (NULL, false, "x", &named_float32, TagSet ());
    tap.tassert ("type::are_identical - struct different field type", !are_identical (s1, s2));
  }
  {
    TagSet ts;
    ts.insert ("some tag");
    Struct* s1 = (new Struct ())->append_field (NULL, false, "x", &named_int, ts);
    Struct* s2 = (new Struct ())->append_field (NULL, false, "x", &named_int, TagSet ());
    tap.tassert ("type::are_identical - struct different field tags", !are_identical (s1, s2));
  }
  {
    Struct* s1 = (new Struct ())->append_field (new Package (), false, "x", &named_int, TagSet ());
    Struct* s2 = (new Struct ())->append_field (new Package (), false, "x", &named_int, TagSet ());
    tap.tassert ("type::are_identical - struct different package with private field", !are_identical (s1, s2));
  }

  {
    const Pointer* p1 = (new Struct ())->get_pointer ();
    const Pointer* p2 = (new Struct ())->get_pointer ();
    tap.tassert ("type::are_identical - pointers same", are_identical (p1, p2));
  }

  {
    const Pointer* p1 = (new Struct ())->get_pointer ();
    const Pointer* p2 = (new Struct ())->append_field (NULL, false, "x", &named_int, TagSet ())->get_pointer ();
    tap.tassert ("type::are_identical - pointers different", !are_identical (p1, p2));
  }

  tap.print_plan ();

  return 0;
}
