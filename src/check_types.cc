// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/check_types.h"

#include <iostream>

#include "src/ast.h"
#include "src/define_symbol.h"
#include "src/error.h"
#include "src/type.h"

namespace rcgo {

void CheckTypes(ast::Node* node, Block* block, type::Factory* type_factory,
                ErrorReporter* error_reporter) {
  struct Visitor : public ast::DefaultNodeVisitor {
    Block* const block;
    type::Factory* type_factory;
    ErrorReporter* error_reporter;

    Visitor(Block* a_block, type::Factory* a_type_factory,
            ErrorReporter* a_error_reporter)
        : block(a_block), type_factory(a_type_factory),
          error_reporter(a_error_reporter) {}

    void DefaultAction(ast::Node* ast) override { std::cout << *ast << std::endl; abort(); /* NOT_COVERED */ }

    void Visit(ast::SourceFile* ast) override { VisitAll(ast->decls); }

    void Visit(ast::TypeDecl* ast) override {
      // Do nothing.
    }

    void Visit(ast::FuncDecl* ast) override {
      if (ast->optional_body == nullptr) {
        return;
      }

      // Create a block
      symbol::Function* function = ast->function;
      assert(function != nullptr);
      Block blk(function->type(), block);
      CheckTypes(ast->optional_body, &blk, type_factory, error_reporter);
    }

    void Visit(ast::Block* ast) override {
      symbol::Table symbol_table(block->package());
      MutableBlock blk(&symbol_table, block);
      for (auto statement : ast->statements) {
        CheckTypes(statement, &blk, type_factory, error_reporter);
      }
    }

    void Visit(ast::ExpressionStatement* ast) override {
      VisitAll(ast->expression);
    }

    void Visit(ast::Call* ast) override {
      ast->operand->Accept(this);
      VisitAll(ast->arguments);
      value::ValueList arguments;
      for (ast::Node* node : ast->arguments) {
        arguments.push_back(node->out_value());
      }
      value::Value::Call(ast->operand->out_value(), arguments, error_reporter);
    }

    void Visit(ast::Identifier* ast) override {
      symbol::Symbol* symbol = block->FindGlobalSymbol(ast->identifier);
      if (symbol == nullptr) {
        error_reporter->Insert(NotDeclared(ast->location, ast->identifier));
        ast->in_value(value::Value::MakeError());
        return;
      }

      if (DefineSymbol(symbol, block, type_factory, error_reporter)) {
        ast->in_value(symbol->GetValue());
      } else {
        ast->in_value(value::Value::MakeError());
      }
    }

    void Visit(ast::Literal* ast) override { /* Do nothing. */ }

    // void Visit(ast::Multiply* node) override {
    //   node->left->Accept(this);
    //   node->right->Accept(this);
    //   node->in_value(
    //       Value::Multiply(node->left->out_value(), node->right->out_value()));
    //   // TODO(jrw972): Report errors.
    // }
  };

  Visitor visitor(block, type_factory, error_reporter);
  node->Accept(&visitor);
}

}  // namespace rcgo
