/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "TypeVisitor.hpp"
#include "Type.hpp"

#define VISIT(typ) void DefaultTypeVisitor::visit (const typ& type) { default_action (type); }

namespace rcgo {

  TypeVisitor::~TypeVisitor ()
  { }

  VISIT (ErrorType)
  VISIT (BoolType)
  VISIT (Complex64Type)
  VISIT (Complex128Type)
  VISIT (Float32Type)
  VISIT (Float64Type)
  VISIT (IntType)
  VISIT (Int8Type)
  VISIT (Int16Type)
  VISIT (Int32Type)
  VISIT (Int64Type)
  VISIT (StringType)
  VISIT (UintType)
  VISIT (Uint8Type)
  VISIT (Uint16Type)
  VISIT (Uint32Type)
  VISIT (Uint64Type)
  VISIT (UintptrType)
  VISIT (ArrayType)
  VISIT (FunctionType)
  VISIT (InterfaceType)
  VISIT (MapType)
  VISIT (PointerType)
  VISIT (SliceType)
  VISIT (StructType)
  VISIT (DefinedType)
  VISIT (AliasType)

  DefaultTypeVisitor::DefaultTypeVisitor ()
  { }

  DefaultTypeVisitor::~DefaultTypeVisitor ()
  { }

  void DefaultTypeVisitor::default_action (const Type& type)
  { }
}
