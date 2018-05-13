// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/enter_predeclared_symbols.h"

namespace rcgo {

void EnterPredeclaredSymbols(type::Factory* a_type_factory,
                             SymbolTable* a_symbol_table) {
  type::Factory* tf = a_type_factory;
  SymbolTable* st = a_symbol_table;
  const Location& l = Location::builtin;

  // Types:

  const type::DefinedType* string_type =
      tf->MakeDefinedType(&type::String::instance);

  // Error Interface
  ParameterSymbol * ps = new ParameterSymbol("", l, NULL, string_type, false);
  type::Function* ft = tf->MakeFunction()->AppendResult(ps);
  InterfaceMethodSymbol* ims = new InterfaceMethodSymbol("Error", l, NULL, ft);
  const type::Type* error_type = tf->MakeInterface()->AppendMethod(ims);

  st->Insert(new TypeSymbol("bool", l, NULL,
                            tf->MakeDefinedType(&type::Bool::instance)));
  st->Insert(new TypeSymbol("byte", l, NULL,
                            tf->MakeAlias(&type::Uint8::instance)));
  st->Insert(new TypeSymbol("complex64", l, NULL,
                            tf->MakeDefinedType(&type::Complex64::instance)));
  st->Insert(new TypeSymbol("complex128", l, NULL,
                            tf->MakeDefinedType(&type::Complex128::instance)));
  st->Insert(new TypeSymbol("error", l, NULL,
                            tf->MakeDefinedType(error_type)));
  st->Insert(new TypeSymbol("float32", l, NULL,
                            tf->MakeDefinedType(&type::Float32::instance)));
  st->Insert(new TypeSymbol("float64", l, NULL,
                            tf->MakeDefinedType(&type::Float64::instance)));
  st->Insert(new TypeSymbol("int", l, NULL,
                            tf->MakeDefinedType(&type::Int::instance)));
  st->Insert(new TypeSymbol("int8", l, NULL,
                            tf->MakeDefinedType(&type::Int8::instance)));
  st->Insert(new TypeSymbol("int16", l, NULL,
                            tf->MakeDefinedType(&type::Int16::instance)));
  st->Insert(new TypeSymbol("int32", l, NULL,
                            tf->MakeDefinedType(&type::Int32::instance)));
  st->Insert(new TypeSymbol("int64", l, NULL,
                            tf->MakeDefinedType(&type::Int64::instance)));
  st->Insert(new TypeSymbol("rune", l, NULL,
                            tf->MakeAlias(&type::Int32::instance)));
  st->Insert(new TypeSymbol("string", l, NULL, string_type));
  st->Insert(new TypeSymbol("uint", l, NULL,
                            tf->MakeDefinedType(&type::Uint::instance)));
  st->Insert(new TypeSymbol("uint8", l, NULL,
                            tf->MakeDefinedType(&type::Uint8::instance)));
  st->Insert(new TypeSymbol("uint16", l, NULL,
                            tf->MakeDefinedType(&type::Uint16::instance)));
  st->Insert(new TypeSymbol("uint32", l, NULL,
                            tf->MakeDefinedType(&type::Uint32::instance)));
  st->Insert(new TypeSymbol("uint64", l, NULL,
                            tf->MakeDefinedType(&type::Uint64::instance)));
  st->Insert(new TypeSymbol("uintptr", l, NULL,
                            tf->MakeDefinedType(&type::Uintptr::instance)));
  // Constants:
  st->Insert(new ConstantSymbol("true", l, NULL,
                                          Value::MakeBoolean(true)));
  st->Insert(new ConstantSymbol("false", l, NULL,
                                          Value::MakeBoolean(false)));
  st->Insert(new ConstantSymbol("iota", l, NULL, NULL));
  // Zero value:
  st->Insert(new ConstantSymbol("nil", l, NULL, NULL));
  // Functions:
  st->Insert(new FunctionSymbol("append", l, NULL, NULL));
  st->Insert(new FunctionSymbol("cap", l, NULL, NULL));
  st->Insert(new FunctionSymbol("complex", l, NULL, NULL));
  st->Insert(new FunctionSymbol("copy", l, NULL, NULL));
  st->Insert(new FunctionSymbol("delete", l, NULL, NULL));
  st->Insert(new FunctionSymbol("imag", l, NULL, NULL));
  st->Insert(new FunctionSymbol("len", l, NULL, NULL));
  st->Insert(new FunctionSymbol("make", l, NULL, NULL));
  st->Insert(new FunctionSymbol("new", l, NULL, NULL));
  st->Insert(new FunctionSymbol("panic", l, NULL, NULL));
  st->Insert(new FunctionSymbol("print", l, NULL, NULL));
  st->Insert(new FunctionSymbol("println", l, NULL, NULL));
  st->Insert(new FunctionSymbol("real", l, NULL, NULL));
  st->Insert(new FunctionSymbol("recover", l, NULL, NULL));
}

}  // namespace rcgo
