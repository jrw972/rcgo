#ifndef type_hpp
#define type_hpp

#include "strtab.hpp"
#include "types.hpp"
#include "debug.hpp"
#include "util.hpp"
#include <vector>
#include <string>
#include <sstream>

class const_type_visitor_t;

struct type_t
{
  virtual void accept (const_type_visitor_t& visitor) const = 0;
  virtual std::string to_string () const = 0;
  virtual size_t alignment () const = 0;
  virtual size_t size () const = 0;
};

inline std::ostream&
operator<< (std::ostream& o, const type_t& type)
{
  return o << type.to_string ();
}

class void_type_t : public type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "void"; }
  size_t alignment () const { not_reached; }
  size_t size () const { return 0; }
  static const void_type_t* instance ();
private:
  void_type_t () { }
};

class bool_type_t : public type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<bool>"; }
  size_t alignment () const { return 1; }
  size_t size () const { return 1; }
  static const bool_type_t* instance ();
private:
  bool_type_t () { }
};

class uint_type_t : public type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<uint>"; }
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  static const uint_type_t* instance ();
private:
  uint_type_t () { }
};

class string_type_t : public type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<string>"; }
  size_t alignment () const { unimplemented; }
  size_t size () const { unimplemented; }
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
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "@" << *base_type_;
    return str.str ();
  }
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  static const type_t* make (const type_t* base_type);
private:
  pointer_type_t (const type_t* base_type) : base_type_t (base_type) { }
};

class pointer_to_immutable_type_t : public type_t, public base_type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "$" << *base_type_;
    return str.str ();
  }
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  static const type_t* make (const type_t* base_type);
private:
  pointer_to_immutable_type_t (const type_t* base_type) : base_type_t (base_type) { }
};

class pointer_to_foreign_type_t : public type_t, public base_type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "?" << *base_type_;
    return str.str ();
  }
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  static const type_t* make (const type_t* base_type);
private:
  pointer_to_foreign_type_t (const type_t* base_type) : base_type_t (base_type) { }
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
  const_iterator begin () const { return fields_.begin (); }
  const_iterator end () const { return fields_.end (); }
  void append (string_t field_name, const type_t * field_type);
  field_t * find (string_t name) const;
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
  array_type_t (size_t dimension,
                const type_t* base_type)
    : base_type_t (base_type)
    , dimension_ (dimension)
  { }

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << '[' << dimension_ << ']' << *base_type_;
    return str.str ();
  }
  size_t alignment () const { return base_type_->alignment (); }
  size_t size () const
  {
    size_t s = base_type_->size ();
    s = align_up (s, base_type_->alignment ());
    return s * dimension_;
  }
  size_t dimension () const { return dimension_; }
private:
  size_t dimension_;
};

class signature_type_t : public type_t
{
public:
  typedef std::vector<parameter_t*> ParametersType;
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const;
  size_t alignment () const { not_reached; }
  size_t size () const { not_reached; }

  size_t arity () const { return parameters_.size (); }
  parameter_t* at (size_t idx) const { return parameters_.at (idx); }
  ParametersType::const_iterator begin () const { return parameters_.begin (); }
  ParametersType::const_iterator end () const { return parameters_.end (); }
  parameter_t * find (string_t name) const;

  void
  prepend (string_t parameter_name,
           const type_t * parameter_type, bool is_receiver);

  void
  append (string_t parameter_name,
          const type_t * parameter_type, bool is_receiver);

private:
  ParametersType parameters_;
};

class func_type_t : public type_t
{
public:
  func_type_t (const signature_type_t * signature,
               const type_t * return_type)
    : signature_ (signature)
    , return_type_ (return_type)
  { }

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "func " << *signature_ << ' ' << *return_type_;
    return str.str ();
  }
  size_t alignment () const { return sizeof (void*); }
  size_t size () const { return sizeof (void*); }
  const signature_type_t* signature () const { return signature_; }
  const type_t* return_type () const { return return_type_; }
