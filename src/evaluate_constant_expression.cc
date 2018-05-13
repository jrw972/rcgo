// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/evaluate_constant_expression.h"

#include <iostream>

#include "src/ast.h"
#include "src/define_symbol.h"

namespace rcgo {

Value EvaluateConstantExpression(ast::Node* a_node, const Block& a_block,
                                 type::Factory* a_type_factory,
                                 ErrorReporter* a_error_reporter) {
  struct ConstantExpressionVisitor : public ast::DefaultNodeVisitor {
    type::Factory* type_factory;
    const Block& block;
    ErrorReporter* error_reporter;

    ConstantExpressionVisitor(
        type::Factory* a_type_factory, const Block& a_block,
        ErrorReporter* a_error_reporter)
        : type_factory(a_type_factory), block(a_block),
          error_reporter(a_error_reporter) {}

    void DefaultAction(ast::Node* ast) override {
      std::cout << "TODO: ConstantExpressionVisitor::default_action "
                << ast << std::endl;
      abort();
    }

    void Visit(ast::Literal* ast) override { /* Do nothing. */ }

    void Visit(ast::Identifier* ast) override {
      Symbol* s = block.FindGlobalSymbol(ast->identifier);
      if (s == NULL) {
        error_reporter->Insert(NotDeclared(ast->location, ast->identifier));
        ast->value = Value::MakeError();
        return;
      }
      ConstantSymbol* c = symbol_cast<ConstantSymbol>(s);
      if (c == NULL) {
        error_reporter->Insert(
            DoesNotReferToAConstant(ast->location, ast->identifier));
        ast->value = Value::MakeError();
        return;
      }

      if (DefineSymbol(s, block, type_factory, error_reporter)) {
        ast->value = c->value();
      } else {
        ast->value = Value::MakeError();
      }
    }

    void Visit(ast::Multiply* node) override {
      node->left->Accept(this);
      node->right->Accept(this);
      node->value = GetValue(node->left).Multiply(GetValue(node->right));
    }
  };

  ConstantExpressionVisitor visitor(a_type_factory, a_block, a_error_reporter);
  a_node->Accept(&visitor);
  return GetValue(a_node);
}

}  // namespace rcgo
