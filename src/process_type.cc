// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/process_type.h"

#include <cassert>

#include "src/check_types.h"
#include "src/define_symbol.h"
#include "src/process_qualified_identifier.h"

namespace rcgo {
namespace {

struct Visitor : public ast::DefaultNodeVisitor {
  type::Factory* type_factory;
  Block* block;
  ErrorReporter* error_reporter;
  type::Struct* struct_type;
  type::Function* function_type;
  type::Interface* interface_type;
  bool results;
  const type::Type* type;

  Visitor(type::Factory* a_type_factory, Block* a_block,
          ErrorReporter* a_error_reporter)
      : type_factory(a_type_factory), block(a_block),
        error_reporter(a_error_reporter), struct_type(NULL),
        function_type(NULL), interface_type(NULL), results(false), type(NULL) {}

  void DefaultAction(ast::Node* ast) override { abort(); /* NOT_COVERED */ }

  void Visit(ast::Array* ast) override {
    const type::Type* element_type =
        ProcessType(ast->element_type, block, type_factory, error_reporter);
    if (ast->optional_length == NULL) {
      error_reporter->Insert(ArrayLengthRequired(ast->location));
      type = &type::Error::instance;
      return;
    }
    CheckTypes(ast->optional_length, block, type_factory, error_reporter);
    value::ValuePtr length = ast->optional_length->converted_value();
    if (length->IsError()) {
      type = &type::Error::instance;
      return;
    }
    if (!length->RequireConstant(error_reporter)) {
      type = &type::Error::instance;
      return;
    }
    if (!length->ConvertTo(&type::Int::instance)) {
      // TODO(jrw972):  Report conversion error.
      abort();
      type = &type::Error::instance;
      return;
    }
    if (length->int_value() < 0) {
      error_reporter->Insert(
          ArrayLengthIsNegative(ast->optional_length->location));
      type = &type::Error::instance;
      return;
    }
    type = element_type->GetArray(length->int_value());
  }

  void Visit(ast::Slice* ast) override {
    const type::Type* element_type =
        ProcessType(ast->element_type, block, type_factory, error_reporter);
    type = element_type->GetSlice();
  }

  void Visit(ast::Pointer* ast) override {
    const type::Type* base_type =
        ProcessType(ast->base_type, block, type_factory, error_reporter);
    type = base_type->GetPointer();
  }

  void Visit(ast::Struct* ast) override {
    struct_type = type_factory->MakeStruct(block->package());
    VisitAll(ast->fields);
    type = struct_type;
  }

  void Visit(ast::Field* ast) override {
    // TODO(jrw972):  Handle recursive types.
    const type::Type* field_type =
        ProcessType(ast->type_literal, block, type_factory, error_reporter);
    for (ast::Node* node : ast->identifier_list) {
      ast::Identifier* id = ast::Cast<ast::Identifier>(node);
      symbol::Symbol* previous_field = struct_type->Find(id->identifier);
      symbol::Field* field = struct_type->AppendField(
          id->identifier, id->location, field_type, ast->tag, false);
      if (previous_field != NULL) {
        error_reporter->Insert(DuplicateSymbol(field, previous_field));
      }
    }
  }

  void Visit(ast::EmbeddedField* ast) override {
    // TODO(jrw972):  Handle recursive types.
    symbol::Symbol* s =
        ProcessQualifiedIdentifier(ast->type_name, *block, error_reporter);
    if (s == NULL) {
      return;
    }

    symbol::Type* ts = symbol::Cast<symbol::Type>(s);
    if (ts == NULL) {
      error_reporter->Insert(
          DoesNotReferToAType(ast->type_name->location, s->identifier));
      return;
    }

    symbol::Symbol* previous_field = struct_type->Find(s->identifier);

    const type::Type* t = ts->type();
    if (ast->is_pointer) {
      t = t->GetPointer();
    }

    symbol::Field* field = struct_type->AppendField(
        s->identifier, ast->type_name->location, t, ast->tag, true);

    if (previous_field != NULL) {
      error_reporter->Insert(DuplicateSymbol(field, previous_field));
    }
  }

  void Visit(ast::Identifier* ast) override {
    symbol::Symbol* s = ProcessQualifiedIdentifier(ast, *block, error_reporter);
    if (s == NULL) {
      type = &type::Error::instance;
      return;
    }
    symbol::Type* t = symbol::Cast<symbol::Type>(s);
    if (t == NULL) {
      error_reporter->Insert(
          DoesNotReferToAType(ast->location, ast->identifier));
      type = &type::Error::instance;
      return;
    }
    if (DefineSymbol(t, block, type_factory, error_reporter)) {
      type = t->type();
    } else {
      type = &type::Error::instance;
    }
  }