private:
  const signature_type_t *signature_;
  const type_t *return_type_;
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
  const signature_type_t* signature () const { return signature_; }
private:
  const signature_type_t *signature_;
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
  size_t alignment () const { not_reached; }
  size_t size () const { not_reached; }
  const signature_type_t * signature () const { return signature_; }
private:
  const signature_type_t *signature_;
};

class named_type_t : public type_t
{
public:
  named_type_t (string_t name)
    : name_ (name)
    , subtype_ (NULL)
  { }

  named_type_t (string_t name,
                const type_t* subtype)
    : name_ (name)
    , subtype_ (subtype)
  { }

  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    return get (name_);
  }

  void subtype (const type_t* s) { subtype_ = s; }
  const type_t* subtype () const { return subtype_; }
  virtual size_t alignment () const { return subtype_->alignment (); }
  virtual size_t size () const { return subtype_->size (); }

  method_t* add_method (ast_t* node, string_t identifier, const signature_type_t* signature, const type_t* return_type);
  method_t* get_method (string_t identifier) const;

  void add_action (action_t* action) { actions_.push_back (action); }
  typedef std::vector<action_t*> ActionsType;
  ActionsType::const_iterator actions_begin () const { return actions_.begin (); }
  ActionsType::const_iterator actions_end () const { return actions_.end (); }

  typedef std::vector<reaction_t*> ReactionsType;
  reaction_t * add_reaction (ast_t* node, string_t identifier, const signature_type_t * signature);
  reaction_t * get_reaction (string_t identifier) const;

  void add_bind (bind_t* bind) { binds_.push_back (bind); }
  typedef std::vector<bind_t*> BindsType;
  BindsType::const_iterator binds_begin () const { return binds_.begin (); }
  BindsType::const_iterator binds_end () const { return binds_.end (); }

private:
  string_t name_;
  const type_t* subtype_;
  std::vector<method_t*> methods_;
  ActionsType actions_;
  ReactionsType reactions_;
  BindsType binds_;
};

class immutable_type_t : public type_t, public base_type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "immutable " << *base_type_;
    return str.str ();
  }
  size_t alignment () const { return base_type_->alignment (); }
  size_t size () const { return base_type_->size (); }
  static const type_t* make (const type_t* base_type);
private:
  immutable_type_t (const type_t* base_type)
    : base_type_t (base_type)
  { }
};

class foreign_type_t : public type_t, public base_type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "foreign " << *base_type_;
    return str.str ();
  }
  size_t alignment () const { return base_type_->alignment (); }
  size_t size () const { return base_type_->size (); }
  static const type_t* make (const type_t* base_type);
private:
  foreign_type_t (const type_t* base_type)
    : base_type_t (base_type)
  { }
};

class untyped_boolean_type_t : public type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<untyped boolean>"; }
  size_t alignment () const { not_reached; }
  size_t size () const { not_reached; }
  static const untyped_boolean_type_t* instance ();
private:
  untyped_boolean_type_t () { }
};

class untyped_integer_type_t : public type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<untyped integer>"; }
  size_t alignment () const { not_reached; }
  size_t size () const { not_reached; }
  static const untyped_integer_type_t* instance ();
private:
  untyped_integer_type_t () { }
};

class untyped_string_type_t : public type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<untyped string>"; }
  size_t alignment () const { not_reached; }
  size_t size () const { not_reached; }
  static const untyped_string_type_t* instance ();
private:
  untyped_string_type_t () { }
};

class untyped_nil_type_t : public type_t
{
public:
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const { return "<nil>"; }
  size_t alignment () const { not_reached; }
  size_t size () const { not_reached; }
  static const untyped_nil_type_t* instance ();
private:
  untyped_nil_type_t () { }
};

class iota_type_t : public type_t
{
public:
  iota_type_t (size_t bound) : bound_ (bound) { }
  void accept (const_type_visitor_t& visitor) const;
  std::string to_string () const
  {
    std::stringstream str;
    str << "<iota " << bound_ << '>';
    return str.str ();
  }
  size_t alignment () const { not_reached; }
  size_t size () const { not_reached; }
  size_t bound () const { return bound_; }
private:
  size_t bound_;
};

