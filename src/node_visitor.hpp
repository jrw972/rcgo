#ifndef RC_SRC_NODE_VISITOR_HPP
#define RC_SRC_NODE_VISITOR_HPP

#include "types.hpp"

namespace ast
{

struct NodeVisitor
{
  virtual void visit (Identifier& node) = 0;
  virtual void visit (IdentifierList& node) = 0;
  virtual void visit (Receiver& node) = 0;
  virtual void visit (ArrayTypeSpec& node) = 0;
  virtual void visit (SliceTypeSpec& node) = 0;
  virtual void visit (MapTypeSpec& node) = 0;
  virtual void visit (EmptyTypeSpec& node) = 0;
  virtual void visit (FieldListTypeSpec& node) = 0;
  virtual void visit (HeapTypeSpec& node) = 0;
  virtual void visit (IdentifierListTypeSpec& node) = 0;
  virtual void visit (IdentifierTypeSpec& node) = 0;
  virtual void visit (PointerTypeSpec& node) = 0;
  virtual void visit (PushPortTypeSpec& node) = 0;
  virtual void visit (PullPortTypeSpec& node) = 0;
  virtual void visit (SignatureTypeSpec& node) = 0;
  virtual void visit (TypeExpression& node) = 0;
  virtual void visit (BinaryArithmeticExpr& node) = 0;
  virtual void visit (AddressOfExpr& node) = 0;
  virtual void visit (CallExpr& node) = 0;
  virtual void visit (ConversionExpr& node) = 0;
  virtual void visit (DereferenceExpr& node) = 0;
  virtual void visit (IdentifierExpr& node) = 0;
  virtual void visit (IndexExpr& node) = 0;
  virtual void visit (SliceExpr& node) = 0;
  virtual void visit (EmptyExpr& node) = 0;
  virtual void visit (IndexedPushPortCallExpr& node) = 0;
  virtual void visit (ListExpr& node) = 0;
  virtual void visit (LiteralExpr& node) = 0;
  virtual void visit (UnaryArithmeticExpr& node) = 0;
  virtual void visit (PushPortCallExpr& node) = 0;
  virtual void visit (SelectExpr& node) = 0;
  virtual void visit (EmptyStatement& node) = 0;
  virtual void visit (AddAssignStatement& node) = 0;
  virtual void visit (ChangeStatement& node) = 0;
  virtual void visit (AssignStatement& node) = 0;
  virtual void visit (ExpressionStatement& node) = 0;
  virtual void visit (IfStatement& node) = 0;
  virtual void visit (WhileStatement& node) = 0;
  virtual void visit (ListStatement& node) = 0;
  virtual void visit (ReturnStatement& node) = 0;
  virtual void visit (IncrementDecrementStatement& node) = 0;
  virtual void visit (SubtractAssignStatement& node) = 0;
  virtual void visit (ActivateStatement& node) = 0;
  virtual void visit (VarStatement& node) = 0;
  virtual void visit (BindPushPortStatement& node) = 0;
  virtual void visit (BindPushPortParamStatement& node) = 0;
  virtual void visit (BindPullPortStatement& node) = 0;
  virtual void visit (ForIotaStatement& node) = 0;
  virtual void visit (Action& node) = 0;
  virtual void visit (Const& node) = 0;
  virtual void visit (DimensionedAction& node) = 0;
  virtual void visit (Bind& node) = 0;
  virtual void visit (Function& node) = 0;
  virtual void visit (Getter& node) = 0;
  virtual void visit (Initializer& node) = 0;
  virtual void visit (Instance& node) = 0;
  virtual void visit (Method& node) = 0;
  virtual void visit (Reaction& node) = 0;
  virtual void visit (DimensionedReaction& node) = 0;
  virtual void visit (Type& node) = 0;
  virtual void visit (SourceFile& node) = 0;
  virtual void visit (ElementList& node) = 0;
  virtual void visit (Element& node) = 0;
  virtual void visit (CompositeLiteral& node) = 0;
};

struct DefaultNodeVisitor : public NodeVisitor
{
  virtual void visit (Identifier& node);
  virtual void visit (IdentifierList& node);
  virtual void visit (Receiver& node);
  virtual void visit (ArrayTypeSpec& node);
  virtual void visit (SliceTypeSpec& node);
  virtual void visit (MapTypeSpec& node);
  virtual void visit (EmptyTypeSpec& node);
  virtual void visit (FieldListTypeSpec& node);
  virtual void visit (HeapTypeSpec& node);
  virtual void visit (IdentifierListTypeSpec& node);
  virtual void visit (IdentifierTypeSpec& node);
  virtual void visit (PointerTypeSpec& node);
  virtual void visit (PushPortTypeSpec& node);
  virtual void visit (PullPortTypeSpec& node);
  virtual void visit (SignatureTypeSpec& node);
  virtual void visit (TypeExpression& node);
  virtual void visit (BinaryArithmeticExpr& node);
  virtual void visit (AddressOfExpr& node);
  virtual void visit (CallExpr& node);
  virtual void visit (ConversionExpr& node);
  virtual void visit (DereferenceExpr& node);
  virtual void visit (IdentifierExpr& node);
  virtual void visit (IndexExpr& node);
  virtual void visit (SliceExpr& node);
  virtual void visit (EmptyExpr& node);
  virtual void visit (IndexedPushPortCallExpr& node);
  virtual void visit (ListExpr& node);
  virtual void visit (LiteralExpr& node);
  virtual void visit (UnaryArithmeticExpr& node);
  virtual void visit (PushPortCallExpr& node);
  virtual void visit (SelectExpr& node);
  virtual void visit (EmptyStatement& node);
  virtual void visit (AddAssignStatement& node);
  virtual void visit (ChangeStatement& node);
  virtual void visit (AssignStatement& node);
  virtual void visit (ExpressionStatement& node);
  virtual void visit (IfStatement& node);
  virtual void visit (WhileStatement& node);
  virtual void visit (ListStatement& node);
  virtual void visit (ReturnStatement& node);
  virtual void visit (IncrementDecrementStatement& node);
  virtual void visit (SubtractAssignStatement& node);
  virtual void visit (ActivateStatement& node);
  virtual void visit (VarStatement& node);
  virtual void visit (BindPushPortStatement& node);
  virtual void visit (BindPushPortParamStatement& node);
  virtual void visit (BindPullPortStatement& node);
  virtual void visit (ForIotaStatement& node);
  virtual void visit (Action& node);
  virtual void visit (Const& node);
  virtual void visit (DimensionedAction& node);
  virtual void visit (Bind& node);
  virtual void visit (Function& node);
  virtual void visit (Getter& node);
  virtual void visit (Initializer& node);
  virtual void visit (Instance& node);
  virtual void visit (Method& node);
  virtual void visit (Reaction& node);
  virtual void visit (DimensionedReaction& node);
  virtual void visit (Type& node);
  virtual void visit (SourceFile& node);
  virtual void visit (ElementList& node);
  virtual void visit (Element& node);
  virtual void visit (CompositeLiteral& node);
  virtual void default_action (Node& node);
};

}

#endif // RC_SRC_AST_VISITOR_HPP
