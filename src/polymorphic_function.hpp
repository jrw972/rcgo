#ifndef RC_SRC_POLYMORPHIC_FUNCTION_HPP
#define RC_SRC_POLYMORPHIC_FUNCTION_HPP

#include "symbol.hpp"
#include "operation.hpp"

namespace decl
{

struct PolymorphicFunction : public Symbol
{
  PolymorphicFunction (const source::Identifier& identifier);

  virtual void accept (PolymorphicFunctionVisitor& visitor) const = 0;

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

struct LogicNot : public decl::PolymorphicFunction
{
  LogicNot ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const
  {
    receiver_access = args[0].receiver_access;
    flag = false;
  }

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Posate : public decl::PolymorphicFunction
{
  Posate ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const
  {
    UNIMPLEMENTED;
  }

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const
  {
    UNIMPLEMENTED;
  }
};

struct Negate : public decl::PolymorphicFunction
{
  Negate ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const
  {
    UNIMPLEMENTED;
  }

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_op) const
  {
    UNIMPLEMENTED;
  }
};

struct Complement : public decl::PolymorphicFunction
{
  Complement ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const
  {
    UNIMPLEMENTED;
  }

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const
  {
    UNIMPLEMENTED;
  }
};

template <typename Op>
struct BinaryValueComputer
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct LogicOrComputer
{
  static runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValueList& arg_vals,
      runtime::ListOperation* arg_ops);
};

struct LogicAndComputer
{
  static runtime::Operation* generate_code (const semantic::ExpressionValue& result,
      const semantic::ExpressionValueList& arg_vals,
      runtime::ListOperation* arg_ops);
};

template <typename Computer>
struct BinaryArithmetic : public decl::PolymorphicFunction
{
  BinaryArithmetic (const source::Identifier& identifier)
    : PolymorphicFunction (identifier)
  { }

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const
  {
    flag = false;
    receiver_access = std::max (args[0].receiver_access,
                                args[1].receiver_access);
  }

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

template <typename B>
struct BinaryShift : public decl::PolymorphicFunction
{
  BinaryShift (const source::Identifier& identifier) :
    PolymorphicFunction (identifier)
  { }

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const
  {
    UNIMPLEMENTED;
  }

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct SMultiply
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct Multiply : public BinaryArithmetic<BinaryValueComputer<SMultiply> >
{
  Multiply ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SDivide
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct Divide : public BinaryArithmetic<BinaryValueComputer<SDivide> >
{
  Divide ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SModulus
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct Modulus : public BinaryArithmetic<BinaryValueComputer<SModulus> >
{
  Modulus ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct LeftShift : public BinaryShift<runtime::LeftShifter>
{
  LeftShift ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct RightShift : public BinaryShift<runtime::RightShifter>
{
  RightShift ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SBitAnd
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct BitAnd : public BinaryArithmetic<BinaryValueComputer<SBitAnd> >
{
  BitAnd ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SBitAndNot
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct BitAndNot : public BinaryArithmetic<BinaryValueComputer<SBitAndNot> >
{
  BitAndNot ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SAdd
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct Add : public BinaryArithmetic<BinaryValueComputer<SAdd> >
{
  Add ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SSubtract
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct Subtract : public BinaryArithmetic<BinaryValueComputer<SSubtract> >
{
  Subtract ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SBitOr
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct BitOr : public BinaryArithmetic<BinaryValueComputer<SBitOr> >
{
  BitOr ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SBitXor
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct BitXor : public BinaryArithmetic<BinaryValueComputer<SBitXor> >
{
  BitXor ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SEqual
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct Equal : public BinaryArithmetic<BinaryValueComputer<SEqual> >
{
  Equal ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SNotEqual
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct NotEqual : public BinaryArithmetic<BinaryValueComputer<SNotEqual> >
{
  NotEqual ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SLessThan
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct LessThan : public BinaryArithmetic<BinaryValueComputer<SLessThan> >
{
  LessThan ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SLessEqual
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct LessEqual : public BinaryArithmetic<BinaryValueComputer<SLessEqual> >
{
  LessEqual ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SMoreThan
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct MoreThan : public BinaryArithmetic<BinaryValueComputer<SMoreThan> >
{
  MoreThan ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct SMoreEqual
{
  static runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops);
};

struct MoreEqual : public BinaryArithmetic<BinaryValueComputer<SMoreEqual> >
{
  MoreEqual ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct LogicOr : public BinaryArithmetic<LogicOrComputer>
{
  LogicOr ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct LogicAnd : public BinaryArithmetic<LogicAndComputer>
{
  LogicAnd ();
  virtual void accept (PolymorphicFunctionVisitor& visitor) const;
};

struct Move : public decl::PolymorphicFunction
{
  Move ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Merge : public decl::PolymorphicFunction
{
  Merge ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Len : public decl::PolymorphicFunction
{
  Len ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const
  {
    UNIMPLEMENTED;
  }

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Cap : public decl::PolymorphicFunction
{
  Cap ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const
  {
    UNIMPLEMENTED;
  }

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct New : public decl::PolymorphicFunction
{
  New ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Make : public decl::PolymorphicFunction
{
  Make ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Append : public decl::PolymorphicFunction
{
  Append ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const
  {
    UNIMPLEMENTED;
  }

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Copy : public decl::PolymorphicFunction
{
  Copy ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Println : public decl::PolymorphicFunction
{
  Println ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Complex : public decl::PolymorphicFunction
{
  Complex ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Delete : public decl::PolymorphicFunction
{
  Delete ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Imag : public decl::PolymorphicFunction
{
  Imag ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Panic : public decl::PolymorphicFunction
{
  Panic ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Print : public decl::PolymorphicFunction
{
  Print ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Real : public decl::PolymorphicFunction
{
  Real ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

struct Recover : public decl::PolymorphicFunction
{
  Recover ();

  virtual void accept (PolymorphicFunctionVisitor& visitor) const;

  virtual void
  compute_receiver_access (const semantic::ExpressionValueList& args,
                           ReceiverAccess& receiver_access,
                           bool& flag) const;

  virtual runtime::Operation*
  generate_code (const semantic::ExpressionValue& result,
                 const semantic::ExpressionValueList& arg_vals,
                 runtime::ListOperation* arg_ops) const;
};

extern Posate posate;
extern Negate negate;
extern LogicNot logic_not;
extern Complement complement;

extern Multiply multiply;
extern Divide divide;
extern Modulus modulus;
extern LeftShift left_shift;
extern RightShift right_shift;
extern BitAnd bit_and;
extern BitAndNot bit_and_not;
extern Add add;
extern Subtract subtract;
extern BitOr bit_or;
extern BitXor bit_xor;
extern Equal equal;
extern NotEqual not_equal;
extern LessThan less_than;
extern LessEqual less_equal;
extern MoreThan more_than;
extern MoreEqual more_equal;
extern LogicOr logic_or;
extern LogicAnd logic_and;

extern Append append_func;
extern Cap cap_func;
extern Complex complex_func;
extern Copy copy_func;
extern Delete delete_func;
extern Imag imag_func;
extern Len len_func;
extern Make make_func;
extern Merge merge_func;
extern Move move_func;
extern New new_func;
extern Panic panic_func;
extern Print print_func;
extern Println println_func;
extern Real real_func;
extern Recover recover_func;
}

#endif // RC_SRC_POLYMORPHIC_FUNCTION_HPP
