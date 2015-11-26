#include "Type.hpp"
#include "action.hpp"
#include "reaction.hpp"
#include "field.hpp"
#include "Callable.hpp"
#include "bind.hpp"

namespace Type
{
  using namespace decl;

  std::ostream&
  operator<< (std::ostream& o, const Type& type)
  {
    return o << type.ToString ();
  }

  bool Type::IsString () const
  {
    unimplemented;
  }
  bool Type::IsComplex () const
  {
    unimplemented;
  }
  bool Type::IsSliceOfBytes () const
  {
    unimplemented;
  }
  bool Type::IsSliceOfRunes () const
  {
    unimplemented;
  }

  const Pointer*
  Type::GetPointer () const
  {
    if (pointer_ == NULL)
      {
        const_cast<Type*> (this)->pointer_ = new Pointer (this);
      }
    return pointer_;
  }

  const Slice*
  Type::GetSlice () const
  {
    if (slice_ == NULL)
      {
        const_cast<Type*> (this)->slice_ = new Slice (this);
      }
    return slice_;
  }

  const Array*
  Type::GetArray (UintValueType dimension) const
  {
    const Array* a;
    ArraysType::const_iterator pos = arrays_.find (dimension);
    if (pos != arrays_.end ())
      {
        a = pos->second;
      }
    else
      {
        a = new Array (dimension, this);
        const_cast<Type*> (this)->arrays_[dimension] = a;
      }
    return a;
  }

  const Heap*
  Type::GetHeap () const
  {
    if (heap_ == NULL)
      {
        const_cast<Type*> (this)->heap_ = new Heap (this);
      }
    return heap_;
  }

  std::string
  Array::ToString () const
  {
    std::stringstream str;
    str << '[' << dimension << ']' << *base_;
    return str.str ();
  }


  reaction_t *
  NamedType::GetReaction (const std::string& identifier) const
  {
    for (std::vector<reaction_t*>::const_iterator pos = reactions_.begin (),
         limit = reactions_.end ();
         pos != limit;
         ++pos)
      {
        reaction_t *a = *pos;
        if (a->name == identifier)
          {
            return a;
          }
      }

    return NULL;
  }

  ::Method*
  NamedType::GetMethod (const std::string& identifier) const
  {
    for (std::vector< ::Method*>::const_iterator pos = this->methods_.begin (),
         limit = this->methods_.end ();
         pos != limit;
         ++pos)
      {
        ::Method* a = *pos;
        if (a->name == identifier)
          {
            return a;
          }
      }

    return NULL;
  }

  Initializer*
  NamedType::GetInitializer (const std::string& identifier) const
  {
    for (std::vector<Initializer*>::const_iterator pos = this->initializers_.begin (),
         limit = this->initializers_.end ();
         pos != limit;
         ++pos)
      {
        Initializer* a = *pos;
        if (a->name == identifier)
          {
            return a;
          }
      }

    return NULL;
  }

  Getter*
  NamedType::GetGetter (const std::string& identifier) const
  {
    for (std::vector<Getter*>::const_iterator pos = this->getters_.begin (),
         limit = this->getters_.end ();
         pos != limit;
         ++pos)
      {
        Getter* a = *pos;
        if (a->name == identifier)
          {
            return a;
          }
      }

    return NULL;
  }

  Action*
  NamedType::GetAction (const std::string& identifier) const
  {
    for (ActionsType::const_iterator pos = this->actions_.begin (),
         limit = this->actions_.end ();
         pos != limit;
         ++pos)
      {
        Action* a = *pos;
        if (a->name == identifier)
          {
            return a;
          }
      }

    return NULL;
  }

  bind_t*
  NamedType::GetBind (const std::string& identifier) const
  {
    for (BindsType::const_iterator pos = this->binds_.begin (),
         limit = this->binds_.end ();
         pos != limit;
         ++pos)
      {
        bind_t* a = *pos;
        if (a->name == identifier)
          {
            return a;
          }
      }

    return NULL;
  }

