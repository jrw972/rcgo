#ifndef function_hpp
#define function_hpp

#include "types.hpp"
#include "type.hpp"
#include "memory_model.hpp"

struct function_t
{
  function_t (ast_function_t* node_,
              const std::string& name_,
              const function_type_t* func_type_,
              const Symbol* return_symbol_)
    : node (node_)
    , name (name_)
    , function_type (func_type_)
    , return_symbol (return_symbol_)
  { }

  ast_function_t* const node;
  std::string const name;
  const function_type_t* const function_type;
  const Symbol* const return_symbol;
  memory_model_t memory_model;
};

#endif /* function_hpp */
