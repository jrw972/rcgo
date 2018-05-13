#ifndef SRC_AST_H_
#define SRC_AST_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <string>
#include <vector>

#include "src/location.h"
#include "src/token.h"
#include "src/value.h"

namespace rcgo {
namespace ast {

// Abstract Syntax Tree for rcgo

struct NodeVisitor;

struct Node {
  explicit Node(const Location& a_location)
    : location(a_location) {}
  virtual ~Node() {}
  virtual void Accept(NodeVisitor* visitor) = 0;

  Location const location;

 protected:
  static void Delete(Node* node) {
    if (node != NULL) delete node;
  }

  template <typename T>
  static void Delete(const T& list) {
    for (ast::Node* node : list) delete node;
  }
};

typedef std::vector<Node*> ListType;

struct Array : public Node {
  Array(const Location& a_location, Node* a_optional_length,
        Node* a_element_type)
      : Node(a_location), optional_length(a_optional_length),
        element_type(a_element_type) {}
  ~Array() override {
    Delete(optional_length);
    Delete(element_type);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const optional_length;
  Node* const element_type;
};

struct Slice : public Node {
  Slice(const Location& a_location, Node* a_element_type)
      : Node(a_location), element_type(a_element_type) {}
  ~Slice() override { Delete(element_type); }
  void Accept(NodeVisitor* visitor) override;

  Node* const element_type;
};

struct Struct : public Node {
  Struct(const Location& a_location, const ListType& a_fields)
      : Node(a_location), fields(a_fields) {}
  ~Struct() override { Delete(fields); }
  void Accept(NodeVisitor* visitor) override;

  ListType const fields;
};

struct Interface : public Node {
  Interface(const Location& a_location, const ListType& a_methods)
      : Node(a_location), methods(a_methods) {}
  ~Interface() override { Delete(methods); }
  void Accept(NodeVisitor* visitor) override;

  ListType const methods;
};

struct Map : public Node {
  Map(const Location& a_location, Node* a_key_type, Node* a_value_type)
      : Node(a_location), key_type(a_key_type), value_type(a_value_type) {}
  ~Map() override {
    Delete(key_type);
    Delete(value_type);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const key_type;
  Node* const value_type;
};

struct EmbeddedField : public Node {
  EmbeddedField(const Location& a_location, bool a_is_pointer,
                Node* a_type_name, const std::string& a_tag)
      : Node(a_location), is_pointer(a_is_pointer), type_name(a_type_name),
        tag(a_tag) {}
  ~EmbeddedField() override { Delete(type_name); }
  void Accept(NodeVisitor* visitor) override;

  bool const is_pointer;
  Node* const type_name;
  std::string const tag;
};

struct Field : public Node {
  Field(const Location& a_location, const ListType& a_identifier_list,
        Node* a_type, const std::string& a_tag)
      : Node(a_location), identifier_list(a_identifier_list), type(a_type),
        tag(a_tag) {}
  ~Field() override {
    Delete(identifier_list);
    Delete(type);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const identifier_list;
  Node* const type;
  std::string const tag;
};

struct AnonymousMethodSpec : public Node {
  AnonymousMethodSpec(const Location& a_location, Node* a_type_name)
      : Node(a_location), type_name(a_type_name) {}
  ~AnonymousMethodSpec() override { Delete(type_name); }
  void Accept(NodeVisitor* visitor) override;

  Node* const type_name;
};

struct MethodSpec : public Node {
  MethodSpec(const Location& a_location, Node* a_identifier, Node* a_signature)
      : Node(location), identifier(a_identifier), signature(a_signature) {}
  ~MethodSpec() override {
    Delete(identifier);
    Delete(signature);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const identifier;
  Node* const signature;
};

struct Pointer : public Node {
  Pointer(const Location& a_location, Node* a_base_type)
      : Node(a_location), base_type(a_base_type) {}
  ~Pointer() override { Delete(base_type); }
  void Accept(NodeVisitor* visitor) override;

  Node* const base_type;
};

struct FunctionType : public Node {
  FunctionType(const Location& a_location, Node* a_signature)
      : Node(a_location), signature(a_signature) {}
  ~FunctionType() override { Delete(signature); }
  void Accept(NodeVisitor* visitor) override;

