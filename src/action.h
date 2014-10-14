#ifndef action_h
#define action_h

#include "types.h"
#include "strtab.h"

action_t*
action_make (type_t* type);

bool action_is_action (const action_t* action);

bool action_is_reaction (const action_t* action);

type_t* action_type (const action_t* action);

action_t*
reaction_make (type_t* type,
               size_t number,
               string_t identifier,
               type_t* signature);

type_t* reaction_type (const action_t* reaction);

type_t* reaction_signature (const action_t* reaction);

string_t reaction_name (const action_t* reaction);

const type_t* reaction_component_type (const action_t* reaction);

void action_add_trigger_group (action_t* action, trigger_group_t* trigger_group);

trigger_group_t** action_trigger_group_begin (const action_t* action);

trigger_group_t** action_trigger_group_end (const action_t* action);

trigger_group_t** action_trigger_group_next (trigger_group_t** pos);

#endif /* action_h */
