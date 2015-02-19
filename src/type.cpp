#include "type.hpp"
#include "action.hpp"
#include "parameter.hpp"
#include "field.hpp"
#include "method.hpp"

method_t*
named_type_t::add_method (ast_t* node,
                          string_t identifier,
                          const signature_type_t* signature,
                          const type_t* return_type)
{
  method_t *g =
    method_make (this,
                 node,
                 identifier, signature, return_type);
  this->methods_.push_back (g);
  return g;
}

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
      if (streq (method_name (a), identifier))
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

    void visit (const immutable_type_t& type)
    {
      type.base_type ()->accept (*this);
    }

    void visit (const foreign_type_t& type)
    {
      type.base_type ()->accept (*this);
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

    void visit (const immutable_type_t& type)
    {
      type.base_type ()->accept (*this);
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
      return method_type (m);
    }

  reaction_t* r = type_select_reaction (type, identifier);
  if (r)
    {
      return r->reaction_type ();
    }

  return NULL;
}

const pointer_type_t*
type_to_pointer (const type_t * type)
{
  return dynamic_cast<const pointer_type_t*> (type);
}

const pointer_to_immutable_type_t*
type_to_pointer_to_immutable (const type_t * type)
{
  return dynamic_cast<const pointer_to_immutable_type_t*> (type);
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

size_t
type_parameter_count (const type_t * type)
{
  struct visitor : public const_type_visitor_t
  {
    size_t count;
    visitor () : count (0) { }

    void visit (const func_type_t& type)
    {
      type.signature ()->accept (*this);
    }

    void visit (const immutable_type_t& type)
    {
      type.base_type ()->accept (*this);
    }

    void visit (const port_type_t& type)
    {
      type.signature ()->accept (*this);
    }

    void visit (const signature_type_t& type)
    {
      count = type.arity ();
    }
  };
  visitor v;
  type->accept (v);
  return v.count;
}

const type_t *
type_parameter_type (const type_t * type, size_t idx)
{
  struct visitor : public const_type_visitor_t
  {
    const type_t* retval;
    size_t idx;
    visitor (size_t i) : retval (NULL), idx (i) { }

    void visit (const func_type_t& type)
    {
      type.signature ()->accept (*this);
    }

    void visit (const immutable_type_t& type)
    {
      type.base_type ()->accept (*this);
    }

    void visit (const port_type_t& type)
    {
      type.signature ()->accept (*this);
    }

    void visit (const signature_type_t& type)
    {
      retval = parameter_type (type.at (idx));
    }
  };
  visitor v (idx);
  type->accept (v);
  return v.retval;
}

const type_t *
type_return_type (const type_t * type)
{
  struct visitor : public const_type_visitor_t
  {
    const type_t* retval;
    visitor () : retval (NULL) { }

    void visit (const func_type_t& type)
    {
      retval = type.return_type ();
    }

    void visit (const immutable_type_t& type)
    {
      retval = type_return_type (type.base_type ());
    }

    void visit (const port_type_t& type)
    {
      retval = void_type_t::instance ();
    }
  };
  visitor v;
  type->accept (v);
  return v.retval;
}

const reaction_type_t*
type_to_reaction (const type_t * type)
{
  return dynamic_cast<const reaction_type_t*> (type);
}

const signature_type_t*
type_to_signature (const type_t * type)
{
  return dynamic_cast<const signature_type_t*> (type);
}

bool
type_is_compatible_port_reaction (const port_type_t * port, const reaction_type_t * reaction)
{
  const signature_type_t *port_signature = port->signature ();
  const signature_type_t *reaction_signature = reaction->signature ();

  size_t port_arity = port_signature->arity ();
  size_t reaction_arity = reaction_signature->arity ();
  if (port_arity != reaction_arity)
    {
      return false;
    }

  size_t idx;
  for (idx = 0; idx != port_arity; ++idx)
    {
      parameter_t *port_parameter = port_signature->at (idx);
      const type_t *port_parameter_type = parameter_type (port_parameter);
      parameter_t *reaction_parameter = reaction_signature->at (idx);
      const type_t *reaction_parameter_type = parameter_type (reaction_parameter);

      if (!type_is_convertible (port_parameter_type, reaction_parameter_type))
        {
          return false;
        }
    }

  return true;
}

const immutable_type_t* type_to_immutable (const type_t* type)
{
  return dynamic_cast<const immutable_type_t*> (type);
}

const foreign_type_t* type_to_foreign (const type_t* type)
{
  return dynamic_cast<const foreign_type_t*> (type);
}

const heap_type_t*
type_to_heap (const type_t* type)
{
  return dynamic_cast<const heap_type_t*> (type);
}

bool type_is_any_pointer (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    bool flag;
    visitor () : flag (false) { }
    void visit (const pointer_type_t&) { flag = true; }
    void visit (const pointer_to_immutable_type_t&) { flag = true; }
    void visit (const pointer_to_foreign_type_t&) { flag = true; }
  };
  visitor v;
  type->accept (v);
  return v.flag;
}