  void Visit(ast::Selector* ast) override {
    symbol::Symbol* s = ProcessQualifiedIdentifier(ast, *block, error_reporter);
    if (s == NULL) {
      type = &type::Error::instance;
      return;
    }
    symbol::Type* t = symbol::Cast<symbol::Type>(s);
    if (t == NULL) {
      error_reporter->Insert(DoesNotReferToAType(ast->location, s->identifier));
      type = &type::Error::instance;
      return;
    }
    if (DefineSymbol(t, block, type_factory, error_reporter)) {
      type = t->type();
    } else {
      type = &type::Error::instance;
    }
  }

  void Visit(ast::FunctionType* ast) override {
    function_type = type_factory->MakeFunction(block->package());
    ast->signature->Accept(this);
    type = function_type;
  }

  void Visit(ast::Signature* ast) override {
    results = false;
    VisitAll(ast->parameters);
    results = true;
    VisitAll(ast->results);
  }

  void Visit(ast::ParameterDecl* ast) override {
    assert(function_type != NULL);

    const type::Type* parameter_type =
        ProcessType(ast->type_literal, block, type_factory, error_reporter);
    if (!ast->identifier_list.empty()) {
      for (ast::Node* node : ast->identifier_list) {
        ast::Identifier* id = ast::Cast<ast::Identifier>(node);
        symbol::Symbol* previous_parameter =
            function_type->Find(id->identifier);
        symbol::Parameter* parameter;
        if (results) {
          parameter = function_type->AppendResult(
              id->identifier, id->location, parameter_type);
        } else {
          parameter = function_type->AppendParameter(
              id->identifier, id->location, parameter_type, ast->variadic);
        }
        if (previous_parameter != NULL) {
          error_reporter->Insert(
              DuplicateSymbol(parameter, previous_parameter));
        }
      }
    } else {
      if (results) {
        function_type->AppendResult(
            "", ast->location, parameter_type);
      } else {
        function_type->AppendParameter(
            "", ast->location, parameter_type, ast->variadic);
      }
    }
  }

  void Visit(ast::Interface* ast) override {
    interface_type = type_factory->MakeInterface(block->package());
    VisitAll(ast->methods);
    type = interface_type;
  }

  void Visit(ast::MethodSpec* ast) override {
    assert(interface_type != NULL);
    ast::Identifier* id = ast::Cast<ast::Identifier>(ast->identifier);

    function_type = type_factory->MakeFunction(block->package());
    ast->signature->Accept(this);

    symbol::Symbol* previous_method = interface_type->Find(id->identifier);
    symbol::InterfaceMethod* method = interface_type->AppendMethod(
        id->identifier, id->location, function_type);
    if (previous_method != NULL) {
      error_reporter->Insert(DuplicateSymbol(method, previous_method));
    }
  }

  void Visit(ast::AnonymousMethodSpec* ast) override {
    symbol::Symbol* s =
        ProcessQualifiedIdentifier(ast->type_name, *block, error_reporter);
    if (s == NULL) {
      return;
    }

    symbol::Type* ts = symbol::Cast<symbol::Type>(s);
    if (ts == NULL) {
      error_reporter->Insert(
          DoesNotReferToAType(ast->type_name->location, s->identifier));
      return;
    }

    if (!DefineSymbol(ts, block, type_factory, error_reporter)) {
      return;
    }

    const type::Type* t = ts->type();
    const type::Interface* it =
        type::Cast<type::Interface>(t->UnderlyingType());
    if (it == NULL) {
      error_reporter->Insert(
          DoesNotReferToAnInterface(ast->type_name->location, s->identifier));
      return;
    }

    for (type::Interface::const_method_iterator pos = it->MethodBegin(),
             limit = it->MethodEnd(); pos != limit; ++pos) {
      const symbol::InterfaceMethod* method = *pos;
      symbol::Symbol* previous_method =
          interface_type->Find(method->identifier);
      symbol::InterfaceMethod* new_method = interface_type->AppendMethod(
          method->identifier, ast->location, method->type);
      if (previous_method != NULL) {
        error_reporter->Insert(DuplicateSymbol(new_method, previous_method));
      }
    }
  }

  void Visit(ast::Map* ast) override {
    const type::Type* key_type = ProcessType(ast->key_type, block, type_factory,
                                       error_reporter);
    const type::Type* value_type =
        ProcessType(ast->value_type, block, type_factory, error_reporter);
    if (!Comparable(key_type)) {
      error_reporter->Insert(KeyTypeNotComparable(ast->location));
    }
    type = value_type->GetMap(key_type);
  }
};

}  // namespace

const type::Type* ProcessType(
    ast::Node* ast, Block* block, type::Factory* type_factory,
    ErrorReporter* error_reporter) {
  Visitor visitor(type_factory, block, error_reporter);
  ast->Accept(&visitor);
  return visitor.type;
}

const type::Function* ProcessFunction(
    ast::Signature* ast, Block* block, type::Factory* type_type_factory,
    ErrorReporter* error_reporter) {
  Visitor tv(type_type_factory, block, error_reporter);
  tv.function_type = type_type_factory->MakeFunction(block->package());
  ast->Accept(&tv);
  return tv.function_type;
}

}  // namespace rcgo
