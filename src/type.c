#define _GNU_SOURCE
#include "type.h"
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "util.h"
#include "action.h"
#include "parameter.h"
#include "field.h"
#include "method.h"

typedef struct list_t list_t;
struct list_t {
  list_t* next;
  const type_t* type;
};

static bool
list_contains (const list_t* list,
               const type_t* type)
{
  if (list == NULL)
    {
      return false;
    }
  else if (list->type == type)
    {
      return true;
    }
  else
    {
      return list_contains (list->next, type);
    }
}

typedef struct {
  const char* (*to_string) (const type_t* type);
  type_t * (*select) (const type_t * type, string_t identifier);
  field_t * (*select_field) (const type_t * type, string_t identifier);
  bool (*leaks_mutable_pointers) (const type_t * type);
  bool (*leaks_non_foreign_pointers) (const type_t * type);
  bool (*contains_foreign_pointers) (const type_t* type, list_t* list);
  size_t (*alignment) (const type_t * type);
  size_t (*size) (const type_t* type);
  bool (*copyable) (const type_t* type);
  bool (*movable) (const type_t* type);
  bool (*can_dereference) (const type_t* type);
  type_t* (*dereference) (const type_t* type);
  bool (*can_assign) (const type_t* type);
  bool (*callable) (const type_t* type);
  type_t * (*parameter_type) (const type_t * type, size_t idx);
  size_t (*parameter_count) (const type_t * type);
  type_t * (*return_type) (const type_t* type);
  bool (*is_boolean) (const type_t* type);
  void (*move_guts_to) (type_t * from, type_t * to);
  bool (*is_convertible_from) (const type_t* to, const type_t* from);
  bool (*is_untyped) (void);
  bool (*is_equivalent) (const type_t* x, const type_t* y);
  bool (*is_arithmetic) (void);
} vtable_t;

static const char* no_to_string (const type_t* type)
{
  not_reached;
}

static type_t* cannot_select (const type_t* type, string_t identifier)
{
  return NULL;
}

static type_t* no_select (const type_t* type, string_t identifier)
{
  unimplemented;
}

static field_t* cannot_select_field (const type_t* type, string_t identifier)
{
  return NULL;
}

static field_t* no_select_field (const type_t* type, string_t identifier)
{
  unimplemented;
}

static bool does_not_leak_mutable_pointers (const type_t* type)
{
  return false;
}

static bool leaks_mutable_pointers (const type_t* type)
{
  return true;
}

static bool does_not_leak_mutable_or_immutable_pointers (const type_t* type)
{
  return false;
}

static bool does_not_contain_foreign_pointers (const type_t* type, list_t* list)
{
  return false;
}

static size_t no_alignment (const type_t* type)
{
  not_reached;
}

static size_t no_size (const type_t* type)
{
  not_reached;
}

static bool no_leaks_non_foreign_pointers (const type_t* type)
{
  unimplemented;
}

static bool no_contains_foreign_pointers (const type_t* type, list_t* list)
{
  unimplemented;
}

static bool no_copyable (const type_t* type)
{
  unimplemented;
}

static bool copyable (const type_t* type)
{
  return true;
}

static bool movable (const type_t* type)
{
  return true;
}

static bool not_movable (const type_t* type)
{
  return false;
}

static bool no_can_dereference (const type_t* type)
{
  unimplemented;
}

static bool can_dereference (const type_t* type)
{
  return true;
}

static type_t* no_dereference (const type_t* type)
{
  unimplemented;
}

static bool no_can_assign (const type_t* type)
{
  unimplemented;
}

static bool can_assign (const type_t* type)
{
  return true;
}

static bool cannot_assign (const type_t* type)
{
  return false;
}

static bool no_callable (const type_t* type)
{
  unimplemented;
}

static bool callable (const type_t* type)
{
  return true;
}

static type_t * no_parameter_type (const type_t * type, size_t idx)
{
  unimplemented;
}

static size_t no_parameter_count (const type_t * type)
{
  unimplemented;
}

static type_t * no_return_type (const type_t* type)
{
  unimplemented;
}

static bool no_is_boolean (const type_t* type)
{
  unimplemented;
}

static bool is_boolean (const type_t* type)
{
  return true;
}

static void no_move_guts_to (type_t * from, type_t * to)
{
  unimplemented;
}

static bool no_is_convertible_from (const type_t* to, const type_t* from)
{
  unimplemented;
}

static bool is_untyped (void)
{
  return true;
}

static bool is_not_untyped (void)
{
  return false;
}

static bool yes (void)
{
  return true;
}

static bool no1 (const type_t* t)
{
  return false;
}

struct type_t
{
  const vtable_t* vtable;
  TypeKind kind;
  bool has_name;
  string_t name;
  VECTOR_DECL (methods, method_t *);
  union
  {
    struct
    {
      type_t *field_list;
      VECTOR_DECL (actions, action_t *);
      VECTOR_DECL (reactions, action_t *);
      VECTOR_DECL (bindings, binding_t *);
    } component;
    struct
    {
      VECTOR_DECL (fields, field_t *);
      ptrdiff_t offset;
      size_t alignment;
    } field_list;
    struct
    {
      type_t* base_type;
    } foreign;
    struct
    {
      const type_t *signature;
      type_t *return_type;
    } func;
    struct
    {
      type_t* base_type;
    } heap;
    struct
    {
      type_t *base_type;
    } immutable;
    struct
    {
      type_t *base_type;
    } pointer;
    struct
    {
      const type_t *signature;
    } port;
    struct
    {
      type_t *signature;
    } reaction;
    struct
    {
      VECTOR_DECL (parameters, parameter_t *);
    } signature;
    struct
    {
      type_t * field_list;
    } struct_;
  };
};

static size_t
type_alignment (const type_t * type)
{
  return type->vtable->alignment (type);
}

static bool
internal_contains_foreign_pointers (const type_t* type,
                                    list_t* list)
{
  if (list_contains (list, type))
    {
      return false;
    }

  list_t head;
  head.next = list;
  head.type = type;
  return type->vtable->contains_foreign_pointers (type, &head);
}

static size_t bool_alignment (const type_t* type)
{
  return 1;
}

static size_t bool_size (const type_t* type)
{
  return 1;
}

static bool bool_is_convertible_from (const type_t* to, const type_t* from)
{
  // Converting to bool.
  return from->kind == UntypedBool;
}

static vtable_t bool_vtable = {
 to_string: no_to_string,
 select: cannot_select,
 select_field: cannot_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: does_not_leak_mutable_or_immutable_pointers,
 contains_foreign_pointers: does_not_contain_foreign_pointers,
 alignment: bool_alignment,
 size: bool_size,
 copyable: copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: bool_is_convertible_from,
 is_untyped: is_not_untyped,
};

static type_t* component_select (const type_t* type, string_t identifier)
{
  type_t *t = type_select (type->component.field_list, identifier);
  if (t != NULL)
    {
      return t;
    }

  {
    VECTOR_FOREACH (ptr, limit, type->component.reactions, action_t *)
      {
        if (streq (identifier, reaction_name (*ptr)))
          {
            return reaction_type (*ptr);
          }
      }
  }

  return NULL;
}

static field_t* component_select_field (const type_t* type, string_t identifier)
{
  return type_select_field (type->component.field_list, identifier);
}

static bool component_leaks_mutable_pointers (const type_t* type)
{
  return type_leaks_mutable_pointers (type->component.field_list);
}

static size_t component_alignment (const type_t* type)
{
  return type_alignment (type->component.field_list);
}

static size_t component_size (const type_t* type)
{
  return type_size (type->component.field_list);
}

