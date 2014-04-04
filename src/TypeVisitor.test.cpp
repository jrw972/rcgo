/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "TypeVisitor.hpp"
#include "Type.hpp"

using namespace rcgo;

#define VISIT_TEST(c, arg) TEST_CASE ("DefaultTypeVisitor::visit(" #c ")") { \
    TypeFactory factory;                                                \
    const c* type = arg;                                                \
    DefaultTypeVisitor visitor;                                         \
    visitor.visit (*type);                                              \
}

VISIT_TEST (ErrorType, &ErrorType::instance);
VISIT_TEST (BoolType, &BoolType::instance);
VISIT_TEST (Complex64Type, &Complex64Type::instance);
VISIT_TEST (Complex128Type, &Complex128Type::instance);
VISIT_TEST (Float32Type, &Float32Type::instance);
VISIT_TEST (Float64Type, &Float64Type::instance);
VISIT_TEST (IntType, &IntType::instance);
VISIT_TEST (Int8Type, &Int8Type::instance);
VISIT_TEST (Int16Type, &Int16Type::instance);
VISIT_TEST (Int32Type, &Int32Type::instance);
VISIT_TEST (Int64Type, &Int64Type::instance);
VISIT_TEST (StringType, &StringType::instance);
VISIT_TEST (UintType, &UintType::instance);
VISIT_TEST (Uint8Type, &Uint8Type::instance);
VISIT_TEST (Uint16Type, &Uint16Type::instance);
VISIT_TEST (Uint32Type, &Uint32Type::instance);
VISIT_TEST (Uint64Type, &Uint64Type::instance);
VISIT_TEST (UintptrType, &UintptrType::instance);
VISIT_TEST (ArrayType, UintptrType::instance.get_array (3));
VISIT_TEST (FunctionType, factory.make_function_type ());
VISIT_TEST (InterfaceType, factory.make_interface_type ());
VISIT_TEST (MapType, UintptrType::instance.get_map(&UintptrType::instance));
VISIT_TEST (PointerType, UintType::instance.get_pointer ());
VISIT_TEST (SliceType, UintType::instance.get_slice ());
VISIT_TEST (StructType, factory.make_struct_type ());
VISIT_TEST (DefinedType, factory.make_defined_type (&UintType::instance));
VISIT_TEST (AliasType, factory.make_alias_type (&UintType::instance));
