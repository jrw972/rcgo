#ifndef RC_SRC_AST_HPP
#define RC_SRC_AST_HPP

#include <vector>

#include "types.hpp"
#include "location.hpp"
#include "value.hpp"

namespace ast
{

struct Node
{
  virtual ~Node() { }
  virtual void accept (Visitor& visitor) = 0;
  virtual void visit_children (Visitor& visitor) = 0;

  util::Location const location;

  const type::Type* type;
  semantic::Value value;
  type::field_t* field;
  bool reset_mutability;
  const decl::Callable* callable;
  const decl::Template* temp;
  ExpressionKind expression_kind;
  Mutability intrinsic_mutability;
  Mutability indirection_mutability;
  bool receiver_state;
  ReceiverAccess receiver_access;
  runtime::Operation* operation;

protected:
  Node (unsigned int line_);
};

struct List : public Node
{
  typedef std::vector<Node*> ChildrenType;
  typedef ChildrenType::const_iterator ConstIterator;

  virtual void visit_children (Visitor& visitor);
  size_t size () const;
  size_t empty () const;
  List* append (Node * child);
  Node* at (size_t idx) const;
  ConstIterator begin () const;
  ConstIterator end () const;

private:
  ChildrenType children_;
protected:
  List (unsigned int line) : Node (line) { }
};

struct Identifier : public Node
{
  Identifier (unsigned int line, const std::string& id);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor) { }

  std::string const identifier;
};

struct IdentifierList : public List
{
  IdentifierList (unsigned int line);
  virtual void accept (Visitor& visitor);
};

struct Receiver : public Node
{
  Receiver (unsigned int line,
            Identifier* this_id,
            Mutability m,
            Mutability dm,
            bool isP,
            Identifier* type_id);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Identifier* const this_identifier;
  Mutability const mutability;
  Mutability const indirection_mutability;
  bool const is_pointer;
  Identifier* const type_identifier;
};

struct ArrayTypeSpec : public Node
{
  ArrayTypeSpec (unsigned int line, Node* dim, Node* base);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

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

  virtual void visit_children (Visitor& visitor)
  {
    child->accept (visitor);
  }

  T* const child;
};

struct EmptyTypeSpec : public Node
{
  EmptyTypeSpec (unsigned int line);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor) { }
};

struct FieldListTypeSpec : public List
{
  FieldListTypeSpec (unsigned int line);
  virtual void accept (Visitor& visitor);

  bool is_component;
};

struct HeapTypeSpec : public Unary<>
{
  HeapTypeSpec (unsigned int line, Node* child);
  virtual void accept (Visitor& visitor);
};

struct IdentifierTypeSpec : public Unary<Identifier>
{
  IdentifierTypeSpec (unsigned int line, Identifier* child);
  virtual void accept (Visitor& visitor);
};

struct IdentifierListTypeSpec : public Node
{
  IdentifierListTypeSpec (unsigned int line,
                          List* id_list,
                          Mutability m,
                          Mutability dm,
                          Node* type_sp);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  List* const identifier_list;
  Mutability const mutability;
  Mutability const indirection_mutability;
  Node* const type_spec;
};

struct PointerTypeSpec : public Unary<>
{
  PointerTypeSpec (unsigned int line, Node* child);
  virtual void accept (Visitor& visitor);
};

struct SliceTypeSpec : public Unary<>
{
  SliceTypeSpec (unsigned int line, Node* child);
  virtual void accept (Visitor& visitor);
};

struct MapTypeSpec : public Node
{
  MapTypeSpec (unsigned int line, Node* k, Node* v);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const key;
  Node* const value;
};

struct PushPortTypeSpec : public Node
{
  PushPortTypeSpec (unsigned int line, Node* sig);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const signature;
};

struct PullPortTypeSpec : public Node
{
  PullPortTypeSpec (unsigned int line,
                    Node* sig,
                    Mutability dm,
                    Node* rt);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const signature;
  Mutability const indirection_mutability;
  Node* const return_type;
};

struct SignatureTypeSpec : public List
{
  SignatureTypeSpec (unsigned int line);
  virtual void accept (Visitor& visitor);
};

struct TypeExpression : public Node
{
  TypeExpression (unsigned int line, Node* ts);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const type_spec;
};

struct Binary : public Node
{
  Binary (unsigned int line, Node* l, Node* r);
  virtual void visit_children (Visitor& visitor);

  Node* const left;
  Node* const right;
};

struct BinaryArithmeticExpr : public Binary
{
  BinaryArithmeticExpr (unsigned int line,
                        BinaryArithmetic a,
                        Node* left,
                        Node* right);
  virtual void accept (Visitor& visitor);

  BinaryArithmetic const arithmetic;
};

struct AddressOfExpr : public Unary<>
{
  AddressOfExpr (unsigned int line, Node* child);
  virtual void accept (Visitor& visitor);
};

struct CallExpr : public Node
{
  CallExpr (unsigned int line, Node* e, List* a);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const expr;
  List* const args;