  void
  Struct::Append (const std::string& field_name, const Type* field_type)
  {
    size_t alignment = field_type->Alignment ();
    offset_ = util::AlignUp (offset_, alignment);

    field_t *field = new field_t (field_name, field_type, offset_);
    fields_.push_back (field);

    offset_ += field_type->Size ();
    if (alignment > alignment_)
      {
        alignment_ = alignment;
      }
  }

  field_t *
  Struct::Find (const std::string& name) const
  {
    for (std::vector<field_t*>::const_iterator field = fields_.begin (),
         limit = fields_.end ();
         field != limit;
         ++field)
      {
        if (name == (*field)->name)
          {
            return (*field);
          }
      }
    return NULL;
  }

  field_t *
  type_select_field (const Type* type, const std::string& identifier)
  {
    struct visitor : public DefaultVisitor
    {
      field_t* retval;
      const std::string& identifier;
      visitor (const std::string& id) : retval (NULL), identifier (id) { }

      void visit (const NamedType& type)
      {
        type.UnderlyingType ()->Accept (*this);
      }

      void visit (const Component& type)
      {
        retval = type.Find (identifier);
      }

      void visit (const Struct& type)
      {
        retval = type.Find (identifier);
      }

    };
    visitor v (identifier);
    type->Accept (v);
    return v.retval;
  }

  ::Method*
  type_select_method (const Type* type, const std::string& identifier)
  {
    struct visitor : public DefaultVisitor
    {
      ::Method* retval;
      const std::string& identifier;
      visitor (const std::string& id) : retval (NULL), identifier (id) { }

      void visit (const NamedType& type)
      {
        retval = type.GetMethod (identifier);
      }
    };
    visitor v (identifier);
    type->Accept (v);
    return v.retval;
  }

  Initializer*
  type_select_initializer (const Type* type, const std::string& identifier)
  {
    struct visitor : public DefaultVisitor
    {
      Initializer* retval;
      const std::string& identifier;
      visitor (const std::string& id) : retval (NULL), identifier (id) { }

      void visit (const NamedType& type)
      {
        retval = type.GetInitializer (identifier);
      }
    };
    visitor v (identifier);
    type->Accept (v);
    return v.retval;
  }

  Getter*
  type_select_getter (const Type* type, const std::string& identifier)
  {
    struct visitor : public DefaultVisitor
    {
      Getter* retval;
      const std::string& identifier;
      visitor (const std::string& id) : retval (NULL), identifier (id) { }

      void visit (const NamedType& type)
      {
        retval = type.GetGetter (identifier);
      }
    };
    visitor v (identifier);
    type->Accept (v);
    return v.retval;
  }

  reaction_t *
  type_select_reaction (const Type* type, const std::string& identifier)
  {
    struct visitor : public DefaultVisitor
    {
      reaction_t* retval;
      const std::string& identifier;
      visitor (const std::string& id) : retval (NULL), identifier (id) { }

      void visit (const NamedType& type)
      {
        retval = type.GetReaction (identifier);
      }
    };
    visitor v (identifier);
    type->Accept (v);
    return v.retval;
  }

  Action *
  type_select_action (const Type* type, const std::string& identifier)
  {
    struct visitor : public DefaultVisitor
    {
      Action* retval;
      const std::string& identifier;
      visitor (const std::string& id) : retval (NULL), identifier (id) { }

      void visit (const NamedType& type)
      {
        retval = type.GetAction (identifier);
      }
    };
    visitor v (identifier);
    type->Accept (v);
    return v.retval;
  }

  bind_t *
  type_select_bind (const Type* type, const std::string& identifier)
  {
    struct visitor : public DefaultVisitor
    {
      bind_t* retval;
      const std::string& identifier;
      visitor (const std::string& id) : retval (NULL), identifier (id) { }

      void visit (const NamedType& type)
      {
        retval = type.GetBind (identifier);
      }
    };
    visitor v (identifier);
    type->Accept (v);
    return v.retval;
  }

