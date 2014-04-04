/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Ast.hpp"
#include "AstVisitor.hpp"
#include "Token.hpp"

#include <iostream>

#define ACCEPT(c)  void c::accept (AstVisitor& visitor) { visitor.visit (*this); }

namespace rcgo {

  Ast::Ast (const Location& aLocation)
    : location (aLocation)
  { }

  Ast::Ast (const ChildrenType& a_children)
    : location (a_children.front ()->location)
    , m_children (a_children)
  { }

  Ast::~Ast ()
  {
    for (const_iterator pos = m_children.begin (),
           limit = m_children.end ();
         pos != limit;
         ++pos)
      {
        delete *pos;
      }
  }

  void Ast::visit_children (AstVisitor& visitor)
  {
  for (ChildrenType::const_iterator pos = begin (),
         limit = end ();
       pos != limit;
       ++pos)
    {
      (*pos)->accept (visitor);
    }
  }

  void Ast::append (Ast* item)
  {
    if (item != NULL)
      {
        m_children.push_back (item);
      }
  }

  Ast* Ast::at (size_t idx) const
  {
    return m_children.at (idx);
  }

  Ast::const_iterator Ast::begin () const
  {
    return m_children.begin ();
  }

  Ast::const_iterator Ast::end () const
  {
    return m_children.end ();
  }

  size_t Ast::size () const
  {
    return m_children.size ();
  }

  ListAst::ListAst (const Location& a_location)
    : Ast (a_location)
  { }

  ListAst::ListAst (const ChildrenType& a_children)
    : Ast (a_children)
  { }

  ListAst::ListAst (Ast* a_item)
    : Ast (a_item->location)
  {
    append (a_item);
  }

  ListAst::~ListAst ()
  { }

  ACCEPT (ListAst)

  ArrayAst::ArrayAst (const Location& a_location, Ast* a_optional_length, Ast* aElementType)
    : Ast (a_location)
    , optional_length (a_optional_length)
    , element_type (aElementType)
  {
    assert (element_type != NULL);
    append (optional_length);
    append (element_type);
  }

  ArrayAst::~ArrayAst ()
  { }

  ACCEPT (ArrayAst)

  SliceTypeAst::SliceTypeAst (const Location& a_location, Ast* aElementType)
    : Ast (a_location)
    , element_type (aElementType)
  {
    assert (element_type != NULL);
    append (element_type);
  }

  SliceTypeAst::~SliceTypeAst ()
  { }

  ACCEPT (SliceTypeAst)

  StructAst::StructAst (const Location& a_location)
    : ListAst (a_location)
  { }

  StructAst::~StructAst ()
  { }

  ACCEPT (StructAst)

  InterfaceAst::InterfaceAst (const Location& a_location)
    : ListAst (a_location)
  { }

  InterfaceAst::~InterfaceAst ()
  { }

  ACCEPT (InterfaceAst)

  MapAst::MapAst (const Location& a_location, Ast* aKeyType, Ast* aValueType)
    : Ast (a_location)
    , key_type (aKeyType)
    , value_type (aValueType)
  {
    assert (key_type != NULL);
    assert (value_type != NULL);
    append (key_type);
    append (value_type);
  }

  MapAst::~MapAst ()
  { }

  ACCEPT (MapAst)

  EmbeddedFieldAst::EmbeddedFieldAst (const Location& a_location, bool aIsPointer, Ast* aTypeName, const std::string& aTag)
    : Ast (a_location)
    , is_pointer (aIsPointer)
    , type_name (aTypeName)
    , tag (aTag)
  {
    assert (type_name != NULL);
    append (type_name);
  }

  EmbeddedFieldAst::~EmbeddedFieldAst ()
  { }

  ACCEPT (EmbeddedFieldAst)

  FieldAst::FieldAst (Ast* aIdentifierList, Ast* aType, const std::string& aTag)
    : Ast (aIdentifierList->location)
    , identifier_list (aIdentifierList)
    , type (aType)
    , tag (aTag)
  {
    assert (identifier_list != NULL);
    assert (type != NULL);
    append (identifier_list);
    append (type);
  }

  FieldAst::~FieldAst ()
  { }

  ACCEPT (FieldAst)

