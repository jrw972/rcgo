#include "Symbol.hpp"
#include "SymbolVisitor.hpp"

#define ACCEPT(type) void type::accept (SymbolVisitor& visitor) { visitor.visit (*this); } \
void type::accept (ConstSymbolVisitor& visitor) const { visitor.visit (*this); }

ACCEPT(InstanceSymbol)
ACCEPT(ParameterSymbol)
ACCEPT(TypeSymbol)
ACCEPT(TypedConstantSymbol)
ACCEPT(VariableSymbol)
ACCEPT(HiddenSymbol)
