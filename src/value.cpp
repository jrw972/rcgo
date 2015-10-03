#include "value.hpp"

void
value_t::print (std::ostream& out, const type_t* type) const
{
    if (present)
        {
            struct visitor : public const_type_visitor_t
            {
                const value_t& tv;
                std::ostream& out;

                visitor (const value_t& v, std::ostream& o)
                    : tv (v)
                    , out (o)
                { }

                void default_action (const type_t& type)
                {
                    type_not_reached(type);
                }

                void visit (const bool_type_t& type)
                {
                    out << " value=" << tv.ref (type);
                }

                void visit (const int_type_t& type)
                {
                    out << " value=" << tv.ref (type);
                }

                void visit (const uint_type_t& type)
                {
                    out << " value=" << tv.ref (type);
                }

                void visit (const int8_type_t& type)
                {
                    out << " value=" << (int)tv.ref (type);
                }

                void visit (const uint8_type_t& type)
                {
                    out << " value=" << (int)tv.ref (type);
                }

                void visit (const uint32_type_t& type)
                {
                    out << " value=" << tv.ref (type);
                }

                void visit (const uint64_type_t& type)
                {
                    out << " value=" << tv.ref (type);
                }

                void visit (const float64_type_t& type)
                {
                    out << " value=" << tv.ref (type);
                }

                void visit (const nil_type_t& type)
                {
                    out << " value=" << (void*)NULL;
                }

                void visit (const string_type_t& type)
                {
                    const string_type_t::ValueType& r = tv.ref (type);
                    out << " value=" << std::string (r.ptr, r.length);
                }

                void visit (const function_type_t& type)
                {
                    out << " value=<function>";
                }

                void visit (const method_type_t& type)
                {
                    out << " value=<method>";
                }

                void visit (const slice_type_t& type)
                {
                    const slice_type_t::ValueType& s = tv.ref (type);
                    out << " value={" << s.ptr << ',' << s.length << ',' << s.capacity << '}';
                }
            };
            visitor v (*this, out);
            type_strip (type)->accept (v);
        }
}