static void component_move_guts_to (type_t * from, type_t * to)
{
  to->component.field_list = from->component.field_list;
  VECTOR_MOVE (to->component.actions, from->component.actions);
  VECTOR_MOVE (to->component.reactions, from->component.reactions);
  VECTOR_MOVE (to->component.bindings, from->component.bindings);
}

static vtable_t component_vtable = {
 to_string: no_to_string,
 select: component_select,
 select_field: component_select_field,
 leaks_mutable_pointers: component_leaks_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: component_alignment,
 size: component_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: component_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static type_t* field_list_select (const type_t* type, string_t identifier)
{
  field_t *field = type_field_list_find (type, identifier);
  if (field != NULL)
    {
      return field_type (field);
    }
  else
    {
      return NULL;
    }
}

static field_t* field_list_select_field (const type_t* type, string_t identifier)
{
  return type_field_list_find (type, identifier);
}

static bool field_list_leaks_mutable_pointers (const type_t* type)
{
  VECTOR_FOREACH (pos, limit, type->field_list.fields, field_t*)
    {
      field_t* field = *pos;
      if (type_leaks_mutable_pointers (field_type (field)))
        {
          return true;
        }
    }

  return false;
}

static bool field_list_contains_foreign_pointers (const type_t* type, list_t* list)
{
  VECTOR_FOREACH (pos, limit, type->field_list.fields, field_t*)
    {
      field_t* field = *pos;
      if (internal_contains_foreign_pointers (field_type (field), list))
        {
          return true;
        }
    }

  return false;
}

static size_t field_list_alignment (const type_t* type)
{
  return type->field_list.alignment;
}

static size_t field_list_size (const type_t* type)
{
  return type->field_list.offset;
}

static bool field_list_copyable (const type_t* type)
{
  VECTOR_FOREACH (pos, limit, type->field_list.fields, field_t*)
    {
      field_t* field = *pos;
      if (!type_copyable (field_type (field)))
        {
          return false;
        }
    }

  return true;
}

static vtable_t field_list_vtable = {
 to_string: no_to_string,
 select: field_list_select,
 select_field: field_list_select_field,
 leaks_mutable_pointers: field_list_leaks_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: field_list_contains_foreign_pointers,
 alignment: field_list_alignment,
 size: field_list_size,
 copyable: field_list_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static const char* foreign_to_string (const type_t* type)
{
  char *str;
  asprintf (&str, "?%s", type_to_string (type->foreign.base_type));
  return str;
}

static type_t * foreign_select (const type_t * type, string_t identifier)
{
  return type_select (type->foreign.base_type, identifier);
}

static bool foreign_contains_foreign_pointers (const type_t* type, list_t* list)
{
  return internal_contains_foreign_pointers (type->foreign.base_type, list);
}

static size_t foreign_alignment (const type_t* type)
{
  return type_alignment (type->foreign.base_type);
}

static size_t foreign_size (const type_t* type)
{
  return type_size (type->foreign.base_type);
}

static bool foreign_copyable (const type_t* type)
{
  return type_copyable (type->foreign.base_type);
}

static vtable_t foreign_vtable = {
 to_string: foreign_to_string,
 select: foreign_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: does_not_leak_mutable_or_immutable_pointers,
 contains_foreign_pointers: foreign_contains_foreign_pointers,
 alignment: foreign_alignment,
 size: foreign_size,
 copyable: foreign_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: cannot_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static size_t func_alignment (const type_t* type)
{
  return sizeof (void*);
}

static type_t * func_parameter_type (const type_t * type, size_t idx)
{
  return type_parameter_type (type->func.signature, idx);
}

static size_t func_parameter_count (const type_t * type)
{
  return type_parameter_count (type->func.signature);
}

static type_t * func_return_type (const type_t* type)
{
  return type->func.return_type;
}

static vtable_t func_vtable = {
 to_string: no_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: func_alignment,
 size: no_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: callable,
 parameter_type: func_parameter_type,
 parameter_count: func_parameter_count,
 return_type: func_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static const char* heap_to_string (const type_t* type)
{
  char *str;
  asprintf (&str, "heap %s", type_to_string (type->heap.base_type));
  return str;
}

static bool heap_leaks_mutable_pointers (const type_t* type)
{
  unimplemented;
}

static bool heap_leaks_non_foreign_pointers (const type_t* type)
{
  return type_leaks_non_foreign_pointers (type->heap.base_type);
}

static bool heap_contains_foreign_pointers (const type_t* type, list_t* list)
{
  return internal_contains_foreign_pointers (type->heap.base_type, list);
}

static bool heap_is_equivalent (const type_t* x, const type_t* y)
{
  return type_equivalent (x->heap.base_type, y->heap.base_type);
}

static vtable_t heap_vtable = {
 to_string:   heap_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: heap_leaks_mutable_pointers,
 leaks_non_foreign_pointers: heap_leaks_non_foreign_pointers,
 contains_foreign_pointers: heap_contains_foreign_pointers,
 copyable: copyable,
 movable: movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
 is_equivalent: heap_is_equivalent,
};

static const char* immutable_to_string (const type_t* type)
{
  char *str;
  asprintf (&str, "const %s", type_to_string (type->immutable.base_type));
  return str;
}

static type_t* immutable_select (const type_t* type, string_t identifier)
{
  type_t* t = type_select (type->immutable.base_type, identifier);
  if (t == NULL)
    {
      return NULL;
    }
  if (type_is_immutable (t) || type_is_foreign (t))
    {
      return t;
    }
  if (type_is_pointer (t))
    {
      t = type_make_pointer_to_immutable (type_pointer_base_type (t));
    }
  return type_make_immutable (t);
}

static field_t* immutable_select_field (const type_t* type, string_t identifier)
{
  return type_select_field (type->immutable.base_type, identifier);
}

static bool immutable_leaks_mutable_pointers (const type_t* type)
{
  return type_leaks_mutable_pointers (type->immutable.base_type);
}

static bool immutable_contains_foreign_pointers (const type_t* type, list_t* list)
{
  return internal_contains_foreign_pointers (type->immutable.base_type, list);
}

static size_t immutable_alignment (const type_t* type)
{
  return type_alignment (type->immutable.base_type);
}

static size_t immutable_size (const type_t* type)
{
  return type_size (type->immutable.base_type);
}

static bool immutable_can_dereference (const type_t* type)
{
  return type_can_dereference (type->immutable.base_type);
}

static type_t* immutable_dereference (const type_t* type)
{
  return type_make_immutable (type_dereference (type->immutable.base_type));
}

static bool immutable_callable (const type_t* type)
{
  return type_callable (type->immutable.base_type);
}

static type_t * immutable_parameter_type (const type_t * type, size_t idx)
{
  return type_parameter_type (type->immutable.base_type, idx);
}

static size_t immutable_parameter_count (const type_t * type)
{
  return type_parameter_count (type->immutable.base_type);
}

static type_t * immutable_return_type (const type_t* type)
{
  return type_return_type (type->immutable.base_type);
}

static bool immutable_is_boolean (const type_t* type)
{
  return type_is_boolean (type->immutable.base_type);
}

// Does not leak mutable pointers by definition.
static vtable_t immutable_vtable = {
 to_string:   immutable_to_string,
 select: immutable_select,
 select_field: immutable_select_field,
 leaks_mutable_pointers: immutable_leaks_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: immutable_contains_foreign_pointers,
 alignment: immutable_alignment,
 size: immutable_size,
 copyable: copyable,
 movable: not_movable,
 can_dereference: immutable_can_dereference,
 dereference: immutable_dereference,
 can_assign: cannot_assign,
 callable: immutable_callable,
 parameter_type: immutable_parameter_type,
 parameter_count: immutable_parameter_count,
 return_type: immutable_return_type,
 is_boolean: immutable_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static const char* pointer_to_string (const type_t* type)
{
  char *str;
  asprintf (&str, "@%s", type_to_string (type->pointer.base_type));
  return str;
}

static bool pointer_leaks_non_foreign_pointers (const type_t* type)
{
  return !type_is_foreign (type->foreign.base_type);
}

static bool pointer_contains_foreign_pointers (const type_t* type, list_t* list)
{
  if (type_is_foreign (type->pointer.base_type))
    {
      return true;
    }
  return internal_contains_foreign_pointers (type->pointer.base_type, list);
}

static size_t pointer_alignment (const type_t* type)
{
  return sizeof (void*);
}

static size_t pointer_size (const type_t* type)
{
  return sizeof (void*);
}

static type_t* pointer_dereference (const type_t* type)
{
  return type->pointer.base_type;
}

static bool pointer_is_convertible_from (const type_t* to, const type_t* from)
{
  // Converting to a pointer.
  if (from->kind == UntypedNil)
    {
      return true;
    }
  if (from->kind == TypePointer)
    {
      if (type_equivalent (to->pointer.base_type, from->pointer.base_type))
        {
          return true;
        }
      if (type_is_immutable (to->pointer.base_type) &&
          type_equivalent (to->pointer.base_type->immutable.base_type, from->pointer.base_type))
        {
          // We can always cast to immutable.
          return true;
        }
      if (type_is_foreign (to->pointer.base_type) &&
          type_equivalent (to->pointer.base_type->foreign.base_type, from->pointer.base_type))
        {
          // We can always cast to foreign.
          return true;
        }
      if (type_is_foreign (to->pointer.base_type) &&
          type_is_immutable (from->pointer.base_type) &&
          type_equivalent (to->pointer.base_type->foreign.base_type, from->pointer.base_type->immutable.base_type))
        {
          // We can always cast from immutable to foreign.
          return true;
        }
    }

  return false;
}

static bool pointer_is_equivalent (const type_t* x, const type_t* y)
{
  return type_equivalent (x->pointer.base_type, y->pointer.base_type);;
}

static vtable_t pointer_vtable = {
 to_string:   pointer_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: leaks_mutable_pointers,
 leaks_non_foreign_pointers: pointer_leaks_non_foreign_pointers,
 contains_foreign_pointers: pointer_contains_foreign_pointers,
 alignment: pointer_alignment,
 size: pointer_size,
 copyable: copyable,
 movable: not_movable,
 can_dereference: can_dereference,
 dereference: pointer_dereference,
 can_assign: can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: pointer_is_convertible_from,
 is_untyped: is_not_untyped,
 is_equivalent: pointer_is_equivalent,
};

static const char* pointer_to_foreign_to_string (const type_t* type)
{
  char *str;
  asprintf (&str, "?%s", type_to_string (type->pointer.base_type));
  return str;
}

static type_t* pointer_to_foreign_dereference (const type_t* type)
{
  if (type_is_foreign (type->pointer.base_type))
    {
      return type->pointer.base_type;
    }
  if (type_is_immutable (type->pointer.base_type))
    {
      return type_make_foreign (type->pointer.base_type->immutable.base_type);
    }
  return type_make_foreign (type->pointer.base_type);
}

static bool pointer_to_foreign_is_convertible_from (const type_t* to, const type_t* from)
{
  // Converting to a pointer.
  if (from->kind == UntypedNil)
    {
      return true;
    }
  if (from->kind == TypePointer || from->kind == TypePointerToImmutable || from->kind == TypePointerToForeign)
    {
      if (type_equivalent (to->pointer.base_type, from->pointer.base_type))
        {
          return true;
        }
      if (type_is_immutable (to->pointer.base_type) &&
          type_equivalent (to->pointer.base_type->immutable.base_type, from->pointer.base_type))
        {
          // We can always cast to immutable.
          return true;
        }
      if (type_is_foreign (to->pointer.base_type) &&
          type_equivalent (to->pointer.base_type->foreign.base_type, from->pointer.base_type))
        {
          // We can always cast to foreign.
          return true;
        }
      if (type_is_foreign (to->pointer.base_type) &&
          type_is_immutable (from->pointer.base_type) &&
          type_equivalent (to->pointer.base_type->foreign.base_type, from->pointer.base_type->immutable.base_type))
        {
          // We can always cast from immutable to foreign.
          return true;
        }
    }

  return false;
}

static vtable_t pointer_to_foreign_vtable = {
 to_string: pointer_to_foreign_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 contains_foreign_pointers: pointer_contains_foreign_pointers,
 alignment: pointer_alignment,
 size: pointer_size,
 copyable: copyable,
 movable: not_movable,
 can_dereference: can_dereference,
 dereference: pointer_to_foreign_dereference,
 can_assign: cannot_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: pointer_to_foreign_is_convertible_from,
 is_untyped: is_not_untyped,
 leaks_non_foreign_pointers: no1,
};

static const char* pointer_to_immutable_to_string (const type_t* type)
{
  char *str;
  asprintf (&str, "$%s", type_to_string (type->pointer.base_type));
  return str;
}

static bool pointer_to_immutable_is_convertible_from (const type_t* to, const type_t* from)
{
  // Converting to a pointer.
  if (from->kind == UntypedNil)
    {
      return true;
    }
  if (from->kind == TypePointer || from->kind == TypePointerToImmutable)
    {
      if (type_equivalent (to->pointer.base_type, from->pointer.base_type))
        {
          return true;
        }
      if (type_is_immutable (to->pointer.base_type) &&
          type_equivalent (to->pointer.base_type->immutable.base_type, from->pointer.base_type))
        {
          // We can always cast to immutable.
          return true;
        }
      if (type_is_foreign (to->pointer.base_type) &&
          type_equivalent (to->pointer.base_type->foreign.base_type, from->pointer.base_type))
        {
          // We can always cast to foreign.
          return true;
        }
      if (type_is_foreign (to->pointer.base_type) &&
          type_is_immutable (from->pointer.base_type) &&
          type_equivalent (to->pointer.base_type->foreign.base_type, from->pointer.base_type->immutable.base_type))
        {
          // We can always cast from immutable to foreign.
          return true;
        }
    }

  return false;
}

static type_t* pointer_to_immutable_dereference (const type_t* type)
{
  if (type_is_immutable (type->pointer.base_type) ||
      type_is_foreign (type->pointer.base_type))
    {
      return type->pointer.base_type;
    }
  return type_make_immutable (type->pointer.base_type);
}

static vtable_t pointer_to_immutable_vtable = {
 to_string:   pointer_to_immutable_to_string,
 select: cannot_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: pointer_leaks_non_foreign_pointers,
 contains_foreign_pointers: pointer_contains_foreign_pointers,
 alignment: pointer_alignment,
 size: pointer_size,
 copyable: copyable,
 movable: not_movable,
 can_dereference: can_dereference,
 dereference: pointer_to_immutable_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: pointer_to_immutable_is_convertible_from,
 is_untyped: is_not_untyped,
};

static size_t port_alignment (const type_t* type)
{
  return sizeof (void*);
}

static size_t port_size (const type_t* type)
{
  return sizeof (void*);
}

static type_t * port_parameter_type (const type_t * type, size_t idx)
{
  return type_parameter_type (type->port.signature, idx);
}

static size_t port_parameter_count (const type_t * type)
{
  return type_parameter_count (type->port.signature);
}

static type_t * port_return_type (const type_t* type)
{
  return type_make_void ();
}

static vtable_t port_vtable = {
 to_string:   no_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: port_alignment,
 size: port_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: port_parameter_type,
 parameter_count: port_parameter_count,
 return_type: port_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static vtable_t reaction_vtable = {
 to_string:   no_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: no_alignment,
 size: no_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static bool signature_leaks_non_foreign_pointers (const type_t* type)
{
  VECTOR_FOREACH (ptr, limit, type->signature.parameters, parameter_t *)
    {
      if (type_leaks_non_foreign_pointers (parameter_type (*ptr)))
        {
          return true;
        }
    }

  return false;
}

static type_t * signature_parameter_type (const type_t * type, size_t idx)
{
  return parameter_type (VECTOR_AT (type->signature.parameters, idx));
}

static size_t signature_parameter_count (const type_t * type)
{
  return VECTOR_SIZE (type->signature.parameters);
}

static const char* signature_to_string (const type_t* type)
{
  char *str;
  asprintf (&str, "(");
  bool flag = false;
  VECTOR_FOREACH (ptr, limit, type->signature.parameters, parameter_t *)
    {
      if (flag)
        {
          asprintf (&str, "%s, ", str);
        }

      asprintf (&str, "%s, %s", str, type_to_string (parameter_type (*ptr)));
      flag = true;
    }
  asprintf (&str, "%s)", str);
  return str;
}

static vtable_t signature_vtable = {
 to_string:   signature_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: signature_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: no_alignment,
 size: no_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: signature_parameter_type,
 parameter_count: signature_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static size_t string_alignment (const type_t* type)
{
  unimplemented;
}

static vtable_t string_vtable = {
 to_string:   no_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: string_alignment,
 size: no_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static type_t* struct_select (const type_t* type, string_t identifier)
{
  return type_select (type->struct_.field_list, identifier);
}

static field_t* struct_select_field (const type_t* type, string_t identifier)
{
  return type_select_field (type->struct_.field_list, identifier);
}

static bool struct_leaks_mutable_pointers (const type_t* type)
{
  return type_leaks_mutable_pointers (type->struct_.field_list);
}

static bool struct_contains_foreign_pointers (const type_t* type, list_t* list)
{
  return internal_contains_foreign_pointers (type->struct_.field_list, list);
}

static size_t struct_alignment (const type_t* type)
{
  return type_alignment (type->struct_.field_list);
}

static size_t struct_size (const type_t* type)
{
  return type_size (type->struct_.field_list);
}

static bool struct_copyable (const type_t* type)
{
  return type_copyable (type->struct_.field_list);
}

static void struct_move_guts_to (type_t * from, type_t * to)
{
  to->struct_.field_list = from->struct_.field_list;
}

static vtable_t struct_vtable = {
 to_string:   no_to_string,
 select: struct_select,
 select_field: struct_select_field,
 leaks_mutable_pointers: struct_leaks_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: struct_contains_foreign_pointers,
 alignment: struct_alignment,
 size: struct_size,
 copyable: struct_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: struct_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static vtable_t undefined_vtable = {
 to_string:   no_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: no_alignment,
 size: no_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static size_t uint_alignment (const type_t* type)
{
  return sizeof (void*);
}

static size_t uint_size (const type_t* type)
{
  return sizeof (void*);
}

static bool uint_is_convertible_from (const type_t* to, const type_t* from)
{
  // Converting to uint.
  return from->kind == TypeUint || from->kind == UntypedInteger;
}

static vtable_t uint_vtable = {
 to_string: no_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: does_not_leak_mutable_or_immutable_pointers,
 contains_foreign_pointers: does_not_contain_foreign_pointers,
 alignment: uint_alignment,
 size: uint_size,
 copyable: copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: uint_is_convertible_from,
 is_untyped: is_not_untyped,
 is_arithmetic: yes
};

static size_t void_size (const type_t* type)
{
  return 0;
}

static vtable_t void_vtable = {
 to_string:   no_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: no_alignment,
 size: void_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_not_untyped,
};

static const char* untyped_bool_to_string (const type_t* type)
{
  return "untyped bool";
}

static vtable_t untyped_bool_vtable = {
 to_string:   untyped_bool_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: does_not_contain_foreign_pointers,
 alignment: no_alignment,
 size: no_size,
 copyable: copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_untyped,
};

static const char* untyped_integer_to_string (const type_t* type)
{
  return "untyped integer";
}

static vtable_t untyped_integer_vtable = {
 to_string:   untyped_integer_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: no_alignment,
 size: no_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_untyped,
};

static const char* untyped_nil_to_string (const type_t* type)
{
  return "nil";
}

static vtable_t untyped_nil_vtable = {
 to_string:   untyped_nil_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: no_alignment,
 size: no_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_untyped,
};

static const char* untyped_string_to_string (const type_t* type)
{
  return "untyped string";
}

static vtable_t untyped_string_vtable = {
 to_string:   untyped_string_to_string,
 select: no_select,
 select_field: no_select_field,
 leaks_mutable_pointers: does_not_leak_mutable_pointers,
 leaks_non_foreign_pointers: no_leaks_non_foreign_pointers,
 contains_foreign_pointers: no_contains_foreign_pointers,
 alignment: no_alignment,
 size: no_size,
 copyable: no_copyable,
 movable: not_movable,
 can_dereference: no_can_dereference,
 dereference: no_dereference,
 can_assign: no_can_assign,
 callable: no_callable,
 parameter_type: no_parameter_type,
 parameter_count: no_parameter_count,
 return_type: no_return_type,
 is_boolean: no_is_boolean,
 move_guts_to: no_move_guts_to,
 is_convertible_from: no_is_convertible_from,
 is_untyped: is_untyped,
};

// TODO:  Replace functions with interface.

const char *
type_to_string (const type_t * type)
{
  if (type->has_name)
    {
      return get (type->name);
    }
  else
    {
      return type->vtable->to_string (type);

      /* switch (type->kind) */
      /*   { */
      /*   case TypeUndefined: */
      /*     return "undefined"; */
      /*   case TypeVoid: */
      /*     return "void"; */
      /*   case TypeBool: */
      /*     unimplemented; */
      /*   case TypeComponent: */
      /*     unimplemented; */
      /*   case TypeForeign: */
      /*     unimplemented; */
      /*   case TypeImmutable: */
      /*     { */
      /*       char *str; */
      /*       asprintf (&str, "$%s", type_to_string (type->immutable.base_type)); */
      /*       return str; */
      /*     } */
      /*   case TypeHeap: */
      /*     unimplemented; */
      /*   case TypePointer: */
      /*     unimplemented; */
      /*   case TypePointerToForeign: */
      /*     unimplemented; */
      /*   case TypePointerToImmutable: */
      /*     unimplemented; */
      /*   case TypePort: */
      /*     { */
      /*       char *str; */
      /*       asprintf (&str, "port %s", type_to_string (type->port.signature)); */
      /*       return str; */
      /*     } */
      /*   case TypeReaction: */
      /*     { */
      /*       char *str; */
      /*       asprintf (&str, "reaction %s", */
      /*   	      type_to_string (type->reaction.signature)); */
      /*       return str; */
      /*     } */
      /*   case TypeFieldList: */
      /*     unimplemented; */
      /*   case TypeSignature: */
      /*   case TypeString: */
      /*     unimplemented; */
      /*   case TypeFunc: */
      /*     { */
      /*       char *str; */
      /*       asprintf (&str, "func %s %s", type_to_string (type->func.signature), type_to_string (type->func.return_type)); */
      /*       return str; */
      /*     } */

      /*   } */
    }
}

void
type_move_guts_to (type_t * to, type_t * from)
{
  assert (to->kind == TypeUndefined);
  assert (from->kind != TypeUndefined);

  to->vtable = from->vtable;
  to->kind = from->kind;

  to->methods = from->methods;
  to->methods_size = from->methods_size;
  to->methods_capacity = from->methods_capacity;

  /* Do not copy has_name and name. */

  to->vtable->move_guts_to (from, to);

  free (from);
}

size_t
type_size (const type_t * type)
{
  return type->vtable->size (type);
}

static type_t *
make (const vtable_t* vtable, TypeKind kind)
{
  assert (vtable != NULL);
  type_t *retval = xmalloc (sizeof (type_t));
  retval->vtable = vtable;
  retval->kind = kind;
  VECTOR_INIT (retval->methods, method_t *, 0, NULL);
  return retval;
}

static type_t *
duplicate (const type_t * type)
{
  unimplemented;
  /* type_t *retval = make (type->vtable, type->kind); */
  /* switch (retval->kind) */
  /*   { */
  /*   case TypeUndefined: */
  /*     unimplemented; */
  /*   case TypeVoid: */
  /*     unimplemented; */
  /*   case TypeBool: */
  /*     unimplemented; */
  /*   case TypeComponent: */
  /*     unimplemented; */
  /*   case TypeForeign: */
  /*     unimplemented; */
  /*   case TypeHeap: */
  /*     unimplemented; */

  /*   case TypeImmutable: */
  /*     unimplemented; */

  /*   case TypePointer: */
  /*     unimplemented; */
  /*   case TypePointerToForeign: */
  /*     unimplemented; */
  /*   case TypePointerToImmutable: */
  /*     unimplemented; */

  /*   case TypePort: */
  /*     unimplemented; */
  /*   case TypeReaction: */
  /*     unimplemented; */
  /*   case TypeFieldList: */
  /*     unimplemented; */
  /*   case TypeSignature: */
  /*     unimplemented; */
  /*   case TypeString: */
  /*     unimplemented; */
  /*   case TypeFunc: */
  /*     unimplemented; */
  /*   case TypeUint: */
  /*     unimplemented; */
  /*   case TypeStruct: */
  /*     unimplemented; */

  /*   case UntypedUndefined: */
  /*     unimplemented; */
  /*   case UntypedNil: */
  /*     unimplemented; */

  /*   case UntypedBool: */
  /*     unimplemented; */
  /*   case UntypedInteger: */
  /*     unimplemented; */
  /*   case UntypedString: */
  /*     unimplemented; */

  /*   } */
  /* return retval; */
}

const type_t *
type_set_name (const type_t * type, string_t name)
{
  type_t *t;

  if (type->has_name)
    {
      /* Duplicate. */
      t = duplicate (type);
    }
  else
    {
      /* Cast away constness. */
      t = (type_t *) type;
    }

  t->has_name = true;
  t->name = name;
  return t;
}

string_t
type_get_name (const type_t * type)
{
  assert (type->has_name);
  return type->name;
}

bool
type_is_named (const type_t * type)
{
  return type->has_name;
}

type_t *
type_make_undefined (void)
{
  return make (&undefined_vtable, TypeUndefined);
}

bool
type_is_undefined (const type_t * type)
{
  return type->kind == TypeUndefined;
}

type_t *
type_make_bool (void)
{
  return make (&bool_vtable, TypeBool);
}

type_t *
type_pointer_base_type (const type_t * type)
{
  assert (type->kind == TypePointer ||
          type->kind == TypePointerToForeign ||
          type->kind == TypePointerToImmutable);
  return type->pointer.base_type;
}

type_t *
type_make_void ()
{
  static type_t *t = NULL;
  if (t == NULL)
    {
      t = make (&void_vtable, TypeVoid);
    }
  return t;
}

type_t *
type_make_component (type_t * field_list)
{
  type_t *c = make (&component_vtable, TypeComponent);
  /* Prepend the field list with a pointer for the runtime. */
  type_field_list_prepend (field_list, enter ("0"), type_make_immutable (type_make_pointer (type_make_void ())));
  c->component.field_list = field_list;
  VECTOR_INIT (c->component.actions, action_t *, 0, NULL);
  VECTOR_INIT (c->component.reactions, action_t *, 0, NULL);
  VECTOR_INIT (c->component.bindings, binding_t *, 0, NULL);
  return c;
}

type_t *
type_make_pointer (type_t * base_type)
{
  type_t *t = make (&pointer_vtable, TypePointer);
  t->pointer.base_type = base_type;
  return t;
}

type_t *
type_make_pointer_to_foreign (type_t * base_type)
{
  type_t *t = make (&pointer_to_foreign_vtable, TypePointerToForeign);
  t->pointer.base_type = base_type;
  return t;
}

type_t *
type_make_pointer_to_immutable (type_t * base_type)
{
  type_t *t = make (&pointer_to_immutable_vtable, TypePointerToImmutable);
  t->pointer.base_type = base_type;
  return t;
}

type_t *
type_make_port (const type_t * signature)
{
  type_t *retval = make (&port_vtable, TypePort);
  retval->port.signature = signature;
  return retval;
}

bool
type_is_port (const type_t * type)
{
  return type->kind == TypePort;
}

bool
type_is_component (const type_t * type)
{
  return type->kind == TypeComponent;
}

action_t *
type_component_add_action (type_t * type, ast_t* node)
{
  assert (type_is_component (type));
  action_t *action = action_make (type, node, VECTOR_SIZE (type->component.actions));
  VECTOR_PUSH (type->component.actions, action_t *, action);
  return action;
}

action_t *
type_component_add_reaction (type_t * component_type, ast_t* node, string_t identifier,
			     type_t * signature)
{
  assert (type_is_component (component_type));
  action_t *r =
    reaction_make (component_type,
                   node,
		   identifier, signature);
  VECTOR_PUSH (component_type->component.reactions, action_t *, r);
  return r;
}

method_t *type_add_method (type_t * type,
                           ast_t* node,
                           string_t identifier,
                           type_t * signature,
                           type_t * return_type)
{
  assert (type_is_named (type));
  method_t *g =
    method_make (type,
                 node,
                 identifier, signature, return_type);
  VECTOR_PUSH (type->methods, method_t*, g);
  return g;
}

TypeKind
type_kind (const type_t * type)
{
  return type->kind;
}

/* bool */
/* type_can_represent (const type_t * type, untyped_value_t u) */
/* { */
/*   switch (type->kind) */
/*     { */
/*     case TypeUndefined: */
/*       return false; */
/*     case TypeVoid: */
/*       unimplemented; */
/*     case TypeBool: */
/*       return u.kind == UntypedBool; */
/*     case TypeComponent: */
/*       unimplemented; */
/*     case TypePointer: */
/*       unimplemented; */
/*     case TypePort: */
/*       unimplemented; */
/*     case TypeReaction: */
/*       unimplemented; */
/*     case TypeFieldList: */
/*       unimplemented; */
/*     case TypeSignature: */
/*       unimplemented; */
/*     case TypeString: */
/*       return u.kind == UntypedString; */
/*     case TypeFunc: */
/*       unimplemented; */
/*     case TypeUint: */
/*       // TODO:  This will need to change if we expand the range of untyped integers. */
/*       return u.kind == UntypedInteger; */
/*           case TypeStruct: */
/*             unimplemented; */

/*     } */
/*   not_reached; */
/* } */

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

action_t *
type_component_get_reaction (const type_t * component_type,
			     string_t identifier)
{
  assert (component_type->kind == TypeComponent);

  VECTOR_FOREACH (pos, limit, component_type->component.reactions, action_t *)
    {
      action_t *a = *pos;
      if (streq (reaction_name (a), identifier))
        {
          return a;
        }
    }

  return NULL;
}

method_t *type_get_method (const type_t * type,
                           string_t identifier)
{
  if (type_is_immutable (type))
    {
      type = type_immutable_base_type (type);
    }

  VECTOR_FOREACH (pos, limit, type->methods, method_t *)
    {
      method_t *a = *pos;
      if (streq (method_name (a), identifier))
        {
          return a;
        }
    }

  return NULL;
}

/* static bool */
/* compatible_signatures (const signature_t* output, const signature_t* input) */
/* { */
/*   if (output->parameters_size != input->parameters_size) */
/*     { */
/*       return false; */
/*     } */

/*   size_t idx, limit; */
/*   for (idx = 0, limit = output->parameters_size; idx != limit; ++idx) */
/*     { */
/*       if (output->parameters[idx].type != input->parameters[idx].type) */
/*         { */
/*           return false; */
/*         } */
/*     } */

/*   return true; */
/* } */

bool
type_is_bindable (const type_t * output, const type_t * input)
{
  if (output->kind != TypePort)
    {
      return false;
    }

  if (input->kind != TypeReaction)
    {
      return false;
    }

  unimplemented;
  //return compatible_signatures (output->port.signature, input->reaction.signature);
}

size_t type_action_count (const type_t* type)
{
  assert (type_is_component (type));
  return VECTOR_SIZE (type->component.actions);
}

action_t**
type_actions_begin (const type_t * type)
{
  assert (type_is_component (type));
  return VECTOR_BEGIN (type->component.actions);
}

action_t **
type_actions_end (const type_t * type)
{
  assert (type_is_component (type));
  return VECTOR_END (type->component.actions);
}

action_t **
type_actions_next (action_t ** action)
{
  return VECTOR_NEXT (action);
}

bool
type_check_arg (const type_t * type, size_t idx, const type_t * arg)
{
  unimplemented;
  /* // type must be callable. */
  /* switch (type->kind) */
  /*   { */
  /*   case TypePort: */
  /*     return signature_check_arg (type->port.signature, idx, arg); */
  /*   case TypeUndefined: */
  /*   case TypeVoid: */
  /*   case TypeBool: */
  /*   case TypeComponent: */
  /*   case TypePointer: */
  /*   case TypeReaction: */
  /*   case TypeFieldList: */
  /*     return false; */
  /*   } */

  bug ("unhandled case");
}

const type_t *
type_return_value (const type_t * type)
{
  unimplemented;
  /* switch (type->kind) */
  /*   { */
  /*   case TypePort: */
  /*     return type_make_void (); */
  /*   case TypeUndefined: */
  /*   case TypeVoid: */
  /*   case TypeBool: */
  /*   case TypeComponent: */
  /*   case TypeForeign: */
  /*     unimplemented; */
  /*   case TypeHeap: */
  /*     unimplemented; */

  /*   case TypeImmutable: */
  /*     unimplemented; */
  /*   case TypePointer: */
  /*   case TypePointerToForeign: */
  /*   case TypePointerToImmutable: */
  /*   case TypeReaction: */
  /*   case TypeFieldList: */
  /*   case TypeSignature: */
  /*   case TypeString: */
  /*     unimplemented; */
  /*   case TypeFunc: */
  /*     unimplemented; */
  /*   case TypeUint: */
  /*     unimplemented; */
  /*   case TypeStruct: */
  /*     unimplemented; */

  /*   case UntypedUndefined: */
  /*     unimplemented; */
  /*   case UntypedNil: */
  /*     unimplemented; */

  /*   case UntypedBool: */
  /*     unimplemented; */
  /*   case UntypedInteger: */
  /*     unimplemented; */
  /*   case UntypedString: */
  /*     unimplemented; */

  /*   } */

  /* bug ("unhandled case"); */
}

type_t *
type_make_field_list (void)
{
  type_t *t = make (&field_list_vtable, TypeFieldList);
  VECTOR_INIT (t->field_list.fields, field_t *, 0, NULL);
  return t;
}

void type_field_list_prepend (type_t* field_list,
                              string_t the_field_name, type_t* the_field_type)
{
  assert (field_list->kind == TypeFieldList);

  // Append a null and shift everything down.
  VECTOR_PUSH (field_list->field_list.fields, field_t*, NULL);

  {
    size_t idx;
    for (idx = VECTOR_SIZE (field_list->field_list.fields) - 1;
         idx > 0;
         --idx)
      {
        VECTOR_SET (field_list->field_list.fields, idx, VECTOR_AT (field_list->field_list.fields, idx - 1));
      }
  }

  // Prepend.
  VECTOR_SET (field_list->field_list.fields, 0, field_make (the_field_name, the_field_type, 0));

  // Recalculate all the offsets.
  field_list->field_list.offset = 0;
  field_list->field_list.alignment = 0;
  VECTOR_FOREACH (pos, limit, field_list->field_list.fields, field_t*)
    {
      field_t* field = *pos;
      size_t alignment = type_alignment (field_type (field));
      field_list->field_list.offset = align_up (field_list->field_list.offset, alignment);
      field_set_offset (field, field_list->field_list.offset);
      field_list->field_list.offset += type_size (field_type (field));
      if (alignment > field_list->field_list.alignment)
        {
          field_list->field_list.alignment = alignment;
        }
    }
}

void
type_field_list_append (type_t * field_list,
			string_t field_name, type_t * field_type)
{
  assert (field_list->kind == TypeFieldList);
  size_t alignment = type_alignment (field_type);
  field_list->field_list.offset =
    align_up (field_list->field_list.offset, alignment);

  field_t *field =
    field_make (field_name, field_type, field_list->field_list.offset);
  VECTOR_PUSH (field_list->field_list.fields, field_t *, field);

  field_list->field_list.offset += type_size (field_type);
  if (alignment > field_list->field_list.alignment)
    {
      field_list->field_list.alignment = alignment;
    }
}

field_t *
type_field_list_find (const type_t * type, string_t name)
{
  VECTOR_FOREACH (field, limit, type->field_list.fields, field_t *)
    {
      if (streq (name, field_name (*field)))
        {
          return (*field);
        }
    }
  return NULL;
}

type_t *
type_select (const type_t * type, string_t identifier)
{
  type_t* t = type->vtable->select (type, identifier);
  if (t != NULL)
    {
      return t;
    }

  {
    VECTOR_FOREACH (ptr, limit, type->methods, method_t *)
      {
        if (streq (identifier, method_name (*ptr)))
          {
            return method_type (*ptr);
          }
      }
  }

  return NULL;
}

field_t *
type_select_field (const type_t * type, string_t identifier)
{
  return type->vtable->select_field (type, identifier);
}

bool
type_is_pointer (const type_t * type)
{
  return type->kind == TypePointer;
}

bool type_is_pointer_to_foreign (const type_t * type)
{
  return type->kind == TypePointerToForeign;
}

bool type_is_pointer_to_immutable (const type_t * type)
{
  return type->kind == TypePointerToImmutable;
}

bool
type_equivalent (const type_t * x, const type_t* y)
{
  if (x->has_name || y->has_name)
    {
      // Named types must be exactly the same.
      return x == y;
    }

  if (x->kind != y->kind)
    {
      return false;
    }

  return x->vtable->is_equivalent (x, y);

  /* switch (x->kind) */
  /*   { */
  /*   case TypeImmutable: */
  /*     return type_equivalent (x->immutable.base_type, y->immutable.base_type); */
  /*   case TypeForeign: */
  /*     return type_equivalent (x->foreign.base_type, y->foreign.base_type); */
  /*   } */
}

bool
type_convertible (const type_t * to, const type_t * from)
{
  // We can always convert to immutable or foreign so strip them off.
  if (type_is_immutable (to))
    {
      to = to->immutable.base_type;
    }
  else if (type_is_foreign (to))
    {
      to = to->foreign.base_type;
    }

  // Check if we can strip immutable/foreign from from.
  if (type_is_immutable (from) && !type_leaks_mutable_pointers (from))
    {
      from = from->immutable.base_type;
    }
  else if (type_is_foreign (from) && !type_leaks_non_foreign_pointers (from))
    {
      from = from->foreign.base_type;
    }

  if (from->has_name)
    {
      // If from is named, to must be exactly the same.
      return to == from;
    }

  return to->vtable->is_convertible_from (to, from);

  /* switch (to->kind) */
  /*   { */
  /*   case TypeHeap: */
  /*     if (from->kind == TypeHeap) */
  /*       { */
  /*         return type_equivalent (to->heap.base_type, from->heap.base_type); */
  /*       } */
  /*     break; */

  /*   case UntypedNil: */
  /*     // It does not make sense to convert to nil. */
  /*     return false; */

  /*   } */

  /* return false; */
}

type_t *
type_make_signature (void)
{
  type_t *t = make (&signature_vtable, TypeSignature);
  VECTOR_INIT (t->signature.parameters, parameter_t *, 0, NULL);
  return t;
}

parameter_t *
type_signature_find (const type_t * signature, string_t name)
{
  VECTOR_FOREACH (ptr, limit, signature->signature.parameters, parameter_t *)
    {
      if (streq (parameter_name ((*ptr)), name))
        {
          return *ptr;
        }
    }
  return NULL;
}

void type_signature_prepend (type_t * signature, string_t parameter_name,
                             type_t * parameter_type, bool is_receiver)
{
  parameter_t *p = parameter_make (parameter_name, parameter_type, is_receiver);
  VECTOR_PUSH_FRONT (signature->signature.parameters, parameter_t *, p);
}

void
type_signature_append (type_t * signature, string_t parameter_name,
		       type_t * parameter_type, bool is_receiver)
{
  parameter_t *p = parameter_make (parameter_name, parameter_type, is_receiver);
  VECTOR_PUSH (signature->signature.parameters, parameter_t *, p);
}

parameter_t **
type_signature_begin (const type_t * signature)
{
  return VECTOR_BEGIN (signature->signature.parameters);
}

parameter_t **
type_signature_end (const type_t * signature)
{
  return VECTOR_END (signature->signature.parameters);
}

parameter_t **
type_signature_next (parameter_t ** pos)
{
  return pos + 1;
}

bool
type_callable (const type_t * type)
{
  return type->vtable->callable (type);

  /* switch (type->kind) */
  /*   { */
  /*   case TypePort: */
  /*     return true; */
  /*   } */
}

size_t
type_parameter_count (const type_t * type)
{
  return type->vtable->parameter_count (type);
}

type_t *
type_parameter_type (const type_t * type, size_t idx)
{
  return type->vtable->parameter_type (type, idx);
}

type_t *
type_return_type (const type_t * type)
{
  return type->vtable->return_type (type);
}

bool
type_is_reaction (const type_t * type)
{
  return type->kind == TypeReaction;
}

bool
type_is_signature (const type_t * type)
{
  return type->kind == TypeSignature;
}

type_t *
type_make_reaction (type_t * signature)
{
  type_t *r = make (&reaction_vtable, TypeReaction);
  r->reaction.signature = signature;
  return r;
}

type_t *
type_reaction_signature (const type_t * reaction)
{
  assert (reaction->kind == TypeReaction);
  return reaction->reaction.signature;
}

bool
type_compatible_port_reaction (const type_t * port, const type_t * reaction)
{
  assert (port->kind == TypePort);
  assert (reaction->kind == TypeReaction);

  const type_t *port_signature = port->port.signature;
  const type_t *reaction_signature = reaction->reaction.signature;

  size_t port_arity = type_signature_arity (port_signature);
  size_t reaction_arity = type_signature_arity (reaction_signature);
  if (port_arity != reaction_arity)
    {
      return false;
    }

  size_t idx;
  for (idx = 0; idx != port_arity; ++idx)
    {
      parameter_t *port_parameter =
	VECTOR_AT (port_signature->signature.parameters, idx);
      type_t *port_parameter_type = parameter_type (port_parameter);
      parameter_t *reaction_parameter =
	VECTOR_AT (reaction_signature->signature.parameters, idx);
      type_t *reaction_parameter_type = parameter_type (reaction_parameter);

      if (!type_convertible (port_parameter_type, reaction_parameter_type))
	{
	  return false;
	}
    }

  return true;
}


size_t
type_signature_arity (const type_t * signature)
{
  assert (signature->kind == TypeSignature);
  return VECTOR_SIZE (signature->signature.parameters);
}

type_t *type_make_func (const type_t * signature,
                        type_t * return_type)
{
  type_t *r = make (&func_vtable, TypeFunc);
  r->func.signature = signature;
  r->func.return_type = return_type;
  return r;
}

const type_t *type_func_signature (const type_t * func)
{
  assert (func->kind == TypeFunc);
  return func->func.signature;
}

type_t *type_func_return_type (const type_t * func)
{
  assert (func->kind == TypeFunc);
  return func->func.return_type;
}

field_t **
type_field_list_begin (const type_t * field_list)
{
  assert (field_list->kind == TypeFieldList);
  return VECTOR_BEGIN (field_list->field_list.fields);
}

field_t **
type_field_list_end (const type_t * field_list)
{
  assert (field_list->kind == TypeFieldList);
  return VECTOR_END (field_list->field_list.fields);
}

field_t **
type_field_list_next (field_t ** field)
{
  return field + 1;
}

const type_t *
type_component_field_list (const type_t * component)
{
  assert (component->kind == TypeComponent);
  return component->component.field_list;
}

void
type_component_add_binding (type_t * component, binding_t * binding)
{
  assert (component->kind == TypeComponent);
  VECTOR_PUSH (component->component.bindings, binding_t *, binding);
}

binding_t **
type_component_bindings_begin (const type_t * component)
{
  assert (component->kind == TypeComponent);
  return VECTOR_BEGIN (component->component.bindings);
}

binding_t **
type_component_bindings_end (const type_t * component)
{
  assert (component->kind == TypeComponent);
  return VECTOR_END (component->component.bindings);
}

binding_t **
type_component_bindings_next (binding_t ** pos)
{
  return pos + 1;
}

action_t **
type_component_actions_begin (const type_t * component)
{
  assert (component->kind == TypeComponent);
  return VECTOR_BEGIN (component->component.actions);
}

action_t **
type_component_actions_end (const type_t * component)
{
  assert (component->kind == TypeComponent);
  return VECTOR_END (component->component.actions);
}

action_t **
type_component_actions_next (action_t ** pos)
{
  return pos + 1;
}

void type_print_value (const type_t* type,
                       void* value)
{
  unimplemented;
  /* switch (type->kind) */
  /*   { */
  /*   case TypeUndefined: */
  /*     unimplemented; */
  /*   case TypeVoid: */
  /*     unimplemented; */
  /*   case TypeBool: */
  /*     { */
  /*       bool* b = value; */
  /*       if (*b) */
  /*         { */
  /*           printf ("true"); */
  /*         } */
  /*       else */
  /*         { */
  /*           printf ("false"); */
  /*         } */
  /*     } */
  /*     break; */
  /*   case TypeComponent: */
  /*     type_print_value (type->component.field_list, value); */
  /*     break; */

  /*   case TypeHeap: */
  /*     unimplemented; */

  /*   case TypeImmutable: */
  /*     unimplemented; */

  /*   case TypeForeign: */
  /*     unimplemented; */

  /*   case TypePointer: */
  /*   case TypePointerToForeign: */
  /*   case TypePointerToImmutable: */
  /*   case TypePort: */
  /*     { */
  /*       char** ptr = value; */
  /*       printf ("%p", *ptr); */
  /*     } */
  /*     break; */
  /*   case TypeReaction: */
  /*     unimplemented; */
  /*   case TypeFieldList: */
  /*     { */
  /*       printf ("{"); */
  /*       VECTOR_FOREACH (pos, limit, type->field_list.fields, field_t*) */
  /*         { */
  /*           const field_t* field = *pos; */
  /*           printf ("(%p)%s=", value + field_offset (field), get (field_name (field))); */
  /*           type_print_value (field_type (field), value + field_offset (field)); */
  /*           printf (","); */
  /*         } */
  /*       printf ("}"); */
  /*     } */
  /*     break; */
  /*   case TypeSignature: */
  /*     unimplemented; */
  /*   case TypeString: */
  /*     unimplemented; */
  /*   case TypeFunc: */
  /*     unimplemented; */

  /*   case TypeUint: */
  /*     printf ("%zd\n", *(uint64_t*)value); */
  /*     break; */

  /*   case TypeStruct: */
  /*     unimplemented; */

  /*   case UntypedUndefined: */
  /*     unimplemented; */
  /*   case UntypedNil: */
  /*     unimplemented; */

  /*   case UntypedBool: */
  /*     unimplemented; */
  /*   case UntypedInteger: */
  /*     unimplemented; */
  /*   case UntypedString: */
  /*     unimplemented; */

  /*   } */
}

type_t* type_make_string (void)
{
  return make (&string_vtable, TypeString);
}

type_t *
type_make_uint (void)
{
  return make (&uint_vtable, TypeUint);
}

bool type_is_arithmetic (const type_t* type)
{
  return type->vtable->is_arithmetic ();
  /* switch (type->kind) */
  /*   { */
  /*   case TypeUndefined: */
  /*     unimplemented; */
  /*   case TypeVoid: */
  /*     unimplemented; */
  /*   case TypeBool: */
  /*     unimplemented; */
  /*   case TypeComponent: */
  /*     unimplemented; */
  /*   case TypeForeign: */
  /*     unimplemented; */
  /*   case TypeHeap: */
  /*     unimplemented; */

  /*   case TypeImmutable: */
  /*     unimplemented; */


  /*   case TypePointer: */
  /*     unimplemented; */
  /*   case TypePointerToForeign: */
  /*     unimplemented; */
  /*   case TypePointerToImmutable: */
  /*     unimplemented; */

  /*   case TypePort: */
  /*     unimplemented; */
  /*   case TypeReaction: */
  /*     unimplemented; */
  /*   case TypeFieldList: */
  /*     unimplemented; */
  /*   case TypeSignature: */
  /*     unimplemented; */
  /*   case TypeString: */
  /*     unimplemented; */
  /*   case TypeFunc: */
  /*     unimplemented; */
  /*   case TypeUint: */
  /*     return true; */
  /*   case TypeStruct: */
  /*     unimplemented; */

  /*   case UntypedUndefined: */
  /*     unimplemented; */
  /*   case UntypedNil: */
  /*     unimplemented; */

  /*   case UntypedBool: */
  /*     unimplemented; */
  /*   case UntypedInteger: */
  /*     unimplemented; */
  /*   case UntypedString: */
  /*     unimplemented; */

  /*   } */
  /* not_reached; */
}


type_t *
type_make_struct (type_t * field_list)
{
  type_t *c = make (&struct_vtable, TypeStruct);
  c->struct_.field_list = field_list;
  return c;
}

bool type_is_struct (const type_t* type)
{
  return type->kind == TypeStruct;
}

type_t* type_make_untyped_nil (void)
{
  return make (&untyped_nil_vtable, UntypedNil);
}

type_t* type_make_untyped_bool (void)
{
  return make (&untyped_bool_vtable, UntypedBool);
}

type_t* type_make_untyped_string (void)
{
  return make (&untyped_string_vtable, UntypedString);
}

type_t* type_make_untyped_integer (void)
{
  return make (&untyped_integer_vtable, UntypedInteger);
}

bool type_is_untyped (const type_t* type)
{
  return type->vtable->is_untyped ();
}

type_t* type_make_immutable (type_t* type)
{
  // Don't chain immutables.
  if (type_is_immutable (type))
    {
      return type;
    }

  type_t* t = make (&immutable_vtable, TypeImmutable);
  t->immutable.base_type = type;
  return t;
}

bool type_is_immutable (const type_t* type)
{
  return type->kind == TypeImmutable;
}

type_t* type_immutable_base_type (const type_t* type)
{
  assert (type->kind == TypeImmutable);
  return type->immutable.base_type;
}

bool type_leaks_mutable_pointers (const type_t* type)
{
  return type->vtable->leaks_mutable_pointers (type);
}

bool type_leaks_non_foreign_pointers (const type_t* type)
{
  return type->vtable->leaks_non_foreign_pointers (type);
}

type_t* type_make_foreign (type_t* type)
{
  // Don't chain foreign.
  if (type_is_foreign (type))
    {
      return type;
    }

  type_t* t = make (&foreign_vtable, TypeForeign);
  t->foreign.base_type = type;
  return t;
}

bool type_is_foreign (const type_t* type)
{
  return type->kind == TypeForeign;
}

bool type_contains_foreign_pointers (const type_t* type)
{
  return internal_contains_foreign_pointers (type, NULL);
}

type_t* type_make_heap (type_t* type)
{
  type_t* t = make (&heap_vtable, TypeHeap);
  t->heap.base_type = type;
  return t;
}

bool type_copyable (const type_t* type)
{
  return type->vtable->copyable (type);
}

bool type_movable (const type_t* type)
{
  return type->vtable->movable (type);
}

bool type_is_heap (const type_t* type)
{
  return type->kind == TypeHeap;
}

type_t* type_heap_base_type (const type_t* type)
{
  assert (type_is_heap (type));
  return type->heap.base_type;
}

bool type_can_dereference (const type_t* type)
{
  return type->vtable->can_dereference (type);
}

type_t* type_foreign_base_type (const type_t* type)
{
  assert (type_is_foreign (type));
  return type->foreign.base_type;
}

bool type_is_any_pointer (const type_t* type)
{
  return type->kind == TypePointer ||
    type->kind == TypePointerToForeign ||
    type->kind == TypePointerToImmutable;
}

type_t *type_dereference (const type_t* type)
{
  assert (type_can_dereference (type));
  return type->vtable->dereference (type);
}

bool type_can_assign (const type_t* type)
{
  return type->vtable->can_assign (type);
}

bool type_is_boolean (const type_t* type)
{
  return type->vtable->is_boolean (type);
}

bool type_is_moveable (const type_t* type)
{
  if (type_is_pointer_to_foreign (type))
    {
      const type_t* b = type_pointer_base_type (type);
      if (type_is_heap (b))
        {
          return true;
        }
    }
  return false;
}

type_t* type_move (const type_t* type)
{
  assert (type_is_moveable (type));
  return type_make_pointer (type_make_heap (type_heap_base_type (type_pointer_base_type (type))));
}

bool type_is_mergeable (const type_t* type)
{
  return type_is_moveable (type);
}

type_t* type_merge (const type_t* type)
{
  assert (type_is_mergeable (type));
  return type_make_pointer (type_heap_base_type (type_pointer_base_type (type)));
}

bool type_is_changeable (const type_t* type)
{
  if (type_is_pointer (type))
    {
      const type_t* b = type_pointer_base_type (type);
      if (type_is_heap (b))
        {
          return true;
        }
    }
  return false;
}

bool type_is_valid_root (const type_t* proposed, const type_t* change_type)
{
  assert (type_is_changeable (change_type));

  if (!type_is_pointer (proposed))
    {
      return false;
    }
  proposed = type_pointer_base_type (proposed);
  change_type = type_heap_base_type (type_pointer_base_type (change_type));
  return type_equivalent (proposed, change_type);
}
