#ifndef function_hpp
#define function_hpp

#include "types.hpp"
#include "strtab.hpp"
#include "type.hpp"
#include "memory_model.hpp"

struct function_t {
  function_t (ast_t* node_, string_t name_, const function_type_t* func_type_)
    : node (node_)
    , name (name_)
    , func_type (func_type_)
  { }

  ast_t* const node;
  string_t const name;
  const function_type_t* const func_type;
  memory_model_t memory_model;
};

#endif /* function_hpp */
