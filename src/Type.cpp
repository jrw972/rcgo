/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Type.hpp"
#include "TypeVisitor.hpp"
#include "Symbol.hpp"

#include <iostream>

#define ACCEPT(typ) void typ::accept (TypeVisitor& visitor) const { visitor.visit (*this); }

namespace rcgo {

  Type::Type ()
    : m_slice_type (NULL)
    , m_pointer_type (NULL)
  { }

  Type::~Type ()
  {
    for (ArrayTypes::const_iterator pos = m_array_types.begin (),
           limit = m_array_types.end ();
         pos != limit;
         ++pos)
      {
        delete pos->second;
      }

    if (m_slice_type != NULL)
      {
        delete m_slice_type;
      }

    for (MapTypes::const_iterator pos = m_map_types.begin (),
           limit = m_map_types.end ();
         pos != limit;
         ++pos)
      {
        delete pos->second;
      }

    if (m_pointer_type != NULL)
      {
        delete m_pointer_type;
      }
  }

  const Type* Type::underlying_type () const
  {
    return this;
  }

  bool Type::is_defined () const
  {
    return false;
  }

  const ArrayType* Type::get_array (int length) const
  {
    ArrayTypes::const_iterator pos = m_array_types.find (length);
    if (pos != m_array_types.end ())
      {
        return pos->second;
      }
    else
      {
        ArrayType* at = new ArrayType (length, this);
        const_cast<ArrayTypes&> (m_array_types).insert (std::make_pair (length, at));
        return at;
      }
  }

  const SliceType* Type::get_slice () const
  {
    if (m_slice_type == NULL)
      {
        const_cast<SliceType*&> (m_slice_type) = new SliceType (this);
      }
    return m_slice_type;
  }

  const MapType* Type::get_map (const Type* a_key_type) const
  {
    MapTypes::const_iterator pos = m_map_types.find (a_key_type);
    if (pos != m_map_types.end ())
      {
        return pos->second;
      }
    else
      {
        MapType* mt = new MapType (a_key_type, this);
        const_cast<MapTypes&> (m_map_types).insert (std::make_pair (a_key_type, mt));
        return mt;
      }
  }

  const PointerType* Type::get_pointer () const
  {
    if (m_pointer_type == NULL)
      {
        const_cast<PointerType*&> (m_pointer_type) = new PointerType (this);
      }
    return m_pointer_type;
  }

  ACCEPT (ErrorType);
  ErrorType const ErrorType::instance;
  ErrorType::ErrorType () { }

  ACCEPT (BoolType);
  BoolType const BoolType::instance;
  BoolType::BoolType () { }

  ACCEPT (Complex64Type);
  Complex64Type const Complex64Type::instance;
  Complex64Type::Complex64Type () { }

  ACCEPT (Complex128Type);
  Complex128Type const Complex128Type::instance;
  Complex128Type::Complex128Type () { }

  ACCEPT (Float32Type);
  Float32Type const Float32Type::instance;
  Float32Type::Float32Type () { }

  ACCEPT (Float64Type);
  Float64Type const Float64Type::instance;
  Float64Type::Float64Type () { }

  ACCEPT (IntType);
  IntType const IntType::instance;
  IntType::IntType () { }

  ACCEPT (Int8Type);
  Int8Type const Int8Type::instance;
  Int8Type::Int8Type () { }

  ACCEPT (Int16Type);
  Int16Type const Int16Type::instance;
  Int16Type::Int16Type () { }

  ACCEPT (Int32Type);
  Int32Type const Int32Type::instance;
  Int32Type::Int32Type () { }

  ACCEPT (Int64Type);
  Int64Type const Int64Type::instance;
  Int64Type::Int64Type () { }

  ACCEPT (StringType);
  StringType const StringType::instance;
  StringType::StringType () { }

  ACCEPT (UintType);
  UintType const UintType::instance;
  UintType::UintType () { }

  ACCEPT (Uint8Type);
  Uint8Type const Uint8Type::instance;
  Uint8Type::Uint8Type () { }

