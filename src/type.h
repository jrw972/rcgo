#ifndef type_h
#define type_h

#include "untyped_value.h"
#include "typed_value.h"
#include "strtab.h"
#include "types.h"

typedef enum
{ TypeUndefined,
  TypeVoid,
  TypeBool,
  TypeComponent,
  TypePointer,
  TypePort,
  TypeReaction,
  TypeFieldList,
  TypeSignature,
  TypeString,
  TypeGetter,
} TypeKind;

typedef enum
{ PointerToMutable,
  PointerToImmutable,
} PointerKind;

const char *type_to_string (const type_t * type);

/* Copy the guts of from to to and delete from. */
void type_move (type_t * to, type_t * from);

size_t type_size (const type_t * type);

const type_t *type_set_name (const type_t * type, string_t name);

string_t type_get_name (const type_t * type);

bool type_is_named (const type_t * type);

type_t *type_make_undefined (void);

bool type_is_undefined (const type_t * type);

type_t *type_make_bool (void);

type_t *type_pointer_base_type (const type_t * type);

type_t *type_make_component (type_t * type);

type_t *type_make_pointer (PointerKind kind, type_t * base_type);

type_t *type_make_port (const type_t * signature);

bool type_is_port (const type_t * type);

bool type_is_component (const type_t * type);

action_t *type_component_add_action (type_t * type, ast_t* node);

action_t *type_component_add_reaction (type_t * component_type,
                                       ast_t* node,
				       string_t identifier,
				       type_t * signature);

getter_t *type_component_add_getter (type_t * component_type,
                                     ast_t* node,
                                     string_t identifier,
                                     type_t * signature,
                                     type_t * return_type);

TypeKind type_kind (const type_t * type);

bool type_can_represent (const type_t * type, untyped_value_t u);

bool type_assignable (const type_t * target, const type_t * source);

const type_t *type_logic_not (const type_t * type);

const type_t *type_logic_and (const type_t * x, const type_t * y);

const type_t *type_logic_or (const type_t * x, const type_t * y);

type_t *type_select (const type_t * type, string_t identifier);

field_t* type_select_field (const type_t* type, string_t identifier);

bool type_is_boolean (const type_t * type);

action_t *type_component_get_reaction (const type_t * component_type,
				       string_t identifier);

getter_t *type_component_get_getter (const type_t * component_type,
                                     string_t identifier);

bool type_is_bindable (const type_t * output, const type_t * input);

action_t **type_actions_begin (const type_t * type);

action_t **type_actions_end (const type_t * type);

action_t **type_actions_next (action_t ** action);

bool type_check_arg (const type_t * type, size_t idx, const type_t * arg);

const type_t *type_return_value (const type_t * type);

type_t *type_make_field_list (void);

field_t *type_field_list_find (const type_t * field_list,
			       string_t field_name);

void type_field_list_prepend (type_t* field_list,
                              string_t field_name, type_t* field_type);

void type_field_list_append (type_t * field_list,
			     string_t field_name, type_t * field_type);

bool type_is_pointer (const type_t * type);

PointerKind type_pointer_kind (const type_t * type);

bool type_equivalent (const type_t * left, const type_t * right);

type_t *type_make_signature (void);

parameter_t *type_signature_find (const type_t * signature,
				  string_t parameter_name);

void type_signature_append (type_t * signature, string_t parameter_name,
			    type_t * parameter_type);

parameter_t **type_signature_begin (const type_t * signature);

parameter_t **type_signature_end (const type_t * signature);

parameter_t **type_signature_next (parameter_t **);

bool type_callable (const type_t * type);

size_t type_parameter_count (const type_t * type);

type_t *type_parameter_type (const type_t * type, size_t size);

type_t *type_return_type (const type_t * type);

bool type_is_reaction (const type_t * type);

bool type_is_signature (const type_t * type);

type_t *type_make_reaction (type_t * signature);

type_t *type_reaction_signature (const type_t * reaction);

bool type_compatible_port_reaction (const type_t * port,
				    const type_t * reaction);

size_t type_signature_arity (const type_t * signature);

type_t *type_make_getter (const type_t * signature,
                          type_t * return_type);

const type_t *type_getter_signature (const type_t * getter);

type_t *type_getter_return_type (const type_t * getter);

field_t **type_field_list_begin (const type_t * field_list);

field_t **type_field_list_end (const type_t * field_list);

field_t **type_field_list_next (field_t ** field);

const type_t *type_component_field_list (const type_t * component);

void type_component_add_binding (type_t * component, binding_t * binding);

binding_t **type_component_bindings_begin (const type_t * component);

binding_t **type_component_bindings_end (const type_t * component);

binding_t **type_component_bindings_next (binding_t ** pos);

action_t **type_component_actions_begin (const type_t * component);

action_t **type_component_actions_end (const type_t * component);

action_t **type_component_actions_next (action_t ** pos);

void type_print_value (const type_t* type,
                       void* value);

type_t* type_make_string (void);

#endif /* type_h */
