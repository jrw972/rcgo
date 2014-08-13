#ifndef abstract_value_h
#define abstract_value_h

#include "typed_value.h"
#include "untyped_value.h"

/*
  An abstract value describes the result of an expression.
  If the expression is constant, then the abstract value will be either an untyped value or a typed value.
  If the expression is not constant, then the abstract value describes the resulting type.
*/
typedef struct abstract_value_t abstract_value_t;

/* The kind values are important for conversion.  See abstract_value_homogenize. */
typedef enum
{
  UntypedValue = 0, TypedValue = 1, Typed = 2, UndefinedValue = 3
} AbstractValueKind;

struct abstract_value_t
{
  AbstractValueKind kind;
  union
  {
    untyped_value_t untyped_value;
    typed_value_t typed_value;
    const type_t *typed;
  };
  select_result_t selected_field;
};

const char* abstract_value_to_string (abstract_value_t a);

AbstractValueKind abstract_value_kind (abstract_value_t a);

bool abstract_value_is_undefined (abstract_value_t a);

abstract_value_t abstract_value_make_untyped_value (untyped_value_t u);

untyped_value_t abstract_value_get_untyped_value (abstract_value_t a);

typed_value_t abstract_value_get_typed_value (abstract_value_t a);

bool abstract_value_is_typed_value (abstract_value_t a);

abstract_value_t abstract_value_make_typed (const type_t * type,
                                            select_result_t selected_field);

const type_t *abstract_value_get_typed (abstract_value_t a);

bool abstract_value_is_typed (abstract_value_t a);

abstract_value_t abstract_value_logic_not (abstract_value_t v);

abstract_value_t
abstract_value_logic_and (abstract_value_t x, abstract_value_t y);

abstract_value_t
abstract_value_logic_or (abstract_value_t x, abstract_value_t y);

bool abstract_value_assignable (abstract_value_t target,
				abstract_value_t source);

abstract_value_t abstract_value_dereference (abstract_value_t x);

abstract_value_t
abstract_value_select (abstract_value_t v, string_t identifier);

bool abstract_value_is_boolean (abstract_value_t v);

bool abstract_value_is_bindable (abstract_value_t output,
                                 abstract_value_t input);

action_t* abstract_value_get_reaction (abstract_value_t v);

bool abstract_value_check_arg (abstract_value_t v, size_t idx, abstract_value_t arg);

abstract_value_t abstract_value_return_value (abstract_value_t v);

select_result_t abstract_value_selected_field (abstract_value_t v);

#endif /* abstract_value_h */
