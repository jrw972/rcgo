#include "polymorphic_function.hpp"

#include "symbol_visitor.hpp"
#include "error_reporter.hpp"
#include "polymorphic_function_visitor.hpp"

namespace decl
{
using namespace semantic;
using namespace util;
using namespace type;

PolymorphicFunction::PolymorphicFunction (const std::string& id,
    const util::Location& loc)
  : Symbol (id, loc)
{ }

void
PolymorphicFunction::accept (SymbolVisitor& visitor)
{
  visitor.visit (*this);
}

void
PolymorphicFunction::accept (ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

LogicNot::LogicNot ()
  : PolymorphicFunction ("!", util::builtin)
{ }

void LogicNot::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation* LogicNot::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_val,
    runtime::ListOperation* arg_op) const
{
  return runtime::make_unary<runtime::LogicNotter> (result.type, arg_op);
}

Posate::Posate ()
  : PolymorphicFunction ("+", util::builtin)
{ }

void Posate::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

Negate::Negate ()
  : PolymorphicFunction ("-", util::builtin)
{ }

void Negate::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

Complement::Complement ()
  : PolymorphicFunction ("^", util::builtin)
{ }

void Complement::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

template <typename Op>
runtime::Operation* BinaryValueComputer<Op>::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops)
{
  return Op::generate_code (result, arg_vals, arg_ops);
}

runtime::Operation* LogicOrComputer::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops)
{
  return new runtime::LogicOr (arg_ops->list[0], arg_ops->list[1]);
}

runtime::Operation* LogicAndComputer::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops)
{
  return new runtime::LogicAnd (arg_ops->list[0], arg_ops->list[1]);
}

template <typename Computer>
runtime::Operation*
BinaryArithmetic<Computer>::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return Computer::generate_code (result, arg_vals, arg_ops);
}

template <typename B>
runtime::Operation* BinaryShift<B>::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return B::generate_code (result, arg_vals, arg_ops);
}

template class BinaryArithmetic<BinaryValueComputer<SMultiply> >;
template class BinaryArithmetic<BinaryValueComputer<SDivide> >;
template class BinaryArithmetic<BinaryValueComputer<SModulus> >;
template class BinaryShift <runtime::LeftShifter>;
template class BinaryShift <runtime::RightShifter>;
template class BinaryArithmetic<BinaryValueComputer<SBitAnd> >;
template class BinaryArithmetic<BinaryValueComputer<SBitAndNot> >;
template class BinaryArithmetic<BinaryValueComputer<SAdd> >;
template class BinaryArithmetic<BinaryValueComputer<SSubtract> >;
template class BinaryArithmetic<BinaryValueComputer<SBitOr> >;
template class BinaryArithmetic<BinaryValueComputer<SBitXor> >;
template class BinaryArithmetic<BinaryValueComputer<SEqual> >;
template class BinaryArithmetic<BinaryValueComputer<SNotEqual> >;
template class BinaryArithmetic<BinaryValueComputer<SLessThan> >;
template class BinaryArithmetic<BinaryValueComputer<SLessEqual> >;
template class BinaryArithmetic<BinaryValueComputer<SMoreThan> >;
template class BinaryArithmetic<BinaryValueComputer<SMoreEqual> >;
template class BinaryArithmetic<LogicOrComputer>;
template class BinaryArithmetic<LogicAndComputer>;

LeftShift::LeftShift ()
  : BinaryShift ("<<", util::builtin)
{ }

void LeftShift::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

RightShift::RightShift ()
  : BinaryShift (">>", util::builtin)
{ }