  AnonymousMethodSpecAst::AnonymousMethodSpecAst (Ast* aTypeName)
    : Ast (aTypeName->location)
    , type_name (aTypeName)
  {
    assert (type_name != NULL);
    append (type_name);
  }

  AnonymousMethodSpecAst::~AnonymousMethodSpecAst ()
  { }

  ACCEPT (AnonymousMethodSpecAst)

  MethodSpecAst::MethodSpecAst (Ast* aIdentifier, Ast* aSignature)
    : Ast (aIdentifier->location)
    , identifier (aIdentifier)
    , signature (aSignature)
  {
    assert (identifier != NULL);
    assert (signature != NULL);
    append (identifier);
    append (signature);
  }

  MethodSpecAst::~MethodSpecAst ()
  { }

  ACCEPT (MethodSpecAst)

  PointerAst::PointerAst (const Location& a_location, Ast* aBaseType)
    : Ast (a_location)
    , base_type (aBaseType)
  {
    assert (base_type != NULL);
    append (base_type);
  }

  PointerAst::~PointerAst ()
  { }

  ACCEPT (PointerAst)

  FunctionTypeAst::FunctionTypeAst (const Location& a_location, Ast* aSignature)
    : Ast (a_location)
    , signature (aSignature)
  {
    assert (signature != NULL);
    append (signature);
  }

  FunctionTypeAst::~FunctionTypeAst ()
  { }

  ACCEPT (FunctionTypeAst)

  SignatureAst::SignatureAst (Ast* aParameters, Ast* aResult)
    : Ast (aParameters->location)
    , parameters (aParameters)
    , results (aResult)
  {
    assert (parameters != NULL);
    assert (results != NULL);
    append (parameters);
    append (results);
  }

  SignatureAst::~SignatureAst ()
  { }

  ACCEPT (SignatureAst)

  ParameterDeclAst::ParameterDeclAst (Ast* aIdentifierList, bool aVariadic, Ast* aType)
    : Ast (aIdentifierList != NULL ? aIdentifierList->location : aType->location)
    , optional_identifier_list (aIdentifierList)
    , variadic (aVariadic)
    , type (aType)
  {
    assert (type != NULL);
    append (optional_identifier_list);
    append (type);
  }

  ParameterDeclAst::~ParameterDeclAst ()
  { }

  ACCEPT (ParameterDeclAst)

  LiteralAst::LiteralAst (const Token& aToken)
    : Ast (aToken.location ())
  {
    value = aToken.value ();
  }

  LiteralAst::~LiteralAst ()
  { }

  ACCEPT (LiteralAst)

  CompositeLiteralAst::CompositeLiteralAst (Ast* aType, Ast* aValue)
    : Ast (aType->location)
    , type (aType)
    , value (aValue)
  {
    assert (type != NULL);
    assert (value != NULL);
    append (type);
    append (value);
  }

  CompositeLiteralAst::~CompositeLiteralAst ()
  { }

  ACCEPT (CompositeLiteralAst)

  KeyedElementAst::KeyedElementAst (Ast* aKey, Ast* aElement)
    : Ast (aKey != NULL ? aKey->location : aElement->location)
    , optional_key (aKey)
    , element (aElement)
  {
    assert (element != NULL);
    append (optional_key);
    append (element);
  }

  KeyedElementAst::~KeyedElementAst ()
  { }

  ACCEPT (KeyedElementAst)

  FunctionLiteralAst::FunctionLiteralAst (const Location& a_location, Ast* aType, Ast* aBody)
    : Ast (a_location)
    , type (aType)
    , body (aBody)
  {
    assert (type != NULL);
    assert (body != NULL);
    append (type);
    append (body);
  }

  FunctionLiteralAst::~FunctionLiteralAst ()
  { }

  ACCEPT (FunctionLiteralAst)

  IdentifierAst::IdentifierAst (const Token& a_token)
    : Ast (a_token.location ())
    , identifier (a_token.identifier ())
  { }

  IdentifierAst::~IdentifierAst ()
  { }

  ACCEPT (IdentifierAst)

