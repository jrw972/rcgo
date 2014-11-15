#define _GNU_SOURCE
#include "type.h"
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "util.h"
#include "action.h"
#include "parameter.h"
#include "field.h"

struct type_t
{
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
    } component;
    struct
    {
      VECTOR_DECL (fields, field_t *);
      ptrdiff_t offset;
      size_t alignment;
    } field_list;
    struct
    {
      type_t *base_type;
      PointerKind kind;
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
  };
};

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
	case TypePointer:
	  {
	    char *str;
	    asprintf (&str, "$%s", type_to_string (type->pointer.base_type));
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
    }

  free (from);
}

size_t
type_size (const type_t * type)
{
  switch (type->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      return 1;
    case TypeComponent:
      return type_size (type->component.field_list);
    case TypePointer:
      return 8;
    case TypePort:
      return 8;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      return type->field_list.offset;
    case TypeSignature:
      unimplemented;
    }

  not_reached;
}

static type_t *
make (TypeKind kind)
{
  type_t *retval = xmalloc (sizeof (type_t));
  retval->kind = kind;
  return retval;
}

static type_t *
duplicate (const type_t * type)
{
  type_t *retval = make (type->kind);
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
  return make (TypeUndefined);
}

bool
type_is_undefined (const type_t * type)
{
  return type->kind == TypeUndefined;
}

type_t *
type_make_bool (void)
{
  return make (TypeBool);
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
      t = make (TypeVoid);
    }
  return t;
}

type_t *
type_make_component (type_t * field_list)
{
  type_t *c = make (TypeComponent);
  /* Prepend the field list with a pointer for the runtime. */
  type_field_list_prepend (field_list, enter ("0"), type_make_pointer (PointerToMutable, make_void ()));
  c->component.field_list = field_list;
  VECTOR_INIT (c->component.actions, action_t *, 0, NULL);
  VECTOR_INIT (c->component.reactions, action_t *, 0, NULL);
  VECTOR_INIT (c->component.bindings, binding_t *, 0, NULL);
  return c;
}

type_t *
type_make_pointer (PointerKind kind, type_t * base_type)
{
  type_t *t = make (TypePointer);
  t->pointer.base_type = base_type;
  t->pointer.kind = kind;
  return t;
}

type_t *
type_make_port (const type_t * signature)
{
  type_t *retval = make (TypePort);
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

static size_t
type_alignment (const type_t * type)
{
  switch (type->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      return 1;
    case TypeComponent:
      return type_alignment (type->component.field_list);
    case TypePointer:
      return 8;
    case TypePort:
      return 8;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      return type->field_list.alignment;
    case TypeSignature:
      unimplemented;
    }
  not_reached;
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
    case TypeVoid:
      unimplemented;
    case TypeBool:
      return untyped_value_is_bool (u);
    case TypeComponent:
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
    case TypeVoid:
      unimplemented;
    case TypeBool:
      return target == source;
    case TypeComponent:
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

bool
type_is_boolean (const type_t * type)
{
  return type->kind == TypeBool;
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
    case TypePointer:
    case TypeReaction:
    case TypeFieldList:
    case TypeSignature:
      return false;
    }

  bug ("unhandled case");
}

type_t *
type_make_field_list (void)
{
  type_t *t = make (TypeFieldList);
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
  switch (type->kind)
    {
    case TypeUndefined:
      unimplemented;
    case TypeVoid:
      unimplemented;
    case TypeBool:
      unimplemented;
    case TypeComponent:
      {
	type_t *t = type_select (type->component.field_list, identifier);
	if (t != NULL)
	  {
	    return t;
	  }

	VECTOR_FOREACH (ptr, limit, type->component.reactions, action_t *)
	{
	  if (streq (identifier, reaction_name (*ptr)))
	    {
	      return reaction_type (*ptr);
	    }
	}

	return NULL;
      }
    case TypePointer:
      unimplemented;
    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
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
    case TypeSignature:
      unimplemented;
    }

  bug ("unhandled case");
}

field_t *
type_select_field (const type_t * type, string_t identifier)
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
      return type_select_field (type->component.field_list, identifier);
    case TypePointer:
      unimplemented;
    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    case TypeFieldList:
      return type_field_list_find (type, identifier);
    case TypeSignature:
      unimplemented;
    }

  bug ("unhandled case");
}

bool
type_is_pointer (const type_t * type)
{
  return type->kind == TypePointer;
}

PointerKind
type_pointer_kind (const type_t * type)
{
  assert (type->kind == TypePointer);
  return type->pointer.kind;
}

bool
type_equivalent (const type_t * left, const type_t * right)
{
  if (left->has_name || right->has_name)
    {
      return left == right;
    }

  unimplemented;
}

type_t *
type_make_signature (void)
{
  type_t *t = make (TypeSignature);
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

void
type_signature_append (type_t * signature, string_t parameter_name,
		       type_t * parameter_type)
{
  parameter_t *p = parameter_make (parameter_name, parameter_type);
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
  type_t *r = make (TypeReaction);
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

      if (!type_equivalent (port_parameter_type, reaction_parameter_type))
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
    }
}
