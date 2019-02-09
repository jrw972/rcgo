// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/ast.h"

#include "src/token.h"

namespace rcgo {
namespace ast {

void Array::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Slice::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Struct::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Interface::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Map::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void EmbeddedField::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Field::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void AnonymousMethodSpec::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void MethodSpec::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Pointer::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void FunctionType::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Signature::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void ParameterDecl::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Literal::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void LiteralValue::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void CompositeLiteral::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void KeyedElement::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void FunctionLiteral::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Identifier::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void ConstSpec::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void ConstDecl::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void TypeSpec::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void TypeDecl::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void VarSpec::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void VarDecl::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Block::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void FuncDecl::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void MethodDecl::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void TypeAssertion::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Selector::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void SliceOp::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Index::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Call::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Equal::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void NotEqual::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void LessThan::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void LessThanEqual::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void GreaterThan::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void GreaterThanEqual::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Add::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Subtract::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void BitOr::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void BitXor::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Multiply::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Divide::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Modulus::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void LeftShift::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void RightShift::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void BitAnd::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void BitAndNot::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Assign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void ShortAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void AddAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void SubtractAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void BitOrAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void BitXorAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void MultiplyAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void DivideAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void ModulusAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void LeftShiftAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void RightShiftAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void BitAndAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void BitAndNotAssign::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Posate::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Negate::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void LogicalNot::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void BitNot::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Indirection::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Address::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void TypeExpression::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Or::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void And::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void LabeledStatement::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Goto::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Fallthrough::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Defer::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void EmptyStatement::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void ExpressionStatement::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Increment::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Decrement::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Return::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Break::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Continue::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void If::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Switch::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Case::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void For::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Range::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void ForClause::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void SourceFile::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void ImportDecl::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void ImportSpec::Accept(NodeVisitor* visitor) { visitor->Visit(this); }
void Error::Accept(NodeVisitor* visitor) { visitor->Visit(this); }

std::ostream& operator<<(std::ostream& out, Node& ast) {
  struct Visitor : public NodeVisitor {
    std::ostream& out;

    explicit Visitor(std::ostream& a_out)
        : out(a_out) {}

    void PrintField(bool comma, const char* name, Node* value) {
      if (comma) out << ',';
      out << '"' << name << '"' << ':';
      if (value) {
        value->Accept(this);
      } else {
        out << "null";
      }
    }

    void PrintField(bool comma, const char* name, const std::string& value) {
      if (comma) out << ',';
      out << '"' << name << '"' << ':' << '"' << value << '"';
    }

    void PrintField(bool comma, const char* name, const char* value) {
      if (comma) out << ',';
      out << '"' << name << '"' << ':' << '"' << value << '"';
    }

    void PrintField(bool comma, const char* name, bool value) {
      if (comma) out << ',';
      out << '"' << name << '"' << ':' << '"' <<(value ? "true" : "false")
          << '"';
    }

    void PrintField(bool comma, const char* name, const value::Value& value) {
      if (comma) out << ',';
      out << '"' << name << '"' << ':' << '"' << "<value>" << '"';
    }

    void PrintField(bool comma, const char* name, const ListType& value) {
      if (comma) out << ',';
      out << '"' << name << '"' << ':'
          << '[';
      bool flag = false;
      for (Node* node : value) {
        if (flag) out << ',';
        node->Accept(this);
        flag = true;
      }
      out << ']';
    }

    void Visit(Array* ast) {
      out << '{';
      PrintField(false, "class", "Array");
      PrintField(true, "optional_length", ast->optional_length);
      PrintField(true, "element_type", ast->element_type);
      out << '}';
    }
    void Visit(Slice* ast) {
      out << '{';
      PrintField(false, "class", "Slice");
      PrintField(true, "element_type", ast->element_type);
      out << '}';
    }
    void Visit(Struct* ast) {
      out << '{';
      PrintField(false, "class", "Struct");
      PrintField(true, "fields", ast->fields);
      out << '}';
    }
    void Visit(Interface* ast) {
      out << '{';
      PrintField(false, "class", "Interface");
      PrintField(true, "methods", ast->methods);
      out << '}';
    }
    void Visit(Map* ast) {
      out << '{';
      PrintField(false, "class", "Map");
      PrintField(true, "key_type", ast->key_type);
      PrintField(true, "value_type", ast->value_type);
      out << '}';
    }
    void Visit(EmbeddedField* ast) {
      out << '{';
      PrintField(false, "class", "EmbeddedField");
      PrintField(true, "is_pointer", ast->is_pointer);
      PrintField(true, "type_name", ast->type_name);
      PrintField(true, "tag", ast->tag);
      out << '}';
    }
    void Visit(Field* ast) {
      out << '{';
      PrintField(false, "class", "Field");
      PrintField(true, "identifier_list", ast->identifier_list);
      PrintField(true, "type_literal", ast->type_literal);
      PrintField(true, "tag", ast->tag);
      out << '}';
    }
    void Visit(AnonymousMethodSpec* ast) {
      out << '{';
      PrintField(false, "class", "AnonymousMethodSpec");
      PrintField(true, "type_name", ast->type_name);
      out << '}';
    }
    void Visit(MethodSpec* ast) {
      out << '{';
      PrintField(false, "class", "MethodSpec");
      PrintField(true, "identifier", ast->identifier);
      PrintField(true, "signature", ast->signature);
      out << '}';
    }
    void Visit(Pointer* ast) {
      out << '{';
      PrintField(false, "class", "Pointer");
      PrintField(true, "base_type", ast->base_type);
      out << '}';
    }
    void Visit(FunctionType* ast) {
      out << '{';
      PrintField(false, "class", "FunctionType");
      PrintField(true, "signature", ast->signature);
      out << '}';
    }
    void Visit(Signature* ast) {
      out << '{';
      PrintField(false, "class", "Signature");
      PrintField(true, "parameters", ast->parameters);
      PrintField(true, "results", ast->results);
      out << '}';
    }
    void Visit(ParameterDecl* ast) {
      out << '{';
      PrintField(false, "class", "ParameterDecl");
      PrintField(true, "optional_identifier_list", ast->identifier_list);
      PrintField(true, "variadic", ast->variadic);
      PrintField(true, "type_literal", ast->type_literal);
      out << '}';
    }
    void Visit(Literal* ast) {
      out << '{';
      PrintField(false, "class", "Literal");
      PrintField(true, "in_value", ast->computed_value());
      out << '}';
    }
    void Visit(CompositeLiteral* ast) {
      out << '{';
      PrintField(false, "class", "CompositeLiteral");
      PrintField(true, "type_literal", ast->type_literal);
      PrintField(true, "value", ast->value);
      out << '}';
    }
    void Visit(KeyedElement* ast) {
      out << '{';
      PrintField(false, "class", "KeyedElement");
      PrintField(true, "optional_key", ast->optional_key);
      PrintField(true, "element", ast->element);
      out << '}';
    }
    void Visit(FunctionLiteral* ast) {
      out << '{';
      PrintField(false, "class", "FunctionLiteral");
      PrintField(true, "type_literal", ast->type_literal);
      PrintField(true, "body", ast->body);
      out << '}';
    }
    void Visit(Identifier* ast) {
      out << '{';
      PrintField(false, "class", "Identifier");
      PrintField(true, "identifier", ast->identifier);
      out << '}';
    }
    void Visit(ConstSpec* ast) {
      out << '{';
      PrintField(false, "class", "ConstSpec");
      PrintField(true, "identifier_list", ast->identifier_list);
      PrintField(true, "optional_type_literal", ast->optional_type_literal);
      PrintField(true, "expression_list", ast->expression_list);
      out << '}';
    }
    void Visit(TypeSpec* ast) {
      out << '{';
      PrintField(false, "class", "TypeSpec");
      PrintField(true, "identifier", ast->identifier);
      PrintField(true, "type_literal", ast->type_literal);
      PrintField(true, "is_alias", ast->is_alias);
      out << '}';
    }
    void Visit(VarSpec* ast) {
      out << '{';
      PrintField(false, "class", "VarSpec");
      PrintField(true, "identifier_list", ast->identifier_list);
      PrintField(true, "optional_type_literal", ast->optional_type_literal);
      PrintField(true, "optional_expression_list", ast->expression_list);
      out << '}';
    }
    void Visit(FuncDecl* ast) {
      out << '{';
      PrintField(false, "class", "FuncDecl");
      PrintField(true, "identifier", ast->identifier);
      PrintField(true, "signature", ast->signature);
      PrintField(true, "optional_body", ast->optional_body);
      out << '}';
    }
    void Visit(MethodDecl* ast) {
      out << '{';
      PrintField(false, "class", "MethodDecl");
      PrintField(true, "receiver", ast->receiver);
      PrintField(true, "identifier", ast->identifier);
      PrintField(true, "signature", ast->signature);
      PrintField(true, "optional_body", ast->optional_body);
      out << '}';
    }
    void Visit(TypeAssertion* ast) {
      out << '{';
      PrintField(false, "class", "TypeAssertion");
      PrintField(true, "operand", ast->operand);
      PrintField(true, "type_literal", ast->type_literal);
      out << '}';
    }
    void Visit(Selector* ast) {
      out << '{';
      PrintField(false, "class", "Selector");
      PrintField(true, "operand", ast->operand);
      PrintField(true, "member", ast->member);
      out << '}';
    }
    void Visit(SliceOp* ast) {
      out << '{';
      PrintField(false, "class", "SliceOp");
      PrintField(true, "operand", ast->operand);
      PrintField(true, "optional_low", ast->optional_low);
      PrintField(true, "optional_high", ast->optional_high);
      PrintField(true, "optional_capacity", ast->optional_capacity);
      out << '}';
    }
    void Visit(Index* ast) {
      out << '{';
      PrintField(false, "class", "Index");
      PrintField(true, "operand", ast->operand);
      PrintField(true, "index", ast->index);
      out << '}';
    }
    void Visit(Call* ast) {
      out << '{';
      PrintField(false, "class", "Call");
      PrintField(true, "operand", ast->operand);
      PrintField(true, "arguments", ast->arguments);
      PrintField(true, "variadic", ast->variadic);
      out << '}';
    }
    void Visit(Equal* ast) {
      out << '{';
      PrintField(false, "class", "Equal");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(NotEqual* ast) {
      out << '{';
      PrintField(false, "class", "NotEqual");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(LessThan* ast) {
      out << '{';
      PrintField(false, "class", "LessThan");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(LessThanEqual* ast) {
      out << '{';
      PrintField(false, "class", "LessThanEqual");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(GreaterThan* ast) {
      out << '{';
      PrintField(false, "class", "GreaterThan");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(GreaterThanEqual* ast) {
      out << '{';
      PrintField(false, "class", "GreaterThanEqual");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(Add* ast) {
      out << '{';
      PrintField(false, "class", "Add");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(Subtract* ast) {
      out << '{';
      PrintField(false, "class", "Subtract");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(BitOr* ast) {
      out << '{';
      PrintField(false, "class", "BitOr");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(BitXor* ast) {
      out << '{';
      PrintField(false, "class", "BitXor");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(Multiply* ast) {
      out << '{';
      PrintField(false, "class", "Multiply");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(Divide* ast) {
      out << '{';
      PrintField(false, "class", "Divide");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(Modulus* ast) {
      out << '{';
      PrintField(false, "class", "Modulus");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(LeftShift* ast) {
      out << '{';
      PrintField(false, "class", "LeftShift");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(RightShift* ast) {
      out << '{';
      PrintField(false, "class", "RightShift");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(BitAnd* ast) {
      out << '{';
      PrintField(false, "class", "BitAnd");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(BitAndNot* ast) {
      out << '{';
      PrintField(false, "class", "BitAndNot");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(Assign* ast) {
      out << '{';
      PrintField(false, "class", "Assign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(ShortAssign* ast) {
      out << '{';
      PrintField(false, "class", "ShortAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(AddAssign* ast) {
      out << '{';
      PrintField(false, "class", "AddAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(SubtractAssign* ast) {
      out << '{';
      PrintField(false, "class", "SubtractAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(BitOrAssign* ast) {
      out << '{';
      PrintField(false, "class", "BitOrAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(BitXorAssign* ast) {
      out << '{';
      PrintField(false, "class", "BitXorAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(MultiplyAssign* ast) {
      out << '{';
      PrintField(false, "class", "MultiplyAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(DivideAssign* ast) {
      out << '{';
      PrintField(false, "class", "DivideAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(ModulusAssign* ast) {
      out << '{';
      PrintField(false, "class", "ModulusAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(LeftShiftAssign* ast) {
      out << '{';
      PrintField(false, "class", "LeftShiftAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(RightShiftAssign* ast) {
      out << '{';
      PrintField(false, "class", "RightShiftAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(BitAndAssign* ast) {
      out << '{';
      PrintField(false, "class", "BitAndAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(BitAndNotAssign* ast) {
      out << '{';
      PrintField(false, "class", "BitAndNotAssign");
      PrintField(true, "left", ast->left);
      PrintField(true, "right", ast->right);
      out << '}';
    }
    void Visit(TypeExpression* ast) {
      out << '{';
      PrintField(false, "class", "TypeExpression");
      out << '}';
    }
    void Visit(Or* ast) {
      out << '{';
      PrintField(false, "class", "Or");
      PrintField(true, "expressions", ast->expressions);
      out << '}';
    }
    void Visit(And* ast) {
      out << '{';
      PrintField(false, "class", "And");
      PrintField(true, "expressions", ast->expressions);
      out << '}';
    }
    void Visit(LabeledStatement* ast) {
      out << '{';
      PrintField(false, "class", "LabeledStatement");
      PrintField(true, "label", ast->label);
      PrintField(true, "statement", ast->statement);
      out << '}';
    }
    void Visit(Goto* ast) {
      out << '{';
      PrintField(false, "class", "Goto");
      PrintField(true, "label", ast->label);
      out << '}';
    }
    void Visit(Fallthrough* ast) {
      out << '{';
      PrintField(false, "class", "Fallthrough");
      out << '}';
    }
    void Visit(Defer* ast) {
      out << '{';
      PrintField(false, "class", "Defer");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(EmptyStatement* ast) {
      out << '{';
      PrintField(false, "class", "EmptyStatement");
      out << '}';
    }
    void Visit(ExpressionStatement* ast) {
      out << '{';
      PrintField(false, "class", "ExpressionStatement");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(Increment* ast) {
      out << '{';
      PrintField(false, "class", "Increment");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(Decrement* ast) {
      out << '{';
      PrintField(false, "class", "Decrement");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(Return* ast) {
      out << '{';
      PrintField(false, "class", "Return");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(Break* ast) {
      out << '{';
      PrintField(false, "class", "Break");
      PrintField(true, "optional_label", ast->optional_label);
      out << '}';
    }
    void Visit(Continue* ast) {
      out << '{';
      PrintField(false, "class", "Continue");
      PrintField(true, "optional_label", ast->optional_label);
      out << '}';
    }
    void Visit(If* ast) {
      out << '{';
      PrintField(false, "class", "If");
      PrintField(true, "optional_statement", ast->optional_statement);
      PrintField(true, "expression", ast->expression);
      PrintField(true, "true_branch", ast->true_branch);
      PrintField(true, "false_branch", ast->false_branch);
      out << '}';
    }
    void Visit(Switch* ast) {
      out << '{';
      PrintField(false, "class", "Switch");
      PrintField(true, "optional_statement", ast->optional_statement);
      PrintField(true, "optional_expression", ast->optional_expression);
      PrintField(true, "body", ast->body);
      out << '}';
    }
    void Visit(Case* ast) {
      out << '{';
      PrintField(false, "class", "Case");
      PrintField(true, "optional_expression_list", ast->expression_list);
      PrintField(true, "statement", ast->statement);
      out << '}';
    }
    void Visit(For* ast) {
      out << '{';
      PrintField(false, "class", "For");
      PrintField(true, "clause", ast->clause);
      PrintField(true, "body", ast->body);
      out << '}';
    }
    void Visit(Range* ast) {
      out << '{';
      PrintField(false, "class", "Range");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(ForClause* ast) {
      out << '{';
      PrintField(false, "class", "ForClause");
      PrintField(true, "optional_init_stmt", ast->optional_init_stmt);
      PrintField(true, "optional_condition", ast->optional_condition);
      PrintField(true, "optional_post_stmt", ast->optional_post_stmt);
      out << '}';
    }
    void Visit(SourceFile* ast) {
      out << '{';
      PrintField(false, "class", "SourceFile");
      PrintField(true, "package", ast->package);
      PrintField(true, "imports", ast->imports);
      PrintField(true, "decls", ast->decls);
      out << '}';
    }
    void Visit(ImportDecl* ast) {
      out << '{';
      PrintField(false, "class", "ImportDecl");
      PrintField(true, "import_specs", ast->import_specs);
      out << '}';
    }
    void Visit(ImportSpec* ast) {
      out << '{';
      PrintField(false, "class", "ImportSpec");
      PrintField(true, "dot", ast->dot);
      PrintField(true, "optional_identifier", ast->optional_identifier);
      PrintField(true, "import_path", ast->import_path);
      out << '}';
    }
    void Visit(Error* ast) {
      out << '{';
      PrintField(false, "class", "Error");
      out << '}';
    }
    void Visit(LiteralValue* ast) {
      out << '{';
      PrintField(false, "class", "LiteralValue");
      PrintField(true, "elements", ast->elements);
      out << '}';
    }
    void Visit(ConstDecl* ast) {
      out << '{';
      PrintField(false, "class", "ConstDecl");
      PrintField(true, "const_specs", ast->const_specs);
      out << '}';
    }
    void Visit(VarDecl* ast) {
      out << '{';
      PrintField(false, "class", "VarDecl");
      PrintField(true, "var_specs", ast->var_specs);
      out << '}';
    }
    void Visit(Block* ast) {
      out << '{';
      PrintField(false, "class", "Block");
      PrintField(true, "statements", ast->statements);
      out << '}';
    }
    void Visit(TypeDecl* ast) {
      out << '{';
      PrintField(false, "class", "TypeDecl");
      PrintField(true, "type_specs", ast->type_specs);
      out << '}';
    }
    void Visit(Posate* ast) {
      out << '{';
      PrintField(false, "class", "Posate");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(Address* ast) {
      out << '{';
      PrintField(false, "class", "Address");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(BitNot* ast) {
      out << '{';
      PrintField(false, "class", "BitNot");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(Indirection* ast) {
      out << '{';
      PrintField(false, "class", "Indirection");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(LogicalNot* ast) {
      out << '{';
      PrintField(false, "class", "LogicalNot");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
    void Visit(Negate* ast) {
      out << '{';
      PrintField(false, "class", "Negate");
      PrintField(true, "expression", ast->expression);
      out << '}';
    }
  };
  Visitor visitor(out);
  ast.Accept(&visitor);

  return out;
}

}  // namespace ast
}  // namespace rcgo
