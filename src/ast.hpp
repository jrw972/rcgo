#ifndef ast_hpp
#define ast_hpp

#include "debug.hpp"
#include <vector>
#include "action.hpp"
#include "Symbol.hpp"

class ast_visitor_t;
class ast_const_visitor_t;

struct ast_t
{
  virtual ~ast_t() { }
  virtual void accept (ast_visitor_t& visitor) = 0;
  virtual void accept (ast_const_visitor_t& visitor) const = 0;
  virtual void print (std::ostream& out) const = 0;

  void visit_children (ast_visitor_t& visitor)
  {
    for (iterator pos = begin (), limit = end (); pos != limit; ++pos)
      {
        (*pos)->accept (visitor);
      }
  }

  void visit_children (ast_const_visitor_t& visitor) const
  {
    for (const_iterator pos = begin (), limit = end (); pos != limit; ++pos)
      {
        (*pos)->accept (visitor);
      }
  }

  typedef std::vector<ast_t*> ChildrenType;
  typedef ChildrenType::iterator iterator;
  typedef ChildrenType::const_iterator const_iterator;
  typedef std::vector<Symbol*> SymbolsType;

private:
  ast_t* parent_;
  ChildrenType children;
public:
  Location const location;
  SymbolsType symbols;
  typed_value_t typed_value;

  size_t size () const
  {
    return children.size ();
  }

  ast_t * append (ast_t * child)
  {
    child->parent_ = this;
    children.push_back (child);
    return this;
  }

  ast_t* prepend (ast_t * child)
  {
    child->parent_ = this;
    children.insert (children.begin (), child);
    return this;
  }

  ast_t * const &
  at (size_t idx) const
  {
    assert (idx < children.size ());
    return children[idx];
  }

  ast_t * &
  at (size_t idx)
  {
    assert (idx < children.size ());
    return children[idx];
  }

  void
  set (size_t idx, ast_t * child)
  {
    assert (idx < children.size ());
    child->parent_ = this;
    children[idx] = child;
  }

  iterator
  get_child_ptr (size_t idx)
  {
    assert (idx < this->children.size ());
    return this->children.begin () + idx;
  }

  iterator begin ()
  {
    return children.begin ();
  }
  const_iterator begin () const
  {
    return children.begin ();
  }

  iterator end ()
  {
    return children.end ();
  }
  const_iterator end () const
  {
    return children.end ();
  }

  void EnterSymbol (Symbol* s)
  {
    symbols.push_back (s);
  }

  Symbol *
  FindSymbol (const std::string& identifier) const
  {
    for (SymbolsType::const_iterator pos = symbols.begin (),
         limit = symbols.end ();
         pos != limit;
         ++pos)
      {
        Symbol *s = *pos;
        if (identifier == s->identifier)
          {
            return s;
          }
      }

    /* Not found in this scope.  Try the parent. */
    if (parent_)
      {
        return parent_->FindSymbol (identifier);
      }
    else
      {
        return NULL;
      }
  }

  Symbol *
  FindSymbolCurrent (const std::string& identifier) const
  {
    for (std::vector<Symbol*>::const_iterator pos = symbols.begin (),
         limit = symbols.end ();
         pos != limit;
         ++pos)
      {
        Symbol *s = *pos;
        if (identifier == s->identifier)
          {
            return s;
          }
      }

    return NULL;
  }

  virtual Symbol *
  GetReceiverSymbol () const
  {
    if (parent_ == NULL)
      {
        return NULL;
      }
    return parent_->GetReceiverSymbol ();
  }

  virtual ParameterSymbol *
  GetReturnSymbol () const
  {
    if (parent_ == NULL)
      {
        return NULL;
      }
    return parent_->GetReturnSymbol ();
  }

  Symbol* Activate ();
  void Change ();

  virtual const Type::Type*
  GetReceiverType () const
  {
    if (parent_ == NULL)
      {
        return NULL;
      }
    return parent_->GetReceiverType ();
  }

  virtual Activation*
  GetActivation () const
  {
    if (parent_ == NULL)
      {
        return NULL;
      }
    return parent_->GetActivation ();
  }

  virtual action_reaction_base_t*
  GetAction () const
  {
    if (parent_ == NULL)
      {
        return NULL;
      }
    return parent_->GetAction ();
  }

  virtual Initializer*
  GetInitializer () const
  {
    if (parent_ == NULL)
      {
        return NULL;
      }
    return parent_->GetInitializer ();
  }

  virtual Getter*
  GetGetter () const
  {
    if (parent_ == NULL)
      {
        return NULL;
      }
    return parent_->GetGetter ();
  }

  ast_t* parent () const
  {
    return parent_;
  }

protected:
  ast_t (unsigned int line_, size_t children_count)
    : parent_ (NULL)
    , location (line_)
  {
    assert (location.Line != 0);
    children.resize (children_count);
  }
};

std::ostream&
operator<< (std::ostream& out, const ast_t& node);

class ast_identifier_t : public ast_t
{
public:
  ast_identifier_t (unsigned int line, const std::string& id)
    : ast_t (line, 0)
    , identifier (id)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "identifier " << identifier;
  }

  std::string const identifier;
};