  ACCEPT (Uint16Type);
  Uint16Type const Uint16Type::instance;
  Uint16Type::Uint16Type () { }

  ACCEPT (Uint32Type);
  Uint32Type const Uint32Type::instance;
  Uint32Type::Uint32Type () { }

  ACCEPT (Uint64Type);
  Uint64Type const Uint64Type::instance;
  Uint64Type::Uint64Type () { }

  ACCEPT (UintptrType);
  UintptrType const UintptrType::instance;
  UintptrType::UintptrType () { }

  ArrayType::ArrayType (int a_length, const Type* a_element_type)
    : length (a_length)
    , element_type (a_element_type)
  { }

  ArrayType::~ArrayType ()
  { }

  ACCEPT (ArrayType);

  TypeFactory::~TypeFactory ()
  {
    for (ListType::const_iterator pos = m_list.begin (),
           limit = m_list.end ();
         pos != limit;
         ++pos)
      {
        delete *pos;
      }
  }

  FunctionType* TypeFactory::make_function_type ()
  {
    FunctionType* t = new FunctionType ();
    m_list.push_back (t);
    return t;
  }

  InterfaceType* TypeFactory::make_interface_type ()
  {
    InterfaceType* t = new InterfaceType ();
    m_list.push_back (t);
    return t;
  }

  StructType* TypeFactory::make_struct_type ()
  {
    StructType* t = new StructType ();
    m_list.push_back (t);
    return t;
  }

  DefinedType* TypeFactory::make_defined_type (const Type* a_type)
  {
    DefinedType* t = new DefinedType (a_type);
    m_list.push_back (t);
    return t;
  }

  AliasType* TypeFactory::make_alias_type (const Type* a_type)
  {
    AliasType* t = new AliasType (a_type);
    m_list.push_back (t);
    return t;
  }

  FunctionType::FunctionType ()
  { }

  FunctionType::~FunctionType ()
  { }

  ACCEPT (FunctionType);

  FunctionType* FunctionType::append_parameter (ParameterSymbol* parameter)
  {
    assert (!is_variadic ());
    insert (parameter);
    m_parameter_list.push_back (parameter);
    return this;
  }

  size_t FunctionType::parameter_count () const
  {
    return m_parameter_list.size ();
  }

  FunctionType::const_parameter_iterator FunctionType::parameter_begin () const
  {
    return m_parameter_list.begin ();
  }

  FunctionType::const_parameter_iterator FunctionType::parameter_end () const
  {
    return m_parameter_list.end ();
  }

  FunctionType* FunctionType::append_result (ParameterSymbol* parameter)
  {
    insert (parameter);
    m_result_list.push_back (parameter);
    return this;
  }

  size_t FunctionType::result_count () const
  {
    return m_result_list.size ();
  }

  FunctionType::const_parameter_iterator FunctionType::result_begin () const
  {
    return m_result_list.begin ();
  }

  FunctionType::const_parameter_iterator FunctionType::result_end () const
  {
    return m_result_list.end ();
  }

  bool FunctionType::is_variadic () const
  {
    return !m_parameter_list.empty () && m_parameter_list.back ()->is_variadic;
  }

  InterfaceType::InterfaceType ()
  { }

  InterfaceType::~InterfaceType ()
  { }

  ACCEPT (InterfaceType);

  InterfaceType* InterfaceType::append_method (InterfaceMethodSymbol* method)
  {
    insert (method);
    m_method_list.push_back (method);
    return this;
  }

  size_t InterfaceType::method_count () const
  {
    return m_method_list.size ();
  }

  InterfaceType::const_method_iterator InterfaceType::method_begin () const
  {
    return m_method_list.begin ();
  }

  InterfaceType::const_method_iterator InterfaceType::method_end () const
  {
    return m_method_list.end ();
  }

  MapType::MapType (const Type* a_key_type, const Type* a_value_type)
    : key_type (a_key_type)
    , value_type (a_value_type)
  { }