  Node* const signature;
};

struct Signature : public Node {
  Signature(const Location& a_location, const ListType& a_parameters,
            const ListType& a_results)
      : Node(a_location), parameters(a_parameters), results(a_results) {}
  ~Signature() override {
    Delete(parameters);
    Delete(results);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const parameters;
  ListType const results;
};

struct ParameterDecl : public Node {
  ParameterDecl(const Location& a_location, const ListType& a_identifier_list,
                bool a_variadic, Node* a_type)
      : Node(a_location), identifier_list(a_identifier_list),
        variadic(a_variadic), type(a_type) {}
  ~ParameterDecl() override {
    Delete(identifier_list);
    Delete(type);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const identifier_list;
  bool const variadic;
  Node* const type;
};

struct Literal : public Node {
  explicit Literal(const Token& a_token)
      : Node(a_token.location()), value(a_token.value()) {}
  void Accept(NodeVisitor* visitor) override;

  Value value;
};

struct LiteralValue : public Node {
  LiteralValue(const Location& a_location, const ListType& a_elements)
      : Node(a_location), elements(a_elements) {}
  ~LiteralValue() override { Delete(elements); }
  void Accept(NodeVisitor* visitor) override;

  ListType const elements;
};

struct CompositeLiteral : public Node {
  CompositeLiteral(const Location& a_location, Node* a_type, Node* a_value)
      : Node(a_location), type(a_type), value(a_value) {}
  ~CompositeLiteral() override {
    Delete(type);
    Delete(value);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const type;
  Node* const value;
};

struct KeyedElement : public Node {
  KeyedElement(const Location& a_location, Node* a_key, Node* a_element)
      : Node(location), optional_key(a_key), element(a_element) {}
  ~KeyedElement() override {
    Delete(optional_key);
    Delete(element);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const optional_key;
  Node* const element;
};

struct FunctionLiteral : public Node {
  FunctionLiteral(const Location& a_location, Node* a_type,
                  Node* a_body)
      : Node(a_location), type(a_type), body(a_body) {}
  ~FunctionLiteral() override {
    Delete(type);
    Delete(body);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const type;
  Node* const body;
};

struct Identifier : public Node {
  explicit Identifier(const Token& a_token)
      : Node(a_token.location()), identifier(a_token.identifier()) {}
  void Accept(NodeVisitor* visitor) override;

  std::string const identifier;
  Value value;
};

struct ConstSpec : public Node {
  ConstSpec(const Location& a_location, const ListType& a_identifier_list,
            Node* a_type, const ListType& a_expression_list)
      : Node(a_location), identifier_list(a_identifier_list),
        optional_type(a_type), expression_list(a_expression_list), type(NULL) {}
  ~ConstSpec() override {
    Delete(identifier_list);
    Delete(optional_type);
    Delete(expression_list);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const identifier_list;
  Node* const optional_type;
  ListType const expression_list;

  const type::Type* type;
};

struct ConstDecl : public Node {
  ConstDecl(const Location& a_location, const ListType& a_const_specs)
      : Node(a_location), const_specs(a_const_specs) {}
  ~ConstDecl() override { Delete(const_specs); }
  void Accept(NodeVisitor* visitor) override;

  ListType const const_specs;
};

struct TypeSpec : public Node {
  TypeSpec(const Location& a_location, Node* a_identifier, Node* a_type,
           bool a_is_alias)
      : Node(a_location), identifier(a_identifier), type(a_type),
        is_alias(a_is_alias) {}
  ~TypeSpec() override {
    Delete(identifier);
    Delete(type);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const identifier;
  Node* const type;
  bool const is_alias;
};

struct TypeDecl : public Node {
  TypeDecl(const Location& a_location, const ListType& a_type_specs)
      : Node(a_location), type_specs(a_type_specs) {}
  ~TypeDecl() override { Delete(type_specs); }
  void Accept(NodeVisitor* visitor) override;

  ListType const type_specs;
};

struct VarSpec : public Node {
  VarSpec(const Location& a_location, const ListType& a_identifier_list,
          Node* a_type, const ListType& a_expression_list)
      : Node(a_location), identifier_list(a_identifier_list),
        optional_type(a_type), expression_list(a_expression_list) {}
  ~VarSpec() override {
    Delete(identifier_list);
    Delete(optional_type);
    Delete(expression_list);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const identifier_list;
  Node* const optional_type;
  ListType const expression_list;
};

struct VarDecl : public Node {
  VarDecl(const Location& a_location, const ListType& a_var_specs)
      : Node(a_location), var_specs(a_var_specs) {}
  ~VarDecl() override { Delete(var_specs); }
  void Accept(NodeVisitor* visitor) override;

  ListType const var_specs;
};

struct Block : public Node {
  Block(const Location& a_location, const ListType& a_statements)
      : Node(a_location), statements(a_statements) {}
  ~Block() override { Delete(statements); }
  void Accept(NodeVisitor* visitor) override;

