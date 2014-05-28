#include "type.h"
#include <stdlib.h>
#include <string.h>
#include "debug.h"

struct field_t {
  string_t name;
  const type_t* type;
  field_t* next;
};

static field_t*
make_field (string_t name,
            const type_t* type)
{
  field_t* retval = malloc (sizeof (field_t));
  memset (retval, 0, sizeof (field_t));
  retval->name = name;
  retval->type = type;
  return retval;
}

struct type_t
{
  TypeKind kind;
  bool has_name;
  string_t name;
  type_t* ptr_to_immutable;
  type_t* ptr_to_mutable;
  union {
    struct {
      field_t* field_list;
      size_t action_count;
    } component;
    struct {
      const type_t* base_type;
    } pointer_to_immutable;
    struct {
      const type_t* base_type;
    } pointer_to_mutable;
  };
};

static type_t *
make (TypeKind kind)
{
  type_t *retval = malloc (sizeof (type_t));
  memset (retval, 0, sizeof (type_t));
  retval->kind = kind;
  return retval;
}

static type_t*
duplicate (const type_t* type)
{
  type_t* retval = make (type->kind);
  switch (retval->kind) {
  case TypeUndefined:
    unimplemented;
  case TypeBool:
    unimplemented;
  case TypeComponent:
    unimplemented;
  case TypePointerToImmutable:
    unimplemented;
  case TypePointerToMutable:
    unimplemented;
  }
  return retval;
}

const type_t* type_set_name (const type_t* type, string_t name)
{
  type_t* t;

  if (type->has_name) {
    /* Duplicate. */
    t = duplicate (type);
  }
  else {
    /* Cast away constness. */
    t = (type_t*)type;
  }

  t->has_name = true;
  t->name = name;
  return t;
}

string_t type_get_name (const type_t* type)
{
  assert (type->has_name);
  return type->name;
}

bool type_is_named (const type_t* type)
{
  return type->has_name;
}

const type_t *
type_make_undefined (void)
{
  static type_t *retval = NULL;
  if (retval == NULL)
    {
      retval = make (TypeUndefined);
    }
  return retval;
}

const type_t *
type_make_bool (void)
{
  return make (TypeBool);
}

const type_t *type_get_pointer_to_immutable (const type_t* type)
{
  if (type->ptr_to_immutable == NULL) {
    type_t* p = make (TypePointerToImmutable);
    p->pointer_to_immutable.base_type = type;
    ((type_t*)type)->ptr_to_immutable = p;
  }
  return type->ptr_to_immutable;
}

const type_t *type_get_pointer_to_mutable (const type_t* type)
{
  if (type->ptr_to_mutable == NULL) {
    type_t* p = make (TypePointerToMutable);
    p->pointer_to_mutable.base_type = type;
    ((type_t*)type)->ptr_to_mutable = p;
  }
  return type->ptr_to_mutable;
}

const type_t *type_pointer_base_type (const type_t* type)
{
  if (type->kind == TypePointerToImmutable) {
    return type->pointer_to_immutable.base_type;
  }
  else if (type->kind == TypePointerToMutable) {
    return type->pointer_to_mutable.base_type;
  }
  else {
    return type_make_undefined ();
  }
}

type_t *
type_make_component (void)
{
  return make (TypeComponent);
}

bool type_is_component (const type_t* type)
{
  return type->kind == TypeComponent;
}

size_t type_add_action (const type_t* type)
{
  assert (type_is_component (type));
  return ((type_t*)type)->component.action_count++;
}

size_t type_action_count (const type_t* type)
{
  assert (type_is_component (type));
  return type->component.action_count;
}

bool type_append_field (type_t* type, string_t field_name, const type_t* field_type)
{
  assert (type->kind == TypeComponent);

  field_t** ptr;
  for (ptr = &(type->component.field_list);
       *ptr != NULL;
       ptr = &(*ptr)->next) {
    if (streq (field_name, (*ptr)->name)) {
      /* Duplicate field name. */
      return false;
    }
  }

  *ptr = make_field (field_name, field_type);
  return true;
}

const field_t* type_field_list (const type_t* type)
{
  assert (type->kind == TypeComponent);
  return type->component.field_list;
}

string_t field_name (const field_t* field)
{
  return field->name;
}

const type_t* field_type (const field_t* field)
{
  return field->type;
}

const field_t* field_next (const field_t* field)
{
  return field->next;
}

TypeKind
type_kind (const type_t * type)
{
  return type->kind;
}

bool
type_can_represent (const type_t * type, untyped_value_t u)
{
  switch (type->kind)
    {
    case TypeUndefined:
      return false;
    case TypeBool:
      return untyped_value_is_bool (u);
    case TypeComponent:
      unimplemented;
    case TypePointerToImmutable:
      unimplemented;
    case TypePointerToMutable:
      unimplemented;
    }
  bug ("unhandled case");
}

bool
type_assignable (const type_t * target, const type_t * source)
{
  switch (target->kind)
    {
    case TypeUndefined:
      return false;
    case TypeBool:
      return target == source;
    case TypeComponent:
      unimplemented;
    case TypePointerToImmutable:
      unimplemented;
    case TypePointerToMutable:
      unimplemented;
    }
  bug ("unhandled case");
}

const type_t *
type_logic_not (const type_t * type)
{
  if (type->kind == TypeBool)
    {
      return type;
    }
  else
    {
      return type_make_undefined ();
    }
}

const type_t *
type_logic_and (const type_t * x, const type_t * y)
{
  if (x->kind == TypeBool && x == y)
    {
      return x;
    }
  else
    {
      return type_make_undefined ();
    }
}

const type_t *
type_logic_or (const type_t * x, const type_t * y)
{
  if (x->kind == TypeBool && x == y)
    {
      return x;
    }
  else
    {
      return type_make_undefined ();
    }
}

const type_t *type_dereference (const type_t * type)
{
  switch (type->kind) {
  case TypeUndefined:
    unimplemented;
  case TypeBool:
    unimplemented;
  case TypeComponent:
    unimplemented;
  case TypePointerToImmutable:
    return type->pointer_to_immutable.base_type;
  case TypePointerToMutable:
    return type->pointer_to_mutable.base_type;
  }

  bug ("unhandled case");
}

bool type_is_pointer_to_mutable (const type_t * type)
{
  return type->kind == TypePointerToMutable;
}

static const type_t*
find_field_or_undefined (const field_t* field,
                         string_t identifier)
{
  for (; field != NULL; field = field->next) {
    if (streq (identifier, field->name)) {
      return field->type;
    }
  }

  return type_make_undefined ();
}

const type_t *type_select (const type_t* type,
                           string_t identifier)
{
  switch (type->kind) {
  case TypeUndefined:
    unimplemented;
  case TypeBool:
    unimplemented;
  case TypeComponent:
    return find_field_or_undefined (type->component.field_list, identifier);
  case TypePointerToImmutable:
    unimplemented;
  case TypePointerToMutable:
    unimplemented;
  }

  bug ("unhandled case");
}

bool type_is_boolean (const type_t* type)
{
  return type->kind == TypeBool;
}
