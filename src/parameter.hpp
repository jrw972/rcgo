#ifndef parameter_hpp
#define parameter_hpp

#include "types.hpp"
#include "typed_value.hpp"

struct parameter_t
{
  ast::Node* const defining_node;
  std::string const name;
  typed_value_t const value;
  bool const is_receiver;

  parameter_t (ast::Node* dn,
               const std::string& n, const typed_value_t& tv, bool is_receiver_)
    : defining_node (dn)
    , name (n)
    , value (tv)
    , is_receiver (is_receiver_)
  { }
};

#endif /* parameter_hpp */
