// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/define_symbol.h"

#include <cassert>
#include <iostream>

#include "src/check_types.h"
#include "src/process_type.h"

namespace rcgo {

bool DefineSymbol(
    symbol::Symbol* symbol, Block* file_block, type::Factory* type_factory,
    ErrorReporter* er) {
  struct DefinePackageSymbolVisitor : public ast::DefaultNodeVisitor {
    type::Factory* type_factory;
    Block* block;
    ErrorReporter* error_reporter;
    symbol::Symbol* symbol;

    DefinePackageSymbolVisitor(
        type::Factory* a_factory, Block* a_block,
        ErrorReporter* a_error_reporter, symbol::Symbol* a_symbol)
        : type_factory(a_factory), block(a_block),
          error_reporter(a_error_reporter), symbol(a_symbol) {}

    void DefaultAction(ast::Node* ast) override {
      std::cout << "TODO: DefinedPackageSymbolVisitor::default_action " << ast
                << std::endl;
      abort();
    }

    void Visit(ast::TypeSpec* ast) override {
      symbol::Type* s = symbol::Cast<symbol::Type>(symbol);
      assert(s != NULL);
      const type::Type* t =
          ProcessType(ast->type_literal, block, type_factory, error_reporter);
      if (ast->is_alias) {
        s->type(type_factory->MakeAlias(t));
      } else {
        s->type(type_factory->MakeDefinedType(t));
      }
    }

    void Visit(ast::ConstSpec* ast) override {
      symbol::Constant* s = symbol::Cast<symbol::Constant>(symbol);

      // TODO(jrw972):  Handle case when no expressions given.
      assert(!ast->expression_list.empty());
      const type::Type* type = ast->type();
      if (type == nullptr && ast->optional_type_literal != nullptr) {
        type = ProcessType(ast->optional_type_literal, block, type_factory,
                           error_reporter);
        ast->type(type);
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
        s->value(value::Value::MakeError());
        return;
      }

      ast::Node* e = ast->expression_list.at(idx);
      CheckTypes(e, block, type_factory, error_reporter);
      value::ValuePtr value = e->converted_value();
      if (value->IsError()) {
        s->value(value);
        return;
      }
      if (!value->RequireConstant(error_reporter)) {
        s->value(value);
        return;
      }
      if (type != nullptr && !value->ConvertTo(type)) {
        // TODO(jrw972):  Report conversion error.
        abort();
        s->value(value);
        return;
      }
      s->value(value);
    }

    void Visit(ast::FuncDecl* ast) override {
      symbol::Function* s = symbol::Cast<symbol::Function>(symbol);
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