bool type_is_assignable (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    bool flag;
    visitor () : flag (false) { }
    void visit (const bool_type_t&) { flag = true; }
    void visit (const field_list_type_t& type)
    {
      for (std::vector<field_t*>::const_iterator pos = type.begin (),
             limit = type.end ();
           pos != limit;
           ++pos)
        {
          field_t* field = *pos;
          if (!type_is_assignable (field_type (field)))
            {
              flag = false;
              return;
            }
        }

      flag = true;
    }
    void visit (const named_type_t& type) { type.subtype ()->accept (*this); }
    void visit (const pointer_type_t&) { flag = true; }
    void visit (const struct_type_t& type) { type.field_list ()->accept (*this); }
    void visit (const uint_type_t&) { flag = true; }
  };
  visitor v;
  type->accept (v);
  return v.flag;
}

const bool_type_t*
type_to_bool (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    const bool_type_t* retval;
    visitor () : retval (NULL) { }
    void visit (const named_type_t& type) { type.subtype ()->accept (*this); }
    void visit (const bool_type_t& type) { retval = &type; }
    void visit (const immutable_type_t& type) { type.base_type ()->accept (*this); }
  };
  visitor v;
  type->accept (v);
  return v.retval;
}

const type_t*
type_move (const type_t* type)
{
  const pointer_to_foreign_type_t* ptf = dynamic_cast<const pointer_to_foreign_type_t*> (type);
  if (ptf)
    {
      const heap_type_t* h = dynamic_cast<const heap_type_t*> (ptf->base_type ());
      if (h)
        {
          return pointer_type_t::make (ptf->base_type ());
        }
    }

  return NULL;
}