  MapType::~MapType ()
  { }

  ACCEPT (MapType);

  PointerType::PointerType (const Type* a_base_type)
    : base_type (a_base_type)
  { }

  PointerType::~PointerType ()
  { }

  ACCEPT (PointerType);

  SliceType::SliceType (const Type* a_element_type)
    : element_type (a_element_type)
  { }

  SliceType::~SliceType ()
  { }

  ACCEPT (SliceType);

  StructType::StructType ()
  { }

  StructType::~StructType ()
  { }

  ACCEPT (StructType);

  StructType* StructType::append_field (FieldSymbol* field)
  {
    insert (field);
    m_field_list.push_back (field);
    return this;
  }

  size_t StructType::field_count () const
  {
    return m_field_list.size ();
  }

  StructType::const_field_iterator StructType::field_begin () const
  {
    return m_field_list.begin ();
  }

  StructType::const_field_iterator StructType::field_end () const
  {
    return m_field_list.end ();
  }

  NamedType::NamedType (const Type* a_type)
    : m_type (a_type)
  { }

  DefinedType::DefinedType (const Type* a_type)
    : NamedType (a_type->underlying_type ()) // Paranoia
  { }

  DefinedType::~DefinedType ()
  { }

  ACCEPT (DefinedType);

  const Type* DefinedType::underlying_type () const
  {
    return m_type;
  }

  bool DefinedType::is_defined () const
  {
    return true;
  }

  AliasType::AliasType (const Type* a_type)
    : NamedType (a_type)
  { }

  AliasType::~AliasType ()
  { }

  ACCEPT (AliasType);

  const Type* AliasType::underlying_type () const
  {
    return m_type->underlying_type ();
  }

