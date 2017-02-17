#include "node.hpp"

#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "type.hpp"

using namespace source;

#define ACCEPT(type) void                               \
  type::accept (NodeVisitor& visitor)                 \
  {                                                     \
    visitor.visit (*this);                              \
  }

#define PRINT(type) void                               \
  type::print (std::ostream& out) const               \
  {                                                     \
    out << #type ;                               \
  }

namespace ast
{
using namespace decl;
using namespace semantic;

std::ostream&
operator<< (std::ostream& out, const Node& node)
{
  node.print (out);
  return out;
}

ACCEPT (ActionDecl)
ACCEPT (Activate)
ACCEPT (AddAssign)
ACCEPT (AddressOf)
ACCEPT (Array)
ACCEPT (Assign)
ACCEPT (BinderDecl)
ACCEPT (BindPullPort)
ACCEPT (BindPushPort)
ACCEPT (BindPushPortParameter)
ACCEPT (Call)
ACCEPT (Change)
ACCEPT (CompositeLiteral)
ACCEPT (ConstDecl)
ACCEPT (Conversion)
ACCEPT (Dereference)
ACCEPT (Element)
ACCEPT (ElementList)
ACCEPT (EmptyExpression)
ACCEPT (EmptyStatement)
ACCEPT (EmptyType)
ACCEPT (ExpressionList)
ACCEPT (ExpressionStatement)
ACCEPT (FieldList)
ACCEPT (ForIota)
ACCEPT (FunctionDecl)
ACCEPT (GetterDecl)
ACCEPT (Heap)
ACCEPT (IdentifierExpression)
ACCEPT (IdentifierList)
ACCEPT (IdentifierType)
ACCEPT (If)
ACCEPT (IncrementDecrement)
ACCEPT (Index)
ACCEPT (IndexSlice)
ACCEPT (IndexedPushPortCall)
ACCEPT (InitializerDecl)
ACCEPT (InstanceDecl)
ACCEPT (Literal)
ACCEPT (Map)
ACCEPT (MethodDecl)
ACCEPT (ParameterList)
ACCEPT (Pointer)
ACCEPT (PullPort)
ACCEPT (PushPort)
ACCEPT (PushPortCall)
ACCEPT (ReactionDecl)
ACCEPT (Receiver)
ACCEPT (Return)
ACCEPT (Select)
ACCEPT (Slice)
ACCEPT (StatementList)
ACCEPT (SubtractAssign)
ACCEPT (TopLevelDeclList)
ACCEPT (TypeDecl)
ACCEPT (TypeExpression)
ACCEPT (VarDecl)
ACCEPT (VariableList)
ACCEPT (While)

PRINT (ActionDecl)
PRINT (Activate)
PRINT (AddAssign)
PRINT (AddressOf)
PRINT (Array)
PRINT (Assign)
PRINT (BinderDecl)
PRINT (BindPullPort)
PRINT (BindPushPort)
PRINT (BindPushPortParameter)
PRINT (Call)
PRINT (Change)
PRINT (CompositeLiteral)
PRINT (ConstDecl)
PRINT (Conversion)
PRINT (Dereference)
PRINT (Element)
PRINT (ElementList)
PRINT (EmptyExpression)
PRINT (EmptyStatement)
PRINT (EmptyType)
PRINT (ExpressionList)
PRINT (ExpressionStatement)
PRINT (FieldList)
PRINT (ForIota)
PRINT (FunctionDecl)
PRINT (GetterDecl)
PRINT (Heap)
PRINT (IdentifierList)
PRINT (IdentifierType)
PRINT (If)
PRINT (IncrementDecrement)
PRINT (Index)
PRINT (IndexSlice)
PRINT (IndexedPushPortCall)
PRINT (InitializerDecl)
PRINT (InstanceDecl)
PRINT (Literal)
PRINT (Map)
PRINT (MethodDecl)
PRINT (ParameterList)
PRINT (Pointer)
PRINT (PullPort)
PRINT (PushPort)
PRINT (PushPortCall)
PRINT (ReactionDecl)
PRINT (Receiver)
PRINT (Return)
PRINT (Select)
PRINT (Slice)
PRINT (StatementList)
PRINT (SubtractAssign)
PRINT (TopLevelDeclList)
PRINT (TypeDecl)
PRINT (TypeExpression)
PRINT (VarDecl)
PRINT (VariableList)
PRINT (While)

Node::~Node() { }