const type_t* type_merge (const type_t* type)
{
  const pointer_to_foreign_type_t* ptf = dynamic_cast<const pointer_to_foreign_type_t*> (type);

  if (ptf)
    {
      const heap_type_t* h = dynamic_cast<const heap_type_t*> (ptf->base_type ());
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
  const pointer_type_t* ptf = dynamic_cast<const pointer_type_t*> (type);

  if (ptf)
    {
      const heap_type_t* h = dynamic_cast<const heap_type_t*> (ptf->base_type ());
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
untyped_iota_type_t::accept (const_type_visitor_t& visitor) const
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
foreign_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
func_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
heap_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
immutable_type_t::accept (const_type_visitor_t& visitor) const
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
untyped_boolean_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
untyped_integer_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
untyped_nil_type_t::accept (const_type_visitor_t& visitor) const
{
  visitor.visit (*this);
}

void
untyped_string_type_t::accept (const_type_visitor_t& visitor) const
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

const component_type_t* type_to_component (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    const component_type_t* retval;
    visitor () : retval (NULL) { }

    void visit (const component_type_t& type) { retval = &type; }
    void visit (const named_type_t& type) { type.subtype ()->accept (*this); }
    void visit (const foreign_type_t& type) { type.base_type ()->accept (*this); }
    void visit (const immutable_type_t& type) { type.base_type ()->accept (*this); }
  };

  visitor v;
  type->accept (v);
  return v.retval;
}

const array_type_t* type_to_array (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    const array_type_t* retval;
    visitor () : retval (NULL) { }

    void visit (const array_type_t& type) { retval = &type; }
    void visit (const named_type_t& type) { type.subtype ()->accept (*this); }
    void visit (const foreign_type_t& type) { type.base_type ()->accept (*this); }
    void visit (const immutable_type_t& type) { type.base_type ()->accept (*this); }
  };

  visitor v;
  type->accept (v);
  return v.retval;
}

// Returns true if two types are equal.
// If one type is a named type, then the other must be the same named type.
// Otherwise, the types must have the same structure.
bool
type_is_equal (const type_t * x, const type_t* y)
{
  if (dynamic_cast<const named_type_t*> (x) || dynamic_cast<const named_type_t*> (y))
    {
      // Named types must be exactly the same.
      return x == y;
    }

  struct visitor : public const_type_visitor_t
  {
    bool flag;
    const type_t* other;

    visitor (const type_t* o) : flag (false), other (o) { }

    void visit (const pointer_type_t& type)
    {
      const pointer_type_t* t = dynamic_cast<const pointer_type_t*> (other);
      if (t != NULL)
        {
          flag = type_is_equal (type.base_type (), t->base_type ());
        }
    }

    void visit (const pointer_to_immutable_type_t& type)
    {
      const pointer_to_immutable_type_t* t = dynamic_cast<const pointer_to_immutable_type_t*> (other);
      if (t != NULL)
        {
          flag = type_is_equal (type.base_type (), t->base_type ());
        }
    }

    void visit (const pointer_to_foreign_type_t& type)
    {
      const pointer_to_foreign_type_t* t = dynamic_cast<const pointer_to_foreign_type_t*> (other);
      if (t != NULL)
        {
          flag = type_is_equal (type.base_type (), t->base_type ());
        }
    }

    void visit (const heap_type_t& type)
    {
      const heap_type_t* t = dynamic_cast<const heap_type_t*> (other);
      if (t != NULL)
        {
          flag = type_is_equal (type.base_type (), t->base_type ());
        }
    }

    void visit (const immutable_type_t& type)
    {
      const immutable_type_t* t = dynamic_cast<const immutable_type_t*> (other);
      if (t != NULL)
        {
          flag = type_is_equal (type.base_type (), t->base_type ());
        }
    }

    void visit (const bool_type_t& type)
    {
      flag = dynamic_cast<const bool_type_t*> (other) != NULL;
    }

    void visit (const uint_type_t& type)
    {
      flag = dynamic_cast<const uint_type_t*> (other) != NULL;
    }

    void visit (const int_type_t& type)
    {
      flag = dynamic_cast<const int_type_t*> (other) != NULL;
    }

    void visit (const untyped_boolean_type_t& type)
    {
      flag = dynamic_cast<const untyped_boolean_type_t*> (other) != NULL;
    }

    void visit (const untyped_integer_type_t& type)
    {
      flag = dynamic_cast<const untyped_integer_type_t*> (other) != NULL;
    }
  };

  visitor v (y);
  x->accept (v);
  return v.flag;
}

// Strips off immutable/foreign.
const type_t*
type_strip (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    const type_t* retval;
    visitor () : retval (NULL) { }

    void visit (const foreign_type_t& type)
    {
      type.base_type ()->accept (*this);
    }
    void visit (const immutable_type_t& type)
    {
      type.base_type ()->accept (*this);
    }
    void default_action (const type_t& type)
    {
      retval = &type;
    }
  };

  visitor v;
  type->accept (v);
  return v.retval;
}

static bool type_is_immutable_safe (const type_t* type)
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

    void visit (const uint_type_t& type)
    {
      flag = true;
    }

    void visit (const immutable_type_t& type)
    {
      flag = true;
    }

    void visit (const foreign_type_t& type)
    {
      flag = true;
    }

    void visit (const pointer_type_t& type)
    {
      if (type_to_immutable (type.base_type ()) ||
          type_to_foreign (type.base_type ()))
        {
          flag = true;
        }
    }

    void visit (const pointer_to_immutable_type_t& type)
    {
      flag = true;
    }

    void visit (const pointer_to_foreign_type_t& type)
    {
      flag = true;
    }

    void visit (const component_type_t& type)
    {
      type.field_list ()->accept (*this);
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

// Strips off immuable/foreign if safe to do so.
static const type_t* safe_strip (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    const type_t* retval;

    visitor () : retval (NULL) { }

    void visit (const foreign_type_t& type)
    {
      if (type_is_foreign_safe (type.base_type ()))
        {
          type.base_type ()->accept (*this);
        }
      else
        {
          retval = &type;
        }
    }

    void visit (const immutable_type_t& type)
    {
      if (type_is_immutable_safe (type.base_type ()))
        {
          type.base_type ()->accept (*this);
        }
      else
        {
          retval = &type;
        }
    }

    void default_action (const type_t& type)
    {
      retval = &type;
    }
  };

  visitor v;
  type->accept (v);
  return v.retval;
}

