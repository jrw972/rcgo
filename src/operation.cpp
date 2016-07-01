#include "operation.hpp"

#include <error.h>

#include "callable.hpp"
#include "composition.hpp"
#include "heap.hpp"

namespace runtime
{

using namespace type;
using namespace semantic;

struct heap_link_t
{
  Heap* heap;
  pthread_mutex_t mutex;
  size_t change_count;
};

Control
Load::execute (ExecutorBase& exec) const
{
  child->execute (exec);
  void* ptr = exec.stack ().pop_pointer ();
  exec.stack ().load (ptr, arch::size (type));
  return Control_Continue;
}


Control
IndexSlice::execute (ExecutorBase& exec) const
{
  base->execute (exec);
  runtime::Slice s;
  exec.stack ().pop (s);
  index->execute (exec);
  long i;
  exec.stack ().pop (i);

  if (i < 0 || static_cast<unsigned long> (i) >= s.length)
    {
      error_at_line (-1, 0, location.file.c_str (), location.line,
                     "slice index is out of bounds (E35)");

    }

  exec.stack ().push_pointer (static_cast<char*> (s.ptr) + i * arch::unit_size (type));

  return Control_Continue;
}

template<typename T>
struct ConvertToInt : public Operation
{
  ConvertToInt (const Operation* c) : child (c) { }
  Control execute (ExecutorBase& exec) const
  {
    child->execute (exec);
    T in;
    exec.stack ().pop (in);
    long out = in;
    exec.stack ().push (out);
    return Control_Continue;
  }
  virtual void dump () const
  {
    std::cout << "ConvertToInt(";
    child->dump ();
    std::cout << ")\n";
  }
  const Operation* const child;
};

// template<typename T>
// static Operation*
// makeConvertToInt (const T& t, const Operation* child)
// {
//   return new ConvertToInt<T> (child);
// }

// struct ConvertToIntListener
// {
//   const Operation* const child;
//   Operation* operation;
//   ConvertToIntListener (const Operation* c) : child (c), operation (NULL) { }

//   void NotIntegral (const type::Type& type)
//   {
//     NOT_REACHED;
//   }

//   void operator() (const Uint8& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Uint16& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Uint32& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Uint64& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }

//   void operator() (const Int8& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Int16& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Int32& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Int64& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }

//   void operator() (const Uint& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const type::Int& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Uintptr& type)
//   {
//     operation = makeConvertToInt (type, child);
//   }
//   void operator() (const Rune& type)
//   {
//     NotIntegral (type);
//   }
//   void operator() (const Integer& type)
//   {
//     NotIntegral (type);
//   }
// };

Operation*
MakeConvertToInt (const Operation* c, const type::Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
      return new ConvertToInt<uint8_t> (c);
    case Uint16_Kind:
      return new ConvertToInt<uint16_t> (c);
    case Uint32_Kind:
      return new ConvertToInt<uint32_t> (c);
    case Uint64_Kind:
      return new ConvertToInt<uint64_t> (c);
    case Int8_Kind:
      return new ConvertToInt<int8_t> (c);
    case Int16_Kind:
      return new ConvertToInt<int16_t> (c);
    case Int32_Kind:
      return new ConvertToInt<int32_t> (c);
    case Int64_Kind:
      return new ConvertToInt<int64_t> (c);
    case Uint_Kind:
      return new ConvertToInt<unsigned long> (c);
    case Int_Kind:
      return new ConvertToInt<long> (c);
    case Uintptr_Kind:
      return new ConvertToInt<size_t> (c);

    default:
      TYPE_NOT_REACHED (*type);
    }
}

template<typename T>
struct ConvertToUint : public Operation
{
  ConvertToUint (const Operation* c) : child (c) { }
  Control execute (ExecutorBase& exec) const
  {
    child->execute (exec);
    T in;
    exec.stack ().pop (in);
    unsigned long out = in;
    exec.stack ().push (out);
    return Control_Continue;
  }
  virtual void dump () const
  {
    std::cout << "ConvertToUint(";
    child->dump ();
    std::cout << ")";
  }
  const Operation* const child;
};

Operation*
MakeConvertToUint (const Operation* c, const type::Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
      return new ConvertToUint<uint8_t> (c);
    case Uint16_Kind:
      return new ConvertToUint<uint16_t> (c);
    case Uint32_Kind:
      return new ConvertToUint<uint32_t> (c);
    case Uint64_Kind:
      return new ConvertToUint<uint64_t> (c);
    case Int8_Kind:
      return new ConvertToUint<int8_t> (c);
    case Int16_Kind:
      return new ConvertToUint<int16_t> (c);
    case Int32_Kind:
      return new ConvertToUint<int32_t> (c);
    case Int64_Kind:
      return new ConvertToUint<int64_t> (c);
    case Uint_Kind:
      return new ConvertToUint<unsigned long> (c);
    case Int_Kind:
      return new ConvertToUint<long> (c);
    case Uintptr_Kind:
      return new ConvertToUint<size_t> (c);

    default:
      TYPE_NOT_REACHED (*type);
    }
}

