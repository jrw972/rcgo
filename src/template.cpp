#include "template.hpp"

#include "symbol_visitor.hpp"

namespace decl
{

void Template::compute_receiver_access (const semantic::ExpressionValueList& args,
                                        ReceiverAccess& receiver_access,
                                        bool& flag) const
{
  NOT_REACHED;
}

runtime::Operation* Template::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::Operation* arg_ops) const
{
  NOT_REACHED;
}

runtime::Operation* Template::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValue& arg_val,
    runtime::Operation* arg_op) const
{
  NOT_REACHED;
}

runtime::Operation* Template::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValue& left_val,
    runtime::Operation* left_op,
    const semantic::ExpressionValue& right_val,
    runtime::Operation* right_op) const
{
  NOT_REACHED;
}

TemplateSymbol::TemplateSymbol (const std::string& id,
                                const util::Location& loc)
  : Symbol (id, loc)
{ }

void
TemplateSymbol::accept (SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
TemplateSymbol::accept (ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

const type::Type* TemplateSymbol::symbol_type () const
{
  return type::Template::Instance ();
}

}
