#include "tap.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "callable.hpp"
#include "visitor_helper.hpp"
#include "symbol_visitor.hpp"

using namespace type;
using namespace decl;
using namespace util;

int
main (int argc, char** argv)
{
  Tap tap;

  Location loc;

  {
    decl::Initializer init ("blah", NULL);
    Instance symbol ("name", loc, &named_int, &init);
    tap.tassert ("Instance::Instance",
                 symbol.name == "name"
                 && symbol.location == loc
                 && symbol.in_progress == false
                 && symbol.defined () == true
                 && symbol.type == &named_int
                 && symbol.initializer == &init
                 && symbol.instance == NULL
                );
    symbol.offset (53);
    tap.tassert ("Instance::offset", symbol.offset () == 53);
    TestVisitor<SymbolVisitor, Instance> v;
    symbol.accept (v);
    tap.tassert ("Instance::accept(SymbolVisitor)", v.item == &symbol);
    TestConstVisitor<ConstSymbolVisitor, Instance> cv;
    symbol.accept (cv);
    tap.tassert ("Instance::accept(ConstSymbolVisitor)", cv.item == &symbol);
  }

  {
    Parameter* symbol = Parameter::make (loc, "name", &named_int, Immutable, Immutable);
    tap.tassert ("Parameter::make",
                 symbol->name == "name"
                 && symbol->location == loc
                 && symbol->in_progress == false
                 && symbol->defined () == true
                 && symbol->type == &named_int
                 && symbol->intrinsic_mutability == Immutable
                 && symbol->indirection_mutability == Immutable
                 && symbol->kind == Parameter::Ordinary
                );
    static_cast<Symbol*> (symbol)->offset (53);
    tap.tassert ("Parameter::offset", symbol->offset () == 53);
    TestVisitor<SymbolVisitor, Parameter> v;
    symbol->accept (v);
    tap.tassert ("Parameter::accept(SymbolVisitor)", v.item == symbol);
    TestConstVisitor<ConstSymbolVisitor, Parameter> cv;
    symbol->accept (cv);
    tap.tassert ("Parameter::accept(ConstSymbolVisitor)", cv.item == symbol);
  }

  {
    Parameter* symbol = Parameter::make_return (loc, "name", &named_int, Immutable);
    tap.tassert ("Parameter::make_return",
                 symbol->name == "name"
                 && symbol->location == loc
                 && symbol->in_progress == false
                 && symbol->defined () == true
                 && symbol->type == &named_int
                 && symbol->intrinsic_mutability == Mutable
                 && symbol->indirection_mutability == Immutable
                 && symbol->kind == Parameter::Return
                );
    static_cast<Symbol*> (symbol)->offset (53);
    tap.tassert ("Parameter::offset", symbol->offset () == 53);
    TestVisitor<SymbolVisitor, Parameter> v;
    symbol->accept (v);
    tap.tassert ("Parameter::accept(SymbolVisitor)", v.item == symbol);
    TestConstVisitor<ConstSymbolVisitor, Parameter> cv;
    symbol->accept (cv);
    tap.tassert ("Parameter::accept(ConstSymbolVisitor)", cv.item == symbol);
  }

  {
    Parameter* symbol = Parameter::make_receiver (loc, "name", &named_int, Immutable, Immutable);
    tap.tassert ("Parameter::make_receiver",
                 symbol->name == "name"
                 && symbol->location == loc
                 && symbol->in_progress == false
                 && symbol->defined () == true
                 && symbol->type == &named_int
                 && symbol->intrinsic_mutability == Immutable
                 && symbol->indirection_mutability == Immutable
                 && symbol->kind == Parameter::Receiver
                );
    static_cast<Symbol*> (symbol)->offset (53);
    tap.tassert ("Parameter::offset", symbol->offset () == 53);
    TestVisitor<SymbolVisitor, Parameter> v;
    symbol->accept (v);
    tap.tassert ("Parameter::accept(SymbolVisitor)", v.item == symbol);
    TestConstVisitor<ConstSymbolVisitor, Parameter> cv;
    symbol->accept (cv);
    tap.tassert ("Parameter::accept(ConstSymbolVisitor)", cv.item == symbol);
  }

  {
    Parameter* original = Parameter::make (loc, "name", &named_int, Immutable, Immutable);
    Parameter* symbol = original->duplicate (Mutable);
    Parameter* dup = symbol->duplicate (Foreign);
    static_cast<Symbol*> (original)->offset (34);
    tap.tassert ("Parameter::duplicate",
                 symbol->name == "name"
                 && symbol->location == loc
                 && symbol->in_progress == false
                 && symbol->defined () == true
                 && symbol->type == &named_int
                 && symbol->intrinsic_mutability == Immutable
                 && symbol->indirection_mutability == Mutable
                 && symbol->kind == Parameter::Ordinary_Duplicate
                 && symbol->offset () == 34
                 && dup->indirection_mutability == Foreign
                 && dup->offset () == 34
                );
  }

  {
    Parameter* original = Parameter::make_receiver (loc, "name", &named_int, Immutable, Immutable);
    Parameter* symbol = original->duplicate (Mutable);
    Parameter* dup = symbol->duplicate (Foreign);
    static_cast<Symbol*> (original)->offset (34);
    tap.tassert ("Parameter::duplicate",
                 symbol->name == "name"
                 && symbol->location == loc
                 && symbol->in_progress == false
                 && symbol->defined () == true
                 && symbol->type == &named_int
                 && symbol->intrinsic_mutability == Immutable
                 && symbol->indirection_mutability == Mutable
                 && symbol->kind == Parameter::Receiver_Duplicate
                 && symbol->offset () == 34
                 && dup->indirection_mutability == Foreign
                 && dup->offset () == 34
                );
  }

  {
    Parameter* safe1_symbol = Parameter::make (loc, "name", &named_int, Immutable, Immutable);
    Parameter* safe2_symbol = Parameter::make (loc, "name", named_int.get_pointer (), Immutable, Foreign);
    Parameter* unsafe_symbol = Parameter::make (loc, "name", named_int.get_pointer (), Immutable, Immutable);

    tap.tassert ("Parameter::is_foreign_safe",
                 safe1_symbol->is_foreign_safe ()
                 && safe2_symbol->is_foreign_safe ()
                 && !unsafe_symbol->is_foreign_safe ());
  }

  {
    semantic::Value value;
    value.present = true;
    value.int_value = 37;
    Constant symbol ("name", loc, &named_int, value);
    tap.tassert ("Constant::Constant",
                 symbol.name == "name"
                 && symbol.location == loc
                 && symbol.in_progress == false
                 && symbol.defined () == true
                 && symbol.type == &named_int
                 && symbol.value.int_value == 37
                );
    TestVisitor<SymbolVisitor, Constant> v;
    symbol.accept (v);
    tap.tassert ("Constant::accept(SymbolVisitor)", v.item == &symbol);
    TestConstVisitor<ConstSymbolVisitor, Constant> cv;
    symbol.accept (cv);
    tap.tassert ("Constant::accept(ConstSymbolVisitor)", cv.item == &symbol);
  }

  {
    Variable symbol ("name", loc, &named_int, Immutable, Immutable);
    Variable* dup = symbol.duplicate ();
    static_cast<Symbol&> (symbol).offset (53);
    tap.tassert ("Variable::Variable",
                 symbol.name == "name"
                 && symbol.location == loc
                 && symbol.in_progress == false
                 && symbol.defined () == true
                 && symbol.type == &named_int
                 && symbol.intrinsic_mutability == Immutable
                 && symbol.indirection_mutability == Immutable

                 && dup->name == "name"
                 && dup->location == loc
                 && dup->in_progress == false
                 && dup->defined () == true
                 && dup->type == &named_int
                 && dup->intrinsic_mutability == Foreign
                 && dup->indirection_mutability == Foreign

                );
    tap.tassert ("Parameter::offset", dup->offset () == 53);
    TestVisitor<SymbolVisitor, Variable> v;
    symbol.accept (v);
    tap.tassert ("Variable::accept(SymbolVisitor)", v.item == &symbol);
    TestConstVisitor<ConstSymbolVisitor, Variable> cv;
    symbol.accept (cv);
    tap.tassert ("Variable::accept(ConstSymbolVisitor)", cv.item == &symbol);
  }

  {
    Variable var ("name", loc, &named_int, Immutable, Immutable);
    Hidden symbol (&var, loc);
    tap.tassert ("Hidden::Hidden",
                 symbol.name == "name"
                 && symbol.location == loc
                 && symbol.in_progress == false
                 && symbol.defined () == true
                );
    TestVisitor<SymbolVisitor, Hidden> v;
    symbol.accept (v);
    tap.tassert ("Hidden::accept(SymbolVisitor)", v.item == &symbol);
    TestConstVisitor<ConstSymbolVisitor, Hidden> cv;
    symbol.accept (cv);
    tap.tassert ("Hidden::accept(ConstSymbolVisitor)", cv.item == &symbol);
  }

  tap.print_plan ();

  return 0;
}
