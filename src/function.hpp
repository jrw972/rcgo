#ifndef function_hpp
#define function_hpp

#include "types.hpp"
#include "strtab.hpp"
#include "type.hpp"
#include "memory_model.hpp"

struct function_t {
  function_t (ast_t* node_, string_t name_, const function_type_t* func_type_, const symbol_t* return_symbol_)
    : node (node_)
    , name (name_)
    , function_type (func_type_)
    , return_symbol (return_symbol_)
  { }

  ast_t* const node;
  string_t const name;
  const function_type_t* const function_type;
  const symbol_t* const return_symbol;
  memory_model_t memory_model;
};

#endif /* function_hpp */
