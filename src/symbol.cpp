#include "symbol.hpp"

#include <error.h>

#include "symbol_visitor.hpp"
#include "ast.hpp"

namespace decl
{

std::string const ReturnSymbol ("0return");

#define ACCEPT(type) void type::accept (SymbolVisitor& visitor) { visitor.visit (*this); } \
void type::accept (ConstSymbolVisitor& visitor) const { visitor.visit (*this); }

ACCEPT(InstanceSymbol)
ACCEPT(ParameterSymbol)
ACCEPT(TypeSymbol)
ACCEPT(ConstantSymbol)
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

std::ostream&
operator<< (std::ostream& out, const Symbol& s)
{
  struct Visitor : public ConstSymbolVisitor
  {
    std::ostream& out;

    Visitor (std::ostream& o) : out (o) { }

    void defaultAction (const Symbol& s)
    {
      NOT_REACHED;
    }

    virtual void visit (const BuiltinFunction& s)
    {
      out << "BuiltinFunction " << s.identifier << '\n';
    }
    virtual void visit (const decl::Template& s)
    {
      out << "Template " << s.identifier << '\n';
    }
    virtual void visit (const Function& s)
    {
      out << "Function " << s.identifier << '\n';
    }
    virtual void visit (const InstanceSymbol& s)
    {
      out << "InstanceSymbol " << s.identifier << '\n';
    }
    virtual void visit (const ParameterSymbol& s)
    {
      out << "Parameter " << s.identifier << '\n';
    }
    virtual void visit (const TypeSymbol& s)
    {
      out << "Type " << s.identifier << '\n';
    }
    virtual void visit (const ConstantSymbol& s)
    {
      out << "Constant " << s.identifier << '\n';
    }
    virtual void visit (const VariableSymbol& s)
    {
      out << "Variable " << s.identifier << '\n';
    }
    virtual void visit (const HiddenSymbol& s)
    {
      UNIMPLEMENTED;
    }
  };
  Visitor v (out);
  s.accept (v);
  return out;
}

}