Control
LogicOr::execute (ExecutorBase& exec) const
{
  left->execute (exec);
  bool b;
  exec.stack ().pop (b);
  if (b)
    {
      exec.stack ().push (b);
    }
  else
    {
      right->execute (exec);
    }
  return Control_Continue;
}

Control
LogicAnd::execute (ExecutorBase& exec) const
{
  left->execute (exec);
  bool b;
  exec.stack ().pop (b);
  if (!b)
    {
      exec.stack ().push (b);
    }
  else
    {
      right->execute (exec);
    }
  return Control_Continue;
}

Operation* make_literal (const type::Type* type, const Value& value)
{
  assert (value.present);

  switch (type->underlying_type ()->kind ())
    {
    case Bool_Kind:
      return make_literal (value.bool_value);
    case Uint8_Kind:
      return make_literal (value.uint8_value);
    case Uint16_Kind:
      return make_literal (value.uint16_value);
    case Uint32_Kind:
      return make_literal (value.uint32_value);
    case Uint64_Kind:
      return make_literal (value.uint64_value);
    case Int8_Kind:
      return make_literal (value.int8_value);
    case Int16_Kind:
      return make_literal (value.int16_value);
    case Int32_Kind:
      return make_literal (value.int32_value);
    case Int64_Kind:
      return make_literal (value.int64_value);
    case Uint_Kind:
      return make_literal (value.uint_value);
    case Int_Kind:
      return make_literal (value.int_value);
    case Float64_Kind:
      return make_literal (value.float64_value);
    case String_Kind:
      return make_literal (value.string_value);
    case Pointer_Kind:
      return make_literal (value.pointer_value);
    case Slice_Kind:
      return make_literal (value.slice_value);
    default:
      TYPE_NOT_REACHED (*type);
    }
}

Control
ListOperation::execute (ExecutorBase& exec) const
{
  for (ListType::const_iterator pos = list.begin (), limit = list.end ();
       pos != limit;
       ++pos)
    {
      Control r = (*pos)->execute (exec);
      if (r == Control_Return)
        {
          return r;
        }
    }
  return Control_Continue;
}

Control
FunctionCall::execute (ExecutorBase& exec) const
{
  // Create space for the return.
  exec.stack ().reserve (arch::size_on_stack (callable->return_parameter_list ()));

  // Push the arguments.
  arguments->execute (exec);

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Setup the frame.
  exec.stack ().setup (callable->memory_model.locals_size_on_stack ());

  // Do the call.
  callable->call (exec);

  // Tear down the frame.
  exec.stack ().teardown ();

  // Pop the fake instruction pointer.
  exec.stack ().pop_pointer ();

  // Pop the arguments.
  exec.stack ().popn (arch::size_on_stack (callable->parameter_list ()));

  return Control_Continue;
}

Control
MethodCall::execute (ExecutorBase& exec) const
{
  // Create space for the return.
  exec.stack ().reserve (arch::size_on_stack (callable->return_parameter_list ()));

  // Push the receiver.
  receiver->execute (exec);

  // Push the arguments.
  arguments->execute (exec);

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Setup the frame.
  exec.stack ().setup (callable->memory_model.locals_size_on_stack ());

  // Do the call.
  callable->call (exec);

  // Tear down the frame.
  exec.stack ().teardown ();

  // Pop the fake instruction pointer.
  exec.stack ().pop_pointer ();

  // Pop the arguments.
  exec.stack ().popn (arch::size_on_stack (callable->parameter_list ()));

  return Control_Continue;
}

Control
DynamicPullPortCall::execute (ExecutorBase& exec) const
{
  func->execute (exec);
  PullPort pp;
  exec.stack ().pop (pp);

  // Create space for the return.
  exec.stack ().reserve (arch::size_on_stack (pp.getter->return_parameter_list ()));

  // Push the arguments.
  exec.stack ().push_pointer (pp.instance);
  arguments->execute (exec);

  // Push a fake instruction pointer.
  exec.stack ().push_pointer (NULL);

  // Setup the frame.
  exec.stack ().setup (pp.getter->memory_model.locals_size_on_stack ());

  // Do the call.
  pp.getter->call (exec);

  // Tear down the frame.
  exec.stack ().teardown ();

  // Pop the fake instruction pointer.
  exec.stack ().pop_pointer ();

  // Pop the arguments.
  exec.stack ().popn (arch::size_on_stack (pp.getter->parameter_list ()));

  return Control_Continue;
}

