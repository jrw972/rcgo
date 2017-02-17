#include "tap.hpp"
#include "scope.hpp"
#include "parameter_list.hpp"
#include "type.hpp"
#include "symbol.hpp"
#include "symbol_cast.hpp"
#include "identifier.hpp"

using namespace type;
using namespace decl;
using namespace source;

int
main (int argc, char** argv)
{
  Tap tap;
  const Location& loc = Location::builtin;

  {
    Scope st;
    tap.tassert ("SymbolTable::SymbolTable ()",
                 st.package () == NULL
                 && st.return_parameter_list () == NULL);

  }

  {
    Scope* st = new Scope ();
    Symbol* s1 = new Variable (Identifier ("x", loc), Immutable, Immutable, &named_int);
    st->enter_symbol (s1);
    Symbol* s = st->find_local_symbol ("x");
    tap.tassert ("SymbolTable::find_local_symbol yes", s == s1);
    st = st->open ();
    s = st->find_local_symbol ("x");
    tap.tassert ("SymbolTable::find_local_symbol no", s == NULL);
    st = st->close ();
    st = st->close ();
  }

  {
    Scope* st = new Scope ();
    Symbol* s1 = new Variable (Identifier ("x", loc), Immutable, Immutable, &named_int);
    st->enter_symbol (s1);
    st = st->open ();

    tap.tassert ("SymbolTable::find_global_symbol yes", st->find_global_symbol ("x") == s1);
    tap.tassert ("SymbolTable::find_global_symbol no", st->find_global_symbol ("y") == NULL);
    st = st->close ();
    st = st->close ();
  }

  {
    Parameter* x = Parameter::make (Identifier ("x", loc), Immutable, Immutable, &named_int);
    Parameter* y = Parameter::make_return (Identifier ("y", loc), Immutable, &named_int);
    ParameterList* pl = new ParameterList (loc);
    pl->append (x);
    ParameterList* rpl = new ParameterList (loc);
    rpl->append (y);
    Scope* st = new Scope ();
    st = st->open (pl, rpl);
    st = st->open ();

    tap.tassert ("SymbolTable::open_scope (pl, rpl)",
                 st->find_global_symbol ("x") == x
                 && st->find_global_symbol ("y") == y
                 && st->return_parameter_list () == rpl);

    st = st->close ();
    st = st->close ();
    st = st->close ();
  }

  {
    Parameter* iota = Parameter::make (Identifier ("IOTA", loc), Immutable, Immutable, &named_int);
    Parameter* x = Parameter::make (Identifier ("x", loc), Immutable, Immutable, &named_int);
    Parameter* y = Parameter::make_return (Identifier ("y", loc), Immutable, &named_int);
    ParameterList* pl = new ParameterList (loc);
    pl->append (x);
    ParameterList* rpl = new ParameterList (loc);
    rpl->append (y);
    Scope* st = new Scope ();
    st = st->open (iota, pl, rpl);
    st = st->open ();

    tap.tassert ("SymbolTable::open_scope (IOTA, pl, rpl)",
                 st->find_global_symbol ("IOTA") == iota
                 && st->find_global_symbol ("x") == x
                 && st->find_global_symbol ("y") == y
                 && st->return_parameter_list () == rpl);

    st = st->close ();
    st = st->close ();
    st = st->close ();
  }

  {
    Parameter* r = Parameter::make_receiver (Identifier ("this", loc), Immutable, Immutable, &named_int);
    Parameter* x1 = Parameter::make (Identifier ("x1", loc), Immutable, Immutable, &named_int);
    Parameter* x2 = Parameter::make (Identifier ("x2", loc), Immutable, Foreign, named_int.get_pointer ());
    Parameter* x3 = Parameter::make (Identifier ("x3", loc), Immutable, Foreign, named_int.get_pointer ());

    Variable v1 (Identifier ("v1", loc), Immutable, Mutable, named_int.get_pointer ());
    Variable v2 (Identifier ("v2", loc), Immutable, Foreign, named_int.get_pointer ());

    Hidden h3 (x3);

    ParameterList* pl = new ParameterList (loc);
    pl->append (r)->append (x1)->append (x2)->append (x3);
    ParameterList* rpl = new ParameterList (loc);
    Scope* st = new Scope ();
    st = st->open (pl, rpl);
    st = st->open ();
    st->enter_symbol (&v1);
    st->enter_symbol (&v2);
    st->enter_symbol (&h3);
    st = st->open ();
    st->activate ();

    const Parameter* r_after = symbol_cast<Parameter> (st->find_global_symbol ("this"));
    const Parameter* x1_after = symbol_cast<Parameter> (st->find_global_symbol ("x1"));
    const Hidden* x2_after = symbol_cast<Hidden> (st->find_global_symbol ("x2"));
    const Variable* v1_after = symbol_cast<Variable> (st->find_global_symbol ("v1"));
    const Hidden* v2_after = symbol_cast<Hidden> (st->find_global_symbol ("v2"));
    const Hidden* x3_after = symbol_cast<Hidden> (st->find_global_symbol ("x3"));

    tap.tassert ("SymbolTable::activate ()",
                 r_after->indirection_mutability == Mutable
                 && x1_after == x1
                 && x2_after != NULL
                 && v1_after == &v1
                 && v2_after != NULL
                 && x3_after == &h3);

    std::cout << (r_after->indirection_mutability == Mutable) << '\n'
              << (x1_after == x1) << '\n'
              << (x2_after != NULL) << '\n'
              << (v1_after == &v1) << '\n'
              << (v2_after != NULL) << '\n'
              << (x3_after == &h3) << '\n';
    std::cout << x3 << '\n';
    std::cout << &h3 << '\n';
    std::cout << x3_after << '\n';

    st = st->close ();
    st = st->close ();
    st = st->close ();
    st = st->close ();
  }

  {
    Parameter* r = Parameter::make_receiver (Identifier ("this", loc), Immutable, Immutable, named_int.get_pointer ());
    Parameter* x1 = Parameter::make (Identifier ("x1", loc), Immutable, Immutable, &named_int);
    Parameter* x2 = Parameter::make (Identifier ("x2", loc), Immutable, Immutable, named_int.get_pointer ());
    Parameter* x3 = Parameter::make (Identifier ("x3", loc), Immutable, Foreign, named_int.get_pointer ());

    Variable v1 (Identifier ("v1", loc), Immutable, Mutable, &named_int);
    Variable v2 (Identifier ("v2", loc), Immutable, Mutable, named_int.get_pointer ());

    Hidden h3 (x3);

    ParameterList* pl = new ParameterList (loc);
    pl->append (r)->append (x1)->append (x2)->append (x3);
    ParameterList* rpl = new ParameterList (loc);
    Scope* st = new Scope ();
    st = st->open (pl, rpl);
    st = st->open ();
    st->enter_symbol (&v1);
    st->enter_symbol (&v2);
    st->enter_symbol (&h3);
    st = st->open ();
    st->change ();

    const Parameter* r_after = symbol_cast<Parameter> (st->find_global_symbol ("this"));
    const Parameter* x1_after = symbol_cast<Parameter> (st->find_global_symbol ("x1"));
    const Parameter* x2_after = symbol_cast<Parameter> (st->find_global_symbol ("x2"));
    const Variable* v1_after = symbol_cast<Variable> (st->find_global_symbol ("v1"));
    const Variable* v2_after = symbol_cast<Variable> (st->find_global_symbol ("v2"));
    const Hidden* x3_after = symbol_cast<Hidden> (st->find_global_symbol ("x3"));

    tap.tassert ("SymbolTable::change ()",
                 r_after->indirection_mutability == Foreign
                 && x1_after == x1
                 && x2_after->indirection_mutability == Foreign
                 && v1_after == &v1
                 && v2_after->indirection_mutability == Foreign
                 && x3_after == &h3);

    st = st->close ();
    st = st->close ();
    st = st->close ();
    st = st->close ();
  }

  tap.print_plan ();

  return 0;
}