  bool identical (const Type* x, const Type* y)
  {
    if (x->is_defined () || y->is_defined ())
      {
        return x == y;
      }

    const Type* xu = x->underlying_type ();
    const Type* yu = y->underlying_type ();

    if (xu == yu)
      {
        return true;
      }

    struct Visitor : public DefaultTypeVisitor {
      const Type* const yuu;

      bool flag;

      Visitor (const Type* a_yu)
        : yuu (a_yu)
        , flag (false)
      { }

      virtual void visit (const ArrayType& type)
      {
        // Two array types are identical if they have identical
        // element types and the same array length.
        const ArrayType* xu = &type;
        const ArrayType* yu = type_cast<ArrayType> (yuu);
        if (yu == NULL)
          {
            return;
          }

        flag = identical (xu->element_type, yu->element_type) && xu->length == yu->length;
      }

      virtual void visit (const MapType& type)
      {
        // Two map types are identical if they have identical key and value types.
        const MapType* xu = &type;
        const MapType* yu = type_cast<MapType> (yuu);
        if (yu == NULL)
          {
            return;
          }

        flag = identical (xu->key_type, yu->key_type) && identical (xu->value_type, yu->value_type);
      }

      virtual void visit (const SliceType& type)
      {
        // Two slice types are identical if they have identical
        // element types.
        const SliceType* xu = &type;
        const SliceType* yu = type_cast<SliceType> (yuu);
        if (yu == NULL)
          {
            return;
          }

        flag = identical (xu->element_type, yu->element_type);
      }

      virtual void visit (const PointerType& type)
      {
        // Two pointer types are identical if they have identical base types.
        const PointerType* xu = &type;
        const PointerType* yu = type_cast<PointerType> (yuu);
        if (yu == NULL)
          {
            return;
          }

        flag = identical (xu->base_type, yu->base_type);
      }

      virtual void visit (const StructType& type)
      {
        // Two struct types are identical if they have the same
        // sequence of fields, and if corresponding fields have the
        // same names, and identical types, and identical
        // tags. Non-exported field names from different packages are
        // always different.
        const StructType* xu = &type;
        const StructType* yu = type_cast<StructType> (yuu);
        if (yu == NULL)
          {
            return;
          }
        if (xu->field_count () != yu->field_count ())
          {
            return;
          }
        for (StructType::const_field_iterator xpos = xu->field_begin (), ypos = yu->field_begin (), limit = xu->field_end (); xpos != limit; ++xpos, ++ypos)
          {
            const FieldSymbol* xfield = *xpos;
            const FieldSymbol* yfield = *ypos;
            if (xfield->identifier != yfield->identifier ||
                different (xfield->type, yfield->type) ||
                xfield->tag != yfield->tag)
              {
                return;
              }
            if (!xfield->exported &&
                xfield->package != yfield->package)
              {
                return;
              }
          }

        flag = true;
      }

      virtual void visit (const FunctionType& type)
      {
        // Two function types are identical if they have the same
        // number of parameters and result values, corresponding
        // parameter and result types are identical, and either both
        // functions are variadic or neither is. Parameter and result
        // names are not required to match.
        const FunctionType* xu = &type;
        const FunctionType* yu = type_cast<FunctionType> (yuu);
        if (yu == NULL)
          {
            return;
          }
        if (xu->parameter_count () != yu->parameter_count () ||
            xu->result_count () != yu->result_count () ||
            xu->is_variadic () != yu->is_variadic ())
          {
            return;
          }

        for (FunctionType::const_parameter_iterator xpos = xu->parameter_begin (), ypos = yu->parameter_begin (), limit = xu->parameter_end (); xpos != limit; ++xpos, ++ypos)
          {
            const ParameterSymbol* xparam = *xpos;
            const ParameterSymbol* yparam = *ypos;
            if (different (xparam->type, yparam->type))
              {
                return;
              }
          }

        for (FunctionType::const_parameter_iterator xpos = xu->result_begin (), ypos = yu->result_begin (), limit = xu->result_end (); xpos != limit; ++xpos, ++ypos)
          {
            const ParameterSymbol* xparam = *xpos;
            const ParameterSymbol* yparam = *ypos;
            if (different (xparam->type, yparam->type))
              {
                return;
              }
          }

        flag = true;
      }

      virtual void visit (const InterfaceType& type)
      {
        // Two interface types are identical if they have the same set
        // of methods with the same names and identical function
        // types. Non-exported method names from different packages
        // are always different. The order of the methods is
        // irrelevant.
        const InterfaceType* xu = &type;
        const InterfaceType* yu = type_cast<InterfaceType> (yuu);
        if (yu == NULL)
          {
            return;
          }
        if (xu->method_count () != yu->method_count ())
          {
            return;
          }
        for (InterfaceType::const_method_iterator xpos = xu->method_begin (), ypos = yu->method_begin (), limit = xu->method_end (); xpos != limit; ++xpos, ++ypos)
          {
            const InterfaceMethodSymbol* xmethod = *xpos;
            const InterfaceMethodSymbol* ymethod = *ypos;
            if (xmethod->identifier != ymethod->identifier ||
                different (xmethod->type, ymethod->type))
              {
                return;
              }
            if (!xmethod->exported &&
                xmethod->package != ymethod->package)
              {
                return;
              }
          }

        flag = true;
      }
    };

    Visitor visitor (yu);
    xu->accept (visitor);

    return visitor.flag;
  }

  bool different (const Type* x, const Type* y)
  {
    return !identical (x, y);
  }