static bool convertible (const type_t * to, const type_t * from)
{
  if (type_is_equal (to, from))
    {
      return true;
    }

  // The types are not equal.  Conversion is necessary.

  struct visitor : public const_type_visitor_t
  {
    bool flag;
    const type_t* from;
    visitor (const type_t* f) : flag (false), from (f) { }

    void visit (const named_type_t& type)
    {
      // Only allow conversions from untyped.
      if (type_is_untyped (from))
        {
          type.subtype ()->accept (*this);
        }
    }

    void visit (const bool_type_t& type)
    {
      // Only allow conversions from untyped.
      flag = dynamic_cast<const untyped_boolean_type_t*> (from) != NULL;
    }

    void visit (const uint_type_t& type)
    {
      // Only allow conversions from untyped.
      flag =
        (dynamic_cast<const untyped_integer_type_t*> (from) != NULL) ||
        (dynamic_cast<const untyped_iota_type_t*> (from) != NULL);
    }

    void visit (const string_type_t& type)
    {
      // Only allow conversions from untyped.
      flag = dynamic_cast<const untyped_string_type_t*> (from) != NULL;
    }

    void visit (const pointer_type_t& type)
    {
      // Only allow conversions from untyped.
      if (dynamic_cast<const untyped_nil_type_t*> (from) != NULL)
        {
          flag = true;
        }
    }

    void visit (const pointer_to_immutable_type_t& type)
    {
      // Converting to a pointer.
      if (dynamic_cast<const untyped_nil_type_t*> (from) != NULL)
        {
          flag = true;
        }
      else if (dynamic_cast<const pointer_type_t*> (from) != NULL)
        {
          flag = type_is_equal (type.base_type (), dynamic_cast<const pointer_type_t*> (from)->base_type ());
        }
    }

    void visit (const pointer_to_foreign_type_t& type)
    {
      // Converting to a pointer.
      if (dynamic_cast<const untyped_nil_type_t*> (from) != NULL)
        {
          flag = true;
        }
      else if (dynamic_cast<const pointer_type_t*> (from) != NULL)
        {
          flag = type_is_equal (type.base_type (), dynamic_cast<const pointer_type_t*> (from)->base_type ());
        }
      else if (dynamic_cast<const pointer_to_immutable_type_t*> (from) != NULL)
        {
          flag = type_is_equal (type.base_type (), dynamic_cast<const pointer_to_immutable_type_t*> (from)->base_type ());
        }
    }

    void visit (const untyped_integer_type_t& type)
    {
      flag = dynamic_cast<const untyped_iota_type_t*> (from) != NULL;
    }

  };
  visitor v (from);
  to->accept (v);
  return v.flag;
}

bool
type_is_convertible (const type_t * from, const type_t * to)
{
  // We can always convert to immutable or foreign so strip them off.
  to = type_strip (to);

  // Check if we can strip immutable/foreign from from.
  from = safe_strip (from);

  return convertible (to, from);
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

    void visit (const immutable_type_t& type)
    {
      type.base_type ()->accept (*this);
    }

    void visit (const foreign_type_t& type)
    {
      flag = true;
    }

    void visit (const pointer_type_t& type)
    {
      if (type_to_foreign (type.base_type ()))
        {
          flag = true;
        }
    }

    void visit (const pointer_to_immutable_type_t& type)
    {
      if (type_to_foreign (type.base_type ()))
        {
          flag = true;
        }
    }

    void visit (const pointer_to_foreign_type_t& type)
    {
      flag = true;
    }

    void visit (const component_type_t& type)
    {
      type.field_list ()->accept (*this);
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
          if (!type_is_immutable_safe (field_type (field)))
            {
              return;
            }
        }

      flag = true;
    }

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

bool
type_is_callable (const type_t * type)
{
  struct visitor : public const_type_visitor_t
  {
    bool flag;

    visitor () : flag (false) { }

    void visit (const func_type_t& type)
    {
      flag = true;
    }

    void visit (const immutable_type_t& type)
    {
      type.base_type ()->accept (*this);
    }
  };
  visitor v;
  type->accept (v);
  return v.flag;
}

