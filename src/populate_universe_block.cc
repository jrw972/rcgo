// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/populate_universe_block.h"

namespace rcgo {

void PopulateUniverseBlock(
    type::Factory* a_type_factory, MutableBlock* a_block) {
  type::Factory* tf = a_type_factory;
  const Location& l = Location::builtin;

  // Types:
  const type::DefinedType* string_type =
      tf->MakeDefinedType(&type::String::instance);

  // Error Interface
  type::Function* error_function = tf->MakeFunction(a_block->package());
  error_function->AppendResult("", l, string_type);

  type::Interface* error_type = tf->MakeInterface(a_block->package());
  error_type->AppendMethod("Error", l, error_function);

  type::Function* print_function = tf->MakeFunction(a_block->package());
  print_function->AppendParameter("", l, string_type, false);

  a_block->Insert(
      a_block->MakeType("bool", l, tf->MakeDefinedType(&type::Bool::instance)));
  a_block->Insert(
      a_block->MakeType("byte", l, tf->MakeAlias(&type::Uint8::instance)));
  a_block->Insert(
      a_block->MakeType(
          "complex64", l, tf->MakeDefinedType(&type::Complex64::instance)));
  a_block->Insert(
      a_block->MakeType(
          "complex128", l, tf->MakeDefinedType(&type::Complex128::instance)));
  a_block->Insert(
      a_block->MakeType("error", l, tf->MakeDefinedType(error_type)));
  a_block->Insert(
      a_block->MakeType(
          "float32", l, tf->MakeDefinedType(&type::Float32::instance)));
  a_block->Insert(
      a_block->MakeType(
          "float64", l, tf->MakeDefinedType(&type::Float64::instance)));
  a_block->Insert(
      a_block->MakeType("int", l, tf->MakeDefinedType(&type::Int::instance)));
  a_block->Insert(
      a_block->MakeType("int8", l, tf->MakeDefinedType(&type::Int8::instance)));
  a_block->Insert(
      a_block->MakeType(
          "int16", l, tf->MakeDefinedType(&type::Int16::instance)));
  a_block->Insert(
      a_block->MakeType(
          "int32", l, tf->MakeDefinedType(&type::Int32::instance)));
  a_block->Insert(
      a_block->MakeType(
          "int64", l, tf->MakeDefinedType(&type::Int64::instance)));
  a_block->Insert(
      a_block->MakeType("rune", l, tf->MakeAlias(&type::Int32::instance)));
  a_block->Insert(
      a_block->MakeType("string", l, string_type));
  a_block->Insert(
      a_block->MakeType("uint", l, tf->MakeDefinedType(&type::Uint::instance)));
  a_block->Insert(
      a_block->MakeType(
          "uint8", l, tf->MakeDefinedType(&type::Uint8::instance)));
  a_block->Insert(
      a_block->MakeType(
          "uint16", l, tf->MakeDefinedType(&type::Uint16::instance)));
  a_block->Insert(
      a_block->MakeType(
          "uint32", l, tf->MakeDefinedType(&type::Uint32::instance)));
  a_block->Insert(
      a_block->MakeType(
          "uint64", l, tf->MakeDefinedType(&type::Uint64::instance)));
  a_block->Insert(
      a_block->MakeType(
          "uintptr", l, tf->MakeDefinedType(&type::Uintptr::instance)));
  // Constants:
  a_block->Insert(
      a_block->MakeConstant("true", l, value::Value::MakeUntypedConstant(value::UntypedConstant::MakeBoolean(true))));
  a_block->Insert(
      a_block->MakeConstant("false", l, value::Value::MakeUntypedConstant(value::UntypedConstant::MakeBoolean(false))));
  a_block->Insert(a_block->MakeConstant("iota", l, nullptr));
  // Zero value:
  a_block->Insert(a_block->MakeConstant("nil", l, nullptr));
  // Functions:
  a_block->Insert(
      a_block->MakeFunction("append", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("cap", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("complex", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("copy", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("delete", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("imag", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("len", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("make", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("new", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("panic", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(a_block->MakeFunction("print", l, print_function));
  a_block->Insert(
      a_block->MakeFunction("println", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("real", l, static_cast<ast::Node*>(nullptr)));
  a_block->Insert(
      a_block->MakeFunction("recover", l, static_cast<ast::Node*>(nullptr)));
}

}  // namespace rcgo
