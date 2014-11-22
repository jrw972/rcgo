#ifndef stack_frame_h
#define stack_frame_h

#include "types.h"
#include "rtstring.h"

stack_frame_t* stack_frame_make (memory_model_t* memory_model,
                                 size_t size);

void stack_frame_push_pointer (stack_frame_t* stack_frame,
                               void* pointer);

void* stack_frame_pop_pointer (stack_frame_t* stack_frame);

void stack_frame_push_bool (stack_frame_t* stack_frame,
                            bool b);

bool stack_frame_pop_bool (stack_frame_t* stack_frame);

void stack_frame_push_uint (stack_frame_t* stack_frame,
                            uint64_t b);

uint64_t stack_frame_pop_uint (stack_frame_t* stack_frame);

void stack_frame_push_string (stack_frame_t* stack_frame,
                              rtstring_t b);

rtstring_t stack_frame_pop_string (stack_frame_t* stack_frame);

/* Copy size bytes from base_pointer + offset to the top of the stack. */
void stack_frame_push (stack_frame_t* stack_frame,
                       ptrdiff_t offset,
                       size_t size);

/* Reserve size bytes on the top of the stack . */
void stack_frame_reserve (stack_frame_t* stack_frame,
                          size_t size);

/* Copy size bytes from ptr to the top of the stack. */
void stack_frame_load (stack_frame_t* stack_frame,
                       void* ptr,
                       size_t size);

/* Copy size bytes from the top of the stack to ptr and remove that many bytes from the stack. */
void stack_frame_store_heap (stack_frame_t* stack_frame,
                             void* ptr,
                             size_t size);

/* Copy size bytes from the top of the stack to base_pointer + offset and remove that many bytes from the stack. */
void stack_frame_store_stack (stack_frame_t* stack_frame,
                              ptrdiff_t offset,
                              size_t size);

// Return the base pointer.
char* stack_frame_base_pointer (const stack_frame_t* stack_frame);

// Set the base pointer.
void stack_frame_set_base_pointer (stack_frame_t* stack_frame,
                                   char* base_pointer);

// Push the old base pointer and set a new base pointer.
void stack_frame_push_base_pointer (stack_frame_t* stack_frame);

// Pop a base pointer.
void stack_frame_pop_base_pointer (stack_frame_t* stack_frame);

// Return the top of the stack.
char* stack_frame_top (const stack_frame_t* stack_frame);

void stack_frame_set_top (stack_frame_t* stack_frame,
                          char* top);

// Pop size bytes from the top of the stack.
void stack_frame_pop (stack_frame_t* stack_frame, size_t size);

// Return a pointer to the return instruction pointer.
void* stack_frame_ip (const stack_frame_t* stack_frame);

bool stack_frame_empty (const stack_frame_t* stack_frame);

void stack_frame_dump (const stack_frame_t* stack_frame);

#endif /* stack_frame_h */
