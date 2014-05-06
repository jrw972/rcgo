#ifndef type_h
#define type_h

#include "untyped_value.h"

typedef struct type_t type_t;

typedef enum
{ TypeUndefined,
  TypeBool
} TypeKind;

type_t *type_make_undefined (void);

type_t *type_make_bool (void);

TypeKind type_kind (const type_t * type);

bool type_can_represent (const type_t * type, untyped_value_t u);

bool type_assignable (const type_t * target, const type_t * source);

const type_t *type_logic_not (const type_t * type);

const type_t *type_logic_and (const type_t * x, const type_t * y);

const type_t *type_logic_or (const type_t * x, const type_t * y);

#endif /* type_h */