Control
Instance::execute (ExecutorBase& exec) const
{
  exec.stack ().push_pointer (instance->instance->component);
  return Control_Continue;
}

Control
SetRestoreCurrentInstance::execute (ExecutorBase& exec) const
{
  ComponentInfoBase* new_info = component_to_info (static_cast<component_t*> (exec.stack ().read_pointer (receiver_offset)));
  ComponentInfoBase* old_info = exec.current_info (new_info);
  Control ca = child->execute (exec);
  exec.current_info (old_info);
  return ca;
}

Control
Clear::execute (ExecutorBase& exec) const
{
  exec.stack ().clear (offset, size);
  return Control_Continue;
}

Control
Assign::execute (ExecutorBase& exec) const
{
  left->execute (exec);
  void* ptr = exec.stack ().pop_pointer ();
  right->execute (exec);
  exec.stack ().store (ptr, size);
  return Control_Continue;
}

template <typename T>
struct AddAssign : public Operation
{
  AddAssign (Operation* l, Operation* r) : left (l), right (r)
  {
    assert (left != NULL);
    assert (right != NULL);
  }
  Control
  execute (ExecutorBase& exec) const
  {
    left->execute (exec);
    T* ptr = static_cast<T*> (exec.stack ().pop_pointer ());
    right->execute (exec);
    T v;
    exec.stack ().pop (v);
    *ptr += v;
    return Control_Continue;
  }
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const left;
  Operation* const right;
};

Operation* make_add_assign (Operation* l, Operation* r, const type::Type* t)
{
  switch (t->underlying_kind ())
    {
    case Uint8_Kind:
      return new AddAssign<uint8_t> (l, r);
    case Uint16_Kind:
      return new AddAssign<uint16_t> (l, r);
    case Uint32_Kind:
      return new AddAssign<uint32_t> (l, r);
    case Uint64_Kind:
      return new AddAssign<uint64_t> (l, r);
    case Int8_Kind:
      return new AddAssign<int8_t> (l, r);
    case Int16_Kind:
      return new AddAssign<int16_t> (l, r);
    case Int32_Kind:
      return new AddAssign<int32_t> (l, r);
    case Int64_Kind:
      return new AddAssign<int64_t> (l, r);
    case Float32_Kind:
      return new AddAssign<float> (l, r);
    case Float64_Kind:
      return new AddAssign<double> (l, r);
    case Complex64_Kind:
      return new AddAssign<Complex64> (l, r);
    case Complex128_Kind:
      return new AddAssign<Complex128> (l, r);
    case Uint_Kind:
      return new AddAssign<unsigned long> (l, r);
    case Int_Kind:
      return new AddAssign<long> (l, r);
    case Uintptr_Kind:
      return new AddAssign<size_t> (l, r);
    default:
      NOT_REACHED;
    }
}

Control
Reference::execute (ExecutorBase& exec) const
{
  exec.stack ().push_address (offset);
  return Control_Continue;
}

Control
Select::execute (ExecutorBase& exec) const
{
  base->execute (exec);
  char* ptr = static_cast<char*> (exec.stack ().pop_pointer ());
  ptr += offset;
  exec.stack ().push_pointer (ptr);
  return Control_Continue;
}

Control
IndexArray::execute (ExecutorBase& exec) const
{
  base->execute (exec);
  void* ptr = exec.stack ().pop_pointer ();
  index->execute (exec);
  long i;
  exec.stack ().pop (i);
  if (i < 0 || i >= type->dimension)
    {
      error_at_line (-1, 0, location.file.c_str (), location.line,
                     "array index is out of bounds (E148)");
    }
  exec.stack ().push_pointer (static_cast<char*> (ptr) + i * arch::unit_size (type));
  return Control_Continue;
}

Control
SliceArray::execute (ExecutorBase& exec) const
{
  base->execute (exec);
  char* ptr = static_cast<char*> (exec.stack ().pop_pointer ());
  long low_val = 0;
  if (low)
    {
      low->execute (exec);
      exec.stack ().pop (low_val);
    }
  long high_val = type->dimension;
  if (high)
    {
      high->execute (exec);
      exec.stack ().pop (high_val);
    }
  long max_val = type->dimension;
  if (max)
    {
      max->execute (exec);
      exec.stack ().pop (max_val);
    }

  // Bounds check.
  if (!(0 <= low_val &&
        low_val <= high_val &&
        high_val <= max_val &&
        max_val <= type->dimension))
    {
      error_at_line (-1, 0, location.file.c_str (), location.line,
                     "slice index is out of range (E223)");
    }

  runtime::Slice slice_val;
  slice_val.length = high_val - low_val;
  slice_val.capacity = max_val - low_val;
  slice_val.ptr = slice_val.length ? ptr + low_val * arch::unit_size (type) : NULL;
  exec.stack ().push (slice_val);

  return Control_Continue;
}

