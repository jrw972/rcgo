#ifndef Ast_hpp
#define Ast_hpp

#include "types.hpp"
#include <vector>
#include "Location.hpp"
#include "typed_value.hpp"

namespace Ast
{

  struct Node
  {
    typedef std::vector<Node*> ChildrenType;
    typedef ChildrenType::iterator Iterator;
    typedef ChildrenType::const_iterator ConstIterator;
    typedef std::vector<Symbol*> SymbolsType;
    typedef SymbolsType::const_iterator ConstSymbolIterator;

    enum Context
    {
      Parent, // Use the context of the parent.
      Action,
      Reaction,
      Initializer,
      Getter,
    };

    virtual ~Node() { }

    virtual void Accept (Visitor& visitor) = 0;
    virtual void Accept (ConstVisitor& visitor) const = 0;

    Node* GetParent () const
    {
      return m_parent;
    }
    size_t Size () const
    {
      return m_children.size ();
    }
    Node* Prepend (Node * child);
    Node* Append (Node * child);
    Node* const & At (size_t idx) const
    {
      return m_children.at (idx);
    }
    Node* & At (size_t idx)
    {
      return m_children. at (idx);
    }
    Iterator Begin ()
    {
      return m_children.begin ();
    }
    Iterator End ()
    {
      return m_children.end ();
    }
    ConstIterator Begin () const
    {
      return m_children.begin ();
    }
    ConstIterator End () const
    {
      return m_children.end ();
    }
    void VisitChildren (Visitor& visitor);
    void VisitChildren (ConstVisitor& visitor) const;

    ConstSymbolIterator SymbolsBegin () const
    {
      return m_symbols.begin ();
    }
    ConstSymbolIterator SymbolsEnd () const
    {
      return m_symbols.end ();
    }
    void EnterSymbol (Symbol* s)
    {
      m_symbols.push_back (s);
    }
    Symbol* FindGlobalSymbol (const std::string& identifier) const;
    Symbol* FindLocalSymbol (const std::string& identifier) const;
    virtual const Type::Type* GetReceiverType () const;
    void Activate ();
    void Change ();

    Context GetContext () const;
    bool GetInMutableSection () const;

    bool inMutableSection;
    Location const location;
    typed_value_t typed_value;

  private:
    Node* m_parent;
    ChildrenType m_children;
    SymbolsType m_symbols;
  protected:
    Context o_context;
    Node (unsigned int line_, size_t children_count);
    void set (size_t idx, Node* child);
  };

  std::ostream&
  operator<< (std::ostream& out, const Node& node);

  class ast_identifier_t : public Node
  {
  public:
    ast_identifier_t (unsigned int line, const std::string& id)
      : Node (line, 0)
      , identifier (id)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    std::string const identifier;
  };

  struct ast_identifier_list_t : public Node
  {
    ast_identifier_list_t (unsigned int line)
      : Node (line, 0)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_receiver_t : public Node
  {
    enum
    {
      THIS_IDENTIFIER,
      TYPE_IDENTIFIER,
      COUNT,
    };

    ast_receiver_t (unsigned int line, Node* this_identifier, Mutability m, Mutability dm, bool isP, Node* type_identifier)
      : Node (line, COUNT)
      , mutability (m)
      , dereferenceMutability (dm)
      , isPointer (isP)
    {
      set (THIS_IDENTIFIER, this_identifier);
      set (TYPE_IDENTIFIER, type_identifier);
    }

    Node* this_identifier () const
    {
      return At (THIS_IDENTIFIER);
    }
    Node* type_identifier () const
    {
      return At (TYPE_IDENTIFIER);
    }

    virtual void Accept (Visitor& visitor);
    virtual void Accept (ConstVisitor& visitor) const;

    Mutability const mutability;
    Mutability const dereferenceMutability;
    bool const isPointer;
    Symbol* this_symbol;
  };

  struct ast_array_type_spec_t : public Node
  {
    enum
    {
      DIMENSION,
      BASE_TYPE,
      COUNT,
    };

