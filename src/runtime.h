#ifndef runtime_h
#define runtime_h

#include "types.h"

runtime_t* runtime_make (instance_table_t* instance_table);

void runtime_allocate_instances (runtime_t* runtime);

void runtime_create_bindings (runtime_t* runtime);

void runtime_run (runtime_t* runtime, memory_model_t* memory_model, size_t stack_size);

#endif /* runtime_h */