Control
SliceSlice::execute (ExecutorBase& exec) const
{
  base->execute (exec);
  runtime::Slice s;
  exec.stack ().pop (s);
  long low_val = 0;
  if (low)
    {
      low->execute (exec);
      exec.stack ().pop (low_val);
    }
  long high_val = s.length;
  if (high)
    {
      high->execute (exec);
      exec.stack ().pop (high_val);
    }
  long max_val = s.capacity;
  if (max)
    {
      max->execute (exec);
      exec.stack ().pop (max_val);
    }

  // Bounds check.
  if (!(0 <= low_val &&
        low_val <= high_val &&
        high_val <= max_val &&
        max_val <= static_cast<long> (s.capacity)))
    {
      error_at_line (-1, 0, location.file.c_str (), location.line,
                     "slice index is out of range (E22)");
    }

  s.length = high_val - low_val;
  s.capacity = max_val - low_val;
  s.ptr = s.length ? static_cast<char*> (s.ptr) + low_val * arch::unit_size (type) : NULL;
  exec.stack ().push (s);

  return Control_Continue;
}

Control
Return::execute (ExecutorBase& exec) const
{
  child->execute (exec);
  exec.stack ().move (return_offset, return_size);
  return Control_Return;
}

Control
If::execute (ExecutorBase& exec) const
{
  condition->execute (exec);
  bool c;
  exec.stack ().pop (c);
  if (c)
    {
      return true_branch->execute (exec);
    }
  else
    {
      return false_branch->execute (exec);
    }
}

Control
While::execute (ExecutorBase& exec) const
{
  for (;;)
    {
      condition->execute (exec);
      bool c;
      exec.stack ().pop (c);
      if (c)
        {
          Control r = body->execute (exec);
          if (r == Control_Return)
            {
              return r;
            }
        }
      else
        {
          return Control_Continue;
        }
    }
}

Control
Change::execute (ExecutorBase& exec) const
{
  root->execute (exec);
  heap_link_t* hl = static_cast<heap_link_t*> (exec.stack ().pop_pointer ());
  if (hl == NULL)
    {
      // Heap link is null.
      UNIMPLEMENTED;
    }
  pthread_mutex_lock (&hl->mutex);
  ++hl->change_count;
  pthread_mutex_unlock (&hl->mutex);

  // Save the old heap.
  Heap* old_heap = exec.heap ();
  // Set the the new heap.
  exec.heap (hl->heap);

  char** root_value = static_cast<char**> (exec.stack ().get_address (root_offset));

  // Push a pointer to the root object.
  *root_value = static_cast<char*> (hl->heap->root ());

  Control ca = body->execute (exec);

  // Restore the old heap.
  exec.heap (old_heap);

  pthread_mutex_lock (&hl->mutex);
  --hl->change_count;
  pthread_mutex_unlock (&hl->mutex);

  return ca;
}

template <typename T>
struct Increment : public Operation
{
  Increment (Operation* c) : child (c) { }
  virtual Control execute (ExecutorBase& exec) const
  {
    child->execute (exec);
    T* ptr = static_cast<T*> (exec.stack ().pop_pointer ());
    ++*ptr;
    return Control_Continue;
  }
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const child;
};

template <typename T>
struct Decrement : public Operation
{
  Decrement (Operation* c) : child (c) { }
  virtual Control execute (ExecutorBase& exec) const
  {
    child->execute (exec);
    T* ptr = static_cast<T*> (exec.stack ().pop_pointer ());
    --*ptr;
    return Control_Continue;
  }
  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const child;
};

Operation* make_increment (Operation* child, const type::Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
      return new Increment<uint8_t> (child);
    case Uint16_Kind:
      return new Increment<uint16_t> (child);
    case Uint32_Kind:
      return new Increment<uint32_t> (child);
    case Uint64_Kind:
      return new Increment<uint64_t> (child);
    case Int8_Kind:
      return new Increment<int8_t> (child);
    case Int16_Kind:
      return new Increment<int16_t> (child);
    case Int32_Kind:
      return new Increment<int32_t> (child);
    case Int64_Kind:
      return new Increment<int64_t> (child);
    case Float32_Kind:
      return new Increment<float> (child);
    case Float64_Kind:
      return new Increment<double> (child);
    case Complex64_Kind:
      return new Increment<Complex64> (child);
    case Complex128_Kind:
      return new Increment<Complex128> (child);
    case Uint_Kind:
      return new Increment<unsigned long> (child);
    case Int_Kind:
      return new Increment<long> (child);
    case Uintptr_Kind:
      return new Increment<size_t> (child);
    default:
      TYPE_NOT_REACHED (*type);
    }
}