struct const_type_visitor_t
{
  virtual ~const_type_visitor_t () { }
  virtual void visit (const array_type_t& type) { default_action (type); }
  virtual void visit (const bool_type_t& type) { default_action (type); }
  virtual void visit (const component_type_t& type) { default_action (type); }
  virtual void visit (const field_list_type_t& type) { default_action (type); }
  virtual void visit (const foreign_type_t& type) { default_action (type); }
  virtual void visit (const func_type_t& type) { default_action (type); }
  virtual void visit (const heap_type_t& type) { default_action (type); }
  virtual void visit (const immutable_type_t& type) { default_action (type); }
  virtual void visit (const iota_type_t& type) { default_action (type); }
  virtual void visit (const named_type_t& type) { default_action (type); }
  virtual void visit (const pointer_to_foreign_type_t& type) { default_action (type); }
  virtual void visit (const pointer_to_immutable_type_t& type) { default_action (type); }
  virtual void visit (const pointer_type_t& type) { default_action (type); }
  virtual void visit (const port_type_t& type) { default_action (type); }
  virtual void visit (const reaction_type_t& type) { default_action (type); }
  virtual void visit (const signature_type_t& type) { default_action (type); }
  virtual void visit (const string_type_t& type) { default_action (type); }
  virtual void visit (const struct_type_t& type) { default_action (type); }
  virtual void visit (const uint_type_t& type) { default_action (type); }
  virtual void visit (const untyped_boolean_type_t& type) { default_action (type); }
  virtual void visit (const untyped_integer_type_t& type) { default_action (type); }
  virtual void visit (const untyped_nil_type_t& type) { default_action (type); }
  virtual void visit (const untyped_string_type_t& type) { default_action (type); }
  virtual void visit (const void_type_t& type) { default_action (type); }
  virtual void default_action (const type_t& type) { }
};

// Cast functions.
const bool_type_t*
type_to_bool (const type_t * type);

const pointer_type_t*
type_to_pointer (const type_t * type);

const pointer_to_immutable_type_t*
type_to_pointer_to_immutable (const type_t * type);

const heap_type_t*
type_to_heap (const type_t* type);

const signature_type_t*
type_to_signature (const type_t * type);

const component_type_t*
type_to_component (const type_t* type);

const array_type_t*
type_to_array (const type_t* type);

const port_type_t*
type_to_port (const type_t* port);

const reaction_type_t*
type_to_reaction (const type_t * type);

const immutable_type_t*
type_to_immutable (const type_t* type);

const foreign_type_t*
type_to_foreign (const type_t* type);

// Operations.
const type_t*
type_select (const type_t* type, string_t identifier);

field_t*
type_select_field (const type_t* type, string_t identifier);

reaction_t*
type_select_reaction (const type_t* type, string_t identifier);

const type_t*
type_dereference (const type_t* type);

const type_t*
type_move (const type_t* type);

const type_t*
type_merge (const type_t* type);

const type_t*
type_change (const type_t* type);

// Predicates.
bool
type_is_convertible (const type_t * from, const type_t * to);

bool
type_is_equal (const type_t* x, const type_t* y);

bool
type_is_callable (const type_t * type);

bool
type_is_compatible_port_reaction (const port_type_t * port,
                                  const reaction_type_t * reaction);

bool
type_is_foreign_safe (const type_t* type);

bool
type_is_any_pointer (const type_t* type);

bool
type_is_assignable (const type_t* type);

bool
type_is_untyped (const type_t* type);

// Getters.
size_t
type_parameter_count (const type_t * type);

const type_t*
type_parameter_type (const type_t * type, size_t size);

const type_t*
type_return_type (const type_t * type);

const type_t*
type_pointer_base_type (const type_t* type);

const type_t*
type_strip (const type_t* type);

#endif /* type_hpp */
