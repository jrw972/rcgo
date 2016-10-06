#include "type.hpp"

#include <sstream>
#include <algorithm>

#include "callable.hpp"
#include "arch.hpp"
#include "parameter_list.hpp"
#include "symbol_visitor.hpp"
#include "semantic.hpp"
#include "node.hpp"
#include "error_reporter.hpp"
#include "process_type.hpp"

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

std::string Type::to_error_string () const
{
  std::stringstream str;
  print (str);
  return str.str ();
}

const Type* Type::underlying_type () const
{
  return this;
}

Kind Type::underlying_kind () const
{
  return underlying_type ()->kind ();
}

const Type* Type::default_type () const
{
  return this;
}

bool Type::is_untyped () const
{
  return kind () >= Untyped_Nil_Kind && kind () <= Untyped_String_Kind;
}

bool Type::is_unnamed () const
{
  return kind () >= Bool_Kind && kind () <= File_Descriptor_Kind;
}

bool Type::is_named () const
{
  return kind () == Named_Kind;
}

bool Type::is_numeric () const
{
  switch (underlying_kind ())
    {
    case Untyped_Rune_Kind:
    case Untyped_Integer_Kind:
    case Untyped_Float_Kind:
    case Untyped_Complex_Kind:
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

bool
Type::contains_pointer () const
{
  return underlying_type ()->contains_pointer_i ();
}

bool
Type::contains_pointer_i () const
{
  return false;
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
Type::get_array (long dimension) const
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

Field* Type::find_field (const std::string& name) const
{
  return underlying_type ()->find_field_i (name);
}

decl::Method* Type::find_method (const std::string& identifier) const
{
  return NULL;
}
decl::Initializer* Type::find_initializer (const std::string& identifier) const
{
  return NULL;
}
decl::Getter* Type::find_getter (const std::string& identifier) const
{
  return NULL;
}
decl::Action* Type::find_action (const std::string& identifier) const
{
  return NULL;
}
decl::Reaction* Type::find_reaction (const std::string& identifier) const
{
  return NULL;
}
decl::Bind* Type::find_bind (const std::string& identifier) const
{
  return NULL;
}

decl::Callable* Type::find_callable (const std::string& name) const
{
  return NULL;
}

const NamedType* Type::to_named_type () const
{
  return NULL;
}

void NamedType::accept (decl::SymbolVisitor& visitor)
{
  visitor.visit (*this);
}
void NamedType::accept (decl::ConstSymbolVisitor& visitor) const
{
  visitor.visit (*this);
}

bool
NamedType::process_declaration_i (util::ErrorReporter& er, Scope* file_scope)
{
  process_type (typedecl_->type, er, file_scope, true);
  if (typedecl_->type->eval.expression_kind == TypeExpressionKind)
    {
      underlying_type (typedecl_->type->eval.type);
      return true;
    }

  return false;
}

struct check_member_func
{
  util::ErrorReporter& er;
  Scope& scope;

  check_member_func (util::ErrorReporter& a_er,
                     Scope& a_scope)
    : er (a_er)
    , scope (a_scope)
  { }

  void operator() (Symbol* symbol)
  {
    const Symbol* prev = scope.find_local_symbol (symbol->name);
    if (prev)
      {
        er.already_declared (symbol->location, symbol->name, prev->location);
      }
    scope.enter_symbol (symbol);
  }

};

struct process_declaration_func
{
  util::ErrorReporter& er;
  Scope* file_scope;

  process_declaration_func (util::ErrorReporter& a_er,
                            Scope* a_file_scope)
    : er (a_er)
    , file_scope (a_file_scope)
  { }

  template <typename T>
  void operator() (T* t)
  {
    t->process_declaration (er, file_scope);
  }

};

void
NamedType::post_process_declaration_i (util::ErrorReporter& er, Scope* file_scope)
{
  Scope s;
  // Enter all fields in a struct.
  const Struct* st = underlying_type ()->to_struct ();
  if (st)
    {
      for (Struct::const_iterator pos = st->begin (), limit = st->end ();
           pos != limit;
           ++pos)
        {
          s.enter_symbol ((*pos));
        }
    }

  std::for_each (methods_.begin (), methods_.end (), check_member_func (er, s));
  std::for_each (initializers_.begin (), initializers_.end (), check_member_func (er, s));
  std::for_each (getters_.begin (), getters_.end (), check_member_func (er, s));
  std::for_each (actions_.begin (), actions_.end (), check_member_func (er, s));
  std::for_each (reactions_.begin (), reactions_.end (), check_member_func (er, s));
  std::for_each (binds_.begin (), binds_.end (), check_member_func (er, s));

  std::for_each (methods_.begin (), methods_.end (), process_declaration_func (er, file_scope));
  std::for_each (initializers_.begin (), initializers_.end (), process_declaration_func (er, file_scope));
  std::for_each (getters_.begin (), getters_.end (), process_declaration_func (er, file_scope));
  std::for_each (actions_.begin (), actions_.end (), process_declaration_func (er, file_scope));
  std::for_each (reactions_.begin (), reactions_.end (), process_declaration_func (er, file_scope));
  std::for_each (binds_.begin (), binds_.end (), process_declaration_func (er, file_scope));
}

const Array* Type::to_array () const
{
  return NULL;
}
const Slice* Type::to_slice () const
{
  return NULL;
}
const Struct* Type::to_struct () const
{
  return NULL;
}
const Pointer* Type::to_pointer () const
{
  return NULL;
}
const Function* Type::to_function () const
{
  return NULL;
}
const PushPort* Type::to_push_port () const
{
  return NULL;
}
const Reaction* Type::to_reaction () const
{
  return NULL;
}
const PullPort* Type::to_pull_port () const
{
  return NULL;
}
const Interface* Type::to_interface () const
{
  return NULL;
}
const Map* Type::to_map () const
{
  return NULL;
}
const Heap* Type::to_heap () const
{
  return NULL;
}
const Method* Type::to_method () const
{
  return NULL;
}
const Initializer* Type::to_initializer () const
{
  return NULL;
}
const Getter* Type::to_getter () const
{
  return NULL;
}

Field* Type::find_field_i (const std::string& name) const
{
  return NULL;
}

void
Array::print (std::ostream& out) const
{
  out << '[' << dimension << ']' << *base_type;
}



decl::Reaction *
NamedType::find_reaction (const std::string& identifier) const
{
  for (std::vector<decl::Reaction*>::const_iterator pos = reactions_.begin (),
       limit = reactions_.end ();
       pos != limit;
       ++pos)
    {
      decl::Reaction *a = *pos;
      if (a->name == identifier)
        {
          return a;
        }
    }

  return NULL;
}

decl::Method*
NamedType::find_method (const std::string& identifier) const
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

decl::Initializer*
NamedType::find_initializer (const std::string& identifier) const
{
  for (std::vector<decl::Initializer*>::const_iterator pos = this->initializers_.begin (),
       limit = this->initializers_.end ();
       pos != limit;
       ++pos)
    {
      decl::Initializer* a = *pos;
      if (a->name == identifier)
        {
          return a;
        }
    }

  return NULL;
}

decl::Getter*
NamedType::find_getter (const std::string& identifier) const
{
  for (std::vector<decl::Getter*>::const_iterator pos = this->getters_.begin (),
       limit = this->getters_.end ();
       pos != limit;
       ++pos)
    {
      decl::Getter* a = *pos;
      if (a->name == identifier)
        {
          return a;
        }
    }

  return NULL;
}

Action*
NamedType::find_action (const std::string& identifier) const
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
NamedType::find_bind (const std::string& identifier) const
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
  out << "struct {";
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
Struct::append_field (Package* package,
                      bool is_anonymous,
                      const std::string& field_name,
                      const util::Location& location,
                      const Type* field_type,
                      const TagSet& tags)
{
  Field* field = new Field (this, package, is_anonymous, field_name, location, field_type, tags);
  fields_.push_back (field);
  return this;
}

Field *
Struct::find_field_i (const std::string& name) const
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

bool
Type::has_member (const std::string& identifier) const
{
  return
    this->find_field (identifier) ||
    this->find_method (identifier) ||
    this->find_initializer (identifier) ||
    this->find_getter (identifier) ||
    this->find_reaction (identifier) ||
    this->find_action (identifier) ||
    this->find_bind (identifier);
}

const Type*
Type::move () const
{
  const Pointer* ptf = this->to_pointer ();
  if (ptf)
    {
      const Heap* h = ptf->base_type->to_heap ();
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
  const Pointer* ptf = this->to_pointer ();

  if (ptf)
    {
      const Heap* h = ptf->base_type->to_heap ();
      if (h)
        {
          return h->base_type->get_pointer ();
        }
    }

  return NULL;
}

static bool
function_base_are_identical (const FunctionBase* type1,
                             const FunctionBase* type2)
{
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

      Struct::const_iterator pos1 = type1->begin ();
      Struct::const_iterator limit1 = type1->end ();
      Struct::const_iterator pos2 = type2->begin ();
      Struct::const_iterator limit2 = type2->end ();

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
      return function_base_are_identical (type1, type2);
    }
    case Push_Port_Kind:
    {
      const PushPort* type1 = x->to_push_port ();
      const PushPort* type2 = y->to_push_port ();
      return function_base_are_identical (type1, type2);
    }
    case Pull_Port_Kind:
    {
      const PullPort* type1 = x->to_pull_port ();
      const PullPort* type2 = y->to_pull_port ();
      return function_base_are_identical (type1, type2);
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
      NOT_REACHED;
    }
}

#define INSTANCE(type) const type* \
type::instance () \
{ \
  static type* i = new type ();                 \
  return i; \
}

INSTANCE(FileDescriptor)
INSTANCE(UntypedNil)
INSTANCE(UntypedBoolean)
INSTANCE(UntypedRune)
INSTANCE(UntypedInteger)
INSTANCE(UntypedFloat)
INSTANCE(UntypedComplex)
INSTANCE(UntypedString)
INSTANCE(PolymorphicFunction)
INSTANCE(String)

Component::Component (Package* package, const util::Location& location)
  : Struct ()
{
  /* Prepend the field list with a pointer for the runtime. */
  append_field (package, true, "0runtime", location, Int32::instance ()->get_pointer (), TagSet ());
}

bool
Array::contains_pointer_i () const
{
  return base_type->contains_pointer ();
}

bool
Heap::contains_pointer_i () const
{
  return base_type->contains_pointer ();
}

bool
Pointer::contains_pointer_i () const
{
  return true;
}

bool
Slice::contains_pointer_i () const
{
  return true;
}

bool
String::contains_pointer_i () const
{
  return true;
}

bool
FileDescriptor::contains_pointer_i () const
{
  return true;
}

bool
Struct::contains_pointer_i () const
{
  for (Struct::const_iterator pos = begin (), limit = end ();
       pos != limit;
       ++pos)
    {
      if ((*pos)->type->contains_pointer ())
        {
          return true;
        }
    }

  return false;
}

NamedType::NamedType (ast::TypeDecl* a_typedecl)
  : Symbol (a_typedecl->identifier->identifier, a_typedecl->identifier->location)
  , underlyingType_ (NULL)
  , typedecl_ (a_typedecl)
{ }

NamedType::NamedType (const std::string& name,
                      const util::Location& location,
                      const Type* subtype)
  : Symbol (name, location)
  , underlyingType_ (subtype->underlying_type ())
  , typedecl_ (NULL)
{
  assert (underlyingType_->is_unnamed ());
  state_ = Symbol::Defined;
}

void NamedType::print (std::ostream& out) const
{
  out << name;
}

void NamedType::underlying_type (const Type* u)
{
  underlyingType_ = u->underlying_type ();
  assert (underlyingType_->is_unnamed ());
}

const Type* NamedType::underlying_type () const
{
  return underlyingType_;
}

Kind NamedType::kind () const
{
  return Named_Kind;
}
void NamedType::insert_method (decl::Method* method)
{
  methods_.push_back (method);
}
void NamedType::insert_initializer (decl::Initializer* initializer)
{
  initializers_.push_back (initializer);
}
void NamedType::insert_getter (decl::Getter* getter)
{
  getters_.push_back (getter);
}
NamedType::GettersType::const_iterator NamedType::getters_begin () const
{
  return getters_.begin ();
}
NamedType::GettersType::const_iterator NamedType::getters_end () const
{
  return getters_.end ();
}
void NamedType::insert_action (decl::Action* action)
{
  actions_.push_back (action);
}
NamedType::ActionsType::const_iterator NamedType::actions_begin () const
{
  return actions_.begin ();
}
NamedType::ActionsType::const_iterator NamedType::actions_end () const
{
  return actions_.end ();
}
void NamedType::insert_reaction (decl::Reaction* reaction)
{
  reactions_.push_back (reaction);
}
NamedType::ReactionsType::const_iterator NamedType::reactions_begin () const
{
  return reactions_.begin ();
}
NamedType::ReactionsType::const_iterator NamedType::reactions_end () const
{
  return reactions_.end ();
}
void NamedType::insert_bind (decl::Bind* bind)
{
  binds_.push_back (bind);
}
NamedType::BindsType::const_iterator NamedType::binds_begin () const
{
  return binds_.begin ();
}
NamedType::BindsType::const_iterator NamedType::binds_end () const
{
  return binds_.end ();
}
const NamedType* NamedType::to_named_type () const
{
  return this;
}

void
Function::print (std::ostream& out) const
{
  out << "func " << *parameter_list << ' ' << *return_parameter_list;
}

void
PushPort::print (std::ostream& out) const
{
  out << "push " << *parameter_list;
}

void
PullPort::print (std::ostream& out) const
{
  out << "pull " << *parameter_list << ' ' << *return_parameter_list;
}

void
Method::print (std::ostream& out) const
{
  out << '(' << *receiver_parameter->type << ')' << " func " << *parameter_list << ' ' << *return_parameter_list;
}

void
Initializer::print (std::ostream& out) const
{
  out << '(' << *receiver_parameter->type << ')' << " init " << *parameter_list << ' ' << *return_parameter_list;
}

void
Getter::print (std::ostream& out) const
{
  out << '(' << *receiver_parameter->type << ')' << " getter " << *parameter_list << ' ' << *return_parameter_list;
}

void
Reaction::print (std::ostream& out) const
{
  out << '(' << *receiver_parameter->type << ')' << " reaction " << *parameter_list;
}

Function*
MethodBase::make_function_type (Parameter* this_parameter,
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
  return new Function (sig, return_parameter_list);
}

MethodBase::MethodBase (const NamedType* named_type_,
                        Parameter* receiver_parameter_,
                        const ParameterList* a_parameter_list,
                        const ParameterList* a_return_parameter_list)
  : named_type (named_type_)
  , receiver_parameter (receiver_parameter_)
  , parameter_list (a_parameter_list)
  , return_parameter_list (a_return_parameter_list)
  , function_type (make_function_type (receiver_parameter_, a_parameter_list, a_return_parameter_list))
{
}

const Type*
UntypedBoolean::default_type () const
{
  return &named_bool;
}

const Type*
UntypedRune::default_type () const
{
  return &named_rune;
}

const Type*
UntypedInteger::default_type () const
{
  return &named_int;
}

const Type*
UntypedFloat::default_type () const
{
  return &named_float64;
}

const Type*
UntypedComplex::default_type () const
{
  return &named_complex128;
}

const Type*
UntypedString::default_type () const
{
  return &named_string;
}

const Type*
choose (const Type* x, const Type* y)
{
  return (x->kind () > y->kind ()) ? x : y;
}

bool
are_assignable (const Type* from, const Value& from_value, const Type* to)
{
  if (to->is_untyped ())
    {
      return false;
    }

  if (are_identical (from, to))
    {
      return true;
    }

  if (are_identical (from->underlying_type (), to->underlying_type ()) &&
      (!from->is_named () || !to->is_named ()))
    {
      return true;
    }

  // TODO:  T is an interface type and x implements T.

  if (from->kind () == Untyped_Nil_Kind &&
      (to->underlying_kind () == Pointer_Kind ||
       to->underlying_kind () == Function_Kind ||
       to->underlying_kind () == Slice_Kind ||
       to->underlying_kind () == Map_Kind ||
       to->underlying_kind () == Interface_Kind))
    {
      return true;
    }

  if (from->is_untyped () && from_value.representable (from, to))
    {
      return true;
    }

  return false;
}

Callable*
NamedType::find_callable (const std::string& name) const
{
  decl::Method* m = this->find_method (name);
  if (m)
    {
      return m;
    }

  decl::Initializer* i = this->find_initializer (name);
  if (i)
    {
      return i;
    }

  decl::Getter* g = this->find_getter (name);
  if (g)
    {
      return g;
    }

  decl::Reaction* r = this->find_reaction (name);
  if (r)
    {
      return r;
    }

  return NULL;
}

bool Type::is_typed_integer () const
{
  switch (underlying_type ()->kind ())
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
      return true;
    default:
      return false;
    }
}

bool Type::is_typed_unsigned_integer () const
{
  switch (underlying_kind ())
    {
    case Uint8_Kind:
    case Uint16_Kind:
    case Uint32_Kind:
    case Uint64_Kind:
    case Uint_Kind:
      return true;
    default:
      return false;
    }
}

bool Type::is_typed_float () const
{
  switch (underlying_kind ())
    {
    case Float32_Kind:
    case Float64_Kind:
      return true;
    default:
      return false;
    }
}

bool Type::is_typed_complex () const
{
  switch (underlying_kind ())
    {
    case Complex64_Kind:
    case Complex128_Kind:
      return true;
    default:
      return false;
    }
}

bool Type::is_typed_numeric () const
{
  switch (underlying_kind ())
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

bool Type::is_untyped_numeric () const
{
  switch (underlying_kind ())
    {
    case Untyped_Rune_Kind:
    case Untyped_Integer_Kind:
    case Untyped_Float_Kind:
    case Untyped_Complex_Kind:
      return true;
    default:
      return false;
    }
}

bool Type::is_typed_string () const
{
  return underlying_kind () == String_Kind;
}

bool Type::is_any_string () const
{
  return underlying_kind () == String_Kind || underlying_kind () == Untyped_String_Kind;
}

bool Type::is_slice_of_bytes () const
{
  const Slice* slice_type = underlying_type ()->to_slice ();
  if (slice_type == NULL)
    {
      return false;
    }
  return slice_type->base_type->underlying_type ()->kind () == Uint8_Kind;
}

bool Type::is_slice_of_runes () const
{
  const Slice* slice_type = underlying_type ()->to_slice ();
  if (slice_type == NULL)
    {
      return false;
    }
  return slice_type->base_type->underlying_type ()->kind () == Int32_Kind;
}

bool Type::is_arithmetic () const
{
  switch (underlying_kind ())
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
    case Untyped_Rune_Kind:
    case Untyped_Integer_Kind:
    case Untyped_Float_Kind:
    case Untyped_Complex_Kind:
      return true;
    default:
      return false;
    }
}

bool Type::is_integral () const
{
  switch (underlying_kind ())
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
    case Untyped_Rune_Kind:
    case Untyped_Integer_Kind:
      return true;
    default:
      return false;
    }
}

bool Type::is_any_boolean () const
{
  return underlying_kind () == Bool_Kind || underlying_kind () == Untyped_Boolean_Kind;
}

const Pointer*
Type::pointer_to_array () const
{
  const Pointer* p = underlying_type ()->to_pointer ();
  if (p != NULL && p->base_type->underlying_kind () == Array_Kind)
    {
      return p;
    }
  return NULL;
}

util::Location loc ("<builtin>");
NamedType named_bool ("bool", loc, Bool::instance ());

NamedType named_uint8 ("uint8", loc, Uint8::instance ());
NamedType named_uint16 ("uint16", loc, Uint16::instance ());
NamedType named_uint32 ("uint32", loc, Uint32::instance ());
NamedType named_uint64 ("uint64", loc, Uint64::instance ());

NamedType named_int8 ("int8", loc, Int8::instance ());
NamedType named_int16 ("int16", loc, Int16::instance ());
NamedType named_int32 ("int32", loc, Int32::instance ());
NamedType named_int64 ("int64", loc, Int64::instance ());

NamedType named_float32 ("float32", loc, Float32::instance ());
NamedType named_float64 ("float64", loc, Float64::instance ());

NamedType named_complex64 ("complex64", loc, Complex64::instance ());
NamedType named_complex128 ("complex128", loc, Complex128::instance ());

NamedType named_byte ("byte", loc, Uint8::instance ());
NamedType named_rune ("rune", loc, Int32::instance ());

NamedType named_uint ("uint", loc, Uint::instance ());
NamedType named_int ("int", loc, Int::instance ());
NamedType named_uintptr ("uintptr", loc, Uintptr::instance ());

NamedType named_string ("string", loc, String::instance ());

NamedType named_file_descriptor ("FileDescriptor", loc, FileDescriptor::instance ());
NamedType named_timespec ("timespec", loc, (new Struct ())->append_field (NULL, false, "tv_sec", loc, &named_uint64, TagSet ())->append_field (NULL, false, "tv_nsec", loc, &named_uint64, TagSet ()));

void Interface::print (std::ostream& out) const
{
  out << "interface {";
  for (MethodsType::const_iterator pos = methods.begin (), limit = methods.end ();
       pos != limit;
       ++pos)
    {
      out << pos->first << ' ' << *pos->second << ';';
    }
  out << '}';
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

    case Untyped_Rune_Kind:
    case Untyped_Integer_Kind:
    case Untyped_Float_Kind:
    case Untyped_Complex_Kind:
      if (right_type->is_untyped_numeric ())
        {
          return choose (left_type, right_type);
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

    case Untyped_Rune_Kind:
    case Untyped_Integer_Kind:
      if (right_type->underlying_kind () == Untyped_Rune_Kind ||
          right_type->underlying_kind () == Untyped_Integer_Kind)
        {
          return choose (left_type, right_type);
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
    case Untyped_Nil_Kind:
    case Untyped_Boolean_Kind:
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

    case Untyped_Rune_Kind:
    case Untyped_Integer_Kind:
    case Untyped_Float_Kind:
    case Untyped_Complex_Kind:
      if (right_type->is_untyped_numeric ())
        {
          return choose (left_type, right_type);
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

    case Untyped_Rune_Kind:
    case Untyped_Integer_Kind:
    case Untyped_Float_Kind:
      if (right_type->underlying_kind () == Untyped_Rune_Kind ||
          right_type->underlying_kind () == Untyped_Integer_Kind ||
          right_type->underlying_kind () == Untyped_Float_Kind)
        {
          return choose (left_type, right_type);
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
       left_type->underlying_kind () == Untyped_Boolean_Kind) &&
      (right_type->underlying_kind () == Bool_Kind ||
       right_type->underlying_kind () == Untyped_Boolean_Kind))
    {
      return choose (left_type, right_type);
    }

  return NULL;
}

BaseType::BaseType (const Type* a_base_type)
  : base_type (a_base_type)
{ }

void Pointer::print (std::ostream& out) const
{
  out << "*" << *base_type;
}
Kind Pointer::kind () const
{
  return Pointer_Kind;
}
Field* Pointer::find_field_i (const std::string& name) const
{
  return base_type->find_field (name);
}
decl::Callable* Pointer::find_callable (const std::string& name) const
{
  return base_type->find_callable (name);
}
const Pointer* Pointer::to_pointer () const
{
  return this;
}
Pointer::Pointer (const Type* base) : BaseType (base) { }
void Slice::print (std::ostream& out) const
{
  out << "[]" << *base_type;
}
Kind Slice::kind () const
{
  return Slice_Kind;
}
const Slice* Slice::to_slice () const
{
  return this;
}
Slice::Slice (const Type* base) : BaseType (base) { }
Kind Array::kind () const
{
  return Array_Kind;
}
const Array* Array::to_array () const
{
  return this;
}
Array::Array (long d, const Type* base) : BaseType (base), dimension (d) { }

Kind Map::kind () const
{
  return Map_Kind;
}
const Map* Map::to_map () const
{
  return this;
}

Map::Map (const Type* a_key_type,
          const Type* a_value_type)
  : key_type (a_key_type)
  , value_type (a_value_type)
{ }

const Map* Map::make (const Type* key_type, const Type* value_type)
{
  MapType::const_iterator pos = maps_.find (std::make_pair (key_type, value_type));
  if (pos == maps_.end ())
    {
      pos = maps_.insert (std::make_pair (std::make_pair (key_type, value_type), new Map (key_type, value_type))).first;
    }
  return pos->second;
}

Map::MapType Map::maps_;

void Heap::print (std::ostream& out) const
{
  out << "heap " << *base_type;
}
Kind Heap::kind () const
{
  return Heap_Kind;
}
const Heap* Heap::to_heap () const
{
  return this;
}
Heap::Heap (const Type* base) : BaseType (base) { }

Kind Struct::kind () const
{
  return Struct_Kind;
}
const Struct* Struct::to_struct () const
{
  return this;
}
Struct::const_iterator Struct::begin () const
{
  return fields_.begin ();
}
Struct::const_iterator Struct::end () const
{
  return fields_.end ();
}
size_t Struct::field_count () const
{
  return fields_.size ();
}
void
Component::print (std::ostream& out) const
{
  out << "component {";
  FieldsType::const_iterator pos = fields_.begin (), limit = fields_.end ();
  ++pos;
  for (; pos != limit; ++pos)
    {
      Field* f = *pos;
      out << f->name << ' ' << *(f->type) << ';';
    }
  out << '}';
}
Kind Component::kind () const
{
  return Component_Kind;
}
FunctionBase::FunctionBase (const decl::ParameterList* a_parameter_list,
                            const decl::ParameterList* a_return_parameter_list)
  : parameter_list (a_parameter_list)
  , return_parameter_list (a_return_parameter_list)
{ }

Kind Function::kind () const
{
  return type::Function_Kind;
}
const Function* Function::to_function () const
{
  return this;
}

PushPort::PushPort (const decl::ParameterList* a_parameter_list,
                    const decl::ParameterList* a_return_parameter_list)
  : FunctionBase (a_parameter_list, a_return_parameter_list)
{
  assert (a_return_parameter_list->empty ());
}
PullPort::PullPort (const decl::ParameterList* a_parameter_list,
                    const decl::ParameterList* a_return_parameter_list)
  : FunctionBase (a_parameter_list, a_return_parameter_list)
{ }
Function::Function (const decl::ParameterList* a_parameter_list,
                    const decl::ParameterList* a_return_parameter_list)
  : FunctionBase (a_parameter_list, a_return_parameter_list)
{ }
Kind PushPort::kind () const
{
  return type::Push_Port_Kind;
}
Kind PullPort::kind () const
{
  return type::Pull_Port_Kind;
}
const PushPort* PushPort::to_push_port () const
{
  return this;
}
const PullPort* PullPort::to_pull_port () const
{
  return this;
}

Method::Method (const NamedType* a_named_type,
                decl::Parameter* a_receiver_parameter,
                const decl::ParameterList* a_parameter_list,
                const decl::ParameterList* a_return_parameter_list)
  : MethodBase (a_named_type, a_receiver_parameter, a_parameter_list, a_return_parameter_list)
{ }

Initializer::Initializer (const NamedType* a_named_type,
                          decl::Parameter* a_receiver_parameter,
                          const decl::ParameterList* a_parameter_list,
                          const decl::ParameterList* a_return_parameter_list)
  : MethodBase (a_named_type, a_receiver_parameter, a_parameter_list, a_return_parameter_list)
{ }

Getter::Getter (const NamedType* a_named_type,
                decl::Parameter* a_receiver_parameter,
                const decl::ParameterList* a_parameter_list,
                const decl::ParameterList* a_return_parameter_list)
  : MethodBase (a_named_type, a_receiver_parameter, a_parameter_list, a_return_parameter_list)
{ }

Reaction::Reaction (const NamedType* a_named_type,
                    decl::Parameter* a_receiver_parameter,
                    const decl::ParameterList* a_parameter_list,
                    const decl::ParameterList* a_return_parameter_list)
  : MethodBase (a_named_type, a_receiver_parameter, a_parameter_list, a_return_parameter_list)
{
  assert (a_return_parameter_list->empty ());
}

Kind Method::kind () const
{
  return Method_Kind;
}

Kind Initializer::kind () const
{
  return Initializer_Kind;
}

Kind Getter::kind () const
{
  return Getter_Kind;
}

Kind Reaction::kind () const
{
  return Reaction_Kind;
}

Interface::Interface (decl::Package* a_package)
  : package (a_package)
{ }

Kind Interface::kind () const
{
  return Interface_Kind;
}
const Interface* Interface::to_interface () const
{
  return this;
}

Kind UntypedNil::kind () const
{
  return Untyped_Nil_Kind;
}

void UntypedNil::print (std::ostream& out) const
{
  out << "<<nil>>";
}

UntypedNil::UntypedNil () { }

Kind UntypedBoolean::kind () const
{
  return Untyped_Boolean_Kind;
}

void UntypedBoolean::print (std::ostream& out) const
{
  out << "<<boolean>>";
}

UntypedBoolean::UntypedBoolean () { }

Kind UntypedRune::kind () const
{
  return Untyped_Rune_Kind;
}

void UntypedRune::print (std::ostream& out) const
{
  out << "<<rune>>";
}

UntypedRune::UntypedRune () { }

Kind UntypedInteger::kind () const
{
  return Untyped_Integer_Kind;
}

void UntypedInteger::print (std::ostream& out) const
{
  out << "<<integer>>";
}

UntypedInteger::UntypedInteger () { }

Kind UntypedFloat::kind () const
{
  return Untyped_Float_Kind;
}

void UntypedFloat::print (std::ostream& out) const
{
  out << "<<float>>";
}

UntypedFloat::UntypedFloat () { }

Kind UntypedComplex::kind () const
{
  return Untyped_Complex_Kind;
}

void UntypedComplex::print (std::ostream& out) const
{
  out << "<<complex>>";
}

UntypedComplex::UntypedComplex () { }

Kind UntypedString::kind () const
{
  return Untyped_String_Kind;
}

void UntypedString::print (std::ostream& out) const
{
  out << "<<string>>";
}

UntypedString::UntypedString () { }

void PolymorphicFunction::print (std::ostream& out) const
{
  out << "<<polymorphic function>>";
}

Kind PolymorphicFunction::kind () const
{
  return Polymorphic_Function_Kind;
}
PolymorphicFunction::PolymorphicFunction () { }

void FileDescriptor::print (std::ostream& out) const
{
  out << "<FileDescriptor>";
}

Kind FileDescriptor::kind () const
{
  return File_Descriptor_Kind;
}

FileDescriptor::FileDescriptor () { }

const Initializer* Initializer::to_initializer () const
{
  return this;
}

const Method* Method::to_method () const
{
  return this;
}

const Getter* Getter::to_getter () const
{
  return this;
}

const Reaction* Reaction::to_reaction () const
{
  return this;
}

void String::print (std::ostream& out) const
{
  out << "<string>";
}

Kind String::kind () const
{
  return String_Kind;
}

String::String () { }

}
