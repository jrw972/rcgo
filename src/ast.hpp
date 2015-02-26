#ifndef ast_h
#define ast_h

#include "strtab.hpp"
#include "symtab.hpp"
#include "debug.hpp"
#include <vector>

class ast_visitor_t;
class ast_const_visitor_t;

struct ast_t {
  virtual ~ast_t() { }
  virtual void accept (ast_visitor_t& visitor) = 0;
  virtual void accept (ast_const_visitor_t& visitor) const = 0;

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
  const char * const file;
  unsigned int const line;
  symtab_t *symtab;

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
    : file (in_file)
    , line (line_)
    , symtab (NULL)
  {
    assert (line != 0);
    children.resize (children_count);
  }
};

class symbol_holder
{
public:
  symbol_holder ()
    : symbol_ (NULL)
  { }

  void symbol (symbol_t* s)
  {
    assert (symbol_ == NULL);
    symbol_ = s;
  }

  symbol_t* symbol () const { return symbol_; }

private:
  symbol_t* symbol_;
};

class ast_identifier_t : public ast_t
{
public:
  ast_identifier_t (unsigned int line, size_t children_count, string_t identifier)
    : ast_t (line, children_count)
    , identifier_ (identifier)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  string_t identifier () const { return identifier_; }

private:
  string_t identifier_;
};

