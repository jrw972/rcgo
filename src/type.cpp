#include "type.hpp"
#include "action.hpp"
#include "parameter.hpp"
#include "field.hpp"
#include "Callable.hpp"

const slice_type_t*
type_t::getSliceType () const
{
    return new slice_type_t (this);
}

reaction_t *
named_type_t::get_reaction (const std::string& identifier) const
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

Method*
named_type_t::get_method (const std::string& identifier) const
{
    for (std::vector<Method*>::const_iterator pos = this->methods_.begin (),
            limit = this->methods_.end ();
            pos != limit;
            ++pos)
        {
            Method* a = *pos;
            if (a->name == identifier)
                {
                    return a;
                }
        }

    return NULL;
}

Initializer*
named_type_t::get_initializer (const std::string& identifier) const
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
named_type_t::get_getter (const std::string& identifier) const
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


void
field_list_type_t::append (const std::string& field_name, const type_t * field_type)
{
    size_t alignment = field_type->alignment ();
    offset_ = util::AlignUp (offset_, alignment);

    field_t *field = new field_t (field_name, field_type, offset_);
    fields_.push_back (field);

    offset_ += field_type->size ();
    if (alignment > alignment_)
        {
            alignment_ = alignment;
        }
}

field_t *
field_list_type_t::find (const std::string& name) const
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
type_select_field (const type_t * type, const std::string& identifier)
{
    struct visitor : public const_type_visitor_t
    {
        field_t* retval;
        const std::string& identifier;
        visitor (const std::string& id) : retval (NULL), identifier (id) { }

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

Method*
type_select_method (const type_t * type, const std::string& identifier)
{
    struct visitor : public const_type_visitor_t
    {
        Method* retval;
        const std::string& identifier;
        visitor (const std::string& id) : retval (NULL), identifier (id) { }

        void visit (const named_type_t& type)
        {
            retval = type.get_method (identifier);
        }
    };
    visitor v (identifier);
    type->accept (v);
    return v.retval;
}

Initializer*
type_select_initializer (const type_t * type, const std::string& identifier)
{
    struct visitor : public const_type_visitor_t
    {
        Initializer* retval;
        const std::string& identifier;
        visitor (const std::string& id) : retval (NULL), identifier (id) { }

        void visit (const named_type_t& type)
        {
            retval = type.get_initializer (identifier);
        }
    };
    visitor v (identifier);
    type->accept (v);
    return v.retval;
}

Getter*
type_select_getter (const type_t * type, const std::string& identifier)
{
    struct visitor : public const_type_visitor_t
    {
        Getter* retval;
        const std::string& identifier;
        visitor (const std::string& id) : retval (NULL), identifier (id) { }

        void visit (const named_type_t& type)
        {
            retval = type.get_getter (identifier);
        }
    };
    visitor v (identifier);
    type->accept (v);
    return v.retval;
}

reaction_t *
type_select_reaction (const type_t * type, const std::string& identifier)
{
    struct visitor : public const_type_visitor_t
    {
        reaction_t* retval;
        const std::string& identifier;
        visitor (const std::string& id) : retval (NULL), identifier (id) { }

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
type_select (const type_t* type, const std::string& identifier)
{
    field_t* f = type_select_field (type, identifier);
    if (f)
        {
            return f->type;
        }

    Method* m = type_select_method (type, identifier);
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

    return NULL;
}

const parameter_t *
signature_type_t::find (const std::string& name) const
{
    for (ParametersType::const_iterator ptr = parameters_.begin (),
            limit = parameters_.end ();
            ptr != limit;
            ++ptr)
        {
            if ((*ptr)->name == name)
                {
                    return *ptr;
                }
        }
    return NULL;
}

const type_t*
type_move (const type_t* type)
{
    const pointer_type_t* ptf = type_cast<pointer_type_t> (type);
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
    const pointer_type_t* ptf = type_cast<pointer_type_t> (type);

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

#define ACCEPT(type) void \
type::accept (const_type_visitor_t& visitor) const \
{ \
  visitor.visit (*this); \
}

ACCEPT(enum_type_t)
ACCEPT(field_list_type_t)
ACCEPT(named_type_t)
ACCEPT(component_type_t)
ACCEPT(struct_type_t)
ACCEPT(push_port_type_t)
ACCEPT(pull_port_type_t)
ACCEPT(bool_type_t)
ACCEPT(function_type_t)
ACCEPT(getter_type_t)
ACCEPT(method_type_t)
ACCEPT(initializer_type_t)
ACCEPT(heap_type_t)
ACCEPT(FileDescriptor_type_t)
ACCEPT(pointer_type_t)
ACCEPT(pointer_const_type_t)
ACCEPT(pointer_foreign_type_t)
ACCEPT(reaction_type_t)
ACCEPT(signature_type_t)
ACCEPT(string_type_t)
ACCEPT(int_type_t)
ACCEPT(int8_type_t)
ACCEPT(int64_type_t)
ACCEPT(uint_type_t)
ACCEPT(uint8_type_t)
ACCEPT(uint16_type_t)
ACCEPT(uint32_type_t)
ACCEPT(uint64_type_t)
ACCEPT(uint128_type_t)
ACCEPT(float64_type_t)
ACCEPT(void_type_t)
ACCEPT(nil_type_t)
ACCEPT(array_type_t)
ACCEPT(slice_type_t)
ACCEPT(slice_const_type_t)
ACCEPT(slice_foreign_type_t)

static bool
structurally_equal (const type_t* x, const type_t* y)
{
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

        void visit (const uint8_type_t& type)
        {
            flag = &type == other;
        }

        void visit (const uint16_type_t& type)
        {
            flag = &type == other;
        }

        void visit (const uint32_type_t& type)
        {
            flag = &type == other;
        }

        void visit (const uint64_type_t& type)
        {
            flag = &type == other;
        }

        void visit (const int_type_t& type)
        {
            flag = &type == other;
        }

        void visit (const int8_type_t& type)
        {
            flag = &type == other;
        }

        void visit (const float64_type_t& type)
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
                            const parameter_t *x_parameter = x->at (idx);
                            const type_t *x_parameter_type = x_parameter->value.type;
                            const parameter_t *y_parameter = y->at (idx);
                            const type_t *y_parameter_type = y_parameter->value.type;

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
                    flag = x->dimension == y->dimension && type_is_equal (x->base_type (), y->base_type ());
                }
        }

        void visit (const slice_type_t& type)
        {
            const slice_type_t* x = &type;
            const slice_type_t* y = type_cast<slice_type_t> (other);
            if (y)
                {
                    flag = type_is_equal (x->base_type (), y->base_type ());
                }
        }

        void visit (const string_type_t& type)
        {
            flag = &type == other;
        }

        void visit (const function_type_t& type)
        {
            const function_type_t* x = &type;
            const function_type_t* y = type_cast<function_type_t> (other);
            if (y)
                {
                    flag = type_is_equal (x->signature (), y->signature ()) &&
                           type_is_equal (x->return_parameter ()->value.type, y->return_parameter ()->value.type) &&
                           x->return_parameter ()->value.is_foreign_safe () && y->return_parameter ()->value.is_foreign_safe ();
                }
        }
    };

    visitor v (y);
    x->accept (v);
    return v.flag;
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

    if (type_cast<named_type_t> (x) && type_cast<named_type_t> (y))
        {
            // Named types must be exactly the same.
            return false;
        }

    return structurally_equal (type_strip (x), type_strip (y));
}

// bool
// type_is_convertible (const type_t * to, const type_t* from)
// {
//   if (to == from)
//     {
//       return true;
//     }

//   if (type_cast<named_type_t> (from))
//     {
//       // Named types must be exactly the same.
//       return false;
//     }

//   // Strip a named type on to.
//   const named_type_t* nt = type_cast<named_type_t> (to);
//   if (nt)
//     {
//       to = nt->subtype ();
//     }

//   if (type_is_equal (to, from))
//     {
//       return true;
//     }

//   // to and from are not equal and neither refer to a named type.

//   struct visitor : public const_type_visitor_t
//   {
//     bool flag;
//     const type_t* from;

//     visitor (const type_t* o) : flag (false), from (o) { }

//     void visit (const pointer_type_t& type)
//     {
//       flag = (type_cast<nil_type_t> (from) != NULL);
//     }

//     void visit (const uint_type_t& type)
//     {
//       flag = (type_cast<iota_type_t> (from) != NULL);
//     }

//     void visit (const int_type_t& type)
//     {
//       flag = (type_cast<iota_type_t> (from) != NULL);
//     }
//   };

//   visitor v (from);
//   to->accept (v);
//   return v.flag;
// }

std::string
signature_type_t::to_string () const
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

            str << (*ptr)->value.type->to_string ();
            flag = true;
        }
    str << ')';
    return str.str ();
}

