#define _GNU_SOURCE
#include "type.h"
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "util.h"
#include "action.h"
#include "parameter.h"
#include "field.h"
#include "func.h"

typedef struct {
  type_t * (*select) (const type_t * type, string_t identifier);
  field_t * (*select_field) (const type_t * type, string_t identifier);
  bool (*leaks_mutable_pointers) (const type_t * type);
  size_t (*alignment) (const type_t * type);
  size_t (*size) (const type_t* type);
} vtable_t;

static type_t* no_select (const type_t* type, string_t identifier)
{
  unimplemented;
}

static field_t* no_select_field (const type_t* type, string_t identifier)
{
  unimplemented;
}

static bool does_not_leak_mutable_pointers (const type_t* type)
{
  return false;
}

static size_t no_alignment (const type_t* type)
{
  unimplemented;
}

static size_t no_size (const type_t* type)
{
  unimplemented;
}

struct type_t
{
  const vtable_t* vtable;
  TypeKind kind;
  bool has_name;
  string_t name;
  union
  {
    struct
    {
      type_t *field_list;
      VECTOR_DECL (actions, action_t *);
      VECTOR_DECL (reactions, action_t *);
      VECTOR_DECL (bindings, binding_t *);
      VECTOR_DECL (funcs, func_t *);
    } component;
    struct
    {
      VECTOR_DECL (fields, field_t *);
      ptrdiff_t offset;
      size_t alignment;
    } field_list;
    struct
    {
      const type_t *signature;
      type_t *return_type;
    } func;
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

static size_t bool_alignment (const type_t* type)
{
  return 1;
}

static size_t bool_size (const type_t* type)
{
  return 1;
}

static vtable_t bool_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, bool_alignment, bool_size };

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

  {
    VECTOR_FOREACH (ptr, limit, type->component.funcs, func_t *)
      {
        if (streq (identifier, func_name (*ptr)))
          {
            return func_type (*ptr);
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

static vtable_t component_vtable = { component_select, component_select_field, component_leaks_mutable_pointers, component_alignment, component_size };

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

static size_t field_list_alignment (const type_t* type)
{
  return type->field_list.alignment;
}

static size_t field_list_size (const type_t* type)
{
  return type->field_list.offset;
}

static vtable_t field_list_vtable = { field_list_select, field_list_select_field, field_list_leaks_mutable_pointers, field_list_alignment, field_list_size };

static vtable_t func_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, no_alignment, no_size };

static type_t* immutable_select (const type_t* type, string_t identifier)
{
  return type_select (type->immutable.base_type, identifier);
}

static field_t* immutable_select_field (const type_t* type, string_t identifier)
{
  return type_select_field (type->immutable.base_type, identifier);
}

static size_t immutable_alignment (const type_t* type)
{
  return type_alignment (type->immutable.base_type);
}

static size_t immutable_size (const type_t* type)
{
  return type_size (type->immutable.base_type);
}

// Does not leak mutable pointers by definition.
static vtable_t immutable_vtable = { immutable_select, immutable_select_field, does_not_leak_mutable_pointers, immutable_alignment, immutable_size };

static bool pointer_leaks_mutable_pointers (const type_t* type)
{
  return !type_is_immutable (type->pointer.base_type);
}

static size_t pointer_alignment (const type_t* type)
{
  return sizeof (void*);
}

static size_t pointer_size (const type_t* type)
{
  return sizeof (void*);
}

static vtable_t pointer_vtable = { no_select, no_select_field, pointer_leaks_mutable_pointers, pointer_alignment, pointer_size };

static size_t port_alignment (const type_t* type)
{
  return sizeof (void*);
}

static size_t port_size (const type_t* type)
{
  return sizeof (void*);
}

static vtable_t port_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, port_alignment, port_size };

static vtable_t reaction_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, no_alignment, no_size };

static vtable_t signature_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, no_alignment, no_size };

static vtable_t string_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, no_alignment, no_size };

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

static size_t struct_alignment (const type_t* type)
{
  return type_alignment (type->struct_.field_list);
}

static size_t struct_size (const type_t* type)
{
  return type_size (type->struct_.field_list);
}

static vtable_t struct_vtable = { struct_select, struct_select_field, struct_leaks_mutable_pointers, struct_alignment, struct_size };

static vtable_t undefined_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, no_alignment, no_size };

