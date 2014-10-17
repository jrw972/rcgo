#ifndef asm_h
#define asm_h

#include <stddef.h>
#include <stdio.h>

typedef size_t asm_label_t;
typedef struct asm_module_t asm_module_t;
typedef struct asm_function_t asm_function_t;
typedef struct asm_data_t asm_data_t;

asm_module_t *asm_module_make (void);

asm_function_t *asm_module_make_function (asm_module_t * module,
					  const char *id);

asm_data_t *asm_module_make_data (asm_module_t * module, const char *id);

void asm_module_write (asm_module_t * module, FILE * file);

void asm_function_end (asm_function_t * function);

void asm_function_comment (asm_function_t * function, const char *comment);

void asm_function_jmp (asm_function_t * function, asm_label_t label);

void asm_function_memclr (asm_function_t * function, size_t size);

asm_label_t asm_function_end_label (asm_function_t * function);

void asm_data_label (asm_data_t * data, const char *label);

void asm_data_null_pointer (asm_data_t * data);

#endif /* asm_h */
