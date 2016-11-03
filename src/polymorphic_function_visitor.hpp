#ifndef RC_SRC_POLYMORPHIC_FUNCTION_VISITOR_HPP
#define RC_SRC_POLYMORPHIC_FUNCTION_VISITOR_HPP

#include "types.hpp"

namespace decl
{

struct PolymorphicFunctionVisitor
{
  virtual void visit (const LogicNot& pf) = 0;
  virtual void visit (const Negate& pf) = 0;
  virtual void visit (const Posate& pf) = 0;
  virtual void visit (const Complement& pf) = 0;

  virtual void visit (const Multiply& pf) = 0;
  virtual void visit (const Divide& pf) = 0;
  virtual void visit (const Modulus& pf) = 0;
  virtual void visit (const Add& pf) = 0;
  virtual void visit (const Subtract& pf) = 0;

  virtual void visit (const Equal& pf) = 0;
  virtual void visit (const NotEqual& pf) = 0;
  virtual void visit (const LessThan& pf) = 0;
  virtual void visit (const LessEqual& pf) = 0;
  virtual void visit (const MoreThan& pf) = 0;
  virtual void visit (const MoreEqual& pf) = 0;

  virtual void visit (const BitAnd& pf) = 0;
  virtual void visit (const BitAndNot& pf) = 0;
  virtual void visit (const BitOr& pf) = 0;
  virtual void visit (const BitXor& pf) = 0;
  virtual void visit (const LeftShift& pf) = 0;
  virtual void visit (const RightShift& pf) = 0;

  virtual void visit (const LogicAnd& pf) = 0;
  virtual void visit (const LogicOr& pf) = 0;

  virtual void visit (const Len& pf) = 0;
  virtual void visit (const Cap& pf) = 0;
  virtual void visit (const New& pf) = 0;
  virtual void visit (const Make& pf) = 0;
  virtual void visit (const Move& pf) = 0;
  virtual void visit (const Merge& pf) = 0;
  virtual void visit (const Append& pf) = 0;
  virtual void visit (const Copy& pf) = 0;
  virtual void visit (const Println& pf) = 0;
};

}

#endif // RC_SRC_POLYMORPHIC_FUNCTION_VISITOR_HPP
