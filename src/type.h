#ifndef SRC_TYPE_H_
#define SRC_TYPE_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <map>
#include <string>
#include <vector>

#include "src/symbol.h"

namespace rcgo {
namespace type {

struct Visitor;

struct Array;
struct Map;
struct Pointer;
struct Slice;

struct Factory;

struct Type {
  Type() : m_slice_type(NULL), m_pointer_type(NULL) {}
  virtual ~Type();
  virtual void Accept(Visitor* visitor) const = 0;
  virtual const Type* UnderlyingType() const { return this; }
  virtual bool IsDefined() const { return false; }
  virtual bool IsAlias() const { return false; }
  const Array* GetArray(int length) const;
  const Slice* GetSlice() const;
  const Map* GetMap(const Type* key_type) const;
  const Pointer* GetPointer() const;

 private:
  typedef std::map<int, Array*> ArrayTypes;
  ArrayTypes m_array_types;
  Slice* m_slice_type;
  typedef std::map<const Type*, Map*> MapTypes;
  MapTypes m_map_types;
  Pointer* m_pointer_type;
};

// This type indicates an error in type processing.
struct Error : public Type {
  void Accept(Visitor* visitor) const override;
  static const Error instance;
 private:
  Error() {}
  ~Error() {}
};

// Atoms

struct Bool : public Type {
  void Accept(Visitor* visitor) const override;
  static const Bool instance;
 private:
  Bool() {}
  ~Bool() {}
};

struct Complex64 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Complex64 instance;
 private:
  Complex64() {}
  ~Complex64() {}
};

struct Complex128 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Complex128 instance;
 private:
  Complex128() {}
  ~Complex128() {}
};

struct Float32 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Float32 instance;
 private:
  Float32() {}
  ~Float32() {}
};

struct Float64 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Float64 instance;
 private:
  Float64() {}
  ~Float64() {}
};

struct Int : public Type {
  void Accept(Visitor* visitor) const override;
  static const Int instance;
 private:
  Int() {}
  ~Int() {}
};

struct Int8 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Int8 instance;
 private:
  Int8() {}
  ~Int8() {}
};

struct Int16 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Int16 instance;
 private:
  Int16() {}
  ~Int16() {}
};

struct Int32 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Int32 instance;
 private:
  Int32() {}
  ~Int32() {}
};

struct Int64 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Int64 instance;
 private:
  Int64() {}
  ~Int64() {}
};

struct String : public Type {
  void Accept(Visitor* visitor) const override;
  static const String instance;
 private:
  String() {}
  ~String() {}
};

struct Uint : public Type {
  void Accept(Visitor* visitor) const override;
  static const Uint instance;
 private:
  Uint() {}
  ~Uint() {}
};

struct Uint8 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Uint8 instance;
 private:
  Uint8() {}
  ~Uint8() {}
};

struct Uint16 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Uint16 instance;
 private:
  Uint16() {}
  ~Uint16() {}
};

struct Uint32 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Uint32 instance;
 private:
  Uint32() {}
  ~Uint32() {}
};

struct Uint64 : public Type {
  void Accept(Visitor* visitor) const override;
  static const Uint64 instance;
 private:
  Uint64() {}
  ~Uint64() {}
};

struct Uintptr : public Type {
  void Accept(Visitor* visitor) const override;
  static const Uintptr instance;
 private:
  Uintptr() {}
  ~Uintptr() {}
};

// Compounds

struct Array : public Type {
  void Accept(Visitor* visitor) const override;

  int const length;
  const Type* const element_type;

 private:
  Array(int a_length, const Type* a_element_type)
      : length(a_length) , element_type(a_element_type) {}
  ~Array() {}
  friend struct Type;
};

struct Map : public Type {
  void Accept(Visitor* visitor) const override;

  const Type* const key_type;
  const Type* const value_type;

 private:
  Map(const Type* a_key_type, const Type* a_value_type)
      : key_type(a_key_type), value_type(a_value_type) {}
  ~Map() {}
  friend struct Type;
};

struct Pointer : public Type {
  void Accept(Visitor* visitor) const override;

  const Type* const base_type;

 private:
  explicit Pointer(const Type* a_base_type) : base_type(a_base_type) {}
  ~Pointer() {}
  friend struct Type;
};

struct Slice : public Type {
  void Accept(Visitor* visitor) const override;