  const Type*
  type_select (const Type* type, const std::string& identifier)
  {
    field_t* f = type_select_field (type, identifier);
    if (f)
      {
        return f->type;
      }

    ::Method* m = type_select_method (type, identifier);
    if (m)
      {
        return m->methodType;
      }

    Initializer* i = type_select_initializer (type, identifier);
    if (i)
      {
        return i->initializerType;
      }

    Getter* g = type_select_getter (type, identifier);
    if (g)
      {
        return g->getterType;
      }

    reaction_t* r = type_select_reaction (type, identifier);
    if (r)
      {
        return r->reaction_type;
      }

    Action* a = type_select_action (type, identifier);
    if (a)
      {
        return Void::Instance ();
      }

    bind_t* b = type_select_bind (type, identifier);
    if (b)
      {
        return Void::Instance ();
      }

    return NULL;
  }

  ParameterSymbol*
  Signature::Find (const std::string& name) const
  {
    for (ParametersType::const_iterator ptr = parameters_.begin (),
         limit = parameters_.end ();
         ptr != limit;
         ++ptr)
      {
        if ((*ptr)->identifier == name)
          {
            return *ptr;
          }
      }
    return NULL;
  }

  const Type*
  type_move (const Type* type)
  {
    const Pointer* ptf = type_cast<Pointer> (type);
    if (ptf)
      {
        const Heap* h = type_cast<Heap> (ptf->Base ());
        if (h)
          {
            return ptf->Base ()->GetPointer ();
          }
      }

    return NULL;
  }

  const Type* type_merge (const Type* type)
  {
    const Pointer* ptf = type_cast<Pointer> (type);

    if (ptf)
      {
        const Heap* h = type_cast<Heap> (ptf->Base ());
        if (h)
          {
            return h->Base ()->GetPointer ();
          }
      }

    return NULL;
  }

  const Type*
  type_change (const Type* type)
  {
    const Pointer* ptf = type_cast<Pointer> (type);

    if (ptf)
      {
        const Heap* h = type_cast<Heap> (ptf->Base ());
        if (h)
          {
            return h->Base ()->GetPointer ();
          }
      }

    return NULL;
  }

#define ACCEPT(type) void \
type::Accept (Visitor& visitor) const \
{ \
  visitor.visit (*this); \
}

#define T_ACCEPT(type) template<> void type::Accept (Visitor& visitor) const { visitor.visit (*this); }

  T_ACCEPT(Enum)
  ACCEPT(NamedType)
  ACCEPT(Component)
  ACCEPT(Struct)
  T_ACCEPT(Bool)
  ACCEPT(Function)
  ACCEPT(Method)
  ACCEPT(Heap)
  ACCEPT(FileDescriptor)
  ACCEPT(Pointer)
  ACCEPT(Signature)
  T_ACCEPT(Int)
  T_ACCEPT(Int8)
  T_ACCEPT(Int16)
  T_ACCEPT(Int32)
  T_ACCEPT(Int64)
  T_ACCEPT(Uint)
  T_ACCEPT(Uint8)
  T_ACCEPT(Uint16)
  T_ACCEPT(Uint32)
  T_ACCEPT(Uint64)
  T_ACCEPT(Float32)
  T_ACCEPT(Float64)
  T_ACCEPT(Complex64)
  T_ACCEPT(Complex128)
  T_ACCEPT(StringU)
  T_ACCEPT(Uintptr)
  ACCEPT(Void)
  ACCEPT(Nil)
  ACCEPT(Boolean)
  ACCEPT(Rune)
  ACCEPT(Integer)
  ACCEPT(Float)
  ACCEPT(Complex)
  ACCEPT(String)
  ACCEPT(Array)
  ACCEPT(Slice)
  ACCEPT(Template)