#define INSTANCE(type) const type* \
type::instance () \
{ \
  static type i; \
  return &i; \
}

INSTANCE(void_type_t)
INSTANCE(bool_type_t)
INSTANCE(int_type_t)
INSTANCE(int8_type_t)
INSTANCE(int64_type_t)
INSTANCE(uint_type_t)
INSTANCE(uint8_type_t)
INSTANCE(uint16_type_t)
INSTANCE(uint32_type_t)
INSTANCE(uint64_type_t)
INSTANCE(uint128_type_t)
INSTANCE(float64_type_t)
INSTANCE(string_type_t)
INSTANCE(nil_type_t)
INSTANCE(FileDescriptor_type_t)

const pointer_type_t*
pointer_type_t::make (const type_t* base_type)
{
    return new pointer_type_t (base_type);
}

field_list_type_t::field_list_type_t (bool insert_runtime) : offset_ (0), alignment_ (0)
{
    if (insert_runtime)
        {
            /* Prepend the field list with a pointer for the runtime. */
            append ("0", pointer_type_t::make (void_type_t::instance ()));
        }
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

        void visit (const pointer_const_type_t& type)
        {
            retval = type.base_type ();
        }
    };
    visitor v;
    type->accept (v);
    return v.retval;
}

bool
type_contains_pointer (const type_t* type)
{
    struct visitor : public const_type_visitor_t
    {
        bool flag;

        visitor () : flag (false) { }

        void default_action (const type_t& type)
        {
            type_not_reached (type);
        }

        void visit (const named_type_t& type)
        {
            type.subtype ()->accept (*this);
        }

        void visit (const bool_type_t& type)
        { }

        void visit (const int_type_t& type)
        { }

        void visit (const int8_type_t& type)
        { }

        void visit (const uint_type_t& type)
        { }

        void visit (const uint8_type_t& type)
        { }

        void visit (const uint16_type_t& type)
        { }

        void visit (const uint32_type_t& type)
        { }

        void visit (const uint64_type_t& type)
        { }

        void visit (const float64_type_t& type)
        { }

        void visit (const enum_type_t& type)
        { }

        void visit (const array_type_t& type)
        {
            type.base_type ()->accept (*this);
        }

        void visit (const slice_type_t& type)
        {
            flag = true;
        }

        void visit (const pointer_type_t& type)
        {
            flag = true;
        }

        void visit (const pointer_const_type_t& type)
        {
            flag = true;
        }

        void visit (const string_type_t& type)
        {
            flag = true;
        }

        void visit (const FileDescriptor_type_t& type)
        {
            flag = true;
        }

        void visit (const struct_type_t& type)
        {
            type.field_list ()->accept (*this);
        }

        void visit (const field_list_type_t& type)
        {
            for (field_list_type_t::const_iterator pos = type.begin (), limit = type.end ();
                    pos != limit;
                    ++pos)
                {
                    (*pos)->type->accept (*this);
                }
        }
    };
    visitor v;
    type->accept (v);
    return v.flag;
}