struct ast_identifier_list_t : public ast_t
{
  ast_identifier_list_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_array_type_spec_t : public ast_t
{
  ast_array_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_component_type_spec_t : public ast_t
{
  ast_component_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_empty_type_spec_t : public ast_t
{
  ast_empty_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_field_list_type_spec_t : public ast_t
{
  ast_field_list_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_foreign_type_spec_t : public ast_t
{
  ast_foreign_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_heap_type_spec_t : public ast_t
{
  ast_heap_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_identifier_type_spec_t : public ast_t
{
  ast_identifier_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_identifier_list_type_spec_t : public ast_t
{
  ast_identifier_list_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_pointer_type_spec_t : public ast_t
{
  ast_pointer_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_pointer_to_immutable_type_spec_t : public ast_t
{
  ast_pointer_to_immutable_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_pointer_to_foreign_type_spec_t : public ast_t
{
  ast_pointer_to_foreign_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_port_type_spec_t : public ast_t
{
  ast_port_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_signature_type_spec_t : public ast_t
{
  ast_signature_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_struct_type_spec_t : public ast_t
{
  ast_struct_type_spec_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

class ast_expr_t : public ast_t
{
public:
  ast_expr_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void set_type (typed_value_t typed_value)
  {
    typed_value_ = typed_value;
  }

  typed_value_t get_type () const { return typed_value_; }

private:
  typed_value_t typed_value_;
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

enum Arithmetic
  {
    ADD,
    SUBTRACT,
  };

struct ast_binary_arithmetic_expr_t : public ast_binary_expr_t
{
  ast_binary_arithmetic_expr_t (unsigned int line, Arithmetic a, ast_t* left, ast_t* right)
    : ast_binary_expr_t (line, left, right)
    , arithmetic (a)
  { }

  const Arithmetic arithmetic;

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_address_of_expr_t : public ast_unary_expr_t
{
  ast_address_of_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
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
    children[EXPR] = expr;
    children[ARGS] = args;
  }

  ast_t* expr () const { return children[EXPR]; }
  iterator expr_iter () { return begin () + EXPR; }
  ast_t* args () const { return children[ARGS]; }
  iterator args_iterator () { return begin () + ARGS; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_dereference_expr_t : public ast_expr_t
{
  enum
    {
      CHILD,
      COUNT
    };

  ast_dereference_expr_t (unsigned int line, ast_t* child)
    : ast_expr_t (line, COUNT)
  {
    children[CHILD] = child;
  }

  ast_t* child () const { return children[CHILD]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_equal_expr_t : public ast_binary_expr_t
{
  ast_equal_expr_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_expr_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_list_expr_t : public ast_expr_t
{
  ast_list_expr_t (unsigned int line, size_t children_count)
    : ast_expr_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_identifier_expr_t : public ast_unary_expr_t
{
  ast_identifier_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

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
};

struct ast_logic_not_expr_t : public ast_unary_expr_t
{
  ast_logic_not_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_logic_and_expr_t : public ast_expr_t
{
  ast_logic_and_expr_t (unsigned int line, size_t children_count)
    : ast_expr_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_logic_or_expr_t : public ast_expr_t
{
  ast_logic_or_expr_t (unsigned int line, size_t children_count)
    : ast_expr_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_merge_expr_t : public ast_unary_expr_t
{
  ast_merge_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_move_expr_t : public ast_unary_expr_t
{
  ast_move_expr_t (unsigned int line, ast_t* child)
    : ast_unary_expr_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_new_expr_t : public ast_expr_t
{
  ast_new_expr_t (unsigned int line, size_t children_count)
    : ast_expr_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_not_equal_expr_t : public ast_binary_expr_t
{
  ast_not_equal_expr_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_expr_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_port_call_expr_t : public ast_expr_t
{
  enum
    {
      IDENTIFIER,
      ARGS,
      COUNT,
    };

  ast_port_call_expr_t (unsigned int line, ast_t* identifier, ast_t* args)
    : ast_expr_t (line, COUNT)
  {
    children[IDENTIFIER] = identifier;
    children[ARGS] = args;
  }

  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* args () const { return children[ARGS]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

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
    , field (NULL)
  {
    children[BASE] = base;
    children[IDENTIFIER] = identifier;
  }

  ast_t* base () const { return children[BASE]; }
  iterator base_iter () { return begin () + BASE; }
  ast_t* identifier () const { return children[IDENTIFIER]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  field_t* field;
};

struct ast_literal_expr_t : public ast_expr_t
{
  ast_literal_expr_t (unsigned int line, size_t children_count)
    : ast_expr_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
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

struct ast_add_assign_statement_t : public ast_binary_t
{
  ast_add_assign_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_assign_statement_t : public ast_binary_t
{
  ast_assign_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
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

  symbol_holder root_symbol;
};

struct ast_expression_statement_t : public ast_t
{
  ast_expression_statement_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_if_statement_t : public ast_t
{
  enum
    {
      CONDITION,
      TRUE_BRANCH,
      COUNT,
    };

  ast_if_statement_t (unsigned int line, ast_t* condition, ast_t* true_branch)
    : ast_t (line, COUNT)
  {
    children[CONDITION] = condition;
    children[TRUE_BRANCH] = true_branch;
  }

  ast_t* condition () const { return children[CONDITION]; }
  iterator condition_iter () { return begin () + CONDITION; }
  ast_t* true_branch () const { return children[TRUE_BRANCH]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
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
};

struct ast_println_statement_t : public ast_t
{
  ast_println_statement_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_return_statement_t : public ast_t
{
  ast_return_statement_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_increment_statement_t : public ast_unary_t
{
  ast_increment_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_decrement_statement_t : public ast_unary_t
{
  ast_decrement_statement_t (unsigned int line, ast_t* child)
    : ast_unary_t (line, child)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_list_statement_t : public ast_t
{
  ast_list_statement_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_subtract_assign_statement_t : public ast_binary_t
{
  ast_subtract_assign_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
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

  symbol_holder this_symbol;
};

struct ast_var_statement_t : public ast_t
{
  ast_var_statement_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  std::vector<symbol_holder> symbols;
};

struct ast_bind_statement_t : public ast_binary_t
{
  ast_bind_statement_t (unsigned int line, ast_t* left, ast_t* right)
    : ast_binary_t (line, left, right)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_bind_param_statement_t : public ast_t
{
  enum
    {
      LEFT,
      RIGHT,
      PARAM,
      COUNT,
    };

  ast_bind_param_statement_t (unsigned int line, ast_t* left, ast_t* right, ast_t* param)
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
};

struct ast_bind_statement_list_t : public ast_t
{
  ast_bind_statement_list_t (unsigned int line)
    : ast_t (line, 0)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_action_t : public ast_t
{
  enum
    {
      RECEIVER,
      PRECONDITION,
      BODY,
      COUNT
    };

  ast_action_t (unsigned int line, ast_t* receiver, ast_t* precondition, ast_t* body)
    : ast_t (line, COUNT)
  {
    children[RECEIVER] = receiver;
    children[PRECONDITION] = precondition;
    children[BODY] = body;
  }

  ast_t* receiver () const { return children[RECEIVER]; }
  ast_t* precondition () const { return children[PRECONDITION]; }
  iterator precondition_iter () { return children.begin () + PRECONDITION; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  symbol_holder this_symbol;
};

struct ast_dimensioned_action_t : public ast_t
{
  enum
    {
      DIMENSION,
      RECEIVER,
      PRECONDITION,
      BODY,
      COUNT
    };

  ast_dimensioned_action_t (unsigned int line, ast_t* dimension, ast_t* receiver, ast_t* precondition, ast_t* body)
    : ast_t (line, COUNT)
    , action (NULL)
  {
    children[DIMENSION] = dimension;
    children[RECEIVER] = receiver;
    children[PRECONDITION] = precondition;
    children[BODY] = body;
  }

  ast_t* dimension () const { return children[DIMENSION]; }
  iterator dimension_iter () { return children.begin () + DIMENSION; }
  ast_t* receiver () const { return children[RECEIVER]; }
  ast_t* precondition () const { return children[PRECONDITION]; }
  iterator precondition_iter () { return children.begin () + PRECONDITION; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  symbol_holder this_symbol;
  symbol_holder iota_symbol;
  action_t* action;
};

struct ast_bind_t : public ast_t
{
  enum
    {
      RECEIVER,
      BODY,
      COUNT,
    };

  ast_bind_t (unsigned int line, ast_t * receiver, ast_t * body)
    : ast_t (line, COUNT)
  {
    children[RECEIVER] = receiver;
    children[BODY] = body;
  }

  ast_t* receiver () const { return children[RECEIVER]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  symbol_holder this_symbol;
};

struct ast_function_t : public ast_t
{
  ast_function_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  symbol_holder function_symbol;
  symbol_holder return_symbol;
};

struct ast_instance_t : public ast_t
{
  ast_instance_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  symbol_holder symbol;
};

struct ast_method_t : public ast_t
{
  ast_method_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  symbol_holder return_symbol;
};

struct ast_reaction_t : public ast_t
{
  enum
    {
      RECEIVER,
      IDENTIFIER,
      SIGNATURE,
      BODY,
      COUNT,
    };

  ast_reaction_t (unsigned int line, ast_t* receiver, ast_t* identifier, ast_t* signature, ast_t* body)
    : ast_t (line, COUNT)
  {
    children[RECEIVER] = receiver;
    children[IDENTIFIER] = identifier;
    children[SIGNATURE] = signature;
    children[BODY] = body;
  }

  ast_t* receiver () const { return children[RECEIVER]; }
  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* signature () const { return children[SIGNATURE]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  symbol_holder this_symbol;
};

struct ast_dimensioned_reaction_t : public ast_t
{
  enum
    {
      DIMENSION,
      RECEIVER,
      IDENTIFIER,
      SIGNATURE,
      BODY,
      COUNT,
    };

  ast_dimensioned_reaction_t (unsigned int line, ast_t* dimension, ast_t* receiver, ast_t* identifier, ast_t* signature, ast_t* body)
    : ast_t (line, COUNT)
    , reaction (NULL)
  {
    children[DIMENSION] = dimension;
    children[RECEIVER] = receiver;
    children[IDENTIFIER] = identifier;
    children[SIGNATURE] = signature;
    children[BODY] = body;
  }

  ast_t* dimension () const { return children[DIMENSION]; }
  iterator dimension_iter () { return children.begin () + DIMENSION; }
  ast_t* receiver () const { return children[RECEIVER]; }
  ast_t* identifier () const { return children[IDENTIFIER]; }
  ast_t* signature () const { return children[SIGNATURE]; }
  ast_t* body () const { return children[BODY]; }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  symbol_holder this_symbol;
  symbol_holder iota_symbol;
  reaction_t* reaction;
};

class ast_receiver_definition_t : public ast_t
{
public:
  enum Kind
    {
      AstPointerReceiver,
      AstPointerToImmutableReceiver,
    };

  ast_receiver_definition_t (unsigned int line, size_t children_count, Kind kind)
    : ast_t (line, children_count)
    , kind_ (kind)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  Kind kind () const { return kind_; }

private:
  Kind kind_;
};

struct ast_type_definition_t : public ast_t
{
  ast_type_definition_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;

  symbol_holder symbol;
};

struct ast_top_level_list_t : public ast_t
{
  ast_top_level_list_t (unsigned int line, size_t children_count)
    : ast_t (line, children_count)
  { }

  void accept (ast_visitor_t& visitor);
  void accept (ast_const_visitor_t& visitor) const;
};

struct ast_visitor_t
{
  virtual void visit (ast_identifier_t& ast) { default_action (ast); }
  virtual void visit (ast_identifier_list_t& ast) { default_action (ast); }

  virtual void visit (ast_array_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_component_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_empty_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_field_list_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_foreign_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_heap_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_identifier_list_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_identifier_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_pointer_to_foreign_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_pointer_to_immutable_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_pointer_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_port_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_signature_type_spec_t& ast) { default_action (ast); }
  virtual void visit (ast_struct_type_spec_t& ast) { default_action (ast); }

  virtual void visit (ast_binary_arithmetic_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_address_of_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_call_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_dereference_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_equal_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_identifier_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_index_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_indexed_port_call_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_list_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_literal_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_logic_and_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_logic_not_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_logic_or_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_merge_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_move_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_new_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_not_equal_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_port_call_expr_t& ast) { default_action (ast); }
  virtual void visit (ast_select_expr_t& ast) { default_action (ast); }

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

  virtual void visit (ast_bind_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_bind_param_statement_t& ast) { default_action (ast); }
  virtual void visit (ast_bind_statement_list_t& ast) { default_action (ast); }

  virtual void visit (ast_action_t& ast) { default_action (ast); }
  virtual void visit (ast_dimensioned_action_t& ast) { default_action (ast); }
  virtual void visit (ast_bind_t& ast) { default_action (ast); }
  virtual void visit (ast_function_t& ast) { default_action (ast); }
  virtual void visit (ast_instance_t& ast) { default_action (ast); }
  virtual void visit (ast_method_t& ast) { default_action (ast); }
  virtual void visit (ast_reaction_t& ast) { default_action (ast); }
  virtual void visit (ast_dimensioned_reaction_t& ast) { default_action (ast); }
  virtual void visit (ast_receiver_definition_t& ast) { default_action (ast); }
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
  virtual void visit (const ast_field_list_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_foreign_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_heap_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_identifier_list_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_identifier_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_pointer_to_foreign_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_pointer_to_immutable_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_pointer_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_port_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_signature_type_spec_t& ast) { default_action (ast); }
  virtual void visit (const ast_struct_type_spec_t& ast) { default_action (ast); }

  virtual void visit (const ast_binary_arithmetic_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_address_of_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_call_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_dereference_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_equal_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_identifier_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_index_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_indexed_port_call_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_list_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_literal_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_logic_and_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_logic_not_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_logic_or_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_merge_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_move_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_new_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_not_equal_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_port_call_expr_t& ast) { default_action (ast); }
  virtual void visit (const ast_select_expr_t& ast) { default_action (ast); }

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

  virtual void visit (const ast_bind_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_bind_param_statement_t& ast) { default_action (ast); }
  virtual void visit (const ast_bind_statement_list_t& ast) { default_action (ast); }
  virtual void visit (const ast_var_statement_t& ast) { default_action (ast); }

  virtual void visit (const ast_action_t& ast) { default_action (ast); }
  virtual void visit (const ast_dimensioned_action_t& ast) { default_action (ast); }
  virtual void visit (const ast_bind_t& ast) { default_action (ast); }
  virtual void visit (const ast_function_t& ast) { default_action (ast); }
  virtual void visit (const ast_instance_t& ast) { default_action (ast); }
  virtual void visit (const ast_method_t& ast) { default_action (ast); }
  virtual void visit (const ast_reaction_t& ast) { default_action (ast); }
  virtual void visit (const ast_dimensioned_reaction_t& ast) { default_action (ast); }
  virtual void visit (const ast_receiver_definition_t& ast) { default_action (ast); }
  virtual void visit (const ast_type_definition_t& ast) { default_action (ast); }

  virtual void visit (const ast_top_level_list_t& ast) { default_action (ast); }

  virtual void default_action (const ast_t& ast) { }
};

/* Generic functions. */

void ast_print (const ast_t& ast);

void ast_set_symtab (ast_t * ast, symtab_t * symtab);

string_t ast_get_identifier (const ast_t* ast);

/* Decl */

ast_t *ast_make_identifier (unsigned int line, string_t identifier);

ast_t *ast_make_identifier_list (unsigned int line);

#define IDENTIFIER_LIST 0
#define TYPE_SPEC 1

ast_t *ast_make_identifier_list_type_spec (unsigned int line,
                                           ast_t * identifier_list,
					   ast_t * type_spec);

#define RECEIVER_THIS_IDENTIFIER 0
#define RECEIVER_TYPE_IDENTIFIER 1

ast_t *ast_make_receiver (unsigned int line, ast_t * this_identifier,
                          ast_t * type_identifier,
                          ast_receiver_definition_t::Kind kind);

/* Def */

#define INSTANCE_IDENTIFIER 0
#define INSTANCE_TYPE_IDENTIFIER 1
#define INSTANCE_INITIALIZER 2

ast_t *ast_make_instance_def (unsigned int line, ast_t * instance_id, ast_t * type_id, ast_t * initializer);

void ast_instance_set_type (ast_t * ast, type_t * type);

type_t *ast_instance_get_type (ast_t * ast);

ast_t *ast_make_top_level_list (void);

#define TYPE_IDENTIFIER 0
#define TYPE_TYPE_SPEC 1

ast_t *ast_make_type_def (unsigned int line, ast_t * identifier, ast_t * type_spec);

#define METHOD_RECEIVER 0
#define METHOD_IDENTIFIER 1
#define METHOD_SIGNATURE 2
#define METHOD_RETURN_TYPE 3
#define METHOD_BODY 4

ast_t *ast_make_method_def (unsigned int line, ast_t * receiver, ast_t * identifier,
                            ast_t * signature, ast_t * return_type,
                            ast_t* body);

#define FUNCTION_IDENTIFIER 0
#define FUNCTION_SIGNATURE 1
#define FUNCTION_RETURN_TYPE 2
#define FUNCTION_BODY 3

ast_t *ast_make_function_def (unsigned int line, ast_t * identifier, ast_t * signature, ast_t* return_type, ast_t* body);

/* Expr */

#define UNARY_CHILD 0

ast_t *ast_make_logic_and (unsigned int line, ast_t * left, ast_t * right);

ast_t *ast_make_logic_or (unsigned int line, ast_t * left, ast_t * right);

/* Stmt */

ast_t *ast_make_expr_stmt (unsigned int line, ast_t * expr);

#define VAR_IDENTIFIER_LIST 0
#define VAR_TYPE_SPEC 1

ast_t *ast_make_var_stmt (unsigned int line, ast_t * identifier_list, ast_t * type_spec);

ast_t *ast_make_stmt_list (unsigned int line);

ast_t *ast_make_return_stmt (unsigned int line, ast_t * expr);

ast_t *ast_make_bind_list_stmt (unsigned int line);

ast_t *ast_make_println_stmt (unsigned int line, ast_t * expr);

/* TypeSpec */

ast_t *ast_make_field_list (unsigned int line);

#define IDENTIFIER_TYPE_SPEC_CHILD 0

ast_t *ast_make_identifier_type_spec (unsigned int line, ast_t * identifier);

#define PORT_SIGNATURE 0

ast_t *ast_make_port (unsigned int line, ast_t * signature);

#define COMPONENT_FIELD_LIST 0

ast_t *ast_make_component_type_spec (unsigned int line, ast_t * field_list);

#define STRUCT_FIELD_LIST 0

ast_t *ast_make_struct_type_spec (unsigned int line, ast_t * field_list);

ast_t *ast_make_empty_type_spec (unsigned int line);

#define POINTER_BASE_TYPE 0

ast_t *ast_make_pointer_type_spec (unsigned int line, ast_t* type_spec);

ast_t *ast_make_pointer_to_immutable_type_spec (unsigned int line, ast_t* type_spec);

ast_t *ast_make_pointer_to_foreign_type_spec (unsigned int line, ast_t* type_spec);

#define FOREIGN_BASE_TYPE 0

ast_t *ast_make_foreign_type_spec (unsigned int line, ast_t* type_spec);

#define HEAP_BASE_TYPE 0

ast_t *ast_make_heap_type_spec (unsigned int line, ast_t * type);

#define ARRAY_DIMENSION 0
#define ARRAY_BASE_TYPE 1

ast_t *ast_make_array_type_spec (unsigned int line, ast_t * dimension, ast_t * type_spec);

ast_t *ast_make_expression_list (unsigned int line);

ast_t *ast_make_typed_literal (unsigned int line, typed_value_t value);

typed_value_t ast_get_typed_value (const ast_t* node);

ast_t *ast_make_signature (unsigned int line);

action_t *ast_get_current_action (const ast_t * node);

type_t *ast_get_current_receiver_type (const ast_t * node);

ast_t *ast_make_new_expr (unsigned int line, ast_t* identifier);

ast_t *ast_make_merge_expr (unsigned int line, ast_t* expr);

#define AST_FOREACH(child, parent) size_t idx; size_t limit; ast_t* child; \
  for (idx = 0, limit = (parent)->size (), child = ((idx < (parent)->size ()) ? (parent)->at (idx) : NULL); idx != limit; ++idx, child = (idx < ((parent)->size ()) ? (parent)->at (idx) : NULL))

named_type_t *
get_current_receiver_type (const ast_t * node);

trigger_t *
get_current_trigger (const ast_t * node);

action_reaction_base_t *
get_current_action (const ast_t * node);

method_t *
get_current_method (const ast_t * node);

function_t *
get_current_function (const ast_t * node);

const type_t *
get_current_return_type (const ast_t * node);

#endif /* ast_h */