  const Type* const element_type;

 private:
  explicit Slice(const Type* a_element_type) : element_type(a_element_type) {}
  ~Slice() {}
  friend struct Type;
};

struct Function : public Type, public symbol::Table {
  typedef std::vector<const symbol::Parameter*> ParameterList;
  typedef ParameterList::const_iterator const_parameter_iterator;

  void Accept(Visitor* visitor) const override;
  symbol::Parameter* AppendParameter(
      const std::string& a_identifier, const Location& a_location,
      const type::Type* a_type, bool a_is_variadic);
  size_t ParameterCount() const { return m_parameter_list.size(); }
  const_parameter_iterator ParameterBegin() const {
    return m_parameter_list.begin();
  }
  const_parameter_iterator ParameterEnd() const {
    return m_parameter_list.end();
  }
  const symbol::Parameter* ParameterAt(size_t idx) const {
    return m_parameter_list.at(idx);
  }
  symbol::Parameter* AppendResult(
      const std::string& a_identifier, const Location& a_location,
      const type::Type* a_type);
  size_t ResultCount() const { return m_result_list.size(); }
  const_parameter_iterator ResultBegin() const { return m_result_list.begin(); }
  const_parameter_iterator ResultEnd() const { return m_result_list.end(); }
  bool IsVariadic() const {
    return !m_parameter_list.empty() && m_parameter_list.back()->is_variadic;
  }

 private:
  explicit Function(const Package* a_package) : symbol::Table(a_package) {}
  ~Function() {}
  friend Factory;

  ParameterList m_parameter_list;
  ParameterList m_result_list;
};

struct Interface : public Type, public symbol::Table {
  typedef std::vector<const symbol::InterfaceMethod*> MethodList;
  typedef MethodList::const_iterator const_method_iterator;

  void Accept(Visitor* visitor) const override;
  symbol::InterfaceMethod* AppendMethod(
      const std::string& a_identifier, const Location& a_location,
      const type::Function* a_type);
  size_t MethodCount() const { return m_method_list.size(); }
  const_method_iterator MethodBegin() const { return m_method_list.begin(); }
  const_method_iterator MethodEnd() const { return m_method_list.end(); }
 private:
  explicit Interface(const Package* a_package) : symbol::Table(a_package) {}
  ~Interface() {}
  friend Factory;

  MethodList m_method_list;
};

struct Struct : public Type, public symbol::Table {
  typedef std::vector<const symbol::Field*> FieldList;
  typedef FieldList::const_iterator const_field_iterator;

  void Accept(Visitor* visitor) const override;
  symbol::Field* AppendField(
      const std::string& a_identifier, const Location& a_location,
      const type::Type* a_type, const std::string& a_tag, bool a_is_embedded);
  size_t FieldCount() const { return m_field_list.size(); }
  const_field_iterator FieldBegin() const { return m_field_list.begin(); }
  const_field_iterator FieldEnd() const { return m_field_list.end(); }
 private:
  explicit Struct(const Package* a_package) : symbol::Table(a_package) {}
  ~Struct() {}
  friend Factory;

  FieldList m_field_list;
};

// Named types and aliases
struct NamedType : public Type {
 protected:
  explicit NamedType(const Type* a_type) : m_type(a_type) {}

  const Type* const m_type;
};

struct DefinedType : public NamedType {
  void Accept(Visitor* visitor) const override;
  const Type* UnderlyingType() const override { return m_type; }
  bool IsDefined() const override { return true; }
 private:
  explicit DefinedType(const Type* a_type)
      : NamedType(a_type->UnderlyingType()) {}  // Paranoia
  ~DefinedType() {}

  friend Factory;
};

struct Alias : public NamedType {
  void Accept(Visitor* visitor) const override;
  const Type* UnderlyingType() const override {
    return m_type->UnderlyingType();
  }
  bool IsAlias() const override { return true; }
 private:
  explicit Alias(const Type* a_type) : NamedType(a_type) {}
  ~Alias() {}

  friend Factory;
};

struct Factory {
  ~Factory();
  Function* MakeFunction(const Package* a_package);
  Interface* MakeInterface(const Package* a_package);
  Struct* MakeStruct(const Package* a_package);
  DefinedType* MakeDefinedType(const Type* a_type);
  Alias* MakeAlias(const Type* a_type);

