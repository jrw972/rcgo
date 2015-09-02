#ifndef ast_hpp
#define ast_hpp

#include "symtab.hpp"
#include "debug.hpp"
#include <vector>

class ast_visitor_t;
class ast_const_visitor_t;

struct ast_t {
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

  ChildrenType children;
  location_t const location;
  symtab_t *symtab;
  typed_value_t typed_value;

  size_t size () const { return children.size (); }

  ast_t * append (ast_t * child)
  {
    children.push_back (child);
    return this;
  }

  ast_t* prepend (ast_t * child)
  {
    children.insert (children.begin (), child);
    return this;
  }

  ast_t *
  at (size_t idx) const
  {
    assert (idx < children.size ());
    return children.at (idx);
  }

  void
  set (size_t idx, ast_t * child)
  {
    assert (idx < children.size ());
    children[idx] = child;
  }

  iterator
  get_child_ptr (size_t idx)
  {
    assert (idx < this->children.size ());
    return this->children.begin () + idx;
  }

  iterator begin () { return children.begin (); }
  const_iterator begin () const { return children.begin (); }

  iterator end () { return children.end (); }
  const_iterator end () const { return children.end (); }

protected:
  ast_t (unsigned int line_, size_t children_count)
    : location (in_file, line_)
    , symtab (NULL)
  {
    assert (location.line != 0);
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
  void print (std::ostream& out) const { out << "identifier " << identifier; }

  std::string const identifier;
};

struct ast_identifier_list_t : public ast_t
{
  ast_identifier_list_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "identifier_list"; }
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
    children[DIMENSION] = dimension;
    children[BASE_TYPE] = base_type;
  }

  ast_t* dimension () const { return children[DIMENSION]; }
  ast_t* base_type () const { return children[BASE_TYPE]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
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
    children[CHILD] = child;
  }

  ast_t* child () const { return children[CHILD]; }
  iterator child_iter () { return begin () + CHILD; }
};

struct ast_component_type_spec_t : public ast_unary_t
{
  ast_component_type_spec_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_empty_type_spec_t : public ast_t
{
  ast_empty_type_spec_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
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
    children[VALUES] = values;
  }