bool type_is_untyped (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    bool flag;

    visitor () : flag (false) { }

    void visit (const untyped_boolean_type_t& type)
    {
      flag = true;
    }

    void visit (const untyped_integer_type_t& type)
    {
      flag = true;
    }

    void visit (const untyped_nil_type_t& type)
    {
      flag = true;
    }

    void visit (const untyped_string_type_t& type)
    {
      flag = true;
    }

    void visit (const untyped_iota_type_t& type)
    {
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
  {
    const immutable_type_t* type = dynamic_cast<const immutable_type_t*> (base_type);
    if (type)
      {
        return pointer_to_immutable_type_t::make (type->base_type ());
      }
  }
  {
    const foreign_type_t* type = dynamic_cast<const foreign_type_t*> (base_type);
    if (type)
      {
        return pointer_to_foreign_type_t::make (type->base_type ());
      }
  }
  return new pointer_type_t (base_type);
}

const type_t*
pointer_to_immutable_type_t::make (const type_t* base_type)
{
  {
    const immutable_type_t* type = dynamic_cast<const immutable_type_t*> (base_type);
    if (type)
      {
        return new pointer_to_immutable_type_t (type->base_type ());
      }
  }
  {
    const foreign_type_t* type = dynamic_cast<const foreign_type_t*> (base_type);
    if (type)
      {
        return pointer_to_foreign_type_t::make (type->base_type ());
      }
  }
  return new pointer_to_immutable_type_t (base_type);
}

const type_t*
pointer_to_foreign_type_t::make (const type_t* base_type)
{
  {
    const immutable_type_t* type = dynamic_cast<const immutable_type_t*> (base_type);
    if (type)
      {
        return new pointer_to_foreign_type_t (type->base_type ());
      }
  }
  {
    const foreign_type_t* type = dynamic_cast<const foreign_type_t*> (base_type);
    if (type)
      {
        return new pointer_to_foreign_type_t (type->base_type ());
      }
  }
  return new pointer_to_foreign_type_t (base_type);
}

field_list_type_t::field_list_type_t (bool insert_runtime) : offset_ (0), alignment_ (0)
{
  if (insert_runtime)
    {
      /* Prepend the field list with a pointer for the runtime. */
      append (enter ("0"), immutable_type_t::make (pointer_type_t::make (void_type_t::instance ())));
    }
}


const type_t*
immutable_type_t::make (const type_t* base_type)
{
  if (type_to_immutable (base_type) || type_to_foreign (base_type))
    {
      return base_type;
    }

  const pointer_type_t* pt = dynamic_cast<const pointer_type_t*> (base_type);
  if (pt)
    {
      base_type = pointer_to_immutable_type_t::make (pt->base_type ());
    }

  return new immutable_type_t (base_type);
}

const type_t*
foreign_type_t::make (const type_t* base_type)
{
  if (type_to_foreign (base_type))
    {
      return base_type;
    }
  {
    const immutable_type_t* type = dynamic_cast<const immutable_type_t*> (base_type);
    if (type)
      {
        // Strip immutability.
        base_type = type->base_type ();
      }
  }

  return new foreign_type_t (base_type);
}

const untyped_boolean_type_t*
untyped_boolean_type_t::instance ()
{
  static untyped_boolean_type_t i;
  return &i;
}

const untyped_integer_type_t*
untyped_integer_type_t::instance ()
{
  static untyped_integer_type_t i;
  return &i;
}

const untyped_string_type_t*
untyped_string_type_t::instance ()
{
  static untyped_string_type_t i;
  return &i;
}

const untyped_nil_type_t*
untyped_nil_type_t::instance ()
{
  static untyped_nil_type_t i;
  return &i;
}

const port_type_t* type_to_port (const type_t* port)
{
  return dynamic_cast<const port_type_t*> (port);
}

const type_t *type_dereference (const type_t* type)
{
  struct visitor : public const_type_visitor_t
  {
    const type_t* retval;
    visitor () : retval (NULL) { }
    void visit (const immutable_type_t& type)
    {
      retval = type_dereference (type.base_type ());
      if (retval != NULL)
        {
          retval = immutable_type_t::make (retval);
        }
    }
    void visit (const foreign_type_t& type)
    {
      retval = type_dereference (type.base_type ());
      if (retval != NULL)
        {
          retval = foreign_type_t::make (retval);
        }
    }
    void visit (const pointer_type_t& type)
    {
      retval = type.base_type ();
    }
    void visit (const pointer_to_foreign_type_t& type)
    {
      retval = foreign_type_t::make (type.base_type ());
    }
    void visit (const pointer_to_immutable_type_t& type)
    {
      retval = immutable_type_t::make (type.base_type ());
    }
   };
  visitor v;
  type->accept (v);
  return v.retval;
}

const type_t* type_pointer_base_type (const type_t* type)
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
