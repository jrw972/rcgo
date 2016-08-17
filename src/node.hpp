#ifndef RC_SRC_NODE_HPP
#define RC_SRC_NODE_HPP

#include <vector>

#include "types.hpp"
#include "location.hpp"
#include "expression_value.hpp"

namespace ast
{

struct Node
{
  virtual ~Node();
  virtual void accept (NodeVisitor& visitor) = 0;
  virtual void visit_children (NodeVisitor& visitor) = 0;
  virtual void print (std::ostream& out = std::cout) const = 0;

  util::Location const location;

  semantic::ExpressionValue eval;

  type::Field* field;
  bool reset_mutability;
  // TODO:  Abstract a callable with a polymorphic function.
  const decl::Callable* callable;
  // TODO:  Move to ExpressionValue.
  const decl::PolymorphicFunction* polymorphic_function;
  // TODO:  Get rid of this member.
  runtime::Operation* operation;

protected:
  Node (unsigned int line_);
};

struct List : public Node
{
  typedef std::vector<Node*> ChildrenType;
  typedef ChildrenType::const_iterator ConstIterator;

  virtual void visit_children (NodeVisitor& visitor);
  size_t size () const;
  size_t empty () const;
  List* append (Node * child);
  Node* at (size_t idx) const;
  ConstIterator begin () const;
  ConstIterator end () const;

private:
  ChildrenType children_;
protected:
  List (unsigned int line);
};

struct Identifier : public Node
{
  Identifier (unsigned int line, const std::string& id);
  virtual void accept (NodeVisitor& visitor);
  virtual void visit_children (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  std::string const identifier;
};

struct IdentifierList : public List
{
  IdentifierList (unsigned int line);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Receiver : public Node
{
  Receiver (unsigned int line, Identifier* a_identifier, Mutability a_mutability, Mutability a_indirection_mutability, bool a_is_pointer, Identifier* a_type);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Identifier* const identifier;
  Mutability const mutability;
  Mutability const indirection_mutability;
  bool const is_pointer;
  Identifier* const type;
};

struct Array : public Node
{
  Array (unsigned int line, Node* dim, Node* base);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const dimension;
  Node* const base_type;
};

template <typename T = Node>
struct Unary : public Node
{
  Unary (unsigned int line, T* c)
    : Node (line)
    , child (c)
  { }

  virtual void visit_children (NodeVisitor& visitor)
  {
    child->accept (visitor);
  }

  T* const child;
};

struct EmptyType : public Node
{
  EmptyType (unsigned int line);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);
};

struct FieldList : public List
{
  FieldList (unsigned int line);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;