Operation* make_decrement (Operation* child, const type::Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
      return new Decrement<uint8_t> (child);
    case Uint16_Kind:
      return new Decrement<uint16_t> (child);
    case Uint32_Kind:
      return new Decrement<uint32_t> (child);
    case Uint64_Kind:
      return new Decrement<uint64_t> (child);
    case Int8_Kind:
      return new Decrement<int8_t> (child);
    case Int16_Kind:
      return new Decrement<int16_t> (child);
    case Int32_Kind:
      return new Decrement<int32_t> (child);
    case Int64_Kind:
      return new Decrement<int64_t> (child);
    case Float32_Kind:
      return new Decrement<float> (child);
    case Float64_Kind:
      return new Decrement<double> (child);
    case Complex64_Kind:
      return new Decrement<Complex64> (child);
    case Complex128_Kind:
      return new Decrement<Complex128> (child);
    case Uint_Kind:
      return new Decrement<unsigned long> (child);
    case Int_Kind:
      return new Decrement<long> (child);
    case Uintptr_Kind:
      return new Decrement<size_t> (child);
    default:
      TYPE_NOT_REACHED (*type);
    }
}

Control
Activate::execute (ExecutorBase& exec) const
{
  // Save the base pointer to return to.
  // Currently, this is done in the port call.
  // char* return_bp = *reinterpret_cast<char**> (exec.stack ().base_pointer ());
  // Insert into the mutable phase list.
  *reinterpret_cast<char**> (exec.stack ().base_pointer ()) = exec.mutable_phase_base_pointer ();
  exec.mutable_phase_base_pointer (exec.stack ().base_pointer ());

  // The caller pushed a (fake) instruction pointer which is just
  // before the base pointer.  Overwrite it with the body.
  Operation** ip = static_cast<Operation**> (exec.stack ().pointer_to_instruction_pointer ());
  *ip = body;
  // Execute the expression list.
  port_calls->execute (exec);

  return Control_Return;
}

static void push_port_call (ExecutorBase& exec,
                            Operation* args,
                            size_t arguments_size,
                            ptrdiff_t receiver_offset,
                            ptrdiff_t port_offset,
                            ptrdiff_t array_offset)
{
  // Push all of the arguments.
  char* top_before = exec.stack ().top ();
  args->execute (exec);

  // Find the port to activate.
  PushPort* port = *reinterpret_cast<PushPort**> (static_cast<char*> (exec.stack ().read_pointer (receiver_offset)) + port_offset + array_offset);

  char* base_pointer = exec.stack ().base_pointer ();

  // Activate all the reactions bound to the port.
  while (port != NULL)
    {
      // Set up a frame.
      // Push the parameter.
      if (port->reaction->has_dimension ())
        {
          exec.stack ().push<long> (port->parameter);
        }
      // Push the instance.
      exec.stack ().push_pointer (port->instance);
      // Push the arguments.
      exec.stack ().load (top_before, arguments_size);
      // Push an instruction pointer.
      exec.stack ().push_pointer (NULL);

      // Jump to the last frame.
      exec.stack ().base_pointer (exec.mutable_phase_base_pointer ());

      exec.stack ().setup (port->reaction->memory_model.locals_size_on_stack ());

      port->reaction->call (exec);

      // Move back to our frame.
      exec.stack ().base_pointer (base_pointer);

      port = port->next;
    }
}

Control
PushPortCall::execute (ExecutorBase& exec) const
{
  push_port_call (exec, args, arguments_size, receiver_offset, port_offset, 0);
  return Control_Continue;
}

Control
IndexedPushPortCall::execute (ExecutorBase& exec) const
{
  index->execute (exec);
  long idx;
  exec.stack ().pop (idx);
  push_port_call (exec, args, arguments_size, receiver_offset, port_offset, idx * arch::unit_size (array_type));
  return Control_Continue;
}

Control
Push::execute (ExecutorBase& exec) const
{
  Control ca = body->execute (exec);
  exec.push ();
  return ca;
}

Control
ForIota::execute (ExecutorBase& exec) const
{
  for (long idx = 0; idx != limit; ++idx)
    {
      long* i = static_cast<long*> (exec.stack ().get_address (offset));
      *i = idx;
      Control r = body->execute (exec);
      if (r == Control_Return)
        {
          return r;
        }
    }
  return Control_Continue;
}

