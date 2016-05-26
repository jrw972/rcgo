#include "type.hpp"

#include "action.hpp"
#include "reaction.hpp"
#include "field.hpp"
#include "callable.hpp"
#include "bind.hpp"
#include "arch.hpp"
#include "parameter_list.hpp"

namespace type
{
using namespace decl;
using namespace semantic;

Type::Type ()
  : pointer_ (NULL)
  , slice_ (NULL)
  , heap_ (NULL)
{ }

Type::~Type () { }

std::string Type::to_string () const
{
  std::stringstream str;
  print (str);
  return str.str ();
}

bool Type::is_untyped () const
{
  return kind () >= Nil_Kind && kind () <= Untyped_String_Kind;
}

bool Type::is_unnamed () const
{
  return kind () >= Void_Kind && kind () <= File_Descriptor_Kind;
}

bool Type::is_named () const
{
  return kind () == Named_Kind;
}


std::ostream&
operator<< (std::ostream& o, const Type& type)
{
  type.print (o);
  return o;
}

const Pointer*
Type::get_pointer () const
{
  if (pointer_ == NULL)
    {
      const_cast<Type*> (this)->pointer_ = new Pointer (this);
    }
  return pointer_;
}

const Slice*
Type::get_slice () const
{
  if (slice_ == NULL)
    {
      const_cast<Type*> (this)->slice_ = new Slice (this);
    }
  return slice_;
}

const Array*
Type::get_array (IntValueType dimension) const
{
  assert (dimension >= 0);
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
Type::get_heap () const
{
  if (heap_ == NULL)
    {
      const_cast<Type*> (this)->heap_ = new Heap (this);
    }
  return heap_;
}

void
Array::print (std::ostream& out) const
{
  out << '[' << dimension << ']' << *base_type;
}


Reaction *
NamedType::get_reaction (const std::string& identifier) const
{
  for (std::vector<Reaction*>::const_iterator pos = reactions_.begin (),
       limit = reactions_.end ();
       pos != limit;
       ++pos)
    {
      Reaction *a = *pos;
      if (a->name == identifier)
        {
          return a;
        }
    }

  return NULL;
}

decl::Method*
NamedType::get_method (const std::string& identifier) const
{
  for (std::vector< decl::Method*>::const_iterator pos = this->methods_.begin (),
       limit = this->methods_.end ();
       pos != limit;
       ++pos)
    {
      decl::Method* a = *pos;
      if (a->name == identifier)
        {
          return a;
        }
    }

  return NULL;
}

Initializer*
NamedType::get_initializer (const std::string& identifier) const
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
NamedType::get_getter (const std::string& identifier) const
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
NamedType::get_action (const std::string& identifier) const
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

Bind*
NamedType::get_bind (const std::string& identifier) const
{
  for (BindsType::const_iterator pos = this->binds_.begin (),
       limit = this->binds_.end ();
       pos != limit;
       ++pos)
    {
      Bind* a = *pos;
      if (a->name == identifier)
        {
          return a;
        }
    }

  return NULL;
}

void
Struct::print (std::ostream& out) const
{
  out << '{';
  for (FieldsType::const_iterator pos = fields_.begin (), limit = fields_.end ();
       pos != limit;
       ++pos)
    {
      Field* f = *pos;
      out << f->name << ' ' << *(f->type) << ';';
    }
  out << '}';
}

Struct*
Struct::append_field (Package* package, bool is_anonymous, const std::string& field_name, const Type* field_type, const TagSet& tags)
{
  size_t alignment = field_type->alignment ();
  offset_ = util::align_up (offset_, alignment);

  Field *field = new Field (package, is_anonymous, field_name, field_type, tags, offset_);
  fields_.push_back (field);

  offset_ += field_type->Size ();
  if (alignment > alignment_)
    {
      alignment_ = alignment;
    }

  return this;
}

Field *
Struct::get_field_i (const std::string& name) const
{
  for (std::vector<Field*>::const_iterator field = fields_.begin (),
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

const Type*
Type::select (const std::string& identifier) const
{
  Field* f = this->get_field (identifier);
  if (f)
    {
      return f->type;
    }

  decl::Method* m = this->get_method (identifier);
  if (m)
    {
      return m->methodType;
    }

  Initializer* i = this->get_initializer (identifier);
  if (i)
    {
      return i->initializerType;
    }

  Getter* g = this->get_getter (identifier);
  if (g)
    {
      return g->getterType;
    }

  Reaction* r = this->get_reaction (identifier);
  if (r)
    {
      return r->reaction_type;
    }

  Action* a = this->get_action (identifier);
  if (a)
    {
      return Void::Instance ();
    }

  Bind* b = this->get_bind (identifier);
  if (b)
    {
      return Void::Instance ();
    }

  return NULL;
}

const Type*
Type::move () const
{
  const Pointer* ptf = type_cast<Pointer> (this);
  if (ptf)
    {
      const Heap* h = type_cast<Heap> (ptf->base_type);
      if (h)
        {
          return ptf;
        }
    }

  return NULL;
}

const Type*
Type::merge_change () const
{
  const Pointer* ptf = type_cast<Pointer> (this);

  if (ptf)
    {
      const Heap* h = type_cast<Heap> (ptf->base_type);
      if (h)
        {
          return h->base_type->get_pointer ();
        }
    }

  return NULL;
}

#define ACCEPT(type) void \
type::accept (Visitor& visitor) const \
{ \
  visitor.visit (*this); \
}

#define T_ACCEPT(type) template<> void type::accept (Visitor& visitor) const { visitor.visit (*this); }

ACCEPT(Array)
ACCEPT(Boolean)
ACCEPT(Complex)
ACCEPT(Component)
ACCEPT(FileDescriptor)
ACCEPT(Float)
ACCEPT(Function)
ACCEPT(Heap)
ACCEPT(Integer)
ACCEPT(Interface)
ACCEPT(Map)
ACCEPT(Method)
ACCEPT(NamedType)
ACCEPT(Nil)
ACCEPT(Pointer)
ACCEPT(Rune)
ACCEPT(Slice)
ACCEPT(String)
ACCEPT(Struct)
ACCEPT(Template)
ACCEPT(Void)
T_ACCEPT(Bool)
T_ACCEPT(Complex128)
T_ACCEPT(Complex64)
T_ACCEPT(Float32)
T_ACCEPT(Float64)
T_ACCEPT(Int)
T_ACCEPT(Int16)
T_ACCEPT(Int32)
T_ACCEPT(Int64)
T_ACCEPT(Int8)
T_ACCEPT(StringU)
T_ACCEPT(Uint)
T_ACCEPT(Uint16)
T_ACCEPT(Uint32)
T_ACCEPT(Uint64)
T_ACCEPT(Uint8)
T_ACCEPT(Uintptr)

bool
are_identical (const Type* x, const Type* y)
{
  if ((x == &named_uint8 && y == &named_byte) ||
      (x == &named_byte &&  y == &named_uint8))
    {
      return true;
    }

  if (x == y)
    {
      return true;
    }

  if (x->to_named_type () ||
      y->to_named_type ())
    {
      return false;
    }

  if (x->kind () != y->kind ())
    {
      return false;
    }

  switch (x->kind ())
    {
    case Array_Kind:
    {
      const Array* type1 = x->to_array ();
      const Array* type2 = y->to_array ();
      return are_identical (type1->base_type, type2->base_type) && type1->dimension == type2->dimension;
    }
    case Slice_Kind:
    {
      const Slice* type1 = x->to_slice ();
      const Slice* type2 = y->to_slice ();
      return are_identical (type1->base_type, type2->base_type);
    }
    case Struct_Kind:
    case Component_Kind:
    {
      const Struct* type1 = x->to_struct ();
      const Struct* type2 = y->to_struct ();

      if (type1->field_count () != type2->field_count ())
        {
          return false;
        }

      Struct::const_iterator pos1 = type1->Begin ();
      Struct::const_iterator limit1 = type1->End ();
      Struct::const_iterator pos2 = type2->Begin ();
      Struct::const_iterator limit2 = type2->End ();

      for (; pos1 != limit1 && pos2 != limit2; ++pos1, ++pos2)
        {
          Field* f1 = *pos1;
          Field* f2 = *pos2;
          if (f1->is_anonymous != f2->is_anonymous)
            {
              return false;
            }
          if (!f1->is_anonymous && f1->name != f2->name)
            {
              return false;
            }
          if (!f1->is_anonymous && util::is_lowercase (f1->name) && f1->package != f2->package)
            {
              return false;
            }
          if (!are_identical (f1->type, f2->type))
            {
              return false;
            }
          if (f1->tags != f2->tags)
            {
              return false;
            }
        }

      return true;
    }
    case Pointer_Kind:
    {
      const Pointer* type1 = x->to_pointer ();
      const Pointer* type2 = y->to_pointer ();
      return are_identical (type1->base_type, type2->base_type);
    }
    case Function_Kind:
    {
      const Function* type1 = x->to_function ();
      const Function* type2 = y->to_function ();

      if (type1->function_kind != type2->function_kind)
        {
          return false;
        }

      if (type1->parameter_list->size () != type2->parameter_list->size ())
        {
          return false;
        }

      if (type1->return_parameter_list->size () != type2->return_parameter_list->size ())
        {
          return false;
        }

      if (type1->return_parameter_list->is_variadic () != type2->return_parameter_list->is_variadic ())
        {
          return false;
        }

      for (ParameterList::const_iterator pos1 = type1->parameter_list->begin (),
           limit1 = type1->parameter_list->end (),
           pos2 = type2->parameter_list->begin (),
           limit2 = type2->parameter_list->end ();
           pos1 != limit1 && pos2 != limit2;
           ++pos1, ++pos2)
        {
          if (!are_identical ((*pos1)->type, (*pos2)->type))
            {
              return false;
            }
        }

      for (ParameterList::const_iterator pos1 = type1->return_parameter_list->begin (),
           limit1 = type1->return_parameter_list->end (),
           pos2 = type2->return_parameter_list->begin (),
           limit2 = type2->return_parameter_list->end ();
           pos1 != limit1 && pos2 != limit2;
           ++pos1, ++pos2)
        {
          if (!are_identical ((*pos1)->type, (*pos2)->type))
            {
              return false;
            }
        }

      return true;
    }
    case Interface_Kind:
    {
      const Interface* type1 = x->to_interface ();
      const Interface* type2 = y->to_interface ();

      if (type1->methods.size () != type2->methods.size ())
        {
          return false;
        }

      for (Interface::MethodsType::const_iterator pos1 = type1->methods.begin (), limit1 = type1->methods.end (),
           pos2 = type2->methods.begin (), limit2 = type2->methods.end ();
           pos1 != limit1 && pos2 != limit2;
           ++pos1, ++pos2)
        {
          if (pos1->first != pos2->first)
            {
              return false;
            }

          if (util::is_lowercase (pos1->first) && type1->package != type2->package)
            {
              return false;
            }

          if (!are_identical (pos1->second, pos2->second))
            {
              return false;
            }
        }

      return true;
    }
    case Map_Kind:
    {
      const Map* type1 = x->to_map ();
      const Map* type2 = y->to_map ();
      return are_identical (type1->key_type, type2->key_type) && are_identical (type1->value_type, type2->value_type);
    }

    case Heap_Kind:
    {
      const Heap* type1 = x->to_heap ();
      const Heap* type2 = y->to_heap ();
      return are_identical (type1->base_type, type2->base_type);
    }

    default:
      break;
    }

  NOT_REACHED;
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
INSTANCE(Template)

Struct::Struct () : offset_ (0), alignment_ (0)
{
}

Component::Component (Package* package)
  : Struct ()
{
  /* Prepend the field list with a pointer for the runtime. */
  append_field (package, true, "0runtime", Void::Instance ()->get_pointer (), TagSet ());
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
      TYPE_NOT_REACHED (type);
    }

    void visit (const NamedType& type)
    {
      type.UnderlyingType ()->accept (*this);
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

    void visit (const Array& type)
    {
      type.base_type->accept (*this);
    }

    void visit (const Pointer& type)
    {
      flag = true;
    }

    void visit (const Heap& type)
    {
      type.base_type->accept (*this);
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
          (*pos)->type->accept (*this);
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
  type->accept (v);
  return v.flag;
}

NamedType::NamedType (const std::string& name,
                      const Type* subtype)
  : name_ (name)
  , underlyingType_ (subtype->UnderlyingType ())
{
  assert (underlyingType_->is_unnamed ());
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
      type.UnderlyingType ()->accept (*this);
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
  type->accept (v);
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
      type.UnderlyingType ()->accept (*this);
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
  type->accept (v);
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
      type.UnderlyingType ()->accept (*this);
    }

    void visit (const Float64& type)
    {
      flag = true;
    }
  };
  visitor v;
  type->accept (v);
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
      type.UnderlyingType ()->accept (*this);
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
  };
  visitor v;
  type->accept (v);
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
  type->accept (v);
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
          type.UnderlyingType ()->accept (*this);
        }

        void visit (const Uint& type)
        {
          result = array_type.base_type;
        }

        void visit (const Int& type)
        {
          result = array_type.base_type;
        }
      };
      visitor v (type);
      index->accept (v);
      result = v.result;
    }

  };
  visitor v (index);
  base->accept (v);
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
  type->accept (v);
  return v.flag;
}

