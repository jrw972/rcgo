#ifndef getter_hpp
#define getter_hpp

#include "types.hpp"
#include "type.hpp"
#include "memory_model.hpp"

struct getter_t
{
  getter_t (ast_getter_t* n,
            const std::string& na,
            const getter_type_t* getter_type_,
            const Symbol* return_symbol_)
    : node (n)
    , name (na)
    , getter_type (getter_type_)
    , return_symbol (return_symbol_)
  { }

  ast_getter_t* const node;
  std::string const name;
  const getter_type_t * const getter_type;
  const Symbol* const return_symbol;
  memory_model_t memory_model;
};

#endif /* getter_hpp */
