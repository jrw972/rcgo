#include "value.hpp"

#include "Type.hpp"

using namespace Type;

struct alpha_visitor : public Type::DefaultVisitor
{
  bool flag;
  Complex::ValueType x;

  alpha_visitor (Complex::ValueType z) : flag (false), x (z) { }

  void default_action (const Type::Type& type)
  {
    type_not_reached (type);
  }

  template <typename T>
  void doit (const T& type)
  {
    typename T::ValueType y;
    y = x;
    Complex::ValueType z;
    z = y;
    flag = (x == z);
  }

  void visit (const Uint& type)
  {
    doit (type);
  }

  void visit (const Int& type)
  {
    doit (type);
  }

  void visit (const Integer& type)
  {
    doit (type);
  }
};

static bool to_and_back (Complex::ValueType x, const Type::Type* type)
{
  alpha_visitor v (x);
  type->Accept (v);
  return v.flag;
}

static void convert_numeric (value_t& value, Complex::ValueType x, const Type::Type* type)
{
  struct visitor : public Type::DefaultVisitor
  {
    value_t& value;
    Complex::ValueType x;

    visitor (value_t& v, Complex::ValueType z) : value (v), x (z) { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Uint& type)
    {
      value.ref (type) = x;
    }

    void visit (const Int& type)
    {
      value.ref (type) = x;
    }

    void visit (const Integer& type)
    {
      value.ref (type) = x;
    }
  };

  visitor v (value, x);
  type->Accept (v);
}

bool
value_t::representable (const Type::Type* from, const Type::Type* to) const
{
  assert (present);

  struct visitor : public Type::DefaultVisitor
  {
    const value_t& value;
    const Type::Type* to;
    bool flag;

    visitor (const value_t& v, const Type::Type* t) : value (v), to (t), flag (false) { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Type::Int& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      flag = to_and_back (x, to->UnderlyingType ());
    }

    void visit (const Type::Boolean& type)
    {
      flag =
        type_cast<Type::Boolean> (to) != NULL ||
        type_cast<Type::Bool> (to) != NULL;
    }

    void visit (const Type::Rune& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      flag = to_and_back (x, to->UnderlyingType ());
    }

    void visit (const Type::Integer& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      flag = to_and_back (x, to->UnderlyingType ());
    }

    void visit (const Type::String& type)
    {
      if (Type::type_cast<Type::StringU> (to->UnderlyingType ()))
        {
          flag = true;
        }
    }
  } v (*this, to->UnderlyingType ());
  from->UnderlyingType ()->Accept (v);
  return v.flag;
}

void
value_t::convert (const Type::Type* from, const Type::Type* to)
{
  assert (present);

  struct visitor : public Type::DefaultVisitor
  {
    value_t& value;
    const Type::Type* to;

    visitor (value_t& v, const Type::Type* t) : value (v), to (t) { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Type::Boolean& type)
    {
      if (type_cast<Type::Bool> (to) != NULL)
        {
          value.ref (*Type::Bool::Instance ()) = value.ref (type);
        }
    }

    void visit (const Type::Rune& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Integer& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::Int& type)
    {
      Complex::ValueType x;
      x = value.ref (type);
      convert_numeric (value, x, to);
    }

    void visit (const Type::String& type)
    {
      if (Type::type_cast<Type::StringU> (to))
        {
          value.stringu_value_ = value.string_value_;
          return;
        }

      not_reached;
    }

    void visit (const Type::Nil& type)
    {
      if (Type::type_cast<Type::Pointer> (to))
        {
          value.pointer_value_ = NULL;
          return;
        }
      not_reached;
    }

  } v (*this, to->UnderlyingType ());

  from->UnderlyingType ()->Accept (v);
}

void
value_t::print (std::ostream& out, const Type::Type* type) const
{
  if (present)
    {
      struct visitor : public Type::DefaultVisitor
      {
        const value_t& tv;
        std::ostream& out;

        visitor (const value_t& v, std::ostream& o)
          : tv (v)
          , out (o)
        { }

        void default_action (const Type::Type& type)
        {
          type_not_reached(type);
        }

        void visit (const Type::Enum& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Type::Bool& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Int& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Int8& type)
        {
          out << " value=" << (int)tv.ref (type);
        }

        void visit (const Uint& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Uint8& type)
        {
          out << " value=" << (int)tv.ref (type);
        }

        void visit (const Uint16& type)
        {
          out << " value=" << (int)tv.ref (type);
        }

        void visit (const Uint32& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Uint64& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Float64& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Nil& type)
        {
          out << " value=" << (void*)NULL;
        }

        void visit (const Type::Function& type)
        {
          out << " value=<function>";
        }

        void visit (const Type::Method& type)
        {
          out << " value=<method>";
        }

        void visit (const Slice& type)
        {
          const Slice::ValueType& s = tv.ref (type);
          out << " value={" << s.ptr << ',' << s.length << ',' << s.capacity << '}';
        }
        void visit (const StringU& type)
        {
          const StringU::ValueType& s = tv.ref (type);
          out << " value={" << s.ptr << ',' << s.length << '}';
        }

        void visit (const Pointer& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Boolean& type)
        {
          out << " value=" << tv.ref (type);
        }
        void visit (const Integer& type)
        {
          out << " value=" << tv.ref (type);
        }
        void visit (const Float& type)
        {
          out << " value=" << tv.ref (type);
        }
        void visit (const String& type)
        {
          const String::ValueType& s = tv.ref (type);
          out << " value={" << s.ptr << ',' << s.length << '}';
        }
        void visit (const Type::Template& type)
        {
          out << " value=<template>";
        }
      };
      visitor v (*this, out);
      type_strip (type)->Accept (v);
    }
}