bool
type_is_castable (const Type* x, const Type* y)
{
  return type_is_numeric (x) && type_is_numeric (y);
}

void
Function::print (std::ostream& out) const
{
  switch (function_kind)
    {
    case FUNCTION:
      out << "func " << *parameter_list << ' ' << *GetReturnParameter ()->type;
      break;
    case PUSH_PORT:
      out << "push " << *parameter_list;
      break;
    case PULL_PORT:
      out << "pull " << *parameter_list << ' ' << *GetReturnParameter ()->type;
      break;
    }
}

void
Method::print (std::ostream& out) const
{
  switch (method_kind)
    {
    case METHOD:
      out << '(' << *receiver_type () << ')' << " func " << *parameter_list << ' ' << *return_type ();
      break;
    case INITIALIZER:
      out << '(' << *receiver_type () << ')' << " init " << *parameter_list << ' ' << *return_type ();
      break;
    case GETTER:
      out << '(' << *receiver_type () << ')' << " getter " << *parameter_list << ' ' << *return_type ();
      break;
    case REACTION:
      out << '(' << *receiver_type () << ')' << " reaction " << *parameter_list;
      break;
    }
}

Function*
Method::make_function_type (ParameterSymbol* this_parameter,
                            const ParameterList* parameter_list,
                            const ParameterList* return_parameter_list)
{
  ParameterList* sig = new ParameterList (parameter_list->location);

  sig->append (this_parameter);
  for (ParameterList::const_iterator pos = parameter_list->begin (),
       limit = parameter_list->end ();
       pos != limit;
       ++pos)
    {
      sig->append (*pos);
    }
  return new Function (Function::FUNCTION, sig, return_parameter_list);
}

