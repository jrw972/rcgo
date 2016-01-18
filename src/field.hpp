#ifndef RC_SRC_FIELD_HPP
#define RC_SRC_FIELD_HPP

#include "types.hpp"

namespace type
{

struct Field
{
  std::string const name;
  const type::Type* const type;
  ptrdiff_t const offset;

  Field (const std::string& n, const type::Type* t, ptrdiff_t o)
    : name (n)
    , type (t)
    , offset (o)
  { }
};

}

#endif // RC_SRC_FIELD_HPP