  ListType const statements;
};

struct FuncDecl : public Node {
  FuncDecl(const Location& a_location, Node* a_identifier, Node* a_signature,
           Node* a_body)
      : Node(a_location), identifier(a_identifier), signature(a_signature),
        optional_body(a_body) {}
  ~FuncDecl() override {
    Delete(identifier);
    Delete(signature);
    Delete(optional_body);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const identifier;
  Node* const signature;
  Node* const optional_body;
};

struct MethodDecl : public Node {
  MethodDecl(const Location& a_location, const ListType& a_receiver,
             Node* a_identifier, Node* a_signature, Node* a_body)
    : Node(a_location), receiver(a_receiver), identifier(a_identifier),
      signature(a_signature), optional_body(a_body) {}
  ~MethodDecl() override {
    Delete(receiver);
    Delete(identifier);
    Delete(signature);
    Delete(optional_body);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const receiver;
  Node* const identifier;
  Node* const signature;
  Node* const optional_body;
};

struct TypeAssertion : public Node {
  TypeAssertion(const Location& a_location, Node* a_operand, Node* a_type)
      : Node(a_location), operand(a_operand), type(a_type) {}
  ~TypeAssertion() override {
    Delete(operand);
    Delete(type);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const operand;
  Node* const type;
};

struct Selector : public Node {
  Selector(const Location& a_location, Node* a_operand, Node* a_member)
      : Node(a_location), operand(a_operand), member(a_member) {}
  ~Selector() override {
    Delete(operand);
    Delete(member);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const operand;
  Node* const member;
};

struct SliceOp : public Node {
  SliceOp(const Location& a_location, Node* a_operand, Node* a_low,
          Node* a_high, Node* a_capacity)
      : Node(a_location), operand(a_operand), optional_low(a_low),
        optional_high(a_high), optional_capacity(a_capacity) {}
  ~SliceOp() override {
    Delete(operand);
    Delete(optional_low);
    Delete(optional_high);
    Delete(optional_capacity);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const operand;
  Node* const optional_low;
  Node* const optional_high;
  Node* const optional_capacity;
};

struct Index : public Node {
  Index(const Location& a_location, Node* a_operand, Node* a_index)
      : Node(a_location), operand(a_operand), index(a_index) {}
  ~Index() override {
    Delete(operand);
    Delete(index);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const operand;
  Node* const index;
};

struct Call : public Node {
  Call(const Location& a_location, Node* a_operand, const ListType& a_arguments,
       bool a_variadic)
      : Node(a_location), operand(a_operand), arguments(a_arguments),
        variadic(a_variadic) {}
  ~Call() override {
    Delete(operand);
    Delete(arguments);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const operand;
  ListType const arguments;
  bool const variadic;
};

struct BinaryOp : public Node {
  BinaryOp(const Location& a_location, Node* a_left, Node* a_right)
      : Node(a_location), left(a_left), right(a_right) {}
  ~BinaryOp() override {
    Delete(left);
    Delete(right);
  }

