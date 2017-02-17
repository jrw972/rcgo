#ifndef RC_SRC_NODE_HPP
#define RC_SRC_NODE_HPP

#include <vector>

#include "types.hpp"
#include "location.hpp"
#include "semantic.hpp"
#include "identifier.hpp"

namespace ast
{

struct Node
{
  virtual ~Node();
  virtual void accept (NodeVisitor& visitor) = 0;
  virtual void visit_children (NodeVisitor& visitor) = 0;
  virtual void print (std::ostream& out = std::cout) const = 0;

  source::Location const location;

  semantic::ExpressionValue eval;

  decl::Field* field;
  bool reset_mutability;
  // TODO:  Abstract a callable with a polymorphic function.
  const decl::Callable* callable;
  // TODO:  Get rid of this member.
  runtime::Operation* operation;

protected:
  Node (const source::Location& a_location);
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
  List (const source::Location& location);
};

struct IdentifierList : public Node
{
  IdentifierList (const source::Location& location);
  virtual void accept (NodeVisitor& visitor);
  virtual void visit_children (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;

  typedef std::vector<source::Identifier> IdentifiersType;
  IdentifiersType identifiers;

  IdentifierList* append (const source::Identifier& identifier);
  size_t size () const;
  typedef IdentifiersType::const_iterator const_iterator;
  const_iterator begin () const;
  const_iterator end () const;
};

struct Receiver : public Node
{
  Receiver (const source::Location& location, const source::Identifier& a_identifier, Mutability a_mutability, Mutability a_indirection_mutability, bool a_is_pointer, const source::Identifier& a_type);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  source::Identifier const identifier;
  Mutability const mutability;
  Mutability const indirection_mutability;
  bool const is_pointer;
  source::Identifier const type_identifier;

  decl::Symbol* identifier_symbol;
  decl::Symbol* type_identifier_symbol;
};

struct Array : public Node
{
  Array (const source::Location& location, Node* dim, Node* base);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const dimension;
  Node* const base_type;
};

template <typename T = Node>
struct Unary : public Node
{
  Unary (const source::Location& location, T* c)
    : Node (location)
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
  EmptyType (const source::Location& location);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);
};

struct FieldList : public List
{
  FieldList (const source::Location& location);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;

