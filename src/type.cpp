#include "type.hpp"
#include "action.hpp"
#include "parameter.hpp"
#include "field.hpp"
#include "method.hpp"

reaction_t *
named_type_t::get_reaction (string_t identifier) const
{
  for (std::vector<reaction_t*>::const_iterator pos = reactions_.begin (),
         limit = reactions_.end ();
       pos != limit;
       ++pos)
    {
      reaction_t *a = *pos;
      if (streq (a->name (), identifier))
        {
          return a;
        }
    }

  return NULL;
}

method_t*
named_type_t::get_method (string_t identifier) const
{
  for (std::vector<method_t*>::const_iterator pos = this->methods_.begin (),
         limit = this->methods_.end ();
       pos != limit;
       ++pos)
    {
      method_t *a = *pos;
      if (streq (a->name, identifier))
        {
          return a;
        }
    }

  return NULL;
}

void
field_list_type_t::append (string_t field_name, const type_t * field_type)
{
  size_t alignment = field_type->alignment ();
  offset_ = align_up (offset_, alignment);

  field_t *field = field_make (field_name, field_type, offset_);
  fields_.push_back (field);

  offset_ += field_type->size ();
  if (alignment > alignment_)
    {
      alignment_ = alignment;
    }
}

field_t *
field_list_type_t::find (string_t name) const
{
  for (std::vector<field_t*>::const_iterator field = fields_.begin (),
         limit = fields_.end ();
       field != limit;
       ++field)
    {
      if (streq (name, field_name (*field)))
        {
          return (*field);
        }
    }
  return NULL;
}

field_t *
type_select_field (const type_t * type, string_t identifier)
{
  struct visitor : public const_type_visitor_t
  {
    field_t* retval;
    string_t identifier;
    visitor (string_t id) : retval (NULL), identifier (id) { }

    void visit (const named_type_t& type)
    {
      type.subtype ()->accept (*this);
    }

    void visit (const component_type_t& type)
    {
      type.field_list ()->accept (*this);
    }

    void visit (const field_list_type_t& type)
    {
      retval = type.find (identifier);
    }

    void visit (const struct_type_t& type)
    {
      type.field_list ()->accept (*this);
    }

  };
  visitor v (identifier);
  type->accept (v);
  return v.retval;
}

method_t *
type_select_method (const type_t * type, string_t identifier)
{
  struct visitor : public const_type_visitor_t
  {
    method_t* retval;
    string_t identifier;
    visitor (string_t id) : retval (NULL), identifier (id) { }

    void visit (const named_type_t& type)
    {
      retval = type.get_method (identifier);
    }
  };
  visitor v (identifier);
  type->accept (v);
  return v.retval;
}

reaction_t *
type_select_reaction (const type_t * type, string_t identifier)
{
  struct visitor : public const_type_visitor_t
  {
    reaction_t* retval;
    string_t identifier;
    visitor (string_t id) : retval (NULL), identifier (id) { }

    void visit (const named_type_t& type)
    {
      retval = type.get_reaction (identifier);
    }
  };
  visitor v (identifier);
  type->accept (v);
  return v.retval;
}

const type_t*
type_select (const type_t* type, string_t identifier)
{
  field_t* f = type_select_field (type, identifier);
  if (f)
    {
      return field_type (f);
    }

  method_t* m = type_select_method (type, identifier);
  if (m)
    {
      return m->method_type;
    }

  reaction_t* r = type_select_reaction (type, identifier);
  if (r)
    {
      return r->reaction_type ();
    }

  return NULL;
}

parameter_t *
signature_type_t::find (string_t name) const
{
  for (std::vector<parameter_t*>::const_iterator ptr = parameters_.begin (),
         limit = parameters_.end ();
       ptr != limit;
       ++ptr)
    {
      if (streq (parameter_name ((*ptr)), name))
        {
          return *ptr;
        }
    }
  return NULL;
}

void
signature_type_t::prepend (string_t parameter_name,
                           const type_t * parameter_type, bool is_receiver)
{
  parameter_t *p = parameter_make (parameter_name, parameter_type, is_receiver);
  parameters_.insert (parameters_.begin (), p);
}

