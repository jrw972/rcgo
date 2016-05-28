#include "type.hpp"

#include "tap.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "bind.hpp"
#include "package.hpp"
#include "parameter_list.hpp"
#include "visitor_helper.hpp"

#include <sstream>

using namespace type;
using namespace decl;

template <typename T>
void
scalar_test (Tap& tap,
             const std::string& type,
             const std::string& rep,
             size_t alignment,
             size_t size,
             type::Kind kind)
{
  {
    TestConstVisitor <DefaultVisitor, T> visitor;
    T::instance ()->accept (visitor);
    tap.tassert (type + "::accept", visitor.item == T::instance ());
  }

  {
    std::stringstream str;
    T::instance ()->print (str);
    tap.tassert (type + "::print", str.str () == rep);
  }

  {
    tap.tassert (type + "::alignment", T::instance ()->alignment () == alignment);
  }

  {
    tap.tassert (type + "::size", T::instance ()->size () == size);
  }

  {
    tap.tassert (type + "::kind", T::instance ()->kind () == kind);
  }
}

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
    NamedType foo ("foo");
    tap.tassert ("NamedType::NamedType(1)", foo.underlying_type () == NULL);
  }

  {
    NamedType foo ("foo", Int::instance ());
    tap.tassert ("NamedType::NamedType(2)", foo.underlying_type () == Int::instance ());
  }

  {
    TestConstVisitor <DefaultVisitor, NamedType> visitor;
    NamedType foo ("foo");
    foo.accept (visitor);
    tap.tassert ("NamedType::accept", visitor.item == &foo);
  }

  {
    NamedType foo ("foo");
    std::stringstream str;
    foo.print (str);
    tap.tassert ("NamedType::print", str.str () == "foo");
  }

  {
    NamedType foo ("foo");
    foo.underlying_type (&named_int);
    tap.tassert ("NamedType::underlying_type", foo.underlying_type () == Int::instance ());
  }

  {
    NamedType foo ("foo", Int::instance ());
    tap.tassert ("NamedType::alignment", foo.alignment () == Int::instance ()->alignment ());
  }

  {
    NamedType foo ("foo", Int::instance ());
    tap.tassert ("NamedType::size", foo.size () == Int::instance ()->size ());
  }

  {
    NamedType foo ("foo");
    tap.tassert ("NamedType::kind", foo.kind () == Named_Kind);
  }

  {
    util::Location loc;
    NamedType foo ("foo", new Component (NULL));
    decl::Method* r1 = foo.find_method ("r");
    decl::Method* r = new decl::Method (NULL, "r", new type::Method (&foo, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), (new ParameterList (loc)), ((new ParameterList (loc)))->append (ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable))));
    foo.insert_method (r);
    decl::Method* r2 = foo.find_method ("r");
    tap.tassert ("NamedType::find_method", r1 == NULL && r2 == r);
  }

  {
    util::Location loc;
    NamedType foo ("foo", new Component (NULL));
    decl::Initializer* r1 = foo.find_initializer ("r");
    decl::Initializer* r = new decl::Initializer (NULL, "r", new type::Initializer (&foo, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), (new ParameterList (loc)), ((new ParameterList (loc)))->append (ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable))));
    foo.insert_initializer (r);
    decl::Initializer* r2 = foo.find_initializer ("r");
    tap.tassert ("NamedType::find_initializer", r1 == NULL && r2 == r);
  }

  {
    util::Location loc;
    NamedType foo ("foo", new Component (NULL));
    decl::Getter* r1 = foo.find_getter ("r");
    decl::Getter* r = new decl::Getter (NULL, "r", new type::Getter (&foo, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), (new ParameterList (loc)), ((new ParameterList (loc)))->append (ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable))));
    foo.insert_getter (r);
    decl::Getter* r2 = foo.find_getter ("r");
    tap.tassert ("NamedType::find_getter", r1 == NULL && r2 == r);
  }

  {
    NamedType foo ("foo", new Component (NULL));
    Action* r1 = foo.find_action ("r");
    Action* r = new Action (NULL, NULL, "r");
    foo.insert_action (r);
    Action* r2 = foo.find_action ("r");
    tap.tassert ("NamedType::find_action", r1 == NULL && r2 == r);
  }

  {
    NamedType foo ("foo", new Component (NULL));
    decl::Reaction* r1 = foo.find_reaction ("r");
    decl::Reaction* r = new decl::Reaction (NULL, "r", NULL);
    foo.insert_reaction (r);
    decl::Reaction* r2 = foo.find_reaction ("r");
    tap.tassert ("NamedType::find_reaction", r1 == NULL && r2 == r);
  }

  {
    NamedType foo ("foo", new Component (NULL));
    Bind* r1 = foo.find_bind ("r");
    Bind* r = new Bind (NULL, "r", NULL);
    foo.insert_bind (r);
    Bind* r2 = foo.find_bind ("r");
    tap.tassert ("NamedType::find_bind", r1 == NULL && r2 == r);
  }

  {
    util::Location loc;
    NamedType foo ("foo", new Component (NULL));
    decl::Method* r = new decl::Method (NULL, "r", new type::Method (&foo, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), (new ParameterList (loc)), ((new ParameterList (loc)))->append (ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable))));
    foo.insert_method (r);
    decl::Callable* r2 = foo.find_callable ("r");
    tap.tassert ("NamedType::find_callable", r2 == r);
  }

  {
    TestConstVisitor <DefaultVisitor, Void> visitor;
    Void::instance ()->accept (visitor);
    tap.tassert ("Void::accept", visitor.item == Void::instance ());
  }

  {
    std::stringstream str;
    Void::instance ()->print (str);
    tap.tassert ("Void::print", str.str () == "<void>");
  }

  {
    tap.tassert ("Void::alignment", Void::instance ()->alignment () == 0);
  }

  {
    tap.tassert ("Void::size", Void::instance ()->size () == 0);
  }

  {
    tap.tassert ("Void::kind", Void::instance ()->kind () == Void_Kind);
  }

  scalar_test<Bool> (tap, "Bool", "<bool>", 1, 1, Bool_Kind);
  scalar_test<Uint8> (tap, "Uint8", "<uint8>", 1, 1, Uint8_Kind);
  scalar_test<Uint16> (tap, "Uint16", "<uint16>", 2, 2, Uint16_Kind);
  scalar_test<Uint32> (tap, "Uint32", "<uint32>", 4, 4, Uint32_Kind);
  scalar_test<Uint64> (tap, "Uint64", "<uint64>", 8, 8, Uint64_Kind);
  scalar_test<Int8> (tap, "Int8", "<int8>", 1, 1, Int8_Kind);
  scalar_test<Int16> (tap, "Int16", "<int16>", 2, 2, Int16_Kind);
  scalar_test<Int32> (tap, "Int32", "<int32>", 4, 4, Int32_Kind);
  scalar_test<Int64> (tap, "Int64", "<int64>", 8, 8, Int64_Kind);
  scalar_test<Float32> (tap, "Float32", "<float32>", 4, 4, Float32_Kind);
  scalar_test<Float64> (tap, "Float64", "<float64>", 8, 8, Float64_Kind);
  scalar_test<Complex64> (tap, "Complex64", "<complex64>", 8, 8, Complex64_Kind);
  scalar_test<Complex128> (tap, "Complex128", "<complex128>", 16, 16, Complex128_Kind);
  scalar_test<Uint> (tap, "Uint", "<uint>", 8, 8, Uint_Kind);
  scalar_test<Int> (tap, "Int", "<int>", 8, 8, Int_Kind);
  scalar_test<Uintptr> (tap, "Uintpr", "<uintptr>", 8, 8, Uintptr_Kind);
  scalar_test<String> (tap, "String", "<string>", 16, 16, String_Kind);

  scalar_test<UntypedNil> (tap, "UntypedNil", "<<nil>>", 0, 0, Untyped_Nil_Kind);
  scalar_test<UntypedBoolean> (tap, "UntypedBoolean", "<<boolean>>", 0, 0, Untyped_Boolean_Kind);
  scalar_test<UntypedRune> (tap, "UntypedRune", "<<rune>>", 0, 0, Untyped_Rune_Kind);
  scalar_test<UntypedInteger> (tap, "UntypedInteger", "<<integer>>", 0, 0, Untyped_Integer_Kind);
  scalar_test<UntypedFloat> (tap, "UntypedFloat", "<<float>>", 0, 0, Untyped_Float_Kind);
  scalar_test<UntypedComplex> (tap, "UntypedComplex", "<<complex>>", 0, 0, Untyped_Complex_Kind);
  scalar_test<UntypedString> (tap, "UntypedString", "<<string>>", 0, 0, Untyped_String_Kind);
  scalar_test<PolymorphicFunction> (tap, "PolymorphicFunction", "<<polymorphic function>>", 0, 0, Polymorphic_Function_Kind);
  scalar_test<FileDescriptor> (tap, "FileDescriptor", "<FileDescriptor>", 8, 8, File_Descriptor_Kind);

  {
    TestConstVisitor <DefaultVisitor, Pointer> visitor;
    const Pointer* p = Int::instance ()->get_pointer ();
    p->accept (visitor);
    tap.tassert ("Pointer::accept", visitor.item == p);
  }

  {
    std::stringstream str;
    Int::instance ()->get_pointer ()->print (str);
    tap.tassert ("Pointer::print", str.str () == "*<int>");
  }

  {
    tap.tassert ("Pointer::alignment", Int::instance ()->get_pointer ()->alignment () == 8);
  }

  {
    tap.tassert ("Pointer::size", Int::instance ()->get_pointer ()->size () == 8);
  }

  {
    tap.tassert ("Pointer::kind", Int::instance ()->get_pointer ()->kind () == Pointer_Kind);
  }

  {
    Struct s;
    const Type* p = s.get_pointer ();
    Field* f1 = p->find_field ("r");
    s.append_field (NULL, false, "r", &named_int, TagSet ());
    Field* f2 = p->find_field ("r");
    tap.tassert ("Pointer::find_field", f1 == NULL && f2 != NULL);
  }

  {
    util::Location loc;
    NamedType foo ("foo", new Component (NULL));
    decl::Method* r = new decl::Method (NULL, "r", new type::Method (&foo, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), (new ParameterList (loc)), ((new ParameterList (loc)))->append (ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable))));
    foo.insert_method (r);
    decl::Callable* r2 = foo.get_pointer ()->find_callable ("r");
    tap.tassert ("Pointer::find_callable", r2 == r);
  }

  {
    TestConstVisitor <DefaultVisitor, Slice> visitor;
    const Slice* p = Int::instance ()->get_slice ();
    p->accept (visitor);
    tap.tassert ("Slice::accept", visitor.item == p);
  }

  {
    std::stringstream str;
    Int::instance ()->get_slice ()->print (str);
    tap.tassert ("Slice::print", str.str () == "[]<int>");
  }

  {
    tap.tassert ("Slice::alignment", Int::instance ()->get_slice ()->alignment () == 8);
  }

  {
    tap.tassert ("Slice::size", Int::instance ()->get_slice ()->size () == 24);
  }

  {
    tap.tassert ("Slice::kind", Int::instance ()->get_slice ()->kind () == Slice_Kind);
  }

  {
    tap.tassert ("Slice::unit_size", Int::instance ()->get_slice ()->unit_size () == 8);
  }

  {
    TestConstVisitor <DefaultVisitor, Array> visitor;
    const Array* p = Int::instance ()->get_array (3);
    p->accept (visitor);
    tap.tassert ("Array::accept", visitor.item == p);
  }

  {
    std::stringstream str;
    Int::instance ()->get_array (3)->print (str);
    tap.tassert ("Array::print", str.str () == "[3]<int>");
  }

  {
    tap.tassert ("Array::alignment", Int::instance ()->get_array (3)->alignment () == 8);
  }

  {
    tap.tassert ("Array::size", Int::instance ()->get_array (3)->size () == 24);
  }

  {
    tap.tassert ("Array::kind", Int::instance ()->get_array (3)->kind () == Array_Kind);
  }

  {
    tap.tassert ("Array::unit_size", Int::instance ()->get_array (3)->unit_size () == 8);
  }

  {
    const Map* m1 = Map::make (Int::instance (), Uint::instance ());
    const Map* m2 = Map::make (Int::instance (), Uint::instance ());
    tap.tassert ("Map::make", m1 != NULL && m1 == m2);
  }

  {
    TestConstVisitor <DefaultVisitor, Map> visitor;
    const Map* p = Map::make (Int::instance (), Uint::instance ());
    p->accept (visitor);
    tap.tassert ("Map::accept", visitor.item == p);
  }

  {
    std::stringstream str;
    Map::make (Int::instance (), Uint::instance ())->print (str);
    tap.tassert ("Map::print", str.str () == "map[<int>]<uint>");
  }

  {
    tap.tassert ("Map::alignment # TODO", Map::make (Int::instance (), Uint::instance ())->alignment () != 0);
  }

  {
    tap.tassert ("Map::size # TODO", Map::make (Int::instance (), Uint::instance ())->size () != 0);
  }

  {
    tap.tassert ("Map::kind", Map::make (Int::instance (), Uint::instance ())->kind () == Map_Kind);
  }

  {
    TestConstVisitor <DefaultVisitor, Heap> visitor;
    const Heap* p = Int::instance ()->get_heap ();
    p->accept (visitor);
    tap.tassert ("Heap::accept", visitor.item == p);
  }

  {
    std::stringstream str;
    Int::instance ()->get_heap ()->print (str);
    tap.tassert ("Heap::print", str.str () == "heap <int>");
  }

  {
    tap.tassert ("Heap::alignment # TODO", Int::instance ()->get_heap ()->alignment () != 0);
  }

  {
    tap.tassert ("Heap::size # TODO", Int::instance ()->get_heap ()->size () != 0);
  }

  {
    tap.tassert ("Heap::kind", Int::instance ()->get_heap ()->kind () == Heap_Kind);
  }

  {
    TestConstVisitor <DefaultVisitor, Struct> visitor;
    Struct s;
    s.accept (visitor);
    tap.tassert ("Struct::accept", visitor.item == &s);
  }

  {
    std::stringstream str;
    Struct s;
    s.print (str);
    tap.tassert ("Struct::print", str.str () == "struct {}");
  }

  {
    Struct s;
    s.append_field (NULL, false, "r", &named_int, TagSet ());
    tap.tassert ("Struct::alignment", s.alignment () == 8);
  }

  {
    Struct s;
    s.append_field (NULL, false, "r", &named_int, TagSet ());
    tap.tassert ("Struct::size", s.size () == 8);
  }

  {
    Struct s;
    tap.tassert ("Struct::kind", s.kind () == Struct_Kind);
  }

  {
    Struct s;
    s.append_field (NULL, false, "r", &named_int, TagSet ());
    tap.tassert ("Struct::to_string", s.to_string () == "struct {r int;}");
  }

  {
    Struct s;
    Field* f1 = s.find_field ("r");
    s.append_field (NULL, false, "r", &named_int, TagSet ());
    Field* f2 = s.find_field ("r");
    tap.tassert ("Struct::find_field", f1 == NULL && f2 != NULL);
    tap.tassert ("Struct::field_count", s.field_count () == 1);
  }

  {
    TestConstVisitor <DefaultVisitor, Component> visitor;
    Component s (NULL);
    s.accept (visitor);
    tap.tassert ("Component::accept", visitor.item == &s);
  }

  {
    std::stringstream str;
    Component s (NULL);
    s.print (str);
    tap.tassert ("Component::print", str.str () == "component {}");
  }

  {
    Component s (NULL);
    tap.tassert ("Component::kind", s.kind () == Component_Kind);
  }

  {
    util::Location loc;
    TestConstVisitor <DefaultVisitor, type::Function> visitor;
    type::Function f (new ParameterList (loc), new ParameterList (loc));
    f.accept (visitor);
    tap.tassert ("Function::accept", visitor.item == &f);
  }

  {
    util::Location loc;
    std::stringstream str;
    type::Function f (new ParameterList (loc), new ParameterList (loc));
    f.print (str);
    tap.tassert ("Function::print", str.str () == "func () ()");
  }

  {
    util::Location loc;
    type::Function f (new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("Function::alignment", f.alignment () == 8);
  }

  {
    util::Location loc;
    type::Function f (new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("Function::size", f.size () == 8);
  }

  {
    util::Location loc;
    type::Function f (new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("Function::kind", f.kind () == Function_Kind);
  }

  {
    util::Location loc;
    TestConstVisitor <DefaultVisitor, type::PushPort> visitor;
    type::PushPort f (new ParameterList (loc), new ParameterList (loc));
    f.accept (visitor);
    tap.tassert ("PushPort::accept", visitor.item == &f);
  }

  {
    util::Location loc;
    std::stringstream str;
    type::PushPort f (new ParameterList (loc), new ParameterList (loc));
    f.print (str);
    tap.tassert ("PushPort::print", str.str () == "push ()");
  }

  {
    util::Location loc;
    type::PushPort f (new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("PushPort::alignment", f.alignment () == 8);
  }

  {
    util::Location loc;
    type::PushPort f (new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("PushPort::size", f.size () == 8);
  }

  {
    util::Location loc;
    type::PushPort f (new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("PushPort::kind", f.kind () == Push_Port_Kind);
  }

  {
    util::Location loc;
    TestConstVisitor <DefaultVisitor, type::PullPort> visitor;
    type::PullPort f (new ParameterList (loc), new ParameterList (loc));
    f.accept (visitor);
    tap.tassert ("PullPort::accept", visitor.item == &f);
  }

  {
    util::Location loc;
    std::stringstream str;
    type::PullPort f (new ParameterList (loc), new ParameterList (loc));
    f.print (str);
    tap.tassert ("PullPort::print", str.str () == "pull () ()");
  }

  {
    util::Location loc;
    type::PullPort f (new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("PullPort::alignment", f.alignment () == 8);
  }

  {
    util::Location loc;
    type::PullPort f (new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("PullPort::size", f.size () == 16);
  }

  {
    util::Location loc;
    type::PullPort f (new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("PullPort::kind", f.kind () == Pull_Port_Kind);
  }

  {
    util::Location loc;
    TestConstVisitor <DefaultVisitor, type::Method> visitor;
    type::Method f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    f.accept (visitor);
    tap.tassert ("Method::accept", visitor.item == &f);
  }

  {
    util::Location loc;
    std::stringstream str;
    type::Method f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    f.print (str);
    tap.tassert ("Method::print", str.str () == "(int) func () ()");
  }

  {
    util::Location loc;
    type::Method f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("Method::kind", f.kind () == Method_Kind);
  }

  {
    util::Location loc;
    TestConstVisitor <DefaultVisitor, type::Initializer> visitor;
    type::Initializer f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    f.accept (visitor);
    tap.tassert ("Initializer::accept", visitor.item == &f);
  }

  {
    util::Location loc;
    std::stringstream str;
    type::Initializer f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    f.print (str);
    tap.tassert ("Initializer::print", str.str () == "(int) init () ()");
  }

  {
    util::Location loc;
    type::Initializer f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("Initializer::kind", f.kind () == Initializer_Kind);
  }

  {
    util::Location loc;
    TestConstVisitor <DefaultVisitor, type::Getter> visitor;
    type::Getter f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    f.accept (visitor);
    tap.tassert ("Getter::accept", visitor.item == &f);
  }

  {
    util::Location loc;
    std::stringstream str;
    type::Getter f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    f.print (str);
    tap.tassert ("Getter::print", str.str () == "(int) getter () ()");
  }

  {
    util::Location loc;
    type::Getter f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("Getter::kind", f.kind () == Getter_Kind);
  }

  {
    util::Location loc;
    TestConstVisitor <DefaultVisitor, type::Reaction> visitor;
    type::Reaction f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    f.accept (visitor);
    tap.tassert ("Reaction::accept", visitor.item == &f);
  }

  {
    util::Location loc;
    std::stringstream str;
    type::Reaction f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    f.print (str);
    tap.tassert ("Reaction::print", str.str () == "(int) reaction ()");
  }

  {
    util::Location loc;
    type::Reaction f (&named_int, ParameterSymbol::makeReceiver (loc, "this", &named_int, Immutable, Immutable), new ParameterList (loc), new ParameterList (loc));
    tap.tassert ("Reaction::kind", f.kind () == Reaction_Kind);
  }

  {
    TestConstVisitor <DefaultVisitor, type::Interface> visitor;
    type::Interface f (NULL);
    f.accept (visitor);
    tap.tassert ("Interface::accept", visitor.item == &f);
  }

  {
    std::stringstream str;
    type::Interface f (NULL);
    f.print (str);
    tap.tassert ("Interface::print", str.str () == "interface {}");
  }

  {
    type::Interface f (NULL);
    tap.tassert ("Interface::alignment # TODO", f.alignment () != 0);
  }

  {
    type::Interface f (NULL);
    tap.tassert ("Interface::size # TODO", f.size () != 0);
  }

  {
    type::Interface f (NULL);
    tap.tassert ("Interface::kind", f.kind () == Interface_Kind);
  }

  {
    type::Interface f (NULL);
    const Type* t = &f;
    tap.tassert ("Interface::to_interface", t->to_interface () != NULL);
  }

  {
    util::Location loc;

    Struct s;
    s.append_field (NULL, false, "field", &named_int, TagSet ());

    NamedType nt ("foo", &s);

    decl::Method* method = new decl::Method (NULL, "method", new type::Method (&nt, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), (new ParameterList (loc)), ((new ParameterList (loc)))->append (ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable))));
    decl::Initializer* initializer = new decl::Initializer (NULL, "initializer", new type::Initializer (&nt, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), (new ParameterList (loc)), ((new ParameterList (loc)))->append (ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable))));
    decl::Getter* getter = new decl::Getter (NULL, "getter", new type::Getter (&nt, ParameterSymbol::makeReceiver (loc, "", &type::named_int, Mutable, Mutable), (new ParameterList (loc)), ((new ParameterList (loc)))->append (ParameterSymbol::makeReturn (loc, "", &type::named_int, Immutable))));
    Action* action = new Action (NULL, NULL, "action");
    decl::Reaction* reaction = new decl::Reaction (NULL, "reaction", NULL);
    Bind* bind = new Bind (NULL, "bind", NULL);

    nt.insert_method (method);
    nt.insert_initializer (initializer);
    nt.insert_getter (getter);
    nt.insert_action (action);
    nt.insert_reaction (reaction);
    nt.insert_bind (bind);

    {
      const Type* t = nt.find ("field");
      tap.tassert ("Type::select (field)", t == &named_int);
    }
    {
      const Type* t = nt.find ("method");
      tap.tassert ("Type::select (method)", t == method->callable_type ());
    }
    {
      const Type* t = nt.find ("initializer");
      tap.tassert ("Type::select (initializer)", t == initializer->callable_type ());
    }
    {
      const Type* t = nt.find ("getter");
      tap.tassert ("Type::select (getter)", t == getter->callable_type ());
    }
    {
      const Type* t = nt.find ("action");
      tap.tassert ("Type::select (action)", t == Void::instance ());
    }
    {
      const Type* t = nt.find ("reaction");
      tap.tassert ("Type::select (reaction)", t == reaction->callable_type ());
    }
    {
      const Type* t = nt.find ("bind");
      tap.tassert ("Type::select (bind)", t == Void::instance ());
    }
    {
      const Type* t = nt.find ("not there");
      tap.tassert ("Type::select (NULL)", t == NULL);
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
    tap.tassert ("type::are_identical - named, unnamed", !are_identical (&named_int, named_int.underlying_type ()));
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

  {
    const Heap* p1 = (new Struct ())->get_heap ();
    const Heap* p2 = (new Struct ())->get_heap ();
    tap.tassert ("type::are_identical - heaps same", are_identical (p1, p2));
  }

  {
    const Heap* p1 = (new Struct ())->get_heap ();
    const Heap* p2 = (new Struct ())->append_field (NULL, false, "x", &named_int, TagSet ())->get_heap ();
    tap.tassert ("type::are_identical - heaps different", !are_identical (p1, p2));
  }

  {
    util::Location loc;
    type::Function f1 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    type::Function f2 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    tap.tassert ("type::are_identical - functions same", are_identical (&f1, &f2));
  }
  {
    util::Location loc;
    type::Function f1 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    type::PushPort f2 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    tap.tassert ("type::are_identical - functions different kind", !are_identical (&f1, &f2));
  }
  {
    util::Location loc;
    type::Function f1 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    type::Function f2 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    tap.tassert ("type::are_identical - functions different parameter list", !are_identical (&f1, &f2));
  }
  {
    util::Location loc;
    type::Function f1 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable)));
    type::Function f2 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    tap.tassert ("type::are_identical - functions different return parameter list", !are_identical (&f1, &f2));
  }
  {
    util::Location loc;
    type::Function f1 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_float32, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    type::Function f2 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    tap.tassert ("type::are_identical - functions different parameter type", !are_identical (&f1, &f2));
  }
  {
    util::Location loc;
    type::Function f1 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_int, Immutable)));
    type::Function f2 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    tap.tassert ("type::are_identical - functions different return parameter type", !are_identical (&f1, &f2));
  }
  {
    util::Location loc;
    type::Function f1 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable))
                       ->set_variadic (true));
    type::Function f2 ((new ParameterList (loc))
                       ->append (ParameterSymbol::make (loc, "p", &named_int, Immutable, Immutable))
                       ->append (ParameterSymbol::make (loc, "q", &named_int, Immutable, Immutable)),
                       (new ParameterList (loc))
                       ->append (ParameterSymbol::makeReturn (loc, "r", &named_float32, Immutable))
                       ->append (ParameterSymbol::makeReturn (loc, "s", &named_float32, Immutable)));
    tap.tassert ("type::are_identical - functions different variadic", !are_identical (&f1, &f2));
  }

  {
    util::Location loc;
    Interface i1 (NULL);
    i1.methods["func1"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    i1.methods["Func2"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    Interface i2 (NULL);
    i2.methods["func1"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    i2.methods["Func2"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    tap.tassert ("type::are_identical - interfaces same", are_identical (&i1, &i2));
  }

  {
    util::Location loc;
    Interface i1 (NULL);
    i1.methods["func1"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    i1.methods["Func2"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    Interface i2 (NULL);
    i2.methods["Func2"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    tap.tassert ("type::are_identical - interfaces different method counts", !are_identical (&i1, &i2));
  }

  {
    util::Location loc;
    Interface i1 (NULL);
    i1.methods["func1"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    i1.methods["Func2"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    Interface i2 (NULL);
    i2.methods["skunk1"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    i2.methods["Func2"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    tap.tassert ("type::are_identical - interfaces different method names", !are_identical (&i1, &i2));
  }

  {
    util::Location loc;
    Interface i1 (NULL);
    i1.methods["func1"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    i1.methods["Func2"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    Interface i2 (NULL);
    i2.methods["func1"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    i2.methods["Func2"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_float32, Immutable)));
    tap.tassert ("type::are_identical - interfaces different signatures", !are_identical (&i1, &i2));
  }

  {
    util::Location loc;
    Interface i1 (new Package ());
    i1.methods["func1"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    i1.methods["Func2"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    Interface i2 (new Package ());
    i2.methods["func1"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    i2.methods["Func2"] = new type::Function ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    tap.tassert ("type::are_identical - interfaces different packages", !are_identical (&i1, &i2));
  }

  {
    const Map* m1 = Map::make (&named_int, &named_float32);
    const Map* m2 = Map::make (&named_int, &named_float32);
    tap.tassert ("type::are_identical - maps same", are_identical (m1, m2));
  }

  {
    const Map* m1 = Map::make (&named_float32, &named_float32);
    const Map* m2 = Map::make (&named_int, &named_float32);
    tap.tassert ("type::are_identical - maps different keys", !are_identical (m1, m2));
  }

  {
    const Map* m1 = Map::make (&named_int, &named_int);
    const Map* m2 = Map::make (&named_int, &named_float32);
    tap.tassert ("type::are_identical - maps different values", !are_identical (m1, m2));
  }

  {
    tap.tassert ("type::are_identical - bytes alias for uint8", are_identical (&named_byte, &named_uint8) && are_identical (&named_uint8, &named_byte));
  }

  {
    tap.tassert ("type::are_identical - simple types same", are_identical (Int::instance (), Int::instance ()));
    tap.tassert ("type::are_identical - simple types different", !are_identical (Int::instance (), Int32::instance ()));
  }

  {
    NamedType x ("x", named_int.get_pointer ());
    tap.tassert ("type::to_pointer", named_int.to_pointer () == NULL && named_int.get_pointer ()->to_pointer () != NULL && x.to_pointer () == NULL);
  }

  {
    tap.tassert ("type::to_named_type", named_int.to_named_type () != NULL && Int::instance ()->to_named_type () == NULL);
  }

  {
    tap.tassert ("type::to_array", named_int.to_array () == NULL && named_int.get_array (3)->to_array () != NULL);
  }

  {
    tap.tassert ("type::to_slice", named_int.to_slice () == NULL && named_int.get_slice ()->to_slice () != NULL);
  }

  {
    Struct s;
    tap.tassert ("type::to_struct", named_int.to_struct () == NULL && s.to_struct () != NULL);
  }

  {
    util::Location loc;
    type::Function f ((new ParameterList (loc)), (new ParameterList (loc))->append (ParameterSymbol::makeReturn (loc, "", &named_int, Immutable)));
    tap.tassert ("type::to_function", named_int.to_function () == NULL && f.to_function () != NULL);
  }

  {
    Interface i (NULL);
    tap.tassert ("type::to_interface", named_int.to_interface () == NULL && i.to_interface () != NULL);
  }

  {
    const Map* m = Map::make (&named_int, &named_int);
    tap.tassert ("type::to_map", named_int.to_map () == NULL && m->to_map () != NULL);
  }

  tap.print_plan ();

  return 0;
}