const Type*
Function::GetReturnType () const
{
  return return_parameter_list->at (0)->type;
}

const Type*
Method::receiver_type () const
{
  return receiver_parameter->type;
}

const Type*
Method::return_type () const
{
  return return_parameter_list->at (0)->type;
}

Method::Method (MethodKind k, const NamedType* named_type_,
                ParameterSymbol* receiver_parameter_,
                const ParameterList* a_parameter_list,
                const ParameterList* a_return_parameter_list)
  : method_kind (k)
  , named_type (named_type_)
  , receiver_parameter (receiver_parameter_)
  , function_type (make_function_type (receiver_parameter_, a_parameter_list, a_return_parameter_list))
  , parameter_list (a_parameter_list)
  , return_parameter_list (a_return_parameter_list)
{
}

const Type*
Boolean::DefaultType () const
{
  return &named_bool;
}

const Type*
Rune::DefaultType () const
{
  return &named_rune;
}

const Type*
Integer::DefaultType () const
{
  return &named_int;
}

const Type*
Float::DefaultType () const
{
  return &named_float64;
}

const Type*
Complex::DefaultType () const
{
  return &named_complex128;
}

const Type*
String::DefaultType () const
{
  return &named_string;
}

const Type*
Choose (const Type* x, const Type* y)
{
  return (x->kind () > y->kind ()) ? x : y;
}

