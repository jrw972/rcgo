#include "tap.hpp"
#include "symbol_table.hpp"
#include "parameter_list.hpp"
#include "type.hpp"
#include "symbol.hpp"
#include "symbol_cast.hpp"

using namespace type;
using namespace decl;

int
main (int argc, char** argv)
{
  Tap tap;
  util::Location loc;

  {
    SymbolTable st;
    tap.tassert ("SymbolTable::SymbolTable ()",
                 st.package () == NULL
                 && st.return_parameter_list () == NULL);

  }

  {
    SymbolTable st;
    st.open_scope ();
    Symbol* s1 = new Variable ("x", loc, &named_int, Immutable, Immutable);
    st.enter_symbol (s1);
    Symbol* s = st.find_local_symbol ("x");
    tap.tassert ("SymbolTable::find_local_symbol yes", s == s1);
    st.open_scope ();
    s = st.find_local_symbol ("x");
    tap.tassert ("SymbolTable::find_local_symbol no", s == NULL);
    st.close_scope ();
    st.close_scope ();
  }

  {
    SymbolTable st;
    st.open_scope ();
    Symbol* s1 = new Variable ("x", loc, &named_int, Immutable, Immutable);
    st.enter_symbol (s1);
    st.open_scope ();

    tap.tassert ("SymbolTable::find_global_symbol yes", st.find_global_symbol ("x") == s1);
    tap.tassert ("SymbolTable::find_global_symbol no", st.find_global_symbol ("y") == NULL);
    st.close_scope ();
    st.close_scope ();
  }

  {
    Parameter* x = Parameter::make (loc, "x", &named_int, Immutable, Immutable);
    Parameter* y = Parameter::make_return (loc, "y", &named_int, Immutable);
    ParameterList* pl = new ParameterList (loc);
    pl->append (x);
    ParameterList* rpl = new ParameterList (loc);
    rpl->append (y);
    SymbolTable st;
    st.open_scope (pl, rpl);
    st.open_scope ();

    tap.tassert ("SymbolTable::open_scope (pl, rpl)",
                 st.find_global_symbol ("x") == x
                 && st.find_global_symbol ("y") == y
                 && st.return_parameter_list () == rpl);

    st.close_scope ();
    st.close_scope ();
  }

  {
    Parameter* iota = Parameter::make (loc, "IOTA", &named_int, Immutable, Immutable);
    Parameter* x = Parameter::make (loc, "x", &named_int, Immutable, Immutable);
    Parameter* y = Parameter::make_return (loc, "y", &named_int, Immutable);
    ParameterList* pl = new ParameterList (loc);
    pl->append (x);
    ParameterList* rpl = new ParameterList (loc);
    rpl->append (y);
    SymbolTable st;
    st.open_scope (iota, pl, rpl);
    st.open_scope ();

    tap.tassert ("SymbolTable::open_scope (IOTA, pl, rpl)",
                 st.find_global_symbol ("IOTA") == iota
                 && st.find_global_symbol ("x") == x
                 && st.find_global_symbol ("y") == y
                 && st.return_parameter_list () == rpl);

    st.close_scope ();
    st.close_scope ();
  }

  {
    Parameter* r = Parameter::make_receiver (loc, "this", &named_int, Immutable, Immutable);
    Parameter* x1 = Parameter::make (loc, "x1", &named_int, Immutable, Immutable);
    Parameter* x2 = Parameter::make (loc, "x2", named_int.get_pointer (), Immutable, Foreign);
    Parameter* x3 = Parameter::make (loc, "x3", named_int.get_pointer (), Immutable, Foreign);

    Variable v1 ("v1", loc, named_int.get_pointer (), Immutable, Mutable);
    Variable v2 ("v2", loc, named_int.get_pointer (), Immutable, Foreign);

    Hidden h3 (x3, loc);

    ParameterList* pl = new ParameterList (loc);
    pl->append (r)->append (x1)->append (x2)->append (x3);
    ParameterList* rpl = new ParameterList (loc);
    SymbolTable st;
    st.open_scope (pl, rpl);
    st.open_scope ();
    st.enter_symbol (&v1);
    st.enter_symbol (&v2);
    st.enter_symbol (&h3);
    st.open_scope ();
    st.activate ();

    const Parameter* r_after = symbol_cast<Parameter> (st.find_global_symbol ("this"));
    const Parameter* x1_after = symbol_cast<Parameter> (st.find_global_symbol ("x1"));
    const Hidden* x2_after = symbol_cast<Hidden> (st.find_global_symbol ("x2"));
    const Variable* v1_after = symbol_cast<Variable> (st.find_global_symbol ("v1"));
    const Hidden* v2_after = symbol_cast<Hidden> (st.find_global_symbol ("v2"));
    const Hidden* x3_after = symbol_cast<Hidden> (st.find_global_symbol ("x3"));

    tap.tassert ("SymbolTable::activate ()",
                 r_after->indirection_mutability == Mutable
                 && x1_after == x1
                 && x2_after != NULL
                 && v1_after == &v1
                 && v2_after != NULL
                 && x3_after == &h3);

    st.close_scope ();
    st.close_scope ();
    st.close_scope ();
  }

  {
    Parameter* r = Parameter::make_receiver (loc, "this", named_int.get_pointer (), Immutable, Immutable);
    Parameter* x1 = Parameter::make (loc, "x1", &named_int, Immutable, Immutable);
    Parameter* x2 = Parameter::make (loc, "x2", named_int.get_pointer (), Immutable, Immutable);
    Parameter* x3 = Parameter::make (loc, "x3", named_int.get_pointer (), Immutable, Foreign);

    Variable v1 ("v1", loc, &named_int, Immutable, Mutable);
    Variable v2 ("v2", loc, named_int.get_pointer (), Immutable, Mutable);

    Hidden h3 (x3, loc);

    ParameterList* pl = new ParameterList (loc);
    pl->append (r)->append (x1)->append (x2)->append (x3);
    ParameterList* rpl = new ParameterList (loc);
    SymbolTable st;
    st.open_scope (pl, rpl);
    st.open_scope ();
    st.enter_symbol (&v1);
    st.enter_symbol (&v2);
    st.enter_symbol (&h3);
    st.open_scope ();
    st.change ();

    const Parameter* r_after = symbol_cast<Parameter> (st.find_global_symbol ("this"));
    const Parameter* x1_after = symbol_cast<Parameter> (st.find_global_symbol ("x1"));
    const Parameter* x2_after = symbol_cast<Parameter> (st.find_global_symbol ("x2"));
    const Variable* v1_after = symbol_cast<Variable> (st.find_global_symbol ("v1"));
    const Variable* v2_after = symbol_cast<Variable> (st.find_global_symbol ("v2"));
    const Hidden* x3_after = symbol_cast<Hidden> (st.find_global_symbol ("x3"));

    tap.tassert ("SymbolTable::change ()",
                 r_after->indirection_mutability == Foreign
                 && x1_after == x1
                 && x2_after->indirection_mutability == Foreign
                 && v1_after == &v1
                 && v2_after->indirection_mutability == Foreign
                 && x3_after == &h3);

    st.close_scope ();
    st.close_scope ();
    st.close_scope ();
  }

  tap.print_plan ();

  return 0;
}
