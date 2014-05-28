#ifndef type_h
#define type_h

#include "untyped_value.h"
#include "strtab.h"

typedef struct type_t type_t;
typedef struct field_t field_t;

typedef enum
{ TypeUndefined,
  TypeBool,
  TypeComponent,
  TypePointerToImmutable,
  TypePointerToMutable,
} TypeKind;

const type_t* type_set_name (const type_t* type, string_t name);

string_t type_get_name (const type_t* type);

bool type_is_named (const type_t* type);

const type_t *type_make_undefined (void);

const type_t *type_make_bool (void);

const type_t *type_get_pointer_to_immutable (const type_t* type);

const type_t *type_get_pointer_to_mutable (const type_t* type);

const type_t *type_pointer_base_type (const type_t* type);

type_t * type_make_component (void);

bool type_is_component (const type_t* type);

size_t type_add_action (const type_t* type);

size_t type_action_count (const type_t* type);

bool type_append_field (type_t* type, string_t field_name, const type_t* field_type);

const field_t* type_field_list (const type_t* type);

string_t field_name (const field_t* field);

const type_t* field_type (const field_t* field);

const field_t* field_next (const field_t* field);

TypeKind type_kind (const type_t * type);

bool type_can_represent (const type_t * type, untyped_value_t u);

bool type_assignable (const type_t * target, const type_t * source);

const type_t *type_logic_not (const type_t * type);

const type_t *type_logic_and (const type_t * x, const type_t * y);

const type_t *type_logic_or (const type_t * x, const type_t * y);

const type_t *type_dereference (const type_t * type);

bool type_is_pointer_to_mutable (const type_t * type);

const type_t *type_select (const type_t* type,
                           string_t identifier);

bool type_is_boolean (const type_t* type);

#endif /* type_h */
