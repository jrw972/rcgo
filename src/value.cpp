#include "value.hpp"

using namespace Type;

bool
value_t::representable (const Type::Type* from, const Type::Type* to) const
{
  assert (present);

  struct visitor : public Type::DefaultVisitor
  {
    const Type::Type* to;
    bool flag;

    visitor (const Type::Type* t) : to (t), flag (false) { }

    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }

    void visit (const Type::String& type)
    {
      if (type_cast<Type::StringU> (to->UnderlyingType ()))
        {
          flag = true;
        }
    }
  } v (to);

  from->Accept (v);
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

    void visit (const Type::String& type)
    {
      if (type_cast<Type::StringU> (to->UnderlyingType ()))
        {
          value.stringu_value_ = value.string_value_;
          return;
        }

      not_reached;
    }
  } v (*this, to);

  from->Accept (v);
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

        void visit (const Enum& type)
        {
          out << " value=" << tv.ref (type);
        }

        void visit (const Bool& type)
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
