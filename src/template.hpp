#ifndef RC_SRC_TEMPLATE_HPP
#define RC_SRC_TEMPLATE_HPP

#include "symbol.hpp"
#include "expression_value.hpp"

namespace decl
{

struct Template
{
  virtual void check (util::ErrorReporter& er,
                      const util::Location& loc,
                      semantic::ExpressionValue& result,
                      semantic::ExpressionValueList& arguments) const = 0;

  virtual void compute_receiver_access (const semantic::ExpressionValueList& args,
                                        ReceiverAccess& receiver_access,
                                        bool& flag) const;

  virtual runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValueList& arg_vals,
      runtime::Operation* arg_ops) const;

  virtual runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValue& arg_val,
      runtime::Operation* arg_op) const;

  virtual runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValue& left_val,
      runtime::Operation* left_op,
      const semantic::ExpressionValue& right_val,
      runtime::Operation* right_op) const;
};

struct TemplateSymbol : public Template, public Symbol
{
  TemplateSymbol (const std::string& id,
                  const util::Location& loc);

  // Symbol
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
  virtual const type::Type* symbol_type () const;
};

}

#endif // RC_SRC_TEMPLATE_HPP