void
signature_type_t::append (string_t parameter_name,
                          const type_t * parameter_type, bool is_receiver)
{
  parameter_t *p = parameter_make (parameter_name, parameter_type, is_receiver);
  parameters_.push_back (p);
}

const type_t*
type_move (const type_t* type)
{
  const pointer_to_foreign_type_t* ptf = type_cast<pointer_to_foreign_type_t> (type);
  if (ptf)
    {
      const heap_type_t* h = type_cast<heap_type_t> (ptf->base_type ());
      if (h)
        {
          return pointer_type_t::make (ptf->base_type ());
        }
    }

  return NULL;
}

const type_t* type_merge (const type_t* type)
{
  const pointer_to_foreign_type_t* ptf = type_cast<pointer_to_foreign_type_t> (type);

  if (ptf)
    {
      const heap_type_t* h = type_cast<heap_type_t> (ptf->base_type ());
      if (h)
        {
          return pointer_type_t::make (h->base_type ());
        }
    }

  return NULL;
}

const type_t*
type_change (const type_t* type)
{
  const pointer_type_t* ptf = type_cast<pointer_type_t> (type);

  if (ptf)
    {
      const heap_type_t* h = type_cast<heap_type_t> (ptf->base_type ());
      if (h)
        {
          return pointer_type_t::make (h->base_type ());
        }
    }

  return false;
}

void
field_list_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
iota_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
named_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
component_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
struct_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
port_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
bool_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
function_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
method_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
heap_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
pointer_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
pointer_to_foreign_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
reaction_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
signature_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
string_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
int_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
uint_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
void_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
nil_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
array_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
pointer_to_immutable_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

// Returns true if two types are equal.
// If one type is a named type, then the other must be the same named type.
// Otherwise, the types must have the same structure.
bool
type_is_equal (const type_t * x, const type_t* y)
{
  if (x == y)
    {
      return true;
    }

  if (type_cast<named_type_t> (x) || type_cast<named_type_t> (y))
    {
      // Named types must be exactly the same.
      return false;
    }

  struct visitor : public const_type_visitor_t
  {
    bool flag;
    const type_t* other;

    visitor (const type_t* o) : flag (false), other (o) { }

    void visit (const pointer_type_t& type)
    {
      const pointer_type_t* t = type_cast<pointer_type_t> (other);
      if (t != NULL)
        {
          flag = type_is_equal (type.base_type (), t->base_type ());
        }
    }

    void visit (const pointer_to_immutable_type_t& type)
    {
      const pointer_to_immutable_type_t* t = type_cast<pointer_to_immutable_type_t> (other);
      if (t != NULL)
        {
          flag = type_is_equal (type.base_type (), t->base_type ());
        }
    }

    void visit (const pointer_to_foreign_type_t& type)
    {
      const pointer_to_foreign_type_t* t = type_cast<pointer_to_foreign_type_t> (other);
      if (t != NULL)
        {
          flag = type_is_equal (type.base_type (), t->base_type ());
        }
    }

    void visit (const heap_type_t& type)
    {
      const heap_type_t* t = type_cast<heap_type_t> (other);
      if (t != NULL)
        {
          flag = type_is_equal (type.base_type (), t->base_type ());
        }
    }

    void visit (const bool_type_t& type)
    {
      flag = &type == other;
    }

    void visit (const uint_type_t& type)
    {
      flag = &type == other;
    }

    void visit (const int_type_t& type)
    {
      flag = &type == other;
    }

    void visit (const nil_type_t& type)
    {
      flag = &type == other;
    }

    void visit (const signature_type_t& type)
    {
      const signature_type_t* x = &type;
      const signature_type_t* y = type_cast<signature_type_t> (other);
      if (y)
        {
          size_t x_arity = x->arity ();
          size_t y_arity = y->arity ();
          if (x_arity != y_arity)
            {
              return;
            }

          for (size_t idx = 0; idx != x_arity; ++idx)
            {
              parameter_t *x_parameter = x->at (idx);
              const type_t *x_parameter_type = parameter_type (x_parameter);
              parameter_t *y_parameter = y->at (idx);
              const type_t *y_parameter_type = parameter_type (y_parameter);

              if (!type_is_equal (x_parameter_type, y_parameter_type))
                {
                  return;
                }
            }

          flag = true;
        }
    }

    void visit (const array_type_t& type)
    {
      const array_type_t* x = &type;
      const array_type_t* y = type_cast<array_type_t> (other);
      if (y)
        {
          flag = type_is_equal (x->base_type (), y->base_type ());
        }
    }
  };

  visitor v (y);
  x->accept (v);
  return v.flag;
}