  ast_t* values () const { return children[VALUES]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_field_list_type_spec_t : public ast_t
{
  ast_field_list_type_spec_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_heap_type_spec_t : public ast_unary_t
{
  ast_heap_type_spec_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_identifier_type_spec_t : public ast_unary_t
{
  ast_identifier_type_spec_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "identifier_type"; }
};

struct ast_identifier_list_type_spec_t : public ast_t
{
  enum
    {
      IDENTIFIER_LIST,
      TYPE_SPEC,
      COUNT,
    };

  ast_identifier_list_type_spec_t (unsigned int line, ast_t* identifier_list, ast_t* type_spec, Mutability dereference_mutability_)
    : ast_t (line, COUNT)
    , dereference_mutability (dereference_mutability_)
  {
    children[IDENTIFIER_LIST] = identifier_list;
    children[TYPE_SPEC] = type_spec;
  }

  const Mutability dereference_mutability;

  ast_t* identifier_list () const { return children[IDENTIFIER_LIST]; }
  ast_t* type_spec () const { return children[TYPE_SPEC]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_pointer_type_spec_t : public ast_unary_t
{
  ast_pointer_type_spec_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
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
    children[SIGNATURE] = signature;
  }

  ast_t* signature () const { return children[SIGNATURE]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_pull_port_type_spec_t : public ast_t
{
  enum
    {
      SIGNATURE,
      RETURN_TYPE,
      COUNT,
    };

  ast_pull_port_type_spec_t (unsigned int line, ast_t* signature, ast_t* return_type)
    : ast_t (line, COUNT)
  {
    children[SIGNATURE] = signature;
    children[RETURN_TYPE] = return_type;
  }

  ast_t* signature () const { return children[SIGNATURE]; }
  ast_t* return_type () const { return children[RETURN_TYPE]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "pull_port_type_spec"; }
};

struct ast_signature_type_spec_t : public ast_t
{
  ast_signature_type_spec_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "signature_type_spec"; }
};

struct ast_struct_type_spec_t : public ast_unary_t
{
  ast_struct_type_spec_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

class ast_expr_t : public ast_t
{
public:
  ast_expr_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }
};

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
    children[TYPE_SPEC] = type_spec;
    children[CHILD] = child;
  }

  ast_t* type_spec () const { return children[TYPE_SPEC]; }
  ast_t* child () const { return children[CHILD]; }

  virtual void accept (ast_visitor_t& visitor);
  virtual void accept (ast_const_visitor_t& visitor) const;
  virtual void print (std::ostream& out) const { out << "cast"; }
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
    children[CHILD] = child;
  }

  ast_t* child () const { return children[CHILD]; }
  iterator child_iter () { return begin () + CHILD; }
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
    children[LEFT] = left;
    children[RIGHT] = right;
  }

  ast_t* left () const { return children[LEFT]; }
  iterator left_iter () { return begin () + LEFT; }
  ast_t* right () const { return children[RIGHT]; }
  iterator right_iter () { return begin () + RIGHT; }
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

struct ast_address_of_expr_t : public ast_unary_expr_t
{
  ast_address_of_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_call_expr_t : public ast_expr_t
{
  enum
    {
      EXPR,
      ARGS,
      COUNT,
    };

  enum Kind
    {
      NONE,
      FUNCTION,
      METHOD,
      INITIALIZER,
      GETTER,
      PULL_PORT,
    };

  ast_call_expr_t (unsigned int line, ast_t* expr, ast_t* args)
    : ast_expr_t (line, COUNT)
    , kind (NONE)
  {
    children[EXPR] = expr;
    children[ARGS] = args;
  }

  ast_t* expr () const { return children[EXPR]; }
  iterator expr_iter () { return begin () + EXPR; }
  ast_t* args () const { return children[ARGS]; }
  iterator args_iterator () { return begin () + ARGS; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "call_expr"; }

  Kind kind;
};

struct ast_dereference_expr_t : public ast_unary_expr_t
{
  ast_dereference_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "dereference_expr"; }
};

struct ast_implicit_dereference_expr_t : public ast_unary_expr_t
{
  ast_implicit_dereference_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "implicit dereference"; }
};

struct ast_list_expr_t : public ast_expr_t
{
  ast_list_expr_t (unsigned int line)
    : ast_expr_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "list_expr"; }
};

struct ast_identifier_expr_t : public ast_unary_expr_t
{
  ast_identifier_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "identifier_expr"; }

  symbol_holder symbol;
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
    children[BASE] = base;
    children[INDEX] = index;
  }

  ast_t* base () const { return children[BASE]; }
  iterator base_iter () { return begin () + BASE; }
  ast_t* index () const { return children[INDEX]; }
  iterator index_iter () { return begin () + INDEX; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_logic_not_expr_t : public ast_unary_expr_t
{
  ast_logic_not_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "logic_not_expr"; }
};

struct ast_merge_expr_t : public ast_unary_expr_t
{
  ast_merge_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_move_expr_t : public ast_unary_expr_t
{
  ast_move_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_new_expr_t : public ast_unary_expr_t
{
  ast_new_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
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
    children[IDENTIFIER] = identifier;
    children[ARGS] = args;
  }

  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* args () const { return children[ARGS]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

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
    children[IDENTIFIER] = identifier;
    children[INDEX] = index;
    children[ARGS] = args;
  }

  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* index () const { return children[INDEX]; }
  iterator index_iter () { return children.begin () + INDEX; }
  ast_t* args () const { return children[ARGS]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  field_t* field;
  const array_type_t* array_type;
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
    children[BASE] = base;
    children[IDENTIFIER] = identifier;
  }

  ast_t* base () const { return children[BASE]; }
  iterator base_iter () { return begin () + BASE; }
  ast_t* identifier () const { return children[IDENTIFIER]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "select_expr"; }
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
  void print (std::ostream& out) const { out << "literal"; }
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
    children[LEFT] = left;
    children[RIGHT] = right;
  }

  ast_t* left () const { return children[LEFT]; }
  iterator left_iter () { return begin () + LEFT; }
  ast_t* right () const { return children[RIGHT]; }
  iterator right_iter () { return begin () + RIGHT; }
};

struct ast_empty_statement_t : public ast_t
{
  ast_empty_statement_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_add_assign_statement_t : public ast_binary_t
{
  ast_add_assign_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_assign_statement_t : public ast_binary_t
{
  ast_assign_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "assign_statement"; }
};

struct ast_change_statement_t : public ast_t
{
  enum
    {
      EXPR,
      IDENTIFIER,
      TYPE,
      BODY,
      COUNT
    };

