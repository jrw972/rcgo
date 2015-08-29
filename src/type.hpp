#ifndef type_hpp
#define type_hpp

#include "types.hpp"
#include "debug.hpp"
#include "util.hpp"
#include <vector>
#include <string>
#include <sstream>

class const_type_visitor_t;

struct type_t
{
  enum TypeLevel
    {
      NAMED_TYPE,
      CONVENTIONAL,
      UNTYPED,
    };
  virtual void accept (const_type_visitor_t& visitor) const = 0;
  virtual std::string to_string () const = 0;
  virtual size_t alignment () const = 0;
  virtual size_t size () const = 0;
  // When give the choice between two types, use the one with lower level.
  virtual TypeLevel level () const = 0;
};

inline std::ostream&
operator<< (std::ostream& o, const type_t& type)
{
  return o << type.to_string ();
}

class named_type_t : public type_t
{
public:
  named_type_t (const std::string& name)
    : name_ (name)
    , subtype_ (NULL)
  { }

  named_type_t (const std::string& name,
                const type_t* subtype);

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    return name_;
  }

  void subtype (const type_t* s) { subtype_ = s; }
  const type_t* subtype () const { return subtype_; }
  virtual size_t alignment () const { return subtype_->alignment (); }
  virtual size_t size () const { return subtype_->size (); }
  virtual TypeLevel level () const { return NAMED_TYPE; }

  void add_method (method_t* method) { methods_.push_back (method); }
  method_t* get_method (const std::string& identifier) const;

  void add_action (action_t* action) { actions_.push_back (action); }
  typedef std::vector<action_t*> ActionsType;
  ActionsType::const_iterator actions_begin () const { return actions_.begin (); }
  ActionsType::const_iterator actions_end () const { return actions_.end (); }

  typedef std::vector<reaction_t*> ReactionsType;
  void add_reaction (reaction_t* reaction) { reactions_.push_back (reaction); }
  reaction_t * get_reaction (const std::string& identifier) const;

  void add_bind (bind_t* bind) { binds_.push_back (bind); }
  typedef std::vector<bind_t*> BindsType;
  BindsType::const_iterator binds_begin () const { return binds_.begin (); }
  BindsType::const_iterator binds_end () const { return binds_.end (); }

private:
  std::string const name_;
  const type_t* subtype_;
  std::vector<method_t*> methods_;
  ActionsType actions_;
  ReactionsType reactions_;
  BindsType binds_;
};

class void_type_t : public type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "void"; }
  size_t alignment () const { not_reached; }
  size_t size () const { return 0; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const void_type_t* instance ();
private:
  void_type_t () { }
};

class bool_type_t : public type_t
{
public:
  typedef bool ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<bool>"; }
  size_t alignment () const { return 1; }
  size_t size () const { return 1; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const bool_type_t* instance ();
private:
  bool_type_t () { }
};

class int_type_t : public type_t
{
public:
  typedef int64_t ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<int>"; }
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const int_type_t* instance ();
private:
  int_type_t () { }
};

class int8_type_t : public type_t
{
public:
  typedef int8_t ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<int8>"; }
  size_t alignment () const { return 1; }
  size_t size () const { return 1; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const int8_type_t* instance ();
private:
  int8_type_t () { }
};

class int64_type_t : public type_t
{
public:
  typedef int64_t ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<int64>"; }
  size_t alignment () const { return 1; }
  size_t size () const { return 1; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const int64_type_t* instance ();
private:
  int64_type_t () { }
};

class uint_type_t : public type_t
{
public:
  typedef uint64_t ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<uint>"; }
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const uint_type_t* instance ();
private:
  uint_type_t () { }
};

class uint8_type_t : public type_t
{
public:
  typedef uint8_t ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<uint8>"; }
  size_t alignment () const { return 1; }
  size_t size () const { return 1; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const uint8_type_t* instance ();
private:
  uint8_type_t () { }
};

class uint32_type_t : public type_t
{
public:
  typedef uint32_t ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<uint32>"; }
  size_t alignment () const { return 4; }
  size_t size () const { return 4; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const uint32_type_t* instance ();
private:
  uint32_type_t () { }
};

class uint64_type_t : public type_t
{
public:
  typedef uint64_t ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<uint64>"; }
  size_t alignment () const { return 8; }
  size_t size () const { return 8; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const uint64_type_t* instance ();
private:
  uint64_type_t () { }
};

class uint128_type_t : public type_t
{
public:
  typedef unsigned long long ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<uint128>"; }
  size_t alignment () const { return 16; }
  size_t size () const { return 16; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const uint128_type_t* instance ();
private:
  uint128_type_t () { }
};

class enum_type_t : public type_t
{
public:
  typedef size_t ValueType;