  ConstSpecAst::ConstSpecAst (const Location& aLocation, Ast* aIdentifierList, Ast* aType, Ast* aExpressionList)
    : Ast (aLocation)
    , identifier_list (aIdentifierList)
    , optional_type (aType)
    , optional_expression_list (aExpressionList)
    , type (NULL)
  {
    assert (identifier_list != NULL);
    append (identifier_list);
    append (optional_type);
    append (optional_expression_list);
  }

  ConstSpecAst::~ConstSpecAst ()
  { }

  ACCEPT (ConstSpecAst)

  TypeSpecAst::TypeSpecAst (Ast* aIdentifier, Ast* aType, bool a_is_alias)
    : Ast (aIdentifier->location)
    , identifier (aIdentifier)
    , type (aType)
    , is_alias (a_is_alias)
  {
    assert (identifier);
    assert (type);
    append (identifier);
    append (type);
  }

  TypeSpecAst::~TypeSpecAst ()
  { }

  ACCEPT (TypeSpecAst)

  VarSpecAst::VarSpecAst (Ast* aIdentifierList, Ast* aType, Ast* aExpressionList)
    : Ast (aIdentifierList->location)
    , identifier_list (aIdentifierList)
    , optional_type (aType)
    , optional_expression_list (aExpressionList)
  {
    assert (identifier_list != NULL);
    append (identifier_list);
    append (optional_type);
    append (optional_expression_list);
  }

  VarSpecAst::~VarSpecAst ()
  { }

  ACCEPT (VarSpecAst)

  FuncDeclAst::FuncDeclAst (Ast* aIdentifier, Ast* aSignature, Ast* aBody)
    : Ast (aIdentifier->location)
    , identifier (aIdentifier)
    , signature (aSignature)
    , optional_body (aBody)
  {
    assert (identifier != NULL);
    assert (signature != NULL);
    append (identifier);
    append (signature);
    append (optional_body);
  }

  FuncDeclAst::~FuncDeclAst ()
  { }

  ACCEPT (FuncDeclAst)

  MethodDeclAst::MethodDeclAst (Ast* aReceiver, Ast* aIdentifier, Ast* aSignature, Ast* aBody)
    : Ast (aReceiver->location)
    , receiver (aReceiver)
    , identifier (aIdentifier)
    , signature (aSignature)
    , optional_body (aBody)
  {
    assert (receiver != NULL);
    assert (identifier != NULL);
    assert (signature != NULL);
    append (receiver);
    append (identifier);
    append (signature);
    append (optional_body);
  }

  MethodDeclAst::~MethodDeclAst ()
  { }

  ACCEPT (MethodDeclAst)

  TypeAssertionAst::TypeAssertionAst (Ast* aOperand, Ast* aType)
    : Ast (aOperand->location)
    , operand (aOperand)
    , type (aType)
  {
    assert (operand != NULL);
    assert (type != NULL);
    append (operand);
    append (type);
  }

  TypeAssertionAst::~TypeAssertionAst ()
  { }

  ACCEPT (TypeAssertionAst)

  SelectorAst::SelectorAst (Ast* aOperand, Ast* aMember)
    : Ast (aOperand->location)
    , operand (aOperand)
    , member (aMember)
  {
    assert (operand != NULL);
    assert (member != NULL);
    append (operand);
    append (member);
  }

  SelectorAst::~SelectorAst ()
  { }

  ACCEPT (SelectorAst)

  SliceAst::SliceAst (Ast* aOperand, Ast* aLow, Ast* aHigh, Ast* aCapacity)
    : Ast (aOperand->location)
    , operand (aOperand)
    , optional_low (aLow)
    , optional_high (aHigh)
    , optional_capacity (aCapacity)
  {
    assert (operand != NULL);
    append (operand);
    append (optional_low);
    append (optional_high);
    append (optional_capacity);
  }

  SliceAst::~SliceAst ()
  { }

  ACCEPT (SliceAst)

  IndexAst::IndexAst (Ast* aOperand, Ast* aIndex)
    : Ast (aOperand->location)
    , operand (aOperand)
    , index (aIndex)
  {
    assert (operand != NULL);
    assert (index != NULL);
    append (operand);
    append (index);
  }

  IndexAst::~IndexAst ()
  { }

  ACCEPT (IndexAst)

