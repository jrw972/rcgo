#ifndef memory_model_h
#define memory_model_h

#include "types.h"

memory_model_t* memory_model_make (size_t stack_alignment);

size_t memory_model_stack_alignment (const memory_model_t* memory_model);

ptrdiff_t memory_model_arguments_offset (const memory_model_t* memory_model);

bool memory_model_arguments_empty (const memory_model_t* memory_model);

void memory_model_arguments_push (memory_model_t* memory_model,
                                  size_t size);

void memory_model_arguments_pop (memory_model_t* memory_model,
                                  size_t size);

ptrdiff_t memory_model_locals_offset (const memory_model_t* memory_model);

bool memory_model_locals_empty (const memory_model_t* memory_model);

void memory_model_locals_pop (memory_model_t* memory_model,
                              size_t size);

#endif /* memory_model_h */
