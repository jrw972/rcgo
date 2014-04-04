/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Value.hpp"
#include "AstVisitor.hpp"
#include "Symbol.hpp"
#include "SymbolVisitor.hpp"
#include "SymbolTable.hpp"
#include "Error.hpp"
#include "functions.hpp"

#include <iostream>

namespace rcgo {

  Value
  evaluate_constant_expression (Ast* ast,
                                const Block& block,
                                TypeFactory& factory,
                                ErrorReporter& error_reporter)
  {
    struct ConstantExpressionVisitor : public DefaultAstVisitor {
      TypeFactory& factory;
      const Block& block;
      ErrorReporter& error_reporter;

      ConstantExpressionVisitor (TypeFactory& a_factory, const Block& a_block, ErrorReporter& a_error_reporter)
        : factory (a_factory)
        , block (a_block)
        , error_reporter (a_error_reporter)
      { }

      void default_action (Ast& ast)
      {
        std::cout << "TODO: ConstantExpressionVisitor::default_action " << ast << std::endl;
        abort ();
      }

      void visit (LiteralAst& ast)
      {
        // Do nothing.
      }

      void visit (IdentifierAst& ast)
      {
        Symbol* s = block.find_global_symbol (ast.identifier);
        if (s == NULL)
          {
            error_reporter << NotDeclared (ast.location, ast.identifier);
            ast.value = Value::make_error ();
            return;
          }
        ConstantSymbol* c = symbol_cast<ConstantSymbol> (s);
        if (c == NULL)
          {
            error_reporter << DoesNotReferToAConstant (ast.location, ast.identifier);
            ast.value = Value::make_error ();
            return;
          }

        if (define_symbol (s, block, factory, error_reporter))
          {
            ast.value = c->value ();
          }
        else
          {
            ast.value = Value::make_error ();
          }
      }

      void visit (BinaryAst& ast)
      {
        ast.visit_children (*this);
        switch (ast.kind)
          {
          case BinaryAst::Equal:
            abort ();
          case BinaryAst::NotEqual:
            abort ();
          case BinaryAst::LessThan:
            abort ();
          case BinaryAst::LessThanEqual:
            abort ();
          case BinaryAst::GreaterThan:
            abort ();
          case BinaryAst::GreaterThanEqual:
            abort ();
          case BinaryAst::Add:
            abort ();
          case BinaryAst::Subtract:
            abort ();
          case BinaryAst::BitOr:
            abort ();
          case BinaryAst::BitXor:
            abort ();
          case BinaryAst::Multiply:
            ast.value = ast.left->value.multiply (ast.right->value);
            break;
          case BinaryAst::Divide:
            abort ();
          case BinaryAst::Modulus:
            abort ();
          case BinaryAst::LeftShift:
            abort ();
          case BinaryAst::RightShift:
            abort ();
          case BinaryAst::BitAnd:
            abort ();
          case BinaryAst::BitAndNot:
            abort ();
          }
      }
    };

    ConstantExpressionVisitor visitor (factory, block, error_reporter);
    ast->accept (visitor);
    return ast->value;
  }

}