  CallAst::CallAst (Ast* aOperand, Ast* aArguments, bool aVariadic)
    : Ast (aOperand->location)
    , operand (aOperand)
    , arguments (aArguments)
    , variadic (aVariadic)
  {
    assert (operand != NULL);
    assert (arguments != NULL);
    append (operand);
    append (arguments);
  }

  CallAst::~CallAst ()
  { }

  ACCEPT (CallAst)

  BinaryAst::BinaryAst (Kind aKind, Ast* aLeft, Ast* aRight)
    : Ast (aLeft->location)
    , kind (aKind)
    , left (aLeft)
    , right (aRight)
  {
    assert (left != NULL);
    assert (right != NULL);
    append (left);
    append (right);
  }

  BinaryAst::~BinaryAst ()
  { }

  ACCEPT (BinaryAst)

  std::ostream& operator<< (std::ostream& out, BinaryAst::Kind kind)
  {
    switch (kind)
      {
      case BinaryAst::Equal:
        out << "==";
        break;
      case BinaryAst::NotEqual:
        out << "!=";
        break;
      case BinaryAst::LessThan:
        out << "<";
        break;
      case BinaryAst::LessThanEqual:
        out << "<=";
        break;
      case BinaryAst::GreaterThan:
        out << ">";
        break;
      case BinaryAst::GreaterThanEqual:
        out << ">=";
        break;
      case BinaryAst::Add:
        out << "+";
        break;
      case BinaryAst::Subtract:
        out << "-";
        break;
      case BinaryAst::BitOr:
        out << "|";
        break;
      case BinaryAst::BitXor:
        out << "^";
        break;
      case BinaryAst::Multiply:
        out << "*";
        break;
      case BinaryAst::Divide:
        out << "/";
        break;
      case BinaryAst::Modulus:
        out << "%";
        break;
      case BinaryAst::LeftShift:
        out << "<<";
        break;
      case BinaryAst::RightShift:
        out << ">>";
        break;
      case BinaryAst::BitAnd:
        out << "&";
        break;
      case BinaryAst::BitAndNot:
        out << "&^";
        break;
      }
    return out;
  }

  AssignAst::AssignAst (Kind aKind, Ast* aLeft, Ast* aRight)
    : Ast (aLeft->location)
    , kind (aKind)
    , left (aLeft)
    , right (aRight)
  {
    assert (left != NULL);
    assert (right != NULL);
    append (left);
    append (right);
  }

  AssignAst::~AssignAst ()
  { }

  ACCEPT (AssignAst)

  UnaryAst::UnaryAst (const Location& aLocation, Kind aKind, Ast* aChild)
    : Ast (aLocation)
    , kind (aKind)
    , child (aChild)
  {
    assert (child != NULL);
    append (child);
  }

  UnaryAst::~UnaryAst ()
  { }

  ACCEPT (UnaryAst)

  TypeExpressionAst::TypeExpressionAst (const Location& a_location)
    : Ast (a_location)
  { }

  TypeExpressionAst::~TypeExpressionAst ()
  { }

  ACCEPT (TypeExpressionAst)

  OrAst::OrAst (const Location& a_location)
    : ListAst (a_location)
  { }

  OrAst::~OrAst ()
  { }

  ACCEPT (OrAst)

  AndAst::AndAst (const Location& a_location)
    : ListAst (a_location)
  { }

  AndAst::~AndAst ()
  { }

  ACCEPT (AndAst)

  LabeledStatementAst::LabeledStatementAst (Ast* aLabel, Ast* aStatement)
    : Ast (aLabel->location)
    , label (aLabel)
    , statement (aStatement)
  {
    assert (label != NULL);
    assert (statement != NULL);
    append (label);
    append (statement);
  }

  LabeledStatementAst::~LabeledStatementAst ()
  { }

  ACCEPT (LabeledStatementAst)

  GotoAst::GotoAst (const Location& a_location, Ast* aLabel)
    : Ast (a_location)
    , label (aLabel)
  {
    assert (label != NULL);
    append (label);
  }

  GotoAst::~GotoAst ()
  { }

  ACCEPT (GotoAst)

  FallthroughAst::FallthroughAst (const Location& a_location)
    : Ast (a_location)
  { }

  FallthroughAst::~FallthroughAst ()
  { }