  virtual void accept (const_type_visitor_t& visitor) const;
  virtual std::string to_string () const { return "<enum>"; }
  virtual size_t alignment () const { return sizeof (void*); }
  virtual size_t size () const { return sizeof (void*); }
  virtual TypeLevel level () const { return CONVENTIONAL; }
};

class float64_type_t : public type_t
{
public:
  typedef double ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<float64>"; }
  size_t alignment () const { return 8; }
  size_t size () const { return 8; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const float64_type_t* instance ();
private:
  float64_type_t () { }
};

class string_type_t : public type_t
{
public:
  typedef std::string ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<string>"; }
  size_t alignment () const { unimplemented; }
  size_t size () const { unimplemented; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const string_type_t* instance ();
private:
  string_type_t () { }
};

// Helper class for types that have a base type.
class base_type_t
{
public:
  base_type_t (const type_t* base_type) : base_type_ (base_type) { }
  const type_t* base_type () const { return base_type_; }
protected:
  const type_t* base_type_;
};

class pointer_type_t : public type_t, public base_type_t
{
public:
  typedef void* ValueType;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "@" << *base_type_;
    return str.str ();
  }
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  static const type_t* make (const type_t* base_type);
private:
  pointer_type_t (const type_t* base_type) : base_type_t (base_type) { }
};

struct heap_type_t : public type_t, public base_type_t
{
  heap_type_t (const type_t* base_type) : base_type_t (base_type) { }
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "heap " << *base_type_;
    return str.str ();
  }
  size_t alignment () const { not_reached; }
  size_t size () const { not_reached; }
  virtual TypeLevel level () const { return CONVENTIONAL; }
};

class field_list_type_t : public type_t
{
public:
  typedef std::vector<field_t*> FieldsType;
  typedef FieldsType::const_iterator const_iterator;
  field_list_type_t (bool insert_runtime);
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { unimplemented; }
  size_t alignment () const { return alignment_; }
  size_t size () const { return offset_; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  const_iterator begin () const { return fields_.begin (); }
  const_iterator end () const { return fields_.end (); }
  void append (const std::string& field_name, const type_t * field_type);
  field_t * find (const std::string& name) const;
private:
  FieldsType fields_;
  ptrdiff_t offset_;
  size_t alignment_;
};

class struct_type_t : public type_t
{
public:
  struct_type_t (const field_list_type_t* fl) : field_list_ (fl) { }
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { unimplemented; }
  size_t alignment () const { return field_list_->alignment (); }
  size_t size () const { return field_list_->size (); }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  const field_list_type_t * field_list () const { return field_list_; }
private:
  const field_list_type_t * field_list_;
};

struct component_type_t : public struct_type_t
{
  component_type_t (const field_list_type_t* field_list) : struct_type_t (field_list) { }
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { unimplemented; }
};

class array_type_t : public type_t, public base_type_t
{
public:
  array_type_t (int_type_t::ValueType d,
                const type_t* base_type)
    : base_type_t (base_type)
    , dimension (d)
  { }

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << '[' << dimension << ']' << *base_type_;
    return str.str ();
  }
  size_t alignment () const { return base_type_->alignment (); }
  size_t size () const { return element_size () * dimension; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  size_t element_size () const
  {
    return align_up (base_type_->size (), base_type_->alignment ());
  }

  const int_type_t::ValueType dimension;
};

class signature_type_t : public type_t
{
public:
  typedef std::vector<const parameter_t*> ParametersType;
  typedef ParametersType::const_iterator const_iterator;

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const;
  size_t alignment () const { not_reached; }
  size_t size () const { not_reached; }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  size_t arity () const { return parameters_.size (); }
  const parameter_t* at (size_t idx) const { return parameters_.at (idx); }
  const_iterator begin () const { return parameters_.begin (); }
  const_iterator end () const { return parameters_.end (); }
  const parameter_t * find (const std::string& name) const;

