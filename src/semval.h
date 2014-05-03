#ifndef semval_h
#define semval_h

#include "reference.h"

/* Semantic Value
 *
 * This type describes the meaning of various expressions in the language.
 */

typedef struct semval_t semval_t;
struct semval_t
{
  enum
    { Undefined, Reference, Value, Type } kind;
  union
  {
    reference_t reference;
    abstract_value_t value;
    type_t *type;
  };
};

void
semval_print (semval_t s);

semval_t semval_undefined (void);

bool semval_is_undefined (semval_t s);

semval_t semval_make_reference (semval_t s);

bool semval_is_reference (semval_t s);

semval_t semval_make_value (abstract_value_t v);

bool semval_is_value (semval_t s);

abstract_value_t semval_get_value (semval_t s);

semval_t semval_make_type (type_t * type);

bool semval_is_type (semval_t s);

type_t *semval_get_type (semval_t s);

semval_t semval_dereference (semval_t s);

semval_t semval_logic_not (semval_t s);

semval_t semval_logic_and (semval_t x,
                           semval_t y);

semval_t semval_logic_or (semval_t x,
                          semval_t y);

bool semval_assignable (semval_t left,
                        semval_t right);

#endif /* semval_h */