  bool comparable (const Type* x)
  {
    struct Visitor : public DefaultTypeVisitor {
      bool flag;

      Visitor ()
        : flag (false)
      { }

      void visit (const BoolType&) { flag = true; }
      void visit (const Int8Type&) { flag = true; }
      void visit (const Int16Type&) { flag = true; }
      void visit (const Int32Type&) { flag = true; }
      void visit (const Int64Type&) { flag = true; }
      void visit (const Uint8Type&) { flag = true; }
      void visit (const Uint16Type&) { flag = true; }
      void visit (const Uint32Type&) { flag = true; }
      void visit (const Uint64Type&) { flag = true; }
      void visit (const IntType&) { flag = true; }
      void visit (const UintType&) { flag = true; }
      void visit (const UintptrType&) { flag = true; }
      void visit (const Float32Type&) { flag = true; }
      void visit (const Float64Type&) { flag = true; }
      void visit (const Complex64Type&) { flag = true; }
      void visit (const Complex128Type&) { flag = true; }
      void visit (const StringType&) { flag = true; }
      void visit (const PointerType&) { flag = true; }
      void visit (const InterfaceType&) { flag = true; }
      void visit (const StructType& type)
      {
        for (StructType::const_field_iterator pos = type.field_begin (), limit = type.field_end ();
             pos != limit;
             ++pos)
          {
            const FieldSymbol* field = *pos;
            if (!comparable (field->type))
              {
                return;
              }
          }
        flag = true;
      }
      void visit (const ArrayType& type)
      {
        flag = comparable (type.element_type);
      }
    };

    Visitor visitor;
    x->underlying_type ()->accept (visitor);
    return visitor.flag;
  }

  bool is_arithmetic (const Type* x)
  {
    struct Visitor : public DefaultTypeVisitor {
      bool flag;

      Visitor ()
        : flag (false)
      { }

      void visit (const Int8Type&) { flag = true; }
      void visit (const Int16Type&) { flag = true; }
      void visit (const Int32Type&) { flag = true; }
      void visit (const Int64Type&) { flag = true; }
      void visit (const Uint8Type&) { flag = true; }
      void visit (const Uint16Type&) { flag = true; }
      void visit (const Uint32Type&) { flag = true; }
      void visit (const Uint64Type&) { flag = true; }
      void visit (const IntType&) { flag = true; }
      void visit (const UintType&) { flag = true; }
      void visit (const UintptrType&) { flag = true; }
      void visit (const Float32Type&) { flag = true; }
      void visit (const Float64Type&) { flag = true; }
      void visit (const Complex64Type&) { flag = true; }
      void visit (const Complex128Type&) { flag = true; }
    };

    Visitor visitor;
    x->underlying_type ()->accept (visitor);
    return visitor.flag;
  }

  bool is_signed (const Type* x)
  {
    struct Visitor : public DefaultTypeVisitor {
      bool flag;

      Visitor ()
        : flag (false)
      { }

      void visit (const Int8Type&) { flag = true; }
      void visit (const Int16Type&) { flag = true; }
      void visit (const Int32Type&) { flag = true; }
      void visit (const Int64Type&) { flag = true; }
      void visit (const IntType&) { flag = true; }
      void visit (const Float32Type&) { flag = true; }
      void visit (const Float64Type&) { flag = true; }
      void visit (const Complex64Type&) { flag = true; }
      void visit (const Complex128Type&) { flag = true; }
    };

    Visitor visitor;
    x->underlying_type ()->accept (visitor);
    return visitor.flag;
  }

  bool is_integer (const Type* x)
  {
    struct Visitor : public DefaultTypeVisitor {
      bool flag;

      Visitor ()
        : flag (false)
      { }

      void visit (const Int8Type&) { flag = true; }
      void visit (const Int16Type&) { flag = true; }
      void visit (const Int32Type&) { flag = true; }
      void visit (const Int64Type&) { flag = true; }
      void visit (const Uint8Type&) { flag = true; }
      void visit (const Uint16Type&) { flag = true; }
      void visit (const Uint32Type&) { flag = true; }
      void visit (const Uint64Type&) { flag = true; }
      void visit (const IntType&) { flag = true; }
      void visit (const UintType&) { flag = true; }
      void visit (const UintptrType&) { flag = true; }
    };

    Visitor visitor;
    x->underlying_type ()->accept (visitor);
    return visitor.flag;
  }

  bool is_string (const Type* x)
  {
    return x->underlying_type () == &StringType::instance;
  }

  bool is_boolean (const Type* x)
  {
    return x->underlying_type () == &BoolType::instance;
  }
}
