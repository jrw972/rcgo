#include "memory_model.h"
#include "debug.h"
#include "util.h"

struct memory_model_t {
  size_t stack_alignment;
  ptrdiff_t arguments_offset;
  ptrdiff_t locals_offset;
};

memory_model_t* memory_model_make (size_t stack_alignment)
{
  memory_model_t* mm = xmalloc (sizeof (memory_model_t));
  mm->stack_alignment = stack_alignment;
  mm->arguments_offset = -(ptrdiff_t)sizeof (void*);
  mm->locals_offset = sizeof (void*);
  return mm;
}

size_t memory_model_stack_alignment (const memory_model_t* memory_model)
{
  return memory_model->stack_alignment;
}

ptrdiff_t memory_model_arguments_offset (const memory_model_t* memory_model)
{
  return memory_model->arguments_offset;
}

bool memory_model_arguments_empty (const memory_model_t* memory_model)
{
  return memory_model->arguments_offset == -(ptrdiff_t)sizeof (void*);
}

void memory_model_arguments_push (memory_model_t* memory_model,
                                  size_t size)
{
  memory_model->arguments_offset -= align_up (size, memory_model->stack_alignment);
}

void memory_model_arguments_pop (memory_model_t* memory_model,
                                 size_t size)
{
  memory_model->arguments_offset += align_up (size, memory_model->stack_alignment);
}

ptrdiff_t memory_model_locals_offset (const memory_model_t* memory_model)
{
  return memory_model->locals_offset;
}

bool memory_model_locals_empty (const memory_model_t* memory_model)
{
  return memory_model->locals_offset == sizeof (void*);
}

void memory_model_locals_pop (memory_model_t* memory_model,
                              size_t size)
{
  memory_model->locals_offset -= align_up (size, memory_model->stack_alignment);
}
