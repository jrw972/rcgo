#include "stack_frame.hpp"
#include "debug.hpp"
#include "util.hpp"
#include <string.h>
#include "MemoryModel.hpp"
#include "type.hpp"

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
    size_t s = util::AlignUp (sizeof (void*), MemoryModel::StackAlignment);
    assert (stack_frame->top + s <= stack_frame->limit);
    memcpy (stack_frame->top, &pointer, sizeof (void*));
    stack_frame->top += s;
}

void* stack_frame_pop_pointer (stack_frame_t* stack_frame)
{
    void* retval;
    size_t s = util::AlignUp (sizeof (void*), MemoryModel::StackAlignment);
    assert (stack_frame->top - s >= stack_frame->data);
    stack_frame->top -= s;
    memcpy (&retval, stack_frame->top, sizeof (void*));
    return retval;
}

void* stack_frame_read_pointer (stack_frame_t* stack_frame)
{
    void* retval;
    size_t s = util::AlignUp (sizeof (void*), MemoryModel::StackAlignment);
    assert (stack_frame->top - s >= stack_frame->data);
    memcpy (&retval, stack_frame->top - s, sizeof (void*));
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
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    assert (stack_frame->top - 2 * s >= stack_frame->data);
    stack_frame->top -= s;
    char* y = stack_frame->top;
    stack_frame->top -= s;
    char* x = stack_frame->top;
    stack_frame_push<bool_type_t::ValueType> (stack_frame, memcmp (x, y, size) == 0);
}

void stack_frame_not_equal (stack_frame_t* stack_frame,
                            size_t size)
{
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    assert (stack_frame->top - 2 * s >= stack_frame->data);
    stack_frame->top -= s;
    char* y = stack_frame->top;
    stack_frame->top -= s;
    char* x = stack_frame->top;
    stack_frame_push<bool_type_t::ValueType> (stack_frame, memcmp (x, y, size) != 0);
}

void stack_frame_push (stack_frame_t* stack_frame,
                       ptrdiff_t offset,
                       size_t size)
{
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    char* source = stack_frame->base_pointer + offset;
    assert (source >= stack_frame->data && source + size <= stack_frame->top);
    assert (stack_frame->top + s <= stack_frame->limit);
    memcpy (stack_frame->top, source, size);
    stack_frame->top += s;
}

void stack_frame_reserve (stack_frame_t* stack_frame,
                          size_t size)
{
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    assert (stack_frame->top + s <= stack_frame->limit);
    memset (stack_frame->top, 0, size);
    stack_frame->top += s;
}

void stack_frame_load (stack_frame_t* stack_frame,
                       void* ptr,
                       size_t size)
{
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    assert (stack_frame->top + s <= stack_frame->limit);
    memcpy (stack_frame->top, ptr, size);
    stack_frame->top += s;
}

void stack_frame_store_heap (stack_frame_t* stack_frame,
                             void* ptr,
                             size_t size)
{
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    assert (stack_frame->top - s >= stack_frame->data);
    stack_frame->top -= s;
    memcpy (ptr, stack_frame->top, size);
}

void stack_frame_store_stack (stack_frame_t* stack_frame,
                              ptrdiff_t offset,
                              size_t size)
{
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    char* ptr = stack_frame->base_pointer + offset;
    assert (ptr >= stack_frame->data && ptr + size <= stack_frame->top);
    assert (stack_frame->top - s >= stack_frame->data);
    stack_frame->top -= s;
    memcpy (ptr, stack_frame->top, size);
}

void stack_frame_clear_stack (stack_frame_t* stack_frame,
                              ptrdiff_t offset,
                              size_t size)
{
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    char* ptr = stack_frame->base_pointer + offset;
    assert (ptr >= stack_frame->data && ptr + size <= stack_frame->top);
    memset (ptr, 0, s);
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

void stack_frame_popn (stack_frame_t* stack_frame, size_t size)
{
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
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
    size_t increment = MemoryModel::StackAlignment;
    const char* ptr;
    for (ptr = stack_frame->data; ptr != stack_frame->top; ptr += increment)
        {
            size_t x;
            memcpy (&x, ptr, sizeof (size_t));
            printf ("%p => %zx %s\n", ptr, x, ptr == stack_frame->base_pointer ? "<--" : "");
        }
}

void* stack_frame_address_for_offset (const stack_frame_t* stack_frame,
                                      ptrdiff_t offset)
{
    return stack_frame->base_pointer + offset;
}

void stack_frame_push_tv (stack_frame_t* stack_frame,
                          const typed_value_t& tv)
{
    struct visitor : public const_type_visitor_t
    {
        stack_frame_t* stack_frame;
        const typed_value_t& tv;

        visitor (stack_frame_t* sf, const typed_value_t& t)
            : stack_frame (sf)
            , tv (t)
        { }

        void visit (const bool_type_t& type)
        {
            stack_frame_push (stack_frame, tv.value.ref (type));
        }

        void visit (const int_type_t& type)
        {
            stack_frame_push (stack_frame, tv.value.ref (type));
        }

        void visit (const int8_type_t& type)
        {
            stack_frame_push (stack_frame, tv.value.ref (type));
        }

        void visit (const uint8_type_t& type)
        {
            stack_frame_push (stack_frame, tv.value.ref (type));
        }

        void visit (const float64_type_t& type)
        {
            stack_frame_push (stack_frame, tv.value.ref (type));
        }

        void visit (const slice_type_t& type)
        {
            stack_frame_push (stack_frame, tv.value.ref (type));
        }

        void visit (const string_type_t& type)
        {
            stack_frame_push (stack_frame, tv.value.ref (type));
        }

        void default_action (const type_t& type)
        {
            type_not_reached (type);
        }
    };

    visitor v (stack_frame, tv);
    type_strip (tv.type)->accept (v);
}

struct PopVisitor : public const_type_visitor_t
{
    stack_frame_t* stack_frame;
    typed_value_t& tv;

    PopVisitor (stack_frame_t* sf, typed_value_t& t)
        : stack_frame (sf)
        , tv (t)
    { }

    template <typename T>
    void doit (const T& type)
    {
        typename T::ValueType value;
        stack_frame_pop (stack_frame, value);
        tv.value = value_t (&type, value);
    }

    void visit (const int8_type_t& type)
    {
        doit (type);
    }
    void visit (const uint_type_t& type)
    {
        doit (type);
    }
    void visit (const uint64_type_t& type)
    {
        doit (type);
    }
    void visit (const slice_type_t& type)
    {
        doit (type);
    }
    void visit (const string_type_t& type)
    {
        doit (type);
    }
    void default_action (const type_t& type)
    {
        type_not_reached (type);
    }
};

void stack_frame_pop_tv (stack_frame_t* stack_frame,
                         typed_value_t& tv)
{
    switch (tv.kind)
        {
        case typed_value_t::REFERENCE:
            tv.value = value_t::make_reference (stack_frame_pop_pointer (stack_frame));
            break;
        case typed_value_t::VALUE:
        {
            PopVisitor v (stack_frame, tv);
            type_strip (tv.type)->accept (v);
        }
        break;
        }
}