  ast_change_statement_t (unsigned int line,
                          ast_t * expr,
                          ast_t * identifier,
                          ast_t * type,
                          ast_t * body)
    : ast_t (line, COUNT)
  {
    children[EXPR] = expr;
    children[IDENTIFIER] = identifier;
    children[TYPE] = type;
    children[BODY] = body;
  }

  ast_t* expr () const { return children[EXPR]; }
  iterator expr_iter () { return begin () + EXPR; }
  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* type () const { return children[TYPE]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  symbol_holder root_symbol;
};

struct ast_expression_statement_t : public ast_unary_t
{
  ast_expression_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "expression_statement"; }
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
    children[CONDITION] = condition;
    children[TRUE_BRANCH] = true_branch;
    children[FALSE_BRANCH] = false_branch;
  }

  ast_t* condition () const { return children[CONDITION]; }
  iterator condition_iter () { return begin () + CONDITION; }
  ast_t* true_branch () const { return children[TRUE_BRANCH]; }
  ast_t* false_branch () const { return children[FALSE_BRANCH]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
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
    children[CONDITION] = condition;
    children[BODY] = body;
  }

  ast_t* condition () const { return children[CONDITION]; }
  iterator condition_iter () { return begin () + CONDITION; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_println_statement_t : public ast_unary_t
{
  ast_println_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "println_statement"; }
};

struct ast_return_statement_t : public ast_unary_t
{
  ast_return_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
    , return_symbol (NULL)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "return_statement"; }

  const symbol_t* return_symbol;
};

