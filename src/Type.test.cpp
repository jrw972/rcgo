/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "catch.hpp"

#include "Type.hpp"
#include "TypeVisitor.hpp"
#include "Symbol.hpp"
#include "Package.hpp"

using namespace rcgo;

struct AcceptTestVisitor : public DefaultTypeVisitor {
  const Type* type;

  AcceptTestVisitor ()
    : type (NULL)
  { }

  virtual void default_action (const Type& type)
  {
    this->type = &type;
  }
};

#define ACCEPT_TEST(c, arg) TEST_CASE (#c "::accept(visitor)") { \
    TypeFactory factory;                                                \
    const c* type = arg;                                                \
    AcceptTestVisitor visitor;                                          \
    type->accept (visitor);                                             \
    REQUIRE (type == visitor.type);                                     \
}

ACCEPT_TEST (ErrorType, &ErrorType::instance);
ACCEPT_TEST (BoolType, &BoolType::instance);
ACCEPT_TEST (Complex64Type, &Complex64Type::instance);
ACCEPT_TEST (Complex128Type, &Complex128Type::instance);
ACCEPT_TEST (Float32Type, &Float32Type::instance);
ACCEPT_TEST (Float64Type, &Float64Type::instance);
ACCEPT_TEST (IntType, &IntType::instance);
ACCEPT_TEST (Int8Type, &Int8Type::instance);
ACCEPT_TEST (Int16Type, &Int16Type::instance);
ACCEPT_TEST (Int32Type, &Int32Type::instance);
ACCEPT_TEST (Int64Type, &Int64Type::instance);
ACCEPT_TEST (StringType, &StringType::instance);
ACCEPT_TEST (UintType, &UintType::instance);
ACCEPT_TEST (Uint8Type, &Uint8Type::instance);
ACCEPT_TEST (Uint16Type, &Uint16Type::instance);
ACCEPT_TEST (Uint32Type, &Uint32Type::instance);
ACCEPT_TEST (Uint64Type, &Uint64Type::instance);
ACCEPT_TEST (UintptrType, &UintptrType::instance);
ACCEPT_TEST (ArrayType, UintptrType::instance.get_array (3));
ACCEPT_TEST (FunctionType, factory.make_function_type ());
ACCEPT_TEST (InterfaceType, factory.make_interface_type ());
ACCEPT_TEST (MapType, UintptrType::instance.get_map (&UintptrType::instance));
ACCEPT_TEST (PointerType, UintType::instance.get_pointer ());
ACCEPT_TEST (SliceType, UintType::instance.get_slice ());
ACCEPT_TEST (StructType, factory.make_struct_type ());
ACCEPT_TEST (DefinedType, factory.make_defined_type (&UintType::instance));
ACCEPT_TEST (AliasType, factory.make_alias_type (&UintType::instance));

TEST_CASE ("Type::is_defined ()") {
  REQUIRE (ErrorType::instance.is_defined () == false);
}

TEST_CASE ("Type::get_array ()") {
  const Type* x = UintptrType::instance.get_array (3);
  const Type* y = UintptrType::instance.get_array (3);
  REQUIRE (x == y);
}

TEST_CASE ("Type::get_map ()") {
  const Type* x = UintptrType::instance.get_map (&IntType::instance);
  const Type* y = UintptrType::instance.get_map (&IntType::instance);
  REQUIRE (x == y);
}

TEST_CASE ("StructType::StructType ()") {
  TypeFactory factory;
  StructType* st = factory.make_struct_type ();
  REQUIRE (st->field_count () == 0);
  REQUIRE (st->field_begin () == st->field_end ());
}

TEST_CASE ("StructType::append ()") {
  TypeFactory factory;
  StructType* st = factory.make_struct_type ();
  FieldSymbol* field = new FieldSymbol ("x", Location::builtin, NULL, &UintptrType::instance, "tag", false);
  st->append_field (field);
  REQUIRE (st->field_count () == 1);
  REQUIRE (st->field_begin () != st->field_end ());
  REQUIRE (*(st->field_begin ()) == field);
}

TEST_CASE ("FunctionType::FunctionType ()") {
  TypeFactory factory;
  FunctionType* ft = factory.make_function_type ();
  REQUIRE (ft->parameter_count () == 0);
  REQUIRE (ft->parameter_begin () == ft->parameter_end ());
  REQUIRE (ft->result_count () == 0);
  REQUIRE (ft->result_begin () == ft->result_end ());
  REQUIRE (ft->is_variadic () == false);
}

