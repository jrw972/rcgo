#define _GNU_SOURCE
#include "type.h"
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "util.h"

struct field_t
{
  string_t name;
  const type_t *type;
  size_t number;
  /* TODO:  Remove the next pointer and convert to vector. */
  field_t *next;
};

typedef struct parameter_t parameter_t;
struct parameter_t
{
  string_t name;
  const type_t *type;
};

struct trigger_group_t
{
  const field_t** fields;
  size_t fields_size;
  size_t fields_capacity;
};

struct action_t
{
  bool is_reaction;
  size_t number;
  const type_t *component_type; /* Back-pointer to component type. */
  trigger_group_t* trigger_groups;
  size_t trigger_groups_size;
  size_t trigger_groups_capacity;
  action_t* next;
  /* Reactions only. */
  string_t name;
  const type_t *reaction_type;
};

struct binding_t
{
  select_result_t* outputs;
  size_t outputs_size;
  size_t outputs_capacity;
  select_result_t* inputs;
  size_t inputs_size;
  size_t inputs_capacity;
  binding_t* next;
};

static field_t *
make_field (string_t name, const type_t * type, size_t number)
{
  field_t *retval = malloc (sizeof (field_t));
  memset (retval, 0, sizeof (field_t));
  retval->name = name;
  retval->type = type;
  retval->number = number;
  return retval;
}

struct signature_t {
  parameter_t* parameters;
  size_t parameters_size;
  size_t parameters_capacity;
};

struct type_t
{
  TypeKind kind;
  bool has_name;
  string_t name;
  union
  {
    struct
    {
      field_t *field_list;
      size_t action_reaction_count;
      size_t bind_count;
      action_t* actions;
      size_t field_count;
      binding_t* bindings;
    } component;
    struct
    {
      const type_t *base_type;
    } pointer;
    struct
    {
      signature_t* signature;
    } port;
    struct
    {
      signature_t* signature;
      action_t* reaction;
    } reaction;
  };
};

static const char*
parameter_array_to_string (const parameter_t* begin,
                           const parameter_t* end,
                           bool first)
{
  if (begin == end) {
    return "";
  }

  const char* format;
  if (first) {
    format = "%s %s%s";
  }
  else {
    format = ", %s %s%s";
  }

  char* str;
  asprintf (&str, format, get (begin->name), type_to_string (begin->type), parameter_array_to_string (begin + 1, end, false));

  return str;
}

static const char*
signature_to_string (const signature_t* signature)
{
  char* str;
  asprintf (&str, "(%s)", parameter_array_to_string (signature->parameters, signature->parameters + signature->parameters_size, true));
  return str;
}

const char* type_to_string (const type_t* type)
{
  if (type->has_name) {
    return get (type->name);
  }
  else {
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
          char* str;
          asprintf (&str, "#%s", type_to_string (type->pointer.base_type));
          return str;
        }
      case TypePort:
        {
          char* str;
          asprintf (&str, "port %s", signature_to_string (type->port.signature));
          return str;
        }
      case TypeReaction:
        {
          char* str;
          asprintf (&str, "reaction %s", signature_to_string (type->reaction.signature));
          return str;
        }
      }
  }

  bug ("unhandled case");
}

static type_t *
make (TypeKind kind)
{
  type_t *retval = malloc (sizeof (type_t));
  memset (retval, 0, sizeof (type_t));
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

bool type_is_undefined (const type_t* type)
{
  return type->kind == TypeUndefined;
}

const type_t *
type_make_bool (void)
{
  return make (TypeBool);
}

const type_t *
type_pointer_base_type (const type_t * type)
{
  assert (type->kind == TypePointer);
  return type->pointer.base_type;
}

type_t *
type_make_component (void)
{
  return make (TypeComponent);
}

type_t * type_make_pointer (const type_t* base_type)
{
  type_t* t = make (TypePointer);
  t->pointer.base_type = base_type;
  return t;
}

type_t *type_make_port (signature_t* signature)
{
  type_t* retval = make (TypePort);
  retval->port.signature = signature;
  return retval;
}

bool type_is_port (const type_t* type)
{
  return type->kind == TypePort;
}

bool
type_is_component (const type_t * type)
{
  return type->kind == TypeComponent;
}

static action_t*
make_action (const type_t* type,
             bool is_reaction)
{
  assert (type_is_component (type));
  action_t* action = malloc (sizeof (action_t));
  memset (action, 0, sizeof (action_t));
  action->number = ((type_t *) type)->component.action_reaction_count++;
  action->is_reaction = is_reaction;
  action->component_type = type;
  action->trigger_groups = malloc (sizeof (trigger_group_t));
  action->trigger_groups_size = 0;
  action->trigger_groups_capacity = 1;
  action->next = type->component.actions;
  ((type_t *) type)->component.actions = action;
  return action;
}

action_t*
type_add_action (const type_t * type)
{
  return make_action (type, false);
}

bool action_is_action (const action_t* action)
{
  return !action->is_reaction;
}

bool action_is_reaction (const action_t* action)
{
  return action->is_reaction;
}

size_t action_number (const action_t* action)
{
  return action->number;
}

size_t
type_action_count (const type_t * type)
{
  assert (type_is_component (type));
  size_t count = 0;
  action_t* action;
  for (action = type->component.actions; action != NULL; action = action->next)
    {
      if (action_is_action (action))
        {
          ++count;
        }
    }
  return count;
}

size_t type_add_bind (const type_t* type)
{
  assert (type_is_component (type));
  return ((type_t*)type)->component.bind_count++;
}

size_t
type_bind_count (const type_t * type)
{
  assert (type_is_component (type));
  return type->component.bind_count;
}

bool
type_append_field (type_t * type, string_t field_name,
		   const type_t * field_type)
{
  assert (type->kind == TypeComponent);

  field_t **ptr;
  for (ptr = &(type->component.field_list); *ptr != NULL; ptr = &(*ptr)->next)
    {
      if (streq (field_name, (*ptr)->name))
	{
	  /* Duplicate field name. */
	  return false;
	}
    }

  *ptr = make_field (field_name, field_type, type->component.field_count++);
  return true;
}

const field_t *
type_field_list (const type_t * type)
{
  assert (type->kind == TypeComponent);
  return type->component.field_list;
}

string_t
field_name (const field_t * field)
{
  return field->name;
}

const type_t *
field_type (const field_t * field)
{
  return field->type;
}

size_t field_number (const field_t * field)
{
  return field->number;
}

const field_t *
field_next (const field_t * field)
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

const type_t *
type_dereference (const type_t * type)
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
      return type->pointer.base_type;
    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    }

  bug ("unhandled case");
}

