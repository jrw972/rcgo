#include "arch.hpp"

#include "util.hpp"
#include "type.hpp"
#include "parameter_list.hpp"
#include "symbol.hpp"
#include "runtime_types.hpp"

namespace arch
{
using namespace util;
using namespace type;

static size_t stack_alignment_ = 0;

size_t stack_alignment ()
{
  return stack_alignment_;
}

void set_stack_alignment (size_t a)
{
  stack_alignment_ = a;
}

size_t alignment (const Type* type)
{
  switch (type->underlying_kind ())
    {
    // These types cannot be allocated.
    case Untyped_Nil_Kind:
    case Untyped_Boolean_Kind:
    case Untyped_Rune_Kind:
    case Untyped_Integer_Kind:
    case Untyped_Float_Kind:
    case Untyped_Complex_Kind:
    case Untyped_String_Kind:

    case Heap_Kind:

    case Method_Kind:
    case Initializer_Kind:
    case Getter_Kind:
    case Reaction_Kind:

    case Polymorphic_Function_Kind:

    case Named_Kind:
      NOT_REACHED;

    case Bool_Kind:
      return 1;
    case Uint8_Kind:
      return 1;
    case Uint16_Kind:
      return 2;
    case Uint32_Kind:
      return 4;
    case Uint64_Kind:
      return 8;
    case Int8_Kind:
      return 1;
    case Int16_Kind:
      return 2;
    case Int32_Kind:
      return 4;
    case Int64_Kind:
      return 8;
    case Float32_Kind:
      return 4;
    case Float64_Kind:
      return 8;
    case Complex64_Kind:
      return 4;
    case Complex128_Kind:
      return 8;
    case Uint_Kind:
      return sizeof (unsigned long);
    case Int_Kind:
      return sizeof (long);
    case Uintptr_Kind:
      return sizeof (size_t);
    case String_Kind:
      return sizeof (void*);

    case Struct_Kind:
    case Component_Kind:
    {
      size_t sz = 0;
      const Struct* s = type->underlying_type ()->to_struct ();
      for (Struct::const_iterator pos = s->begin (), limit = s->end ();
           pos != limit;
           ++pos)
        {
          const Type* field_type = (*pos)->type;
          sz = std::max (sz, alignment (field_type));
        }
      return sz;
    }
    case Array_Kind:
    {
      const Array* a = type->underlying_type ()->to_array ();
      return alignment (a->base_type);
    }
    case Map_Kind:
      UNIMPLEMENTED;
    case Pointer_Kind:
      return sizeof (void*);
    case Slice_Kind:
      return sizeof (void*);

    case Function_Kind:
    case Push_Port_Kind:
    case Pull_Port_Kind:
      return sizeof (void*);

    case Interface_Kind:
      UNIMPLEMENTED;

    case File_Descriptor_Kind:
      return sizeof (void*);
    }
  NOT_REACHED;
}

size_t unit_size (const Array* a)
{
  return util::align_up (size (a->base_type), alignment (a->base_type));
}

size_t unit_size (const Slice* s)
{
  return util::align_up (size (s->base_type), alignment (s->base_type));
}

size_t size (const Type* type)
{
  switch (type->underlying_kind ())
    {
    // These types cannot be allocated.
    case Untyped_Nil_Kind:
    case Untyped_Boolean_Kind:
    case Untyped_Rune_Kind:
    case Untyped_Integer_Kind:
    case Untyped_Float_Kind:
    case Untyped_Complex_Kind:
    case Untyped_String_Kind:

    case Heap_Kind:

    case Method_Kind:
    case Initializer_Kind:
    case Getter_Kind:
    case Reaction_Kind:

    case Polymorphic_Function_Kind:

    case Named_Kind:
      NOT_REACHED;

    case Bool_Kind:
      return 1;
    case Uint8_Kind:
      return 1;
    case Uint16_Kind:
      return 2;
    case Uint32_Kind:
      return 4;
    case Uint64_Kind:
      return 8;
    case Int8_Kind:
      return 1;
    case Int16_Kind:
      return 2;
    case Int32_Kind:
      return 4;
    case Int64_Kind:
      return 8;
    case Float32_Kind:
      return 4;
    case Float64_Kind:
      return 8;
    case Complex64_Kind:
      return 8;
    case Complex128_Kind:
      return 16;
    case Uint_Kind:
      return sizeof (unsigned long);
    case Int_Kind:
      return sizeof (long);
    case Uintptr_Kind:
      return sizeof (size_t);
    case String_Kind:
      return sizeof (runtime::String);

    case Struct_Kind:
    case Component_Kind:
    {
      size_t sz = 0;
      const Struct* s = type->underlying_type ()->to_struct ();
      for (Struct::const_iterator pos = s->begin (), limit = s->end ();
           pos != limit;
           ++pos)
        {
          const Type* field_type = (*pos)->type;
          sz = align_up (sz, alignment (field_type));
          sz += size (field_type);
        }
      return sz;
    }
    case Array_Kind:
    {
      const Array* a = type->underlying_type ()->to_array ();
      return unit_size (a) * a->dimension;
    }
    case Map_Kind:
      UNIMPLEMENTED;
    case Pointer_Kind:
      return sizeof (void*);
    case Slice_Kind:
      return sizeof (runtime::Slice);

    case Function_Kind:
    case Push_Port_Kind:
      return sizeof (void*);
    case Pull_Port_Kind:
      return sizeof (runtime::PullPort);

    case Interface_Kind:
      UNIMPLEMENTED;

    case File_Descriptor_Kind:
      return sizeof (void*);
    }
  NOT_REACHED;
}

size_t size_on_stack (const Type* type)
{
  return align_up (size (type), stack_alignment_);
}

size_t size_on_stack (const decl::ParameterList* list)
{
  size_t sz = 0;
  for (decl::ParameterList::const_iterator pos = list->begin (),
       limit = list->end ();
       pos != limit;
       ++pos)
    {
      sz += size_on_stack ((*pos)->type);
    }
  return sz;
}

size_t offset (const decl::Field* field)
{
  size_t sz = 0;
  const Struct* s = field->m_struct;
  for (Struct::const_iterator pos = s->begin ();
       *pos != field;
       ++pos)
    {
      const Type* field_type = (*pos)->type;
      sz = align_up (sz, alignment (field_type));
      sz += size (field_type);
    }
  return align_up (sz, alignment (field->type));
}
}
