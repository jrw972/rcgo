#include "stack.hpp"

#include "util.hpp"
#include "Type.hpp"
#include "typed_value.hpp"

namespace runtime
{

  using namespace Type;

  Stack::Stack (size_t capacity)
    : data_ (new char[capacity])
    , base_pointer_ (NULL)
    , top_ (data_)
    , limit_ (data_ + capacity)
  {
    std::memset (data_, 0, capacity);
  }

  Stack::~Stack ()
  {
    delete[] data_;
  }

  void
  Stack::push_pointer (void* pointer)
  {
    size_t s = util::AlignUp (sizeof (void*), MemoryModel::StackAlignment);
    assert (top_ + s <= limit_);
    std::memcpy (top_, &pointer, sizeof (void*));
    top_ += s;
  }

  void*
  Stack::pop_pointer ()
  {
    void* retval;
    size_t s = util::AlignUp (sizeof (void*), MemoryModel::StackAlignment);
    assert (top_ - s >= data_);
    top_ -= s;
    std::memcpy (&retval, top_, sizeof (void*));
    return retval;
  }

  void*
  Stack::peek_pointer () const
  {
    void* retval;
    size_t s = util::AlignUp (sizeof (void*), MemoryModel::StackAlignment);
    assert (top_ - s >= data_);
    std::memcpy (&retval, top_ - s, sizeof (void*));
    return retval;
  }

  void
  Stack::push_address (ptrdiff_t offset)
  {
    push_pointer (base_pointer_ + offset);
  }

  void*
  Stack::get_address (ptrdiff_t offset)
  {
    return base_pointer_ + offset;
  }

  void*
  Stack::read_pointer (ptrdiff_t offset)
  {
    void* retval;
    const size_t size = sizeof (void*);
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    char* source = base_pointer_ + offset;
    assert (source >= data_ && source + size <= top_);
    assert (top_ + s <= limit_);
    std::memcpy (&retval, source, size);
    return retval;
  }

  void
  Stack::reserve (size_t size)
  {
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    assert (top_ + s <= limit_);
    std::memset (top_, 0, size);
    top_ += s;
  }

  void
  Stack::load (void* ptr,
               size_t size)
  {
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    assert (top_ + s <= limit_);
    std::memcpy (top_, ptr, size);
    top_ += s;
  }

  void
  Stack::store (void* ptr,
                size_t size)
  {
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    assert (top_ - s >= data_);
    top_ -= s;
    std::memcpy (ptr, top_, size);
  }

  void
  Stack::move (ptrdiff_t offset,
               size_t size)
  {
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    char* ptr = base_pointer_ + offset;
    assert (ptr >= data_ && ptr + size <= top_);
    assert (top_ - s >= data_);
    top_ -= s;
    std::memcpy (ptr, top_, size);
  }

  void
  Stack::clear (ptrdiff_t offset,
                size_t size)
  {
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    char* ptr = base_pointer_ + offset;
    assert (ptr >= data_ && ptr + size <= top_);
    std::memset (ptr, 0, s);
  }

  void
  Stack::setup (size_t size)
  {
    char* bp = top_;
    push_pointer (base_pointer_);
    base_pointer_ = bp;
    reserve (size);
  }

  void
  Stack::teardown ()
  {
    top_ = base_pointer_ + sizeof (void*);
    base_pointer_ = (char*)pop_pointer ();
  }

  void
  Stack::popn (size_t size)
  {
    size_t s = util::AlignUp (size, MemoryModel::StackAlignment);
    assert (top_ - s >= data_);
    top_ -= s;
  }

  void
  Stack::dump () const
  {
    printf ("size = %td base_pointer = %p\n", top_ - data_, base_pointer_);
    size_t increment = MemoryModel::StackAlignment;
    const char* ptr;
    for (ptr = data_; ptr != top_; ptr += increment)
      {
        size_t x;
        std::memcpy (&x, ptr, sizeof (size_t));
        printf ("%p => %zx %s\n", ptr, x, ptr == base_pointer_ ? "<--" : "");
      }
  }

  void
  Stack::write (ptrdiff_t offset,
                void* ptr,
                size_t size)
  {
    assert (base_pointer_ + offset >= data_);
    assert (base_pointer_ + offset + size <= limit_);
    std::memcpy (base_pointer_ + offset, ptr, size);
  }

  void
  Stack::push_tv (const typed_value_t& tv)
  {
    struct visitor : public DefaultVisitor
    {
      Stack* stack;
      const typed_value_t& tv;

      visitor (Stack* sf, const typed_value_t& t)
        : stack (sf)
        , tv (t)
      { }

      void visit (const Bool& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Boolean& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Int& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Int8& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Uint& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Uint8& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Uint16& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Uint32& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Uint64& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Float64& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Nil& type)
      {
        stack->push_pointer (0);
      }

      void visit (const Pointer& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Slice& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const StringU& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const Enum& type)
      {
        stack->push (tv.value.ref (type));
      }

      void visit (const String& type)
      {
        stack->push (tv.value.ref (type));
      }

      void default_action (const Type::Type& type)
      {
        type_not_reached (type);
      }
    };

    visitor v (this, tv);
    type_strip (tv.type)->Accept (v);
  }

  struct PopVisitor : public DefaultVisitor
  {
    Stack* stack;
    typed_value_t& tv;

    PopVisitor (Stack* sf, typed_value_t& t)
      : stack (sf)
      , tv (t)
    { }

    template <typename T>
    void doit (const T& type)
    {
      typename T::ValueType value;
      stack->pop (value);
      tv.value = value_t (&type, value);
    }

    void visit (const Int& type)
    {
      doit (type);
    }
    void visit (const Int8& type)
    {
      doit (type);
    }
    void visit (const Uint& type)
    {
      doit (type);
    }
    void visit (const Uint8& type)
    {
      doit (type);
    }
    void visit (const Uint16& type)
    {
      doit (type);
    }
    void visit (const Uint32& type)
    {
      doit (type);
    }
    void visit (const Uint64& type)
    {
      doit (type);
    }
    void visit (const Slice& type)
    {
      doit (type);
    }
    void visit (const StringU& type)
    {
      doit (type);
    }
    void default_action (const Type::Type& type)
    {
      type_not_reached (type);
    }
  };

  void
  Stack::pop_tv (typed_value_t& tv)
  {
    switch (tv.kind)
      {
      case typed_value_t::REFERENCE:
        tv.value = value_t::make_reference (pop_pointer ());
        break;
      case typed_value_t::VALUE:
      {
        PopVisitor v (this, tv);
        type_strip (tv.type)->Accept (v);
      }
      break;
      case typed_value_t::TYPE:
        unimplemented;
        break;
      }
  }

}