bool
assignable (const Type* from, const Value& from_value, const Type* to)
{
  if (to->is_untyped ())
    {
      return false;
    }

  if (are_identical (from, to))
    {
      return true;
    }

  if (are_identical (from->UnderlyingType (), to->UnderlyingType ()) &&
      (!from->is_named () || !to->is_named ()))
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

  if (from->is_untyped () && from_value.representable (from, to))
    {
      return true;
    }

  return false;
}

Field*
Struct::select_field (const std::string& name) const
{
  return get_field (name);
}

Callable*
NamedType::select_callable (const std::string& name) const
{
  decl::Method* m = this->get_method (name);
  if (m)
    {
      return m;
    }

  Initializer* i = this->get_initializer (name);
  if (i)
    {
      return i;
    }

  Getter* g = this->get_getter (name);
  if (g)
    {
      return g;
    }

  Reaction* r = this->get_reaction (name);
  if (r)
    {
      return r;
    }

  return NULL;
}

bool is_typed_integer (const Type* type)
{
  struct visitor : DefaultVisitor
  {
    bool flag;
    visitor () : flag (false) { }
    void visit (const Uint8& type)
    {
      flag = true;
    }
    void visit (const Uint16& type)
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
    void visit (const Int8& type)
    {
      flag = true;
    }
    void visit (const Int16& type)
    {
      flag = true;
    }
    void visit (const Int32& type)
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
    void visit (const Int& type)
    {
      flag = true;
    }
    void visit (const Uintptr& type)
    {
      flag = true;
    }
  };
  visitor v;
  type->UnderlyingType ()->accept (v);
  return v.flag;
}

