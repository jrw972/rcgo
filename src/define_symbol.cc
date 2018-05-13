// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/define_symbol.h"

#include <cassert>
#include <iostream>

#include "src/evaluate_constant_expression.h"
#include "src/process_type.h"

namespace rcgo {

bool DefineSymbol(
    Symbol* symbol, const Block& file_block, type::Factory* type_factory,
    ErrorReporter* er) {
  struct DefinePackageSymbolVisitor : public ast::DefaultNodeVisitor {
    type::Factory* type_factory;
    const Block& block;
    ErrorReporter* error_reporter;
    Symbol* symbol;

    DefinePackageSymbolVisitor(
        type::Factory* a_factory, const Block& a_block,
        ErrorReporter* a_error_reporter, Symbol* a_symbol)
        : type_factory(a_factory), block(a_block),
          error_reporter(a_error_reporter), symbol(a_symbol) {}

    void DefaultAction(ast::Node* ast) override {
      std::cout << "TODO: DefinedPackageSymbolVisitor::default_action " << ast
                << std::endl;
      abort();
    }

    void Visit(ast::TypeSpec* ast) override {
      TypeSymbol* s = symbol_cast<TypeSymbol>(symbol);
      assert(s != NULL);
      const type::Type* t =
          ProcessType(ast->type, block, type_factory, error_reporter);
      if (ast->is_alias) {
        s->type(type_factory->MakeAlias(t));
      } else {
        s->type(type_factory->MakeDefinedType(t));
      }
    }

    void Visit(ast::ConstSpec* ast) override {
      ConstantSymbol* s = symbol_cast<ConstantSymbol>(symbol);

      // TODO(jrw972):  Handle case when no expressions given.
      assert(!ast->expression_list.empty());
      if (ast->type == NULL && ast->optional_type != NULL) {
        ast->type = ProcessType(ast->optional_type, block, type_factory,
                                error_reporter);
      }

      // Find the identifier.
      ast::ListType::const_iterator pos =
          std::find_if(ast->identifier_list.begin(), ast->identifier_list.end(),
                       ast::IdentifierEqual(s->identifier));
      ast::ListType::const_iterator p2 = ast->identifier_list.begin();
      size_t idx = std::distance(p2, pos);
      if (idx >= ast->expression_list.size()) {
        // Value for the symbol indicates error.
        // Error has already been reported.
        s->value(Value());
        return;
      }

      ast::Node* e = ast->expression_list.at(idx);
      s->value(
          EvaluateConstantExpression(e, block, type_factory, error_reporter));
      if (s->value().kind != Value::kError && ast->type != NULL) {
        s->value(s->value().ConvertTo(ast->type));
        if (s->value().kind == Value::kError) {
          // TODO(jrw972):  Conversion error.
          abort();
        }
      }
    }

    void Visit(ast::FuncDecl* ast) override {
      FunctionSymbol* s = symbol_cast<FunctionSymbol>(symbol);
      assert(s != NULL);
      const type::Function* function_type =
          ProcessFunction(ast::Cast<ast::Signature>(ast->signature), block,
                          type_factory, error_reporter);
      s->type(function_type);
    }
  };

  if (symbol->IsNew()) {
    symbol->MakeInProgress();
    DefinePackageSymbolVisitor visitor(type_factory, file_block, er, symbol);
    symbol->ast->Accept(&visitor);
  } else if (symbol->IsInProgress()) {
    er->Insert(SymbolIsDefinedRecursively(symbol));
  } else {
    assert(symbol->IsDefined());
  }
  return symbol->IsDefined();
}

}  // namespace rcgo