TEST_CASE ("FunctionType::append_parameter ()") {
  TypeFactory factory;
  FunctionType* ft = factory.make_function_type ();
  ParameterSymbol* parameter = new ParameterSymbol ("x", Location::builtin, NULL, &UintptrType::instance, false);
  ft->append_parameter (parameter);
  REQUIRE (ft->parameter_count () == 1);
  REQUIRE (ft->parameter_begin () != ft->parameter_end ());
  REQUIRE (*(ft->parameter_begin ()) == parameter);
  REQUIRE (ft->is_variadic () == false);
}

TEST_CASE ("FunctionType::append_result ()") {
  TypeFactory factory;
  FunctionType* ft = factory.make_function_type ();
  ParameterSymbol* parameter = new ParameterSymbol ("x", Location::builtin, NULL, &UintptrType::instance, false);
  ft->append_result (parameter);
  REQUIRE (ft->result_count () == 1);
  REQUIRE (ft->result_begin () != ft->result_end ());
  REQUIRE (*(ft->result_begin ()) == parameter);
}

TEST_CASE ("DefinedType::DefinedType ()") {
  TypeFactory factory;
  DefinedType* dt = factory.make_defined_type (&UintptrType::instance);
  REQUIRE (dt->underlying_type () == &UintptrType::instance);
  REQUIRE (dt->is_defined () == true);
}

TEST_CASE ("AliasType::AliasType ()") {
  TypeFactory factory;
  AliasType* at = factory.make_alias_type (&UintptrType::instance);
  REQUIRE (at->underlying_type () == &UintptrType::instance);
  REQUIRE (at->is_defined () == false);
}

TEST_CASE ("identical (same defined)") {
  TypeFactory factory;
  DefinedType* dt = factory.make_defined_type (&UintptrType::instance);
  REQUIRE (identical (dt, dt));
}

TEST_CASE ("different (different defined)") {
  TypeFactory factory;
  DefinedType* x = factory.make_defined_type (&UintptrType::instance);
  DefinedType* y = factory.make_defined_type (&IntType::instance);
  REQUIRE (different (x, y));
}

TEST_CASE ("identical (atoms)") {
  REQUIRE (identical (&UintptrType::instance, &UintptrType::instance));
}

TEST_CASE ("different (atoms)") {
  REQUIRE (different (&UintptrType::instance, &IntType::instance));
}

TEST_CASE ("different (interface, function)") {
  TypeFactory factory;
  InterfaceType* x = factory.make_interface_type ();
  FunctionType* y = factory.make_function_type ();
  REQUIRE (different (x, y));
}

TEST_CASE ("different (interface 0, interface 1)") {
  TypeFactory factory;
  InterfaceType* x = factory.make_interface_type ();
  InterfaceType* y = factory.make_interface_type ()->append_method (new InterfaceMethodSymbol ("x", Location::builtin, NULL, factory.make_function_type ()));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (interface method names)") {
  TypeFactory factory;
  InterfaceType* x = factory.make_interface_type ()->append_method (new InterfaceMethodSymbol ("x", Location::builtin, NULL, factory.make_function_type ()));
  InterfaceType* y = factory.make_interface_type ()->append_method (new InterfaceMethodSymbol ("y", Location::builtin, NULL, factory.make_function_type ()));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (interface method types)") {
  TypeFactory factory;
  InterfaceType* x = factory.make_interface_type ()->append_method (new InterfaceMethodSymbol ("x", Location::builtin, NULL, factory.make_function_type ()->append_parameter (new ParameterSymbol ("x", Location::builtin, NULL, &UintType::instance, false))));
  InterfaceType* y = factory.make_interface_type ()->append_method (new InterfaceMethodSymbol ("x", Location::builtin, NULL, factory.make_function_type ()->append_parameter (new ParameterSymbol ("x", Location::builtin, NULL, &IntType::instance, false))));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (interface method packages)") {
  TypeFactory factory;
  Package xp ("import path");
  InterfaceType* x = factory.make_interface_type ()->append_method (new InterfaceMethodSymbol ("x", Location::builtin, &xp, factory.make_function_type ()));
  Package yp ("import path");
  InterfaceType* y = factory.make_interface_type ()->append_method (new InterfaceMethodSymbol ("x", Location::builtin, &yp, factory.make_function_type ()));
  REQUIRE (different (x, y));
}

