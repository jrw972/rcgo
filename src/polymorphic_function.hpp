#ifndef RC_SRC_POLYMORPHIC_FUNCTION_HPP
#define RC_SRC_POLYMORPHIC_FUNCTION_HPP

#include "symbol.hpp"
#include "expression_value.hpp"

namespace decl
{

struct PolymorphicFunction : public Symbol
{
  PolymorphicFunction (const std::string& id,
                       const util::Location& loc);

  virtual void
  check (util::ErrorReporter& er,
         const util::Location& loc,
         semantic::ExpressionValue& result,
         semantic::ExpressionValueList& arguments) const = 0;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const = 0;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const = 0;

  // Symbol
  virtual void accept (SymbolVisitor& visitor);
  virtual void accept (ConstSymbolVisitor& visitor) const;
};

}

#endif // RC_SRC_POLYMORPHIC_FUNCTION_HPP
