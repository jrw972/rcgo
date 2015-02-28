#include "stack_frame.hpp"
#include "debug.hpp"
#include "util.hpp"
#include <string.h>
#include "memory_model.hpp"

struct stack_frame_t {
  char* base_pointer;
  char* top;
  char* limit;
  char data[];
};

stack_frame_t* stack_frame_make (size_t capacity)
{
  stack_frame_t* sf = (stack_frame_t*)malloc (sizeof (stack_frame_t) + capacity);
  memset (sf, 0, sizeof (stack_frame_t) + capacity);
  sf->base_pointer = NULL;
  sf->top = sf->data;
  sf->limit = sf->data + capacity;
  return sf;
}

void stack_frame_push_pointer (stack_frame_t* stack_frame,
                               void* pointer)
{
  size_t s = align_up (sizeof (void*), memory_model_t::stack_alignment);
  assert (stack_frame->top + s <= stack_frame->limit);
  memcpy (stack_frame->top, &pointer, sizeof (void*));
  stack_frame->top += s;
}

void* stack_frame_pop_pointer (stack_frame_t* stack_frame)
{
  void* retval;
  size_t s = align_up (sizeof (void*), memory_model_t::stack_alignment);
  assert (stack_frame->top - s >= stack_frame->data);
  stack_frame->top -= s;
  memcpy (&retval, stack_frame->top, sizeof (void*));
  return retval;
}

void stack_frame_push_bool (stack_frame_t* stack_frame,
                            bool b)
{
  size_t s = align_up (sizeof (bool), memory_model_t::stack_alignment);
  assert (stack_frame->top + s <= stack_frame->limit);
  memcpy (stack_frame->top, &b, sizeof (bool));
  stack_frame->top += s;
}

bool stack_frame_pop_bool (stack_frame_t* stack_frame)
{
  bool retval;
  size_t s = align_up (sizeof (bool), memory_model_t::stack_alignment);
  assert (stack_frame->top - s >= stack_frame->data);
  stack_frame->top -= s;
  memcpy (&retval, stack_frame->top, sizeof (bool));
  return retval;
}

void stack_frame_push_uint (stack_frame_t* stack_frame,
                            uint64_t b)
{
  size_t s = align_up (sizeof (uint64_t), memory_model_t::stack_alignment);
  assert (stack_frame->top + s <= stack_frame->limit);
  memcpy (stack_frame->top, &b, sizeof (uint64_t));
  stack_frame->top += s;
}

uint64_t stack_frame_pop_uint (stack_frame_t* stack_frame)
{
  uint64_t retval;
  size_t s = align_up (sizeof (uint64_t), memory_model_t::stack_alignment);
  assert (stack_frame->top - s >= stack_frame->data);
  stack_frame->top -= s;
  memcpy (&retval, stack_frame->top, sizeof (uint64_t));
  return retval;
}

void stack_frame_push_int (stack_frame_t* stack_frame,
                           int64_t b)
{
  size_t s = align_up (sizeof (int64_t), memory_model_t::stack_alignment);
  assert (stack_frame->top + s <= stack_frame->limit);
  memcpy (stack_frame->top, &b, sizeof (int64_t));
  stack_frame->top += s;
}

int64_t stack_frame_pop_int (stack_frame_t* stack_frame)
{
  int64_t retval;
  size_t s = align_up (sizeof (int64_t), memory_model_t::stack_alignment);
  assert (stack_frame->top - s >= stack_frame->data);
  stack_frame->top -= s;
  memcpy (&retval, stack_frame->top, sizeof (int64_t));
  return retval;
}

void stack_frame_push_string (stack_frame_t* stack_frame,
                              rtstring_t b)
{
  size_t s = align_up (sizeof (rtstring_t), memory_model_t::stack_alignment);
  assert (stack_frame->top + s <= stack_frame->limit);
  memcpy (stack_frame->top, &b, sizeof (rtstring_t));
  stack_frame->top += s;
}

rtstring_t stack_frame_pop_string (stack_frame_t* stack_frame)
{
  rtstring_t retval;
  size_t s = align_up (sizeof (rtstring_t), memory_model_t::stack_alignment);
  assert (stack_frame->top - s >= stack_frame->data);
  stack_frame->top -= s;
  memcpy (&retval, stack_frame->top, sizeof (rtstring_t));
  return retval;
}

void stack_frame_push_address (stack_frame_t* stack_frame,
                               ptrdiff_t offset)
{
  stack_frame_push_pointer (stack_frame, stack_frame->base_pointer + offset);
}

void stack_frame_equal (stack_frame_t* stack_frame,
                        size_t size)
{
  size_t s = align_up (size, memory_model_t::stack_alignment);
  assert (stack_frame->top - 2 * s >= stack_frame->data);
  stack_frame->top -= s;
  char* y = stack_frame->top;
  stack_frame->top -= s;
  char* x = stack_frame->top;
  stack_frame_push_bool (stack_frame, memcmp (x, y, size) == 0);
}