  static bool
  structurally_equal (const Type* x, const Type* y)
  {
    struct visitor : public DefaultVisitor
    {
      bool flag;
      const Type* other;

      visitor (const Type* o) : flag (false), other (o) { }

      void visit (const Pointer& type)
      {
        const Pointer* t = type_cast<Pointer> (other);
        if (t != NULL)
          {
            flag = type_is_equal (type.Base (), t->Base ());
          }
      }

      void visit (const Heap& type)
      {
        const Heap* t = type_cast<Heap> (other);
        if (t != NULL)
          {
            flag = type_is_equal (type.Base (), t->Base ());
          }
      }

      void visit (const Bool& type)
      {
        flag = &type == other;
      }

      void visit (const Uint& type)
      {
        flag = &type == other;
      }

      void visit (const Uint8& type)
      {
        flag = &type == other;
      }

      void visit (const Uint16& type)
      {
        flag = &type == other;
      }

      void visit (const Uint32& type)
      {
        flag = &type == other;
      }

      void visit (const Uint64& type)
      {
        flag = &type == other;
      }

      void visit (const Int& type)
      {
        flag = &type == other;
      }

      void visit (const Int8& type)
      {
        flag = &type == other;
      }

      void visit (const Float64& type)
      {
        flag = &type == other;
      }

      void visit (const Nil& type)
      {
        flag = &type == other;
      }

      void visit (const Signature& type)
      {
        const Signature* x = &type;
        const Signature* y = type_cast<Signature> (other);
        if (y)
          {
            size_t x_arity = x->Arity ();
            size_t y_arity = y->Arity ();
            if (x_arity != y_arity)
              {
                return;
              }

            for (size_t idx = 0; idx != x_arity; ++idx)
              {
                unimplemented;
                // const parameter_t *x_parameter = x->At (idx);
                // const Type*x_parameter_type = x_parameter->value.type;
                // const parameter_t *y_parameter = y->At (idx);
                // const Type*y_parameter_type = y_parameter->value.type;

                // if (!type_is_equal (x_parameter_type, y_parameter_type))
                //   {
                //     return;
                //   }
              }

            flag = true;
          }
      }

      void visit (const Array& type)
      {
        const Array* x = &type;
        const Array* y = type_cast<Array> (other);
        if (y)
          {
            flag = x->dimension == y->dimension && type_is_equal (x->Base (), y->Base ());
          }
      }

      void visit (const Slice& type)
      {
        const Slice* x = &type;
        const Slice* y = type_cast<Slice> (other);
        if (y)
          {
            flag = type_is_equal (x->Base (), y->Base ());
          }
      }

      void visit (const Function& type)
      {
        const Function* x = &type;
        const Function* y = type_cast<Function> (other);
        if (y)
          {
            unimplemented;
            // flag = type_is_equal (x->GetSignature (), y->GetSignature ()) &&
            //        type_is_equal (x->GetReturnParameter ()->value.type, y->GetReturnParameter ()->value.type) &&
            //        x->GetReturnParameter ()->value.is_foreign_safe () && y->GetReturnParameter ()->value.is_foreign_safe ();
          }
      }
    };

    visitor v (y);
    x->Accept (v);
    return v.flag;
  }

// Returns true if two types are equal.
// If one type is a named type, then the other must be the same named type.
// Otherwise, the types must have the same structure.
  bool
  type_is_equal (const Type* x, const Type* y)
  {
    if (x == y)
      {
        return true;
      }

    if (type_cast<NamedType> (x) && type_cast<NamedType> (y))
      {
        // Named types must be exactly the same.
        return false;
      }

    return structurally_equal (type_strip (x), type_strip (y));
  }

  struct IdenticalImpl
  {
    bool retval;

    IdenticalImpl () : retval (false) { }

    void operator() (const Array& type1, const Array& type2)
    {
      unimplemented;
    }

    void operator() (const Slice& type1, const Slice& type2)
    {
      retval = Identical (type1.Base (), type2.Base ());
    }

    void operator() (const Struct& type1, const Struct& type2)
    {
      unimplemented;
    }

    void operator() (const Pointer& type1, const Pointer& type2)
    {
      unimplemented;
    }

    void operator() (const Function& type1, const Function& type2)
    {
      unimplemented;
    }

    // TODO:  Interfaces
    // void operator() (const Function& type1, const Function& type2) {
    //   unimplemented;
    // }

    // TODO:  Maps
    // void operator() (const Function& type1, const Function& type2) {
    //   unimplemented;
    // }

    template <typename T1, typename T2>
    void operator() (const T1& type1, const T2& type2)
    {
      retval = static_cast<const Type*> (&type1) == static_cast<const Type*> (&type2);
    }
  };