  List::List (const source::Location& location) : Node (location) { }

void
List::visit_children (NodeVisitor& visitor)
{
  for (ConstIterator pos = begin (), limit = end (); pos != limit; ++pos)
    {
      (*pos)->accept (visitor);
    }
}

List*
List::append (Node * child)
{
  children_.push_back (child);
  return this;
}

size_t List::size () const
{
  return children_.size ();
}
size_t List::empty () const
{
  return children_.empty ();
}
Node* List::at (size_t idx) const
{
  return children_.at (idx);
}
List::ConstIterator List::begin () const
{
  return children_.begin ();
}
List::ConstIterator List::end () const
{
  return children_.end ();
}

  Node::Node (const source::Location& a_location)
  : location (a_location)
  , field (NULL)
  , reset_mutability (false)
  , callable (NULL)
  , operation (NULL)
{
  assert (location.line () != 0);
}

Literal::Literal (const source::Location& location, const ExpressionValue& v)
  : Node (location)
{
  eval = v;
}

  void Literal::visit_children (NodeVisitor& visitor)
  { }

IdentifierList::IdentifierList (const source::Location& location)
  : Node (location)
{ }

void
IdentifierList::visit_children (NodeVisitor& visitor)
{ }

IdentifierList*
IdentifierList::append (const Identifier& identifier)
{
  identifiers.push_back (identifier);
  return this;
}

size_t
IdentifierList::size () const
{
  return identifiers.size ();
}

IdentifierList::const_iterator
IdentifierList::begin () const
{
  return identifiers.begin ();
}

IdentifierList::const_iterator
IdentifierList::end () const
{
  return identifiers.end ();
}

Receiver::Receiver (const source::Location& location,
                    const Identifier& this_id,
                    Mutability m,
                    Mutability dm,
                    bool isP,
                    const Identifier& type_id)
  : Node (location)
  , identifier (this_id)
  , mutability (m)
  , indirection_mutability (dm)
  , is_pointer (isP)
  , type_identifier (type_id)
  , identifier_symbol (NULL)
  , type_identifier_symbol (NULL)
{ }

void
Receiver::visit_children (NodeVisitor& visitor)
{ }

Array::Array (const source::Location& location, Node* dim, Node* base)
  : Node (location)
  , dimension (dim)
  , base_type (base)
{ }

void Array::visit_children (NodeVisitor& visitor)
{
  dimension->accept (visitor);
  base_type->accept (visitor);
}

EmptyType::EmptyType (const source::Location& location)
  : Node (location)
{ }

void EmptyType::visit_children (NodeVisitor& visitor)
{ }

FieldList::FieldList (const source::Location& location)
  : List (location)
  , is_component (false)
{ }

Heap::Heap (const source::Location& location, Node* child)
  : Unary (location, child)
{ }

IdentifierType::IdentifierType (const source::Location& location, const Identifier& a_identifier)
  : Node (location)
  , identifier (a_identifier)
{ }

void
IdentifierType::visit_children (NodeVisitor& visitor)
{ }

VariableList::VariableList (const source::Location& location,
                            IdentifierList* id_list,
                            Mutability m,
                            Mutability dm,
                            Node* type_sp)
  : Node (location)
  , identifiers (id_list)
  , mutability (m)
  , indirection_mutability (dm)
  , type (type_sp)
{ }

void
VariableList::visit_children (NodeVisitor& visitor)
{
  identifiers->accept (visitor);
  type->accept (visitor);
}

Pointer::Pointer (const source::Location& location, Node* child)
  : Unary (location, child)
{ }

Slice::Slice (const source::Location& location, Node* child)
  : Unary (location, child)
{ }

Map::Map (const source::Location& location, Node* k, Node* v)
  : Node (location)
  , key (k)
  , value (v)
{ }

void
Map::visit_children (NodeVisitor& visitor)
{
  key->accept (visitor);
  value->accept (visitor);
}

PushPort::PushPort (const source::Location& location, ParameterList* sig)
  : Node (location)
  , parameters (sig)
{ }

void PushPort::visit_children (NodeVisitor& visitor)
{
  parameters->accept (visitor);
}

PullPort::PullPort (const source::Location& location,
                    ParameterList* pl,
                    ParameterList* rpl)
  : Node (location)
  , parameters (pl)
  , return_parameters (rpl)
{ }

void PullPort::visit_children (NodeVisitor& visitor)
{
  parameters->accept (visitor);
  return_parameters->accept (visitor);
}

ParameterList::ParameterList (const source::Location& location)
  : List (location)
{ }

TypeExpression::TypeExpression (const source::Location& location, Node* ts)
  : Unary<> (location, ts)
{ }

Binary::Binary (const source::Location& location, Node* l, Node* r)
  : Node (location)
  , left (l)
  , right (r)
{ }

void Binary::visit_children (NodeVisitor& visitor)
{
  left->accept (visitor);
  right->accept (visitor);
}

AddressOf::AddressOf (const source::Location& location, Node* child)
  : Unary (location, child)
{ }

Call::Call (const source::Location& location, Node* e, List* a)
  : Node (location)
  , expression (e)
  , arguments (a)
  , function_type (NULL)
  , push_port_type (NULL)
  , pull_port_type (NULL)
  , method_type (NULL)
  , initializer_type (NULL)
  , getter_type (NULL)
  , reaction_type (NULL)
  , parameters (NULL)
  , return_parameters (NULL)
{ }

void Call::visit_children (NodeVisitor& visitor)
{
  expression->accept (visitor);
  arguments->accept (visitor);
}

Conversion::Conversion (const source::Location& location, Node* te, Node* e)
  : Node (location)
  , type (te)
  , argument (e)
{ }
void Conversion::visit_children (NodeVisitor& visitor)
{
  type->accept (visitor);
  argument->accept (visitor);
}

Dereference::Dereference (const source::Location& location, Node* child)
  : Unary (location, child)
{ }

ExpressionList::ExpressionList (const source::Location& location)
  : List (location)
{ }

IdentifierExpression::IdentifierExpression (const source::Location& location, const Identifier& a_identifier)
  : Node (location)
  , identifier (a_identifier)
{ }

void
IdentifierExpression::visit_children (NodeVisitor& visitor)
{ }

void
IdentifierExpression::print (std::ostream& out) const
{
  out << "IdentifierExpression " << identifier.identifier ();
}


Index::Index (const source::Location& location, Node* b, Node* i)
  : Node (location)
  , base (b)
  , index (i)
  , array_type (NULL)
  , slice_type (NULL)
{ }

void Index::visit_children (NodeVisitor& visitor)
{
  base->accept (visitor);
  index->accept (visitor);
}

IndexSlice::IndexSlice (const source::Location& location,
                        Node* b,
                        Node* l,
                        Node* h,
                        Node* m)
  : Node (location)
  , base (b)
  , low (l)
  , low_present (node_cast<EmptyExpression> (low) == NULL)
  , high (h)
  , high_present (node_cast<EmptyExpression> (high) == NULL)
  , max (m)
  , max_present (node_cast<EmptyExpression> (max) == NULL)
  , string_type (NULL)
  , pointer_to_array_type (NULL)
  , array_type (NULL)
  , slice_type (NULL)
{ }

void IndexSlice::visit_children (NodeVisitor& visitor)
{
  base->accept (visitor);
  low->accept (visitor);
  high->accept (visitor);
  max->accept (visitor);
}

EmptyExpression::EmptyExpression (const source::Location& location)
  : Node (location)
{ }

