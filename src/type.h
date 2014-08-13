#ifndef type_h
#define type_h

#include "untyped_value.h"
#include "strtab.h"

typedef enum
{ TypeUndefined,
  TypeVoid,
  TypeBool,
  TypeComponent,
  TypePointer,
  TypePort,
  TypeReaction
} TypeKind;

typedef struct type_t type_t;
typedef struct field_t field_t;
typedef struct signature_t signature_t;
typedef struct action_t action_t;
typedef struct trigger_group_t trigger_group_t;
typedef struct binding_t binding_t;

typedef enum
  {
    SelectResultField,
    SelectResultReaction,
    SelectResultUndefined,
  } SelectResultKind;

typedef struct select_result_t select_result_t;
struct select_result_t {
  SelectResultKind kind;
  union {
    const field_t* field;
    const action_t* reaction;
  };
};

const char* type_to_string (const type_t* type);

const type_t *type_set_name (const type_t * type, string_t name);

string_t type_get_name (const type_t * type);

bool type_is_named (const type_t * type);

const type_t *type_make_undefined (void);

bool type_is_undefined (const type_t* type);

const type_t *type_make_bool (void);

const type_t *type_pointer_base_type (const type_t * type);

type_t *type_make_component (void);

type_t * type_make_pointer (const type_t* base_type);

type_t *type_make_port (signature_t* signature);

bool type_is_port (const type_t* type);

bool type_is_component (const type_t * type);

action_t* type_add_action (const type_t * type);

bool action_is_action (const action_t* action);

bool action_is_reaction (const action_t* action);

size_t action_number (const action_t* action);

size_t type_action_count (const type_t * type);

size_t type_add_bind (const type_t* type);

size_t type_bind_count (const type_t * type);

bool type_append_field (type_t * type, string_t field_name,
			const type_t * field_type);

const field_t *type_field_list (const type_t * type);

string_t field_name (const field_t * field);

const type_t *field_type (const field_t * field);

size_t field_number (const field_t * field);

const field_t *field_next (const field_t * field);

TypeKind type_kind (const type_t * type);

bool type_can_represent (const type_t * type, untyped_value_t u);

bool type_assignable (const type_t * target, const type_t * source);

const type_t *type_logic_not (const type_t * type);

const type_t *type_logic_and (const type_t * x, const type_t * y);

const type_t *type_logic_or (const type_t * x, const type_t * y);

const type_t *type_dereference (const type_t * type);

select_result_t type_select (const type_t * type, string_t identifier);

select_result_t select_result_make_undefined ();

bool type_is_boolean (const type_t * type);

action_t* type_get_reaction (const type_t * component_type, string_t identifier);

action_t* type_add_reaction (const type_t * component_type, string_t identifier, signature_t* signature);

const type_t* reaction_type (const action_t* reaction);

string_t reaction_name (const action_t* reaction);

const type_t* reaction_component_type (const action_t* reaction);

signature_t* signature_make (void);

bool signature_add_parameter (signature_t* signature,
                              string_t name,
                              const type_t* type);

size_t signature_size (const signature_t* signature);

const type_t* signature_type (const signature_t* signature, size_t idx);

string_t signature_name (const signature_t* signature, size_t idx);

bool type_is_bindable (const type_t* output, const type_t* input);

action_t* type_reaction_get_reaction (const type_t* type);

action_t* type_actions_begin (const type_t* type);

action_t* type_actions_end (const type_t* type);

action_t* action_next (action_t* action);

trigger_group_t* action_add_trigger_group (action_t* action);

void trigger_group_add_field (trigger_group_t* group, const field_t* field);

bool type_check_arg (const type_t* type, size_t idx, const type_t* arg);

const type_t* type_return_value (const type_t* type);

binding_t* binding_make (void);

void binding_add (binding_t* binding, select_result_t result, bool input);

void type_add_binding (const type_t* type,
                       binding_t* binding);

trigger_group_t* action_trigger_begin (const action_t* action);

trigger_group_t* action_trigger_end (const action_t* action);

trigger_group_t* trigger_next (trigger_group_t* trigger);

const field_t** trigger_field_begin (const trigger_group_t* trigger);

const field_t** trigger_field_end (const trigger_group_t* trigger);

const field_t** trigger_field_next (const field_t** field);

binding_t* type_bindings_begin (const type_t* type);

binding_t* type_bindings_end (const type_t* type);

binding_t* type_bindings_next (binding_t* binding);

select_result_t* binding_output_begin (binding_t* binding);

select_result_t* binding_output_end (binding_t* binding);

select_result_t* binding_output_next (select_result_t* result);

select_result_t* binding_input_begin (binding_t* binding);

select_result_t* binding_input_end (binding_t* binding);

select_result_t* binding_input_next (select_result_t* result);

#endif /* type_h */