struct ConvertStringToSliceOfBytes : public Operation
{
  ConvertStringToSliceOfBytes (Operation* c) : child (c) { }
  Control
  execute (ExecutorBase& exec) const
  {
    child->execute (exec);
    runtime::String in;
    exec.stack ().pop (in);
    runtime::Slice out;
    out.ptr = exec.heap ()->allocate (in.length);
    memcpy (out.ptr, in.ptr, in.length);
    out.length = in.length;
    out.capacity = in.length;
    exec.stack ().push (out);
    return Control_Continue;
  }

  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  Operation* const child;
};

struct ConvertSliceOfBytesToString : public Operation
{
  ConvertSliceOfBytesToString (Operation* c) : child (c) { }
  Control
  execute (ExecutorBase& exec) const
  {
    child->execute (exec);
    runtime::Slice in;
    exec.stack ().pop (in);
    runtime::String out;
    void* o = exec.heap ()->allocate (in.length);
    memcpy (o, in.ptr, in.length);
    out.ptr = o;
    out.length = in.length;
    exec.stack ().push (out);
    return Control_Continue;
  }

  virtual void dump () const
  {
    std::cout << "ConvertSliceOfBytesToString(";
    child->dump ();
    std::cout << ")";
  }
  Operation* const child;
};

template<typename FromType, typename ToType>
struct Conversion : public Operation
{
  Conversion (Operation* c) : child (c) { }
  virtual Control execute (ExecutorBase& exec) const
  {
    child->execute (exec);
    FromType x;
    exec.stack ().pop (x);
    ToType y = x;
    exec.stack ().push (y);
    return Control_Continue;
  }
  virtual void dump () const
  {
    std::cout << "Conversion(";
    child->dump ();
    std::cout << ")";
  }
  Operation* const child;
};


template <typename T1>
static Operation* make_conversion1 (Operation* c, const type::Type* to)
{
  switch (to->underlying_kind ())
    {
    case Uint8_Kind:
      return new Conversion<T1, uint8_t> (c);
    case Uint16_Kind:
      return new Conversion<T1, uint16_t> (c);
    case Uint32_Kind:
      return new Conversion<T1, uint32_t> (c);
    case Uint64_Kind:
      return new Conversion<T1, uint64_t> (c);
    case Int8_Kind:
      return new Conversion<T1, int8_t> (c);
    case Int16_Kind:
      return new Conversion<T1, int16_t> (c);
    case Int32_Kind:
      return new Conversion<T1, int32_t> (c);
    case Int64_Kind:
      return new Conversion<T1, int64_t> (c);
    case Float32_Kind:
      return new Conversion<T1, float> (c);
    case Float64_Kind:
      return new Conversion<T1, double> (c);
    case Uint_Kind:
      return new Conversion<T1, unsigned long> (c);
    case Int_Kind:
      return new Conversion<T1, long> (c);
    default:
      break;
    }
  NOT_REACHED;
}

Operation* make_conversion (Operation* c, const type::Type* from, const type::Type* to)
{
  if (are_identical (from->underlying_type (), to->underlying_type ()))
    {
      return c;
    }
  else if (from->to_pointer () &&
           to->to_pointer () &&
           are_identical (from->to_pointer (), to->to_pointer ()))
    {
      UNIMPLEMENTED;
    }
  else if ((from->is_typed_integer () || from->is_typed_float ()) &&
           (to->is_typed_integer () || to->is_typed_float ()))
    {
      switch (from->underlying_kind ())
        {
        case Uint8_Kind:
          return make_conversion1<uint8_t> (c, to);
        case Uint16_Kind:
          return make_conversion1<uint16_t> (c, to);
        case Uint32_Kind:
          return make_conversion1<uint32_t> (c, to);
        case Uint64_Kind:
          return make_conversion1<uint64_t> (c, to);
        case Int8_Kind:
          return make_conversion1<int8_t> (c, to);
        case Int16_Kind:
          return make_conversion1<int16_t> (c, to);
        case Int32_Kind:
          return make_conversion1<int32_t> (c, to);
        case Int64_Kind:
          return make_conversion1<int64_t> (c, to);
        case Float32_Kind:
          return make_conversion1<float> (c, to);
        case Float64_Kind:
          return make_conversion1<double> (c, to);
        case Uint_Kind:
          return make_conversion1<unsigned long> (c, to);
        case Int_Kind:
          return make_conversion1<long> (c, to);
        default:
          break;
        }
      NOT_REACHED;
    }
  else if (from->is_typed_complex () && to->is_typed_complex ())
    {
      UNIMPLEMENTED;
    }
  else if (from->is_typed_integer () && to->is_typed_string ())
    {
      UNIMPLEMENTED;
    }
  else if (from->is_slice_of_bytes () && to->is_typed_string ())
    {
      return new ConvertSliceOfBytesToString (c);
    }
  else if (from->is_slice_of_runes () && to->is_typed_string ())
    {
      UNIMPLEMENTED;
    }
  else if (from->is_typed_string () && to->is_slice_of_bytes ())
    {
      return new ConvertStringToSliceOfBytes (c);
    }
  else if (from->is_typed_string () && to->is_slice_of_runes ())
    {
      UNIMPLEMENTED;
    }
  else
    {
      // Assignable.
      UNIMPLEMENTED;
    }
}