bool is_typed_unsigned_integer (const Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
      return true;
    default:
      return false;
    }
}

bool is_typed_float (const Type* type)
{
  switch (type->underlying_kind ())
    {
    case Float32_Kind:
    case Float64_Kind:
      return true;
    default:
      return false;
    }
}

bool is_typed_complex (const Type* type)
{
  switch (type->underlying_kind ())
    {
    case Complex64_Kind:
    case Complex128_Kind:
      return true;
    default:
      return false;
    }
}

bool is_typed_numeric (const Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
    case Int8_Kind:
    case Int16_Kind:
    case Int32_Kind:
    case Int64_Kind:
    case Float32_Kind:
    case Float64_Kind:
    case Complex64_Kind:
    case Complex128_Kind:
    case Uint_Kind:
    case Int_Kind:
    case Uintptr_Kind:
      return true;
    default:
      return false;
    }
}

bool is_untyped_numeric (const Type* type)
{
  switch (type->underlying_kind ())
    {
    case Rune_Kind:
    case Integer_Kind:
    case Float_Kind:
    case Complex_Kind:
      return true;
    default:
      return false;
    }
}

bool is_any_numeric (const Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
    case Int8_Kind:
    case Int16_Kind:
    case Int32_Kind:
    case Int64_Kind:
    case Float32_Kind:
    case Float64_Kind:
    case Complex64_Kind:
    case Complex128_Kind:
    case Uint_Kind:
    case Int_Kind:
    case Uintptr_Kind:
    case Rune_Kind:
    case Integer_Kind:
    case Float_Kind:
    case Complex_Kind:
      return true;
    default:
      return false;
    }
}

bool is_typed_string (const Type* type)
{
  return type->underlying_kind () == String_Kind;
}

bool is_untyped_string (const Type* type)
{
  return type->underlying_kind () == Untyped_String_Kind;
}

bool is_any_string (const Type* type)
{
  return is_typed_string (type) || is_untyped_string (type);
}

bool is_slice_of_bytes (const Type* type)
{
  const Slice* slice_type = type_cast<Slice> (type->UnderlyingType ());
  if (slice_type == NULL)
    {
      return false;
    }
  return type_cast<Uint8> (slice_type->base_type->UnderlyingType ()) != NULL;
}

