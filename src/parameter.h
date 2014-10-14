#ifndef parameter_h
#define parameter_h

#include "types.h"
#include "strtab.h"

parameter_t* parameter_make (string_t name,
                             type_t* type);

string_t parameter_name (const parameter_t* parameter);

type_t* parameter_type (const parameter_t* parameter);

#endif /* parameter_h */
