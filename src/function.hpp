#ifndef function_h
#define function_h

#include "types.hpp"
#include "strtab.hpp"
#include "type.hpp"

function_t* function_make (ast_t* node,
                           string_t identifier,
                           const signature_type_t* signature,
                           const type_t* return_type);

type_t* function_type (const function_t* function);

const signature_type_t* function_signature (const function_t* function);

const type_t* function_return_type (const function_t* function);

void function_set_locals_size (function_t* function,
                               size_t size);

size_t function_get_locals_size (const function_t * function);

#endif /* function_h */
