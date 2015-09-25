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

        void visit (const bool_type_t& type)
        {
          std::cout << " value=" << tv.ref (type);
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

        void visit (const function_type_t& type)
        {
          std::cout << " value=<function>";
        }
      };
      visitor v (*this);
      type_strip (type)->accept (v);
    }
}