static select_result_t
find_field_or_reaction_or_undefined (const field_t * field,
                                     const action_t * reaction,
                                     string_t identifier)
{
  select_result_t retval;

  /* Search through the fields. */
  for (; field != NULL; field = field->next)
    {
      if (streq (identifier, field->name))
	{
          retval.kind = SelectResultField;
          retval.field = field;
	  return retval;
	}
    }

  /* Search through the reactions. */
  for (; reaction != NULL; reaction = reaction->next)
    {
      if (action_is_reaction (reaction) && streq (identifier, reaction->name))
	{
          retval.kind = SelectResultReaction;
          retval.reaction = reaction;
          return retval;
	}
    }

  retval.kind = SelectResultUndefined;
  return retval;
}

select_result_t
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
      return find_field_or_reaction_or_undefined (type->component.field_list,
                                                  type->component.actions,
                                                  identifier);
    case TypePointer:
      unimplemented;
    case TypePort:
      unimplemented;
    case TypeReaction:
      unimplemented;
    }

  bug ("unhandled case");
}

select_result_t select_result_make_undefined ()
{
  select_result_t retval;
  retval.kind = SelectResultUndefined;
  return retval;
}

bool
type_is_boolean (const type_t * type)
{
  return type->kind == TypeBool;
}

action_t* type_get_reaction (const type_t * component_type, string_t identifier)
{
  assert (component_type->kind == TypeComponent);

  action_t* r;
  for (r = component_type->component.actions; r != NULL; r = r->next) {
    if (action_is_reaction (r) && streq (r->name, identifier)) {
      return r;
    }
  }

  return NULL;
}

static type_t*
make_reaction_type (signature_t* signature,
                    action_t* reaction)
{
  assert (action_is_reaction (reaction));
  type_t* t = make (TypeReaction);
  t->reaction.signature = signature;
  t->reaction.reaction = reaction;
  return t;
}

action_t* type_add_reaction (const type_t * component_type, string_t identifier, signature_t* signature)
{
  action_t* r = make_action (component_type, true);
  r->name = identifier;
  r->reaction_type = make_reaction_type (signature, r);
  return r;
}

const type_t* reaction_type (const action_t* reaction)
{
  return reaction->reaction_type;
}

string_t reaction_name (const action_t* reaction)
{
  return reaction->name;
}

const type_t* reaction_component_type (const action_t* reaction)
{
  return reaction->component_type;
}

signature_t* signature_make (void)
{
  signature_t* s = xmalloc (sizeof (signature_t));
  VECTOR_INIT (s->parameters, parameter_t);
  return s;
}

static parameter_t make_parameter (string_t name, const type_t* type)
{
  parameter_t retval = { name:name, type:type };
  return retval;
}

bool signature_add_parameter (signature_t* signature,
                              string_t name,
                              const type_t* type)
{
  size_t idx;
  for (idx = 0; idx != signature->parameters_size; ++idx)
    {
      if (streq (name, signature->parameters[idx].name))
	{
	  /* Duplicate parameter name. */
	  return false;
	}
    }

  VECTOR_PUSH (signature->parameters, parameter_t, make_parameter (name, type));

  return true;
}

size_t signature_size (const signature_t* signature)
{
  return signature->parameters_size;
}

const type_t* signature_type (const signature_t* signature, size_t idx)
{
  return signature->parameters[idx].type;
}

