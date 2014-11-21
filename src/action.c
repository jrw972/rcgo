#include "action.h"
#include "strtab.h"
#include "debug.h"
#include "type.h"
#include "util.h"

struct action_t
{
  bool is_reaction;
  type_t *component_type;	/* Back-pointer to component type. */
    VECTOR_DECL (triggers, trigger_t *);
  VECTOR_DECL (getter_calls, getter_call_t *);
  ast_t* node;
  /* Reactions only. */
  string_t name;
  type_t *reaction_type;
};

action_t *
action_make (type_t * type,
             ast_t* node)
{
  assert (type_is_component (type));
  action_t *action = xmalloc (sizeof (action_t));
  action->is_reaction = false;
  action->component_type = type;
  VECTOR_INIT (action->triggers, trigger_t *, 0, NULL);
  VECTOR_INIT (action->getter_calls, getter_call_t *, 0, NULL);
  action->node = node;
  return action;
}

bool
action_is_action (const action_t * action)
{
  return !action->is_reaction;
}

bool
action_is_reaction (const action_t * action)
{
  return action->is_reaction;
}

type_t *
action_component_type (const action_t * action)
{
  return action->component_type;
}

ast_t* action_node (const action_t* action)
{
  return action->node;
}

action_t *
reaction_make (type_t * type,
               ast_t* node,
	       string_t name, type_t * signature)
{
  assert (type_is_component (type));
  assert (type_is_signature (signature));
  action_t *action = xmalloc (sizeof (action_t));
  action->is_reaction = true;
  action->component_type = type;
  VECTOR_INIT (action->triggers, trigger_t *, 0, NULL);
  action->node = node;
  action->name = name;
  action->reaction_type = type_make_reaction (signature);
  return action;
}

type_t *
reaction_type (const action_t * reaction)
{
  assert (reaction->is_reaction);
  return reaction->reaction_type;
}

type_t *
reaction_signature (const action_t * reaction)
{
  assert (reaction->is_reaction);
  return type_reaction_signature (reaction->reaction_type);
}

string_t
reaction_name (const action_t * reaction)
{
  assert (reaction->is_reaction);
  return reaction->name;
}

void
action_add_trigger (action_t * action, trigger_t * trigger)
{
  VECTOR_PUSH (action->triggers, trigger_t *, trigger);
}

trigger_t **
action_trigger_begin (const action_t * action)
{
  return VECTOR_BEGIN (action->triggers);
}

trigger_t **
action_trigger_end (const action_t * action)
{
  return VECTOR_END (action->triggers);
}

trigger_t **
action_trigger_next (trigger_t ** pos)
{
  return pos + 1;
}

void action_add_getter_call (action_t * action, getter_call_t* gc)
{
  VECTOR_PUSH (action->getter_calls, getter_call_t*, gc);
}

getter_call_t **
action_getter_call_begin (const action_t * action)
{
  return VECTOR_BEGIN (action->getter_calls);
}

getter_call_t **
action_getter_call_end (const action_t * action)
{
  return VECTOR_END (action->getter_calls);
}

getter_call_t **
action_getter_call_next (getter_call_t ** pos)
{
  return pos + 1;
}