struct ast_identifier_list_t : public ast_t
{
  ast_identifier_list_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "identifier_list";
  }
};

struct ast_receiver_t : public ast_t
{
  enum
  {
    THIS_IDENTIFIER,
    TYPE_IDENTIFIER,
    COUNT,
  };

  ast_receiver_t (unsigned int line, ast_t* this_identifier, Mutability m, Mutability dm, bool isP, ast_t* type_identifier)
    : ast_t (line, COUNT)
    , mutability (m)
    , dereferenceMutability (dm)
    , isPointer (isP)
  {
    set (THIS_IDENTIFIER, this_identifier);
    set (TYPE_IDENTIFIER, type_identifier);
  }

  ast_t* this_identifier () const
  {
    return at (THIS_IDENTIFIER);
  }
  ast_t* type_identifier () const
  {
    return at (TYPE_IDENTIFIER);
  }

  virtual void accept (ast_visitor_t& visitor);
  virtual void accept (ast_const_visitor_t& visitor) const;
  virtual void print (std::ostream& out) const
  {
    out << "receiver";
  }

  Mutability const mutability;
  Mutability const dereferenceMutability;
  bool const isPointer;
  Symbol* this_symbol;
};

struct ast_array_type_spec_t : public ast_t
{
  enum
  {
    DIMENSION,
    BASE_TYPE,
    COUNT,
  };

  ast_array_type_spec_t (unsigned int line, ast_t* dimension, ast_t* base_type)
    : ast_t (line, COUNT)
  {
    set (DIMENSION, dimension);
    set (BASE_TYPE, base_type);
  }

  ast_t* dimension () const
  {
    return at (DIMENSION);
  }
  ast_t*& dimension_ref ()
  {
    return at (DIMENSION);
  }
  ast_t* base_type () const
  {
    return at (BASE_TYPE);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "array_type_spec";
  }
};

struct ast_unary_t : public ast_t
{
  enum
  {
    CHILD,
    COUNT
  };

  ast_unary_t (unsigned int line, ast_t* child)
    : ast_t (line, COUNT)
  {
    set (CHILD, child);
  }

  ast_t* child () const
  {
    return at (CHILD);
  }
  ast_t*& child_ref ()
  {
    return at (CHILD);
  }
};

struct ast_empty_type_spec_t : public ast_t
{
  ast_empty_type_spec_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_enum_type_spec_t : public ast_t
{
  enum
  {
    VALUES,
    COUNT,
  };

  ast_enum_type_spec_t (unsigned int line, ast_t* values)
    : ast_t (line, COUNT)
  {
    set (VALUES, values);
  }

  ast_t* values () const
  {
    return at (VALUES);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_field_list_type_spec_t : public ast_t
{
  ast_field_list_type_spec_t (unsigned int line)
    : ast_t (line, 0)
    , IsComponent (false)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  bool IsComponent;
};

struct ast_heap_type_spec_t : public ast_unary_t
{
  ast_heap_type_spec_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "heap_type_spec";
  }
};

struct ast_identifier_type_spec_t : public ast_unary_t
{
  ast_identifier_type_spec_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "identifier_type";
  }
};

struct ast_identifier_list_type_spec_t : public ast_t
{
  enum
  {
    IDENTIFIER_LIST,
    TYPE_SPEC,
    COUNT,
  };

  ast_identifier_list_type_spec_t (unsigned int line, ast_t* identifier_list,
                                   Mutability m, Mutability dm,
                                   ast_t* type_spec)
    : ast_t (line, COUNT)
    , mutability (m)
    , dereferenceMutability (dm)
  {
    set (IDENTIFIER_LIST, identifier_list);
    set (TYPE_SPEC, type_spec);
  }

  const Mutability mutability;
  const Mutability dereferenceMutability;