  void EmptyExpression::visit_children (NodeVisitor& visitor)
  { }

PushPortCall::PushPortCall (const source::Location& location, const Identifier& id, List* a)
  : Node (location)
  , identifier (id)
  , arguments (a)
  , field (NULL)
  , push_port_type (NULL)
  , receiver_parameter (NULL)
{ }

void PushPortCall::visit_children (NodeVisitor& visitor)
{
  arguments->accept (visitor);
}

IndexedPushPortCall::IndexedPushPortCall (const source::Location& location,
                                          const Identifier& id,
    Node* idx,
    List* a)
  : Node (location)
  , identifier (id)
  , index (idx)
  , arguments (a)
  , field (NULL)
  , push_port_type (NULL)
  , receiver_parameter (NULL)
{ }

void IndexedPushPortCall::visit_children (NodeVisitor& visitor)
{
  index->accept (visitor);
  arguments->accept (visitor);
}

Select::Select (const source::Location& location, Node* b, const Identifier& id)
  : Node (location)
  , base (b)
  , identifier (id)
{ }

void Select::visit_children (NodeVisitor& visitor)
{
  base->accept (visitor);
}

EmptyStatement::EmptyStatement (const source::Location& location)
  : Node (location)
{ }

void EmptyStatement::visit_children (NodeVisitor& visitor)
{ }

AddAssign::AddAssign (const source::Location& location, Node* left, Node* right)
  : Binary (location, left, right)
{ }

Assign::Assign (const source::Location& location, Node* left, Node* right)
  : Binary (location, left, right)
{ }

Change::Change (const source::Location& location,
                Node * e,
                const Identifier& id,
                Node * b)
  : Node (location)
  , argument (e)
  , identifier (id)
  , body (b)
{ }

void Change::visit_children (NodeVisitor& visitor)
{
  argument->accept (visitor);
  body->accept (visitor);
}

ExpressionStatement::ExpressionStatement (const source::Location& location, Node* child)
  : Unary (location, child)
{ }

If::If (const source::Location& location,
        Node* s,
        Node* c,
        Node* tb,
        Node* fb)
  : Node (location)
  , before (s)
  , condition (c)
  , true_body (tb)
  , false_body (fb)
{ }

void If::visit_children (NodeVisitor& visitor)
{
  before->accept (visitor);
  condition->accept (visitor);
  true_body->accept (visitor);
  false_body->accept (visitor);
}

While::While (const source::Location& location, Node* c, Node* b)
  : Node (location)
  , condition (c)
  , body (b)
{ }

void While::visit_children (NodeVisitor& visitor)
{
  condition->accept (visitor);
  body->accept (visitor);
}

Return::Return (const source::Location& location, Node* child)
  : Unary (location, child)
  , return_symbol (NULL)
{ }

IncrementDecrement::IncrementDecrement (const source::Location& location, Node* child, Kind a_kind)
  : Unary (location, child)
  , kind (a_kind)
{ }

StatementList::StatementList (const source::Location& location)
  : List (location)
  , is_body (false)
{ }

SubtractAssign::SubtractAssign (const source::Location& location, Node* left, Node* right)
  : Binary (location, left, right)
{ }

Activate::Activate (const source::Location& location, List * el, Node * b)
  : Node (location)
  , arguments (el)
  , body (b)
  , memory_model (NULL)
  , in_action (false)
{ }

void Activate::visit_children (NodeVisitor& visitor)
{
  arguments->accept (visitor);
  body->accept (visitor);
}

VarDecl::VarDecl (const source::Location& location,
                  IdentifierList* il,
                  Mutability m,
                  Mutability dm,
                  Node* ts,
                  List* el)
  : Node (location)
  , identifiers (il)
  , mutability (m)
  , indirection_mutability (dm)
  , type (ts)
  , expressions (el)
{ }

void VarDecl::visit_children (NodeVisitor& visitor)
{
  identifiers->accept (visitor);
  type->accept (visitor);
  expressions->accept (visitor);
}

BindPushPort::BindPushPort (const source::Location& location, Node* left, Node* right)
  : Binary (location, left, right)
{ }

BindPushPortParameter::BindPushPortParameter (const source::Location& location,
    Node* l,
    Node* r,
    Node* p)
  : Node (location)
  , left (l)
  , right (r)
  , param (p)
{ }

void BindPushPortParameter::visit_children (NodeVisitor& visitor)
{
  left->accept (visitor);
  right->accept (visitor);
  param->accept (visitor);
}

BindPullPort::BindPullPort (const source::Location& location, Node* left, Node* right)
  : Binary (location, left, right)
{ }

ForIota::ForIota (const source::Location& location,
                  const Identifier& id,
                  Node* lim,
                  Node* b)
  : Node (location)
  , identifier (id)
  , limit (lim)
  , body (b)
{ }

void ForIota::visit_children (NodeVisitor& visitor)
{
  limit->accept (visitor);
  body->accept (visitor);
}

ActionDecl::ActionDecl (const source::Location& location,
                        Node* d,
                        Receiver* r,
                        const Identifier& i,
                        Node* p,
                        Node* b)
  : Node (location)
  , dimension (d)
  , receiver (r)
  , identifier (i)
  , precondition (p)
  , body (b)
  , action (NULL)
{ }

void ActionDecl::visit_children (NodeVisitor& visitor)
{
  dimension->accept (visitor);
  receiver->accept (visitor);
  precondition->accept (visitor);
  body->accept (visitor);
}

BinderDecl::BinderDecl (const source::Location& location,
                        Receiver* r,
                        const Identifier& i,
                        Node* b)
  : Node (location)
  , receiver (r)
  , identifier (i)
  , body (b)
  , bind (NULL)
{ }

void BinderDecl::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  body->accept (visitor);
}