struct ast_increment_statement_t : public ast_unary_t
{
  ast_increment_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_decrement_statement_t : public ast_unary_t
{
  ast_decrement_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_list_statement_t : public ast_t
{
  ast_list_statement_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "list_statement"; }
};

struct ast_subtract_assign_statement_t : public ast_binary_t
{
  ast_subtract_assign_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_trigger_statement_t : public ast_t
{
  enum
    {
      EXPR_LIST,
      BODY,
      COUNT
    };

  ast_trigger_statement_t (unsigned int line, ast_t * expr_list, ast_t * body)
    : ast_t (line, COUNT)
  {
    children[EXPR_LIST] = expr_list;
    children[BODY] = body;
  }

  ast_t* expr_list () const { return children[EXPR_LIST]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  symbol_holder this_symbol;
};

struct ast_var_statement_t : public ast_t
{
  enum
    {
      IDENTIFIER_LIST,
      TYPE_SPEC,
      COUNT
    };

  ast_var_statement_t (unsigned int line, ast_t* identifier_list, ast_t* type_spec)
    : ast_t (line, COUNT)
  {
    children[IDENTIFIER_LIST] = identifier_list;
    children[TYPE_SPEC] = type_spec;
  }

  ast_t* identifier_list () const { return children[IDENTIFIER_LIST]; }
  ast_t* type_spec () const { return children[TYPE_SPEC]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "var_statement"; }

  std::vector<symbol_holder> symbols;
};

struct ast_var_type_init_statement_t : public ast_t
{
  enum
    {
      IDENTIFIER_LIST,
      TYPE_SPEC,
      INITIALIZER_LIST,
      COUNT
    };

  ast_var_type_init_statement_t (unsigned int line, ast_t* identifier_list, ast_t* type_spec, ast_t* initializer_list)
    : ast_t (line, COUNT)
  {
    children[IDENTIFIER_LIST] = identifier_list;
    children[TYPE_SPEC] = type_spec;
    children[INITIALIZER_LIST] = initializer_list;
  }

  ast_t* identifier_list () const { return children[IDENTIFIER_LIST]; }
  ast_t* type_spec () const { return children[TYPE_SPEC]; }
  ast_t* initializer_list () const { return children[INITIALIZER_LIST]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "var_type_init_statement"; }

  std::vector<symbol_holder> symbols;
};

struct ast_bind_push_port_statement_t : public ast_binary_t
{
  ast_bind_push_port_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
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
    children[LEFT] = left;
    children[RIGHT] = right;
    children[PARAM] = param;
  }

  ast_t* left () const { return children[LEFT]; }
  iterator left_iter () { return begin () + LEFT; }
  ast_t* right () const { return children[RIGHT]; }
  iterator right_iter () { return begin () + RIGHT; }
  ast_t* param () const { return children[PARAM]; }
  iterator param_iter () { return begin () + PARAM; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_bind_pull_port_statement_t : public ast_binary_t
{
  ast_bind_pull_port_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
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
    children[IDENTIFIER] = identifier;
    children[LIMIT] = limit;
    children[BODY] = body;
  }

  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* limit_node () const { return children[LIMIT]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  symbol_holder symbol;
  typed_value_t limit;
};

struct ast_action_t : public ast_t
{
  enum
    {
      THIS_IDENTIFIER,
      TYPE_IDENTIFIER,
      PRECONDITION,
      BODY,
      COUNT
    };

  ast_action_t (unsigned int line, ast_t* this_identifier, ast_t* type_identifier, ast_t* precondition, ast_t* body)
    : ast_t (line, COUNT)
    , action (NULL)
  {
    children[THIS_IDENTIFIER] = this_identifier;
    children[TYPE_IDENTIFIER] = type_identifier;
    children[PRECONDITION] = precondition;
    children[BODY] = body;
  }

  ast_t* this_identifier () const { return children[THIS_IDENTIFIER]; }
  ast_t* type_identifier () const { return children[TYPE_IDENTIFIER]; }
  ast_t* precondition () const { return children[PRECONDITION]; }
  iterator precondition_iter () { return children.begin () + PRECONDITION; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  action_t* action;
  symbol_holder this_symbol;
};

struct ast_dimensioned_action_t : public ast_t
{
  enum
    {
      DIMENSION,
      THIS_IDENTIFIER,
      TYPE_IDENTIFIER,
      PRECONDITION,
      BODY,
      COUNT
    };

  ast_dimensioned_action_t (unsigned int line, ast_t* dimension, ast_t* this_identifier, ast_t* type_identifier, ast_t* precondition, ast_t* body)
    : ast_t (line, COUNT)
    , action (NULL)
  {
    children[DIMENSION] = dimension;
    children[THIS_IDENTIFIER] = this_identifier;
    children[TYPE_IDENTIFIER] = type_identifier;
    children[PRECONDITION] = precondition;
    children[BODY] = body;
  }

  ast_t* dimension () const { return children[DIMENSION]; }
  iterator dimension_iter () { return children.begin () + DIMENSION; }
  ast_t* this_identifier () const { return children[THIS_IDENTIFIER]; }
  ast_t* type_identifier () const { return children[TYPE_IDENTIFIER]; }
  ast_t* precondition () const { return children[PRECONDITION]; }
  iterator precondition_iter () { return children.begin () + PRECONDITION; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  symbol_holder this_symbol;
  symbol_holder iota_symbol;
  action_t* action;
};

struct ast_bind_t : public ast_t
{
  enum
    {
      THIS_IDENTIFIER,
      TYPE_IDENTIFIER,
      BODY,
      COUNT,
    };

  ast_bind_t (unsigned int line, ast_t * this_identifier, ast_t* type_identifier, ast_t * body)
    : ast_t (line, COUNT)
    , bind (NULL)
  {
    children[THIS_IDENTIFIER] = this_identifier;
    children[TYPE_IDENTIFIER] = type_identifier;
    children[BODY] = body;
  }

  ast_t* this_identifier () const { return children[THIS_IDENTIFIER]; }
  ast_t* type_identifier () const { return children[TYPE_IDENTIFIER]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  symbol_holder this_symbol;
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

  ast_function_t (unsigned int line, ast_t* identifier, ast_t* signature, ast_t* return_type, ast_t* body)
    : ast_t (line, COUNT)
    , function (NULL)
  {
    children[IDENTIFIER] = identifier;
    children[SIGNATURE] = signature;
    children[RETURN_TYPE] = return_type;
    children[BODY] = body;
  }

  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* signature () const { return children[SIGNATURE]; }
  ast_t* return_type () const { return children[RETURN_TYPE]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  symbol_holder function_symbol;
  symbol_holder return_symbol;
  function_t* function;
};

struct ast_instance_t : public ast_t
{
  enum
    {
      IDENTIFIER,
      TYPE_IDENTIFIER,
      INITIALIZER,
      COUNT
    };

  ast_instance_t (unsigned int line, ast_t* identifier, ast_t* type_identifier, ast_t* initializer)
    : ast_t (line, COUNT)
  {
    children[IDENTIFIER] = identifier;
    children[TYPE_IDENTIFIER] = type_identifier;
    children[INITIALIZER] = initializer;
  }

  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* type_identifier () const { return children[TYPE_IDENTIFIER]; }
  ast_t* initializer () const { return children[INITIALIZER]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  symbol_holder symbol;
};

struct ast_const_t : public ast_t
{
  enum
    {
      IDENTIFIER,
      TYPE_SPEC,
      EXPR,
      COUNT
    };

  ast_const_t (unsigned int line, ast_t* identifier, ast_t* type_spec, ast_t* expr)
    : ast_t (line, COUNT)
  {
    children[IDENTIFIER] = identifier;
    children[TYPE_SPEC] = type_spec;
    children[EXPR] = expr;
  }

  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* type_spec () const { return children[TYPE_SPEC]; }
  ast_t* expr () const { return children[EXPR]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "ast_const_t"; }

  symbol_holder symbol;
};

struct ast_method_t : public ast_t
{
  enum
    {
      THIS_IDENTIFIER,
      TYPE_IDENTIFIER,
      IDENTIFIER,
      SIGNATURE,
      RETURN_TYPE,
      BODY,
      COUNT
    };

  ast_method_t (unsigned int line, ast_t * this_identifier,
                ast_t* type_identifier, Mutability dm,
                ast_t * identifier,
                ast_t * signature,
                ast_t * return_type,
                Mutability return_dm,
                ast_t* body)
    : ast_t (line, COUNT)
    , method (NULL)
    , dereference_mutability (dm)
    , return_dereference_mutability (return_dm)
  {
    children[THIS_IDENTIFIER] = this_identifier;
    children[TYPE_IDENTIFIER] = type_identifier;
    children[IDENTIFIER] = identifier;
    children[SIGNATURE] = signature;
    children[RETURN_TYPE] = return_type;
    children[BODY] = body;
  }

  ast_t* this_identifier () const { return children[THIS_IDENTIFIER]; }
  ast_t* type_identifier () const { return children[TYPE_IDENTIFIER]; }
  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* signature () const { return children[SIGNATURE]; }
  ast_t* return_type () const { return children[RETURN_TYPE]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  method_t* method;
  Mutability const dereference_mutability;
  Mutability const return_dereference_mutability;
  symbol_holder return_symbol;
};

struct ast_getter_t : public ast_t
{
  enum
    {
      THIS_IDENTIFIER,
      TYPE_IDENTIFIER,
      IDENTIFIER,
      SIGNATURE,
      RETURN_TYPE,
      BODY,
      COUNT
    };

  ast_getter_t (unsigned int line,
                ast_t * this_identifier,
                ast_t* type_identifier,
                ast_t * identifier,
                ast_t * signature,
                ast_t * return_type,
                ast_t* body)
    : ast_t (line, COUNT)
    , getter (NULL)
  {
    children[THIS_IDENTIFIER] = this_identifier;
    children[TYPE_IDENTIFIER] = type_identifier;
    children[IDENTIFIER] = identifier;
    children[SIGNATURE] = signature;
    children[RETURN_TYPE] = return_type;
    children[BODY] = body;
  }

  ast_t* this_identifier () const { return children[THIS_IDENTIFIER]; }
  ast_t* type_identifier () const { return children[TYPE_IDENTIFIER]; }
  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* signature () const { return children[SIGNATURE]; }
  ast_t* return_type () const { return children[RETURN_TYPE]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "getter"; }

  getter_t* getter;
  symbol_holder return_symbol;
};

struct ast_initializer_t : public ast_t
{
  enum
    {
      THIS_IDENTIFIER,
      TYPE_IDENTIFIER,
      IDENTIFIER,
      SIGNATURE,
      BODY,
      COUNT
    };

  ast_initializer_t (unsigned int line, ast_t * this_identifier,
                     ast_t* type_identifier,
                     ast_t * identifier,
                     ast_t * signature,
                     ast_t* body)
    : ast_t (line, COUNT)
    , initializer (NULL)
  {
    children[THIS_IDENTIFIER] = this_identifier;
    children[TYPE_IDENTIFIER] = type_identifier;
    children[IDENTIFIER] = identifier;
    children[SIGNATURE] = signature;
    children[BODY] = body;
  }

  ast_t* this_identifier () const { return children[THIS_IDENTIFIER]; }
  ast_t* type_identifier () const { return children[TYPE_IDENTIFIER]; }
  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* signature () const { return children[SIGNATURE]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { out << "initializer"; }

  initializer_t* initializer;
  symbol_holder return_symbol;
};

struct ast_reaction_t : public ast_t
{
  enum
    {
      THIS_IDENTIFIER,
      TYPE_IDENTIFIER,
      IDENTIFIER,
      SIGNATURE,
      BODY,
      COUNT,
    };

  ast_reaction_t (unsigned int line, ast_t* this_identifier, ast_t* type_identifier, ast_t* identifier, ast_t* signature, ast_t* body)
    : ast_t (line, COUNT)
    , reaction (NULL)
  {
    children[THIS_IDENTIFIER] = this_identifier;
    children[TYPE_IDENTIFIER] = type_identifier;
    children[IDENTIFIER] = identifier;
    children[SIGNATURE] = signature;
    children[BODY] = body;
  }

  ast_t* this_identifier () const { return children[THIS_IDENTIFIER]; }
  ast_t* type_identifier () const { return children[TYPE_IDENTIFIER]; }
  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* signature () const { return children[SIGNATURE]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  reaction_t* reaction;
  symbol_holder this_symbol;
};

struct ast_dimensioned_reaction_t : public ast_t
{
  enum
    {
      DIMENSION,
      THIS_IDENTIFIER,
      TYPE_IDENTIFIER,
      IDENTIFIER,
      SIGNATURE,
      BODY,
      COUNT,
    };

  ast_dimensioned_reaction_t (unsigned int line, ast_t* dimension, ast_t* this_identifier, ast_t* type_identifier, ast_t* identifier, ast_t* signature, ast_t* body)
    : ast_t (line, COUNT)
    , reaction (NULL)
  {
    children[DIMENSION] = dimension;
    children[THIS_IDENTIFIER] = this_identifier;
    children[TYPE_IDENTIFIER] = type_identifier;
    children[IDENTIFIER] = identifier;
    children[SIGNATURE] = signature;
    children[BODY] = body;
  }

  ast_t* dimension () const { return children[DIMENSION]; }
  iterator dimension_iter () { return children.begin () + DIMENSION; }
  ast_t* this_identifier () const { return children[THIS_IDENTIFIER]; }
  ast_t* type_identifier () const { return children[TYPE_IDENTIFIER]; }
  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* signature () const { return children[SIGNATURE]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  symbol_holder this_symbol;
  symbol_holder iota_symbol;
  reaction_t* reaction;
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
    children[IDENTIFIER] = identifier;
    children[TYPE_SPEC] = type_spec;
  }

  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* type_spec () const { return children[TYPE_SPEC]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }

  symbol_holder symbol;
};

struct ast_top_level_list_t : public ast_t
{
  ast_top_level_list_t ()
    : ast_t (-1, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
  void print (std::ostream& out) const { unimplemented; }
};

struct ast_visitor_t
{
  virtual void visit (ast_identifier_t& ast) { default_action (ast); }
  virtual void visit (ast_identifier_list_t& ast) { default_action (ast); }

  virtual void visit (ast_array_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_component_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_empty_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_enum_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_field_list_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_heap_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_identifier_list_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_identifier_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_pointer_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_push_port_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_pull_port_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_signature_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_struct_type_spec_t& ast) { default_action (ast); }

  virtual void visit (ast_cast_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_binary_arithmetic_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_address_of_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_call_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_dereference_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_implicit_dereference_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_identifier_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_index_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_indexed_port_call_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_list_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_literal_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_logic_not_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_merge_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_move_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_new_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_push_port_call_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_select_expr_t& ast) { default_action (ast); }

  virtual void visit (ast_empty_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_add_assign_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_change_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_assign_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_expression_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_if_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_while_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_println_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_list_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_return_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_increment_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_decrement_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_subtract_assign_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_trigger_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_var_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_var_type_init_statement_t& ast) { default_action (ast); }

  virtual void visit (ast_bind_push_port_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_bind_push_port_param_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_bind_pull_port_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_for_iota_statement_t& ast) { default_action (ast); }

  virtual void visit (ast_action_t& ast) { default_action (ast); }
  virtual void visit (ast_const_t& ast) { default_action (ast); }
  virtual void visit (ast_dimensioned_action_t& ast) { default_action (ast); }
  virtual void visit (ast_bind_t& ast) { default_action (ast); }
  virtual void visit (ast_function_t& ast) { default_action (ast); }
  virtual void visit (ast_getter_t& ast) { default_action (ast); }
  virtual void visit (ast_initializer_t& ast) { default_action (ast); }
  virtual void visit (ast_instance_t& ast) { default_action (ast); }
  virtual void visit (ast_method_t& ast) { default_action (ast); }
  virtual void visit (ast_reaction_t& ast) { default_action (ast); }
  virtual void visit (ast_dimensioned_reaction_t& ast) { default_action (ast); }
  virtual void visit (ast_type_definition_t& ast) { default_action (ast); }

  virtual void visit (ast_top_level_list_t& ast) { default_action (ast); }

  virtual void default_action (ast_t& ast) { }
};

struct ast_const_visitor_t
{
  virtual void visit (const ast_identifier_t& ast) { default_action (ast); }
  virtual void visit (const ast_identifier_list_t& ast) { default_action (ast); }

  virtual void visit (const ast_array_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_component_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_empty_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_enum_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_field_list_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_heap_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_identifier_list_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_identifier_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_pointer_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_push_port_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_pull_port_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_signature_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_struct_type_spec_t& ast) { default_action (ast); }

  virtual void visit (const ast_cast_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_binary_arithmetic_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_address_of_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_call_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_dereference_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_implicit_dereference_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_identifier_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_index_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_indexed_port_call_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_list_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_literal_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_logic_not_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_merge_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_move_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_new_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_push_port_call_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_select_expr_t& ast) { default_action (ast); }

  virtual void visit (const ast_empty_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_add_assign_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_change_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_assign_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_expression_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_if_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_while_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_println_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_list_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_return_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_increment_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_decrement_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_subtract_assign_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_trigger_statement_t& ast) { default_action (ast); }

  virtual void visit (const ast_bind_push_port_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_bind_push_port_param_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_bind_pull_port_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_for_iota_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_var_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_var_type_init_statement_t& ast) { default_action (ast); }

  virtual void visit (const ast_action_t& ast) { default_action (ast); }
  virtual void visit (const ast_const_t& ast) { default_action (ast); }
  virtual void visit (const ast_dimensioned_action_t& ast) { default_action (ast); }
  virtual void visit (const ast_bind_t& ast) { default_action (ast); }
  virtual void visit (const ast_function_t& ast) { default_action (ast); }
  virtual void visit (const ast_getter_t& ast) { default_action (ast); }
  virtual void visit (const ast_initializer_t& ast) { default_action (ast); }
  virtual void visit (const ast_instance_t& ast) { default_action (ast); }
  virtual void visit (const ast_method_t& ast) { default_action (ast); }
  virtual void visit (const ast_reaction_t& ast) { default_action (ast); }
  virtual void visit (const ast_dimensioned_reaction_t& ast) { default_action (ast); }
  virtual void visit (const ast_type_definition_t& ast) { default_action (ast); }

  virtual void visit (const ast_top_level_list_t& ast) { default_action (ast); }

  virtual void default_action (const ast_t& ast) { }
};

/* Generic functions. */

void ast_set_symtab (ast_t * ast, symtab_t * symtab);

std::string ast_get_identifier (const ast_t* ast);

void ast_instance_set_type (ast_t * ast, type_t * type);

type_t *ast_instance_get_type (ast_t * ast);

named_type_t *
get_current_receiver_type (const ast_t * node);

trigger_t *
get_current_trigger (const ast_t * node);

action_reaction_base_t *
get_current_action (const ast_t * node);

method_t *
get_current_method (const ast_t * node);

initializer_t *
get_current_initializer (const ast_t * node);

getter_t *
get_current_getter (const ast_t * node);

function_t *
get_current_function (const ast_t * node);

const symbol_t*
get_current_return_symbol (const ast_t * node);

#define ast_not_reached(node) do { std::cerr << node; not_reached; } while (0);

#endif /* ast_hpp */
