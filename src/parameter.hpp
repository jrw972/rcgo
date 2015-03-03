#ifndef parameter_hpp
#define parameter_hpp

#include "types.hpp"
#include "strtab.hpp"
#include "typed_value.hpp"

struct parameter_t
{
  ast_t* const defining_node;
  string_t const name;
  const typed_value_t value;
  bool const is_receiver;

  parameter_t (ast_t* dn,
               string_t name_, const typed_value_t& tv, bool is_receiver_)
    : defining_node (dn)
    , name (name_)
    , value (tv)
    , is_receiver (is_receiver_)
  { }
};

#endif /* parameter_hpp */