bool is_slice_of_runes (const Type* type)
{
  const Slice* slice_type = type_cast<Slice> (type->UnderlyingType ());
  if (slice_type == NULL)
    {
      return false;
    }
  return type_cast<Int32> (slice_type->base_type->UnderlyingType ()) != NULL;
}

bool orderable (const Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
    case Int8_Kind:
    case Int16_Kind:
    case Int32_Kind:
    case Int64_Kind:
    case Float32_Kind:
    case Float64_Kind:
    case Uint_Kind:
    case Int_Kind:
    case Uintptr_Kind:
    case String_Kind:
    case Rune_Kind:
    case Integer_Kind:
    case Float_Kind:
    case Untyped_String_Kind:
      return true;
    default:
      return false;
    }
}

bool arithmetic (const Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
    case Int8_Kind:
    case Int16_Kind:
    case Int32_Kind:
    case Int64_Kind:
    case Float32_Kind:
    case Float64_Kind:
    case Complex64_Kind:
    case Complex128_Kind:
    case Uint_Kind:
    case Int_Kind:
    case Uintptr_Kind:
    case Rune_Kind:
    case Integer_Kind:
    case Float_Kind:
    case Complex_Kind:
      return true;
    default:
      return false;
    }
}

bool integral (const Type* type)
{
  switch (type->underlying_kind ())
    {
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
    case Int8_Kind:
    case Int16_Kind:
    case Int32_Kind:
    case Int64_Kind:
    case Uint_Kind:
    case Int_Kind:
    case Uintptr_Kind:
    case Rune_Kind:
    case Integer_Kind:
      return true;
    default:
      return false;
    }
}

bool is_typed_boolean (const Type* type)
{
  return type->underlying_kind () == Bool_Kind;
}

bool is_untyped_boolean (const Type* type)
{
  return type->underlying_kind () == Boolean_Kind;
}

bool is_any_boolean (const Type* type)
{
  return is_typed_boolean (type) || is_untyped_boolean (type);
}

const Pointer*
pointer_to_array (const Type* type)
{
  const Pointer* p = type_cast<Pointer> (type->UnderlyingType ());
  if (p != NULL && p->base_type->underlying_kind () == Array_Kind)
    {
      return p;
    }
  return NULL;
}

NamedType named_bool ("bool", Bool::Instance ());

NamedType named_uint8 ("uint8", Uint8::Instance ());
NamedType named_uint16 ("uint16", Uint16::Instance ());
NamedType named_uint32 ("uint32", Uint32::Instance ());
NamedType named_uint64 ("uint64", Uint64::Instance ());

NamedType named_int8 ("int8", Int8::Instance ());
NamedType named_int16 ("int16", Int16::Instance ());
NamedType named_int32 ("int32", Int32::Instance ());
NamedType named_int64 ("int64", Int64::Instance ());

NamedType named_float32 ("float32", Float32::Instance ());
NamedType named_float64 ("float64", Float64::Instance ());

NamedType named_complex64 ("complex64", Complex64::Instance ());
NamedType named_complex128 ("complex128", Complex128::Instance ());

NamedType named_byte ("byte", Uint8::Instance ());
NamedType named_rune ("rune", Int::Instance ());

NamedType named_uint ("uint", Uint::Instance ());
NamedType named_int ("int", Int::Instance ());
NamedType named_uintptr ("uintptr", Uintptr::Instance ());

NamedType named_string ("string", StringU::Instance ());

NamedType named_file_descriptor ("FileDescriptor", FileDescriptor::Instance ());
NamedType named_timespec ("timespec", (new Struct ())->append_field (NULL, false, "tv_sec", &named_uint64, TagSet ())->append_field (NULL, false, "tv_nsec", &named_uint64, TagSet ()));

decl::ParameterSymbol* Function::GetParameter (const std::string& name) const
{
  return parameter_list->find (name);
}

decl::ParameterSymbol* Function::GetReturnParameter () const
{
  return return_parameter_list->at (0);
}