    ast_array_type_spec_t (unsigned int line, Node* dimension, Node* base_type)
      : Node (line, COUNT)
    {
      set (DIMENSION, dimension);
      set (BASE_TYPE, base_type);
    }

    Node* dimension () const
    {
      return At (DIMENSION);
    }
    Node*& dimension_ref ()
    {
      return At (DIMENSION);
    }
    Node* base_type () const
    {
      return At (BASE_TYPE);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_unary_t : public Node
  {
    enum
    {
      CHILD,
      COUNT
    };

    ast_unary_t (unsigned int line, Node* child)
      : Node (line, COUNT)
    {
      set (CHILD, child);
    }

    Node* child () const
    {
      return At (CHILD);
    }
    Node*& child_ref ()
    {
      return At (CHILD);
    }
  };

  struct ast_empty_type_spec_t : public Node
  {
    ast_empty_type_spec_t (unsigned int line)
      : Node (line, 0)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_enum_type_spec_t : public Node
  {
    enum
    {
      VALUES,
      COUNT,
    };

    ast_enum_type_spec_t (unsigned int line, Node* values)
      : Node (line, COUNT)
    {
      set (VALUES, values);
    }

    Node* values () const
    {
      return At (VALUES);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_field_list_type_spec_t : public Node
  {
    ast_field_list_type_spec_t (unsigned int line)
      : Node (line, 0)
      , IsComponent (false)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    bool IsComponent;
  };

  struct ast_heap_type_spec_t : public ast_unary_t
  {
    ast_heap_type_spec_t (unsigned int line, Node* child)
      : ast_unary_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_identifier_type_spec_t : public ast_unary_t
  {
    ast_identifier_type_spec_t (unsigned int line, Node* child)
      : ast_unary_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_identifier_list_type_spec_t : public Node
  {
    enum
    {
      IDENTIFIER_LIST,
      TYPE_SPEC,
      COUNT,
    };

    ast_identifier_list_type_spec_t (unsigned int line, Node* identifier_list,
                                     Mutability m, Mutability dm,
                                     Node* type_spec)
      : Node (line, COUNT)
      , mutability (m)
      , dereferenceMutability (dm)
    {
      set (IDENTIFIER_LIST, identifier_list);
      set (TYPE_SPEC, type_spec);
    }

    const Mutability mutability;
    const Mutability dereferenceMutability;

    Node* identifier_list () const
    {
      return At (IDENTIFIER_LIST);
    }
    Node* type_spec () const
    {
      return At (TYPE_SPEC);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_pointer_type_spec_t : public ast_unary_t
  {
    ast_pointer_type_spec_t (unsigned int line, Node* child)
      : ast_unary_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_slice_type_spec_t : public ast_unary_t
  {
    ast_slice_type_spec_t (unsigned int line, Node* child)
      : ast_unary_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_push_port_type_spec_t : public Node
  {
    enum
    {
      SIGNATURE,
      COUNT,
    };

    ast_push_port_type_spec_t (unsigned int line, Node* signature)
      : Node (line, COUNT)
    {
      set (SIGNATURE, signature);
    }

    Node* signature () const
    {
      return At (SIGNATURE);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_pull_port_type_spec_t : public Node
  {
    enum
    {
      SIGNATURE,
      RETURN_TYPE,
      COUNT,
    };

    ast_pull_port_type_spec_t (unsigned int line, Node* signature, Mutability dm, Node* return_type)
      : Node (line, COUNT)
      , dereferenceMutability (dm)
    {
      set (SIGNATURE, signature);
      set (RETURN_TYPE, return_type);
    }

    Node* signature () const
    {
      return At (SIGNATURE);
    }
    Node* return_type () const
    {
      return At (RETURN_TYPE);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    Mutability const dereferenceMutability;
  };

  struct ast_signature_type_spec_t : public Node
  {
    ast_signature_type_spec_t (unsigned int line)
      : Node (line, 0)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  class ast_expr_t : public Node
  {
  public:
    ast_expr_t (unsigned int line, size_t children_count)
      : Node (line, children_count)
    { }
  };

  struct TypeExpression : public ast_expr_t
  {
    enum
    {
      TYPE_SPEC,
      COUNT
    };

    TypeExpression (unsigned int line, Node* type_spec)
      : ast_expr_t (line, COUNT)
    {
      set (TYPE_SPEC, type_spec);
    }

    Node* type_spec () const
    {
      return At (TYPE_SPEC);
    }

    virtual void Accept (Visitor& visitor);
    virtual void Accept (ConstVisitor& visitor) const;
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

    ast_cast_expr_t (unsigned int line, Node* type_spec, Node* child)
      : ast_expr_t (line, COUNT)
    {
      set (TYPE_SPEC, type_spec);
      set (CHILD, child);
    }

    Node* type_spec () const
    {
      return At (TYPE_SPEC);
    }
    Node* child () const
    {
      return At (CHILD);
    }
    Node*& child_ref ()
    {
      return At (CHILD);
    }

    virtual void Accept (Visitor& visitor);
    virtual void Accept (ConstVisitor& visitor) const;
  };

  struct ast_unary_expr_t : public ast_expr_t
  {
    enum
    {
      CHILD,
      COUNT
    };

    ast_unary_expr_t (unsigned int line, Node* child)
      : ast_expr_t (line, COUNT)
    {
      set (CHILD, child);
    }

    Node* child () const
    {
      return At (CHILD);
    }
    Node*& child_ref ()
    {
      return At (CHILD);
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

    ast_binary_expr_t (unsigned int line, Node* left, Node* right)
      : ast_expr_t (line, COUNT)
    {
      set (LEFT, left);
      set (RIGHT, right);
    }

    Node*& left_ref ()
    {
      return At (LEFT);
    }
    Node* left () const
    {
      return At (LEFT);
    }
    Node*& right_ref ()
    {
      return At (RIGHT);
    }
    Node* right () const
    {
      return At (RIGHT);
    }
  };

  struct ast_binary_arithmetic_expr_t : public ast_binary_expr_t
  {
    ast_binary_arithmetic_expr_t (unsigned int line, BinaryArithmetic a, Node* left, Node* right)
      : ast_binary_expr_t (line, left, right)
      , arithmetic (a)
    { }

    const BinaryArithmetic arithmetic;

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_implicit_conversion_expr_t : public ast_unary_expr_t
  {
    ast_implicit_conversion_expr_t (unsigned int line, Node* child)
      : ast_unary_expr_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_address_of_expr_t : public ast_unary_expr_t
  {
    ast_address_of_expr_t (unsigned int line, Node* child)
      : ast_unary_expr_t (line, child)
      , address_of_dereference (false)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    bool address_of_dereference;
  };

  struct ast_call_expr_t : public ast_expr_t
  {
    enum
    {
      EXPR,
      ARGS,
      COUNT,
    };

    ast_call_expr_t (unsigned int line, Node* expr, Node* args)
      : ast_expr_t (line, COUNT)
    {
      set (EXPR, expr);
      set (ARGS, args);
    }

    Node* expr () const
    {
      return At (EXPR);
    }
    Node*& expr_ref ()
    {
      return At (EXPR);
    }
    Node* args () const
    {
      return At (ARGS);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    typed_value_t original_expr_tv;
    bool IsCall;
  };

  struct ast_dereference_expr_t : public ast_unary_expr_t
  {
    ast_dereference_expr_t (unsigned int line, Node* child)
      : ast_unary_expr_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_implicit_dereference_expr_t : public ast_unary_expr_t
  {
    ast_implicit_dereference_expr_t (unsigned int line, Node* child)
      : ast_unary_expr_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_list_expr_t : public ast_expr_t
  {
    ast_list_expr_t (unsigned int line)
      : ast_expr_t (line, 0)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_identifier_expr_t : public ast_unary_expr_t
  {
    ast_identifier_expr_t (unsigned int line, Node* child)
      : ast_unary_expr_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

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

    ast_index_expr_t (unsigned int line, Node* base, Node* index)
      : ast_expr_t (line, COUNT)
    {
      set (BASE, base);
      set (INDEX, index);
    }

    Node* base () const
    {
      return At (BASE);
    }
    Node*& base_ref ()
    {
      return At (BASE);
    }
    Node* index () const
    {
      return At (INDEX);
    }
    Node*& index_ref ()
    {
      return At (INDEX);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
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

    ast_slice_expr_t (unsigned int line, Node* base, Node* low, Node* high)
      : ast_expr_t (line, COUNT)
    {
      set (BASE, base);
      set (LOW, low);
      set (HIGH, high);
    }

    Node* base () const
    {
      return At (BASE);
    }
    Node*& base_ref ()
    {
      return At (BASE);
    }
    Node* low () const
    {
      return At (LOW);
    }
    Node*& low_ref ()
    {
      return At (LOW);
    }
    Node* high () const
    {
      return At (HIGH);
    }
    Node*& high_ref ()
    {
      return At (HIGH);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_unary_arithmetic_expr_t : public ast_unary_expr_t
  {
    ast_unary_arithmetic_expr_t (unsigned int line, UnaryArithmetic a, Node* child)
      : ast_unary_expr_t (line, child)
      , arithmetic (a)
    { }

    const UnaryArithmetic arithmetic;

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_push_port_call_expr_t : public ast_expr_t
  {
    enum
    {
      IDENTIFIER,
      ARGS,
      COUNT,
    };

    ast_push_port_call_expr_t (unsigned int line, Node* identifier, Node* args)
      : ast_expr_t (line, COUNT)
    {
      set (IDENTIFIER, identifier);
      set (ARGS, args);
    }

    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* args () const
    {
      return At (ARGS);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

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
                                  Node * identifier,
                                  Node* index,
                                  Node * args)
      : ast_expr_t (line, COUNT)
    {
      set (IDENTIFIER, identifier);
      set (INDEX, index);
      set (ARGS, args);
    }

    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* index () const
    {
      return At (INDEX);
    }
    Node*& index_ref ()
    {
      return At (INDEX);
    }
    Node* args () const
    {
      return At (ARGS);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

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

    ast_select_expr_t (unsigned int line, Node* base, Node* identifier)
      : ast_expr_t (line, COUNT)
    {
      set (BASE, base);
      set (IDENTIFIER, identifier);
    }

    Node* base () const
    {
      return At (BASE);
    }
    Node*& base_ref ()
    {
      return At (BASE);
    }
    Node* identifier () const
    {
      return At (IDENTIFIER);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_literal_expr_t : public ast_expr_t
  {
    ast_literal_expr_t (unsigned int line, typed_value_t tv)
      : ast_expr_t (line, 0)
    {
      typed_value = tv;
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_binary_t : public Node
  {
    enum
    {
      LEFT,
      RIGHT,
      COUNT
    };

    ast_binary_t (unsigned int line, Node* left, Node* right)
      : Node (line, COUNT)
    {
      set (LEFT, left);
      set (RIGHT, right);
    }

    Node* left () const
    {
      return At (LEFT);
    }
    Node*& left_ref ()
    {
      return At (LEFT);
    }
    Node* right () const
    {
      return At (RIGHT);
    }
    Node*& right_ref ()
    {
      return At (RIGHT);
    }
  };

  struct ast_empty_statement_t : public Node
  {
    ast_empty_statement_t (unsigned int line)
      : Node (line, 0)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_add_assign_statement_t : public ast_binary_t
  {
    ast_add_assign_statement_t (unsigned int line, Node* left, Node* right)
      : ast_binary_t (line, left, right)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_assign_statement_t : public ast_binary_t
  {
    ast_assign_statement_t (unsigned int line, Node* left, Node* right)
      : ast_binary_t (line, left, right)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_change_statement_t : public Node
  {
    enum
    {
      EXPR,
      IDENTIFIER,
      BODY,
      COUNT
    };

    ast_change_statement_t (unsigned int line,
                            Node * expr,
                            Node * identifier,
                            Node * body)
      : Node (line, COUNT)
    {
      set (EXPR, expr);
      set (IDENTIFIER, identifier);
      set (BODY, body);
    }

    Node* expr () const
    {
      return At (EXPR);
    }
    Node*& expr_ref ()
    {
      return At (EXPR);
    }
    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    Symbol* root_symbol;
  };

  struct ast_expression_statement_t : public ast_unary_t
  {
    ast_expression_statement_t (unsigned int line, Node* child)
      : ast_unary_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_if_statement_t : public Node
  {
    enum
    {
      CONDITION,
      TRUE_BRANCH,
      FALSE_BRANCH,
      COUNT,
    };

    ast_if_statement_t (unsigned int line, Node* condition, Node* true_branch, Node* false_branch)
      : Node (line, COUNT)
    {
      set (CONDITION, condition);
      set (TRUE_BRANCH, true_branch);
      set (FALSE_BRANCH, false_branch);
    }

    Node* condition () const
    {
      return At (CONDITION);
    }
    Node*& condition_ref ()
    {
      return At (CONDITION);
    }
    Node* true_branch () const
    {
      return At (TRUE_BRANCH);
    }
    Node* false_branch () const
    {
      return At (FALSE_BRANCH);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_while_statement_t : public Node
  {
    enum
    {
      CONDITION,
      BODY,
      COUNT
    };

    ast_while_statement_t (unsigned int line, Node* condition, Node* body)
      : Node (line, COUNT)
    {
      set (CONDITION, condition);
      set (BODY, body);
    }

    Node* condition () const
    {
      return At (CONDITION);
    }
    Node*& condition_ref ()
    {
      return At (CONDITION);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_return_statement_t : public ast_unary_t
  {
    ast_return_statement_t (unsigned int line, Node* child)
      : ast_unary_t (line, child)
      , return_symbol (NULL)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    const ParameterSymbol* return_symbol;
  };

  struct ast_increment_statement_t : public ast_unary_t
  {
    ast_increment_statement_t (unsigned int line, Node* child)
      : ast_unary_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_decrement_statement_t : public ast_unary_t
  {
    ast_decrement_statement_t (unsigned int line, Node* child)
      : ast_unary_t (line, child)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_list_statement_t : public Node
  {
    ast_list_statement_t (unsigned int line)
      : Node (line, 0)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_subtract_assign_statement_t : public ast_binary_t
  {
    ast_subtract_assign_statement_t (unsigned int line, Node* left, Node* right)
      : ast_binary_t (line, left, right)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_activate_statement_t : public Node
  {
    enum
    {
      EXPR_LIST,
      BODY,
      COUNT
    };

    ast_activate_statement_t (unsigned int line, Node * expr_list, Node * body)
      : Node (line, COUNT)
      , memoryModel (NULL)
    {
      set (EXPR_LIST, expr_list);
      set (BODY, body);
      body->inMutableSection = true;
    }

    Node* expr_list () const
    {
      return At (EXPR_LIST);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    const MemoryModel* memoryModel;
    ReceiverAccess mutable_phase_access;
  };

  struct ast_var_statement_t : public Node
  {
    enum
    {
      IDENTIFIER_LIST,
      TYPE_SPEC,
      EXPRESSION_LIST,
      COUNT
    };

    ast_var_statement_t (unsigned int line, Node* identifier_list, Mutability m, Mutability dm, Node* type_spec, Node* expression_list)
      : Node (line, COUNT)
      , mutability (m)
      , dereferenceMutability (dm)
    {
      set (IDENTIFIER_LIST, identifier_list);
      set (TYPE_SPEC, type_spec);
      set (EXPRESSION_LIST, expression_list);
    }

    Node* identifier_list () const
    {
      return At (IDENTIFIER_LIST);
    }
    Node* type_spec () const
    {
      return At (TYPE_SPEC);
    }
    Node* expression_list () const
    {
      return At (EXPRESSION_LIST);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    Mutability const mutability;
    Mutability const dereferenceMutability;
    std::vector<Symbol*> symbols;
  };

  struct ast_bind_push_port_statement_t : public ast_binary_t
  {
    ast_bind_push_port_statement_t (unsigned int line, Node* left, Node* right)
      : ast_binary_t (line, left, right)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_bind_push_port_param_statement_t : public Node
  {
    enum
    {
      LEFT,
      RIGHT,
      PARAM,
      COUNT,
    };

    ast_bind_push_port_param_statement_t (unsigned int line, Node* left, Node* right, Node* param)
      : Node (line, COUNT)
    {
      set (LEFT, left);
      set (RIGHT, right);
      set (PARAM, param);
    }

    Node* left () const
    {
      return At (LEFT);
    }
    Node* right () const
    {
      return At (RIGHT);
    }
    Node*& right_ref ()
    {
      return At (RIGHT);
    }
    Node* param () const
    {
      return At (PARAM);
    }
    Node*& param_ref ()
    {
      return At (PARAM);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_bind_pull_port_statement_t : public ast_binary_t
  {
    ast_bind_pull_port_statement_t (unsigned int line, Node* left, Node* right)
      : ast_binary_t (line, left, right)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  struct ast_for_iota_statement_t : public Node
  {
    enum
    {
      IDENTIFIER,
      LIMIT,
      BODY,
      COUNT
    };

    ast_for_iota_statement_t (unsigned int line, Node* identifier, Node* limit, Node* body)
      : Node (line, COUNT)
    {
      set (IDENTIFIER, identifier);
      set (LIMIT, limit);
      set (BODY, body);
    }

    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* limit_node () const
    {
      return At (LIMIT);
    }
    Node*& limit_node_ref ()
    {
      return At (LIMIT);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    Symbol* symbol;
    typed_value_t limit;
  };

  struct ast_action_t : public Node
  {
    enum
    {
      RECEIVER,
      IDENTIFIER,
      PRECONDITION,
      BODY,
      COUNT
    };

    ast_action_t (unsigned int line, Node* receiver,
                  Node* identifier,
                  Node* precondition, Node* body)
      : Node (line, COUNT)
      , action (NULL)
    {
      o_context = Action;
      set (RECEIVER, receiver);
      set (IDENTIFIER, identifier);
      set (PRECONDITION, precondition);
      set (BODY, body);
    }

    Node* receiver () const
    {
      return At (RECEIVER);
    }
    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* precondition () const
    {
      return At (PRECONDITION);
    }
    Node*& precondition_ref ()
    {
      return At (PRECONDITION);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    action_t* action;

    virtual const Type::Type*
    GetReceiverType () const;
  };

  struct ast_dimensioned_action_t : public Node
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

    ast_dimensioned_action_t (unsigned int line,
                              Node* dimension,
                              Node* receiver,
                              Node* identifier,
                              Node* precondition,
                              Node* body)
      : Node (line, COUNT)
      , action (NULL)
    {
      o_context = Action;
      set (DIMENSION, dimension);
      set (RECEIVER, receiver);
      set (IDENTIFIER, identifier);
      set (PRECONDITION, precondition);
      set (BODY, body);
    }

    Node* dimension () const
    {
      return At (DIMENSION);
    }
    Node*& dimension_ref ()
    {
      return At (DIMENSION);
    }
    Node* receiver () const
    {
      return At (RECEIVER);
    }
    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* precondition () const
    {
      return At (PRECONDITION);
    }
    Node*& precondition_ref ()
    {
      return At (PRECONDITION);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    action_t* action;

    virtual const Type::Type*
    GetReceiverType () const;
  };

  struct ast_bind_t : public Node
  {
    enum
    {
      RECEIVER,
      IDENTIFIER,
      BODY,
      COUNT,
    };

    ast_bind_t (unsigned int line, Node * receiver, Node* identifier, Node * body)
      : Node (line, COUNT)
      , bind (NULL)
    {
      set (RECEIVER, receiver);
      set (IDENTIFIER, identifier);
      set (BODY, body);
    }

    Node* receiver () const
    {
      return At (RECEIVER);
    }
    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    bind_t* bind;
  };

  struct ast_function_t : public Node
  {
    enum
    {
      IDENTIFIER,
      SIGNATURE,
      RETURN_TYPE,
      BODY,
      COUNT
    };

    ast_function_t (unsigned int line, Node* identifier, Node* signature,
                    Mutability dm, Node* return_type, Node* body)
      : Node (line, COUNT)
      , function (NULL)
      , dereferenceMutability (dm)
    {
      set (IDENTIFIER, identifier);
      set (SIGNATURE, signature);
      set (RETURN_TYPE, return_type);
      set (BODY, body);
    }

    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* signature () const
    {
      return At (SIGNATURE);
    }
    Node* return_type () const
    {
      return At (RETURN_TYPE);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    ::Function* function;
    Mutability const dereferenceMutability;
  };

  struct ast_instance_t : public Node
  {
    enum
    {
      IDENTIFIER,
      TYPE_IDENTIFIER,
      INITIALIZER,
      EXPRESSION_LIST,
      COUNT
    };

    ast_instance_t (unsigned int line, Node* identifier, Node* type_identifier, Node* initializer, Node* expression_list)
      : Node (line, COUNT)
    {
      set (IDENTIFIER, identifier);
      set (TYPE_IDENTIFIER, type_identifier);
      set (INITIALIZER, initializer);
      set (EXPRESSION_LIST, expression_list);
    }

    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* type_identifier () const
    {
      return At (TYPE_IDENTIFIER);
    }
    Node* initializer () const
    {
      return At (INITIALIZER);
    }
    Node* expression_list () const
    {
      return At (EXPRESSION_LIST);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    InstanceSymbol* symbol;
  };

  struct ast_const_t : public Node
  {
    enum
    {
      IDENTIFIER_LIST,
      TYPE_SPEC,
      EXPRESSION_LIST,
      COUNT
    };

    ast_const_t (unsigned int line, Node* identifier_list, Node* type_spec, Node* expression_list)
      : Node (line, COUNT)
    {
      set (IDENTIFIER_LIST, identifier_list);
      set (TYPE_SPEC, type_spec);
      set (EXPRESSION_LIST, expression_list);
    }

    Node* identifier_list () const
    {
      return At (IDENTIFIER_LIST);
    }
    Node* type_spec () const
    {
      return At (TYPE_SPEC);
    }
    Node* expression_list () const
    {
      return At (EXPRESSION_LIST);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    TypedConstantSymbol* symbol;
  };

  struct ast_method_t : public Node
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
                  Node * receiver,
                  Node * identifier,
                  Node * signature,
                  Mutability return_dm,
                  Node * return_type,
                  Node* body)
      : Node (line, COUNT)
      , method (NULL)
      , return_dereference_mutability (return_dm)
    {
      set (RECEIVER, receiver);
      set (IDENTIFIER, identifier);
      set (SIGNATURE, signature);
      set (RETURN_TYPE, return_type);
      set (BODY, body);
    }

    Node* receiver () const
    {
      return At (RECEIVER);
    }
    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* signature () const
    {
      return At (SIGNATURE);
    }
    Node* return_type () const
    {
      return At (RETURN_TYPE);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    ::Method* method;
    Mutability const return_dereference_mutability;
  };

  struct ast_getter_t : public Node
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
                  Node * receiver,
                  Node * identifier,
                  Node * signature,
                  Mutability dm,
                  Node * return_type,
                  Node* body)
      : Node (line, COUNT)
      , getter (NULL)
      , dereferenceMutability (dm)
    {
      o_context = Getter;
      set (RECEIVER, receiver);
      set (IDENTIFIER, identifier);
      set (SIGNATURE, signature);
      set (RETURN_TYPE, return_type);
      set (BODY, body);
    }

    Node* receiver () const
    {
      return At (RECEIVER);
    }
    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* signature () const
    {
      return At (SIGNATURE);
    }
    Node* return_type () const
    {
      return At (RETURN_TYPE);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    ::Getter* getter;
    Mutability const dereferenceMutability;
  };

  struct ast_initializer_t : public Node
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
                       Node* receiver,
                       Node * identifier,
                       Node * signature,
                       Mutability return_dm,
                       Node* return_type,
                       Node* body)
      : Node (line, COUNT)
      , return_dereference_mutability (return_dm)
      , initializer (NULL)
    {
      o_context = Initializer;
      set (RECEIVER, receiver);
      set (IDENTIFIER, identifier);
      set (SIGNATURE, signature);
      set (RETURN_TYPE, return_type);
      set (BODY, body);
    }

    Node* receiver () const
    {
      return At (RECEIVER);
    }
    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* signature () const
    {
      return At (SIGNATURE);
    }
    Node* return_type () const
    {
      return At (RETURN_TYPE);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    Mutability const return_dereference_mutability;
    ::Initializer* initializer;

    virtual const Type::Type*
    GetReceiverType () const
    {
      unimplemented;
    }
  };

  struct ast_reaction_t : public Node
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

    ast_reaction_t (unsigned int line, Node* receiver, Node* identifier, Node* signature, Node* body)
      : Node (line, COUNT)
      , reaction (NULL)
    {
      o_context = Reaction;
      set (RECEIVER, receiver);
      set (IDENTIFIER, identifier);
      set (SIGNATURE, signature);
      set (RETURN_TYPE, new ast_empty_type_spec_t (line));
      set (BODY, body);
    }

    Node* receiver () const
    {
      return At (RECEIVER);
    }
    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* signature () const
    {
      return At (SIGNATURE);
    }
    Node* return_type () const
    {
      return At (RETURN_TYPE);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    reaction_t* reaction;

    virtual const Type::Type*
    GetReceiverType () const;
  };

  struct ast_dimensioned_reaction_t : public Node
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
                                Node* dimension,
                                Node* receiver,
                                Node* identifier,
                                Node* signature,
                                Node* body)
      : Node (line, COUNT)
      , reaction (NULL)
    {
      o_context = Reaction;
      set (DIMENSION, dimension);
      set (RECEIVER, receiver);
      set (IDENTIFIER, identifier);
      set (SIGNATURE, signature);
      set (RETURN_TYPE, new ast_empty_type_spec_t (line));
      set (BODY, body);
    }

    Node* dimension () const
    {
      return At (DIMENSION);
    }
    Node*& dimension_ref ()
    {
      return At (DIMENSION);
    }
    Node* receiver () const
    {
      return At (RECEIVER);
    }
    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* signature () const
    {
      return At (SIGNATURE);
    }
    Node* return_type () const
    {
      return At (RETURN_TYPE);
    }
    Node* body () const
    {
      return At (BODY);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    reaction_t* reaction;

    virtual const Type::Type*
    GetReceiverType () const;
  };

  struct Type : public Node
  {
    enum
    {
      IDENTIFIER,
      TYPE_SPEC,
      COUNT
    };

    Type (unsigned int line, Node* identifier, Node* type_spec)
      : Node (line, COUNT)
    {
      set (IDENTIFIER, identifier);
      set (TYPE_SPEC, type_spec);
    }

    Node* identifier () const
    {
      return At (IDENTIFIER);
    }
    Node* type_spec () const
    {
      return At (TYPE_SPEC);
    }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;

    TypeSymbol* symbol;
  };

  struct SourceFile : public Node
  {
    SourceFile ()
      : Node (-1, 0)
    { }

    void Accept (Visitor& visitor);
    void Accept (ConstVisitor& visitor) const;
  };

  /* Generic functions. */

  std::string ast_get_identifier (const Node* ast);

}

#define ast_not_reached(node) do { std::cerr << node; not_reached; } while (0);

#endif /* Ast_hpp */
