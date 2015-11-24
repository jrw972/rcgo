#include "Symbol.hpp"
#include "SymbolVisitor.hpp"
#include "ast.hpp"
#include <error.h>

std::string const ReturnSymbol ("0return");

#define ACCEPT(type) void type::accept (SymbolVisitor& visitor) { visitor.visit (*this); } \
void type::accept (ConstSymbolVisitor& visitor) const { visitor.visit (*this); }

ACCEPT(InstanceSymbol)
ACCEPT(ParameterSymbol)
ACCEPT(TypeSymbol)
ACCEPT(TypedConstantSymbol)
ACCEPT(VariableSymbol)
ACCEPT(HiddenSymbol)

void
ParameterSymbol::check_foreign_safe () const
{
  if (type_contains_pointer (type) && dereference_mutability != FOREIGN)
    {
      switch (kind)
        {
        case Ordinary:
        case OrdinaryDuplicate:
        case Receiver:
        case ReceiverDuplicate:
        case Iota:
          error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                         "parameter is not foreign safe (E106)");
          break;
        case Return:
          error_at_line (-1, 0, definingNode->location.File.c_str (), definingNode->location.Line,
                         "return parameter is not foreign safe (E107)");
          break;
        }
    }
}
