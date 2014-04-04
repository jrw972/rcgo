#ifndef RCGO_SRC_AST_HPP
#define RCGO_SRC_AST_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"

#include "Location.hpp"
#include "Value.hpp"

namespace rcgo {

  struct Ast {
    typedef std::vector<Ast*> ChildrenType;
    typedef ChildrenType::const_iterator const_iterator;

    Ast (const Location& a_location);
    Ast (const ChildrenType& a_children);
    virtual ~Ast ();
    virtual void accept (AstVisitor& visitor) = 0;
    virtual void visit_children (AstVisitor& visitor);
    void append (Ast* a_item);
    Ast* at (size_t idx) const;
    const_iterator begin () const;
    const_iterator end () const;
    size_t size () const;

    Location const location;
    Value value;

  private:
    ChildrenType m_children;
  };

  struct ListAst : public Ast {
    ListAst (const Location& a_location);
    ListAst (const ChildrenType& a_children);
    ListAst (Ast* a_item);
    ~ListAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct ArrayAst : public Ast {
    ArrayAst (const Location& a_location, Ast* a_optional_length, Ast* a_element_type);
    virtual ~ArrayAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const optional_length;
    Ast* const element_type;
  };

  struct SliceTypeAst : public Ast {
    SliceTypeAst (const Location& a_location, Ast* a_element_type);
    virtual ~SliceTypeAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const element_type;
  };

