#ifndef method_h
#define method_h

#include "types.hpp"
#include "type.hpp"

struct method_t
{
  method_t (ast_method_t* n,
            const std::string& na,
            const method_type_t* method_type_,
            const symbol_t* return_symbol_)
    : node (n)
    , name (na)
    , method_type (method_type_)
    , return_symbol (return_symbol_)
  { }

  ast_method_t* const node;
  std::string const name;
  const method_type_t * const method_type;
  const symbol_t* const return_symbol;
  memory_model_t memory_model;
};

#endif /* method_h */
