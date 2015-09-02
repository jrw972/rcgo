#ifndef initializer_hpp
#define initializer_hpp

#include "types.hpp"
#include "type.hpp"
#include "memory_model.hpp"

struct initializer_t
{
  initializer_t (ast_initializer_t* n,
                 const std::string& na,
                 const initializer_type_t* initializer_type_)
    : node (n)
    , name (na)
    , initializer_type (initializer_type_)
  { }

  ast_initializer_t* const node;
  std::string const name;
  const initializer_type_t * const initializer_type;
  memory_model_t memory_model;
};

#endif /* initializer_hpp */