  const type::Function* function_type;
  const type::Method* method_type;
  const type::Signature* signature;
  const decl::ParameterSymbol* return_parameter;
};

struct ConversionExpr : public Node
{
  ConversionExpr (unsigned int line, Node* te, Node* e);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const type_expr;
  Node* const expr;
};

struct DereferenceExpr : public Unary<>
{
  DereferenceExpr (unsigned int line, Node* child);
  virtual void accept (Visitor& visitor);
};

struct ListExpr : public List
{
  ListExpr (unsigned int line);
  virtual void accept (Visitor& visitor);
};

struct IdentifierExpr : public Unary<Identifier>
{
  IdentifierExpr (unsigned int line, Identifier* child);
  virtual void accept (Visitor& visitor);

  decl::Symbol* symbol;
};

struct IndexExpr : public Node
{
  IndexExpr (unsigned int line, Node* b, Node* i);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const base;
  Node* const index;

  const type::Array* array_type;
  const type::Slice* slice_type;
};

struct SliceExpr : public Node
{
  SliceExpr (unsigned int line,
             Node* b,
             Node* l,
             Node* h,
             Node* m);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const base;
  Node* const low;
  bool const low_present;
  Node* const high;
  bool const high_present;
  Node* const max;
  bool const max_present;

  const type::String* string_type;
  const type::Pointer* pointer_to_array_type;
  const type::Array* array_type;
  const type::Slice* slice_type;
};

struct EmptyExpr : public Node
{
  EmptyExpr (unsigned int line);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor) { }
};

struct UnaryArithmeticExpr : public Unary<>
{
  UnaryArithmeticExpr (unsigned int line, UnaryArithmetic a, Node* child);
  virtual void accept (Visitor& visitor);

  const UnaryArithmetic arithmetic;
};

struct PushPortCallExpr : public Node
{
  PushPortCallExpr (unsigned int line, Identifier* id, List* a);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Identifier* const identifier;
  List* const args;

  type::field_t* field;
  decl::ParameterSymbol* receiver_parameter;
};

struct IndexedPushPortCallExpr : public Node
{
  IndexedPushPortCallExpr (unsigned int line,
                           Identifier* id,
                           Node* idx,
                           List* a);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Identifier* const identifier;
  Node* const index;
  List* const args;

  type::field_t* field;
  decl::ParameterSymbol* receiver_parameter;
  const type::Array* array_type;
};

struct SelectExpr : public Node
{
  SelectExpr (unsigned int line, Node* b, Identifier* id);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const base;
  Identifier* const identifier;
};

struct LiteralExpr : public Node
{
  LiteralExpr (unsigned int line,
               const type::Type* t,
               const semantic::Value& v);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor) { }
};

struct EmptyStatement : public Node
{
  EmptyStatement (unsigned int line);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor) { }
};

struct AddAssignStatement : public Binary
{
  AddAssignStatement (unsigned int line, Node* left, Node* right);
  virtual void accept (Visitor& visitor);
};

struct AssignStatement : public Binary
{
  AssignStatement (unsigned int line, Node* left, Node* right);
  virtual void accept (Visitor& visitor);
};

struct ChangeStatement : public Node
{
  ChangeStatement (unsigned int line,
                   Node * e,
                   Identifier * id,
                   Node * b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const expr;
  Identifier* const identifier;
  Node* const body;

  decl::VariableSymbol* root_symbol;
};

struct ExpressionStatement : public Unary<>
{
  ExpressionStatement (unsigned int line, Node* child);
  virtual void accept (Visitor& visitor);
};

struct IfStatement : public Node
{
  IfStatement (unsigned int line,
               Node* s,
               Node* c,
               Node* tb,
               Node* fb);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const statement;
  Node* const condition;
  Node* const true_branch;
  Node* const false_branch;
};

struct WhileStatement : public Node
{
  WhileStatement (unsigned int line, Node* c, Node* b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const condition;
  Node* const body;
};

struct ReturnStatement : public Unary<>
{
  ReturnStatement (unsigned int line, Node* child);
  virtual void accept (Visitor& visitor);

  const decl::ParameterSymbol* return_symbol;
};

struct IncrementStatement : public Unary<>
{
  IncrementStatement (unsigned int line, Node* child);
  virtual void accept (Visitor& visitor);
};

struct DecrementStatement : public Unary<>
{
  DecrementStatement (unsigned int line, Node* child);
  virtual void accept (Visitor& visitor);
};

struct ListStatement : public List
{
  ListStatement (unsigned int line);
  virtual void accept (Visitor& visitor);
};

struct SubtractAssignStatement : public Binary
{
  SubtractAssignStatement (unsigned int line, Node* left, Node* right);
  virtual void accept (Visitor& visitor);
};

struct ActivateStatement : public Node
{
  ActivateStatement (unsigned int line, List * el, Node * b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  List* const expr_list;
  Node* const body;

  const runtime::MemoryModel* memory_model;
  ReceiverAccess mutable_phase_access;
  bool in_action;
};

struct VarStatement : public Node
{
  VarStatement (unsigned int line,
                List* il,
                Mutability m,
                Mutability dm,
                Node* ts,
                List* el);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  List* const identifier_list;
  Mutability const mutability;
  Mutability const dereferenceMutability;
  Node* const type_spec;
  List* const expression_list;

