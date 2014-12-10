#ifndef func_h
#define func_h

#include "types.h"
#include "strtab.h"

func_t*
func_make (type_t * type,
             ast_t* node,
             string_t identifier, type_t * signature, type_t* return_type);

type_t*
func_component_type (const func_t* func);

ast_t*
func_node (const func_t* func);

type_t*
func_type (const func_t* func);

const type_t*
func_signature (const func_t* func);

type_t*
func_return_type (const func_t* func);

string_t
func_name (const func_t* func);

void func_set_locals_size (func_t* func,
                             size_t size);

size_t func_get_locals_size (const func_t * func);

#endif /* func_h */