named_type_t::named_type_t (const std::string& name,
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
type_is_integral (const type_t* type)
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

        void visit (const int8_type_t& type)
        {
            flag = true;
        }

        void visit (const int64_type_t& type)
        {
            flag = true;
        }

        void visit (const uint_type_t& type)
        {
            flag = true;
        }

        void visit (const uint8_type_t& type)
        {
            flag = true;
        }

        void visit (const uint32_type_t& type)
        {
            flag = true;
        }

        void visit (const uint64_type_t& type)
        {
            flag = true;
        }

        void visit (const uint128_type_t& type)
        {
            flag = true;
        }
    };
    visitor v;
    type->accept (v);
    return v.flag;
}

bool
type_is_unsigned_integral (const type_t* type)
{
    struct visitor : public const_type_visitor_t
    {
        bool flag;
        visitor () : flag (false) { }

        void visit (const named_type_t& type)
        {
            type.subtype ()->accept (*this);
        }

        void visit (const uint_type_t& type)
        {
            flag = true;
        }

        void visit (const uint8_type_t& type)
        {
            flag = true;
        }
    };
    visitor v;
    type->accept (v);
    return v.flag;
}

bool
type_is_floating (const type_t* type)
{
    struct visitor : public const_type_visitor_t
    {
        bool flag;
        visitor () : flag (false) { }

        void visit (const named_type_t& type)
        {
            type.subtype ()->accept (*this);
        }

        void visit (const float64_type_t& type)
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

        void visit (const int8_type_t& type)
        {
            flag = true;
        }

        void visit (const int64_type_t& type)
        {
            flag = true;
        }

        void visit (const uint_type_t& type)
        {
            flag = true;
        }

        void visit (const uint8_type_t& type)
        {
            flag = true;
        }

        void visit (const uint32_type_t& type)
        {
            flag = true;
        }

        void visit (const uint64_type_t& type)
        {
            flag = true;
        }

        void visit (const enum_type_t& type)
        {
            flag = true;
        }
    };
    visitor v;
    type->accept (v);
    return v.flag;
}

