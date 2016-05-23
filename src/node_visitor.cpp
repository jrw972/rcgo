#include "node_visitor.hpp"

#include "node.hpp"

namespace ast
{

void DefaultNodeVisitor::visit (Identifier& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IdentifierList& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Receiver& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (ArrayTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (SliceTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (MapTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (EmptyTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (FieldListTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (HeapTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IdentifierListTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IdentifierTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (PointerTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (PushPortTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (PullPortTypeSpec& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (SignatureTypeSpec& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (TypeExpression& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (BinaryArithmeticExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (AddressOfExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (CallExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ConversionExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (DereferenceExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IdentifierExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IndexExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (SliceExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (EmptyExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IndexedPushPortCallExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ListExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (LiteralExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (UnaryArithmeticExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (PushPortCallExpr& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (SelectExpr& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (EmptyStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (AddAssignStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ChangeStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (AssignStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ExpressionStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IfStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (WhileStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ListStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ReturnStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IncrementDecrementStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (SubtractAssignStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ActivateStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (VarStatement& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (BindPushPortStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (BindPushPortParamStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (BindPullPortStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ForIotaStatement& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (Action& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Const& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (DimensionedAction& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Bind& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Function& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Getter& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Initializer& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Instance& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Method& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Reaction& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (DimensionedReaction& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Type& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (SourceFile& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (ElementList& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (Element& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (CompositeLiteral& node)
{
  default_action (node);
}

void DefaultNodeVisitor::default_action (Node& node)
{ }

}
