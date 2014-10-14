#include "asm.h"
#include "debug.h"

asm_module_t*
asm_module_make (void)
{
  unimplemented;
}

asm_function_t*
asm_module_make_function (asm_module_t* module, const char* id)
{
  unimplemented;
}

asm_data_t*
asm_module_make_data (asm_module_t* module, const char* id)
{
  unimplemented;
}

void
asm_module_write (asm_module_t* module, FILE* file)
{
  unimplemented;
}

void
asm_function_end (asm_function_t* function)
{
  unimplemented;
}

void
asm_function_comment (asm_function_t* function,
                      const char* comment)
{
  unimplemented;
}

void
asm_function_jmp (asm_function_t* function,
                  asm_label_t label)
{
  unimplemented;
}

void
asm_function_memclr (asm_function_t* function,
                     size_t size)
{
  unimplemented;
}

asm_label_t
asm_function_end_label (asm_function_t* function)
{
  unimplemented;
}

void
asm_data_label (asm_data_t* data,
                const char* label)
{
  unimplemented;
}

void
asm_data_null_pointer (asm_data_t* data)
{
  unimplemented;
}