  ast_t* identifier_list () const
  {
    return at (IDENTIFIER_LIST);
  }
  ast_t* type_spec () const
  {
    return at (TYPE_SPEC);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_pointer_type_spec_t : public ast_unary_t
{
  ast_pointer_type_spec_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_slice_type_spec_t : public ast_unary_t
{
  ast_slice_type_spec_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "slice_type_spec";
  }
};

struct ast_push_port_type_spec_t : public ast_t
{
  enum
  {
    SIGNATURE,
    COUNT,
  };

  ast_push_port_type_spec_t (unsigned int line, ast_t* signature)
    : ast_t (line, COUNT)
  {
    set (SIGNATURE, signature);
  }

  ast_t* signature () const
  {
    return at (SIGNATURE);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_pull_port_type_spec_t : public ast_t
{
  enum
  {
    SIGNATURE,
    RETURN_TYPE,
    COUNT,
  };

  ast_pull_port_type_spec_t (unsigned int line, ast_t* signature, Mutability dm, ast_t* return_type)
    : ast_t (line, COUNT)
    , dereferenceMutability (dm)
  {
    set (SIGNATURE, signature);
    set (RETURN_TYPE, return_type);
  }

  ast_t* signature () const
  {
    return at (SIGNATURE);
  }
  ast_t* return_type () const
  {
    return at (RETURN_TYPE);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "pull_port_type_spec";
  }

  Mutability const dereferenceMutability;
};

struct ast_signature_type_spec_t : public ast_t
{
  ast_signature_type_spec_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "signature_type_spec";
  }
};

class ast_expr_t : public ast_t
{
public:
  ast_expr_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }
};

struct ast_type_expr_t : public ast_expr_t
{
  enum
  {
    TYPE_SPEC,
    COUNT
  };

  ast_type_expr_t (unsigned int line, ast_t* type_spec)
    : ast_expr_t (line, COUNT)
  {
    set (TYPE_SPEC, type_spec);
  }

  ast_t* type_spec () const
  {
    return at (TYPE_SPEC);
  }

  virtual void accept (ast_visitor_t& visitor);
  virtual void accept (ast_const_visitor_t& visitor) const;
  virtual void print (std::ostream& out) const
  {
    out << "type";
  }
};

// TODO:  This class can be removed.
struct ast_cast_expr_t : public ast_expr_t
{
  enum
  {
    TYPE_SPEC,
    CHILD,
    COUNT
  };

  ast_cast_expr_t (unsigned int line, ast_t* type_spec, ast_t* child)
    : ast_expr_t (line, COUNT)
  {
    set (TYPE_SPEC, type_spec);
    set (CHILD, child);
  }

  ast_t* type_spec () const
  {
    return at (TYPE_SPEC);
  }
  ast_t* child () const
  {
    return at (CHILD);
  }
  ast_t*& child_ref ()
  {
    return at (CHILD);
  }

  virtual void accept (ast_visitor_t& visitor);
  virtual void accept (ast_const_visitor_t& visitor) const;
  virtual void print (std::ostream& out) const
  {
    out << "cast";
  }
};

struct ast_unary_expr_t : public ast_expr_t
{
  enum
  {
    CHILD,
    COUNT
  };

  ast_unary_expr_t (unsigned int line, ast_t* child)
    : ast_expr_t (line, COUNT)
  {
    set (CHILD, child);
  }

  ast_t* child () const
  {
    return at (CHILD);
  }
  ast_t*& child_ref ()
  {
    return at (CHILD);
  }
};

struct ast_binary_expr_t : public ast_expr_t
{
  enum
  {
    LEFT,
    RIGHT,
    COUNT
  };

  ast_binary_expr_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_expr_t (line, COUNT)
  {
    set (LEFT, left);
    set (RIGHT, right);
  }

  ast_t*& left_ref ()
  {
    return at (LEFT);
  }
  ast_t* left () const
  {
    return at (LEFT);
  }
  ast_t*& right_ref ()
  {
    return at (RIGHT);
  }
  ast_t* right () const
  {
    return at (RIGHT);
  }
};

struct ast_binary_arithmetic_expr_t : public ast_binary_expr_t
{
  ast_binary_arithmetic_expr_t (unsigned int line, BinaryArithmetic a, ast_t* left, ast_t* right)
    : ast_binary_expr_t (line, left, right)
    , arithmetic (a)
  { }

  const BinaryArithmetic arithmetic;

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << binary_arithmetic_symbol (arithmetic);
  }
};

struct ast_implicit_conversion_expr_t : public ast_unary_expr_t
{
  ast_implicit_conversion_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "implicit_conversion";
  }
};

struct ast_address_of_expr_t : public ast_unary_expr_t
{
  ast_address_of_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "address_of";
  }
};

struct ast_call_expr_t : public ast_expr_t
{
  enum
  {
    EXPR,
    ARGS,
    COUNT,
  };

  ast_call_expr_t (unsigned int line, ast_t* expr, ast_t* args)
    : ast_expr_t (line, COUNT)
  {
    set (EXPR, expr);
    set (ARGS, args);
  }

  ast_t* expr () const
  {
    return at (EXPR);
  }
  ast_t*& expr_ref ()
  {
    return at (EXPR);
  }
  ast_t* args () const
  {
    return at (ARGS);
  }
  iterator args_iterator ()
  {
    return begin () + ARGS;
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "call_expr";
  }
};

struct ast_dereference_expr_t : public ast_unary_expr_t
{
  ast_dereference_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "dereference_expr";
  }
};

struct ast_implicit_dereference_expr_t : public ast_unary_expr_t
{
  ast_implicit_dereference_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "implicit dereference";
  }
};

struct ast_list_expr_t : public ast_expr_t
{
  ast_list_expr_t (unsigned int line)
    : ast_expr_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "list_expr";
  }
};

struct ast_identifier_expr_t : public ast_unary_expr_t
{
  ast_identifier_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "identifier_expr";
  }

  Symbol* symbol;
};

struct ast_index_expr_t : public ast_expr_t
{
  enum
  {
    BASE,
    INDEX,
    COUNT,
  };

  ast_index_expr_t (unsigned int line, ast_t* base, ast_t* index)
    : ast_expr_t (line, COUNT)
  {
    set (BASE, base);
    set (INDEX, index);
  }

  ast_t* base () const
  {
    return at (BASE);
  }
  ast_t*& base_ref ()
  {
    return at (BASE);
  }
  ast_t* index () const
  {
    return at (INDEX);
  }
  ast_t*& index_ref ()
  {
    return at (INDEX);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "index";
  }
};