  bool is_component;
};

struct Heap : public Unary<>
{
  Heap (const source::Location& location, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

// TODO:  Use this node more often in the grammar to eliminate duplicate code.
struct IdentifierType : public Node
{
  IdentifierType (const source::Location& location, const source::Identifier& identifier);
  virtual void accept (NodeVisitor& visitor);
  virtual void visit_children (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;

  source::Identifier const identifier;
};

struct VariableList : public Node
{
  VariableList (const source::Location& location, IdentifierList* a_identifiers, Mutability a_mutability, Mutability a_indirection_mutability, Node* a_type);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  IdentifierList* const identifiers;
  Mutability const mutability;
  Mutability const indirection_mutability;
  Node* const type;
};

struct Pointer : public Unary<>
{
  Pointer (const source::Location& location, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Slice : public Unary<>
{
  Slice (const source::Location& location, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Map : public Node
{
  Map (const source::Location& location, Node* k, Node* v);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const key;
  Node* const value;
};

struct PushPort : public Node
{
  PushPort (const source::Location& location, ParameterList* a_parameters);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  ParameterList* const parameters;
};

struct PullPort : public Node
{
  PullPort (const source::Location& location, ParameterList* a_parameters, ParameterList* a_return_parameters);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  ParameterList* const parameters;
  ParameterList* const return_parameters;
};

struct ParameterList : public List
{
  ParameterList (const source::Location& location);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct TypeExpression : public Unary<>
{
  TypeExpression (const source::Location& location, Node* a_type);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Binary : public Node
{
  Binary (const source::Location& location, Node* l, Node* r);
  virtual void visit_children (NodeVisitor& visitor);

  Node* const left;
  Node* const right;
};

// TODO:  Use Call?
struct AddressOf : public Unary<>
{
  AddressOf (const source::Location& location, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Call : public Node
{
  Call (const source::Location& location, Node* e, List* a);
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
  Conversion (const source::Location& location, Node* te, Node* e);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const type;
  Node* const argument;
};

// TODO:  Rename to Indirection.
// TODO:  Use Call?
struct Dereference : public Unary<>
{
  Dereference (const source::Location& location, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct ExpressionList : public List
{
  ExpressionList (const source::Location& location);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct IdentifierExpression : public Node
{
  IdentifierExpression (const source::Location& location, const source::Identifier& a_identifier);
  virtual void accept (NodeVisitor& visitor);
  virtual void visit_children (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;

  source::Identifier const identifier;
  decl::Symbol* symbol;
};

struct Index : public Node
{
  Index (const source::Location& location, Node* a_base, Node* a_index);
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
  IndexSlice (const source::Location& location, Node* a_base, Node* a_low, Node* a_high, Node* a_max);
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
  EmptyExpression (const source::Location& location);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);
};

struct PushPortCall : public Node
{
  PushPortCall (const source::Location& location, const source::Identifier& a_identifier, List* a_arguments);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  source::Identifier const identifier;
  List* const arguments;

  decl::Field* field;
  const type::PushPort* push_port_type;
  decl::Parameter* receiver_parameter;
};

struct IndexedPushPortCall : public Node
{
  IndexedPushPortCall (const source::Location& location, const source::Identifier& a_identifier, Node* a_index, List* a_arguments);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  source::Identifier const identifier;
  Node* const index;
  List* const arguments;

  decl::Field* field;
  const type::PushPort* push_port_type;
  decl::Parameter* receiver_parameter;
  const type::Array* array_type;
};

struct Select : public Node
{
  Select (const source::Location& location, Node* a_base, const source::Identifier& a_identifier);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const base;
  source::Identifier const identifier;
};

struct Literal : public Node
{
  Literal (const source::Location& location, const semantic::ExpressionValue& a_value);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);
};

struct EmptyStatement : public Node
{
  EmptyStatement (const source::Location& location);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);
};

struct AddAssign : public Binary
{
  AddAssign (const source::Location& location, Node* left, Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Assign : public Binary
{
  Assign (const source::Location& location, Node* left, Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Change : public Node
{
  Change (const source::Location& location,
          Node* e,
          const source::Identifier& id,
          Node * b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const argument;
  source::Identifier const identifier;
  Node* const body;

  decl::Variable* root_symbol;
};

struct ExpressionStatement : public Unary<>
{
  ExpressionStatement (const source::Location& location, Node* child);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct If : public Node
{
  If (const source::Location& location, Node* a_before, Node* a_condition, Node* a_true_body, Node* false_body);
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
  While (const source::Location& location, Node* a_condition, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const condition;
  Node* const body;
};

struct Return : public Unary<>
{
  Return (const source::Location& location, Node* child);
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
  IncrementDecrement (const source::Location& location, Node* child, Kind a_kind);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  Kind const kind;
};

struct StatementList : public List
{
  StatementList (const source::Location& location);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;

  bool is_body;
};

struct SubtractAssign : public Binary
{
  SubtractAssign (const source::Location& location, Node* left, Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Activate : public Node
{
  Activate (const source::Location& location, List * el, Node * b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  List* const arguments;
  Node* const body;

  const runtime::MemoryModel* memory_model;
  ReceiverAccess mutable_phase_access;
  bool in_action;
};

struct VarDecl : public Node
{
  VarDecl (const source::Location& location, IdentifierList* a_identifiers, Mutability a_mutability, Mutability a_indirection_mutability, Node* a_type, List* ael);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  // TODO:  Use a VariableList.
  IdentifierList* const identifiers;
  Mutability const mutability;
  Mutability const indirection_mutability;
  Node* const type;
  List* const expressions;

  typedef std::vector<decl::Variable*> SymbolsType;
  SymbolsType symbols;
};

struct BindPushPort : public Binary
{
  BindPushPort (const source::Location& location, Node* left, Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct BindPushPortParameter : public Node
{
  BindPushPortParameter (const source::Location& location,
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
  BindPullPort (const source::Location& location, Node* left, Node* right);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct ForIota : public Node
{
  ForIota (const source::Location& location, const source::Identifier& a_identifier, Node* a_limit, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  source::Identifier const identifier;
  Node* const limit;
  Node* const body;

  decl::Variable* symbol;
  long limit_value;
};

struct ActionDecl : public Node
{
  ActionDecl (const source::Location& location,
              Node* d,
              Receiver* r,
              const source::Identifier& i,
              Node* p,
              Node* b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const dimension;
  Receiver* const receiver;
  source::Identifier const identifier;
  Node* const precondition;
  Node* const body;

  decl::Action* action;
};

struct BinderDecl : public Node
{
  BinderDecl (const source::Location& location,
              Receiver* r,
              const source::Identifier& i,
              Node* b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Receiver* const receiver;
  source::Identifier const identifier;
  Node* const body;

  decl::Bind* bind;
};

struct FunctionDecl : public Node
{
  FunctionDecl (const source::Location& location, const source::Identifier& a_identifier, ParameterList* a_parameters, ParameterList* return_parameters, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  source::Identifier const identifier;
  ParameterList* const parameters;
  ParameterList* const return_parameters;
  Node* const body;

  decl::Function* symbol;
};

struct InstanceDecl : public Node
{
  InstanceDecl (const source::Location& location, const source::Identifier& a_identifier, Node* a_type, const source::Identifier& a_initializer, List* a_arguments);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  source::Identifier const identifier;
  Node* const type;
  source::Identifier const initializer;
  List* const arguments;

  decl::Instance* symbol;
};

struct ConstDecl : public Node
{
  ConstDecl (const source::Location& location,
             IdentifierList* il,
             Node* ts,
             List* el);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  IdentifierList* const identifiers;
  Node* const type;
  List* const expressions;

  typedef std::vector<decl::Constant*> SymbolsType;
  SymbolsType symbols;
};

struct MethodDecl : public Node
{
  MethodDecl (const source::Location& location, Receiver* a_receiver, const source::Identifier& a_identifier,  ParameterList* a_parameters, ParameterList* a_return_parameters, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Receiver* const receiver;
  source::Identifier const identifier;
  ParameterList* const parameters;
  ParameterList* const return_parameters;
  Node* const body;

  decl::Method* method;
};

struct GetterDecl : public Node
{
  GetterDecl (const source::Location& location, Receiver* a_receiver, const source::Identifier& a_identifier, ParameterList* a_parameters, ParameterList* a_return_parameters, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Receiver* const receiver;
  source::Identifier const identifier;
  ParameterList* const parameters;
  ParameterList* const return_parameters;
  Node* const body;

  decl::Getter* getter;
};

struct InitializerDecl : public Node
{
  InitializerDecl (const source::Location& location, Receiver* a_receiver, const source::Identifier& a_identifier, ParameterList* a_parameters, ParameterList* a_return_parameters, Node* a_body);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Receiver* const receiver;
  source::Identifier const identifier;
  ParameterList* const parameters;
  ParameterList* const return_parameters;
  Node* const body;

  decl::Initializer* initializer;
};

struct ReactionDecl : public Node
{
  ReactionDecl (const source::Location& location,
                Node* d,
                Receiver* r,
                const source::Identifier& i,
                ParameterList* pl,
                Node* b);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const dimension;
  Receiver* const receiver;
  source::Identifier const identifier;
  ParameterList* const parameters;
  ParameterList* const return_parameters;
  Node* const body;

  decl::Reaction* reaction;
};

struct TypeDecl : public Node
{
  TypeDecl (const source::Location& location, const source::Identifier& a_identifier, Node* a_type);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  source::Identifier const identifier;
  Node* const type;

  type::NamedType* symbol;
};

struct ElementList : public List
{
  ElementList (const source::Location& location);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
};

struct Element : public Node
{
  Element (const source::Location& location, Node* a_key, Node* a_value);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const key;
  Node* const value;
};

struct CompositeLiteral : public Node
{
  CompositeLiteral (const source::Location& location,
                    Node* lt,
                    List* lv);
  virtual void accept (NodeVisitor& visitor);
  virtual void print (std::ostream& out) const;
  virtual void visit_children (NodeVisitor& visitor);

  Node* const type;
  List* const value;
};

  struct TopLevelDeclList : public List
  {
    TopLevelDeclList (const source::Location& location);
    virtual void accept (NodeVisitor& visitor);
    virtual void print (std::ostream& out = std::cout) const;
  };

Call* make_unary (const source::Location& location, decl::PolymorphicFunction* func, Node* child);

Call* make_binary (const source::Location& location, decl::PolymorphicFunction* func, Node* left, Node* right);

std::ostream& operator<< (std::ostream& out, const Node& node);
}

#define NODE_NOT_REACHED(node) do { std::cerr << node << std::endl; NOT_REACHED; } while (0);

#endif // RC_SRC_NODE_HPP
