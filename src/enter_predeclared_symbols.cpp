/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Symbol.hpp"
#include "Type.hpp"

namespace rcgo {

  void enter_predeclared_symbols (TypeFactory& a_type_factory,
                                  SymbolTable& universe_st)
  {
    // Types:
    const DefinedType* string_type = a_type_factory.make_defined_type (&StringType::instance);
    const Type* error_type = a_type_factory.make_interface_type ()->append_method (new InterfaceMethodSymbol ("Error", Location::builtin, NULL, a_type_factory.make_function_type ()->append_result (new ParameterSymbol ("", Location::builtin, NULL, string_type, false))));
    universe_st.insert (new TypeSymbol ("bool", Location::builtin, NULL, a_type_factory.make_defined_type (&BoolType::instance)));
    universe_st.insert (new TypeSymbol ("byte", Location::builtin, NULL, a_type_factory.make_alias_type (&Uint8Type::instance)));
    universe_st.insert (new TypeSymbol ("complex64", Location::builtin, NULL, a_type_factory.make_defined_type (&Complex64Type::instance)));
    universe_st.insert (new TypeSymbol ("complex128", Location::builtin, NULL, a_type_factory.make_defined_type (&Complex128Type::instance)));
    universe_st.insert (new TypeSymbol ("error", Location::builtin, NULL, a_type_factory.make_defined_type (error_type)));
    universe_st.insert (new TypeSymbol ("float32", Location::builtin, NULL, a_type_factory.make_defined_type (&Float32Type::instance)));
    universe_st.insert (new TypeSymbol ("float64", Location::builtin, NULL, a_type_factory.make_defined_type (&Float64Type::instance)));
    universe_st.insert (new TypeSymbol ("int", Location::builtin, NULL, a_type_factory.make_defined_type (&IntType::instance)));
    universe_st.insert (new TypeSymbol ("int8", Location::builtin, NULL, a_type_factory.make_defined_type (&Int8Type::instance)));
    universe_st.insert (new TypeSymbol ("int16", Location::builtin, NULL, a_type_factory.make_defined_type (&Int16Type::instance)));
    universe_st.insert (new TypeSymbol ("int32", Location::builtin, NULL, a_type_factory.make_defined_type (&Int32Type::instance)));
    universe_st.insert (new TypeSymbol ("int64", Location::builtin, NULL, a_type_factory.make_defined_type (&Int64Type::instance)));
    universe_st.insert (new TypeSymbol ("rune", Location::builtin, NULL, a_type_factory.make_alias_type (&Int32Type::instance)));
    universe_st.insert (new TypeSymbol ("string", Location::builtin, NULL, string_type));
    universe_st.insert (new TypeSymbol ("uint", Location::builtin, NULL, a_type_factory.make_defined_type (&UintType::instance)));
    universe_st.insert (new TypeSymbol ("uint8", Location::builtin, NULL, a_type_factory.make_defined_type (&Uint8Type::instance)));
    universe_st.insert (new TypeSymbol ("uint16", Location::builtin, NULL, a_type_factory.make_defined_type (&Uint16Type::instance)));
    universe_st.insert (new TypeSymbol ("uint32", Location::builtin, NULL, a_type_factory.make_defined_type (&Uint32Type::instance)));
    universe_st.insert (new TypeSymbol ("uint64", Location::builtin, NULL, a_type_factory.make_defined_type (&Uint64Type::instance)));
    universe_st.insert (new TypeSymbol ("uintptr", Location::builtin, NULL, a_type_factory.make_defined_type (&UintptrType::instance)));
    // Constants:
    universe_st.insert (new ConstantSymbol ("true", Location::builtin, NULL, Value::make_boolean (true)));
    universe_st.insert (new ConstantSymbol ("false", Location::builtin, NULL, Value::make_boolean (false)));
    universe_st.insert (new ConstantSymbol ("iota", Location::builtin, NULL, NULL));
    // Zero value:
    universe_st.insert (new ConstantSymbol ("nil", Location::builtin, NULL, NULL));
    // Functions:
    universe_st.insert (new FunctionSymbol ("append", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("cap", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("close", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("complex", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("copy", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("delete", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("imag", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("len", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("make", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("new", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("panic", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("print", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("println", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("real", Location::builtin, NULL, NULL));
    universe_st.insert (new FunctionSymbol ("recover", Location::builtin, NULL, NULL));
  }

}
