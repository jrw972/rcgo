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
  { Undefined, Reference, Value } kind;
  union
  {
    reference_t reference;
    abstract_value_t value;
  };
};

const char* semval_to_string (semval_t s);

semval_t semval_undefined (void);

bool semval_is_undefined (semval_t s);

semval_t semval_make_reference (reference_t reference);

bool semval_is_reference (semval_t s);

semval_t semval_make_value (abstract_value_t v);

bool semval_is_value (semval_t s);

abstract_value_t semval_get_value (semval_t s);

semval_t semval_explicit_dereference (semval_t s);

semval_t semval_implicit_dereference (semval_t s);

semval_t semval_logic_not (semval_t s);

semval_t semval_logic_and (semval_t x, semval_t y);

semval_t semval_logic_or (semval_t x, semval_t y);

bool semval_assignable (semval_t left, semval_t right);

semval_t semval_select (semval_t s, string_t identifier);

bool semval_is_boolean (semval_t s);

bool semval_bindable (semval_t output, semval_t input);

action_t* semval_get_reaction (semval_t s);

typedef struct semval_list_t semval_list_t;

semval_list_t* semval_list_make (void);

void semval_list_append (semval_list_t* list,
                         semval_t semval);

bool semval_list_contains_undefined (const semval_list_t* list);

semval_t semval_call (semval_t expr, const semval_list_t* args);

select_result_t semval_selected_field (semval_t semval);

#endif /* semval_h */