struct ast_slice_expr_t : public ast_expr_t
{
  enum
  {
    BASE,
    LOW,
    HIGH,
    COUNT,
  };

  ast_slice_expr_t (unsigned int line, ast_t* base, ast_t* low, ast_t* high)
    : ast_expr_t (line, COUNT)
  {
    set (BASE, base);
    set (LOW, low);
    set (HIGH, high);
  }

  ast_t* base () const
  {
    return at (BASE);
  }
  ast_t*& base_ref ()
  {
    return at (BASE);
  }
  ast_t* low () const
  {
    return at (LOW);
  }
  ast_t*& low_ref ()
  {
    return at (LOW);
  }
  ast_t* high () const
  {
    return at (HIGH);
  }
  ast_t*& high_ref ()
  {
    return at (HIGH);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "slice";
  }
};

struct ast_logic_not_expr_t : public ast_unary_expr_t
{
  ast_logic_not_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "logic_not_expr";
  }
};

struct ast_push_port_call_expr_t : public ast_expr_t
{
  enum
  {
    IDENTIFIER,
    ARGS,
    COUNT,
  };

  ast_push_port_call_expr_t (unsigned int line, ast_t* identifier, ast_t* args)
    : ast_expr_t (line, COUNT)
  {
    set (IDENTIFIER, identifier);
    set (ARGS, args);
  }

  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* args () const
  {
    return at (ARGS);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  field_t* field;
};

struct ast_indexed_port_call_expr_t : public ast_expr_t
{
  enum
  {
    IDENTIFIER,
    INDEX,
    ARGS,
    COUNT
  };

  ast_indexed_port_call_expr_t (unsigned int line,
                                ast_t * identifier,
                                ast_t* index,
                                ast_t * args)
    : ast_expr_t (line, COUNT)
  {
    set (IDENTIFIER, identifier);
    set (INDEX, index);
    set (ARGS, args);
  }

  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* index () const
  {
    return at (INDEX);
  }
  ast_t*& index_ref ()
  {
    return at (INDEX);
  }
  ast_t* args () const
  {
    return at (ARGS);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  field_t* field;
  const Type::Array* array_type;
};

struct ast_select_expr_t : public ast_expr_t
{
  enum
  {
    BASE,
    IDENTIFIER,
    COUNT,
  };

  ast_select_expr_t (unsigned int line, ast_t* base, ast_t* identifier)
    : ast_expr_t (line, COUNT)
  {
    set (BASE, base);
    set (IDENTIFIER, identifier);
  }

  ast_t* base () const
  {
    return at (BASE);
  }
  ast_t*& base_ref ()
  {
    return at (BASE);
  }
  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "select_expr";
  }
};

struct ast_literal_expr_t : public ast_expr_t
{
  ast_literal_expr_t (unsigned int line, typed_value_t tv)
    : ast_expr_t (line, 0)
  {
    typed_value = tv;
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "literal";
  }
};

struct ast_binary_t : public ast_t
{
  enum
  {
    LEFT,
    RIGHT,
    COUNT
  };

  ast_binary_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_t (line, COUNT)
  {
    set (LEFT, left);
    set (RIGHT, right);
  }

  ast_t* left () const
  {
    return at (LEFT);
  }
  ast_t*& left_ref ()
  {
    return at (LEFT);
  }
  ast_t* right () const
  {
    return at (RIGHT);
  }
  ast_t*& right_ref ()
  {
    return at (RIGHT);
  }
};

struct ast_empty_statement_t : public ast_t
{
  ast_empty_statement_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_add_assign_statement_t : public ast_binary_t
{
  ast_add_assign_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_assign_statement_t : public ast_binary_t
{
  ast_assign_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "assign_statement";
  }
};

struct ast_change_statement_t : public ast_t
{
  enum
  {
    EXPR,
    IDENTIFIER,
    BODY,
    COUNT
  };

  ast_change_statement_t (unsigned int line,
                          ast_t * expr,
                          ast_t * identifier,
                          ast_t * body)
    : ast_t (line, COUNT)
  {
    set (EXPR, expr);
    set (IDENTIFIER, identifier);
    set (BODY, body);
  }

  ast_t* expr () const
  {
    return at (EXPR);
  }
  ast_t*& expr_ref ()
  {
    return at (EXPR);
  }
  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  Symbol* root_symbol;
};

struct ast_expression_statement_t : public ast_unary_t
{
  ast_expression_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "expression_statement";
  }
};

struct ast_if_statement_t : public ast_t
{
  enum
  {
    CONDITION,
    TRUE_BRANCH,
    FALSE_BRANCH,
    COUNT,
  };

  ast_if_statement_t (unsigned int line, ast_t* condition, ast_t* true_branch, ast_t* false_branch)
    : ast_t (line, COUNT)
  {
    set (CONDITION, condition);
    set (TRUE_BRANCH, true_branch);
    set (FALSE_BRANCH, false_branch);
  }

