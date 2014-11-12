#ifndef instance_h
#define instance_h

#include "types.h"

instance_table_t *instance_table_make (void);

instance_t *instance_table_insert (instance_table_t * table,
				   const type_t * type,
                                   bool is_top_level);

void instance_table_insert_subinstance (instance_table_t * table,
					instance_t * instance,
					field_t * field,
					instance_t * subinstance);

instance_t* instance_table_get_subinstance (const instance_table_t * table,
                                            instance_t * instance,
                                            const field_t * field);

void instance_table_enumerate_bindings (instance_table_t * table);

void instance_table_analyze_composition (const instance_table_t * table);

void instance_table_dump (const instance_table_t * table);

instance_t** instance_table_begin (instance_table_t* table);

instance_t** instance_table_end (instance_table_t* table);

instance_t** instance_table_next (instance_t** pos);

concrete_binding_t* instance_table_binding_begin (instance_table_t* table);

concrete_binding_t* instance_table_binding_end (instance_table_t* table);

concrete_binding_t* instance_table_binding_next (concrete_binding_t* pos);

const type_t* instance_type (const instance_t* instance);

void instance_set_ptr (instance_t* instance, void* ptr);

void* instance_get_ptr (const instance_t* instance);

bool instance_is_top_level (const instance_t* instance);

instance_t* concrete_binding_output_instance (const concrete_binding_t* binding);

const field_t* concrete_binding_output_port (const concrete_binding_t* binding);

instance_t* concrete_binding_input_instance (const concrete_binding_t* binding);

const action_t* concrete_binding_input_reaction (const concrete_binding_t* binding);

#endif /* instance_h */