Control
Popn::execute (ExecutorBase& exec) const
{
  Control r = child->execute (exec);
  exec.stack ().popn (size);
  return r;
}

Control PrintlnOp::execute (ExecutorBase& exec) const
{
  ListOperation* lop = static_cast<ListOperation*> (args);

  exec.lock_stdout ();
  for (size_t idx = 0; idx != lop->list.size (); ++idx)
    {
      lop->list[idx]->execute (exec);

      switch (evals[idx].type->underlying_kind ())
        {
        case Bool_Kind:
        {
          bool x;
          exec.stack ().pop (x);
          if (x)
            {
              printf ("true");
            }
          else
            {
              printf ("false");
            }
        }
        break;

        case Uint8_Kind:
        {
          uint8_t x;
          exec.stack ().pop (x);
          printf ("%u", x);
        }
        break;

        case Uint16_Kind:
        {
          uint16_t x;
          exec.stack ().pop (x);
          printf ("%u", x);
        }
        break;

        case Uint32_Kind:
        {
          uint32_t x;
          exec.stack ().pop (x);
          printf ("%u", x);
        }
        break;

        case Uint64_Kind:
        {
          uint64_t x;
          exec.stack ().pop (x);
          printf ("%lu", x);
        }
        break;

        case Int8_Kind:
        {
          int8_t x;
          exec.stack ().pop (x);
          printf ("%d", x);
        }
        break;

        case Int16_Kind:
        {
          int16_t x;
          exec.stack ().pop (x);
          printf ("%d", x);
        }
        break;

        case Int32_Kind:
        {
          int32_t x;
          exec.stack ().pop (x);
          printf ("%d", x);
        }
        break;

        case Int64_Kind:
        {
          int64_t x;
          exec.stack ().pop (x);
          printf ("%ld", x);
        }
        break;

        case Float32_Kind:
        {
          float x;
          exec.stack ().pop (x);
          printf ("%g", x);
        }
        break;

        case Float64_Kind:
        {
          double x;
          exec.stack ().pop (x);
          printf ("%g", x);
        }
        break;

        case Complex64_Kind:
        case Complex128_Kind:
          TYPE_NOT_REACHED (*evals[idx].type);

        case Uint_Kind:
        {
          unsigned long x;
          exec.stack ().pop (x);
          printf ("%lu", x);
        }
        break;

        case Int_Kind:
        {
          long x;
          exec.stack ().pop (x);
          printf ("%ld", x);
        }
        break;

        case Uintptr_Kind:
        {
          size_t x;
          exec.stack ().pop (x);
          printf ("%lu", x);
        }
        break;

        case String_Kind:
        {
          runtime::String x;
          exec.stack ().pop (x);
          fwrite (x.ptr, 1, x.length, stdout);
        }
        break;

        case Pointer_Kind:
        {
          void* x;
          exec.stack ().pop (x);
          printf ("%p", x);
        }
        break;

        default:
          TYPE_NOT_REACHED (*evals[idx].type);
        }

    }
  printf ("\n");
  exec.unlock_stdout ();

  return Control_Continue;
}

static heap_link_t*
make_heap_link (Heap* heap,
                Heap* allocator)
{
  // Allocate a new heap link in the parent.
  heap_link_t* hl = static_cast<heap_link_t*> (allocator->allocate (sizeof (heap_link_t)));
  // Set up the link.
  hl->heap = heap;
  pthread_mutex_init (&hl->mutex, NULL);
  return hl;
}

Control NewOp::execute (ExecutorBase& exec) const
{
  // Allocate a new instance of the type.
  const type::Heap* heap_type = type->to_heap ();
  if (heap_type == NULL)
    {
      exec.stack ().push (exec.heap ()->allocate (arch::size (type)));
    }
  else
    {
      const type::Type* t = heap_type->base_type;
      // Allocate a new heap and root object.
      Heap* h = new Heap (arch::size (t));
      // Insert it into its parent.
      Heap* h2 = exec.heap ();
      h2->insert_child (h);
      // Allocate a new heap link in the parent.
      heap_link_t* hl = make_heap_link (h, h2);
      exec.stack ().push (hl);
    }

  return Control_Continue;
}

