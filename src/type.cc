// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/type.h"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>

#include "src/symbol.h"

namespace rcgo {
namespace type {

Type::~Type() {
  for (ArrayTypes::value_type p : m_array_types) {
    delete p.second;
  }

  if (m_slice_type != NULL) {
    delete m_slice_type;
  }

  for (MapTypes::value_type p : m_map_types) {
    delete p.second;
  }

  if (m_pointer_type != NULL) {
    delete m_pointer_type;
  }
}

const Array* Type::GetArray(int length) const {
  ArrayTypes::const_iterator pos = m_array_types.find(length);
  if (pos != m_array_types.end()) {
    return pos->second;
  } else {
    Array* at = new Array(length, this);
    const_cast<ArrayTypes&>(m_array_types).insert(std::make_pair(length, at));
    return at;
  }
}

const Slice* Type::GetSlice() const {
  if (m_slice_type == NULL) {
    const_cast<Slice*&>(m_slice_type) = new Slice(this);
  }
  return m_slice_type;
}

const Map* Type::GetMap(const Type* a_key_type) const {
  MapTypes::const_iterator pos = m_map_types.find(a_key_type);
  if (pos != m_map_types.end()) {
    return pos->second;
  } else {
    Map* mt = new Map(a_key_type, this);
    const_cast<MapTypes&>(m_map_types).insert(std::make_pair(a_key_type, mt));
    return mt;
  }
}

const Pointer* Type::GetPointer() const {
  if (m_pointer_type == NULL) {
    const_cast<Pointer*&>(m_pointer_type) = new Pointer(this);
  }
  return m_pointer_type;
}

void Error::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Error const Error::instance;

void Bool::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Bool const Bool::instance;

void Complex64::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Complex64 const Complex64::instance;

void Complex128::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Complex128 const Complex128::instance;

void Float32::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Float32 const Float32::instance;

void Float64::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Float64 const Float64::instance;

void Int::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Int const Int::instance;

void Int8::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Int8 const Int8::instance;

void Int16::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Int16 const Int16::instance;

void Int32::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Int32 const Int32::instance;

void Int64::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Int64 const Int64::instance;

void String::Accept(Visitor* visitor) const { visitor->Visit(*this); }
String const String::instance;

void Uint::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Uint const Uint::instance;

void Uint8::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Uint8 const Uint8::instance;

void Uint16::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Uint16 const Uint16::instance;

void Uint32::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Uint32 const Uint32::instance;

void Uint64::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Uint64 const Uint64::instance;

void Uintptr::Accept(Visitor* visitor) const { visitor->Visit(*this); }
Uintptr const Uintptr::instance;

void Array::Accept(Visitor* visitor) const { visitor->Visit(*this); }

Factory::~Factory() {
  for (ListType::value_type type : m_list) {
    delete type;
  }
}

Function* Factory::MakeFunction(const Package* a_package) {
  Function* t = new Function(a_package);
  m_list.push_back(t);
  return t;
}

Interface* Factory::MakeInterface(const Package* a_package) {
  Interface* t = new Interface(a_package);
  m_list.push_back(t);
  return t;
}

Struct* Factory::MakeStruct(const Package* a_package) {
  Struct* t = new Struct(a_package);
  m_list.push_back(t);
  return t;
}

DefinedType* Factory::MakeDefinedType(const Type* a_type) {
  DefinedType* t = new DefinedType(a_type);
  m_list.push_back(t);
  return t;
}

Alias* Factory::MakeAlias(const Type* a_type) {
  Alias* t = new Alias(a_type);
  m_list.push_back(t);
  return t;
}

void Function::Accept(Visitor* visitor) const { visitor->Visit(*this); }

symbol::Parameter* Function::AppendParameter(
    const std::string& a_identifier, const Location& a_location,
    const type::Type* a_type, bool a_is_variadic) {
  symbol::Parameter* p = new symbol::Parameter(
      a_identifier, a_location, package, a_type, a_is_variadic);
  PushBack(p);
  if (!a_identifier.empty()) {
    Insert(p);
  }
  m_parameter_list.push_back(p);
  return p;
}

symbol::Parameter* Function::AppendResult(
    const std::string& a_identifier, const Location& a_location,
    const type::Type* a_type) {
  symbol::Parameter* p = new symbol::Parameter(
      a_identifier, a_location, package, a_type, false);
  PushBack(p);
  if (!a_identifier.empty()) {
    Insert(p);
  }
  m_result_list.push_back(p);
  return p;
}

void Interface::Accept(Visitor* visitor) const { visitor->Visit(*this); }

symbol::InterfaceMethod* Interface::AppendMethod(
    const std::string& a_identifier, const Location& a_location,
    const type::Function* a_type) {
  symbol::InterfaceMethod* im = new symbol::InterfaceMethod(a_identifier, a_location, package, a_type);
  PushBack(im);
  Insert(im);
  // TODO(jrw972): Do we need m_method_list?
  m_method_list.push_back(im);
  return im;
}

void Map::Accept(Visitor* visitor) const { visitor->Visit(*this); }

void Pointer::Accept(Visitor* visitor) const { visitor->Visit(*this); }

void Slice::Accept(Visitor* visitor) const { visitor->Visit(*this); }

void Struct::Accept(Visitor* visitor) const { visitor->Visit(*this); }

symbol::Field* Struct::AppendField(
      const std::string& a_identifier, const Location& a_location,
      const type::Type* a_type, const std::string& a_tag, bool a_is_embedded) {
  symbol::Field* f = new symbol::Field(a_identifier, a_location, package, a_type, a_tag, a_is_embedded);
  PushBack(f);
  Insert(f);
  // TODO(jrw972):  Do we need m_field_list?
  m_field_list.push_back(f);
  return f;
}

void DefinedType::Accept(Visitor* visitor) const { visitor->Visit(*this); }

void Alias::Accept(Visitor* visitor) const { visitor->Visit(*this); }

bool Identical(const Type* x, const Type* y) {
  if (x->IsDefined() || y->IsDefined()) {
    return x == y;
  }

  const Type* xu = x->UnderlyingType();
  const Type* yu = y->UnderlyingType();

  if (xu == yu) {
    return true;
  }

  struct Visitor : public DefaultVisitor {
    const Type* const yuu;

    bool flag;

    explicit Visitor(const Type* a_yu) : yuu(a_yu), flag(false) {}

    void Visit(const Array& type) override {
      // Two array types are identical if they have identical
      // element types and the same array length.
      const Array* xu = &type;
      const Array* yu = Cast<Array>(yuu);
      if (yu == NULL) {
        return;
      }

      flag = Identical(xu->element_type, yu->element_type) &&
          xu->length == yu->length;
    }

    void Visit(const Map& type) override {
      // Two map types are identical if they have identical key and value types.
      const Map* xu = &type;
      const Map* yu = Cast<Map>(yuu);
      if (yu == NULL) {
        return;
      }

      flag = Identical(xu->key_type, yu->key_type) &&
          Identical(xu->value_type, yu->value_type);
    }

    void Visit(const Slice& type) override {
      // Two slice types are identical if they have identical
      // element types.
      const Slice* xu = &type;
      const Slice* yu = Cast<Slice>(yuu);
      if (yu == NULL) {
        return;
      }

      flag = Identical(xu->element_type, yu->element_type);
    }

    void Visit(const Pointer& type) override {
      // Two pointer types are identical if they have identical base types.
      const Pointer* xu = &type;
      const Pointer* yu = Cast<Pointer>(yuu);
      if (yu == NULL) {
        return;
      }

      flag = Identical(xu->base_type, yu->base_type);
    }

    void Visit(const Struct& type) override {
      // Two struct types are identical if they have the same
      // sequence of fields, and if corresponding fields have the
      // same names, and identical types, and identical
      // tags. Non-exported field names from different packages are
      // always different.
      const Struct* xu = &type;
      const Struct* yu = Cast<Struct>(yuu);
      if (yu == NULL) {
        return;
      }
      if (xu->FieldCount() != yu->FieldCount()) {
        return;
      }
      for (Struct::const_field_iterator xpos = xu->FieldBegin(),
               ypos = yu->FieldBegin(), limit = xu->FieldEnd(); xpos != limit;
           ++xpos, ++ypos) {
        const symbol::Field* xfield = *xpos;
        const symbol::Field* yfield = *ypos;
        if (xfield->identifier != yfield->identifier ||
            Different(xfield->type, yfield->type) ||
            xfield->tag != yfield->tag) {
          return;
        }
        if (!xfield->exported &&
            xfield->package != yfield->package) {
          return;
        }
      }

      flag = true;
    }

    void Visit(const Function& type) override {
      // Two function types are identical if they have the same
      // number of parameters and result values, corresponding
      // parameter and result types are identical, and either both
      // functions are variadic or neither is. Parameter and result
      // names are not required to match.
      const Function* xu = &type;
      const Function* yu = Cast<Function>(yuu);
      if (yu == NULL) {
        return;
      }
      if (xu->ParameterCount() != yu->ParameterCount() ||
          xu->ResultCount() != yu->ResultCount() ||
          xu->IsVariadic() != yu->IsVariadic()) {
        return;
      }

      for (Function::const_parameter_iterator xpos = xu->ParameterBegin(),
               ypos = yu->ParameterBegin(), limit = xu->ParameterEnd();
           xpos != limit; ++xpos, ++ypos) {
        const symbol::Parameter* xparam = *xpos;
        const symbol::Parameter* yparam = *ypos;
        if (Different(xparam->type, yparam->type)) {
          return;
        }
      }

      for (Function::const_parameter_iterator xpos = xu->ResultBegin(),
               ypos = yu->ResultBegin(), limit = xu->ResultEnd();
           xpos != limit; ++xpos, ++ypos) {
        const symbol::Parameter* xparam = *xpos;
        const symbol::Parameter* yparam = *ypos;
        if (Different(xparam->type, yparam->type)) {
          return;
        }
      }

      flag = true;
    }

    void Visit(const Interface& type) override {
      // Two interface types are identical if they have the same set
      // of methods with the same names and identical function
      // types. Non-exported method names from different packages
      // are always different. The order of the methods is
      // irrelevant.
      const Interface* xu = &type;
      const Interface* yu = Cast<Interface>(yuu);
      if (yu == NULL) {
        return;
      }
      if (xu->MethodCount() != yu->MethodCount()) {
        return;
      }
      for (Interface::const_method_iterator xpos = xu->MethodBegin(),
               ypos = yu->MethodBegin(), limit = xu->MethodEnd();
           xpos != limit; ++xpos, ++ypos) {
        const symbol::InterfaceMethod* xmethod = *xpos;
        const symbol::InterfaceMethod* ymethod = *ypos;
        if (xmethod->identifier != ymethod->identifier ||
            Different(xmethod->type, ymethod->type)) {
          return;
        }
        if (!xmethod->exported &&
            xmethod->package != ymethod->package) {
          return;
        }
      }

      flag = true;
    }
  };

  Visitor visitor(yu);
  xu->Accept(&visitor);

  return visitor.flag;
}

bool Different(const Type* x, const Type* y) {
  return !Identical(x, y);
}

bool Comparable(const Type* x) {
  struct Visitor : public DefaultVisitor {
    bool flag;

    Visitor() : flag(false) {}

    void Visit(const Bool&) override { flag = true; }
    void Visit(const Int8&) override { flag = true; }
    void Visit(const Int16&) override { flag = true; }
    void Visit(const Int32&) override { flag = true; }
    void Visit(const Int64&) override { flag = true; }
    void Visit(const Uint8&) override { flag = true; }
    void Visit(const Uint16&) override { flag = true; }
    void Visit(const Uint32&) override { flag = true; }
    void Visit(const Uint64&) override { flag = true; }
    void Visit(const Int&) override { flag = true; }
    void Visit(const Uint&) override { flag = true; }
    void Visit(const Uintptr&) override { flag = true; }
    void Visit(const Float32&) override { flag = true; }
    void Visit(const Float64&) override { flag = true; }
    void Visit(const Complex64&) override { flag = true; }
    void Visit(const Complex128&) override { flag = true; }
    void Visit(const String&) override { flag = true; }
    void Visit(const Pointer&) override { flag = true; }
    void Visit(const Interface&) override { flag = true; }
    void Visit(const Struct& type) override {
      for (Struct::const_field_iterator pos = type.FieldBegin(),
               limit = type.FieldEnd();
           pos != limit; ++pos) {
        const symbol::Field* field = *pos;
        if (!Comparable(field->type)) {
          return;
        }
      }
      flag = true;
    }
    void Visit(const Array& type) override {
      flag = Comparable(type.element_type);
    }
  };

  Visitor visitor;
  x->UnderlyingType()->Accept(&visitor);
  return visitor.flag;
}

bool IsArithmetic(const Type* x) {
  struct Visitor : public DefaultVisitor {
    bool flag;

    Visitor() : flag(false) {}

    void Visit(const Int8&) override { flag = true; }
    void Visit(const Int16&) override { flag = true; }
    void Visit(const Int32&) override { flag = true; }
    void Visit(const Int64&) override { flag = true; }
    void Visit(const Uint8&) override { flag = true; }
    void Visit(const Uint16&) override { flag = true; }
    void Visit(const Uint32&) override { flag = true; }
    void Visit(const Uint64&) override { flag = true; }
    void Visit(const Int&) override { flag = true; }
    void Visit(const Uint&) override { flag = true; }
    void Visit(const Uintptr&) override { flag = true; }
    void Visit(const Float32&) override { flag = true; }
    void Visit(const Float64&) override { flag = true; }
    void Visit(const Complex64&) override { flag = true; }
    void Visit(const Complex128&) override { flag = true; }
  };

  Visitor visitor;
  x->UnderlyingType()->Accept(&visitor);
  return visitor.flag;
}

bool IsSigned(const Type* x) {
  struct Visitor : public DefaultVisitor {
    bool flag;

    Visitor() : flag(false) {}

    void Visit(const Int8&) override { flag = true; }
    void Visit(const Int16&) override { flag = true; }
    void Visit(const Int32&) override { flag = true; }
    void Visit(const Int64&) override { flag = true; }
    void Visit(const Int&) override { flag = true; }
    void Visit(const Float32&) override { flag = true; }
    void Visit(const Float64&) override { flag = true; }
    void Visit(const Complex64&) override { flag = true; }
    void Visit(const Complex128&) override { flag = true; }
  };

  Visitor visitor;
  x->UnderlyingType()->Accept(&visitor);
  return visitor.flag;
}

bool IsInteger(const Type* x) {
  struct Visitor : public DefaultVisitor {
    bool flag;

    Visitor() : flag(false) {}

    void Visit(const Int8&) override { flag = true; }
    void Visit(const Int16&) override { flag = true; }
    void Visit(const Int32&) override { flag = true; }
    void Visit(const Int64&) override { flag = true; }
    void Visit(const Uint8&) override { flag = true; }
    void Visit(const Uint16&) override { flag = true; }
    void Visit(const Uint32&) override { flag = true; }
    void Visit(const Uint64&) override { flag = true; }
    void Visit(const Int&) override { flag = true; }
    void Visit(const Uint&) override { flag = true; }
    void Visit(const Uintptr&) override { flag = true; }
  };

  Visitor visitor;
  x->UnderlyingType()->Accept(&visitor);
  return visitor.flag;
}

bool IsString(const Type* x) {
  return x->UnderlyingType() == &String::instance;
}

bool IsBoolean(const Type* x) {
  return x->UnderlyingType() == &Bool::instance;
}

}  // namespace type
}  // namespace rcgo