void Interface::print (std::ostream& out) const
{
  std::stringstream ss;
  ss << "interface {";
  for (MethodsType::const_iterator pos = methods.begin (), limit = methods.end ();
       pos != limit;
       ++pos)
    {
      ss << pos->first << ' ' << *pos->second << ';';
    }
  ss << '}';
}

void Map::print (std::ostream& out) const
{
  out << "map[" << *key_type << ']' << *value_type;
}

const Type* Arithmetic::pick (const Type* left_type, const Type* right_type)
{
  switch (left_type->underlying_kind ())
    {
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
    case Int8_Kind:
    case Int16_Kind:
    case Int32_Kind:
    case Int64_Kind:
    case Float32_Kind:
    case Float64_Kind:
    case Complex64_Kind:
    case Complex128_Kind:
    case Uint_Kind:
    case Int_Kind:
    case Uintptr_Kind:
      if (left_type == right_type)
        {
          return left_type;
        }
      break;

    case Rune_Kind:
    case Integer_Kind:
    case Float_Kind:
    case Complex_Kind:
      if (is_untyped_numeric (right_type))
        {
          return Choose (left_type, right_type);
        }
      break;

    default:
      break;
    }

  return NULL;
}

const Type* Integral::pick (const Type* left_type, const Type* right_type)
{
  switch (left_type->underlying_kind ())
    {
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
    case Int8_Kind:
    case Int16_Kind:
    case Int32_Kind:
    case Int64_Kind:
    case Uint_Kind:
    case Int_Kind:
    case Uintptr_Kind:
      if (left_type == right_type)
        {
          return left_type;
        }
      break;

    case Rune_Kind:
    case Integer_Kind:
      if (right_type->underlying_kind () == Rune_Kind ||
          right_type->underlying_kind () == Integer_Kind)
        {
          return Choose (left_type, right_type);
        }
      break;

    default:
      break;
    }

  return NULL;
}

const type::Type* Comparable::pick (const type::Type* left_type, const type::Type* right_type)
{
  switch (left_type->underlying_kind ())
    {
    case Nil_Kind:
    case Boolean_Kind:
    case Untyped_String_Kind:
    case Bool_Kind:
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
    case Int8_Kind:
    case Int16_Kind:
    case Int32_Kind:
    case Int64_Kind:
    case Float32_Kind:
    case Float64_Kind:
    case Complex64_Kind:
    case Complex128_Kind:
    case Uint_Kind:
    case Int_Kind:
    case Uintptr_Kind:
    case String_Kind:
    case Pointer_Kind:
      if (left_type == right_type)
        {
          return left_type;
        }
      break;

    case Rune_Kind:
    case Integer_Kind:
    case Float_Kind:
    case Complex_Kind:
      if (is_untyped_numeric (right_type))
        {
          return Choose (left_type, right_type);
        }
      break;

    default:
      break;
    }

  return NULL;
}

const type::Type* Orderable::pick (const type::Type* left_type, const type::Type* right_type)
{
  switch (left_type->underlying_kind ())
    {
    case Untyped_String_Kind:
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
    case Int8_Kind:
    case Int16_Kind:
    case Int32_Kind:
    case Int64_Kind:
    case Float32_Kind:
    case Float64_Kind:
    case Uint_Kind:
    case Int_Kind:
    case Uintptr_Kind:
    case String_Kind:
      if (left_type == right_type)
        {
          return left_type;
        }
      break;

    case Rune_Kind:
    case Integer_Kind:
    case Float_Kind:
      if (right_type->underlying_kind () == Rune_Kind ||
          right_type->underlying_kind () == Integer_Kind ||
          right_type->underlying_kind () == Float_Kind)
        {
          return Choose (left_type, right_type);
        }
      break;

    default:
      break;
    }

  return NULL;
}

const type::Type* Logical::pick (const type::Type* left_type, const type::Type* right_type)
{
  if ((left_type->underlying_kind () == Bool_Kind ||
       left_type->underlying_kind () == Boolean_Kind) &&
      (right_type->underlying_kind () == Bool_Kind ||
       right_type->underlying_kind () == Boolean_Kind))
    {
      return Choose (left_type, right_type);
    }

  return NULL;
}

}
