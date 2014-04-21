#ifndef semval_h
#define semval_h

/* Semantic Value
 *
 * This type describes the meaning of various expressions in the language.
 */

#include <stdbool.h>
#include "type.h"

/*
  A typed value represents an actual value described by the type system.
  These have value semantics (pardon the pun).
 */
typedef struct typed_value_t typed_value_t;
struct typed_value_t
{
  type_t *type;
  union
  {
    bool bool_value;
  };
};

typed_value_t typed_value_make_bool (type_t * type, bool v);

/*
  An untyped value represents an actual value not described by the type system.
  These have value semantics (pardon the pun).
*/
typedef struct untyped_value_t untyped_value_t;
struct untyped_value_t
{
  enum
  { UntypedUndefined, UntypedBool } kind;
  union
  {
    bool bool_value;
  };
};

untyped_value_t untyped_value_make_bool (bool b);

/*
  An abstract value describes the result of an expression.
  If the expression is constant, then the abstract value will be either an untyped value or a typed value.
  If the expression is not constant, then the abstract value describes the resulting type.
*/
typedef struct abstract_value_t abstract_value_t;
struct abstract_value_t
{
  enum
  { UndefinedValue, UntypedValue, TypedValue, Typed } kind;
  union
  {
    untyped_value_t untyped_value;
    typed_value_t typed_value;
    type_t *typed;
  };
};

abstract_value_t abstract_value_make_untyped_value (untyped_value_t u);

typed_value_t abstract_value_get_typed_value (abstract_value_t a);

bool abstract_value_is_typed_value (abstract_value_t a);

/* A reference describes the location of a value. */
typedef struct reference_t reference_t;
struct reference_t
{
  abstract_value_t value;
};

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

semval_t semval_to_typed_value (semval_t s);

semval_t semval_logic_not (semval_t s);

#endif /* semval_h */
