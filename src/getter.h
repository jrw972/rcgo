#ifndef getter_h
#define getter_h

#include "types.h"
#include "strtab.h"

getter_t*
getter_make (type_t * type,
             ast_t* node,
             string_t identifier, type_t * signature, type_t* return_type);

type_t*
getter_component_type (const getter_t* getter);

ast_t*
getter_node (const getter_t* getter);

type_t*
getter_type (const getter_t* getter);

const type_t*
getter_signature (const getter_t* getter);

type_t*
getter_return_type (const getter_t* getter);

string_t
getter_name (const getter_t* getter);

#endif /* getter_h */
