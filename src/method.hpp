#ifndef method_h
#define method_h

#include "types.hpp"
#include "strtab.hpp"
#include "type.hpp"

struct method_t
{
  method_t (ast_t* n,
            string_t na,
            const method_type_t* method_type_)
    : node (n)
    , name (na)
    , method_type (method_type_)
  { }

  ast_t* const node;
  string_t const name;
  const method_type_t * const method_type;
  memory_model_t memory_model;
};

#endif /* method_h */
