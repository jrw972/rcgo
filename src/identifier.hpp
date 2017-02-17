#ifndef RC_SRC_IDENTIFIER_HPP
#define RC_SRC_IDENTIFIER_HPP

#include <string>

#include "location.hpp"

namespace source
{

struct Identifier
{
  Identifier (const std::string& a_identifier,
              const Location& a_location)
    : identifier_ (a_identifier)
    , location_ (a_location)
  { }

  const std::string& identifier () const
  {
    return identifier_;
  }
  const Location& location () const
  {
    return location_;
  }
  bool is_blank () const
  {
    return identifier_ == "_";
  }
  bool is_exported () const
  {
    return !identifier_.empty () && isupper (identifier_[0]);
  }

  bool operator== (const Identifier& other) const
  {
    return this->identifier_ == other.identifier_ && this->location_ == other.location_;
  }

private:
  std::string identifier_;
  Location location_;
};

}

#endif // RC_SRC_IDENTIFIER_HPP