  bool
  Identical (const Type* x, const Type* y)
  {
    if (x == y)
      {
        return true;
      }

    if (type_cast<NamedType> (x) != NULL ||
        type_cast<NamedType> (y) != NULL)
      {
        return x == y;
      }

    IdenticalImpl i;
    DoubleDispatch (x, y, i);
    return i.retval;
  }

  std::string
  Signature::ToString () const
  {
    std::stringstream str;
    str << '(';
    bool flag = false;
    for (ParametersType::const_iterator ptr = parameters_.begin (),
         limit = parameters_.end ();
         ptr != limit;
         ++ptr)
      {
        if (flag)
          {
            str << ", ";
          }

        str << (*ptr)->type->ToString ();
        flag = true;
      }
    str << ')';
    return str.str ();
  }

#define INSTANCE(type) const type* \
type::Instance () \
{ \
  static type* i = new type ();                 \
  return i; \
}

  INSTANCE(Void)
  INSTANCE(FileDescriptor)
  INSTANCE(Nil)
  INSTANCE(Boolean)
  INSTANCE(Rune)
  INSTANCE(Integer)
  INSTANCE(Float)
  INSTANCE(Complex)
  INSTANCE(String)

  Struct::Struct (bool insert_runtime) : offset_ (0), alignment_ (0)
  {
    if (insert_runtime)
      {
        /* Prepend the field list with a pointer for the runtime. */
        Append ("0runtime", Void::Instance ()->GetPointer ());
      }
  }

  const Type* type_dereference (const Type* type)
  {
    struct visitor : public DefaultVisitor
    {
      const Type* retval;

      visitor () : retval (NULL) { }

      void visit (const Pointer& type)
      {
        retval = type.Base ();
      }
    };
    visitor v;
    type->Accept (v);
    return v.retval;
  }

  bool
  type_contains_pointer (const Type* type)
  {
    struct visitor : public DefaultVisitor
    {
      bool flag;

      visitor () : flag (false) { }

      void default_action (const Type& type)
      {
        type_not_reached (type);
      }

      void visit (const NamedType& type)
      {
        type.UnderlyingType ()->Accept (*this);
      }

      void visit (const Void& type)
      { }

      void visit (const Bool& type)
      { }

      void visit (const Int& type)
      { }

      void visit (const Int8& type)
      { }

      void visit (const Int16& type)
      { }

      void visit (const Int32& type)
      { }

      void visit (const Int64& type)
      { }

      void visit (const Uint& type)
      { }

      void visit (const Uint8& type)
      { }

      void visit (const Uint16& type)
      { }

      void visit (const Uint32& type)
      { }

      void visit (const Uint64& type)
      { }

      void visit (const Float32& type)
      { }

      void visit (const Float64& type)
      { }

      void visit (const Complex64& type)
      { }

      void visit (const Complex128& type)
      { }

      void visit (const Enum& type)
      { }

      void visit (const Array& type)
      {
        type.Base ()->Accept (*this);
      }

      void visit (const Pointer& type)
      {
        flag = true;
      }

      void visit (const Slice& type)
      {
        flag = true;
      }

      void visit (const StringU& type)
      {
        flag = true;
      }

      void visit (const FileDescriptor& type)
      {
        flag = true;
      }

      void visit (const Function& type)
      {
        // Do nothing.
      }

      void visit (const Struct& type)
      {
        for (Struct::const_iterator pos = type.Begin (), limit = type.End ();
             pos != limit;
             ++pos)
          {
            (*pos)->type->Accept (*this);
          }
      }

      void visit (const Boolean& type) { }
      void visit (const Integer& type) { }
      void visit (const Float& type) { }
      void visit (const Complex& type) { }
      void visit (const String& type)
      {
        flag = true;
      }
    };
    visitor v;
    type->Accept (v);
    return v.flag;
  }

  NamedType::NamedType (const std::string& name,
                        const Type* subtype)
    : name_ (name)
    , underlyingType_ (subtype->UnderlyingType ())
  {
    assert (underlyingType_->Level () == UNNAMED);
  }