  bool is_component;
};

struct Heap : public Unary<>
{
  Heap (unsigned int line, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

// TODO:  Use this node more often in the grammar to eliminate duplicate code.
struct IdentifierType : public Unary<Identifier>
{
  IdentifierType (unsigned int line, Identifier* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct VariableList : public Node
{
  VariableList (unsigned int line, List* a_identifiers, Mutability a_mutability, Mutability a_indirection_mutability, Node* a_type);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  List* const identifiers;
  Mutability const mutability;
  Mutability const indirection_mutability;
  Node* const type;
};

struct Pointer : public Unary<>
{
  Pointer (unsigned int line, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Slice : public Unary<>
{
  Slice (unsigned int line, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Map : public Node
{
  Map (unsigned int line, Node* k, Node* v);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const key;
  Node* const value;
};

struct PushPort : public Node
{
  PushPort (unsigned int line, List* a_parameters);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  List* const parameters;
};

struct PullPort : public Node
{
  PullPort (unsigned int line, List* a_parameters, List* a_return_parameters);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  List* const parameters;
  List* const return_parameters;
};

struct ParameterList : public List
{
  ParameterList (unsigned int line);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct TypeExpression : public Unary<>
{
  TypeExpression (unsigned int line, Node* a_type);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Binary : public Node
{
  Binary (unsigned int line, Node* l, Node* r);
  virtual void visit_children (NodeVisitor& visitor);

  Node* const left;
  Node* const right;
};

// TODO:  Get rid of this and replace it with a call.
struct BinaryArithmetic : public Binary
{
  BinaryArithmetic (unsigned int line,
                    decl::PolymorphicFunction* temp,
                    Node* left,
                    Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct AddressOf : public Unary<>
{
  AddressOf (unsigned int line, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Call : public Node
{
  Call (unsigned int line, Node* e, List* a);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const expression;
  List* const arguments;

  const type::Function* function_type;
  const type::PushPort* push_port_type;
  const type::PullPort* pull_port_type;
  const type::Method* method_type;
  const type::Initializer* initializer_type;
  const type::Getter* getter_type;
  const type::Reaction* reaction_type;

  const decl::ParameterList* parameters;
  const decl::ParameterList* return_parameters;
};

struct Conversion : public Node
{
  Conversion (unsigned int line, Node* te, Node* e);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const type;
  Node* const argument;
};

// TODO:  Rename to Indirection.
struct Dereference : public Unary<>
{
  Dereference (unsigned int line, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct ExpressionList : public List
{
  ExpressionList (unsigned int line);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct IdentifierExpression : public Unary<Identifier>
{
  IdentifierExpression (unsigned int line, Identifier* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;

  decl::Symbol* symbol;
};

struct Index : public Node
{
  Index (unsigned int line, Node* a_base, Node* a_index);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const base;
  Node* const index;

  const type::Array* array_type;
  const type::Slice* slice_type;
};

struct IndexSlice : public Node
{
  IndexSlice (unsigned int line, Node* a_base, Node* a_low, Node* a_high, Node* a_max);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const base;
  Node* const low;
  bool const low_present;
  Node* const high;
  bool const high_present;
  Node* const max;
  bool const max_present;

  const type::UntypedString* string_type;
  const type::Pointer* pointer_to_array_type;
  const type::Array* array_type;
  const type::Slice* slice_type;
};

struct EmptyExpression : public Node
{
  EmptyExpression (unsigned int line);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);
};

// TODO:  Get rid of this class.
struct UnaryArithmetic : public Unary<>
{
  UnaryArithmetic (unsigned int line, decl::PolymorphicFunction* temp, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct PushPortCall : public Node
{
  PushPortCall (unsigned int line, Identifier* a_identifier, List* a_arguments);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Identifier* const identifier;
  List* const arguments;

  type::Field* field;
  const type::PushPort* push_port_type;
  decl::Parameter* receiver_parameter;
};

struct IndexedPushPortCall : public Node
{
  IndexedPushPortCall (unsigned int line, Identifier* a_identifier, Node* a_index, List* a_arguments);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Identifier* const identifier;
  Node* const index;
  List* const arguments;

  type::Field* field;
  const type::PushPort* push_port_type;
  decl::Parameter* receiver_parameter;
  const type::Array* array_type;
};

struct Select : public Node
{
  Select (unsigned int line, Node* a_base, Identifier* a_identifier);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const base;
  Identifier* const identifier;
};

struct Literal : public Node
{
  Literal (unsigned int line, const type::Type* a_type, const semantic::Value& a_value);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);
};

struct EmptyStatement : public Node
{
  EmptyStatement (unsigned int line);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);
};

struct AddAssign : public Binary
{
  AddAssign (unsigned int line, Node* left, Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Assign : public Binary
{
  Assign (unsigned int line, Node* left, Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Change : public Node
{
  Change (unsigned int line,
          Node * e,
          Identifier * id,
          Node * b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const argument;
  Identifier* const identifier;
  Node* const body;

  decl::Variable* root_symbol;
};

struct ExpressionStatement : public Unary<>
{
  ExpressionStatement (unsigned int line, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct If : public Node
{
  If (unsigned int line, Node* a_before, Node* a_condition, Node* a_true_body, Node* false_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const before;
  Node* const condition;
  Node* const true_body;
  Node* const false_body;
};

// TODO:  This should be For.
struct While : public Node
{
  While (unsigned int line, Node* a_condition, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const condition;
  Node* const body;
};

struct Return : public Unary<>
{
  Return (unsigned int line, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;

  const decl::Parameter* return_symbol;
};

// TODO:  Eliminate this node.  It is syntactic sugar.
struct IncrementDecrement : public Unary<>
{
  enum Kind
  {
    Increment,
    Decrement
  };
  IncrementDecrement (unsigned int line, Node* child, Kind a_kind);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  Kind const kind;
};

struct StatementList : public List
{
  StatementList (unsigned int line);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct SubtractAssign : public Binary
{
  SubtractAssign (unsigned int line, Node* left, Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Activate : public Node
{
  Activate (unsigned int line, List * el, Node * b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  List* const arguments;
  Node* const body;

  const runtime::MemoryModel* memory_model;
  ReceiverAccess mutable_phase_access;
  bool in_action;
};

struct Var : public Node
{
  Var (unsigned int line, List* a_identifiers, Mutability a_mutability, Mutability a_indirection_mutability, Node* a_type, List* ael);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  // TODO:  Use a VariableList.
  List* const identifiers;
  Mutability const mutability;
  Mutability const indirection_mutability;
  Node* const type;
  List* const expressions;

  typedef std::vector<decl::Variable*> SymbolsType;
  SymbolsType symbols;
};

struct BindPushPort : public Binary
{
  BindPushPort (unsigned int line, Node* left, Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct BindPushPortParameter : public Node
{
  BindPushPortParameter (unsigned int line,
                         Node* l,
                         Node* r,
                         Node* p);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const left;
  Node* const right;
  Node* const param;
};

struct BindPullPort : public Binary
{
  BindPullPort (unsigned int line, Node* left, Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct ForIota : public Node
{
  ForIota (unsigned int line, Identifier* a_identifier, Node* a_limit, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Identifier* const identifier;
  Node* const limit;
  Node* const body;

  decl::Variable* symbol;
  long limit_value;
};

struct Action : public Node
{
  Action (unsigned int line,
          Node* r,
          Identifier* i,
          Node* p,
          Node* b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  Node* const precondition;
  Node* const body;

  decl::Action* action;
  const type::Type* type;
};

struct DimensionedAction : public Node
{
  DimensionedAction (unsigned int line,
                     Node* d,
                     Node* r,
                     Identifier* i,
                     Node* p,
                     Node* b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const dimension;
  Node* const receiver;
  Identifier* const identifier;
  Node* const precondition;
  Node* const body;

  decl::Action* action;
  const type::Type* type;
};

struct Bind : public Node
{
  Bind (unsigned int line,
        Node* r,
        Identifier* i,
        Node* b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  Node* const body;

  decl::Bind* bind;
};

struct Function : public Node
{
  Function (unsigned int line, Identifier* a_identifier, List* a_parameters, List* return_parameters, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Identifier* const identifier;
  List* const parameters;
  List* const return_parameters;
  Node* const body;

  decl::Function* function;
};

struct Instance : public Node
{
  Instance (unsigned int line, Identifier* a_identifier, Node* a_type, Identifier* a_initializer, List* a_arguments);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Identifier* const identifier;
  Node* const type;
  Identifier* const initializer;
  List* const arguments;

  decl::Instance* symbol;
};

struct Const : public Node
{
  Const (unsigned int line,
         List* il,
         Node* ts,
         List* el);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  List* const identifiers;
  Node* const type;
  List* const expressions;

  bool done;
};

struct Method : public Node
{
  Method (unsigned int line, Node* a_receiver, Identifier* a_identifier,  List* a_parameters, List* a_return_parameters, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  List* const parameters;
  List* const return_parameters;
  Node* const body;

  decl::Method* method;
};

struct Getter : public Node
{
  Getter (unsigned int line, Node* a_receiver, Identifier* a_identifier, List* a_parameters, List* a_return_parameters, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  List* const parameters;
  List* const return_parameters;
  Node* const body;

  decl::Getter* getter;
};

struct Initializer : public Node
{
  Initializer (unsigned int line, Node* a_receiver, Identifier* a_identifier, List* a_parameters, List* a_return_parameters, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  List* const parameters;
  List* const return_parameters;
  Node* const body;

  decl::Initializer* initializer;
};

struct Reaction : public Node
{
  Reaction (unsigned int line, Node* a_receiver, Identifier* a_identifier, List* a_parameters, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  List* const parameters;
  List* const return_parameters;
  Node* const body;

  decl::Reaction* reaction;
};

struct DimensionedReaction : public Node
{
  DimensionedReaction (unsigned int line,
                       Node* d,
                       Node* r,
                       Identifier* i,
                       List* pl,
                       Node* b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const dimension;
  Node* const receiver;
  Identifier* const identifier;
  List* const parameters;
  List* const return_parameters;
  Node* const body;

  decl::Reaction* reaction;
};

struct Type : public Node
{
  Type (unsigned int line, Identifier* a_identifier, Node* a_type);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Identifier* const identifier;
  Node* const type;
};

struct SourceFile : public List
{
  SourceFile ();
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct ElementList : public List
{
  ElementList (unsigned int line);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Element : public Node
{
  Element (unsigned int line, Node* a_key, Node* a_value);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const key;
  Node* const value;
};

struct CompositeLiteral : public Node
{
  CompositeLiteral (unsigned int line,
                    Node* lt,
                    List* lv);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const type;
  List* const value;
};

std::ostream& operator<< (std::ostream& out, const Node& node);
}

#define AST_NOT_REACHED(node) do { std::cerr << node; NOT_REACHED; } while (0);

#endif // RC_SRC_NODE_HPP