void RightShift::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SMultiply::generate_code (const ExpressionValue& result,
                          const ExpressionValueList& arg_vals,
                          runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<runtime::Multiplier> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

Multiply::Multiply () :
  BinaryArithmetic ("*", util::builtin)
{ }

void Multiply::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SDivide::generate_code (const ExpressionValue& result,
                        const ExpressionValueList& arg_vals,
                        runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<runtime::Divider> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

Divide::Divide () :
  BinaryArithmetic ("/", util::builtin)
{ }

void Divide::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SModulus::generate_code (const ExpressionValue& result,
                         const ExpressionValueList& arg_vals,
                         runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<runtime::Modulizer> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

Modulus::Modulus () :
  BinaryArithmetic ("%", util::builtin)
{ }

void Modulus::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SBitAnd::generate_code (const ExpressionValue& result,
                        const ExpressionValueList& arg_vals,
                        runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<runtime::BitAnder> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

BitAnd::BitAnd () :
  BinaryArithmetic ("&", util::builtin)
{ }

void BitAnd::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SBitAndNot::generate_code (const ExpressionValue& result,
                           const ExpressionValueList& arg_vals,
                           runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<runtime::BitAndNotter> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

BitAndNot::BitAndNot () :
  BinaryArithmetic ("&^", util::builtin)
{ }

void BitAndNot::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SAdd::generate_code (const ExpressionValue& result,
                     const ExpressionValueList& arg_vals,
                     runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<runtime::Adder> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

Add::Add () :
  BinaryArithmetic ("+", util::builtin)
{ }

void Add::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SSubtract::generate_code (const ExpressionValue& result,
                          const ExpressionValueList& arg_vals,
                          runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<runtime::Subtracter> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

Subtract::Subtract () :
  BinaryArithmetic ("-", util::builtin)
{ }

void Subtract::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SBitOr::generate_code (const ExpressionValue& result,
                       const ExpressionValueList& arg_vals,
                       runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<runtime::BitOrer> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

BitOr::BitOr () :
  BinaryArithmetic ("|", util::builtin)
{ }

void BitOr::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SBitXor::generate_code (const ExpressionValue& result,
                        const ExpressionValueList& arg_vals,
                        runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<runtime::BitXorer> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

BitXor::BitXor () :
  BinaryArithmetic ("^", util::builtin)
{ }

void BitXor::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SEqual::generate_code (const ExpressionValue& result,
                       const ExpressionValueList& arg_vals,
                       runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<runtime::Equalizer> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

Equal::Equal () :
  BinaryArithmetic ("==", util::builtin)
{ }

void Equal::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SNotEqual::generate_code (const ExpressionValue& result,
                          const ExpressionValueList& arg_vals,
                          runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<runtime::NotEqualizer> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

NotEqual::NotEqual () :
  BinaryArithmetic ("!=", util::builtin)
{ }

void NotEqual::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SLessThan::generate_code (const ExpressionValue& result,
                          const ExpressionValueList& arg_vals,
                          runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<runtime::LessThaner> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

LessThan::LessThan () :
  BinaryArithmetic ("<", util::builtin)
{ }

void LessThan::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SLessEqual::generate_code (const ExpressionValue& result,
                           const ExpressionValueList& arg_vals,
                           runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<runtime::LessEqualizer> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

LessEqual::LessEqual () :
  BinaryArithmetic ("<=", util::builtin)
{ }

void LessEqual::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SMoreThan::generate_code (const ExpressionValue& result,
                          const ExpressionValueList& arg_vals,
                          runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<runtime::MoreThaner> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

MoreThan::MoreThan () :
  BinaryArithmetic (">", util::builtin)
{ }

void MoreThan::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation*
SMoreEqual::generate_code (const ExpressionValue& result,
                           const ExpressionValueList& arg_vals,
                           runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<runtime::MoreEqualizer> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

MoreEqual::MoreEqual () :
  BinaryArithmetic (">=", util::builtin)
{ }

void MoreEqual::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

LogicAnd::LogicAnd ()
  : BinaryArithmetic ("&&", util::builtin)
{}

void LogicAnd::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

LogicOr::LogicOr ()
  : BinaryArithmetic ("||", util::builtin)
{}

void LogicOr::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

New::New ()
  : PolymorphicFunction ("new", util::builtin)
{ }

void New::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

void New::compute_receiver_access (const semantic::ExpressionValueList& args,
                                   ReceiverAccess& receiver_access,
                                   bool& flag) const
{
  receiver_access = AccessNone;
  flag = false;
}

runtime::Operation* New::generate_code (const semantic::ExpressionValue& result,
                                        const semantic::ExpressionValueList& arg_vals,
                                        runtime::ListOperation* arg_ops) const
{
  return new runtime::NewOp (arg_vals.front ().type);
}

Make::Make ()
  : PolymorphicFunction ("make", util::builtin)
{ }

void Make::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

void Make::compute_receiver_access (const semantic::ExpressionValueList& args,
                                    ReceiverAccess& receiver_access,
                                    bool& flag) const
{
  receiver_access = AccessNone;
  flag = false;
}

runtime::Operation* Make::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  UNIMPLEMENTED;
  //return new runtime::MakeOp (arg_vals.front ().type);
}

Move::Move ()
  : PolymorphicFunction ("move", util::builtin)
{ }

void Move::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

void
Move::compute_receiver_access (const semantic::ExpressionValueList& args,
                               ReceiverAccess& receiver_access,
                               bool& flag) const
{
  // Check if a mutable pointer escapes.
  receiver_access = AccessNone;
  flag = false;
  for (ExpressionValueList::const_iterator pos = args.begin (),
       limit = args.end ();
       pos != limit;
       ++pos)
    {
      receiver_access = std::max (receiver_access, pos->receiver_access);
    }
}

runtime::Operation* Move::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return new runtime::MoveOp (arg_ops);
}

Merge::Merge ()
  : PolymorphicFunction ("merge", util::builtin)
{ }

void Merge::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

void Merge::compute_receiver_access (const semantic::ExpressionValueList& args,
                                     ReceiverAccess& receiver_access,
                                     bool& flag) const
{
  // Check if a mutable pointer escapes.
  receiver_access = AccessNone;
  flag = false;
  for (ExpressionValueList::const_iterator pos = args.begin (),
       limit = args.end ();
       pos != limit;
       ++pos)
    {
      receiver_access = std::max (receiver_access, pos->receiver_access);
    }
}

runtime::Operation* Merge::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return new runtime::MergeOp (arg_ops);
}

Len::Len ()
  : PolymorphicFunction ("len", util::builtin)
{ }

void Len::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation* Len::generate_code (const semantic::ExpressionValue& result,
                                        const semantic::ExpressionValueList& arg_vals,
                                        runtime::ListOperation* arg_ops) const
{
  return new runtime::LenOp (arg_ops);
}

Cap::Cap ()
  : PolymorphicFunction ("cap", util::builtin)
{ }

void Cap::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation* Cap::generate_code (const semantic::ExpressionValue& result,
                                        const semantic::ExpressionValueList& arg_vals,
                                        runtime::ListOperation* arg_ops) const
{
  UNIMPLEMENTED;
  //return new runtime::CapOp (arg_ops);
}

Append::Append ()
  : PolymorphicFunction ("append", util::builtin)
{ }

void Append::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

runtime::Operation* Append::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return make_append (arg_vals[0].type->underlying_type ()->to_slice (), arg_ops);
}

Copy::Copy ()
  : PolymorphicFunction ("copy", util::builtin)
{ }

void Copy::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

void
Copy::compute_receiver_access (const semantic::ExpressionValueList& args,
                               ReceiverAccess& receiver_access,
                               bool& flag) const
{
  // Check if a mutable pointer escapes.
  receiver_access = AccessNone;
  flag = false;
  for (ExpressionValueList::const_iterator pos = args.begin (),
       limit = args.end ();
       pos != limit;
       ++pos)
    {
      receiver_access = std::max (receiver_access, pos->receiver_access);
    }
}

runtime::Operation* Copy::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return new runtime::CopyOp (arg_vals.front ().type, arg_ops);
}

Println::Println ()
  : PolymorphicFunction ("println", decl::builtin)
{ }

void Println::accept (PolymorphicFunctionVisitor& visitor) const
{
  visitor.visit (*this);
}

void Println::compute_receiver_access (const semantic::ExpressionValueList& args,
                                       ReceiverAccess& receiver_access,
                                       bool& flag) const
{
  // Check if a mutable pointer escapes.
  receiver_access = AccessNone;
  flag = false;
  for (ExpressionValueList::const_iterator pos = args.begin (),
       limit = args.end ();
       pos != limit;
       ++pos)
    {
      receiver_access = std::max (receiver_access, pos->receiver_access);
    }
}

runtime::Operation* Println::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return new runtime::PrintlnOp (arg_vals, arg_ops);
}

Complex::Complex ()
  : PolymorphicFunction ("complex", util::builtin)
{ }

void
Complex::accept (PolymorphicFunctionVisitor& visitor) const
{
  UNIMPLEMENTED;
}

void
Complex::compute_receiver_access (const semantic::ExpressionValueList& args,
                                  ReceiverAccess& receiver_access,
                                  bool& flag) const
{
  UNIMPLEMENTED;
}

runtime::Operation*
Complex::generate_code (const semantic::ExpressionValue& result,
                        const semantic::ExpressionValueList& arg_vals,
                        runtime::ListOperation* arg_ops) const
{
  UNIMPLEMENTED;
}

Delete::Delete ()
  : PolymorphicFunction ("delete", util::builtin)
{
}

void
Delete::accept (PolymorphicFunctionVisitor& visitor) const
{
  UNIMPLEMENTED;
}

void
Delete::compute_receiver_access (const semantic::ExpressionValueList& args,
                                 ReceiverAccess& receiver_access,
                                 bool& flag) const
{
  UNIMPLEMENTED;
}

runtime::Operation*
Delete::generate_code (const semantic::ExpressionValue& result,
                       const semantic::ExpressionValueList& arg_vals,
                       runtime::ListOperation* arg_ops) const
{
  UNIMPLEMENTED;
}

Imag::Imag ()
  : PolymorphicFunction ("imag", util::builtin)
{ }

void
Imag::accept (PolymorphicFunctionVisitor& visitor) const
{
  UNIMPLEMENTED;
}

void
Imag::compute_receiver_access (const semantic::ExpressionValueList& args,
                               ReceiverAccess& receiver_access,
                               bool& flag) const
{
  UNIMPLEMENTED;
}

runtime::Operation*
Imag::generate_code (const semantic::ExpressionValue& result,
                     const semantic::ExpressionValueList& arg_vals,
                     runtime::ListOperation* arg_ops) const
{
  UNIMPLEMENTED;
}

Panic::Panic ()
  : PolymorphicFunction ("panic", util::builtin)
{ }

void
Panic::accept (PolymorphicFunctionVisitor& visitor) const
{
  UNIMPLEMENTED;
}

void
Panic::compute_receiver_access (const semantic::ExpressionValueList& args,
                                ReceiverAccess& receiver_access,
                                bool& flag) const
{
  UNIMPLEMENTED;
}

runtime::Operation*
Panic::generate_code (const semantic::ExpressionValue& result,
                      const semantic::ExpressionValueList& arg_vals,
                      runtime::ListOperation* arg_ops) const
{
  UNIMPLEMENTED;
}

Print::Print ()
  : PolymorphicFunction ("print", util::builtin)
{ }

void
Print::accept (PolymorphicFunctionVisitor& visitor) const
{
  UNIMPLEMENTED;
}

void
Print::compute_receiver_access (const semantic::ExpressionValueList& args,
                                ReceiverAccess& receiver_access,
                                bool& flag) const
{
  UNIMPLEMENTED;
}

runtime::Operation*
Print::generate_code (const semantic::ExpressionValue& result,
                      const semantic::ExpressionValueList& arg_vals,
                      runtime::ListOperation* arg_ops) const
{
  UNIMPLEMENTED;
}

Real::Real ()
  : PolymorphicFunction ("real", util::builtin)
{ }

void
Real::accept (PolymorphicFunctionVisitor& visitor) const
{
  UNIMPLEMENTED;
}

void
Real::compute_receiver_access (const semantic::ExpressionValueList& args,
                               ReceiverAccess& receiver_access,
                               bool& flag) const
{
  UNIMPLEMENTED;
}


runtime::Operation*
Real::generate_code (const semantic::ExpressionValue& result,
                     const semantic::ExpressionValueList& arg_vals,
                     runtime::ListOperation* arg_ops) const
{
  UNIMPLEMENTED;
}

Recover::Recover ()
  : PolymorphicFunction ("recover", util::builtin)
{ }

void
Recover::accept (PolymorphicFunctionVisitor& visitor) const
{
  UNIMPLEMENTED;
}


void
Recover::compute_receiver_access (const semantic::ExpressionValueList& args,
                                  ReceiverAccess& receiver_access,
                                  bool& flag) const
{
  UNIMPLEMENTED;
}

runtime::Operation*
Recover::generate_code (const semantic::ExpressionValue& result,
                        const semantic::ExpressionValueList& arg_vals,
                        runtime::ListOperation* arg_ops) const
{
  UNIMPLEMENTED;
}

Posate posate;
Negate negate;
LogicNot logic_not;
Complement complement;

Multiply multiply;
Divide divide;
Modulus modulus;
LeftShift left_shift;
RightShift right_shift;
BitAnd bit_and;
BitAndNot bit_and_not;
Add add;
Subtract subtract;
BitOr bit_or;
BitXor bit_xor;
Equal equal;
NotEqual not_equal;
LessThan less_than;
LessEqual less_equal;
MoreThan more_than;
MoreEqual more_equal;
LogicOr logic_or;
LogicAnd logic_and;

Append append_func;
Cap cap_func;
Complex complex_func;
Copy copy_func;
Delete delete_func;
Imag imag_func;
Len len_func;
Make make_func;
Merge merge_func;
Move move_func;
New new_func;
Panic panic_func;
Print print_func;
Println println_func;
Real real_func;
Recover recover_func;

}