  bool
  type_is_boolean (const Type* type)
  {
    struct visitor : public DefaultVisitor
    {
      bool flag;
      visitor () : flag (false) { }

      void visit (const NamedType& type)
      {
        type.UnderlyingType ()->Accept (*this);
      }

      void visit (const Bool& type)
      {
        flag = true;
      }

      void visit (const Boolean& type)
      {
        flag = true;
      }
    };
    visitor v;
    type->Accept (v);
    return v.flag;
  }

  bool
  type_is_integral (const Type* type)
  {
    struct visitor : public DefaultVisitor
    {
      bool flag;
      visitor () : flag (false) { }

      void visit (const NamedType& type)
      {
        type.UnderlyingType ()->Accept (*this);
      }

      void visit (const Int& type)
      {
        flag = true;
      }

      void visit (const Int8& type)
      {
        flag = true;
      }

      void visit (const Int64& type)
      {
        flag = true;
      }

      void visit (const Uint& type)
      {
        flag = true;
      }

      void visit (const Uint8& type)
      {
        flag = true;
      }

      void visit (const Uint32& type)
      {
        flag = true;
      }

      void visit (const Uint64& type)
      {
        flag = true;
      }

      void visit (const Integer& type)
      {
        flag = true;
      }
    };
    visitor v;
    type->Accept (v);
    return v.flag;
  }

  bool
  type_is_unsigned_integral (const Type* type)
  {
    struct visitor : public DefaultVisitor
    {
      bool flag;
      visitor () : flag (false) { }

      void visit (const NamedType& type)
      {
        type.UnderlyingType ()->Accept (*this);
      }

      void visit (const Uint& type)
      {
        flag = true;
      }

      void visit (const Uint8& type)
      {
        flag = true;
      }
    };
    visitor v;
    type->Accept (v);
    return v.flag;
  }

  bool
  type_is_floating (const Type* type)
  {
    struct visitor : public DefaultVisitor
    {
      bool flag;
      visitor () : flag (false) { }

      void visit (const NamedType& type)
      {
        type.UnderlyingType ()->Accept (*this);
      }

      void visit (const Float64& type)
      {
        flag = true;
      }
    };
    visitor v;
    type->Accept (v);
    return v.flag;
  }

  bool
  type_is_comparable (const Type* type)
  {
    struct visitor : public DefaultVisitor
    {
      bool flag;
      visitor () : flag (false) { }

      void visit (const Pointer& type)
      {
        flag = true;
      }

      void visit (const NamedType& type)
      {
        type.UnderlyingType
        ()->Accept (*this);
      }

      void visit (const Bool& type)
      {
        flag = true;
      }

      void visit (const Int& type)
      {
        flag = true;
      }

      void visit (const Int8& type)
      {
        flag = true;
      }

      void visit (const Int64& type)
      {
        flag = true;
      }

      void visit (const Uint& type)
      {
        flag = true;
      }

      void visit (const Uint8& type)
      {
        flag = true;
      }

      void visit (const Uint32& type)
      {
        flag = true;
      }

      void visit (const Uint64& type)
      {
        flag = true;
      }

      void visit (const Enum& type)
      {
        flag = true;
      }
    };
    visitor v;
    type->Accept (v);
    return v.flag;
  }

  bool
  type_is_orderable (const Type* type)
  {
    struct visitor : public DefaultVisitor
    {
      bool flag;
      visitor () : flag (false) { }

      void visit (const Pointer& type)
      {
        flag = true;
      }

      void visit (const NamedType& type)
      {
        type.UnderlyingType ()->Accept (*this);
      }

      void visit (const Int& type)
      {
        flag = true;
      }

      void visit (const Int8& type)
      {
        flag = true;
      }

      void visit (const Uint& type)
      {
        flag = true;
      }

      void visit (const Enum& type)
      {
        flag = true;
      }
    };
    visitor v;
    type->Accept (v);
    return v.flag;
  }

  bool
  type_is_pointer_compare (const Type* left, const Type* right)
  {
    const Type* l = type_strip (left);
    const Type* r = type_strip (right);

    return
      (type_cast<Pointer> (l) && type_cast<Nil>(r)) ||
      (type_cast<Nil> (l) && type_cast<Pointer> (r));

  }

