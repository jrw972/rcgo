#ifndef RCGO_SRC_TYPE_VISITOR_HPP
#define RCGO_SRC_TYPE_VISITOR_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"
#include "Type.hpp"

namespace rcgo {

  struct TypeVisitor {
    virtual ~TypeVisitor ();
    virtual void visit (const ErrorType& type) = 0;
    virtual void visit (const BoolType& type) = 0;
    virtual void visit (const Complex64Type& type) = 0;
    virtual void visit (const Complex128Type& type) = 0;
    virtual void visit (const Float32Type& type) = 0;
    virtual void visit (const Float64Type& type) = 0;
    virtual void visit (const IntType& type) = 0;
    virtual void visit (const Int8Type& type) = 0;
    virtual void visit (const Int16Type& type) = 0;
    virtual void visit (const Int32Type& type) = 0;
    virtual void visit (const Int64Type& type) = 0;
    virtual void visit (const StringType& type) = 0;
    virtual void visit (const UintType& type) = 0;
    virtual void visit (const Uint8Type& type) = 0;
    virtual void visit (const Uint16Type& type) = 0;
    virtual void visit (const Uint32Type& type) = 0;
    virtual void visit (const Uint64Type& type) = 0;
    virtual void visit (const UintptrType& type) = 0;
    virtual void visit (const ArrayType& type) = 0;
    virtual void visit (const FunctionType& type) = 0;
    virtual void visit (const InterfaceType& type) = 0;
    virtual void visit (const MapType& type) = 0;
    virtual void visit (const PointerType& type) = 0;
    virtual void visit (const SliceType& type) = 0;
    virtual void visit (const StructType& type) = 0;
    virtual void visit (const DefinedType& type) = 0;
    virtual void visit (const AliasType& type) = 0;
  };

  struct DefaultTypeVisitor : public TypeVisitor {
    DefaultTypeVisitor ();
    virtual ~DefaultTypeVisitor ();
    virtual void visit (const ErrorType& type);
    virtual void visit (const BoolType& type);
    virtual void visit (const Complex64Type& type);
    virtual void visit (const Complex128Type& type);
    virtual void visit (const Float32Type& type);
    virtual void visit (const Float64Type& type);
    virtual void visit (const IntType& type);
    virtual void visit (const Int8Type& type);
    virtual void visit (const Int16Type& type);
    virtual void visit (const Int32Type& type);
    virtual void visit (const Int64Type& type);
    virtual void visit (const StringType& type);
    virtual void visit (const UintType& type);
    virtual void visit (const Uint8Type& type);
    virtual void visit (const Uint16Type& type);
    virtual void visit (const Uint32Type& type);
    virtual void visit (const Uint64Type& type);
    virtual void visit (const UintptrType& type);
    virtual void visit (const ArrayType& type);
    virtual void visit (const FunctionType& type);
    virtual void visit (const InterfaceType& type);
    virtual void visit (const MapType& type);
    virtual void visit (const PointerType& type);
    virtual void visit (const SliceType& type);
    virtual void visit (const StructType& type);
    virtual void visit (const DefinedType& type);
    virtual void visit (const AliasType& type);
    virtual void default_action (const Type& type);
  };

  template<typename T>
  const T* type_cast (const Type* type)
  {
    if (type == NULL) { return NULL; }

    struct visitor : public DefaultTypeVisitor {
      const T* retval;
      visitor () : retval (NULL) { }
      void visit (const T& type) { retval = &type; }
    };

    visitor v;
    type->accept (v);
    return v.retval;
  }

}

#endif // RCGO_SRC_TYPE_VISITOR_HPP