string_t signature_name (const signature_t* signature, size_t idx)
{
  return signature->parameters[idx].name;
}

static bool
compatible_signatures (const signature_t* output, const signature_t* input)
{
  if (output->parameters_size != input->parameters_size)
    {
      return false;
    }

  size_t idx, limit;
  for (idx = 0, limit = output->parameters_size; idx != limit; ++idx)
    {
      if (output->parameters[idx].type != input->parameters[idx].type)
        {
          return false;
        }
    }

  return true;
}

bool type_is_bindable (const type_t* output, const type_t* input)
{
  if (output->kind != TypePort) {
    return false;
  }

  if (input->kind != TypeReaction) {
    return false;
  }

  return compatible_signatures (output->port.signature, input->reaction.signature);
}

action_t* type_reaction_get_reaction (const type_t* type)
{
  if (type->kind == TypeReaction) {
    return type->reaction.reaction;
  }
  else {
    return NULL;
  }
}

action_t* type_actions_begin (const type_t* type)
{
  assert (type_is_component (type));
  return type->component.actions;
}

action_t* type_actions_end (const type_t* type)
{
  assert (type_is_component (type));
  return NULL;
}

action_t* action_next (action_t* action)
{
  return action->next;
}

static trigger_group_t* trigger_group_init (trigger_group_t* group)
{
  VECTOR_INIT (group->fields, field_t*);
  return group;
}

trigger_group_t* action_add_trigger_group (action_t* action)
{
  VECTOR_ENSURE_ONE (action->trigger_groups, trigger_group_t);
  return trigger_group_init (action->trigger_groups + action->trigger_groups_size++);
}

void trigger_group_add_field (trigger_group_t* group, const field_t* field)
{
  VECTOR_PUSH (group->fields, const field_t*, field);
}

static bool
signature_check_arg (const signature_t* signature, size_t idx, const type_t* arg)
{
  if (idx >= signature->parameters_size)
    {
      return false;
    }

  return signature->parameters[idx].type == arg;
}

bool type_check_arg (const type_t* type, size_t idx, const type_t* arg)
{
  // type must be callable.
  switch (type->kind)
    {
    case TypePort:
      return signature_check_arg (type->port.signature, idx, arg);
    case TypeUndefined:
    case TypeVoid:
    case TypeBool:
    case TypeComponent:
    case TypePointer:
    case TypeReaction:
      return false;
    }

  bug ("unhandled case");
}

static const type_t* make_void ()
{
  static type_t* t = NULL;
  if (t == NULL)
    {
      t = make (TypeVoid);
    }
  return t;
}

const type_t* type_return_value (const type_t* type)
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
      return false;
    }

  bug ("unhandled case");
}

binding_t* binding_make (void)
{
  binding_t* b = xmalloc (sizeof (binding_t));
  VECTOR_INIT (b->outputs, select_result_t);
  VECTOR_INIT (b->inputs, select_result_t);
  return b;
}

void binding_add (binding_t* binding, select_result_t result, bool input)
{
  if (!input)
    {
      VECTOR_PUSH (binding->outputs, select_result_t, result);
    }
  else
    {
      VECTOR_PUSH (binding->inputs, select_result_t, result);
    }
}

void type_add_binding (const type_t* t,
                       binding_t* binding)
{
  type_t* type = (type_t*)t;
  assert (type_is_component (type));

  binding->next = type->component.bindings;
  type->component.bindings = binding;
}

trigger_group_t* action_trigger_begin (const action_t* action)
{
  return VECTOR_BEGIN (action->trigger_groups);
}

trigger_group_t* action_trigger_end (const action_t* action)
{
  return VECTOR_END (action->trigger_groups);
}

trigger_group_t* trigger_next (trigger_group_t* trigger)
{
  return trigger + 1;
}

const field_t** trigger_field_begin (const trigger_group_t* trigger)
{
  return VECTOR_BEGIN (trigger->fields);
}

const field_t** trigger_field_end (const trigger_group_t* trigger)
{
  return VECTOR_END (trigger->fields);
}

const field_t** trigger_field_next (const field_t** field)
{
  return field + 1;
}

binding_t* type_bindings_begin (const type_t* type)
{
  assert (type_is_component (type));
  return type->component.bindings;
}

binding_t* type_bindings_end (const type_t* type)
{
  assert (type_is_component (type));
  return NULL;
}

binding_t* type_bindings_next (binding_t* binding)
{
  return binding->next;
}

select_result_t* binding_output_begin (binding_t* binding)
{
  return VECTOR_BEGIN (binding->outputs);
}

select_result_t* binding_output_end (binding_t* binding)
{
  return VECTOR_END (binding->outputs);
}

select_result_t* binding_output_next (select_result_t* result)
{
  return result + 1;
}

select_result_t* binding_input_begin (binding_t* binding)
{
  return VECTOR_BEGIN (binding->inputs);
}

select_result_t* binding_input_end (binding_t* binding)
{
  return VECTOR_END (binding->inputs);
}

select_result_t* binding_input_next (select_result_t* result)
{
  return result + 1;
}
