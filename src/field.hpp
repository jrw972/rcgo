#ifndef RC_SRC_FIELD_HPP
#define RC_SRC_FIELD_HPP

#include <set>

#include "types.hpp"

namespace type
{

struct Field
{
  const Struct* const m_struct;
  decl::Package* const package;
  bool const is_anonymous;
  std::string const name;
  const type::Type* const type;
  TagSet const tags;

  Field (const Struct* a_struct,
         decl::Package* a_package,
         bool a_is_anonymous,
         const std::string& a_name,
         const type::Type* a_type,
         const TagSet& a_tags)
    : m_struct (a_struct)
    , package (a_package)
    , is_anonymous (a_is_anonymous)
    , name (a_name)
    , type (a_type)
    , tags (a_tags)
  { }
};

}

#endif // RC_SRC_FIELD_HPP
