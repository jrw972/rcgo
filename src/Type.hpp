#ifndef RCGO_SRC_TYPE_HPP
#define RCGO_SRC_TYPE_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "types.hpp"
#include "SymbolTable.hpp"

namespace rcgo {

  struct Type {
    Type ();
    virtual ~Type ();
    virtual void accept (TypeVisitor& visitor) const = 0;
    virtual const Type* underlying_type () const;
    virtual bool is_defined () const;
    const ArrayType* get_array (int length) const;
    const SliceType* get_slice () const;
    const MapType* get_map (const Type* key_type) const;
    const PointerType* get_pointer () const;

  private:
    typedef std::map<int, ArrayType*> ArrayTypes;
    ArrayTypes m_array_types;
    SliceType* m_slice_type;
    typedef std::map<const Type*, MapType*> MapTypes;
    MapTypes m_map_types;
    PointerType* m_pointer_type;
  };

  // This type indicates an error in type processing.
  struct ErrorType : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const ErrorType instance;
  private:
    ErrorType ();
  };

  // Atoms

  struct BoolType : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const BoolType instance;
  private:
    BoolType ();
  };

  struct Complex64Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Complex64Type instance;
  private:
    Complex64Type ();
  };

  struct Complex128Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Complex128Type instance;
  private:
    Complex128Type ();
  };

  struct Float32Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Float32Type instance;
  private:
    Float32Type ();
  };

  struct Float64Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Float64Type instance;
  private:
    Float64Type ();
  };

  struct IntType : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const IntType instance;
  private:
    IntType ();
  };

  struct Int8Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Int8Type instance;
  private:
    Int8Type ();
  };

  struct Int16Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Int16Type instance;
  private:
    Int16Type ();
  };

  struct Int32Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Int32Type instance;
  private:
    Int32Type ();
  };

  struct Int64Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Int64Type instance;
  private:
    Int64Type ();
  };

  struct StringType : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const StringType instance;
  private:
    StringType ();
  };

  struct UintType : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const UintType instance;
  private:
    UintType ();
  };

  struct Uint8Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Uint8Type instance;
  private:
    Uint8Type ();
  };

  struct Uint16Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Uint16Type instance;
  private:
    Uint16Type ();
  };

  struct Uint32Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Uint32Type instance;
  private:
    Uint32Type ();
  };

  struct Uint64Type : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const Uint64Type instance;
  private:
    Uint64Type ();
  };

  struct UintptrType : public Type {
    virtual void accept (TypeVisitor& visitor) const;
    static const UintptrType instance;
  private:
    UintptrType ();
  };

  // Compounds

  struct ArrayType : public Type {
    virtual void accept (TypeVisitor& visitor) const;

    int const length;
    const Type* const element_type;

  private:
    ArrayType (int a_length, const Type* a_element_type);
    ~ArrayType ();
    friend struct Type;
  };

  struct MapType : public Type {
    ~MapType ();
    virtual void accept (TypeVisitor& visitor) const;

    const Type* const key_type;
    const Type* const value_type;

  private:
    MapType (const Type* a_key_type, const Type* a_value_type);
    friend struct Type;
  };

  struct PointerType : public Type {
    ~PointerType ();
    virtual void accept (TypeVisitor& visitor) const;

    const Type* const base_type;

  private:
    PointerType (const Type* a_base_type);
    friend struct Type;
  };

  struct SliceType : public Type {
    ~SliceType ();
    virtual void accept (TypeVisitor& visitor) const;

    const Type* const element_type;

  private:
    SliceType (const Type* a_element_type);
    friend struct Type;
  };

  struct TypeFactory {
    ~TypeFactory ();
    FunctionType* make_function_type ();
    InterfaceType* make_interface_type ();
    StructType* make_struct_type ();
    DefinedType* make_defined_type (const Type* a_type);
    AliasType* make_alias_type (const Type* a_type);

  private:
    typedef std::vector<Type*> ListType;
    ListType m_list;
  };

  struct FunctionType : public Type, public SymbolTable {
    typedef std::vector<const ParameterSymbol*> ParameterList;
    typedef ParameterList::const_iterator const_parameter_iterator;

    ~FunctionType ();
    virtual void accept (TypeVisitor& visitor) const;
    FunctionType* append_parameter (ParameterSymbol* parameter);
    size_t parameter_count () const;
    const_parameter_iterator parameter_begin () const;
    const_parameter_iterator parameter_end () const;
    FunctionType* append_result (ParameterSymbol* parameter);
    size_t result_count () const;
    const_parameter_iterator result_begin () const;
    const_parameter_iterator result_end () const;
    bool is_variadic () const;
  private:
    FunctionType ();
    friend TypeFactory;

    ParameterList m_parameter_list;
    ParameterList m_result_list;
  };

  struct InterfaceType : public Type, public SymbolTable {
    typedef std::vector<const InterfaceMethodSymbol*> MethodList;
    typedef MethodList::const_iterator const_method_iterator;

    ~InterfaceType ();
    virtual void accept (TypeVisitor& visitor) const;
    InterfaceType* append_method (InterfaceMethodSymbol* method);
    size_t method_count () const;
    const_method_iterator method_begin () const;
    const_method_iterator method_end () const;
  private:
    InterfaceType ();
    friend TypeFactory;

    MethodList m_method_list;
  };

  struct StructType : public Type, public SymbolTable {
    typedef std::vector<const FieldSymbol*> FieldList;
    typedef FieldList::const_iterator const_field_iterator;

    ~StructType ();
    virtual void accept (TypeVisitor& visitor) const;
    StructType* append_field (FieldSymbol* field);
    size_t field_count () const;
    const_field_iterator field_begin () const;
    const_field_iterator field_end () const;
  private:
    StructType ();
    friend TypeFactory;

    FieldList m_field_list;
  };

  // Named types and aliases
  struct NamedType : public Type {
  protected:
    NamedType (const Type* a_type);
    const Type* const m_type;
  };

  struct DefinedType : public NamedType {
    ~DefinedType ();
    virtual void accept (TypeVisitor& visitor) const;
    const Type* underlying_type () const;
    bool is_defined () const;
  private:
    DefinedType (const Type* a_type);
    friend TypeFactory;
  };

  struct AliasType : public NamedType {
    ~AliasType ();
    virtual void accept (TypeVisitor& visitor) const;
    const Type* underlying_type () const;
  private:
    AliasType (const Type* a_type);
    friend TypeFactory;
  };

  bool identical (const Type* x, const Type* y);
  bool different (const Type* x, const Type* y);
  bool comparable (const Type* x);
  bool is_arithmetic (const Type* x);
  bool is_signed (const Type* x);
  bool is_integer (const Type* x);
  bool is_string (const Type* x);
  bool is_boolean (const Type* x);
}

#endif // RCGO_SRC_TYPE_HPP
