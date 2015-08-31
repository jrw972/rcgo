#include "value.hpp"

void
value_t::print (std::ostream& out, const type_t* type) const
{
  if (present)
    {
      struct visitor : public const_type_visitor_t
      {
        const value_t& tv;

        visitor (const value_t& v)
          : tv (v)
        { }

        void default_action (const type_t& type)
        {
          type_not_reached(type);
        }

        void visit (const int_type_t& type)
        {
          std::cout << " value=" << tv.ref (type);
        }

        void visit (const uint_type_t& type)
        {
          std::cout << " value=" << tv.ref (type);
        }

        void visit (const int8_type_t& type)
        {
          std::cout << " value=" << (int)tv.ref (type);
        }

        void visit (const uint8_type_t& type)
        {
          std::cout << " value=" << (int)tv.ref (type);
        }

        void visit (const uint32_type_t& type)
        {
          std::cout << " value=" << tv.ref (type);
        }

        void visit (const uint64_type_t& type)
        {
          std::cout << " value=" << tv.ref (type);
        }

        void visit (const float64_type_t& type)
        {
          std::cout << " value=" << tv.ref (type);
        }

        void visit (const nil_type_t& type)
        {
          std::cout << " value=" << (void*)NULL;
        }

        void visit (const string_type_t& type)
        {
          std::cout << " value=" << tv.ref (type);
        }
      };
      visitor v (*this);
      type_strip (type)->accept (v);
    }
}

int_type_t::ValueType
value_t::integral_value (const type_t* type) const
{
  assert (present);
  assert (type_is_integral (type));

  struct visitor : public const_type_visitor_t
  {
    const value_t& tv;
    int_type_t::ValueType value;

    visitor (const value_t& t) : tv (t) { }

    void default_action (const type_t& type)
    {
      type_not_reached (type);
    }

    void visit (const int_type_t& type)
    {
      value = tv.ref (type);
    }

    void visit (const uint_type_t& type)
    {
      value = tv.ref (type);
    }

    void visit (const uint8_type_t& type)
    {
      value = tv.ref (type);
    }
  };

  visitor v (*this);
  type_strip (type)->accept (v);
  return v.value;
}
