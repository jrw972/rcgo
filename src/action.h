#ifndef action_h
#define action_h

#include "types.h"
#include "strtab.h"

action_t *action_make (type_t * type,
                       ast_t* node,
                       size_t number);

bool action_is_action (const action_t * action);

bool action_is_reaction (const action_t * action);

type_t *action_component_type (const action_t * action);

ast_t* action_node (const action_t* action);

size_t action_number (const action_t* action);

action_t *reaction_make (type_t * type,
                         ast_t* node,
			 string_t identifier, type_t * signature);

type_t *reaction_type (const action_t * reaction);

type_t *reaction_signature (const action_t * reaction);

string_t reaction_name (const action_t * reaction);

void action_add_trigger (action_t * action, trigger_t * trigger);

trigger_t **action_trigger_begin (const action_t * action);

trigger_t **action_trigger_end (const action_t * action);

trigger_t **action_trigger_next (trigger_t ** pos);

void action_set_locals_size (action_t* action,
                             size_t size);

size_t action_get_locals_size (const action_t * action);

#endif /* action_h */