  ast_t* condition () const
  {
    return at (CONDITION);
  }
  ast_t*& condition_ref ()
  {
    return at (CONDITION);
  }
  ast_t* true_branch () const
  {
    return at (TRUE_BRANCH);
  }
  ast_t* false_branch () const
  {
    return at (FALSE_BRANCH);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_while_statement_t : public ast_t
{
  enum
  {
    CONDITION,
    BODY,
    COUNT
  };

  ast_while_statement_t (unsigned int line, ast_t* condition, ast_t* body)
    : ast_t (line, COUNT)
  {
    set (CONDITION, condition);
    set (BODY, body);
  }

  ast_t* condition () const
  {
    return at (CONDITION);
  }
  ast_t*& condition_ref ()
  {
    return at (CONDITION);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_return_statement_t : public ast_unary_t
{
  ast_return_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
    , return_symbol (NULL)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "return_statement";
  }

  const ParameterSymbol* return_symbol;
};

struct ast_increment_statement_t : public ast_unary_t
{
  ast_increment_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_decrement_statement_t : public ast_unary_t
{
  ast_decrement_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_list_statement_t : public ast_t
{
  ast_list_statement_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "list_statement";
  }
};

struct ast_subtract_assign_statement_t : public ast_binary_t
{
  ast_subtract_assign_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_activate_statement_t : public ast_t
{
  enum
  {
    EXPR_LIST,
    BODY,
    COUNT
  };

  ast_activate_statement_t (unsigned int line, ast_t * expr_list, ast_t * body)
    : ast_t (line, COUNT)
    , activation (NULL)
  {
    set (EXPR_LIST, expr_list);
    set (BODY, body);
  }

  ast_t* expr_list () const
  {
    return at (EXPR_LIST);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  Symbol* this_symbol;
  Activation* activation;

  virtual Activation*
  GetActivation () const
  {
    return activation;
  }

};

struct ast_var_statement_t : public ast_t
{
  enum
  {
    IDENTIFIER_LIST,
    TYPE_SPEC,
    EXPRESSION_LIST,
    COUNT
  };

  ast_var_statement_t (unsigned int line, ast_t* identifier_list, Mutability m, Mutability dm, ast_t* type_spec, ast_t* expression_list)
    : ast_t (line, COUNT)
    , mutability (m)
    , dereferenceMutability (dm)
  {
    set (IDENTIFIER_LIST, identifier_list);
    set (TYPE_SPEC, type_spec);
    set (EXPRESSION_LIST, expression_list);
  }

  ast_t* identifier_list () const
  {
    return at (IDENTIFIER_LIST);
  }
  ast_t* type_spec () const
  {
    return at (TYPE_SPEC);
  }
  ast_t* expression_list () const
  {
    return at (EXPRESSION_LIST);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "var_statement";
  }

  Mutability const mutability;
  Mutability const dereferenceMutability;
  std::vector<Symbol*> symbols;
};

struct ast_bind_push_port_statement_t : public ast_binary_t
{
  ast_bind_push_port_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_bind_push_port_param_statement_t : public ast_t
{
  enum
  {
    LEFT,
    RIGHT,
    PARAM,
    COUNT,
  };

  ast_bind_push_port_param_statement_t (unsigned int line, ast_t* left, ast_t* right, ast_t* param)
    : ast_t (line, COUNT)
  {
    set (LEFT, left);
    set (RIGHT, right);
    set (PARAM, param);
  }

  ast_t* left () const
  {
    return at (LEFT);
  }
  iterator left_iter ()
  {
    return begin () + LEFT;
  }
  ast_t* right () const
  {
    return at (RIGHT);
  }
  ast_t*& right_ref ()
  {
    return at (RIGHT);
  }
  ast_t* param () const
  {
    return at (PARAM);
  }
  ast_t*& param_ref ()
  {
    return at (PARAM);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_bind_pull_port_statement_t : public ast_binary_t
{
  ast_bind_pull_port_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_for_iota_statement_t : public ast_t
{
  enum
  {
    IDENTIFIER,
    LIMIT,
    BODY,
    COUNT
  };

  ast_for_iota_statement_t (unsigned int line, ast_t* identifier, ast_t* limit, ast_t* body)
    : ast_t (line, COUNT)
  {
    set (IDENTIFIER, identifier);
    set (LIMIT, limit);
    set (BODY, body);
  }

  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* limit_node () const
  {
    return at (LIMIT);
  }
  ast_t*& limit_node_ref ()
  {
    return at (LIMIT);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  Symbol* symbol;
  typed_value_t limit;
};

struct ast_action_t : public ast_t
{
  enum
  {
    RECEIVER,
    IDENTIFIER,
    PRECONDITION,
    BODY,
    COUNT
  };

  ast_action_t (unsigned int line, ast_t* receiver,
                ast_t* precondition, ast_t* body)
    : ast_t (line, COUNT)
    , action (NULL)
  {
    set (RECEIVER, receiver);
    set (IDENTIFIER, new ast_identifier_t (line, ""));
    set (PRECONDITION, precondition);
    set (BODY, body);
  }

  ast_t* receiver () const
  {
    return at (RECEIVER);
  }
  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* precondition () const
  {
    return at (PRECONDITION);
  }
  ast_t*& precondition_ref ()
  {
    return at (PRECONDITION);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  action_t* action;


  virtual Symbol *
  GetReceiverSymbol () const;

  virtual action_reaction_base_t*
  GetAction () const
  {
    return action;
  }

  virtual const Type::Type*
  GetReceiverType () const
  {
    return action->type ();
  }
};

struct ast_dimensioned_action_t : public ast_t
{
  enum
  {
    DIMENSION,
    RECEIVER,
    IDENTIFIER,
    PRECONDITION,
    BODY,
    COUNT
  };

  ast_dimensioned_action_t (unsigned int line, ast_t* dimension,
                            ast_t* receiver, ast_t* precondition, ast_t* body)
    : ast_t (line, COUNT)
    , action (NULL)
  {
    set (DIMENSION, dimension);
    set (RECEIVER, receiver);
    set (IDENTIFIER, new ast_identifier_t (line, ""));
    set (PRECONDITION, precondition);
    set (BODY, body);
  }

  ast_t* dimension () const
  {
    return at (DIMENSION);
  }
  ast_t*& dimension_ref ()
  {
    return at (DIMENSION);
  }
  ast_t* receiver () const
  {
    return at (RECEIVER);
  }
  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* precondition () const
  {
    return at (PRECONDITION);
  }
  ast_t*& precondition_ref ()
  {
    return at (PRECONDITION);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  action_t* action;

  virtual Symbol *
  GetReceiverSymbol () const;

  virtual action_reaction_base_t*
  GetAction () const
  {
    return action;
  }

  virtual const Type::Type*
  GetReceiverType () const
  {
    return action->type ();
  }
};

struct ast_bind_t : public ast_t
{
  enum
  {
    RECEIVER,
    IDENTIFIER,
    BODY,
    COUNT,
  };

  ast_bind_t (unsigned int line, ast_t * receiver, ast_t * body)
    : ast_t (line, COUNT)
    , bind (NULL)
  {
    set (RECEIVER, receiver);
    set (IDENTIFIER, new ast_identifier_t (line, ""));
    set (BODY, body);
  }

  ast_t* receiver () const
  {
    return at (RECEIVER);
  }
  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  bind_t* bind;
};

struct ast_function_t : public ast_t
{
  enum
  {
    IDENTIFIER,
    SIGNATURE,
    RETURN_TYPE,
    BODY,
    COUNT
  };

  ast_function_t (unsigned int line, ast_t* identifier, ast_t* signature,
                  Mutability dm, ast_t* return_type, ast_t* body)
    : ast_t (line, COUNT)
    , function (NULL)
    , dereferenceMutability (dm)
  {
    set (IDENTIFIER, identifier);
    set (SIGNATURE, signature);
    set (RETURN_TYPE, return_type);
    set (BODY, body);
  }

  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* signature () const
  {
    return at (SIGNATURE);
  }
  ast_t* return_type () const
  {
    return at (RETURN_TYPE);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  virtual ParameterSymbol *
  GetReturnSymbol () const
  {
    return return_symbol;
  }

  // TODO:  Eliminate redundancy.
  Function* function;
  ParameterSymbol* return_symbol;
  Mutability const dereferenceMutability;
};

struct ast_instance_t : public ast_t
{
  enum
  {
    IDENTIFIER,
    TYPE_IDENTIFIER,
    INITIALIZER,
    EXPRESSION_LIST,
    COUNT
  };

  ast_instance_t (unsigned int line, ast_t* identifier, ast_t* type_identifier, ast_t* initializer, ast_t* expression_list)
    : ast_t (line, COUNT)
  {
    set (IDENTIFIER, identifier);
    set (TYPE_IDENTIFIER, type_identifier);
    set (INITIALIZER, initializer);
    set (EXPRESSION_LIST, expression_list);
  }

  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* type_identifier () const
  {
    return at (TYPE_IDENTIFIER);
  }
  ast_t* initializer () const
  {
    return at (INITIALIZER);
  }
  ast_t* expression_list () const
  {
    return at (EXPRESSION_LIST);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  InstanceSymbol* symbol;
};

struct ast_const_t : public ast_t
{
  enum
  {
    IDENTIFIER_LIST,
    TYPE_SPEC,
    EXPRESSION_LIST,
    COUNT
  };

  ast_const_t (unsigned int line, ast_t* identifier_list, ast_t* type_spec, ast_t* expression_list)
    : ast_t (line, COUNT)
  {
    set (IDENTIFIER_LIST, identifier_list);
    set (TYPE_SPEC, type_spec);
    set (EXPRESSION_LIST, expression_list);
  }

  ast_t* identifier_list () const
  {
    return at (IDENTIFIER_LIST);
  }
  ast_t* type_spec () const
  {
    return at (TYPE_SPEC);
  }
  ast_t* expression_list () const
  {
    return at (EXPRESSION_LIST);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "ast_const_t";
  }

  TypedConstantSymbol* symbol;
};

struct ast_method_t : public ast_t
{
  enum
  {
    RECEIVER,
    IDENTIFIER,
    SIGNATURE,
    RETURN_TYPE,
    BODY,
    COUNT
  };

  ast_method_t (unsigned int line,
                ast_t * receiver,
                ast_t * identifier,
                ast_t * signature,
                Mutability return_dm,
                ast_t * return_type,
                ast_t* body)
    : ast_t (line, COUNT)
    , method (NULL)
    , return_dereference_mutability (return_dm)
  {
    set (RECEIVER, receiver);
    set (IDENTIFIER, identifier);
    set (SIGNATURE, signature);
    set (RETURN_TYPE, return_type);
    set (BODY, body);
  }

  ast_t* receiver () const
  {
    return at (RECEIVER);
  }
  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* signature () const
  {
    return at (SIGNATURE);
  }
  ast_t* return_type () const
  {
    return at (RETURN_TYPE);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  Method* method;
  Mutability const return_dereference_mutability;
  ParameterSymbol* return_symbol;

  virtual ParameterSymbol *
  GetReturnSymbol () const
  {
    return return_symbol;
  }

};

struct ast_getter_t : public ast_t
{
  enum
  {
    RECEIVER,
    IDENTIFIER,
    SIGNATURE,
    RETURN_TYPE,
    BODY,
    COUNT
  };

  ast_getter_t (unsigned int line,
                ast_t * receiver,
                ast_t * identifier,
                ast_t * signature,
                Mutability dm,
                ast_t * return_type,
                ast_t* body)
    : ast_t (line, COUNT)
    , getter (NULL)
    , dereferenceMutability (dm)
  {
    set (RECEIVER, receiver);
    set (IDENTIFIER, identifier);
    set (SIGNATURE, signature);
    set (RETURN_TYPE, return_type);
    set (BODY, body);
  }

  ast_t* receiver () const
  {
    return at (RECEIVER);
  }
  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* signature () const
  {
    return at (SIGNATURE);
  }
  ast_t* return_type () const
  {
    return at (RETURN_TYPE);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "getter";
  }

  Getter* getter;
  Mutability const dereferenceMutability;
  ParameterSymbol* return_symbol;

  virtual ParameterSymbol *
  GetReturnSymbol () const
  {
    return return_symbol;
  }

  virtual Getter*
  GetGetter () const
  {
    return getter;
  }
};

struct ast_initializer_t : public ast_t
{
  enum
  {
    RECEIVER,
    IDENTIFIER,
    SIGNATURE,
    RETURN_TYPE,
    BODY,
    COUNT
  };

  ast_initializer_t (unsigned int line,
                     ast_t* receiver,
                     ast_t * identifier,
                     ast_t * signature,
                     Mutability return_dm,
                     ast_t* return_type,
                     ast_t* body)
    : ast_t (line, COUNT)
    , return_dereference_mutability (return_dm)
    , initializer (NULL)
  {
    set (RECEIVER, receiver);
    set (IDENTIFIER, identifier);
    set (SIGNATURE, signature);
    set (RETURN_TYPE, return_type);
    set (BODY, body);
  }

  ast_t* receiver () const
  {
    return at (RECEIVER);
  }
  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* signature () const
  {
    return at (SIGNATURE);
  }
  ast_t* return_type () const
  {
    return at (RETURN_TYPE);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    out << "initializer";
  }

  Mutability const return_dereference_mutability;
  Initializer* initializer;
  Symbol* return_symbol;

  virtual const Type::Type*
  GetReceiverType () const
  {
    unimplemented;
  }

  virtual Initializer*
  GetInitializer () const
  {
    return initializer;
  }
};

struct ast_reaction_t : public ast_t
{
  enum
  {
    RECEIVER,
    IDENTIFIER,
    SIGNATURE,
    RETURN_TYPE,
    BODY,
    COUNT,
  };

  ast_reaction_t (unsigned int line, ast_t* receiver, ast_t* identifier, ast_t* signature, ast_t* body)
    : ast_t (line, COUNT)
    , reaction (NULL)
  {
    set (RECEIVER, receiver);
    set (IDENTIFIER, identifier);
    set (SIGNATURE, signature);
    set (RETURN_TYPE, new ast_empty_type_spec_t (line));
    set (BODY, body);
  }

  ast_t* receiver () const
  {
    return at (RECEIVER);
  }
  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* signature () const
  {
    return at (SIGNATURE);
  }
  ast_t* return_type () const
  {
    return at (RETURN_TYPE);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  reaction_t* reaction;

  virtual Symbol *
  GetReceiverSymbol () const;

  virtual action_reaction_base_t*
  GetAction () const
  {
    return reaction;
  }

  virtual const Type::Type*
  GetReceiverType () const
  {
    return reaction->type ();
  }
};

struct ast_dimensioned_reaction_t : public ast_t
{
  enum
  {
    DIMENSION,
    RECEIVER,
    IDENTIFIER,
    SIGNATURE,
    RETURN_TYPE,
    BODY,
    COUNT,
  };

  ast_dimensioned_reaction_t (unsigned int line,
                              ast_t* dimension,
                              ast_t* receiver,
                              ast_t* identifier,
                              ast_t* signature,
                              ast_t* body)
    : ast_t (line, COUNT)
    , reaction (NULL)
  {
    set (DIMENSION, dimension);
    set (RECEIVER, receiver);
    set (IDENTIFIER, identifier);
    set (SIGNATURE, signature);
    set (RETURN_TYPE, new ast_empty_type_spec_t (line));
    set (BODY, body);
  }

  ast_t* dimension () const
  {
    return at (DIMENSION);
  }
  ast_t*& dimension_ref ()
  {
    return at (DIMENSION);
  }
  ast_t* receiver () const
  {
    return at (RECEIVER);
  }
  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* signature () const
  {
    return at (SIGNATURE);
  }
  ast_t* return_type () const
  {
    return at (RETURN_TYPE);
  }
  ast_t* body () const
  {
    return at (BODY);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  reaction_t* reaction;

  virtual Symbol *
  GetReceiverSymbol () const;

  virtual action_reaction_base_t*
  GetAction () const
  {
    return reaction;
  }

  virtual const Type::Type*
  GetReceiverType () const
  {
    unimplemented;
  }
};

struct ast_type_definition_t : public ast_t
{
  enum
  {
    IDENTIFIER,
    TYPE_SPEC,
    COUNT
  };

  ast_type_definition_t (unsigned int line, ast_t* identifier, ast_t* type_spec)
    : ast_t (line, COUNT)
  {
    set (IDENTIFIER, identifier);
    set (TYPE_SPEC, type_spec);
  }

  ast_t* identifier () const
  {
    return at (IDENTIFIER);
  }
  ast_t* type_spec () const
  {
    return at (TYPE_SPEC);
  }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }

  TypeSymbol* symbol;
};

struct ast_top_level_list_t : public ast_t
{
  ast_top_level_list_t ()
    : ast_t (-1, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const
  {
    unimplemented;
  }
};

struct ast_visitor_t
{
  virtual void visit (ast_identifier_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_identifier_list_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_receiver_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (ast_array_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_slice_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_empty_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_enum_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_field_list_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_heap_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_identifier_list_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_identifier_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_pointer_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_push_port_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_pull_port_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_signature_type_spec_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (ast_cast_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_implicit_conversion_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_type_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_binary_arithmetic_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_address_of_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_call_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_dereference_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_implicit_dereference_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_identifier_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_index_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_slice_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_indexed_port_call_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_list_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_literal_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_logic_not_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_push_port_call_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_select_expr_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (ast_empty_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_add_assign_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_change_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_assign_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_expression_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_if_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_while_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_list_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_return_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_increment_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_decrement_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_subtract_assign_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_activate_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_var_statement_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (ast_bind_push_port_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_bind_push_port_param_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_bind_pull_port_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_for_iota_statement_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (ast_action_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_const_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_dimensioned_action_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_bind_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_function_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_getter_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_initializer_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_instance_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_method_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_reaction_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_dimensioned_reaction_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (ast_type_definition_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (ast_top_level_list_t& ast)
  {
    default_action (ast);
  }

  virtual void default_action (ast_t& ast) { }
};

struct ast_const_visitor_t
{
  virtual void visit (const ast_identifier_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_identifier_list_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_receiver_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (const ast_array_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_slice_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_empty_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_enum_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_field_list_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_heap_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_identifier_list_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_identifier_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_pointer_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_push_port_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_pull_port_type_spec_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_signature_type_spec_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (const ast_cast_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_implicit_conversion_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_type_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_binary_arithmetic_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_address_of_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_call_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_dereference_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_implicit_dereference_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_identifier_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_index_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_slice_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_indexed_port_call_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_list_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_literal_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_logic_not_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_push_port_call_expr_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_select_expr_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (const ast_empty_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_add_assign_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_change_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_assign_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_expression_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_if_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_while_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_list_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_return_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_increment_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_decrement_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_subtract_assign_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_activate_statement_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (const ast_bind_push_port_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_bind_push_port_param_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_bind_pull_port_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_for_iota_statement_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_var_statement_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (const ast_action_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_const_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_dimensioned_action_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_bind_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_function_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_getter_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_initializer_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_instance_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_method_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_reaction_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_dimensioned_reaction_t& ast)
  {
    default_action (ast);
  }
  virtual void visit (const ast_type_definition_t& ast)
  {
    default_action (ast);
  }

  virtual void visit (const ast_top_level_list_t& ast)
  {
    default_action (ast);
  }

  virtual void default_action (const ast_t& ast) { }
};

template<typename T>
T*
ast_cast (ast_t* node)
{
  if (node == NULL) return NULL;

  struct visitor : public ast_visitor_t
  {
    T* retval;

    visitor () : retval (NULL) { }

    void visit (T& type)
    {
      retval = &type;
    }
  };
  visitor v;
  node->accept (v);
  return v.retval;
}

/* Generic functions. */

std::string ast_get_identifier (const ast_t* ast);

void ast_instance_set_type (ast_t * ast, Type::Type * type);

Type::Type *ast_instance_get_type (ast_t * ast);

Method*
get_current_method (const ast_t * node);

Function*
get_current_function (const ast_t * node);

#define ast_not_reached(node) do { std::cerr << node; not_reached; } while (0);

#endif /* ast_hpp */
