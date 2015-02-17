#ifndef parameter_h
#define parameter_h

#include "types.hpp"
#include "strtab.hpp"

parameter_t *parameter_make (string_t name, const type_t * type, bool is_receiver);

string_t parameter_name (const parameter_t * parameter);

const type_t *parameter_type (const parameter_t * parameter);

bool parameter_is_receiver (const parameter_t * parameter);

#endif /* parameter_h */