 private:
  typedef std::vector<Type*> ListType;
  ListType m_list;
};

struct Visitor {
  virtual ~Visitor() {}
  virtual void Visit(const Error& type) = 0;
  virtual void Visit(const Bool& type) = 0;
  virtual void Visit(const Complex64& type) = 0;
  virtual void Visit(const Complex128& type) = 0;
  virtual void Visit(const Float32& type) = 0;
  virtual void Visit(const Float64& type) = 0;
  virtual void Visit(const Int& type) = 0;
  virtual void Visit(const Int8& type) = 0;
  virtual void Visit(const Int16& type) = 0;
  virtual void Visit(const Int32& type) = 0;
  virtual void Visit(const Int64& type) = 0;
  virtual void Visit(const String& type) = 0;
  virtual void Visit(const Uint& type) = 0;
  virtual void Visit(const Uint8& type) = 0;
  virtual void Visit(const Uint16& type) = 0;
  virtual void Visit(const Uint32& type) = 0;
  virtual void Visit(const Uint64& type) = 0;
  virtual void Visit(const Uintptr& type) = 0;
  virtual void Visit(const Array& type) = 0;
  virtual void Visit(const Function& type) = 0;
  virtual void Visit(const Interface& type) = 0;
  virtual void Visit(const Map& type) = 0;
  virtual void Visit(const Pointer& type) = 0;
  virtual void Visit(const Slice& type) = 0;
  virtual void Visit(const Struct& type) = 0;
  virtual void Visit(const DefinedType& type) = 0;
  virtual void Visit(const Alias& type) = 0;
};

struct DefaultVisitor : public Visitor {
  void Visit(const Error& type) override { DefaultAction(type); }
  void Visit(const Bool& type) override { DefaultAction(type); }
  void Visit(const Complex64& type) override { DefaultAction(type); }
  void Visit(const Complex128& type) override { DefaultAction(type); }
  void Visit(const Float32& type) override { DefaultAction(type); }
  void Visit(const Float64& type) override { DefaultAction(type); }
  void Visit(const Int& type) override { DefaultAction(type); }
  void Visit(const Int8& type) override { DefaultAction(type); }
  void Visit(const Int16& type) override { DefaultAction(type); }
  void Visit(const Int32& type) override { DefaultAction(type); }
  void Visit(const Int64& type) override { DefaultAction(type); }
  void Visit(const String& type) override { DefaultAction(type); }
  void Visit(const Uint& type) override { DefaultAction(type); }
  void Visit(const Uint8& type) override { DefaultAction(type); }
  void Visit(const Uint16& type) override { DefaultAction(type); }
  void Visit(const Uint32& type) override { DefaultAction(type); }
  void Visit(const Uint64& type) override { DefaultAction(type); }
  void Visit(const Uintptr& type) override { DefaultAction(type); }
  void Visit(const Array& type) override { DefaultAction(type); }
  void Visit(const Function& type) override { DefaultAction(type); }
  void Visit(const Interface& type) override { DefaultAction(type); }
  void Visit(const Map& type) override { DefaultAction(type); }
  void Visit(const Pointer& type) override { DefaultAction(type); }
  void Visit(const Slice& type) override { DefaultAction(type); }
  void Visit(const Struct& type) override { DefaultAction(type); }
  void Visit(const DefinedType& type) override { DefaultAction(type); }
  void Visit(const Alias& type) override { DefaultAction(type); }
  virtual void DefaultAction(const Type& type) {}
};

template<typename T>
const T* Cast(const Type* type) {
  if (type == NULL) { return NULL; }

  struct Visitor : public DefaultVisitor {
    const T* retval;
    Visitor() : retval(NULL) { }
    void Visit(const T& type) { retval = &type; }
  };

  Visitor v;
  type->Accept(&v);
  return v.retval;
}

bool Identical(const Type* x, const Type* y);
bool Different(const Type* x, const Type* y);
bool IsComparable(const Type* x);
bool IsArithmetic(const Type* x);
bool IsSigned(const Type* x);
bool IsInteger(const Type* x);
bool IsString(const Type* x);
bool IsBoolean(const Type* x);
Type const * Choose(Type const * x, Type const * y);

}  // namespace type
}  // namespace rcgo

#endif  // SRC_TYPE_H_