void stack_frame_not_equal (stack_frame_t* stack_frame,
                            size_t size)
{
  size_t s = align_up (size, memory_model_t::stack_alignment);
  assert (stack_frame->top - 2 * s >= stack_frame->data);
  stack_frame->top -= s;
  char* y = stack_frame->top;
  stack_frame->top -= s;
  char* x = stack_frame->top;
  stack_frame_push_bool (stack_frame, memcmp (x, y, size) != 0);
}

void stack_frame_push (stack_frame_t* stack_frame,
                       ptrdiff_t offset,
                       size_t size)
{
  size_t s = align_up (size, memory_model_t::stack_alignment);
  char* source = stack_frame->base_pointer + offset;
  assert (source >= stack_frame->data && source + size <= stack_frame->top);
  assert (stack_frame->top + s <= stack_frame->limit);
  memcpy (stack_frame->top, source, size);
  stack_frame->top += s;
}

void stack_frame_reserve (stack_frame_t* stack_frame,
                          size_t size)
{
  size_t s = align_up (size, memory_model_t::stack_alignment);
  assert (stack_frame->top + s <= stack_frame->limit);
  memset (stack_frame->top, 0, size);
  stack_frame->top += s;
}

void stack_frame_load (stack_frame_t* stack_frame,
                       void* ptr,
                       size_t size)
{
  size_t s = align_up (size, memory_model_t::stack_alignment);
  assert (stack_frame->top + s <= stack_frame->limit);
  memcpy (stack_frame->top, ptr, size);
  stack_frame->top += s;
}

void stack_frame_store_heap (stack_frame_t* stack_frame,
                             void* ptr,
                             size_t size)
{
  size_t s = align_up (size, memory_model_t::stack_alignment);
  assert (stack_frame->top - s >= stack_frame->data);
  stack_frame->top -= s;
  memcpy (ptr, stack_frame->top, size);
}

void stack_frame_store_stack (stack_frame_t* stack_frame,
                              ptrdiff_t offset,
                              size_t size)
{
  size_t s = align_up (size, memory_model_t::stack_alignment);
  char* ptr = stack_frame->base_pointer + offset;
  assert (ptr >= stack_frame->data && ptr + size <= stack_frame->top);
  assert (stack_frame->top - s >= stack_frame->data);
  stack_frame->top -= s;
  memcpy (ptr, stack_frame->top, size);
}

char* stack_frame_base_pointer (const stack_frame_t* stack_frame)
{
  return stack_frame->base_pointer;
}

void stack_frame_set_base_pointer (stack_frame_t* stack_frame,
                                   char* base_pointer)
{
  stack_frame->base_pointer = base_pointer;
}

void stack_frame_push_base_pointer (stack_frame_t* stack_frame, size_t size)
{
  char* bp = stack_frame->top;
  stack_frame_push_pointer (stack_frame, stack_frame->base_pointer);
  stack_frame->base_pointer = bp;
  stack_frame_reserve (stack_frame, size);
}

void stack_frame_pop_base_pointer (stack_frame_t* stack_frame)
{
  // Adjust the top.
  stack_frame->top = stack_frame->base_pointer + sizeof (void*);
  stack_frame->base_pointer = (char*)stack_frame_pop_pointer (stack_frame);
}

char* stack_frame_top (const stack_frame_t* stack_frame)
{
  return stack_frame->top;
}

void stack_frame_set_top (stack_frame_t* stack_frame,
                          char* top)
{
  stack_frame->top = top;
}

void stack_frame_pop (stack_frame_t* stack_frame, size_t size)
{
  size_t s = align_up (size, memory_model_t::stack_alignment);
  assert (stack_frame->top - s >= stack_frame->data);
  stack_frame->top -= s;
}

void* stack_frame_ip (const stack_frame_t* stack_frame)
{
  return stack_frame->base_pointer - sizeof (void*);
}

bool stack_frame_empty (const stack_frame_t* stack_frame)
{
  return stack_frame->data == stack_frame->top;
}

void stack_frame_dump (const stack_frame_t* stack_frame)
{
  printf ("size = %td base_pointer = %p\n", stack_frame->top - stack_frame->data, stack_frame->base_pointer);
  size_t increment = memory_model_t::stack_alignment;
  const char* ptr;
  for (ptr = stack_frame->data; ptr != stack_frame->top; ptr += increment)
    {
      size_t x;
      memcpy (&x, ptr, sizeof (size_t));
      printf ("%p => %zx %s\n", ptr, x, ptr == stack_frame->base_pointer ? "<--" : "");
    }
}