bool
type_is_orderable (const type_t* type)
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

        void visit (const int8_type_t& type)
        {
            flag = true;
        }

        void visit (const uint_type_t& type)
        {
            flag = true;
        }

        void visit (const enum_type_t& type)
        {
            flag = true;
        }
    };
    visitor v;
    type->accept (v);
    return v.flag;
}

bool
type_is_pointer_compare (const type_t* left, const type_t* right)
{
    const type_t* l = type_strip (left);
    const type_t* r = type_strip (right);

    return
        (type_cast<pointer_type_t> (l) && type_cast<nil_type_t>(r)) ||
        (type_cast<nil_type_t> (l) && type_cast<pointer_type_t> (r));

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
    if (type == v.retval)
        {
            return type;
        }
    else
        {
            return type_strip (v.retval);
        }
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
type_is_index (const type_t* type, int_type_t::ValueType index)
{
    struct visitor : public const_type_visitor_t
    {
        int_type_t::ValueType index;
        bool flag;

        visitor (int_type_t::ValueType i) : index (i), flag (false) { }

        void visit (const array_type_t& type)
        {
            flag = index >= 0 && index < type.dimension;
        }
    };
    visitor v (index);
    type->accept (v);
    return v.flag;
}

bool
type_is_castable (const type_t* x, const type_t* y)
{
    return type_is_numeric (x) && type_is_numeric (y);
}

std::string
function_type_t::to_string () const
{
    std::stringstream str;
    str << "func " << *signature () << ' ' << *return_parameter ()->value.type;
    return str.str ();
}

std::string
method_type_t::to_string () const
{
    std::stringstream str;
    str << '(' << *receiver_type << ')' << " func " << *signature << ' ' << *function_type->return_parameter ()->value.type;
    return str.str ();
}

std::string
getter_type_t::to_string () const
{
    std::stringstream str;
    str << '(' << *receiver_type << ')' << " getter " << *signature << ' ' << *function_type->return_parameter ()->value.type;
    return str.str ();
}

std::string
initializer_type_t::to_string () const
{
    std::stringstream str;
    str << '(' << *receiver_type << ')' << " init " << *signature;
    return str.str ();
}

std::string
pull_port_type_t::to_string () const
{
    std::stringstream str;
    str << "pull " << *signature () << ' ' << *return_parameter ()->value.type;
    return str.str ();
}

parameter_t*
method_base::make_this_parameter (const std::string& this_name,
                                  const type_t* receiver_type,
                                  Mutability dereference_mutability)
{
    typed_value_t this_value = typed_value_t::make_value (receiver_type, typed_value_t::STACK, MUTABLE, dereference_mutability);
    return new parameter_t (NULL, this_name, this_value, true);
}

function_type_t*
method_base::make_function_type (const parameter_t* this_parameter,
                                 const signature_type_t* signature,
                                 const parameter_t* return_parameter)
{
    signature_type_t* sig = new signature_type_t ();

    sig->append (this_parameter);
    for (signature_type_t::const_iterator pos = signature->begin (),
            limit = signature->end ();
            pos != limit;
            ++pos)
        {
            sig->append (*pos);
        }
    return new function_type_t (sig, return_parameter);
}

const type_t*
function_type_t::return_type () const
{
    return return_parameter_->value.type;
}

const type_t*
method_base::return_type () const
{
    return return_parameter->value.type;
}
