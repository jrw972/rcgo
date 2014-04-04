#ifndef RCGO_SRC_AST_VISITOR_HPP
#define RCGO_SRC_AST_VISITOR_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"
#include "Ast.hpp"

namespace rcgo {

  struct AstVisitor {
    AstVisitor ();
    virtual ~AstVisitor ();
    virtual void visit (ListAst& ast) = 0;
    virtual void visit (ArrayAst& ast) = 0;
    virtual void visit (SliceTypeAst& ast) = 0;
    virtual void visit (StructAst& ast) = 0;
    virtual void visit (InterfaceAst& ast) = 0;
    virtual void visit (MapAst& ast) = 0;
    virtual void visit (EmbeddedFieldAst& ast) = 0;
    virtual void visit (FieldAst& ast) = 0;
    virtual void visit (AnonymousMethodSpecAst& ast) = 0;
    virtual void visit (MethodSpecAst& ast) = 0;
    virtual void visit (PointerAst& ast) = 0;
    virtual void visit (FunctionTypeAst& ast) = 0;
    virtual void visit (SignatureAst& ast) = 0;
    virtual void visit (ParameterDeclAst& ast) = 0;
    virtual void visit (LiteralAst& ast) = 0;
    virtual void visit (CompositeLiteralAst& ast) = 0;
    virtual void visit (KeyedElementAst& ast) = 0;
    virtual void visit (FunctionLiteralAst& ast) = 0;
    virtual void visit (IdentifierAst& ast) = 0;
    virtual void visit (ConstSpecAst& ast) = 0;
    virtual void visit (TypeSpecAst& ast) = 0;
    virtual void visit (VarSpecAst& ast) = 0;
    virtual void visit (FuncDeclAst& ast) = 0;
    virtual void visit (MethodDeclAst& ast) = 0;
    virtual void visit (TypeAssertionAst& ast) = 0;
    virtual void visit (SelectorAst& ast) = 0;
    virtual void visit (SliceAst& ast) = 0;
    virtual void visit (IndexAst& ast) = 0;
    virtual void visit (CallAst& ast) = 0;
    virtual void visit (BinaryAst& ast) = 0;
    virtual void visit (AssignAst& ast) = 0;
    virtual void visit (UnaryAst& ast) = 0;
    virtual void visit (TypeExpressionAst& ast) = 0;
    virtual void visit (OrAst& ast) = 0;
    virtual void visit (AndAst& ast) = 0;
    virtual void visit (LabeledStatementAst& ast) = 0;
    virtual void visit (GotoAst& ast) = 0;
    virtual void visit (FallthroughAst& ast) = 0;
    virtual void visit (DeferAst& ast) = 0;
    virtual void visit (EmptyStatementAst& ast) = 0;
    virtual void visit (ExpressionStatementAst& ast) = 0;
    virtual void visit (IncrementAst& ast) = 0;
    virtual void visit (DecrementAst& ast) = 0;
    virtual void visit (ReturnAst& ast) = 0;
    virtual void visit (BreakAst& ast) = 0;
    virtual void visit (ContinueAst& ast) = 0;
    virtual void visit (IfAst& ast) = 0;
    virtual void visit (SwitchAst& ast) = 0;
    virtual void visit (CaseAst& ast) = 0;
    virtual void visit (ForAst& ast) = 0;
    virtual void visit (RangeAst& ast) = 0;
    virtual void visit (ForClauseAst& ast) = 0;
    virtual void visit (SourceFileAst& ast) = 0;
    virtual void visit (ImportDeclListAst& ast) = 0;
    virtual void visit (ImportDeclAst& ast) = 0;
    virtual void visit (ImportSpecAst& ast) = 0;
    virtual void visit (ErrorAst& ast) = 0;
  };

  struct DefaultAstVisitor : public AstVisitor {
    DefaultAstVisitor ();
    virtual ~DefaultAstVisitor ();
    virtual void visit (ListAst& ast);
    virtual void visit (ArrayAst& ast);
    virtual void visit (SliceTypeAst& ast);
    virtual void visit (StructAst& ast);
    virtual void visit (InterfaceAst& ast);
    virtual void visit (MapAst& ast);
    virtual void visit (EmbeddedFieldAst& ast);
    virtual void visit (FieldAst& ast);
    virtual void visit (AnonymousMethodSpecAst& ast);
    virtual void visit (MethodSpecAst& ast);
    virtual void visit (PointerAst& ast);
    virtual void visit (FunctionTypeAst& ast);
    virtual void visit (SignatureAst& ast);
    virtual void visit (ParameterDeclAst& ast);
    virtual void visit (LiteralAst& ast);
    virtual void visit (CompositeLiteralAst& ast);
    virtual void visit (KeyedElementAst& ast);
    virtual void visit (FunctionLiteralAst& ast);
    virtual void visit (IdentifierAst& ast);
    virtual void visit (ConstSpecAst& ast);
    virtual void visit (TypeSpecAst& ast);
    virtual void visit (VarSpecAst& ast);
    virtual void visit (FuncDeclAst& ast);
    virtual void visit (MethodDeclAst& ast);
    virtual void visit (TypeAssertionAst& ast);
    virtual void visit (SelectorAst& ast);
    virtual void visit (SliceAst& ast);
    virtual void visit (IndexAst& ast);
    virtual void visit (CallAst& ast);
    virtual void visit (BinaryAst& ast);
    virtual void visit (AssignAst& ast);
    virtual void visit (UnaryAst& ast);
    virtual void visit (TypeExpressionAst& ast);
    virtual void visit (OrAst& ast);
    virtual void visit (AndAst& ast);
    virtual void visit (LabeledStatementAst& ast);
    virtual void visit (GotoAst& ast);
    virtual void visit (FallthroughAst& ast);
    virtual void visit (DeferAst& ast);
    virtual void visit (EmptyStatementAst& ast);
    virtual void visit (ExpressionStatementAst& ast);
    virtual void visit (IncrementAst& ast);
    virtual void visit (DecrementAst& ast);
    virtual void visit (ReturnAst& ast);
    virtual void visit (BreakAst& ast);
    virtual void visit (ContinueAst& ast);
    virtual void visit (IfAst& ast);
    virtual void visit (SwitchAst& ast);
    virtual void visit (CaseAst& ast);
    virtual void visit (ForAst& ast);
    virtual void visit (RangeAst& ast);
    virtual void visit (ForClauseAst& ast);
    virtual void visit (SourceFileAst& ast);
    virtual void visit (ImportDeclListAst& ast);
    virtual void visit (ImportDeclAst& ast);
    virtual void visit (ImportSpecAst& ast);
    virtual void visit (ErrorAst& ast);
    virtual void default_action (Ast& ast);
  };

  template<typename T>
  T* ast_cast (Ast* ast)
  {
    if (ast == NULL) { return NULL; }

    struct visitor : public DefaultAstVisitor {
      T* retval;
      visitor () : retval (NULL) { }
      void visit (T& ast) { retval = &ast; }
    };

    visitor v;
    ast->accept (v);
    assert (v.retval != NULL);
    return v.retval;
  }

  IdentifierAst* to_identifier (Ast* ast);

}

#endif // RCGO_SRC_AST_VISITOR_HPP
