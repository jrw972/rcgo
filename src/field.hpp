#ifndef RC_SRC_FIELD_HPP
#define RC_SRC_FIELD_HPP

#include <set>

#include "types.hpp"

namespace type
{

struct Field
{
  decl::Package* const package;
  bool const is_anonymous;
  std::string const name;
  const type::Type* const type;
  TagSet const tags;
  ptrdiff_t const offset;

  Field (decl::Package* p, bool a, const std::string& n, const type::Type* t, const TagSet& tgs, ptrdiff_t o)
    : package (p)
    , is_anonymous (a)
    , name (n)
    , type (t)
    , tags (tgs)
    , offset (o)
  { }
};

}

#endif // RC_SRC_FIELD_HPP