  void
  append (const parameter_t* p) { parameters_.push_back (p); }

private:
  ParametersType parameters_;
};

class function_type_t : public type_t
{
public:
  function_type_t (const signature_type_t * signature,
                   const parameter_t * return_parameter)
    : signature_ (signature)
    , return_parameter_ (return_parameter)
  { }

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const;
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  const signature_type_t* signature () const { return signature_; }
  const parameter_t* return_parameter () const { return return_parameter_; }

private:
  const signature_type_t * const signature_;
  const parameter_t * return_parameter_;
};

class method_type_t : public type_t
{
public:
  method_type_t (const named_type_t* named_type_,
                 const std::string& this_name,
                 const type_t* receiver_type_,
                 Mutability dereference_mutability,
                 const signature_type_t * signature,
                 const parameter_t* return_parameter)
    : named_type (named_type_)
    , receiver_type (receiver_type_)
    , this_parameter (make_this_parameter (this_name, receiver_type_, dereference_mutability))
    , function_type (make_function_type (this_parameter, signature, return_parameter))
    , signature_ (signature)
    , bind_type_ (new function_type_t (signature_, return_parameter))
  {
  }

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const;
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  const named_type_t* const named_type;
  const type_t* const receiver_type;
  const parameter_t* const this_parameter;
  const function_type_t* function_type;
  const type_t* bind_type () const { return bind_type_; }
  const signature_type_t* signature () const { return signature_; }
  const parameter_t* return_parameter () const { return function_type->return_parameter (); }

private:
  static parameter_t* make_this_parameter (const std::string& this_name,
                                           const type_t* receiver_type,
                                           Mutability dereference_mutability);
  static function_type_t* make_function_type (const parameter_t* this_parameter,
                                              const signature_type_t* signature,
                                              const parameter_t* return_parameter);
  const signature_type_t* signature_;
  const type_t* bind_type_;
};

class port_type_t : public type_t
{
public:
  port_type_t (const signature_type_t* signature)
    : signature_ (signature)
  { }

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "port " << *signature_;
    return str.str ();
  }
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  const signature_type_t* signature () const { return signature_; }
private:
  const signature_type_t *signature_;
};

class pfunc_type_t : public type_t
{
public:
  pfunc_type_t (const signature_type_t* signature_, const parameter_t* return_parameter_)
    : function_type_ (new function_type_t (signature_, return_parameter_))
  { }

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const;
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (pfunc_t); }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  const signature_type_t * signature () const { return function_type_->signature (); }
  const parameter_t * return_parameter () const { return function_type_->return_parameter (); }
  const type_t* bind_type () const { return function_type_; }

private:
  const function_type_t* function_type_;
};

class reaction_type_t : public type_t
{
public:
  reaction_type_t (const signature_type_t* signature)
    : signature_ (signature)
  { }

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "reaction " << *signature_;
    return str.str ();
  }
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  virtual TypeLevel level () const { return CONVENTIONAL; }

  const signature_type_t * signature () const { return signature_; }
private:
  const signature_type_t *signature_;
};

class nil_type_t : public type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<nil>"; }
  size_t alignment () const { not_reached; }
  size_t size () const { return sizeof (void*); }
  virtual TypeLevel level () const { return UNTYPED; }

