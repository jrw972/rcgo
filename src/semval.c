#define _GNU_SOURCE
#include "semval.h"
#include "type.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symtab.h"
#include "debug.h"

const char* semval_to_string (semval_t s)
{
  switch (s.kind) {
  case Undefined:
    return "undefined semantic value";
  case Reference:
    return reference_to_string (s.reference);
  case Value:
    return abstract_value_to_string (s.value);
  }

  bug ("unhandled case");
}

semval_t
semval_undefined (void)
{
semval_t retval = { kind:Undefined };
  return retval;
}

bool
semval_is_undefined (semval_t s)
{
  return s.kind == Undefined;
}

semval_t
semval_make_reference (reference_t reference)
{
  if (reference_is_undefined (reference)) {
    return semval_undefined ();
  }
  else {
    semval_t retval = { kind: Reference, reference:reference };
    return retval;
  }
}

bool
semval_is_reference (semval_t s)
{
  return s.kind == Reference;
}

semval_t
semval_make_value (abstract_value_t v)
{
semval_t retval = { kind: Value, value:v };
  return retval;
}

bool
semval_is_value (semval_t s)
{
  return s.kind == Value;
}

abstract_value_t
semval_get_value (semval_t s)
{
  assert (s.kind == Value);
  return s.value;
}

semval_t
semval_explicit_dereference (semval_t s)
{
  if (s.kind == Value)
    {
      abstract_value_t av = abstract_value_dereference (s.value);
      return semval_make_reference (reference_make (av));
    }
  else
    {
      return semval_undefined ();
    }
}

semval_t
semval_implicit_dereference (semval_t s)
{
  if (s.kind == Reference)
    {
      return semval_make_value (reference_value (s.reference));
    }
  else
    {
      return semval_undefined ();
    }
}

static semval_t
semval_unary (semval_t s, abstract_value_t (*func) (abstract_value_t))
{
  if (s.kind == Value)
    {
      return semval_make_value (func (s.value));
    }
  else
    {
      return semval_undefined ();
    }
}

static semval_t
semval_binary (semval_t x,
	       semval_t y,
	       abstract_value_t (*func) (abstract_value_t, abstract_value_t))
{
  if (x.kind == Value && y.kind == Value)
    {
      return semval_make_value (func (x.value, y.value));
    }
  else
    {
      return semval_undefined ();
    }
}

semval_t
semval_logic_not (semval_t s)
{
  return semval_unary (s, abstract_value_logic_not);
}

semval_t
semval_logic_and (semval_t x, semval_t y)
{
  return semval_binary (x, y, abstract_value_logic_and);
}

semval_t
semval_logic_or (semval_t x, semval_t y)
{
  return semval_binary (x, y, abstract_value_logic_or);
}

bool
semval_assignable (semval_t left, semval_t right)
{
  /* TODO:  Check the mutable flag. */
  if (!semval_is_reference (left))
    {
      return false;
    }

  if (!semval_is_value (right))
    {
      return false;
    }

  return reference_assignable (left.reference, right.value);
}

semval_t
semval_select (semval_t s, string_t identifier)
{
  if (!semval_is_reference (s))
    {
      return semval_undefined ();
    }

  return semval_make_reference (reference_select (s.reference, identifier));
}

bool
semval_is_boolean (semval_t s)
{
  if (!semval_is_value (s))
    {
      return false;
    }

  return abstract_value_is_boolean (s.value);
}

bool semval_bindable (semval_t output, semval_t input)
{
  return semval_is_reference (output) && semval_is_reference (input) && reference_is_bindable (output.reference, input.reference);
}

action_t* semval_get_reaction (semval_t s)
{
  if (semval_is_reference (s)) {
    return reference_get_reaction (s.reference);
  }
  else {
    return NULL;
  }
}

struct semval_list_t {
  semval_t* array;
  size_t size;
  size_t capacity;
  bool contains_undefined;
};

semval_list_t* semval_list_make (void)
{
  semval_list_t* list = malloc (sizeof (semval_list_t));
  memset (list, 0, sizeof (semval_list_t));
  list->array = malloc (sizeof (semval_t));
  list->size = 0;
  list->capacity = 1;
  return list;
}

void semval_list_append (semval_list_t* list,
                         semval_t semval)
{
  if (semval_is_undefined (semval))
    {
      list->contains_undefined = true;
    }

  if (list->size == list->capacity)
    {
      list->capacity *= 2;
      list->array = realloc (list->array, list->capacity * sizeof (semval_t));
    }

  list->array[list->size++] = semval;
}

bool semval_list_contains_undefined (const semval_list_t* list)
{
  return list->contains_undefined;
}

semval_t semval_call (semval_t expr, const semval_list_t* args)
{
  if (!semval_is_value (expr))
    {
      return semval_undefined ();
    }

  size_t idx, limit;
  for (idx = 0, limit = args->size; idx != limit; ++idx)
    {
      if (!semval_is_value (args->array[idx]))
        {
          return semval_undefined ();
        }
      if (!abstract_value_check_arg (expr.value, idx, args->array[idx].value))
        {
          return semval_undefined ();
        }
    }

  return semval_make_value (abstract_value_return_value (expr.value));
}

select_result_t semval_selected_field (semval_t semval)
{
  switch (semval.kind)
    {
    case Undefined:
      return select_result_make_undefined ();
    case Reference:
      return reference_selected_field (semval.reference);
    case Value:
      return abstract_value_selected_field (semval.value);
    }

  bug ("unhandled case");
}
