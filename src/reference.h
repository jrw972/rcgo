#ifndef reference_h
#define reference_h

#include "abstract_value.h"

/* A reference describes the location of a value. */
typedef struct reference_t reference_t;
struct reference_t
{
  abstract_value_t value;
  bool mutable;
};

void reference_print (reference_t r);

reference_t reference_make (abstract_value_t value,
                            bool mutable);

abstract_value_t reference_value (reference_t r);

bool reference_mutable (reference_t r);

bool reference_assignable (reference_t target, abstract_value_t source);

reference_t reference_select (reference_t r, string_t identifier);

#endif /* reference_h */
