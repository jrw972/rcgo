#ifndef instance_h
#define instance_h

#include "types.h"

instance_table_t* instance_table_make (void);

instance_t* instance_table_insert (instance_table_t* table, const type_t* type);

void instance_table_insert_subinstance (instance_table_t* table,
                                        instance_t* instance,
                                        field_t* field,
                                        instance_t* subinstance);

void
instance_table_enumerate_bindings (instance_table_t* table);

void
instance_table_analyze_composition (const instance_table_t* table);

void instance_table_dump (const instance_table_t* table);

#endif /* instance_h */
