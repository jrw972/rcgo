// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/type.h"

#include "src/catch.hpp"
#include "src/package.h"
#include "src/test_helpers.h"

namespace rcgo {
namespace test {

struct TypeAcceptTestVisitor : public type::DefaultVisitor {
  const type::Type* type;

  TypeAcceptTestVisitor()
      : type(NULL) { }

  void DefaultAction(const type::Type& type) override {
    this->type = &type;
  }
};


#define ACCEPT_TEST(c, arg) TEST_CASE(#c "::accept(visitor)") {         \
    type::Factory factory;                                              \
    const c* type = arg;                                                \
    TypeAcceptTestVisitor visitor;                                      \
    type->Accept(&visitor);                                             \
    REQUIRE(type == visitor.type);                                      \
}

ACCEPT_TEST(type::Error, &type::Error::instance);
ACCEPT_TEST(type::Bool, &type::Bool::instance);
ACCEPT_TEST(type::Complex64, &type::Complex64::instance);
ACCEPT_TEST(type::Complex128, &type::Complex128::instance);
ACCEPT_TEST(type::Float32, &type::Float32::instance);
ACCEPT_TEST(type::Float64, &type::Float64::instance);
ACCEPT_TEST(type::Int, &type::Int::instance);
ACCEPT_TEST(type::Int8, &type::Int8::instance);
ACCEPT_TEST(type::Int16, &type::Int16::instance);
ACCEPT_TEST(type::Int32, &type::Int32::instance);
ACCEPT_TEST(type::Int64, &type::Int64::instance);
ACCEPT_TEST(type::String, &type::String::instance);
ACCEPT_TEST(type::Uint, &type::Uint::instance);
ACCEPT_TEST(type::Uint8, &type::Uint8::instance);
ACCEPT_TEST(type::Uint16, &type::Uint16::instance);
ACCEPT_TEST(type::Uint32, &type::Uint32::instance);
ACCEPT_TEST(type::Uint64, &type::Uint64::instance);
ACCEPT_TEST(type::Uintptr, &type::Uintptr::instance);
ACCEPT_TEST(type::Array, type::Uintptr::instance.GetArray(3));
ACCEPT_TEST(type::Function, factory.MakeFunction());
ACCEPT_TEST(type::Interface, factory.MakeInterface());
ACCEPT_TEST(type::Map,
            type::Uintptr::instance.GetMap(&type::Uintptr::instance));
ACCEPT_TEST(type::Pointer, type::Uint::instance.GetPointer());
ACCEPT_TEST(type::Slice, type::Uint::instance.GetSlice());
ACCEPT_TEST(type::Struct, factory.MakeStruct());
ACCEPT_TEST(type::DefinedType, factory.MakeDefinedType(&type::Uint::instance));
ACCEPT_TEST(type::Alias, factory.MakeAlias(&type::Uint::instance));

TEST_CASE("Type::IsDefined()") {
  REQUIRE(type::Error::instance.IsDefined() == false);
}

TEST_CASE("Type::GetArray()") {
  const type::Type* x = type::Uintptr::instance.GetArray(3);
  const type::Type* y = type::Uintptr::instance.GetArray(3);
  REQUIRE(x == y);
}

TEST_CASE("Type::GetMap()") {
  const type::Type* x = type::Uintptr::instance.GetMap(&type::Int::instance);
  const type::Type* y = type::Uintptr::instance.GetMap(&type::Int::instance);
  REQUIRE(x == y);
}

TEST_CASE("Struct::Struct()") {
  type::Factory factory;
  type::Struct* st = factory.MakeStruct();
  REQUIRE(st->FieldCount() == 0);
  REQUIRE(st->FieldBegin() == st->FieldEnd());
}

TEST_CASE("Struct::append()") {
  type::Factory factory;
  type::Struct* st = factory.MakeStruct();
  FieldSymbol* field =
      new FieldSymbol("x", location, NULL, &type::Uintptr::instance,
                      "tag", false);
  st->AppendField(field);
  REQUIRE(st->FieldCount() == 1);
  REQUIRE(st->FieldBegin() != st->FieldEnd());
  REQUIRE(*(st->FieldBegin()) == field);
}

TEST_CASE("Function::Function()") {
  type::Factory factory;
  type::Function* ft = factory.MakeFunction();
  REQUIRE(ft->ParameterCount() == 0);
  REQUIRE(ft->ParameterBegin() == ft->ParameterEnd());
  REQUIRE(ft->ResultCount() == 0);
  REQUIRE(ft->ResultBegin() == ft->ResultEnd());
  REQUIRE(ft->IsVariadic() == false);
}

TEST_CASE("Function::AppendParameter()") {
  type::Factory factory;
  type::Function* ft = factory.MakeFunction();
  ParameterSymbol* parameter =
      new ParameterSymbol("x", location, NULL,
                          &type::Uintptr::instance, false);
  ft->AppendParameter(parameter);
  REQUIRE(ft->ParameterCount() == 1);
  REQUIRE(ft->ParameterBegin() != ft->ParameterEnd());
  REQUIRE(*(ft->ParameterBegin()) == parameter);
  REQUIRE(ft->IsVariadic() == false);
}

TEST_CASE("type::Function::AppendResult()") {
  type::Factory factory;
  type::Function* ft = factory.MakeFunction();
  ParameterSymbol* parameter =
      new ParameterSymbol("x", location, NULL,
                          &type::Uintptr::instance, false);
  ft->AppendResult(parameter);
  REQUIRE(ft->ResultCount() == 1);
  REQUIRE(ft->ResultBegin() != ft->ResultEnd());
  REQUIRE(*(ft->ResultBegin()) == parameter);
}

TEST_CASE("DefinedType::DefinedType()") {
  type::Factory factory;
  type::DefinedType* dt = factory.MakeDefinedType(&type::Uintptr::instance);
  REQUIRE(dt->UnderlyingType() == &type::Uintptr::instance);
  REQUIRE(dt->IsDefined() == true);
}

TEST_CASE("Alias::Alias()") {
  type::Factory factory;
  type::Alias* at = factory.MakeAlias(&type::Uintptr::instance);
  REQUIRE(at->UnderlyingType() == &type::Uintptr::instance);
  REQUIRE(at->IsDefined() == false);
}

TEST_CASE("Identical(same defined)") {
  type::Factory factory;
  type::DefinedType* dt = factory.MakeDefinedType(&type::Uintptr::instance);
  REQUIRE(Identical(dt, dt));
}

TEST_CASE("Different(different defined)") {
  type::Factory factory;
  type::DefinedType* x = factory.MakeDefinedType(&type::Uintptr::instance);
  type::DefinedType* y = factory.MakeDefinedType(&type::Int::instance);
  REQUIRE(Different(x, y));
}

TEST_CASE("Identical(atoms)") {
  REQUIRE(Identical(&type::Uintptr::instance, &type::Uintptr::instance));
}

TEST_CASE("Different(atoms)") {
  REQUIRE(Different(&type::Uintptr::instance, &type::Int::instance));
}

TEST_CASE("Different(interface, function)") {
  type::Factory factory;
  type::Interface* x = factory.MakeInterface();
  type::Function* y = factory.MakeFunction();
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(interface 0, interface 1)") {
  type::Factory factory;
  type::Interface* x = factory.MakeInterface();
  type::Interface* y =
      factory.MakeInterface()->AppendMethod(
          new InterfaceMethodSymbol("x", location, NULL,
                                    factory.MakeFunction()));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(interface method names)") {
  type::Factory factory;
  type::Interface* x =
      factory.MakeInterface()->AppendMethod(
          new InterfaceMethodSymbol("x", location, NULL,
                                    factory.MakeFunction()));
  type::Interface* y =
      factory.MakeInterface()->AppendMethod(
          new InterfaceMethodSymbol("y", location, NULL,
                                    factory.MakeFunction()));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(interface method types)") {
  type::Factory factory;
  type::Function* fx = factory.MakeFunction()->AppendParameter(
      new ParameterSymbol("x", location, NULL, &type::Uint::instance, false));
  type::Interface* x = factory.MakeInterface() ->AppendMethod(
      new InterfaceMethodSymbol("x", location, NULL, fx));
  type::Function* fy = factory.MakeFunction()->AppendParameter(
      new ParameterSymbol("x", location, NULL, &type::Int::instance, false));
  type::Interface* y = factory.MakeInterface()->AppendMethod(
      new InterfaceMethodSymbol("x", location, NULL, fy));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(interface method packages)") {
  type::Factory factory;
  Package xp("import path");
  type::Interface* x = factory.MakeInterface()->AppendMethod(
      new InterfaceMethodSymbol("x", location, &xp, factory.MakeFunction()));
  Package yp("import path");
  type::Interface* y = factory.MakeInterface()->AppendMethod(
      new InterfaceMethodSymbol("x", location, &yp, factory.MakeFunction()));
  REQUIRE(Different(x, y));
}

TEST_CASE("Identical(interfaces)") {
  type::Factory factory;
  type::Interface* x = factory.MakeInterface()->AppendMethod(
      new InterfaceMethodSymbol("x", location, NULL, factory.MakeFunction()));
  type::Interface* y = factory.MakeInterface()->AppendMethod(
      new InterfaceMethodSymbol("x", location, NULL, factory.MakeFunction()));
  REQUIRE(Identical(x, y));
}

TEST_CASE("Different(struct, function)") {
  type::Factory factory;
  type::Struct* x = factory.MakeStruct();
  type::Function* y = factory.MakeFunction();
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(struct 0, struct 1)") {
  type::Factory factory;
  type::Struct* x = factory.MakeStruct();
  type::Struct* y = factory.MakeStruct()->AppendField(
      new FieldSymbol("x", location, NULL, &type::Uintptr::instance, "tag",
                      false));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(struct field names)") {
  type::Factory factory;
  type::Struct* x = factory.MakeStruct()->AppendField(
      new FieldSymbol("x", location, NULL, &type::Uintptr::instance, "tag",
                      false));
  type::Struct* y = factory.MakeStruct()->AppendField(
      new FieldSymbol("y", location, NULL, &type::Uintptr::instance, "tag",
                      false));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(struct field types)") {
  type::Factory factory;
  type::Struct* x = factory.MakeStruct()->AppendField(
      new FieldSymbol("x", location, NULL, &type::Uintptr::instance, "tag",
                      false));
  type::Struct* y = factory.MakeStruct()->AppendField(
      new FieldSymbol("x", location, NULL, &type::Int::instance, "tag", false));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(struct field tags)") {
  type::Factory factory;
  type::Struct* x = factory.MakeStruct()->AppendField(
      new FieldSymbol("x", location, NULL, &type::Uintptr::instance, "tag",
                      false));
  type::Struct* y = factory.MakeStruct()->AppendField(
      new FieldSymbol("x", location, NULL, &type::Uintptr::instance, "no tag",
                      false));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(struct field packages)") {
  type::Factory factory;
  Package xp("import path");
  type::Struct* x = factory.MakeStruct()->AppendField(
      new FieldSymbol("x", location, &xp, &type::Uintptr::instance, "tag",
                      false));
  Package yp("import path");
  type::Struct* y = factory.MakeStruct()->AppendField(
      new FieldSymbol("x", location, &yp, &type::Uintptr::instance, "tag",
                      false));
  REQUIRE(Different(x, y));
}

TEST_CASE("Identical(structs)") {
  type::Factory factory;
  type::Struct* x = factory.MakeStruct()->AppendField(
      new FieldSymbol("x", location, NULL, &type::Uintptr::instance, "tag",
                      false));
  type::Struct* y = factory.MakeStruct()->AppendField(
      new FieldSymbol("x", location, NULL, &type::Uintptr::instance, "tag",
                      false));
  REQUIRE(Identical(x, y));
}

TEST_CASE("Different(array, struct)") {
  type::Factory factory;
  const type::Type* x = factory.MakeStruct()->GetArray(3);
  const type::Type* y = factory.MakeStruct();
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(array[3], array[4])") {
  type::Factory factory;
  const type::Type* x = factory.MakeStruct()->GetArray(3);
  const type::Type* y = factory.MakeStruct()->GetArray(4);
  REQUIRE(Different(x, y));
}

TEST_CASE("Identical(array[3], array[3])") {
  type::Factory factory;
  const type::Type* x = factory.MakeStruct()->GetArray(3);
  const type::Type* y = factory.MakeStruct()->GetArray(3);
  REQUIRE(Identical(x, y));
}

TEST_CASE("Different(map, struct)") {
  type::Factory factory;
  const type::Type* x = factory.MakeStruct()->GetMap(&type::Int::instance);
  const type::Type* y = factory.MakeStruct();
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(map[int], map[uint])") {
  type::Factory factory;
  const type::Type* x = factory.MakeStruct()->GetMap(&type::Int::instance);
  const type::Type* y = factory.MakeStruct()->GetMap(&type::Uint::instance);
  REQUIRE(Different(x, y));
}

TEST_CASE("Identical(map[int], map[int])") {
  type::Factory factory;
  const type::Type* x = factory.MakeStruct()->GetMap(&type::Int::instance);
  const type::Type* y = factory.MakeStruct()->GetMap(&type::Int::instance);
  REQUIRE(Identical(x, y));
}

TEST_CASE("Different(slice, struct)") {
  type::Factory factory;
  const type::Type* x = factory.MakeStruct()->GetSlice();
  const type::Type* y = factory.MakeStruct();
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(slice[int], slice[uint])") {
  type::Factory factory;
  const type::Type* x = type::Int::instance.GetSlice();
  const type::Type* y = type::Uint::instance.GetSlice();
  REQUIRE(Different(x, y));
}

TEST_CASE("Identical(slice[int], slice[int])") {
  type::Factory factory;
  const type::Type* x = type::Int::instance.GetSlice();
  const type::Type* y = type::Int::instance.GetSlice();
  REQUIRE(Identical(x, y));
}

TEST_CASE("Different(pointer, struct)") {
  type::Factory factory;
  const type::Type* x = factory.MakeStruct()->GetPointer();
  const type::Type* y = factory.MakeStruct();
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(pointer[int], pointer[uint])") {
  type::Factory factory;
  const type::Type* x = type::Int::instance.GetPointer();
  const type::Type* y = type::Uint::instance.GetPointer();
  REQUIRE(Different(x, y));
}

TEST_CASE("Identical(pointer[int], pointer[int])") {
  type::Factory factory;
  const type::Type* x = type::Int::instance.GetPointer();
  const type::Type* y = type::Int::instance.GetPointer();
  REQUIRE(Identical(x, y));
}

TEST_CASE("Different(function, struct)") {
  type::Factory factory;
  type::Function* x = factory.MakeFunction();
  type::Struct* y = factory.MakeStruct();
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(function 0 -> 0, function 1 -> 0)") {
  type::Factory factory;
  type::Function* x = factory.MakeFunction();
  type::Function* y = factory.MakeFunction()->AppendParameter(
      new ParameterSymbol("x", location, NULL, &type::Uintptr::instance,
                          false));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(function 0 -> 0, function 0 -> 1)") {
  type::Factory factory;
  type::Function* x = factory.MakeFunction();
  type::Function* y = factory.MakeFunction()->AppendResult(
      new ParameterSymbol("x", location, NULL, &type::Uintptr::instance,
                          false));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(function 1v -> 0, function 1 -> 0)") {
  type::Factory factory;
  type::Function* x = factory.MakeFunction()->AppendParameter(
      new ParameterSymbol("x", location, NULL, &type::Uintptr::instance, true));
  type::Function* y = factory.MakeFunction()->AppendParameter(
      new ParameterSymbol("x", location, NULL, &type::Uintptr::instance,
                          false));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(function int -> 0, function uint -> 0)") {
  type::Factory factory;
  type::Function* x = factory.MakeFunction()->AppendParameter(
      new ParameterSymbol("x", location, NULL, &type::Int::instance, false));
  type::Function* y = factory.MakeFunction()->AppendParameter(
      new ParameterSymbol("x", location, NULL, &type::Uint::instance, false));
  REQUIRE(Different(x, y));
}

TEST_CASE("Different(function 0 -> int, function 0 -> uint)") {
  type::Factory factory;
  type::Function* x = factory.MakeFunction()->AppendResult(
      new ParameterSymbol("x", location, NULL, &type::Int::instance, false));
  type::Function* y = factory.MakeFunction()->AppendResult(
      new ParameterSymbol("x", location, NULL, &type::Uint::instance, false));
  REQUIRE(Different(x, y));
}

TEST_CASE("Identical(function int -> int, function int -> int)") {
  type::Factory factory;
  type::Function* x = factory.MakeFunction()
      ->AppendParameter(
          new ParameterSymbol("x", location, NULL, &type::Int::instance, false))
      ->AppendResult(
          new ParameterSymbol("x", location, NULL, &type::Int::instance,
                              false));
  type::Function* y = factory.MakeFunction()
      ->AppendParameter(
          new ParameterSymbol("x", location, NULL, &type::Int::instance, false))
      ->AppendResult(
          new ParameterSymbol("x", location, NULL, &type::Int::instance,
                              false));
  REQUIRE(Identical(x, y));
}

TEST_CASE("Comparable") {
  type::Factory factory;
  REQUIRE(Comparable(&type::Bool::instance));
  REQUIRE(Comparable(&type::Int8::instance));
  REQUIRE(Comparable(&type::Int16::instance));
  REQUIRE(Comparable(&type::Int32::instance));
  REQUIRE(Comparable(&type::Int64::instance));
  REQUIRE(Comparable(&type::Uint8::instance));
  REQUIRE(Comparable(&type::Uint16::instance));
  REQUIRE(Comparable(&type::Uint32::instance));
  REQUIRE(Comparable(&type::Uint64::instance));
  REQUIRE(Comparable(&type::Int::instance));
  REQUIRE(Comparable(&type::Uint::instance));
  REQUIRE(Comparable(&type::Uintptr::instance));
  REQUIRE(Comparable(&type::Float32::instance));
  REQUIRE(Comparable(&type::Float64::instance));
  REQUIRE(Comparable(&type::Complex64::instance));
  REQUIRE(Comparable(&type::Complex128::instance));
  REQUIRE(Comparable(&type::String::instance));
  REQUIRE(Comparable(type::String::instance.GetPointer()));
  REQUIRE(Comparable(factory.MakeInterface()));
  REQUIRE(Comparable(factory.MakeStruct()->AppendField(
      new FieldSymbol("f", location, NULL,
                      factory.MakeFunction(), "tag", false))) == false);
  REQUIRE(Comparable(factory.MakeStruct()->AppendField(
      new FieldSymbol("f", location, NULL, &type::Int::instance, "tag",
                      false))));
  REQUIRE(Comparable(type::String::instance.GetArray(3)));
}

TEST_CASE("IsArithmetic") {
  REQUIRE(IsArithmetic(&type::Int8::instance));
  REQUIRE(IsArithmetic(&type::Int16::instance));
  REQUIRE(IsArithmetic(&type::Int32::instance));
  REQUIRE(IsArithmetic(&type::Int64::instance));
  REQUIRE(IsArithmetic(&type::Uint8::instance));
  REQUIRE(IsArithmetic(&type::Uint16::instance));
  REQUIRE(IsArithmetic(&type::Uint32::instance));
  REQUIRE(IsArithmetic(&type::Uint64::instance));
  REQUIRE(IsArithmetic(&type::Int::instance));
  REQUIRE(IsArithmetic(&type::Uint::instance));
  REQUIRE(IsArithmetic(&type::Uintptr::instance));
  REQUIRE(IsArithmetic(&type::Float32::instance));
  REQUIRE(IsArithmetic(&type::Float64::instance));
  REQUIRE(IsArithmetic(&type::Complex64::instance));
  REQUIRE(IsArithmetic(&type::Complex128::instance));
}

TEST_CASE("IsInteger") {
  REQUIRE(IsInteger(&type::Int8::instance));
  REQUIRE(IsInteger(&type::Int16::instance));
  REQUIRE(IsInteger(&type::Int32::instance));
  REQUIRE(IsInteger(&type::Int64::instance));
  REQUIRE(IsInteger(&type::Uint8::instance));
  REQUIRE(IsInteger(&type::Uint16::instance));
  REQUIRE(IsInteger(&type::Uint32::instance));
  REQUIRE(IsInteger(&type::Uint64::instance));
  REQUIRE(IsInteger(&type::Int::instance));
  REQUIRE(IsInteger(&type::Uint::instance));
  REQUIRE(IsInteger(&type::Uintptr::instance));
}

TEST_CASE("IsSigned") {
  REQUIRE(IsSigned(&type::Int8::instance));
  REQUIRE(IsSigned(&type::Int16::instance));
  REQUIRE(IsSigned(&type::Int32::instance));
  REQUIRE(IsSigned(&type::Int64::instance));
  REQUIRE(IsSigned(&type::Int::instance));
  REQUIRE(IsSigned(&type::Float32::instance));
  REQUIRE(IsSigned(&type::Float64::instance));
  REQUIRE(IsSigned(&type::Complex64::instance));
  REQUIRE(IsSigned(&type::Complex128::instance));
}

TEST_CASE("IsString") {
  REQUIRE(IsString(&type::String::instance));
}

TEST_CASE("IsBoolean") {
  REQUIRE(IsBoolean(&type::Bool::instance));
}

#define VISIT_TEST(c, arg) TEST_CASE("DefaultVisitor::visit(" #c ")") { \
    type::Factory factory;                                                \
    const c* type = arg;                                                \
    type::DefaultVisitor visitor;                                   \
    visitor.Visit(*type);                                              \
}