FunctionDecl::FunctionDecl (const source::Location& location,
                            const Identifier& i,
                            ParameterList* pl,
                            ParameterList* rpl,
                            Node* b)
  : Node (location)
  , identifier (i)
  , parameters (pl)
  , return_parameters (rpl)
  , body (b)
  , symbol (NULL)
{ }

void FunctionDecl::visit_children (NodeVisitor& visitor)
{
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

InstanceDecl::InstanceDecl (const source::Location& location,
                            const Identifier& id,
                            Node* tn,
                            const Identifier& init,
                            List* el)
  : Node (location)
  , identifier (id)
  , type (tn)
  , initializer (init)
  , arguments (el)
{ }

void InstanceDecl::visit_children (NodeVisitor& visitor)
{
  type->accept (visitor);
  arguments->accept (visitor);
}

ConstDecl::ConstDecl (const source::Location& location,
                      IdentifierList* il,
                      Node* ts,
                      List* el)
  : Node (location)
  , identifiers (il)
  , type (ts)
  , expressions (el)
{ }

void ConstDecl::visit_children (NodeVisitor& visitor)
{
  identifiers->accept (visitor);
  type->accept (visitor);
  expressions->accept (visitor);
}

MethodDecl::MethodDecl (const source::Location& location,
                        Receiver* r,
                        const Identifier& i,
                        ParameterList* a_parameter_list,
                        ParameterList* a_return_parameter_list,
                        Node* b)
  : Node (location)
  , receiver (r)
  , identifier (i)
  , parameters (a_parameter_list)
  , return_parameters (a_return_parameter_list)
  , body (b)
  , method (NULL)
{ }

void MethodDecl::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

GetterDecl::GetterDecl (const source::Location& location,
                        Receiver* r,
                        const Identifier& i,
                        ParameterList* pl,
                        ParameterList* rpl,
                        Node* b)
  : Node (location)
  , receiver (r)
  , identifier (i)
  , parameters (pl)
  , return_parameters (rpl)
  , body (b)
  , getter (NULL)
{ }

void GetterDecl::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

InitializerDecl::InitializerDecl (const source::Location& location,
                    Receiver* r,
                                  const Identifier& i,
                    ParameterList* pl,
                    ParameterList* rpl,
                    Node* b)
  : Node (location)
  , receiver (r)
  , identifier (i)
  , parameters (pl)
  , return_parameters (rpl)
  , body (b)
  , initializer (NULL)
{ }

void InitializerDecl::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

ReactionDecl::ReactionDecl (const source::Location& location,
                            Node* d,
                            Receiver* r,
                            const Identifier& i,
                            ParameterList* pl,
                            Node* b)
  : Node (location)
  , dimension (d)
  , receiver (r)
  , identifier (i)
  , parameters (pl)
  , return_parameters (new ParameterList (location))
  , body (b)
  , reaction (NULL)
{ }

void ReactionDecl::visit_children (NodeVisitor& visitor)
{
  dimension->accept (visitor);
  receiver->accept (visitor);
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

TypeDecl::TypeDecl (const source::Location& location, const Identifier& i, Node* ts)
  : Node (location)
  , identifier (i)
  , type (ts)
  , symbol (NULL)
{ }

void TypeDecl::visit_children (NodeVisitor& visitor)
{
  type->accept (visitor);
}

ElementList::ElementList (const source::Location& location)
  : List (location)
{ }

Element::Element (const source::Location& location, Node* k, Node* v)
  : Node (location)
  , key (k)
  , value (v)
{ }

void Element::visit_children (NodeVisitor& visitor)
{
  key->accept (visitor);
  value->accept (visitor);
}

CompositeLiteral::CompositeLiteral (const source::Location& location,
                                    Node* lt,
                                    List* lv)
  : Node (location)
  , type (lt)
  , value (lv)
{ }

void CompositeLiteral::visit_children (NodeVisitor& visitor)
{
  type->accept (visitor);
  value->accept (visitor);
}

  TopLevelDeclList::TopLevelDeclList (const source::Location& location)
    : List (location)
  { }

Call* make_unary (const source::Location& location, decl::PolymorphicFunction* func, Node* child)
{
  Literal* e = new Literal (location, ExpressionValue::make_polymorphic_function (func));
  ExpressionList* el = new ExpressionList (location);
  el->append (child);
  return new Call (location, e, el);
}

Call* make_binary (const source::Location& location, decl::PolymorphicFunction* func, Node* left, Node* right)
{
  Literal* e = new Literal (location, ExpressionValue::make_polymorphic_function (func));
  ExpressionList* el = new ExpressionList (location);
  el->append (left);
  el->append (right);
  return new Call (location, e, el);
}

}