  static const nil_type_t* instance ();
private:
  nil_type_t () { }
};

struct const_type_visitor_t
{
  virtual ~const_type_visitor_t () { }
  virtual void visit (const array_type_t& type) { default_action (type); }
  virtual void visit (const bool_type_t& type) { default_action (type); }
  virtual void visit (const component_type_t& type) { default_action (type); }
  virtual void visit (const enum_type_t& type) { default_action (type); }
  virtual void visit (const field_list_type_t& type) { default_action (type); }
  virtual void visit (const function_type_t& type) { default_action (type); }
  virtual void visit (const method_type_t& type) { default_action (type); }
  virtual void visit (const heap_type_t& type) { default_action (type); }
  virtual void visit (const named_type_t& type) { default_action (type); }
  virtual void visit (const pointer_type_t& type) { default_action (type); }
  virtual void visit (const port_type_t& type) { default_action (type); }
  virtual void visit (const pfunc_type_t& type) { default_action (type); }
  virtual void visit (const reaction_type_t& type) { default_action (type); }
  virtual void visit (const signature_type_t& type) { default_action (type); }
  virtual void visit (const string_type_t& type) { default_action (type); }
  virtual void visit (const struct_type_t& type) { default_action (type); }
  virtual void visit (const int_type_t& type) { default_action (type); }
  virtual void visit (const int8_type_t& type) { default_action (type); }
  virtual void visit (const int64_type_t& type) { default_action (type); }
  virtual void visit (const uint_type_t& type) { default_action (type); }
  virtual void visit (const uint8_type_t& type) { default_action (type); }
  virtual void visit (const uint32_type_t& type) { default_action (type); }
  virtual void visit (const uint64_type_t& type) { default_action (type); }
  virtual void visit (const uint128_type_t& type) { default_action (type); }
  virtual void visit (const float64_type_t& type) { default_action (type); }
  virtual void visit (const nil_type_t& type) { default_action (type); }
  virtual void visit (const void_type_t& type) { default_action (type); }
  virtual void default_action (const type_t& type) { }
};

// Select the appropriate object.
field_t*
type_select_field (const type_t* type, const std::string& identifier);

method_t*
type_select_method (const type_t* type, const std::string& identifier);

reaction_t*
type_select_reaction (const type_t* type, const std::string& identifier);

// Return type of selected field, method, or reaction.
const type_t*
type_select (const type_t* type, const std::string& identifier);

// Return the type of indexing into the other type.
const type_t*
type_index (const type_t* base, const type_t* index);

// Return type after applying dereference or NULL.
const type_t*
type_dereference (const type_t* type);

// Return type after applying move or NULL.
const type_t*
type_move (const type_t* type);

// Return type after applying merge or NULL.
const type_t*
type_merge (const type_t* type);

// Return type after applying change or NULL.
const type_t*
type_change (const type_t* type);

// True if the types are equal (strict).
bool
type_is_equal (const type_t* x, const type_t* y);

// True if any pointer is accessible.
bool
type_contains_pointer (const type_t* type);

// True if boolean operators can be applied to values of this type.
bool
type_is_boolean (const type_t* type);

// True if type is an integer.
bool
type_is_integral (const type_t* type);

// True if type is an unsigned integer.
bool
type_is_unsigned_integral (const type_t* type);

// True if type is floating-point.
bool
type_is_floating (const type_t* type);

// True if type is numeric.
inline bool
type_is_numeric (const type_t* type)
{
  return type_is_integral (type) || type_is_floating (type);
}

// True if == or != can be applied to values of this type.
bool
type_is_comparable (const type_t* type);

// True if <, <=, >, or >= can be applied to values of this type.
bool
type_is_orderable (const type_t* type);

// True if index is valid.
bool
type_is_index (const type_t* type, int_type_t::ValueType index);

// True if x can be cast to y.
bool
type_is_castable (const type_t* x, const type_t* y);

bool
type_is_pointer_compare (const type_t* left, const type_t* right);

// Remove a named_type_t.
const type_t*
type_strip (const type_t* type);

// Choose between equivalent types.
inline const type_t*
type_choose (const type_t* x, const type_t* y)
{
  if (y->level () < x->level ()) { return y; }
  return x;
}

// Cast a type to a specific type.
template<typename T>
const T*
type_cast (const type_t * type)
{
  if (type == NULL)
    {
      return NULL;
    }

  struct visitor : public const_type_visitor_t
  {
    const T* retval;

    visitor () : retval (NULL) { }

    void visit (const T& type)
    {
      retval = &type;
    }
  };
  visitor v;
  type->accept (v);
  return v.retval;
}

#define type_not_reached(type) do { std::cerr << type << std::endl; not_reached; } while (0);

#endif /* type_hpp */