VISIT_TEST(type::Error, &type::Error::instance);
VISIT_TEST(type::Bool, &type::Bool::instance);
VISIT_TEST(type::Complex64, &type::Complex64::instance);
VISIT_TEST(type::Complex128, &type::Complex128::instance);
VISIT_TEST(type::Float32, &type::Float32::instance);
VISIT_TEST(type::Float64, &type::Float64::instance);
VISIT_TEST(type::Int, &type::Int::instance);
VISIT_TEST(type::Int8, &type::Int8::instance);
VISIT_TEST(type::Int16, &type::Int16::instance);
VISIT_TEST(type::Int32, &type::Int32::instance);
VISIT_TEST(type::Int64, &type::Int64::instance);
VISIT_TEST(type::String, &type::String::instance);
VISIT_TEST(type::Uint, &type::Uint::instance);
VISIT_TEST(type::Uint8, &type::Uint8::instance);
VISIT_TEST(type::Uint16, &type::Uint16::instance);
VISIT_TEST(type::Uint32, &type::Uint32::instance);
VISIT_TEST(type::Uint64, &type::Uint64::instance);
VISIT_TEST(type::Uintptr, &type::Uintptr::instance);
VISIT_TEST(type::Array, type::Uintptr::instance.GetArray(3));
VISIT_TEST(type::Function, factory.MakeFunction());
VISIT_TEST(type::Interface, factory.MakeInterface());
VISIT_TEST(type::Map, type::Uintptr::instance.GetMap(&type::Uintptr::instance));
VISIT_TEST(type::Pointer, type::Uint::instance.GetPointer());
VISIT_TEST(type::Slice, type::Uint::instance.GetSlice());
VISIT_TEST(type::Struct, factory.MakeStruct());
VISIT_TEST(type::DefinedType, factory.MakeDefinedType(&type::Uint::instance));
VISIT_TEST(type::Alias, factory.MakeAlias(&type::Uint::instance));

}  // namespace test
}  // namespace rcgo