TEST_CASE ("identical (interfaces)") {
  TypeFactory factory;
  InterfaceType* x = factory.make_interface_type ()->append_method (new InterfaceMethodSymbol ("x", Location::builtin, NULL, factory.make_function_type ()));
  InterfaceType* y = factory.make_interface_type ()->append_method (new InterfaceMethodSymbol ("x", Location::builtin, NULL, factory.make_function_type ()));
  REQUIRE (identical (x, y));
}

TEST_CASE ("different (struct, function)") {
  TypeFactory factory;
  StructType* x = factory.make_struct_type ();
  FunctionType* y = factory.make_function_type ();
  REQUIRE (different (x, y));
}

TEST_CASE ("different (struct 0, struct 1)") {
  TypeFactory factory;
  StructType* x = factory.make_struct_type ();
  StructType* y = factory.make_struct_type ()->append_field (new FieldSymbol ("x", Location::builtin, NULL, &UintptrType::instance, "tag", false));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (struct field names)") {
  TypeFactory factory;
  StructType* x = factory.make_struct_type ()->append_field (new FieldSymbol ("x", Location::builtin, NULL, &UintptrType::instance, "tag", false));
  StructType* y = factory.make_struct_type ()->append_field (new FieldSymbol ("y", Location::builtin, NULL, &UintptrType::instance, "tag", false));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (struct field types)") {
  TypeFactory factory;
  StructType* x = factory.make_struct_type ()->append_field (new FieldSymbol ("x", Location::builtin, NULL, &UintptrType::instance, "tag", false));
  StructType* y = factory.make_struct_type ()->append_field (new FieldSymbol ("x", Location::builtin, NULL, &IntType::instance, "tag", false));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (struct field tags)") {
  TypeFactory factory;
  StructType* x = factory.make_struct_type ()->append_field (new FieldSymbol ("x", Location::builtin, NULL, &UintptrType::instance, "tag", false));
  StructType* y = factory.make_struct_type ()->append_field (new FieldSymbol ("x", Location::builtin, NULL, &UintptrType::instance, "no tag", false));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (struct field packages)") {
  TypeFactory factory;
  Package xp ("import path");
  StructType* x = factory.make_struct_type ()->append_field (new FieldSymbol ("x", Location::builtin, &xp, &UintptrType::instance, "tag", false));
  Package yp ("import path");
  StructType* y = factory.make_struct_type ()->append_field (new FieldSymbol ("x", Location::builtin, &yp, &UintptrType::instance, "tag", false));
  REQUIRE (different (x, y));
}

TEST_CASE ("identical (structs)") {
  TypeFactory factory;
  StructType* x = factory.make_struct_type ()->append_field (new FieldSymbol ("x", Location::builtin, NULL, &UintptrType::instance, "tag", false));
  StructType* y = factory.make_struct_type ()->append_field (new FieldSymbol ("x", Location::builtin, NULL, &UintptrType::instance, "tag", false));
  REQUIRE (identical (x, y));
}

TEST_CASE ("different (array, struct)") {
  TypeFactory factory;
  const Type* x = factory.make_struct_type ()->get_array (3);
  const Type* y = factory.make_struct_type ();
  REQUIRE (different (x, y));
}

TEST_CASE ("different (array[3], array[4])") {
  TypeFactory factory;
  const Type* x = factory.make_struct_type ()->get_array (3);
  const Type* y = factory.make_struct_type ()->get_array (4);
  REQUIRE (different (x, y));
}

TEST_CASE ("identical (array[3], array[3])") {
  TypeFactory factory;
  const Type* x = factory.make_struct_type ()->get_array (3);
  const Type* y = factory.make_struct_type ()->get_array (3);
  REQUIRE (identical (x, y));
}

TEST_CASE ("different (map, struct)") {
  TypeFactory factory;
  const Type* x = factory.make_struct_type ()->get_map (&IntType::instance);
  const Type* y = factory.make_struct_type ();
  REQUIRE (different (x, y));
}

TEST_CASE ("different (map[int], map[uint])") {
  TypeFactory factory;
  const Type* x = factory.make_struct_type ()->get_map (&IntType::instance);
  const Type* y = factory.make_struct_type ()->get_map (&UintType::instance);
  REQUIRE (different (x, y));
}

