// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/process_type.h"

#include <cassert>

#include "src/define_symbol.h"
#include "src/evaluate_constant_expression.h"
#include "src/process_qualified_identifier.h"

namespace rcgo {
namespace {

struct Visitor : public ast::DefaultNodeVisitor {
  type::Factory* type_factory;
  const Block& block;
  ErrorReporter* error_reporter;
  type::Struct* struct_type;
  type::Function* function_type;
  type::Interface* interface_type;
  bool results;
  const type::Type* type;

  Visitor(type::Factory* a_type_factory, const Block& a_block,
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
    Value length = EvaluateConstantExpression(ast->optional_length, block,
                                              type_factory, error_reporter);
    if (length.kind == Value::kError) {
      type = &type::Error::instance;
      return;
    }
    length = length.ConvertTo(&type::Int::instance);
    if (length.kind == Value::kError) {
      error_reporter->Insert(
          ArrayLengthNotAnInt(ast->optional_length->location));
      type = &type::Error::instance;
      return;
    }
    if (length.int_value < 0) {
      error_reporter->Insert(
          ArrayLengthIsNegative(ast->optional_length->location));
      type = &type::Error::instance;
      return;
    }
    type = element_type->GetArray(length.int_value);
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
    struct_type = type_factory->MakeStruct();
    VisitAll(ast->fields);
    type = struct_type;
  }

  void Visit(ast::Field* ast) override {
    // TODO(jrw972):  Handle recursive types.
    const type::Type* field_type =
        ProcessType(ast->type, block, type_factory, error_reporter);
    for (ast::Node* node : ast->identifier_list) {
      ast::Identifier* id = ast::Cast<ast::Identifier>(node);
      Symbol* previous_field = struct_type->Find(id->identifier);
      FieldSymbol* field =
          new FieldSymbol(id->identifier, id->location, block.GetPackage(),
                          field_type, ast->tag, false);
      struct_type->AppendField(field);
      if (previous_field != NULL) {
        error_reporter->Insert(DuplicateSymbol(field, previous_field));
      }
    }
  }

  void Visit(ast::EmbeddedField* ast) override {
    // TODO(jrw972):  Handle recursive types.
    Symbol* s =
        ProcessQualifiedIdentifier(ast->type_name, block, error_reporter);
    if (s == NULL) {
      return;
    }

    TypeSymbol* ts = symbol_cast<TypeSymbol>(s);
    if (ts == NULL) {
      error_reporter->Insert(
          DoesNotReferToAType(ast->type_name->location, s->identifier));
      return;
    }

    Symbol* previous_field = struct_type->Find(s->identifier);

    const type::Type* t = ts->type();
    if (ast->is_pointer) {
      t = t->GetPointer();
    }

    FieldSymbol* field =
        new FieldSymbol(s->identifier, ast->type_name->location,
                        block.GetPackage(), t, ast->tag, true);
    struct_type->AppendField(field);

    if (previous_field != NULL) {
      error_reporter->Insert(DuplicateSymbol(field, previous_field));
    }
  }

  void Visit(ast::Identifier* ast) override {
    Symbol* s = ProcessQualifiedIdentifier(ast, block, error_reporter);
    if (s == NULL) {
      type = &type::Error::instance;
      return;
    }
    TypeSymbol* t = symbol_cast<TypeSymbol>(s);
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
    Symbol* s = ProcessQualifiedIdentifier(ast, block, error_reporter);
    if (s == NULL) {
      type = &type::Error::instance;
      return;
    }
    TypeSymbol* t = symbol_cast<TypeSymbol>(s);
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
    function_type = type_factory->MakeFunction();
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
        ProcessType(ast->type, block, type_factory, error_reporter);
    if (!ast->identifier_list.empty()) {
      for (ast::Node* node : ast->identifier_list) {
        ast::Identifier* id = ast::Cast<ast::Identifier>(node);
        Symbol* previous_parameter = function_type->Find(id->identifier);
        ParameterSymbol* parameter =
            new ParameterSymbol(id->identifier, id->location,
                                block.GetPackage(), parameter_type,
                                ast->variadic);
        if (results) {
          function_type->AppendResult(parameter);
        } else {
          function_type->AppendParameter(parameter);
        }
        if (previous_parameter != NULL) {
          error_reporter->Insert(
              DuplicateSymbol(parameter, previous_parameter));
        }
      }
    } else {
      ParameterSymbol* parameter =
          new ParameterSymbol("", ast->location, block.GetPackage(),
                              parameter_type, ast->variadic);
      if (results) {
        function_type->AppendResult(parameter);
      } else {
        function_type->AppendParameter(parameter);
      }
    }
  }

  void Visit(ast::Interface* ast) override {
    interface_type = type_factory->MakeInterface();
    VisitAll(ast->methods);
    type = interface_type;
  }

  void Visit(ast::MethodSpec* ast) override {
    assert(interface_type != NULL);
    ast::Identifier* id = ast::Cast<ast::Identifier>(ast->identifier);

    function_type = type_factory->MakeFunction();
    ast->signature->Accept(this);

    Symbol* previous_method = interface_type->Find(id->identifier);
    InterfaceMethodSymbol* method =
        new InterfaceMethodSymbol(id->identifier, id->location,
                                  block.GetPackage(), function_type);
    interface_type->AppendMethod(method);
    if (previous_method != NULL) {
      error_reporter->Insert(DuplicateSymbol(method, previous_method));
    }
  }

  void Visit(ast::AnonymousMethodSpec* ast) override {
    Symbol* s =
        ProcessQualifiedIdentifier(ast->type_name, block, error_reporter);
    if (s == NULL) {
      return;
    }

    TypeSymbol* ts = symbol_cast<TypeSymbol>(s);
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
      const InterfaceMethodSymbol* method = *pos;
      Symbol* previous_method = interface_type->Find(method->identifier);
      InterfaceMethodSymbol* new_method =
          new InterfaceMethodSymbol(method->identifier, ast->location,
                                    block.GetPackage(), method->type);
      interface_type->AppendMethod(new_method);
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
    ast::Node* ast, const Block& block, type::Factory* type_factory,
    ErrorReporter* error_reporter) {
  Visitor visitor(type_factory, block, error_reporter);
  ast->Accept(&visitor);
  return visitor.type;
}

const type::Function* ProcessFunction(
    ast::Signature* ast, const Block& block, type::Factory* type_type_factory,
    ErrorReporter* error_reporter) {
  Visitor tv(type_type_factory, block, error_reporter);
  tv.function_type = type_type_factory->MakeFunction();
  ast->Accept(&tv);
  return tv.function_type;
}

}  // namespace rcgo