Control MoveOp::execute (ExecutorBase& exec) const
{
  arg->execute (exec);

  heap_link_t* hl;
  exec.stack ().pop (hl);
  if (hl != NULL)
    {
      pthread_mutex_lock (&hl->mutex);
      if (hl->heap != NULL && hl->change_count == 0)
        {
          // Break the link.
          Heap* h = hl->heap;
          hl->heap = NULL;
          pthread_mutex_unlock (&hl->mutex);

          // Remove from parent.
          h->remove_from_parent ();
          // Insert into the new parent.
          Heap* h2 = exec.heap ();
          h2->insert_child (h);

          // Allocate a new heap link in the parent.
          heap_link_t* new_hl = make_heap_link (h, h2);

          // Return the heap link.
          exec.stack ().push (new_hl);
        }
      else
        {
          pthread_mutex_unlock (&hl->mutex);
          exec.stack ().push (NULL);
        }
    }
  else
    {
      exec.stack ().push (NULL);
    }

  return Control_Continue;
}


Control MergeOp::execute (ExecutorBase& exec) const
{
  arg->execute (exec);

  heap_link_t* hl;
  exec.stack ().pop (hl);
  if (hl != NULL)
    {
      pthread_mutex_lock (&hl->mutex);
      if (hl->heap != NULL && hl->change_count == 0)
        {
          // Break the link.
          Heap* h = hl->heap;
          hl->heap = NULL;
          pthread_mutex_unlock (&hl->mutex);

          // Get the heap root.
          char* root = static_cast<char*> (h->root ());

          // Remove from parent.
          h->remove_from_parent ();

          // Merge into the new parent.
          exec.heap ()->merge (h);

          // Return the root.
          exec.stack ().push (root);
        }
      else
        {
          pthread_mutex_unlock (&hl->mutex);
          exec.stack ().push (NULL);
        }
    }
  else
    {
      exec.stack ().push (NULL);
    }

  return Control_Continue;
}

Control LenOp::execute (ExecutorBase& exec) const
{
  arg->execute (exec);
  runtime::Slice slice;
  exec.stack ().pop (slice);
  long retval = slice.length;
  exec.stack ().push (retval);
  return Control_Continue;
}

template <typename T>
struct AppendOp : public Operation
{
  AppendOp (const type::Slice* a_slice_type,
            Operation* a_arg)
    : slice_type (a_slice_type)
    , arg (a_arg)
  { }

  virtual Control execute (ExecutorBase& exec) const
  {
    arg->execute (exec);

    // Pop the element.
    T element;
    exec.stack ().pop (element);

    // Pop the slice.
    runtime::Slice slice;
    exec.stack ().pop (slice);

    const unsigned long new_length = slice.length + 1;
    if (new_length > slice.capacity)
      {
        const unsigned long new_capacity = 2 * new_length;
        void* ptr = exec.heap ()->allocate (new_capacity * arch::unit_size (slice_type));
        memcpy (ptr, slice.ptr, slice.length * arch::unit_size (slice_type));
        slice.ptr = ptr;
        slice.capacity = new_capacity;
      }
    memcpy (static_cast<char*> (slice.ptr) + slice.length * arch::unit_size (slice_type), &element, arch::unit_size (slice_type));
    slice.length = new_length;
    exec.stack ().push (slice);
    return Control_Continue;
  }

  virtual void dump () const
  {
    UNIMPLEMENTED;
  }
  const type::Slice* const slice_type;
  Operation* const arg;
};

Operation* make_append (const type::Slice* slice_type, Operation* args)
{
  switch (slice_type->base_type->underlying_kind ())
    {
    case Uint8_Kind:
      return new AppendOp<uint8_t> (slice_type, args);
    default:
      TYPE_NOT_REACHED (*slice_type->base_type);
    }
}

Control CopyOp::execute (ExecutorBase& exec) const
{
  arg->execute (exec);

  const type::Slice* slice_type = type->underlying_type ()->to_slice ();
  if (slice_type != NULL)
    {
      runtime::Slice in;
      exec.stack ().pop (in);
      runtime::Slice out;
      size_t sz = arch::unit_size (slice_type) * in.length;
      out.ptr = exec.heap ()->allocate (sz);
      memcpy (out.ptr, in.ptr, sz);
      out.length = in.length;
      out.capacity = in.length;
      exec.stack ().push (out);
      return Control_Continue;
    }

  if (type->underlying_type ()->kind () == String_Kind)
    {
      runtime::String in;
      exec.stack ().pop (in);
      runtime::String out;
      void* o = exec.heap ()->allocate (in.length);
      memcpy (o, in.ptr, in.length);
      out.ptr = o;
      out.length = in.length;
      exec.stack ().push (out);
      return Control_Continue;
    }

  NOT_REACHED;
}

}