  ACCEPT (FallthroughAst)

  DeferAst::DeferAst (const Location& a_location, Ast* aExpression)
    : Ast (a_location)
    , expression (aExpression)
  {
    assert (expression != NULL);
    append (expression);
  }

  DeferAst::~DeferAst ()
  { }

  ACCEPT (DeferAst)

  EmptyStatementAst::EmptyStatementAst (const Location& a_location)
    : Ast (a_location)
  { }

  EmptyStatementAst::~EmptyStatementAst ()
  { }

  ACCEPT (EmptyStatementAst)

  ExpressionStatementAst::ExpressionStatementAst (Ast* a_expression)
    : Ast (a_expression->location)
    , expression (a_expression)
  {
    assert (expression != NULL);
    append (expression);
  }

  ExpressionStatementAst::~ExpressionStatementAst ()
  { }

  ACCEPT (ExpressionStatementAst)

  IncrementAst::IncrementAst (Ast* aExpression)
    : Ast (aExpression->location)
    , expression (aExpression)
  {
    assert (expression != NULL);
    append (expression);
  }

  IncrementAst::~IncrementAst ()
  { }

  ACCEPT (IncrementAst)

  DecrementAst::DecrementAst (Ast* aExpression)
    : Ast (aExpression->location)
    , expression (aExpression)
  {
    assert (expression != NULL);
    append (expression);
  }

  DecrementAst::~DecrementAst ()
  { }

  ACCEPT (DecrementAst)

  ReturnAst::ReturnAst (const Location& a_location, Ast* aExpression)
    : Ast (a_location)
    , optional_expression (aExpression)
  {
    append (optional_expression);
  }

  ReturnAst::~ReturnAst ()
  { }

  ACCEPT (ReturnAst)

  BreakAst::BreakAst (const Location& a_location, Ast* a_label)
    : Ast (a_location)
    , optional_label (a_label)
  {
    append (optional_label);
  }

  BreakAst::~BreakAst ()
  { }

  ACCEPT (BreakAst)

  ContinueAst::ContinueAst (const Location& a_location, Ast* aLabel)
    : Ast (a_location)
    , optional_label (aLabel)
  {
    append (optional_label);
  }

  ContinueAst::~ContinueAst ()
  { }

  ACCEPT (ContinueAst)

  IfAst::IfAst (const Location& a_location, Ast* aStatement, Ast* aExpression, Ast* aTrueBranch, Ast* aFalseBranch)
    : Ast (a_location)
    , optional_statement (aStatement)
    , expression (aExpression)
    , true_branch (aTrueBranch)
    , false_branch (aFalseBranch)
  {
    assert (expression != NULL);
    assert (true_branch != NULL);
    assert (false_branch != NULL);
    append (optional_statement);
    append (expression);
    append (true_branch);
    append (false_branch);
  }

  IfAst::~IfAst ()
  { }

  ACCEPT (IfAst)

  SwitchAst::SwitchAst (const Location& aLocation, Ast* aStatement, Ast* aExpression, Ast* aBody)
    : Ast (aLocation)
    , optional_statement (aStatement)
    , optional_expression (aExpression)
    , body (aBody)
  {
    assert (body != NULL);
    append (optional_statement);
    append (optional_expression);
    append (body);
  }

  SwitchAst::~SwitchAst ()
  { }

  ACCEPT (SwitchAst)

  CaseAst::CaseAst (const Location& aLocation, Ast* aExpressionList, Ast* aStatement)
    : Ast (aLocation)
    , optional_expression_list (aExpressionList)
    , statement (aStatement)
  {
    assert (statement != NULL);
    append (optional_expression_list);
    append (statement);
  }

  CaseAst::~CaseAst ()
  { }

  ACCEPT (CaseAst)

  ForAst::ForAst (const Location& aLocation, Ast* aClause, Ast* aBody)
    : Ast (aLocation)
    , clause (aClause)
    , body (aBody)
  {
    assert (clause != NULL);
    assert (body != NULL);
    append (clause);
    append (body);
  }

  ForAst::~ForAst ()
  { }

  ACCEPT (ForAst)