bool
type_is_convertible (const type_t * to, const type_t* from)
{
  if (to == from)
    {
      return true;
    }

  if (type_cast<named_type_t> (from))
    {
      // Named types must be exactly the same.
      return false;
    }

  // Strip a named type on to.
  const named_type_t* nt = type_cast<named_type_t> (to);
  if (nt)
    {
      to = nt->subtype ();
    }

  if (type_is_equal (to, from))
    {
      return true;
    }

  // to and from are not equal and neither refere to a named type.

  struct visitor : public const_type_visitor_t
  {
    bool flag;
    const type_t* from;

    visitor (const type_t* o) : flag (false), from (o) { }

    void visit (const pointer_type_t& type)
    {
      if (type_cast<nil_type_t> (from))
        {
          flag = true;
        }
    }

    void visit (const pointer_to_immutable_type_t& type)
    {
      if (type_cast<nil_type_t> (from))
        {
          flag = true;
        }

      {
        const pointer_type_t* p = type_cast<pointer_type_t> (from);
        if (p)
          {
            flag = type_is_equal (type.base_type (), p->base_type ());
          }
      }
    }

    void visit (const pointer_to_foreign_type_t& type)
    {
      if (type_cast<nil_type_t> (from))
        {
          flag = true;
        }

      {
        const pointer_type_t* p = type_cast<pointer_type_t> (from);
        if (p)
          {
            flag = type_is_equal (type.base_type (), p->base_type ());
          }
      }

      {
        const pointer_to_immutable_type_t* p = type_cast<pointer_to_immutable_type_t> (from);
        if (p)
          {
            flag = type_is_equal (type.base_type (), p->base_type ());
          }
      }
    }

    void visit (const uint_type_t& type)
    {
      flag = type_cast<iota_type_t> (from) != NULL;
    }

    void visit (const int_type_t& type)
    {
      flag = type_cast<iota_type_t*> (from) != NULL;
    }
  };

  visitor v (from);
  to->accept (v);
  return v.flag;
}

bool type_is_immutable_safe (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    bool flag;

    visitor () : flag (false) { }

    void default_action (const type_t& type)
    {
      not_reached;
    }

    void visit (const named_type_t& type)
    {
      type.subtype ()->accept (*this);
    }

    void visit (const bool_type_t& type)
    {
      flag = true;
    }

    void visit (const uint_type_t& type)
    {
      flag = true;
    }

    void visit (const pointer_type_t& type)
    {
      // Do nothing.
    }

    // void visit (const pointer_to_immutable_type_t& type)
    // {
    //   flag = true;
    // }

    // void visit (const pointer_to_foreign_type_t& type)
    // {
    //   flag = true;
    // }

    // void visit (const component_type_t& type)
    // {
    //   type.field_list ()->accept (*this);
    // }

    void visit (const struct_type_t& type)
    {
      type.field_list ()->accept (*this);
    }

    void visit (const field_list_type_t& type)
    {
      for (std::vector<field_t*>::const_iterator pos = type.begin (),
             limit = type.end ();
           pos != limit;
           ++pos)
        {
          field_t* field = *pos;
          if (!type_is_immutable_safe (field_type (field)))
            {
              return;
            }
        }

      flag = true;
    }
  };
  visitor v;
  type->accept (v);
  return v.flag;
}