static size_t uint_alignment (const type_t* type)
{
  return sizeof (void*);
}

static size_t uint_size (const type_t* type)
{
  return sizeof (void*);
}

static vtable_t uint_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, uint_alignment, uint_size };

static vtable_t void_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, no_alignment, no_size };

static vtable_t untyped_bool_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, no_alignment, no_size };

static vtable_t untyped_integer_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, no_alignment, no_size };

static vtable_t untyped_nil_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, no_alignment, no_size };

static vtable_t untyped_string_vtable = { no_select, no_select_field, does_not_leak_mutable_pointers, no_alignment, no_size };

// TODO:  Replace functions with interface.

/* static const char* */
/* parameter_array_to_string (const parameter_t* begin, */
/*                            const parameter_t* end, */
/*                            bool first) */
/* { */
/*   if (begin == end) { */
/*     return ""; */
/*   } */

/*   const char* format; */
/*   if (first) { */
/*     format = "%s %s%s"; */
/*   } */
/*   else { */
/*     format = ", %s %s%s"; */
/*   } */

/*   char* str; */
/*   asprintf (&str, format, get (begin->name), type_to_string (begin->type), parameter_array_to_string (begin + 1, end, false)); */

/*   return str; */
/* } */

const char *
type_to_string (const type_t * type)
{
  if (type->has_name)
    {
      return get (type->name);
    }
  else
    {
      switch (type->kind)
	{
	case TypeUndefined:
	  return "undefined";
	case TypeVoid:
	  unimplemented;
	case TypeBool:
	  unimplemented;
	case TypeComponent:
	  unimplemented;

        case TypeImmutable:
	  {
	    char *str;
	    asprintf (&str, "$%s", type_to_string (type->immutable.base_type));
	    return str;
	  }
	case TypePointer:
	  {
	    char *str;
	    asprintf (&str, "@%s", type_to_string (type->pointer.base_type));
	    return str;
	  }
	case TypePort:
	  {
	    char *str;
	    asprintf (&str, "port %s", type_to_string (type->port.signature));
	    return str;
	  }
	case TypeReaction:
	  {
	    char *str;
	    asprintf (&str, "reaction %s",
		      type_to_string (type->reaction.signature));
	    return str;
	  }
	case TypeFieldList:
	  unimplemented;
	case TypeSignature:
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
        case TypeString:
          unimplemented;
        case TypeFunc:
          {
            char *str;
            asprintf (&str, "func %s %s", type_to_string (type->func.signature), type_to_string (type->func.return_type));
            return str;
          }
        case TypeUint:
          unimplemented;
        case TypeStruct:
          unimplemented;

        case UntypedUndefined:
          unimplemented;

        case UntypedNil:
          return "nil";

        case UntypedBool:
          return "bool";
        case UntypedInteger:
          unimplemented;
        case UntypedString:
          unimplemented;

	}
    }

  bug ("unhandled case");
}

void
type_move (type_t * to, type_t * from)
{
  assert (to->kind == TypeUndefined);
  assert (from->kind != TypeUndefined);

  to->vtable = from->vtable;
  to->kind = from->kind;
  /* Do not copy has_name and name. */

  switch (from->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      unimplemented;
    case TypeComponent:
      to->component.field_list = from->component.field_list;
      VECTOR_MOVE (to->component.actions, from->component.actions);
      VECTOR_MOVE (to->component.reactions, from->component.reactions);
      VECTOR_MOVE (to->component.bindings, from->component.bindings);
      break;
    case TypeImmutable:
      unimplemented;

    case TypePointer:
      unimplemented;
    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeSignature:
      unimplemented;
    case TypeString:
      unimplemented;
    case TypeFunc:
      unimplemented;
    case TypeUint:
      unimplemented;
    case TypeStruct:
      to->struct_.field_list = from->struct_.field_list;
      break;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }

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
  return retval;
}

static type_t *
duplicate (const type_t * type)
{
  type_t *retval = make (type->vtable, type->kind);
  switch (retval->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      unimplemented;
    case TypeComponent:
      unimplemented;
        case TypeImmutable:
          unimplemented;

    case TypePointer:
      unimplemented;
    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeSignature:
      unimplemented;
    case TypeString:
      unimplemented;
    case TypeFunc:
      unimplemented;
    case TypeUint:
      unimplemented;
    case TypeStruct:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }
  return retval;
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
  assert (type->kind == TypePointer);
  return type->pointer.base_type;
}

