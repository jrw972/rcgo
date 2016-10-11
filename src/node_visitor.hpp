#ifndef RC_SRC_NODE_VISITOR_HPP
#define RC_SRC_NODE_VISITOR_HPP

#include "types.hpp"
#include <config.h>

namespace ast
{

struct NodeVisitor
{
  virtual void visit (ActionDecl& node) = 0;
  virtual void visit (Activate& node) = 0;
  virtual void visit (AddAssign& node) = 0;
  virtual void visit (AddressOf& node) = 0;
  virtual void visit (Array& node) = 0;
  virtual void visit (Assign& node) = 0;
  virtual void visit (BinaryArithmetic& node) = 0;
  virtual void visit (BindDecl& node) = 0;
  virtual void visit (BindPullPort& node) = 0;
  virtual void visit (BindPushPort& node) = 0;
  virtual void visit (BindPushPortParameter& node) = 0;
  virtual void visit (Call& node) = 0;
  virtual void visit (Change& node) = 0;
  virtual void visit (CompositeLiteral& node) = 0;
  virtual void visit (ConstDecl& node) = 0;
  virtual void visit (Conversion& node) = 0;
  virtual void visit (Dereference& node) = 0;
  virtual void visit (Element& node) = 0;
  virtual void visit (ElementList& node) = 0;
  virtual void visit (EmptyExpression& node) = 0;
  virtual void visit (EmptyStatement& node) = 0;
  virtual void visit (EmptyType& node) = 0;
  virtual void visit (ExpressionList& node) = 0;
  virtual void visit (ExpressionStatement& node) = 0;
  virtual void visit (FieldList& node) = 0;
  virtual void visit (ForIota& node) = 0;
  virtual void visit (FunctionDecl& node) = 0;
  virtual void visit (GetterDecl& node) = 0;
  virtual void visit (Heap& node) = 0;
  virtual void visit (Identifier& node) = 0;
  virtual void visit (IdentifierExpression& node) = 0;
  virtual void visit (IdentifierList& node) = 0;
  virtual void visit (IdentifierType& node) = 0;
  virtual void visit (If& node) = 0;
  virtual void visit (ImportDeclList& node) = 0;
  virtual void visit (IncrementDecrement& node) = 0;
  virtual void visit (Index& node) = 0;
  virtual void visit (IndexSlice& node) = 0;
  virtual void visit (IndexedPushPortCall& node) = 0;
  virtual void visit (InitDecl& node) = 0;
  virtual void visit (InstanceDecl& node) = 0;
  virtual void visit (Literal& node) = 0;
  virtual void visit (Map& node) = 0;
  virtual void visit (MethodDecl& node) = 0;
  virtual void visit (ParameterList& node) = 0;
  virtual void visit (Pointer& node) = 0;
  virtual void visit (PullPort& node) = 0;
  virtual void visit (PushPort& node) = 0;
  virtual void visit (PushPortCall& node) = 0;
  virtual void visit (ReactionDecl& node) = 0;
  virtual void visit (Receiver& node) = 0;
  virtual void visit (Return& node) = 0;
  virtual void visit (Select& node) = 0;
  virtual void visit (Slice& node) = 0;
  virtual void visit (SourceFile& node) = 0;
  virtual void visit (StatementList& node) = 0;
  virtual void visit (SubtractAssign& node) = 0;
  virtual void visit (TopLevelDeclList& node) = 0;
  virtual void visit (TypeDecl& node) = 0;
  virtual void visit (TypeExpression& node) = 0;
  virtual void visit (UnaryArithmetic& node) = 0;
  virtual void visit (Var& node) = 0;
  virtual void visit (VariableList& node) = 0;
  virtual void visit (While& node) = 0;
};

struct DefaultNodeVisitor : public NodeVisitor
{
  virtual void default_action (Node& node);
  virtual void visit (ActionDecl& node);
  virtual void visit (Activate& node);
  virtual void visit (AddAssign& node);
  virtual void visit (AddressOf& node);
  virtual void visit (Array& node);
  virtual void visit (Assign& node);
  virtual void visit (BinaryArithmetic& node);
  virtual void visit (BindDecl& node);
  virtual void visit (BindPullPort& node);
  virtual void visit (BindPushPort& node);
  virtual void visit (BindPushPortParameter& node);
  virtual void visit (Call& node);
  virtual void visit (Change& node);
  virtual void visit (CompositeLiteral& node);
  virtual void visit (ConstDecl& node);
  virtual void visit (Conversion& node);
  virtual void visit (Dereference& node);
  virtual void visit (Element& node);
  virtual void visit (ElementList& node);
  virtual void visit (EmptyExpression& node);
  virtual void visit (EmptyStatement& node);
  virtual void visit (EmptyType& node);
  virtual void visit (ExpressionList& node);
  virtual void visit (ExpressionStatement& node);
  virtual void visit (FieldList& node);
  virtual void visit (ForIota& node);
  virtual void visit (FunctionDecl& node);
  virtual void visit (GetterDecl& node);
  virtual void visit (Heap& node);
  virtual void visit (Identifier& node);
  virtual void visit (IdentifierExpression& node);
  virtual void visit (IdentifierList& node);
  virtual void visit (IdentifierType& node);
  virtual void visit (If& node);
  virtual void visit (ImportDeclList& node);
  virtual void visit (IncrementDecrement& node);
  virtual void visit (Index& node);
  virtual void visit (IndexSlice& node);
  virtual void visit (IndexedPushPortCall& node);
  virtual void visit (InitDecl& node);
  virtual void visit (InstanceDecl& node);
  virtual void visit (Literal& node);
  virtual void visit (Map& node);
  virtual void visit (MethodDecl& node);
  virtual void visit (ParameterList& node);
  virtual void visit (Pointer& node);
  virtual void visit (PullPort& node);
  virtual void visit (PushPort& node);
  virtual void visit (PushPortCall& node);
  virtual void visit (ReactionDecl& node);
  virtual void visit (Receiver& node);
  virtual void visit (Return& node);
  virtual void visit (Select& node);
  virtual void visit (Slice& node);
  virtual void visit (SourceFile& node);
  virtual void visit (StatementList& node);
  virtual void visit (SubtractAssign& node);
  virtual void visit (TopLevelDeclList& node);
  virtual void visit (TypeDecl& node);
  virtual void visit (TypeExpression& node);
  virtual void visit (UnaryArithmetic& node);
  virtual void visit (Var& node);
  virtual void visit (VariableList& node);
  virtual void visit (While& node);
};

#ifndef COVERAGE
struct TreePrinter
{
  TreePrinter (Node& a_node);
  Node& node;
  void print (std::ostream& out) const;
};

std::ostream& operator<< (std::ostream& out,
                          const TreePrinter& printer);
#endif
}

#endif // RC_SRC_AST_VISITOR_HPP
