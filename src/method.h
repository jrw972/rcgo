#ifndef method_h
#define method_h

#include "types.h"
#include "strtab.h"

method_t*
method_make (type_t * named_type,
             ast_t* node,
             string_t identifier, type_t * signature, type_t* return_type);

type_t*
method_named_type (const method_t* method);

ast_t*
method_node (const method_t* method);

type_t*
method_type (const method_t* method);

const type_t*
method_signature (const method_t* method);

type_t*
method_return_type (const method_t* method);

string_t
method_name (const method_t* method);

void method_set_locals_size (method_t* method,
                             size_t size);

size_t method_get_locals_size (const method_t * method);

#endif /* method_h */