  Node* const left;
  Node* const right;
  Value value;
};

struct Equal : public BinaryOp {
  Equal(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct NotEqual : public BinaryOp {
  NotEqual(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct LessThan : public BinaryOp {
  LessThan(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct LessThanEqual : public BinaryOp {
  LessThanEqual(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct GreaterThan : public BinaryOp {
  GreaterThan(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct GreaterThanEqual : public BinaryOp {
  GreaterThanEqual(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct Add : public BinaryOp {
  Add(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct Subtract : public BinaryOp {
  Subtract(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct BitOr : public BinaryOp {
  BitOr(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct BitXor : public BinaryOp {
  BitXor(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct Multiply : public BinaryOp {
  Multiply(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct Divide : public BinaryOp {
  Divide(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct Modulus : public BinaryOp {
  Modulus(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct LeftShift : public BinaryOp {
  LeftShift(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct RightShift : public BinaryOp {
  RightShift(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct BitAnd : public BinaryOp {
  BitAnd(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct BitAndNot : public BinaryOp {
  BitAndNot(const Location& a_location, Node* a_left, Node* a_right)
      : BinaryOp(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct AssignBase : public Node {
    AssignBase(const Location& a_location, const ListType& a_left,
               const ListType& a_right)
        : Node(a_location), left(a_left), right(a_right) {}
  ~AssignBase() override {
    Delete(left);
    Delete(right);
  }

  ListType const left;
  ListType const right;
};

struct Assign : public AssignBase {
  Assign(const Location& a_location, const ListType& a_left,
         const ListType& a_right)
      : AssignBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct ShortAssign : public AssignBase {
  ShortAssign(const Location& a_location, const ListType& a_left,
              const ListType& a_right)
      : AssignBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct AssignOpBase : public Node {
  AssignOpBase(const Location& a_location, const ListType& a_left,
               Node* a_right)
      : Node(a_location), left(a_left), right(a_right) {}
  ~AssignOpBase() override {
    Delete(left);
    Delete(right);
  }

  ListType const left;
  Node* const right;
};

struct AddAssign : public AssignOpBase {
  AddAssign(const Location& a_location, const ListType& a_left, Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct SubtractAssign : public AssignOpBase {
  SubtractAssign(const Location& a_location, const ListType& a_left,
                 Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct BitOrAssign : public AssignOpBase {
  BitOrAssign(const Location& a_location, const ListType& a_left, Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct BitXorAssign : public AssignOpBase {
  BitXorAssign(const Location& a_location, const ListType& a_left,
               Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct MultiplyAssign : public AssignOpBase {
  MultiplyAssign(const Location& a_location, const ListType& a_left,
                 Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct DivideAssign : public AssignOpBase {
  DivideAssign(const Location& a_location, const ListType& a_left,
               Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct ModulusAssign : public AssignOpBase {
  ModulusAssign(const Location& a_location, const ListType& a_left,
                Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct LeftShiftAssign : public AssignOpBase {
  LeftShiftAssign(const Location& a_location, const ListType& a_left,
                  Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct RightShiftAssign : public AssignOpBase {
  RightShiftAssign(const Location& a_location, const ListType& a_left,
                   Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct BitAndAssign : public AssignOpBase {
  BitAndAssign(const Location& a_location, const ListType& a_left,
               Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct BitAndNotAssign : public AssignOpBase {
  BitAndNotAssign(const Location& a_location, const ListType& a_left,
                  Node* a_right)
      : AssignOpBase(a_location, a_left, a_right) {}
  void Accept(NodeVisitor* visitor) override;
};

struct UnaryOp : public Node {
  UnaryOp(const Location& a_location, Node* a_expression)
      : Node(a_location), expression(a_expression) {}
  ~UnaryOp() override {
    Delete(expression);
  }
  Node* const expression;
  Value value;
};

struct Posate : public UnaryOp {
  explicit Posate(const Location& a_location, Node* a_expression)
      : UnaryOp(a_location, a_expression) {}
  void Accept(NodeVisitor* visitor) override;
};

struct Negate : public UnaryOp {
  explicit Negate(const Location& a_location, Node* a_expression)
      : UnaryOp(a_location, a_expression) {}
  void Accept(NodeVisitor* visitor) override;
};

struct LogicalNot : public UnaryOp {
  explicit LogicalNot(const Location& a_location, Node* a_expression)
      : UnaryOp(a_location, a_expression) {}
  void Accept(NodeVisitor* visitor) override;
};

struct BitNot : public UnaryOp {
  explicit BitNot(const Location& a_location, Node* a_expression)
      : UnaryOp(a_location, a_expression) {}
  void Accept(NodeVisitor* visitor) override;
};

struct Indirection : public UnaryOp {
  explicit Indirection(const Location& a_location, Node* a_expression)
      : UnaryOp(a_location, a_expression) {}
  void Accept(NodeVisitor* visitor) override;
};

struct Address : public UnaryOp {
  explicit Address(const Location& a_location, Node* a_expression)
      : UnaryOp(a_location, a_expression) {}
  void Accept(NodeVisitor* visitor) override;
};

struct TypeExpression : public Node {
  explicit TypeExpression(const Location& a_location)
      : Node(a_location) {}

  void Accept(NodeVisitor* visitor) override;
};

struct Or : public Node {
  Or(const Location& a_location, const ListType& a_expressions)
      : Node(a_location), expressions(a_expressions) {}
  ~Or() override {
    Delete(expressions);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const expressions;
};

struct And : public Node {
  And(const Location& a_location, const ListType& a_expressions)
      : Node(a_location), expressions(a_expressions) {}
  ~And() override {
    Delete(expressions);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const expressions;
};

struct LabeledStatement : public Node {
  LabeledStatement(const Location& a_location, Node* a_label, Node* a_statement)
      : Node(a_location) , label(a_label) , statement(a_statement)
  {}
  ~LabeledStatement() override {
    Delete(label);
    Delete(statement);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const label;
  Node* const statement;
};

struct Goto : public Node {
  Goto(const Location& a_location, Node* a_label)
      : Node(a_location) , label(a_label) {}
  ~Goto() override {
    Delete(label);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const label;
};

struct Fallthrough : public Node {
  explicit Fallthrough(const Location& a_location)
      : Node(a_location) {}
  void Accept(NodeVisitor* visitor) override;
};

struct Defer : public Node {
  Defer(const Location& a_location, Node* a_expression)
      : Node(a_location), expression(a_expression) {}
  ~Defer() override {
    Delete(expression);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const expression;
};

struct EmptyStatement : public Node {
  explicit EmptyStatement(const Location& a_location)
      : Node(a_location) {}
  void Accept(NodeVisitor* visitor) override;
};

struct ExpressionStatement : public Node {
  ExpressionStatement(const Location& a_location, const ListType& a_expression)
      : Node(a_location), expression(a_expression)
  {}
  ~ExpressionStatement() override {
    Delete(expression);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const expression;
};

struct Increment : public Node {
  Increment(const Location& a_location, const ListType& a_expression)
      : Node(a_location), expression(a_expression) {}
  ~Increment() override {
    Delete(expression);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const expression;
};

struct Decrement : public Node {
  explicit Decrement(const Location& a_location, const ListType& a_expression)
      : Node(a_location), expression(a_expression) {}
  ~Decrement() override {
    Delete(expression);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const expression;
};

struct Return : public Node {
  Return(const Location& a_location, const ListType& a_expression)
      : Node(a_location), expression(a_expression) {}
  ~Return() override {
    Delete(expression);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const expression;
};

struct Break : public Node {
  Break(const Location& a_location, Node* a_label)
      : Node(a_location), optional_label(a_label) {}
  ~Break() override {
    Delete(optional_label);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const optional_label;
};

struct Continue : public Node {
  Continue(const Location& a_location, Node* a_label)
      : Node(a_location), optional_label(a_label) {}
  ~Continue() override {
    Delete(optional_label);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const optional_label;
};

struct If : public Node {
  If(const Location& a_location, Node* a_statement, Node* a_expression,
     Node* a_true_branch, Node* a_false_branch)
      : Node(a_location), optional_statement(a_statement),
        expression(a_expression), true_branch(a_true_branch),
        false_branch(a_false_branch) {}
  ~If() override {
    Delete(optional_statement);
    Delete(expression);
    Delete(true_branch);
    Delete(false_branch);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const optional_statement;
  Node* const expression;
  Node* const true_branch;
  Node* const false_branch;
};

struct Switch : public Node {
  Switch(const Location& a_location, Node* a_statement, Node* a_expression,
         const ListType& a_body)
      : Node(a_location), optional_statement(a_statement),
        optional_expression(a_expression), body(a_body) {}
  ~Switch() override {
    Delete(optional_statement);
    Delete(optional_expression);
    Delete(body);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const optional_statement;
  Node* const optional_expression;
  ListType const body;
};

struct Case : public Node {
  Case(const Location& a_location, const ListType& a_expression_list,
       const ListType& a_statement)
      : Node(a_location), expression_list(a_expression_list),
        statement(a_statement) {}
  ~Case() override {
    Delete(expression_list);
    Delete(statement);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const expression_list;
  ListType const statement;
};

struct For : public Node {
  For(const Location& a_location, Node* a_clause, Node* a_body)
      : Node(a_location), clause(a_clause), body(a_body) {}
  ~For() override {
    Delete(clause);
    Delete(body);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const clause;
  Node* const body;
};

struct Range : public Node {
  Range(const Location& a_location, Node* a_expression)
      : Node(a_location) , expression(a_expression) {}
  ~Range() override {
    Delete(expression);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const expression;
};

struct ForClause : public Node {
  ForClause(const Location& a_location, Node* a_init_stmt, Node* a_condition,
            Node* a_post_stmt)
      : Node(a_location), optional_init_stmt(a_init_stmt),
        optional_condition(a_condition),
        optional_post_stmt(a_post_stmt) {}
  ~ForClause() override {
    Delete(optional_init_stmt);
    Delete(optional_condition);
    Delete(optional_post_stmt);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const optional_init_stmt;
  Node* const optional_condition;
  Node* const optional_post_stmt;
};

struct SourceFile : public Node {
  SourceFile(const Location& a_location, Node* a_package,
             const ListType& a_imports, const ListType& a_decls)
      : Node(a_location), package(a_package), imports(a_imports), decls(a_decls)
  {}
  ~SourceFile() override {
    Delete(package);
    Delete(imports);
    Delete(decls);
  }
  void Accept(NodeVisitor* visitor) override;

  Node* const package;
  ListType const imports;
  ListType const decls;
};
typedef std::vector<SourceFile*> SourceFiles;

struct ImportDecl : public Node {
  ImportDecl(const Location& a_location, const ListType& a_import_specs)
      : Node(a_location), import_specs(a_import_specs)
  {}
  ~ImportDecl() override {
    Delete(import_specs);
  }
  void Accept(NodeVisitor* visitor) override;

  ListType const import_specs;
};

struct ImportSpec : public Node {
  ImportSpec(const Location& a_location, bool a_dot, Node* a_identifier,
             const std::string& a_import_path)
      : Node(a_location), dot(a_dot), optional_identifier(a_identifier),
        import_path(a_import_path) {}
  ~ImportSpec() override {
    Delete(optional_identifier);
  }
  void Accept(NodeVisitor* visitor) override;

  bool const dot;
  Node* const optional_identifier;
  std::string const import_path;
};

struct Error : public Node {
  explicit Error(const Location& a_location): Node(a_location) {}
  void Accept(NodeVisitor* visitor) override;
};

struct NodeVisitor {
  virtual ~NodeVisitor() {}
  virtual void Visit(Add* node) = 0;
  virtual void Visit(AddAssign* node) = 0;
  virtual void Visit(Address* node) = 0;
  virtual void Visit(And* node) = 0;
  virtual void Visit(AnonymousMethodSpec* node) = 0;
  virtual void Visit(Array* node) = 0;
  virtual void Visit(Assign* node) = 0;
  virtual void Visit(BitAnd* node) = 0;
  virtual void Visit(BitAndAssign* node) = 0;
  virtual void Visit(BitAndNot* node) = 0;
  virtual void Visit(BitAndNotAssign* node) = 0;
  virtual void Visit(BitNot* node) = 0;
  virtual void Visit(BitOr* node) = 0;
  virtual void Visit(BitOrAssign* node) = 0;
  virtual void Visit(BitXor* node) = 0;
  virtual void Visit(BitXorAssign* node) = 0;
  virtual void Visit(Block* node) = 0;
  virtual void Visit(Break* node) = 0;
  virtual void Visit(Call* node) = 0;
  virtual void Visit(Case* node) = 0;
  virtual void Visit(CompositeLiteral* node) = 0;
  virtual void Visit(ConstDecl* node) = 0;
  virtual void Visit(ConstSpec* node) = 0;
  virtual void Visit(Continue* node) = 0;
  virtual void Visit(Decrement* node) = 0;
  virtual void Visit(Defer* node) = 0;
  virtual void Visit(Divide* node) = 0;
  virtual void Visit(DivideAssign* node) = 0;
  virtual void Visit(EmbeddedField* node) = 0;
  virtual void Visit(EmptyStatement* node) = 0;
  virtual void Visit(Equal* node) = 0;
  virtual void Visit(Error* node) = 0;
  virtual void Visit(ExpressionStatement* node) = 0;
  virtual void Visit(Fallthrough* node) = 0;
  virtual void Visit(Field* node) = 0;
  virtual void Visit(For* node) = 0;
  virtual void Visit(ForClause* node) = 0;
  virtual void Visit(FuncDecl* node) = 0;
  virtual void Visit(FunctionLiteral* node) = 0;
  virtual void Visit(FunctionType* node) = 0;
  virtual void Visit(Goto* node) = 0;
  virtual void Visit(GreaterThan* node) = 0;
  virtual void Visit(GreaterThanEqual* node) = 0;
  virtual void Visit(Identifier* node) = 0;
  virtual void Visit(If* node) = 0;
  virtual void Visit(ImportDecl* node) = 0;
  virtual void Visit(ImportSpec* node) = 0;
  virtual void Visit(Increment* node) = 0;
  virtual void Visit(Index* node) = 0;
  virtual void Visit(Indirection* node) = 0;
  virtual void Visit(Interface* node) = 0;
  virtual void Visit(KeyedElement* node) = 0;
  virtual void Visit(LabeledStatement* node) = 0;
  virtual void Visit(LeftShift* node) = 0;
  virtual void Visit(LeftShiftAssign* node) = 0;
  virtual void Visit(LessThan* node) = 0;
  virtual void Visit(LessThanEqual* node) = 0;
  virtual void Visit(Literal* node) = 0;
  virtual void Visit(LiteralValue* node) = 0;
  virtual void Visit(LogicalNot* node) = 0;
  virtual void Visit(Map* node) = 0;
  virtual void Visit(MethodDecl* node) = 0;
  virtual void Visit(MethodSpec* node) = 0;
  virtual void Visit(Modulus* node) = 0;
  virtual void Visit(ModulusAssign* node) = 0;
  virtual void Visit(Multiply* node) = 0;
  virtual void Visit(MultiplyAssign* node) = 0;
  virtual void Visit(Negate* node) = 0;
  virtual void Visit(NotEqual* node) = 0;
  virtual void Visit(Or* node) = 0;
  virtual void Visit(ParameterDecl* node) = 0;
  virtual void Visit(Pointer* node) = 0;
  virtual void Visit(Posate* node) = 0;
  virtual void Visit(Range* node) = 0;
  virtual void Visit(Return* node) = 0;
  virtual void Visit(RightShift* node) = 0;
  virtual void Visit(RightShiftAssign* node) = 0;
  virtual void Visit(Selector* node) = 0;
  virtual void Visit(ShortAssign* node) = 0;
  virtual void Visit(Signature* node) = 0;
  virtual void Visit(Slice* node) = 0;
  virtual void Visit(SliceOp* node) = 0;
  virtual void Visit(SourceFile* node) = 0;
  virtual void Visit(Struct* node) = 0;
  virtual void Visit(Subtract* node) = 0;
  virtual void Visit(SubtractAssign* node) = 0;
  virtual void Visit(Switch* node) = 0;
  virtual void Visit(TypeAssertion* node) = 0;
  virtual void Visit(TypeDecl* node) = 0;
  virtual void Visit(TypeExpression* node) = 0;
  virtual void Visit(TypeSpec* node) = 0;
  virtual void Visit(VarDecl* node) = 0;
  virtual void Visit(VarSpec* node) = 0;
  void VisitAll(const ListType& list) {
    for (ast::Node* node : list) node->Accept(this);
  }
};

struct DefaultNodeVisitor : public NodeVisitor {
  void Visit(Add* node) override { DefaultAction(node); }
  void Visit(AddAssign* node) override { DefaultAction(node); }
  void Visit(Address* node) override { DefaultAction(node); }
  void Visit(And* node) override { DefaultAction(node); }
  void Visit(AnonymousMethodSpec* node) override { DefaultAction(node); }
  void Visit(Array* node) override { DefaultAction(node); }
  void Visit(Assign* node) override { DefaultAction(node); }
  void Visit(BitAnd* node) override { DefaultAction(node); }
  void Visit(BitAndAssign* node) override { DefaultAction(node); }
  void Visit(BitAndNot* node) override { DefaultAction(node); }
  void Visit(BitAndNotAssign* node) override { DefaultAction(node); }
  void Visit(BitNot* node) override { DefaultAction(node); }
  void Visit(BitOr* node) override { DefaultAction(node); }
  void Visit(BitOrAssign* node) override { DefaultAction(node); }
  void Visit(BitXor* node) override { DefaultAction(node); }
  void Visit(BitXorAssign* node) override { DefaultAction(node); }
  void Visit(Block* node) override { DefaultAction(node); }
  void Visit(Break* node) override { DefaultAction(node); }
  void Visit(Call* node) override { DefaultAction(node); }
  void Visit(Case* node) override { DefaultAction(node); }
  void Visit(CompositeLiteral* node) override { DefaultAction(node); }
  void Visit(ConstDecl* node) override { DefaultAction(node); }
  void Visit(ConstSpec* node) override { DefaultAction(node); }
  void Visit(Continue* node) override { DefaultAction(node); }
  void Visit(Decrement* node) override { DefaultAction(node); }
  void Visit(Defer* node) override { DefaultAction(node); }
  void Visit(Divide* node) override { DefaultAction(node); }
  void Visit(DivideAssign* node) override { DefaultAction(node); }
  void Visit(EmbeddedField* node) override { DefaultAction(node); }
  void Visit(EmptyStatement* node) override { DefaultAction(node); }
  void Visit(Equal* node) override { DefaultAction(node); }
  void Visit(Error* node) override { DefaultAction(node); }
  void Visit(ExpressionStatement* node) override { DefaultAction(node); }
  void Visit(Fallthrough* node) override { DefaultAction(node); }
  void Visit(Field* node) override { DefaultAction(node); }
  void Visit(For* node) override { DefaultAction(node); }
  void Visit(ForClause* node) override { DefaultAction(node); }
  void Visit(FuncDecl* node) override { DefaultAction(node); }
  void Visit(FunctionLiteral* node) override { DefaultAction(node); }
  void Visit(FunctionType* node) override { DefaultAction(node); }
  void Visit(Goto* node) override { DefaultAction(node); }
  void Visit(GreaterThan* node) override { DefaultAction(node); }
  void Visit(GreaterThanEqual* node) override { DefaultAction(node); }
  void Visit(Identifier* node) override { DefaultAction(node); }
  void Visit(If* node) override { DefaultAction(node); }
  void Visit(ImportDecl* node) override { DefaultAction(node); }
  void Visit(ImportSpec* node) override { DefaultAction(node); }
  void Visit(Increment* node) override { DefaultAction(node); }
  void Visit(Index* node) override { DefaultAction(node); }
  void Visit(Indirection* node) override { DefaultAction(node); }
  void Visit(Interface* node) override { DefaultAction(node); }
  void Visit(KeyedElement* node) override { DefaultAction(node); }
  void Visit(LabeledStatement* node) override { DefaultAction(node); }
  void Visit(LeftShift* node) override { DefaultAction(node); }
  void Visit(LeftShiftAssign* node) override { DefaultAction(node); }
  void Visit(LessThan* node) override { DefaultAction(node); }
  void Visit(LessThanEqual* node) override { DefaultAction(node); }
  void Visit(Literal* node) override { DefaultAction(node); }
  void Visit(LiteralValue* node) override { DefaultAction(node); }
  void Visit(LogicalNot* node) override { DefaultAction(node); }
  void Visit(Map* node) override { DefaultAction(node); }
  void Visit(MethodDecl* node) override { DefaultAction(node); }
  void Visit(MethodSpec* node) override { DefaultAction(node); }
  void Visit(Modulus* node) override { DefaultAction(node); }
  void Visit(ModulusAssign* node) override { DefaultAction(node); }
  void Visit(Multiply* node) override { DefaultAction(node); }
  void Visit(MultiplyAssign* node) override { DefaultAction(node); }
  void Visit(Negate* node) override { DefaultAction(node); }
  void Visit(NotEqual* node) override { DefaultAction(node); }
  void Visit(Or* node) override { DefaultAction(node); }
  void Visit(ParameterDecl* node) override { DefaultAction(node); }
  void Visit(Pointer* node) override { DefaultAction(node); }
  void Visit(Posate* node) override { DefaultAction(node); }
  void Visit(Range* node) override { DefaultAction(node); }
  void Visit(Return* node) override { DefaultAction(node); }
  void Visit(RightShift* node) override { DefaultAction(node); }
  void Visit(RightShiftAssign* node) override { DefaultAction(node); }
  void Visit(Selector* node) override { DefaultAction(node); }
  void Visit(ShortAssign* node) override { DefaultAction(node); }
  void Visit(Signature* node) override { DefaultAction(node); }
  void Visit(Slice* node) override { DefaultAction(node); }
  void Visit(SliceOp* node) override { DefaultAction(node); }
  void Visit(SourceFile* node) override { DefaultAction(node); }
  void Visit(Struct* node) override { DefaultAction(node); }
  void Visit(Subtract* node) override { DefaultAction(node); }
  void Visit(SubtractAssign* node) override { DefaultAction(node); }
  void Visit(Switch* node) override { DefaultAction(node); }
  void Visit(TypeAssertion* node) override { DefaultAction(node); }
  void Visit(TypeDecl* node) override { DefaultAction(node); }
  void Visit(TypeExpression* node) override { DefaultAction(node); }
  void Visit(TypeSpec* node) override { DefaultAction(node); }
  void Visit(VarDecl* node) override { DefaultAction(node); }
  void Visit(VarSpec* node) override { DefaultAction(node); }
  virtual void DefaultAction(Node* node) {}
};

template<typename T>
T* Cast(Node* node) {
  if (node == NULL) return NULL;

  struct Visitor : public DefaultNodeVisitor {
    T* retval;
    Visitor() : retval(NULL) {}
    void Visit(T* node) override { retval = node; }
  };

  Visitor v;
  node->Accept(&v);
  return v.retval;
}

struct IdentifierEqual {  // NOT_COVERED
  explicit IdentifierEqual(const std::string& a_identifier)
      : identifier(a_identifier) {}
  bool operator()(Node* a_node) {
    return identifier == Cast<Identifier>(a_node)->identifier;
  }

  std::string const identifier;
};

Value GetValue(Node* node);

std::ostream& operator<< (std::ostream& out, Node& ast);

}  // namespace ast
}  // namespace rcgo

#endif  // SRC_AST_H_
