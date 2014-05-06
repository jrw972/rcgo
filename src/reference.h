#ifndef reference_h
#define reference_h

#include "abstract_value.h"

/* A reference describes the location of a value. */
typedef struct reference_t reference_t;
struct reference_t
{
  abstract_value_t value;
};

void reference_print (reference_t r);

reference_t reference_make (abstract_value_t value);

bool reference_assignable (reference_t target, abstract_value_t source);

#endif /* reference_h */
