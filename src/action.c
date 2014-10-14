#include "action.h"
#include "strtab.h"
#include "debug.h"
#include "type.h"
#include "util.h"

struct action_t
{
  bool is_reaction;
  type_t *component_type; /* Back-pointer to component type. */
  VECTOR_DECL (trigger_groups, trigger_group_t*);
  /* Reactions only. */
  string_t name;
  type_t *reaction_type;
};

action_t*
action_make (type_t* type)
{
  assert (type_is_component (type));
  action_t* action = xmalloc (sizeof (action_t));
  action->is_reaction = false;
  action->component_type = type;
  VECTOR_INIT (action->trigger_groups, trigger_group_t*, 0, NULL);
  return action;
}

bool action_is_action (const action_t* action)
{
  return !action->is_reaction;
}

bool action_is_reaction (const action_t* action)
{
  return action->is_reaction;
}

type_t* action_type (const action_t* action)
{
  return action->component_type;
}

action_t*
reaction_make (type_t* type,
               size_t number,
               string_t name,
               type_t* signature)
{
  assert (type_is_component (type));
  assert (type_is_signature (signature));
  action_t* action = xmalloc (sizeof (action_t));
  action->is_reaction = true;
  action->component_type = type;
  VECTOR_INIT (action->trigger_groups, trigger_group_t*, 0, NULL);
  action->name = name;
  action->reaction_type = type_make_reaction (signature);
  return action;
}

type_t* reaction_type (const action_t* reaction)
{
  assert (reaction->is_reaction);
  return reaction->reaction_type;
}

type_t* reaction_signature (const action_t* reaction)
{
  assert (reaction->is_reaction);
  return type_reaction_signature (reaction->reaction_type);
}

string_t reaction_name (const action_t* reaction)
{
  assert (reaction->is_reaction);
  return reaction->name;
}

const type_t* reaction_component_type (const action_t* reaction)
{
  assert (reaction->is_reaction);
  return reaction->component_type;
}

void action_add_trigger_group (action_t* action, trigger_group_t* trigger_group)
{
  VECTOR_PUSH (action->trigger_groups, trigger_group_t*, trigger_group);
}

trigger_group_t** action_trigger_group_begin (const action_t* action)
{
  return VECTOR_BEGIN (action->trigger_groups);
}

trigger_group_t** action_trigger_group_end (const action_t* action)
{
  return VECTOR_END (action->trigger_groups);
}

trigger_group_t** action_trigger_group_next (trigger_group_t** pos)
{
  return pos + 1;
}