  struct StructAst : public ListAst {
    StructAst (const Location& a_location);
    virtual ~StructAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct InterfaceAst : public ListAst {
    InterfaceAst (const Location& a_location);
    virtual ~InterfaceAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct MapAst : public Ast {
    MapAst (const Location& a_location, Ast* a_key_type, Ast* a_value_type);
    virtual ~MapAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const key_type;
    Ast* const value_type;
  };

  struct EmbeddedFieldAst : public Ast {
    EmbeddedFieldAst (const Location& a_location, bool a_is_pointer, Ast* a_type_name, const std::string& a_tag);
    virtual ~EmbeddedFieldAst ();
    virtual void accept (AstVisitor& visitor);

    bool const is_pointer;
    Ast* const type_name;
    std::string const tag;
  };

  struct FieldAst : public Ast {
    FieldAst (Ast* a_identifier_list, Ast* a_type, const std::string& a_tag);
    virtual ~FieldAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const identifier_list;
    Ast* const type;
    std::string const tag;
  };

  struct AnonymousMethodSpecAst : public Ast {
    AnonymousMethodSpecAst (Ast* a_type_name);
    virtual ~AnonymousMethodSpecAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const type_name;
  };

  struct MethodSpecAst : public Ast {
    MethodSpecAst (Ast* a_identifier, Ast* a_signature);
    virtual ~MethodSpecAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const identifier;
    Ast* const signature;
  };

  struct PointerAst : public Ast {
    PointerAst (const Location& a_location, Ast* a_base_type);
    virtual ~PointerAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const base_type;
  };

  struct FunctionTypeAst : public Ast {
    FunctionTypeAst (const Location& a_location, Ast* a_signature);
    virtual ~FunctionTypeAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const signature;
  };

  struct SignatureAst : public Ast {
    SignatureAst (Ast* a_parameters, Ast* a_results);
    virtual ~SignatureAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const parameters;
    Ast* const results;
  };

  struct ParameterDeclAst : public Ast {
    ParameterDeclAst (Ast* a_identifier_list, bool a_variadic, Ast* a_type);
    virtual ~ParameterDeclAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const optional_identifier_list;
    bool const variadic;
    Ast* const type;
  };

  struct LiteralAst : public Ast {
    LiteralAst (const Token& a_token);
    virtual ~LiteralAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct CompositeLiteralAst : public Ast {
    CompositeLiteralAst (Ast* a_type, Ast* a_value);
    virtual ~CompositeLiteralAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const type;
    Ast* const value;
  };

  struct KeyedElementAst : public Ast {
    KeyedElementAst (Ast* a_key, Ast* a_element);
    virtual ~KeyedElementAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const optional_key;
    Ast* const element;
  };

  struct FunctionLiteralAst : public Ast {
    FunctionLiteralAst (const Location& a_location, Ast* a_type, Ast* a_body);
    virtual ~FunctionLiteralAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const type;
    Ast* const body;
  };

  struct IdentifierAst : public Ast {
    IdentifierAst (const Token& a_token);
    virtual ~IdentifierAst ();
    virtual void accept (AstVisitor& visitor);

    std::string const identifier;
  };

  struct ConstSpecAst : public Ast {
    ConstSpecAst (const Location& a_location, Ast* a_identifier_list, Ast* a_type, Ast* a_expression_list);
    virtual ~ConstSpecAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const identifier_list;
    Ast* const optional_type;
    Ast* const optional_expression_list;

    const Type* type;
  };

  struct TypeSpecAst : public Ast {
    TypeSpecAst (Ast* a_identifier, Ast* a_type, bool a_is_alias);
    virtual ~TypeSpecAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const identifier;
    Ast* const type;
    bool const is_alias;
  };

  struct VarSpecAst : public Ast {
    VarSpecAst (Ast* a_identifier_list, Ast* a_type, Ast* a_expression_list);
    virtual ~VarSpecAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const identifier_list;
    Ast* const optional_type;
    Ast* const optional_expression_list;
  };

  struct FuncDeclAst : public Ast {
    FuncDeclAst (Ast* a_identifier, Ast* a_signature, Ast* a_body);
    virtual ~FuncDeclAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const identifier;
    Ast* const signature;
    Ast* const optional_body;
  };

  struct MethodDeclAst : public Ast {
    MethodDeclAst (Ast* a_receiver, Ast* a_identifier, Ast* a_signature, Ast* a_body);
    virtual ~MethodDeclAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const receiver;
    Ast* const identifier;
    Ast* const signature;
    Ast* const optional_body;
  };

  struct TypeAssertionAst : public Ast {
    TypeAssertionAst (Ast* a_operand, Ast* a_type);
    virtual ~TypeAssertionAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const operand;
    Ast* const type;
  };

  struct SelectorAst : public Ast {
    SelectorAst (Ast* a_operand, Ast* a_member);
    virtual ~SelectorAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const operand;
    Ast* const member;
  };

  struct SliceAst : public Ast {
    SliceAst (Ast* a_operand, Ast* a_low, Ast* a_high, Ast* a_capacity);
    virtual ~SliceAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const operand;
    Ast* const optional_low;
    Ast* const optional_high;
    Ast* const optional_capacity;
  };

  struct IndexAst : public Ast {
    IndexAst (Ast* a_operand, Ast* a_index);
    virtual ~IndexAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const operand;
    Ast* const index;
  };

  struct CallAst : public Ast {
    CallAst (Ast* a_operand, Ast* a_arguments, bool a_variadic);
    virtual ~CallAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const operand;
    Ast* const arguments;
    bool const variadic;
  };

  struct BinaryAst : public Ast {
    enum Kind {
      Equal,
      NotEqual,
      LessThan,
      LessThanEqual,
      GreaterThan,
      GreaterThanEqual,

      Add,
      Subtract,
      BitOr,
      BitXor,

      Multiply,
      Divide,
      Modulus,
      LeftShift,
      RightShift,
      BitAnd,
      BitAndNot,
    };

    BinaryAst (Kind a_kind, Ast* a_left, Ast* a_right);
    virtual ~BinaryAst ();
    virtual void accept (AstVisitor& visitor);

    Kind const kind;
    Ast* const left;
    Ast* const right;
  };

  std::ostream& operator<< (std::ostream& out, BinaryAst::Kind kind);

  struct AssignAst : public Ast {
    enum Kind {
      Assign,
      ShortAssign,

      Add,
      Subtract,
      BitOr,
      BitXor,

      Multiply,
      Divide,
      Modulus,
      LeftShift,
      RightShift,
      BitAnd,
      BitAndNot,
    };

    AssignAst (Kind a_kind, Ast* a_left, Ast* a_right);
    virtual ~AssignAst ();
    virtual void accept (AstVisitor& visitor);

    Kind const kind;
    Ast* const left;
    Ast* const right;
  };

  struct UnaryAst : public Ast {
    enum Kind {
      Posate,
      Negate,
      LogicalNot,
      BitNot,
      Indirection,
      Address,
    };

    UnaryAst (const Location& a_location, Kind a_kind, Ast* a_child);
    virtual ~UnaryAst ();
    virtual void accept (AstVisitor& visitor);

    Kind const kind;
    Ast* const child;
  };

  struct TypeExpressionAst : public Ast {
    TypeExpressionAst (const Location& a_location);
    virtual ~TypeExpressionAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct OrAst : public ListAst {
    OrAst (const Location& a_location);
    virtual ~OrAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct AndAst : public ListAst {
    AndAst (const Location& a_location);
    virtual ~AndAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct LabeledStatementAst : public Ast {
    LabeledStatementAst (Ast* a_label, Ast* a_statement);
    virtual ~LabeledStatementAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const label;
    Ast* const statement;
  };

  struct GotoAst : public Ast {
    GotoAst (const Location& a_location, Ast* a_label);
    virtual ~GotoAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const label;
  };

  struct FallthroughAst : public Ast {
    FallthroughAst (const Location& a_location);
    virtual ~FallthroughAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct DeferAst : public Ast {
    DeferAst (const Location& a_location, Ast* a_expression);
    virtual ~DeferAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const expression;
  };

  struct EmptyStatementAst : public Ast {
    EmptyStatementAst (const Location& a_location);
    virtual ~EmptyStatementAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct ExpressionStatementAst : public Ast {
    ExpressionStatementAst (Ast* a_expression);
    virtual ~ExpressionStatementAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const expression;
  };

  struct IncrementAst : public Ast {
    IncrementAst (Ast* a_expression);
    virtual ~IncrementAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const expression;
  };

  struct DecrementAst : public Ast {
    DecrementAst (Ast* a_expression);
    virtual ~DecrementAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const expression;
  };

  struct ReturnAst : public Ast {
    ReturnAst (const Location& a_location, Ast* a_expression);
    virtual ~ReturnAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const optional_expression;
  };

  struct BreakAst : public Ast {
    BreakAst (const Location& a_location, Ast* a_label);
    virtual ~BreakAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const optional_label;
  };

  struct ContinueAst : public Ast {
    ContinueAst (const Location& a_location, Ast* a_label);
    virtual ~ContinueAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const optional_label;
  };

  struct IfAst : public Ast {
    IfAst (const Location& a_location, Ast* a_statement, Ast* a_expression, Ast* a_true_branch, Ast* a_false_branch);
    virtual ~IfAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const optional_statement;
    Ast* const expression;
    Ast* const true_branch;
    Ast* const false_branch;
  };

  struct SwitchAst : public Ast {
    SwitchAst (const Location& a_location, Ast* a_statement, Ast* a_expression, Ast* a_body);
    virtual ~SwitchAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const optional_statement;
    Ast* const optional_expression;
    Ast* const body;
  };

  struct CaseAst : public Ast {
    CaseAst (const Location& a_location, Ast* a_expression_list, Ast* a_statement);
    virtual ~CaseAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const optional_expression_list;
    Ast* const statement;
  };

  struct ForAst : public Ast {
    ForAst (const Location& a_location, Ast* a_clause, Ast* a_body);
    virtual ~ForAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const clause;
    Ast* const body;
  };

  struct RangeAst : public Ast {
    RangeAst (const Location& a_location, Ast* a_expression);
    virtual ~RangeAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const expression;
  };

  struct ForClauseAst : public Ast {
    ForClauseAst (const Location& a_location, Ast* a_init_stmt, Ast* a_condition, Ast* a_post_stmt);
    virtual ~ForClauseAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const optional_init_stmt;
    Ast* const optional_condition;
    Ast* const optional_post_stmt;
  };

  struct SourceFileAst : public Ast {
    SourceFileAst (const Location& a_location, Ast* a_package, Ast* a_imports, Ast* a_decls);
    virtual ~SourceFileAst ();
    virtual void accept (AstVisitor& visitor);

    Ast* const package;
    Ast* const imports;
    Ast* const decls;
  };

  struct ImportDeclListAst : public ListAst {
    ImportDeclListAst (const Location& a_location);
    virtual ~ImportDeclListAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct ImportDeclAst : public ListAst {
    ImportDeclAst (const Location& a_location);
    virtual ~ImportDeclAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct ImportSpecAst : public Ast {
    ImportSpecAst (const Location& a_location, bool a_dot, Ast* a_identifier, const std::string& a_import_path);
    virtual ~ImportSpecAst ();
    virtual void accept (AstVisitor& visitor);

    bool const dot;
    Ast* const optional_identifier;
    std::string const import_path;
  };

  struct ErrorAst : public Ast {
    ErrorAst (const Location& a_location);
    virtual ~ErrorAst ();
    virtual void accept (AstVisitor& visitor);
  };

  struct IdentifierEqual { // NOT_COVERED
    IdentifierEqual (const std::string& a_identifier);

    bool operator () (Ast* ast);

    std::string const identifier;
  };

  std::ostream& operator<< (std::ostream& out, Ast& ast);
}

#endif // RCGO_SRC_AST_HPP