static type_t *
make_void ()
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
  type_field_list_prepend (field_list, enter ("0"), type_make_immutable (type_make_pointer (make_void ())));
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
  action_t *action = action_make (type, node);
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

func_t *type_component_add_func (type_t * component_type,
                                     ast_t* node,
                                     string_t identifier,
                                     type_t * signature,
                                     type_t * return_type)
{
  assert (type_is_component (component_type));
  func_t *g =
    func_make (component_type,
                 node,
                 identifier, signature, return_type);
  VECTOR_PUSH (component_type->component.funcs, func_t*, g);
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

func_t *type_component_get_func (const type_t * component_type,
                                     string_t identifier)
{
  assert (component_type->kind == TypeComponent);

  VECTOR_FOREACH (pos, limit, component_type->component.funcs, func_t *)
    {
      func_t *a = *pos;
      if (streq (func_name (a), identifier))
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
  switch (type->kind)
    {
    case TypePort:
      return make_void ();
    case TypeUndefined:
    case TypeVoid:
    case TypeBool:
    case TypeComponent:
        case TypeImmutable:
          unimplemented;
    case TypePointer:
    case TypeReaction:
    case TypeFieldList:
    case TypeSignature:
    case TypeString:
      return false;
    case TypeFunc:
      unimplemented;
    case TypeUint:
      unimplemented;
    case TypeStruct:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }

  bug ("unhandled case");
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
  return type->vtable->select (type, identifier);
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

static bool
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

  switch (x->kind)
    {
    case TypeBool:
      unimplemented;
    case TypeComponent:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeFunc:
      unimplemented;

    case TypeImmutable:
      return type_equivalent (x->immutable.base_type, y->immutable.base_type);

    case TypePointer:
      return type_equivalent (x->pointer.base_type, y->pointer.base_type);;

    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    case TypeSignature:
      unimplemented;
    case TypeString:
      unimplemented;
    case TypeStruct:
      unimplemented;
    case TypeUint:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeUndefined:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;
    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;
    }

  not_reached;
}

bool
type_convertible (const type_t * to, const type_t * from)
{
  if (from->has_name)
    {
      // If from is named, to must be exactly the same.
      return to == from;
    }

  switch (to->kind)
    {
    case TypeUndefined:
      unimplemented;

    case TypeVoid:
      unimplemented;

    case TypeBool:
      // Converting to bool.
      return from->kind == UntypedBool;

    case TypeComponent:
      unimplemented;

    case TypeImmutable:
      unimplemented;

    case TypePointer:
      // Converting to a pointer.
      if (from->kind == UntypedNil)
        {
          return true;
        }
      if (from->kind == TypePointer)
        {
          // Converting from a pointer.
          if (type_equivalent (to->pointer.base_type, from->pointer.base_type))
            {
              return true;
            }
          // Maybe to is immutable.
          if (type_equivalent (to->pointer.base_type, type_make_immutable (from->pointer.base_type)))
            {
              return true;
            }
          return false;
        }
      break;

    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeSignature:
      unimplemented;
    case TypeString:
      unimplemented;
    case TypeFunc:
      unimplemented;

    case TypeUint:
      // Converting to uint.
      return from->kind == UntypedInteger;

    case TypeStruct:
      unimplemented;

    case UntypedUndefined:
      unimplemented;

    case UntypedNil:
      // It does not make sense to convert to nil.
      return false;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }

  return false;
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
  switch (type->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      unimplemented;
    case TypeComponent:
      unimplemented;
        case TypeImmutable:
          unimplemented;

    case TypePointer:
      unimplemented;
    case TypePort:
      return true;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeSignature:
      unimplemented;
    case TypeString:
      unimplemented;
    case TypeFunc:
      return true;
    case TypeUint:
      unimplemented;
    case TypeStruct:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }
  not_reached;
}

bool type_called_with_receiver (const type_t * type)
{
  switch (type->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      unimplemented;
    case TypeComponent:
      unimplemented;
        case TypeImmutable:
          unimplemented;

    case TypePointer:
      unimplemented;
    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeSignature:
      unimplemented;
    case TypeString:
      unimplemented;
    case TypeFunc:
      return true;
    case TypeUint:
      unimplemented;
    case TypeStruct:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }
  not_reached;
}

size_t
type_parameter_count (const type_t * type)
{
  switch (type->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      unimplemented;
    case TypeComponent:
      unimplemented;
        case TypeImmutable:
          unimplemented;

    case TypePointer:
      unimplemented;
    case TypePort:
      return type_parameter_count (type->port.signature);
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeSignature:
      return VECTOR_SIZE (type->signature.parameters);
    case TypeString:
      unimplemented;
    case TypeFunc:
      return type_parameter_count (type->func.signature);
    case TypeUint:
      unimplemented;
    case TypeStruct:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }
  not_reached;
}

type_t *
type_parameter_type (const type_t * type, size_t idx)
{
  switch (type->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      unimplemented;
    case TypeComponent:
      unimplemented;
        case TypeImmutable:
          unimplemented;

    case TypePointer:
      unimplemented;
    case TypePort:
      return type_parameter_type (type->port.signature, idx);
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeSignature:
      return parameter_type (VECTOR_AT (type->signature.parameters, idx));
    case TypeString:
      unimplemented;
    case TypeFunc:
      return type_parameter_type (type->func.signature, idx);
    case TypeUint:
      unimplemented;
    case TypeStruct:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }
  not_reached;
}

type_t *
type_return_type (const type_t * type)
{
  switch (type->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      unimplemented;
    case TypeComponent:
      unimplemented;
        case TypeImmutable:
          unimplemented;

    case TypePointer:
      unimplemented;
    case TypePort:
      return make_void ();
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeSignature:
      unimplemented;
    case TypeString:
      unimplemented;
    case TypeFunc:
      return type->func.return_type;
    case TypeUint:
      unimplemented;
    case TypeStruct:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }
  not_reached;
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
  switch (type->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      {
        bool* b = value;
        if (*b)
          {
            printf ("true");
          }
        else
          {
            printf ("false");
          }
      }
      break;
    case TypeComponent:
      type_print_value (type->component.field_list, value);
      break;
        case TypeImmutable:
          unimplemented;

    case TypePointer:
    case TypePort:
      {
        char** ptr = value;
        printf ("%p", *ptr);
      }
      break;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      {
        printf ("{");
        VECTOR_FOREACH (pos, limit, type->field_list.fields, field_t*)
          {
            const field_t* field = *pos;
            printf ("(%p)%s=", value + field_offset (field), get (field_name (field)));
            type_print_value (field_type (field), value + field_offset (field));
            printf (",");
          }
        printf ("}");
      }
      break;
    case TypeSignature:
      unimplemented;
    case TypeString:
      unimplemented;
    case TypeFunc:
      unimplemented;

    case TypeUint:
      printf ("%zd\n", *(uint64_t*)value);
      break;

    case TypeStruct:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }
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
  switch (type->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      unimplemented;
    case TypeComponent:
      unimplemented;
        case TypeImmutable:
          unimplemented;
    case TypePointer:
      unimplemented;
    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      unimplemented;
    case TypeSignature:
      unimplemented;
    case TypeString:
      unimplemented;
    case TypeFunc:
      unimplemented;
    case TypeUint:
      return true;
    case TypeStruct:
      unimplemented;

    case UntypedUndefined:
      unimplemented;
    case UntypedNil:
      unimplemented;

    case UntypedBool:
      unimplemented;
    case UntypedInteger:
      unimplemented;
    case UntypedString:
      unimplemented;

    }
  not_reached;
}


type_t *
type_make_struct (type_t * field_list)
{
  type_t *c = make (&struct_vtable, TypeStruct);
  c->struct_.field_list = field_list;
  return c;
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
  switch (type->kind)
    {
    case TypeBool:
    case TypeComponent:
    case TypeFieldList:
    case TypeFunc:
    case TypeImmutable:
    case TypePointer:
    case TypePort:
    case TypeReaction:
    case TypeSignature:
    case TypeString:
    case TypeStruct:
    case TypeUint:
    case TypeVoid:
    case TypeUndefined:
      return false;

    case UntypedUndefined:
    case UntypedNil:
    case UntypedBool:
    case UntypedInteger:
    case UntypedString:
      return true;
    }
  not_reached;
}

type_t* type_make_immutable (type_t* type)
{
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