  RangeAst::RangeAst (const Location& a_location, Ast* aExpression)
    : Ast (a_location)
    , expression (aExpression)
  {
    assert (expression != NULL);
    append (expression);
  }

  RangeAst::~RangeAst ()
  { }

  ACCEPT (RangeAst)

  ForClauseAst::ForClauseAst (const Location& aLocation, Ast* aInitStmt, Ast* aCondition, Ast* aPostStmt)
    : Ast (aLocation)
    , optional_init_stmt (aInitStmt)
    , optional_condition (aCondition)
    , optional_post_stmt (aPostStmt)
  {
    append (optional_init_stmt);
    append (optional_condition);
    append (optional_post_stmt);
  }

  ForClauseAst::~ForClauseAst ()
  { }

  ACCEPT (ForClauseAst)

  SourceFileAst::SourceFileAst (const Location& aLocation, Ast* aPackage, Ast* aImports, Ast* aDecls)
    : Ast (aLocation)
    , package (aPackage)
    , imports (aImports)
    , decls (aDecls)
  {
    assert (package != NULL);
    assert (imports != NULL);
    assert (decls != NULL);
    append (package);
    append (imports);
    append (decls);
  }

  SourceFileAst::~SourceFileAst ()
  { }

  ACCEPT (SourceFileAst)

  ImportDeclListAst::ImportDeclListAst (const Location& a_location)
    : ListAst (a_location)
  { }

  ImportDeclListAst::~ImportDeclListAst ()
  { }

  ACCEPT (ImportDeclListAst);

  ImportDeclAst::ImportDeclAst (const Location& a_location)
    : ListAst (a_location)
  { }

  ImportDeclAst::~ImportDeclAst ()
  { }

  ACCEPT (ImportDeclAst);

  ImportSpecAst::ImportSpecAst (const Location& a_location, bool a_dot, Ast* a_identifier, const std::string& a_import_path)
    : Ast (a_location)
    , dot (a_dot)
    , optional_identifier (a_identifier)
    , import_path (a_import_path)
  {
    append (optional_identifier);
  }

  ImportSpecAst::~ImportSpecAst ()
  { }

  ACCEPT (ImportSpecAst);

  ErrorAst::ErrorAst (const Location& a_location)
    : Ast (a_location)
  { }

  ErrorAst::~ErrorAst ()
  { }

  ACCEPT (ErrorAst);

  IdentifierEqual::IdentifierEqual (const std::string& a_identifier)
    : identifier (a_identifier)
  { }

  bool IdentifierEqual::operator () (Ast* ast)
  {
    return identifier == to_identifier (ast)->identifier;
  }

