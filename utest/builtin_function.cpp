#include "builtin_function.hpp"

#include "tap.hpp"
#include "parameter_list.hpp"
#include "symbol_visitor.hpp"

#include "symbol_helpers.hpp"

using namespace decl;
using namespace util;
using namespace type;

struct MyBuiltinFunction : public BuiltinFunction
{
  MyBuiltinFunction (const std::string& id,
                     const util::Location& loc,
                     const type::Function* a_type)
    : BuiltinFunction (id, loc, a_type)
  { }

  virtual void call (runtime::ExecutorBase& exec) const { }
};

int
main (int argc, char** argv)
{
  Tap tap;

  {
    Location loc;
    const type::Function* func = new type::Function (type::Function::FUNCTION, new ParameterList (loc), new ParameterList (loc));
    MyBuiltinFunction bif ("func", loc, func);
    tap.tassert ("BuiltinFunction::BuiltinFunction ()",
                 bif.identifier == "func" &&
                 bif.location == loc &&
                 bif.symbol_type () == func &&
                 bif.callable_type () == func &&
                 bif.parameter_list () == func->parameter_list &&
                 bif.return_size_on_stack () == 0 &&
                 bif.receiver_size_on_stack () == 0 &&
                 bif.parameters_size_on_stack () == 0);
  }

  {
    TestSymbolVisitor<BuiltinFunction> visitor;
    Location loc;
    const type::Function* func = new type::Function (type::Function::FUNCTION, new ParameterList (loc), new ParameterList (loc));
    MyBuiltinFunction bif ("func", loc, func);
    bif.accept (visitor);
    tap.tassert ("BuiltinFunction::accept ()",
                 visitor.sym == &bif);
  }

  {
    TestConstSymbolVisitor<BuiltinFunction> visitor;
    Location loc;
    const type::Function* func = new type::Function (type::Function::FUNCTION, new ParameterList (loc), new ParameterList (loc));
    MyBuiltinFunction bif ("func", loc, func);
    bif.accept (visitor);
    tap.tassert ("BuiltinFunction::accept (Const)",
                 visitor.sym == &bif);
  }

  tap.print_plan ();

  return 0;
}
