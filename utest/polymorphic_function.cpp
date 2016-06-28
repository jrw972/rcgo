#include "polymorphic_function.hpp"

#include "tap.hpp"

#include "visitor_helper.hpp"
#include "symbol_visitor.hpp"

using namespace decl;
using namespace util;

struct MyPolymorphicFunction : public PolymorphicFunction
{
  MyPolymorphicFunction (const std::string& id,
                         const util::Location& loc)
    : PolymorphicFunction (id, loc)
  { }

  virtual void
  check (util::ErrorReporter& er,
         const util::Location& loc,
         semantic::ExpressionValue& result,
         semantic::ExpressionValueList& arguments) const
  { }

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const
  { }

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const
  {
    return NULL;
  }
};

int
main (int argc, char** argv)
{
  Tap tap;

  {
    TestVisitor<SymbolVisitor, PolymorphicFunction> visitor;
    MyPolymorphicFunction ts ("id", Location ());
    ts.accept (visitor);
    tap.tassert ("PolymorphicFunction::visit ()", visitor.item == &ts);
  }

  {
    TestConstVisitor<ConstSymbolVisitor, PolymorphicFunction> visitor;
    MyPolymorphicFunction ts ("id", Location ());
    ts.accept (visitor);
    tap.tassert ("PolymorphicFunction::visit (Const)", visitor.item == &ts);
  }

  tap.print_plan ();

  return 0;
}