TEST_CASE ("identical (map[int], map[int])") {
  TypeFactory factory;
  const Type* x = factory.make_struct_type ()->get_map (&IntType::instance);
  const Type* y = factory.make_struct_type ()->get_map (&IntType::instance);
  REQUIRE (identical (x, y));
}

TEST_CASE ("different (slice, struct)") {
  TypeFactory factory;
  const Type* x = factory.make_struct_type ()->get_slice ();
  const Type* y = factory.make_struct_type ();
  REQUIRE (different (x, y));
}

TEST_CASE ("different (slice[int], slice[uint])") {
  TypeFactory factory;
  const Type* x = IntType::instance.get_slice ();
  const Type* y = UintType::instance.get_slice ();
  REQUIRE (different (x, y));
}

TEST_CASE ("identical (slice[int], slice[int])") {
  TypeFactory factory;
  const Type* x = IntType::instance.get_slice ();
  const Type* y = IntType::instance.get_slice ();
  REQUIRE (identical (x, y));
}

TEST_CASE ("different (pointer, struct)") {
  TypeFactory factory;
  const Type* x = factory.make_struct_type ()->get_pointer ();
  const Type* y = factory.make_struct_type ();
  REQUIRE (different (x, y));
}

TEST_CASE ("different (pointer[int], pointer[uint])") {
  TypeFactory factory;
  const Type* x = IntType::instance.get_pointer ();
  const Type* y = UintType::instance.get_pointer ();
  REQUIRE (different (x, y));
}

TEST_CASE ("identical (pointer[int], pointer[int])") {
  TypeFactory factory;
  const Type* x = IntType::instance.get_pointer ();
  const Type* y = IntType::instance.get_pointer ();
  REQUIRE (identical (x, y));
}

TEST_CASE ("different (function, struct)") {
  TypeFactory factory;
  FunctionType* x = factory.make_function_type ();
  StructType* y = factory.make_struct_type ();
  REQUIRE (different (x, y));
}

TEST_CASE ("different (function 0 -> 0, function 1 -> 0)") {
  TypeFactory factory;
  FunctionType* x = factory.make_function_type ();
  FunctionType* y = factory.make_function_type ()->append_parameter (new ParameterSymbol ("x", Location::builtin, NULL, &UintptrType::instance, false));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (function 0 -> 0, function 0 -> 1)") {
  TypeFactory factory;
  FunctionType* x = factory.make_function_type ();
  FunctionType* y = factory.make_function_type ()->append_result (new ParameterSymbol ("x", Location::builtin, NULL, &UintptrType::instance, false));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (function 1v -> 0, function 1 -> 0)") {
  TypeFactory factory;
  FunctionType* x = factory.make_function_type ()->append_parameter (new ParameterSymbol ("x", Location::builtin, NULL, &UintptrType::instance, true));
  FunctionType* y = factory.make_function_type ()->append_parameter (new ParameterSymbol ("x", Location::builtin, NULL, &UintptrType::instance, false));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (function int -> 0, function uint -> 0)") {
  TypeFactory factory;
  FunctionType* x = factory.make_function_type ()->append_parameter (new ParameterSymbol ("x", Location::builtin, NULL, &IntType::instance, false));
  FunctionType* y = factory.make_function_type ()->append_parameter (new ParameterSymbol ("x", Location::builtin, NULL, &UintType::instance, false));
  REQUIRE (different (x, y));
}

TEST_CASE ("different (function 0 -> int, function 0 -> uint)") {
  TypeFactory factory;
  FunctionType* x = factory.make_function_type ()->append_result (new ParameterSymbol ("x", Location::builtin, NULL, &IntType::instance, false));
  FunctionType* y = factory.make_function_type ()->append_result (new ParameterSymbol ("x", Location::builtin, NULL, &UintType::instance, false));
  REQUIRE (different (x, y));
}

TEST_CASE ("identical (function int -> int, function int -> int)") {
  TypeFactory factory;
  FunctionType* x = factory.make_function_type ()->append_parameter (new ParameterSymbol ("x", Location::builtin, NULL, &IntType::instance, false))->append_result (new ParameterSymbol ("x", Location::builtin, NULL, &IntType::instance, false));
  FunctionType* y = factory.make_function_type ()->append_parameter (new ParameterSymbol ("x", Location::builtin, NULL, &IntType::instance, false))->append_result (new ParameterSymbol ("x", Location::builtin, NULL, &IntType::instance, false));
  REQUIRE (identical (x, y));
}

