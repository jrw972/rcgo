#include "ast_visitor.hpp"

#include "ast.hpp"

namespace ast
{

void DefaultVisitor::visit (Identifier& node)
{
  default_action (node);
}
void DefaultVisitor::visit (IdentifierList& node)
{
  default_action (node);
}
void DefaultVisitor::visit (Receiver& node)
{
  default_action (node);
}

void DefaultVisitor::visit (ArrayTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (SliceTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (MapTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (EmptyTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (FieldListTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (HeapTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (IdentifierListTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (IdentifierTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (PointerTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (PushPortTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (PullPortTypeSpec& node)
{
  default_action (node);
}
void DefaultVisitor::visit (SignatureTypeSpec& node)
{
  default_action (node);
}

void DefaultVisitor::visit (TypeExpression& node)
{
  default_action (node);
}
void DefaultVisitor::visit (BinaryArithmeticExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (AddressOfExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (CallExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (ConversionExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (DereferenceExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (IdentifierExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (IndexExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (SliceExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (EmptyExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (IndexedPushPortCallExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (ListExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (LiteralExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (UnaryArithmeticExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (PushPortCallExpr& node)
{
  default_action (node);
}
void DefaultVisitor::visit (SelectExpr& node)
{
  default_action (node);
}

void DefaultVisitor::visit (EmptyStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (AddAssignStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (ChangeStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (AssignStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (ExpressionStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (IfStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (WhileStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (ListStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (ReturnStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (IncrementStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (DecrementStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (SubtractAssignStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (ActivateStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (VarStatement& node)
{
  default_action (node);
}

void DefaultVisitor::visit (BindPushPortStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (BindPushPortParamStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (BindPullPortStatement& node)
{
  default_action (node);
}
void DefaultVisitor::visit (ForIotaStatement& node)
{
  default_action (node);
}

void DefaultVisitor::visit (Action& node)
{
  default_action (node);
}
void DefaultVisitor::visit (Const& node)
{
  default_action (node);
}
void DefaultVisitor::visit (DimensionedAction& node)
{
  default_action (node);
}
void DefaultVisitor::visit (Bind& node)
{
  default_action (node);
}
void DefaultVisitor::visit (Function& node)
{
  default_action (node);
}
void DefaultVisitor::visit (Getter& node)
{
  default_action (node);
}
void DefaultVisitor::visit (Initializer& node)
{
  default_action (node);
}
void DefaultVisitor::visit (Instance& node)
{
  default_action (node);
}
void DefaultVisitor::visit (Method& node)
{
  default_action (node);
}
void DefaultVisitor::visit (Reaction& node)
{
  default_action (node);
}
void DefaultVisitor::visit (DimensionedReaction& node)
{
  default_action (node);
}
void DefaultVisitor::visit (Type& node)
{
  default_action (node);
}

void DefaultVisitor::visit (SourceFile& node)
{
  default_action (node);
}

void DefaultVisitor::visit (ElementList& node)
{
  default_action (node);
}

void DefaultVisitor::visit (Element& node)
{
  default_action (node);
}

void DefaultVisitor::visit (CompositeLiteral& node)
{
  default_action (node);
}

}