  const Type*
  type_strip (const Type* type)
  {
    struct visitor : public DefaultVisitor
    {
      const Type* retval;
      visitor (const Type* t) : retval (t) { }

      void visit (const NamedType& type)
      {
        retval = type.UnderlyingType ();
      }
    };
    visitor v (type);
    type->Accept (v);
    if (type == v.retval)
      {
        return type;
      }
    else
      {
        return type_strip (v.retval);
      }
  }

  const Type*
  type_index (const Type* base, const Type* index)
  {
    struct visitor : public DefaultVisitor
    {
      const Type* index;
      const Type* result;

      visitor (const Type* i) : index (i), result (NULL) { }

      void visit (const Array& type)
      {
        struct visitor : public DefaultVisitor
        {
          const Array& array_type;
          const Type* result;

          visitor (const Array& at) : array_type (at), result (NULL) { }

          void visit (const NamedType& type)
          {
            type.UnderlyingType ()->Accept (*this);
          }

          void visit (const Uint& type)
          {
            result = array_type.Base ();
          }

          void visit (const Int& type)
          {
            result = array_type.Base ();
          }
        };
        visitor v (type);
        index->Accept (v);
        result = v.result;
      }

    };
    visitor v (index);
    base->Accept (v);
    return v.result;
  }

  bool
  type_is_index (const Type* type, Int::ValueType index)
  {
    struct visitor : public DefaultVisitor
    {
      Int::ValueType index;
      bool flag;

      visitor (Int::ValueType i) : index (i), flag (false) { }

      void visit (const Array& type)
      {
        flag = index >= 0 && index < type.dimension;
      }
    };
    visitor v (index);
    type->Accept (v);
    return v.flag;
  }

  bool
  type_is_castable (const Type* x, const Type* y)
  {
    return type_is_numeric (x) && type_is_numeric (y);
  }

  std::string
  Function::ToString () const
  {
    std::stringstream str;
    switch (kind)
      {
      case FUNCTION:
        str << "func " << *GetSignature () << ' ' << *GetReturnParameter ()->type;
        break;
      case PUSH_PORT:
        str << "push " << *GetSignature ();
        break;
      case PULL_PORT:
        str << "pull " << *GetSignature () << ' ' << *GetReturnParameter ()->type;
        break;
      }
    return str.str ();
  }

  std::string
  Method::ToString () const
  {
    std::stringstream str;
    switch (kind)
      {
      case METHOD:
        str << '(' << *receiver_type << ')' << " func " << *signature << ' ' << *return_type ();
        break;
      case INITIALIZER:
        str << '(' << *receiver_type << ')' << " init " << *signature << ' ' << *return_type ();
        break;
      case GETTER:
        str << '(' << *receiver_type << ')' << " getter " << *signature << ' ' << *return_type ();
        break;
      case REACTION:
        str << '(' << *receiver_type << ')' << " reaction " << *signature;
        break;
      }
    return str.str ();
  }

  Function*
  Method::make_function_type (ParameterSymbol* this_parameter,
                              const Signature* signature,
                              ParameterSymbol* return_parameter)
  {
    Signature* sig = new Signature ();

    sig->Append (this_parameter);
    for (Signature::const_iterator pos = signature->Begin (),
         limit = signature->End ();
         pos != limit;
         ++pos)
      {
        sig->Append (*pos);
      }
    return new Function (Function::FUNCTION, sig, return_parameter);
  }

  const Type*
  Function::GetReturnType () const
  {
    return return_parameter_->type;
  }

  const Type*
  Method::return_type () const
  {
    return return_parameter->type;
  }

  Method::Method (Kind k, const NamedType* named_type_,
                  ParameterSymbol* this_parameter_,
                  const Signature * signature_,
                  ParameterSymbol* return_parameter_)
    : kind (k), named_type (named_type_)
    , receiver_type (NULL /*this_parameter_->value.type*/)
    , this_parameter (this_parameter_)
    , function_type (make_function_type (this_parameter_, signature_, return_parameter_))
    , signature (signature_)
    , return_parameter (return_parameter_)
  {
  }