TEST_CASE ("comparable") {
  TypeFactory factory;
  REQUIRE (comparable (&BoolType::instance));
  REQUIRE (comparable (&Int8Type::instance));
  REQUIRE (comparable (&Int16Type::instance));
  REQUIRE (comparable (&Int32Type::instance));
  REQUIRE (comparable (&Int64Type::instance));
  REQUIRE (comparable (&Uint8Type::instance));
  REQUIRE (comparable (&Uint16Type::instance));
  REQUIRE (comparable (&Uint32Type::instance));
  REQUIRE (comparable (&Uint64Type::instance));
  REQUIRE (comparable (&IntType::instance));
  REQUIRE (comparable (&UintType::instance));
  REQUIRE (comparable (&UintptrType::instance));
  REQUIRE (comparable (&Float32Type::instance));
  REQUIRE (comparable (&Float64Type::instance));
  REQUIRE (comparable (&Complex64Type::instance));
  REQUIRE (comparable (&Complex128Type::instance));
  REQUIRE (comparable (&StringType::instance));
  REQUIRE (comparable (StringType::instance.get_pointer ()));
  REQUIRE (comparable (factory.make_interface_type ()));
  REQUIRE (comparable (factory.make_struct_type ()->append_field (new FieldSymbol ("f", Location::builtin, NULL, factory.make_function_type (), "tag", false))) == false);
  REQUIRE (comparable (factory.make_struct_type ()->append_field (new FieldSymbol ("f", Location::builtin, NULL, &IntType::instance, "tag", false))));
  REQUIRE (comparable (StringType::instance.get_array (3)));
}

TEST_CASE ("is_arithmetic") {
  REQUIRE (is_arithmetic (&Int8Type::instance));
  REQUIRE (is_arithmetic (&Int16Type::instance));
  REQUIRE (is_arithmetic (&Int32Type::instance));
  REQUIRE (is_arithmetic (&Int64Type::instance));
  REQUIRE (is_arithmetic (&Uint8Type::instance));
  REQUIRE (is_arithmetic (&Uint16Type::instance));
  REQUIRE (is_arithmetic (&Uint32Type::instance));
  REQUIRE (is_arithmetic (&Uint64Type::instance));
  REQUIRE (is_arithmetic (&IntType::instance));
  REQUIRE (is_arithmetic (&UintType::instance));
  REQUIRE (is_arithmetic (&UintptrType::instance));
  REQUIRE (is_arithmetic (&Float32Type::instance));
  REQUIRE (is_arithmetic (&Float64Type::instance));
  REQUIRE (is_arithmetic (&Complex64Type::instance));
  REQUIRE (is_arithmetic (&Complex128Type::instance));
}

TEST_CASE ("is_integer") {
  REQUIRE (is_integer (&Int8Type::instance));
  REQUIRE (is_integer (&Int16Type::instance));
  REQUIRE (is_integer (&Int32Type::instance));
  REQUIRE (is_integer (&Int64Type::instance));
  REQUIRE (is_integer (&Uint8Type::instance));
  REQUIRE (is_integer (&Uint16Type::instance));
  REQUIRE (is_integer (&Uint32Type::instance));
  REQUIRE (is_integer (&Uint64Type::instance));
  REQUIRE (is_integer (&IntType::instance));
  REQUIRE (is_integer (&UintType::instance));
  REQUIRE (is_integer (&UintptrType::instance));
}

TEST_CASE ("is_signed") {
  REQUIRE (is_signed (&Int8Type::instance));
  REQUIRE (is_signed (&Int16Type::instance));
  REQUIRE (is_signed (&Int32Type::instance));
  REQUIRE (is_signed (&Int64Type::instance));
  REQUIRE (is_signed (&IntType::instance));
  REQUIRE (is_signed (&Float32Type::instance));
  REQUIRE (is_signed (&Float64Type::instance));
  REQUIRE (is_signed (&Complex64Type::instance));
  REQUIRE (is_signed (&Complex128Type::instance));
}

TEST_CASE ("is_string") {
  REQUIRE (is_string (&StringType::instance));
}

TEST_CASE ("is_boolean") {
  REQUIRE (is_boolean (&BoolType::instance));
}