  typedef std::vector<decl::VariableSymbol*> SymbolsType;
  SymbolsType symbols;
};

struct BindPushPortStatement : public Binary
{
  BindPushPortStatement (unsigned int line, Node* left, Node* right);
  virtual void accept (Visitor& visitor);
};

struct BindPushPortParamStatement : public Node
{
  BindPushPortParamStatement (unsigned int line,
                              Node* l,
                              Node* r,
                              Node* p);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const left;
  Node* const right;
  Node* const param;
};

struct BindPullPortStatement : public Binary
{
  BindPullPortStatement (unsigned int line, Node* left, Node* right);
  virtual void accept (Visitor& visitor);
};

struct ForIotaStatement : public Node
{
  ForIotaStatement (unsigned int line,
                    Identifier* id,
                    Node* lim,
                    Node* b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Identifier* const identifier;
  Node* const limit_node;
  Node* const body;

  decl::VariableSymbol* symbol;
  type::Int::ValueType limit;
};

struct Action : public Node
{
  Action (unsigned int line,
          Node* r,
          Identifier* i,
          Node* p,
          Node* b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

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
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

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
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  Node* const body;

  decl::bind_t* bind;
};

struct Function : public Node
{
  Function (unsigned int line,
            Identifier* i,
            Node* s,
            Mutability dm,
            Node* rt,
            Node* b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Identifier* const identifier;
  Node* const signature;
  Mutability const indirection_mutability;
  Node* const return_type;
  Node* const body;

  decl::Function* function;
};

struct Instance : public Node
{
  Instance (unsigned int line,
            Identifier* id,
            Node* tn,
            Identifier* init,
            List* el);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Identifier* const identifier;
  Node* const type_name;
  Identifier* const initializer;
  List* const expression_list;

  decl::InstanceSymbol* symbol;
};

struct Const : public Node
{
  Const (unsigned int line,
         List* il,
         Node* ts,
         List* el);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  List* const identifier_list;
  Node* const type_spec;
  List* const expression_list;

  bool done;
};

struct Method : public Node
{
  Method (unsigned int line,
          Node * r,
          Identifier * i,
          Node * s,
          Mutability return_dm,
          Node * rt,
          Node* b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  Node* const signature;
  Mutability const return_indirection_mutability;
  Node* const return_type;
  Node* const body;

  decl::Method* method;
};

struct Getter : public Node
{
  Getter (unsigned int line,
          Node * r,
          Identifier * i,
          Node * s,
          Mutability dm,
          Node * rt,
          Node* b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  Node* const signature;
  Mutability const indirection_mutability;
  Node* const return_type;
  Node* const body;

  decl::Getter* getter;
};

struct Initializer : public Node
{
  Initializer (unsigned int line,
               Node* r,
               Identifier * i,
               Node * s,
               Mutability return_dm,
               Node* rt,
               Node* b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  Node* const signature;
  Mutability const return_indirection_mutability;
  Node* const return_type;
  Node* const body;

  decl::Initializer* initializer;
};

struct Reaction : public Node
{
  Reaction (unsigned int line,
            Node* r,
            Identifier* i,
            Node* s,
            Node* b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const receiver;
  Identifier* const identifier;
  Node* const signature;
  Node* const return_type;
  Node* const body;

  decl::reaction_t* reaction;
};

struct DimensionedReaction : public Node
{
  DimensionedReaction (unsigned int line,
                       Node* d,
                       Node* r,
                       Identifier* i,
                       Node* s,
                       Node* b);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const dimension;
  Node* const receiver;
  Identifier* const identifier;
  Node* const signature;
  Node* const return_type;
  Node* const body;

  decl::reaction_t* reaction;
};

struct Type : public Node
{
  Type (unsigned int line, Identifier* i, Node* ts);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Identifier* const identifier;
  Node* const type_spec;
};

struct SourceFile : public List
{
  SourceFile ();
  virtual void accept (Visitor& visitor);
};

struct ElementList : public List
{
  ElementList (unsigned int line);
  virtual void accept (Visitor& visitor);
};

struct Element : public Node
{
  Element (unsigned int line, Node* k, Node* v);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const key;
  Node* const value;
};

struct CompositeLiteral : public Node
{
  CompositeLiteral (unsigned int line,
                    Node* lt,
                    List* lv);
  virtual void accept (Visitor& visitor);
  virtual void visit_children (Visitor& visitor);

  Node* const literal_type;
  List* const literal_value;
};

std::ostream& operator<< (std::ostream& out, Node& node);
}

#define AST_NOT_REACHED(node) do { std::cerr << node; NOT_REACHED; } while (0);

#endif // RC_SRC_AST_HPP
