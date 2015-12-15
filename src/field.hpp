#ifndef rc_src_field_hpp
#define rc_src_field_hpp

#include "types.hpp"

namespace type
{

struct field_t
{
  std::string const name;
  const type::Type* const type;
  ptrdiff_t const offset;

  field_t (const std::string& n, const type::Type* t, ptrdiff_t o)
    : name (n)
    , type (t)
    , offset (o)
  { }
};

}

#endif // rc_src_field_hpp
