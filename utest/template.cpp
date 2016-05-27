#include "template.hpp"

#include "tap.hpp"

#include "visitor_helper.hpp"
#include "symbol_visitor.hpp"

using namespace decl;
using namespace util;

struct MyTemplateSymbol : public TemplateSymbol
{
  MyTemplateSymbol (const std::string& id,
                    const util::Location& loc)
    : TemplateSymbol (id, loc)
  { }

  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      semantic::ExpressionValue& result,
                      semantic::ExpressionValueList& arguments) const { }
};

int
main (int argc, char** argv)
{
  Tap tap;

  {
    TestVisitor<SymbolVisitor, TemplateSymbol> visitor;
    MyTemplateSymbol ts ("id", Location ());
    ts.accept (visitor);
    tap.tassert ("TemplateSymbol::visit ()", visitor.item == &ts);
  }

  {
    TestConstVisitor<ConstSymbolVisitor, TemplateSymbol> visitor;
    MyTemplateSymbol ts ("id", Location ());
    ts.accept (visitor);
    tap.tassert ("TemplateSymbol::visit (Const)", visitor.item == &ts);
  }

  {
    MyTemplateSymbol ts ("id", Location ());
    tap.tassert ("TemplateSymbol::symbol_type", ts.symbol_type () == type::Template::instance ());
  }

  tap.print_plan ();

  return 0;
}