  std::ostream& operator<< (std::ostream& out, Ast& ast)
  {
    struct Visitor : public AstVisitor {
      Visitor (std::ostream& a_out)
        : out (a_out)
        , indent (0)
      { }

      std::ostream& out;
      size_t indent;

      void visit (ListAst& ast) { print (ast, "ListAst"); }
      void visit (ArrayAst& ast) { print (ast, "ArrayAst"); }
      void visit (SliceTypeAst& ast) { print (ast, "SliceTypeAst"); }
      void visit (StructAst& ast) { print (ast, "StructAst"); }
      void visit (InterfaceAst& ast) { print (ast, "InterfaceAst"); }
      void visit (MapAst& ast) { print (ast, "MapAst"); }
      void visit (EmbeddedFieldAst& ast) { print (ast, "EmbeddedFieldAst"); }
      void visit (FieldAst& ast) { print (ast, "FieldAst"); }
      void visit (AnonymousMethodSpecAst& ast) { print (ast, "AnonymousMethodSpecAst"); }
      void visit (MethodSpecAst& ast) { print (ast, "MethodSpecAst"); }
      void visit (PointerAst& ast) { print (ast, "PointerAst"); }
      void visit (FunctionTypeAst& ast) { print (ast, "FunctionTypeAst"); }
      void visit (SignatureAst& ast) { print (ast, "SignatureAst"); }
      void visit (ParameterDeclAst& ast) { print (ast, "ParameterDeclAst"); }
      void visit (LiteralAst& ast)
      {
        print_indent ("LiteralAst");
        out << ' ' << ast.value;
        print_children (ast);
      }
      void visit (CompositeLiteralAst& ast) { print (ast, "CompositeLiteralAst"); }
      void visit (KeyedElementAst& ast) { print (ast, "KeyedElementAst"); }
      void visit (FunctionLiteralAst& ast) { print (ast, "FunctionLiteralAst"); }
      void visit (IdentifierAst& ast)
      {
        print_indent ("IdentifierAst");
        out << ' ' << ast.identifier;
        print_children (ast);
      }
      void visit (ConstSpecAst& ast) { print (ast, "ConstSpecAst"); }
      void visit (TypeSpecAst& ast)
      {
        print_indent ("TypeSpecAst");
        out << " is_alias=" << ast.is_alias;
        print_children (ast);
      }
      void visit (VarSpecAst& ast) { print (ast, "VarSpecAst"); }
      void visit (FuncDeclAst& ast) { print (ast, "FuncDeclAst"); }
      void visit (MethodDeclAst& ast) { print (ast, "MethodDeclAst"); }
      void visit (TypeAssertionAst& ast) { print (ast, "TypeAssertionAst"); }
      void visit (SelectorAst& ast) { print (ast, "SelectorAst"); }
      void visit (SliceAst& ast) { print (ast, "SliceAst"); }
      void visit (IndexAst& ast) { print (ast, "IndexAst"); }
      void visit (CallAst& ast) { print (ast, "CallAst"); }
      void visit (BinaryAst& ast)
      {
        print_indent ("BinaryAst");
        out << ' ' << ast.kind;
        print_children (ast);
      }
      void visit (AssignAst& ast) { print (ast, "AssignAst"); }
      void visit (UnaryAst& ast) { print (ast, "UnaryAst"); }
      void visit (TypeExpressionAst& ast) { print (ast, "TypeExpressionAst"); }
      void visit (OrAst& ast) { print (ast, "OrAst"); }
      void visit (AndAst& ast) { print (ast, "AndAst"); }
      void visit (LabeledStatementAst& ast) { print (ast, "LabeledStatementAst"); }
      void visit (GotoAst& ast) { print (ast, "GotoAst"); }
      void visit (FallthroughAst& ast) { print (ast, "FallthroughAst"); }
      void visit (DeferAst& ast) { print (ast, "DeferAst"); }
      void visit (EmptyStatementAst& ast) { print (ast, "EmptyStatementAst"); }
      void visit (ExpressionStatementAst& ast) { print (ast, "ExpressionStatementAst"); }
      void visit (IncrementAst& ast) { print (ast, "IncrementAst"); }
      void visit (DecrementAst& ast) { print (ast, "DecrementAst"); }
      void visit (ReturnAst& ast) { print (ast, "ReturnAst"); }
      void visit (BreakAst& ast) { print (ast, "BreakAst"); }
      void visit (ContinueAst& ast) { print (ast, "ContinueAst"); }
      void visit (IfAst& ast) { print (ast, "IfAst"); }
      void visit (SwitchAst& ast) { print (ast, "SwitchAst"); }
      void visit (CaseAst& ast) { print (ast, "CaseAst"); }
      void visit (ForAst& ast) { print (ast, "ForAst"); }
      void visit (RangeAst& ast) { print (ast, "RangeAst"); }
      void visit (ForClauseAst& ast) { print (ast, "ForClauseAst"); }
      void visit (SourceFileAst& ast) { print (ast, "SourceFileAst"); }
      void visit (ImportDeclListAst& ast) { print (ast, "ImportDeclListAst"); }
      void visit (ImportDeclAst& ast) { print (ast, "ImportDeclAst"); }
      void visit (ImportSpecAst& ast) { print (ast, "ImportSpecAst"); }
      void visit (ErrorAst& ast) { print (ast, "ErrorAst"); }

      void print (Ast& ast, const char* name)
      {
        print_indent (name);
        print_children (ast);
      }

      void print_indent (const char* name)
      {
        for (size_t idx = 0; idx != indent; ++idx)
          {
            out << ' ';
          }
        out << name;
      }

      void print_children (Ast& ast)
      {
        out << '\n';
        indent += 2;
        ast.visit_children (*this);
        indent -= 2;
      }
    };
    Visitor visitor (out);
    ast.accept (visitor);

    return out;
  }

}