std::string
signature_type_t::to_string () const
{
  std::stringstream str;
  str << '(';
  bool flag = false;
  for (std::vector<parameter_t*>::const_iterator ptr = parameters_.begin (),
         limit = parameters_.end ();
       ptr != limit;
       ++ptr)
    {
      if (flag)
        {
          str << ", ";
        }

      str << parameter_type (*ptr)->to_string ();
      flag = true;
    }
  str << ')';
  return str.str ();
}

bool type_is_foreign_safe (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    bool flag;

    visitor () : flag (false) { }

    void default_action (const type_t& type)
    {
      not_reached;
    }

    void visit (const named_type_t& type)
    {
      type.subtype ()->accept (*this);
    }

    void visit (const bool_type_t& type)
    {
      flag = true;
    }

    void visit (const uint_type_t& type)
    {
      flag = true;
    }

    void visit (const pointer_type_t& type)
    {
      // Do nothing.
    }

    // void visit (const pointer_to_immutable_type_t& type)
    // {
    //   flag = true;
    // }

    void visit (const pointer_to_foreign_type_t& type)
    {
      flag = true;
    }

    // void visit (const component_type_t& type)
    // {
    //   type.field_list ()->accept (*this);
    // }

    // void visit (const struct_type_t& type)
    // {
    //   type.field_list ()->accept (*this);
    // }

    // void visit (const field_list_type_t& type)
    // {
    //   for (std::vector<field_t*>::const_iterator pos = type.begin (),
    //          limit = type.end ();
    //        pos != limit;
    //        ++pos)
    //     {
    //       field_t* field = *pos;
    //       if (!type_is_immutable_safe (field_type (field)))
    //         {
    //           return;
    //         }
    //     }

    //   flag = true;
    // }

    void visit (const signature_type_t& type)
    {
      for (signature_type_t::ParametersType::const_iterator pos = type.begin (),
             limit = type.end ();
           pos != limit;
           ++pos)
        {
          parameter_t* parameter = *pos;
          if (!type_is_foreign_safe (parameter_type (parameter)))
            {
              return;
            }
        }

      flag = true;
    }
  };
  visitor v;
  type->accept (v);
  return v.flag;
}

const void_type_t*
void_type_t::instance ()
{
  static void_type_t i;
  return &i;
}

const bool_type_t*
bool_type_t::instance ()
{
  static bool_type_t i;
  return &i;
}

const int_type_t*
int_type_t::instance ()
{
  static int_type_t i;
  return &i;
}

const uint_type_t*
uint_type_t::instance ()
{
  static uint_type_t i;
  return &i;
}

const string_type_t*
string_type_t::instance ()
{
  static string_type_t i;
  return &i;
}

const type_t*
pointer_type_t::make (const type_t* base_type)
{
  return new pointer_type_t (base_type);
}

const type_t*
pointer_to_immutable_type_t::make (const type_t* base_type)
{
  return new pointer_to_immutable_type_t (base_type);
}

const type_t*
pointer_to_foreign_type_t::make (const type_t* base_type)
{
  return new pointer_to_foreign_type_t (base_type);
}

field_list_type_t::field_list_type_t (bool insert_runtime) : offset_ (0), alignment_ (0)
{
  if (insert_runtime)
    {
      /* Prepend the field list with a pointer for the runtime. */
      append (enter ("0"), pointer_type_t::make (void_type_t::instance ()));
    }
}

const nil_type_t*
nil_type_t::instance ()
{
  static nil_type_t i;
  return &i;
}

const type_t* type_dereference (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    const type_t* retval;

    visitor () : retval (NULL) { }

    void visit (const pointer_type_t& type)
    {
      retval = type.base_type ();
    }

    void visit (const pointer_to_immutable_type_t& type)
    {
      retval = type.base_type ();
    }

    void visit (const pointer_to_foreign_type_t& type)
    {
      retval = type.base_type ();
    }
  };
  visitor v;
  type->accept (v);
  return v.retval;
}