  const Type*
  Boolean::DefaultType () const
  {
    return &NamedBool;
  }

  const Type*
  Rune::DefaultType () const
  {
    return &NamedRune;
  }

  const Type*
  Integer::DefaultType () const
  {
    return &NamedInt;
  }

  const Type*
  Float::DefaultType () const
  {
    return &NamedFloat64;
  }

  const Type*
  Complex::DefaultType () const
  {
    return &NamedComplex128;
  }

  const Type*
  String::DefaultType () const
  {
    return &NamedString;
  }

  const Type*
  Choose (const Type* x, const Type* y)
  {
    return (x->Level () > y->Level ()) ? x : y;
  }

  bool
  assignable (const Type*& from, value_t& from_value, const Type* to)
  {
    if (Identical (from, to))
      {
        return true;
      }

    if (Identical (from->UnderlyingType (), to->UnderlyingType ()) &&
        (from->Level () != Type::NAMED || to->Level () != Type::NAMED))
      {
        return true;
      }

    // TODO:  T is an interface type and x implements T.

    if (type_cast<Nil> (from) &&
        (type_cast<Pointer> (to->UnderlyingType ()) ||
         type_cast<Function> (to->UnderlyingType ()) ||
         type_cast<Slice> (to->UnderlyingType ())
         //type_cast<Map> (to->UnderlyingType ()) ||
         //type_cast<Interface> (to->UnderlyingType ())
        ))
      {
        return true;
      }

    if (from->IsUntyped () && from_value.representable (from, to))
      {
        from_value.convert (from, to);
        from = to;
        return true;
      }

    return false;
  }

  Signature*
  Signature::Append (ParameterSymbol* p)
  {
    parameters_.push_back (p);
    size_ += util::AlignUp (p->type->Size (), MemoryModel::StackAlignment);
    return this;
  }

  void
  Signature::check_foreign_safe () const
  {
    for (const_iterator pos = Begin (), limit = End ();
         pos != limit;
         ++pos)
      {
        (*pos)->check_foreign_safe ();
      }
  }

  field_t*
  Struct::select_field (const std::string& name) const
  {
    return Find (name);
  }

  Callable*
  NamedType::select_callable (const std::string& name) const
  {
    ::Method* m = type_select_method (this, name);
    if (m)
      {
        return m;
      }

    Initializer* i = type_select_initializer (this, name);
    if (i)
      {
        return i;
      }

    Getter* g = type_select_getter (this, name);
    if (g)
      {
        return g;
      }

    reaction_t* r = type_select_reaction (this, name);
    if (r)
      {
        return r;
      }

    return NULL;
  }

  NamedType NamedBool ("bool", Bool::Instance ());

  NamedType NamedUint8 ("uint8", Uint8::Instance ());
  NamedType NamedUint16 ("uint16", Uint16::Instance ());
  NamedType NamedUint32 ("uint32", Uint32::Instance ());
  NamedType NamedUint64 ("uint64", Uint64::Instance ());

  NamedType NamedInt8 ("int8", Int8::Instance ());
  NamedType NamedInt16 ("int16", Int16::Instance ());
  NamedType NamedInt32 ("int32", Int32::Instance ());
  NamedType NamedInt64 ("int64", Int64::Instance ());

  NamedType NamedFloat32 ("float32", Float32::Instance ());
  NamedType NamedFloat64 ("float64", Float64::Instance ());

  NamedType NamedComplex64 ("complex64", Complex64::Instance ());
  NamedType NamedComplex128 ("complex128", Complex128::Instance ());

  NamedType NamedByte ("byte", Uint8::Instance ());
  NamedType NamedRune ("rune", Int::Instance ());

  NamedType NamedUint ("uint", Uint::Instance ());
  NamedType NamedInt ("int", Int::Instance ());
  NamedType NamedUintptr ("uintptr", Uintptr::Instance ());

  NamedType NamedString ("string", StringU::Instance ());

  NamedType NamedFileDescriptor ("FileDescriptor", FileDescriptor::Instance ());
}
