#ifndef function_hpp
#define function_hpp

#include "types.hpp"
#include "strtab.hpp"
#include "type.hpp"

struct function_t {
  function_t (ast_t* node_, string_t name_, const function_type_t* func_type_)
    : node (node_)
    , name (name_)
    , func_type (func_type_)
    , locals_size (0)
  { }

  ast_t* const node;
  string_t const name;
  const function_type_t* const func_type;
  size_t locals_size;
};

#endif /* function_hpp */