bool
type_contains_pointer_to_foreign (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    bool flag;

    visitor () : flag (false) { }

    void default_action (const type_t& type)
    {
      not_reached;
    }

    void visit (const named_type_t& type)
    {
      type.subtype ()->accept (*this);
    }

    void visit (const pointer_to_foreign_type_t& type)
    {
      flag = true;
    }

    void visit (const bool_type_t& type)
    {
    }

    void visit (const uint_type_t& type)
    {
    }

    void visit (const pointer_type_t& type)
    {
    }

    void visit (const struct_type_t& type)
    {
      type.field_list ()->accept (*this);
    }

    void visit (const field_list_type_t& type)
    {
      for (std::vector<field_t*>::const_iterator pos = type.begin (),
             limit = type.end ();
           pos != limit;
           ++pos)
        {
          field_t* field = *pos;
          if (type_contains_pointer_to_foreign (field_type (field)))
            {
              flag = true;
              return;
            }
        }
    }
  };
  visitor v;
  type->accept (v);
  return v.flag;
}

named_type_t::named_type_t (string_t name,
                            const type_t* subtype)
  : name_ (name)
{
  // Don't chain named types.
  const named_type_t* t = type_cast<named_type_t> (subtype);
  if (t)
    {
      subtype_ = t->subtype ();
    }
  else
    {
      subtype_ = subtype;
    }
}

bool
type_is_boolean (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    bool flag;
    visitor () : flag (false) { }

    void visit (const named_type_t& type)
    {
      type.subtype ()->accept (*this);
    }

    void visit (const bool_type_t& type)
    {
      flag = true;
    }
  };
  visitor v;
  type->accept (v);
  return v.flag;
}

bool
type_is_arithmetic (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    bool flag;
    visitor () : flag (false) { }

    void visit (const named_type_t& type)
    {
      type.subtype ()->accept (*this);
    }

    void visit (const int_type_t& type)
    {
      flag = true;
    }

    void visit (const uint_type_t& type)
    {
      flag = true;
    }
  };
  visitor v;
  type->accept (v);
  return v.flag;
}

bool
type_is_comparable (const type_t* type)
{
 struct visitor : public const_type_visitor_t
  {
    bool flag;
    visitor () : flag (false) { }

    void visit (const pointer_type_t& type)
    {
      flag = true;
    }

    void visit (const named_type_t& type)
    {
      type.subtype ()->accept (*this);
    }

    void visit (const int_type_t& type)
    {
      flag = true;
    }

    void visit (const uint_type_t& type)
    {
      flag = true;
    }
  };
  visitor v;
  type->accept (v);
  return v.flag;
}

const type_t*
type_strip (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    const type_t* retval;
    visitor (const type_t* t) : retval (t) { }

    void visit (const named_type_t& type)
    {
      retval = type.subtype ();
    }
  };
  visitor v (type);
  type->accept (v);
  return v.retval;
}

const type_t*
type_index (const type_t* base, const type_t* index)
{
  struct visitor : public const_type_visitor_t
  {
    const type_t* index;
    const type_t* result;

    visitor (const type_t* i) : index (i), result (NULL) { }

    void visit (const array_type_t& type)
    {
      struct visitor : public const_type_visitor_t
      {
        const array_type_t& array_type;
        const type_t* result;

        visitor (const array_type_t& at) : array_type (at), result (NULL) { }

        void visit (const named_type_t& type)
        {
          type.subtype ()->accept (*this);
        }

        void visit (const uint_type_t& type)
        {
          result = array_type.base_type ();
        }

        void visit (const int_type_t& type)
        {
          result = array_type.base_type ();
        }

        void visit (const iota_type_t& type)
        {
          if (type.bound () <= array_type.dimension ())
            {
              result = array_type.base_type ();
            }
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
type_is_index (const type_t* type, int64_t index)
{
  struct visitor : public const_type_visitor_t
  {
    int64_t index;
    bool flag;

    visitor (int64_t i) : index (i), flag (false) { }

    void visit (const array_type_t& type)
    {
      flag = index >= 0 && static_cast<size_t> (index) < type.dimension ();
    }
  };
  visitor v (index);
  type->accept (v);
  return v.flag;
}
