#include "node.hpp"

#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "type.hpp"

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

void Identifier::print (std::ostream& out) const
{
  out << "Identifier " << identifier;
}
void IdentifierList::print (std::ostream& out) const
{
  out << "IdentifierList";
}
void Receiver::print (std::ostream& out) const
{
  out << "Receiver";
}
void Array::print (std::ostream& out) const
{
  out << "Array";
}
void Slice::print (std::ostream& out) const
{
  out << "Slice";
}
void Map::print (std::ostream& out) const
{
  out << "Map";
}
void EmptyType::print (std::ostream& out) const
{
  out << "EmptyType";
}
void FieldList::print (std::ostream& out) const
{
  out << "FieldList";
}
void Heap::print (std::ostream& out) const
{
  out << "Heap";
}
void VariableList::print (std::ostream& out) const
{
  out << "VariableList";
}
void IdentifierType::print (std::ostream& out) const
{
  out << "IdentifierType";
}
void Pointer::print (std::ostream& out) const
{
  out << "Pointer";
}
void PushPort::print (std::ostream& out) const
{
  out << "PushPort";
}
void PullPort::print (std::ostream& out) const
{
  out << "PullPort";
}
void ParameterList::print (std::ostream& out) const
{
  out << "ParameterList";
}
void TypeExpression::print (std::ostream& out) const
{
  out << "TypeExpression";
}
void BinaryArithmetic::print (std::ostream& out) const
{
  out << "BinaryArithmetic";
}
void AddressOf::print (std::ostream& out) const
{
  out << "AddressOf";
}
void Call::print (std::ostream& out) const
{
  out << "Call";
}
void Conversion::print (std::ostream& out) const
{
  out << "Conversion";
}
void Dereference::print (std::ostream& out) const
{
  out << "Dereference";
}
void IdentifierExpression::print (std::ostream& out) const
{
  out << "IdentifierExpression";
}
void Index::print (std::ostream& out) const
{
  out << "Index";
}
void IndexSlice::print (std::ostream& out) const
{
  out << "IndexSlice";
}
void EmptyExpression::print (std::ostream& out) const
{
  out << "EmptyExpression";
}
void IndexedPushPortCall::print (std::ostream& out) const
{
  out << "IndexedPushPortCall";
}
void ExpressionList::print (std::ostream& out) const
{
  out << "ExpressionList";
}
void Literal::print (std::ostream& out) const
{
  out << "Literal";
}
void UnaryArithmetic::print (std::ostream& out) const
{
  out << "UnaryArithmetic";
}
void PushPortCall::print (std::ostream& out) const
{
  out << "PushPortCall";
}
void Select::print (std::ostream& out) const
{
  out << "Select";
}
void EmptyStatement::print (std::ostream& out) const
{
  out << "EmptyStatement";
}
void AddAssign::print (std::ostream& out) const
{
  out << "AddAssign";
}
void Change::print (std::ostream& out) const
{
  out << "Change";
}
void Assign::print (std::ostream& out) const
{
  out << "Assign";
}
void ExpressionStatement::print (std::ostream& out) const
{
  out << "ExpressionStatement";
}
void If::print (std::ostream& out) const
{
  out << "If";
}
void While::print (std::ostream& out) const
{
  out << "While";
}
void StatementList::print (std::ostream& out) const
{
  out << "StatementList";
}
void Return::print (std::ostream& out) const
{
  out << "Return";
}
void IncrementDecrement::print (std::ostream& out) const
{
  out << "IncrementDecrement";
}
void SubtractAssign::print (std::ostream& out) const
{
  out << "SubtractAssign";
}
void Activate::print (std::ostream& out) const
{
  out << "Activate";
}
void Var::print (std::ostream& out) const
{
  out << "Var";
}
void BindPushPort::print (std::ostream& out) const
{
  out << "BindPushPort";
}
void BindPushPortParameter::print (std::ostream& out) const
{
  out << "BindPushPortParameter";
}
void BindPullPort::print (std::ostream& out) const
{
  out << "BindPullPort";
}
void ForIota::print (std::ostream& out) const
{
  out << "ForIota";
}
void Action::print (std::ostream& out) const
{
  out << "Action";
}
void Const::print (std::ostream& out) const
{
  out << "Const";
}
void DimensionedAction::print (std::ostream& out) const
{
  out << "DimensionedAction";
}
void Bind::print (std::ostream& out) const
{
  out << "Bind";
}
void Function::print (std::ostream& out) const
{
  out << "Function";
}
void Getter::print (std::ostream& out) const
{
  out << "Getter";
}
void Initializer::print (std::ostream& out) const
{
  out << "Initializer";
}
void Instance::print (std::ostream& out) const
{
  out << "Instance";
}
void Method::print (std::ostream& out) const
{
  out << "Method";
}
void Reaction::print (std::ostream& out) const
{
  out << "Reaction";
}
void DimensionedReaction::print (std::ostream& out) const
{
  out << "DimensionedReaction";
}
void Type::print (std::ostream& out) const
{
  out << "Type";
}
void SourceFile::print (std::ostream& out) const
{
  out << "SourceFile";
}
void ElementList::print (std::ostream& out) const
{
  out << "ElementList";
}
void Element::print (std::ostream& out) const
{
  out << "Element";
}
void CompositeLiteral::print (std::ostream& out) const
{
  out << "CompositeLiteral";
}
void ImportDeclList::print (std::ostream& out) const
{
  out << "ImportDeclList";
}

void TopLevelDeclList::print (std::ostream& out) const
{
  out << "TopLevelDeclList";
}

#define ACCEPT(type) void                               \
  type::accept (NodeVisitor& visitor)                 \
  {                                                     \
    visitor.visit (*this);                              \
  }

ACCEPT (Action)
ACCEPT (Activate)
ACCEPT (AddAssign)
ACCEPT (AddressOf)
ACCEPT (Array)
ACCEPT (Assign)
ACCEPT (BinaryArithmetic)
ACCEPT (Bind)
ACCEPT (BindPullPort)
ACCEPT (BindPushPort)
ACCEPT (BindPushPortParameter)
ACCEPT (Call)
ACCEPT (Change)
ACCEPT (CompositeLiteral)
ACCEPT (Const)
ACCEPT (Conversion)
ACCEPT (Dereference)
ACCEPT (DimensionedAction)
ACCEPT (DimensionedReaction)
ACCEPT (Element)
ACCEPT (ElementList)
ACCEPT (EmptyExpression)
ACCEPT (EmptyStatement)
ACCEPT (EmptyType)
ACCEPT (ExpressionList)
ACCEPT (ExpressionStatement)
ACCEPT (FieldList)
ACCEPT (ForIota)
ACCEPT (Function)
ACCEPT (Getter)
ACCEPT (Heap)
ACCEPT (Identifier)
ACCEPT (IdentifierExpression)
ACCEPT (IdentifierList)
ACCEPT (IdentifierType)
ACCEPT (If)
ACCEPT (IncrementDecrement)
ACCEPT (Index)
ACCEPT (IndexSlice)
ACCEPT (IndexedPushPortCall)
ACCEPT (Initializer)
ACCEPT (Instance)
ACCEPT (Literal)
ACCEPT (Map)
ACCEPT (Method)
ACCEPT (ParameterList)
ACCEPT (Pointer)
ACCEPT (PullPort)
ACCEPT (PushPort)
ACCEPT (PushPortCall)
ACCEPT (Reaction)
ACCEPT (Receiver)
ACCEPT (Return)
ACCEPT (Select)
ACCEPT (Slice)
ACCEPT (SourceFile)
ACCEPT (StatementList)
ACCEPT (SubtractAssign)
ACCEPT (Type)
ACCEPT (TypeExpression)
ACCEPT (UnaryArithmetic)
ACCEPT (Var)
ACCEPT (VariableList)
ACCEPT (While)
ACCEPT (ImportDeclList)
ACCEPT (TopLevelDeclList)

Node::~Node() { }

List::List (unsigned int line) : Node (line) { }

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

Node::Node (unsigned int line_)
  : location (line_)
  , field (NULL)
  , reset_mutability (false)
  , callable (NULL)
  , polymorphic_function (NULL)
  , operation (NULL)
{
  assert (location.line != 0);
}

Literal::Literal (unsigned int line, const ::type::Type* t, const Value& v)
  : Node (line)
{
  eval.type = t;
  eval.value = v;
}

Identifier::Identifier (unsigned int line, const std::string& id)
  : Node (line)
  , identifier (id)
{ }

void Identifier::visit_children (NodeVisitor& visitor) { }

IdentifierList::IdentifierList (unsigned int line)
  : List (line)
{ }

Receiver::Receiver (unsigned int line,
                    Identifier* this_id,
                    Mutability m,
                    Mutability dm,
                    bool isP,
                    Identifier* type_id)
  : Node (line)
  , identifier (this_id)
  , mutability (m)
  , indirection_mutability (dm)
  , is_pointer (isP)
  , type (type_id)
{ }

void
Receiver::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  type->accept (visitor);
}

Array::Array (unsigned int line, Node* dim, Node* base)
  : Node (line)
  , dimension (dim)
  , base_type (base)
{ }

void Array::visit_children (NodeVisitor& visitor)
{
  dimension->accept (visitor);
  base_type->accept (visitor);
}

EmptyType::EmptyType (unsigned int line)
  : Node (line)
{ }

FieldList::FieldList (unsigned int line)
  : List (line)
  , is_component (false)
{ }

Heap::Heap (unsigned int line, Node* child)
  : Unary (line, child)
{ }

IdentifierType::IdentifierType (unsigned int line, Identifier* child)
  : Unary (line, child)
{ }

VariableList::VariableList (unsigned int line,
                            List* id_list,
                            Mutability m,
                            Mutability dm,
                            Node* type_sp)
  : Node (line)
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

Pointer::Pointer (unsigned int line, Node* child)
  : Unary (line, child)
{ }

Slice::Slice (unsigned int line, Node* child)
  : Unary (line, child)
{ }

Map::Map (unsigned int line, Node* k, Node* v)
  : Node (line)
  , key (k)
  , value (v)
{ }

void
Map::visit_children (NodeVisitor& visitor)
{
  key->accept (visitor);
  value->accept (visitor);
}

PushPort::PushPort (unsigned int line, List* sig)
  : Node (line)
  , parameters (sig)
{ }

void PushPort::visit_children (NodeVisitor& visitor)
{
  parameters->accept (visitor);
}

PullPort::PullPort (unsigned int line,
                    List* pl,
                    List* rpl)
  : Node (line)
  , parameters (pl)
  , return_parameters (rpl)
{ }

void PullPort::visit_children (NodeVisitor& visitor)
{
  parameters->accept (visitor);
  return_parameters->accept (visitor);
}

ParameterList::ParameterList (unsigned int line)
  : List (line)
{ }

TypeExpression::TypeExpression (unsigned int line, Node* ts)
  : Unary<> (line, ts)
{ }

Binary::Binary (unsigned int line, Node* l, Node* r)
  : Node (line)
  , left (l)
  , right (r)
{ }

void Binary::visit_children (NodeVisitor& visitor)
{
  left->accept (visitor);
  right->accept (visitor);
}

BinaryArithmetic::BinaryArithmetic (unsigned int line, decl::PolymorphicFunction* a_temp, Node* left, Node* right)
  : Binary (line, left, right)
{
  polymorphic_function = a_temp;
}

AddressOf::AddressOf (unsigned int line, Node* child)
  : Unary (line, child)
{ }

Call::Call (unsigned int line, Node* e, List* a)
  : Node (line)
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

Conversion::Conversion (unsigned int line, Node* te, Node* e)
  : Node (line)
  , type (te)
  , argument (e)
{ }
void Conversion::visit_children (NodeVisitor& visitor)
{
  type->accept (visitor);
  argument->accept (visitor);
}

Dereference::Dereference (unsigned int line, Node* child)
  : Unary (line, child)
{ }

ExpressionList::ExpressionList (unsigned int line)
  : List (line)
{ }

IdentifierExpression::IdentifierExpression (unsigned int line, Identifier* c)
  : Unary (line, c)
{ }

Index::Index (unsigned int line, Node* b, Node* i)
  : Node (line)
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

IndexSlice::IndexSlice (unsigned int line,
                        Node* b,
                        Node* l,
                        Node* h,
                        Node* m)
  : Node (line)
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

EmptyExpression::EmptyExpression (unsigned int line)
  : Node (line)
{ }

UnaryArithmetic::UnaryArithmetic (unsigned int line, decl::PolymorphicFunction* a_temp, Node* child)
  : Unary (line, child)
{
  polymorphic_function = a_temp;
}

PushPortCall::PushPortCall (unsigned int line, Identifier* id, List* a)
  : Node (line)
  , identifier (id)
  , arguments (a)
  , field (NULL)
  , push_port_type (NULL)
  , receiver_parameter (NULL)
{ }

void PushPortCall::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  arguments->accept (visitor);
}

IndexedPushPortCall::IndexedPushPortCall (unsigned int line,
    Identifier* id,
    Node* idx,
    List* a)
  : Node (line)
  , identifier (id)
  , index (idx)
  , arguments (a)
  , field (NULL)
  , push_port_type (NULL)
  , receiver_parameter (NULL)
{ }

void IndexedPushPortCall::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  index->accept (visitor);
  arguments->accept (visitor);
}

Select::Select (unsigned int line, Node* b, Identifier* id)
  : Node (line)
  , base (b)
  , identifier (id)
{ }

void Select::visit_children (NodeVisitor& visitor)
{
  base->accept (visitor);
  identifier->accept (visitor);
}

EmptyStatement::EmptyStatement (unsigned int line)
  : Node (line)
{ }

AddAssign::AddAssign (unsigned int line, Node* left, Node* right)
  : Binary (line, left, right)
{ }

Assign::Assign (unsigned int line, Node* left, Node* right)
  : Binary (line, left, right)
{ }

Change::Change (unsigned int line,
                Node * e,
                Identifier * id,
                Node * b)
  : Node (line)
  , argument (e)
  , identifier (id)
  , body (b)
{ }

void Change::visit_children (NodeVisitor& visitor)
{
  argument->accept (visitor);
  identifier->accept (visitor);
  body->accept (visitor);
}

ExpressionStatement::ExpressionStatement (unsigned int line, Node* child)
  : Unary (line, child)
{ }

If::If (unsigned int line,
        Node* s,
        Node* c,
        Node* tb,
        Node* fb)
  : Node (line)
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

While::While (unsigned int line, Node* c, Node* b)
  : Node (line)
  , condition (c)
  , body (b)
{ }

void While::visit_children (NodeVisitor& visitor)
{
  condition->accept (visitor);
  body->accept (visitor);
}

Return::Return (unsigned int line, Node* child)
  : Unary (line, child)
  , return_symbol (NULL)
{ }

IncrementDecrement::IncrementDecrement (unsigned int line, Node* child, Kind a_kind)
  : Unary (line, child)
  , kind (a_kind)
{ }

StatementList::StatementList (unsigned int line)
  : List (line)
{ }

SubtractAssign::SubtractAssign (unsigned int line, Node* left, Node* right)
  : Binary (line, left, right)
{ }

Activate::Activate (unsigned int line, List * el, Node * b)
  : Node (line)
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

Var::Var (unsigned int line,
          List* il,
          Mutability m,
          Mutability dm,
          Node* ts,
          List* el)
  : Node (line)
  , identifiers (il)
  , mutability (m)
  , indirection_mutability (dm)
  , type (ts)
  , expressions (el)
{ }

void Var::visit_children (NodeVisitor& visitor)
{
  identifiers->accept (visitor);
  type->accept (visitor);
  expressions->accept (visitor);
}

BindPushPort::BindPushPort (unsigned int line, Node* left, Node* right)
  : Binary (line, left, right)
{ }

BindPushPortParameter::BindPushPortParameter (unsigned int line,
    Node* l,
    Node* r,
    Node* p)
  : Node (line)
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

BindPullPort::BindPullPort (unsigned int line, Node* left, Node* right)
  : Binary (line, left, right)
{ }

ForIota::ForIota (unsigned int line,
                  Identifier* id,
                  Node* lim,
                  Node* b)
  : Node (line)
  , identifier (id)
  , limit (lim)
  , body (b)
{ }

void ForIota::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  limit->accept (visitor);
  body->accept (visitor);
}

Action::Action (unsigned int line,
                Node* r,
                Identifier* i,
                Node* p,
                Node* b)
  : Node (line)
  , receiver (r)
  , identifier (i)
  , precondition (p)
  , body (b)
  , action (NULL)
  , type (NULL)
{ }

void Action::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  identifier->accept (visitor);
  precondition->accept (visitor);
  body->accept (visitor);
}

DimensionedAction::DimensionedAction (unsigned int line,
                                      Node* d,
                                      Node* r,
                                      Identifier* i,
                                      Node* p,
                                      Node* b)
  : Node (line)
  , dimension (d)
  , receiver (r)
  , identifier (i)
  , precondition (p)
  , body (b)
  , action (NULL)
  , type (NULL)
{ }

void DimensionedAction::visit_children (NodeVisitor& visitor)
{
  dimension->accept (visitor);
  receiver->accept (visitor);
  identifier->accept (visitor);
  precondition->accept (visitor);
  body->accept (visitor);
}

Bind::Bind (unsigned int line,
            Node* r,
            Identifier* i,
            Node* b)
  : Node (line)
  , receiver (r)
  , identifier (i)
  , body (b)
  , bind (NULL)
{ }

void Bind::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  identifier->accept (visitor);
  body->accept (visitor);
}

Function::Function (unsigned int line,
                    Identifier* i,
                    List* pl,
                    List* rpl,
                    Node* b)
  : Node (line)
  , identifier (i)
  , parameters (pl)
  , return_parameters (rpl)
  , body (b)
  , function (NULL)
{ }

void Function::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

Instance::Instance (unsigned int line,
                    Identifier* id,
                    Node* tn,
                    Identifier* init,
                    List* el)
  : Node (line)
  , identifier (id)
  , type (tn)
  , initializer (init)
  , arguments (el)
{ }

void Instance::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  type->accept (visitor);
  initializer->accept (visitor);
  arguments->accept (visitor);
}

Const::Const (unsigned int line,
              List* il,
              Node* ts,
              List* el)
  : Node (line)
  , identifiers (il)
  , type (ts)
  , expressions (el)
  , done (false)
{ }

void Const::visit_children (NodeVisitor& visitor)
{
  identifiers->accept (visitor);
  type->accept (visitor);
  expressions->accept (visitor);
}

Method::Method (unsigned int line,
                Node * r,
                Identifier * i,
                List* a_parameter_list,
                List* a_return_parameter_list,
                Node* b)
  : Node (line)
  , receiver (r)
  , identifier (i)
  , parameters (a_parameter_list)
  , return_parameters (a_return_parameter_list)
  , body (b)
  , method (NULL)
{ }

void Method::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  identifier->accept (visitor);
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

Getter::Getter (unsigned int line,
                Node * r,
                Identifier * i,
                List* pl,
                List* rpl,
                Node* b)
  : Node (line)
  , receiver (r)
  , identifier (i)
  , parameters (pl)
  , return_parameters (rpl)
  , body (b)
  , getter (NULL)
{ }

void Getter::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  identifier->accept (visitor);
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

Initializer::Initializer (unsigned int line,
                          Node* r,
                          Identifier * i,
                          List* pl,
                          List* rpl,
                          Node* b)
  : Node (line)
  , receiver (r)
  , identifier (i)
  , parameters (pl)
  , return_parameters (rpl)
  , body (b)
  , initializer (NULL)
{ }

void Initializer::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  identifier->accept (visitor);
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

Reaction::Reaction (unsigned int line,
                    Node* r,
                    Identifier* i,
                    List* pl,
                    Node* b)
  : Node (line)
  , receiver (r)
  , identifier (i)
  , parameters (pl)
  , return_parameters (new ParameterList (line))
  , body (b)
  , reaction (NULL)
{ }

void Reaction::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  identifier->accept (visitor);
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

DimensionedReaction::DimensionedReaction (unsigned int line,
    Node* d,
    Node* r,
    Identifier* i,
    List* pl,
    Node* b)
  : Node (line)
  , dimension (d)
  , receiver (r)
  , identifier (i)
  , parameters (pl)
  , return_parameters (new ParameterList (line))
  , body (b)
  , reaction (NULL)
{ }

void DimensionedReaction::visit_children (NodeVisitor& visitor)
{
  dimension->accept (visitor);
  receiver->accept (visitor);
  identifier->accept (visitor);
  parameters->accept (visitor);
  return_parameters->accept (visitor);
  body->accept (visitor);
}

Type::Type (unsigned int line, Identifier* i, Node* ts)
  : Node (line)
  , identifier (i)
  , type (ts)
{ }

void Type::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  type->accept (visitor);
}

SourceFile::SourceFile (unsigned int line, Identifier* a_package,
                        List* a_import_decl_list, List* a_top_level_decl_list)
  : Node (line)
  , package (a_package)
  , import_decl_list (a_import_decl_list)
  , top_level_decl_list (a_top_level_decl_list)
{ }

void SourceFile::visit_children (NodeVisitor& visitor)
{
  package->accept (visitor);
  import_decl_list->accept (visitor);
  top_level_decl_list->accept (visitor);
}

ElementList::ElementList (unsigned int line)
  : List (line)
{ }

Element::Element (unsigned int line, Node* k, Node* v)
  : Node (line)
  , key (k)
  , value (v)
{ }

void Element::visit_children (NodeVisitor& visitor)
{
  key->accept (visitor);
  value->accept (visitor);
}

CompositeLiteral::CompositeLiteral (unsigned int line,
                                    Node* lt,
                                    List* lv)
  : Node (line)
  , type (lt)
  , value (lv)
{ }

void CompositeLiteral::visit_children (NodeVisitor& visitor)
{
  type->accept (visitor);
  value->accept (visitor);
}

void EmptyType::visit_children (NodeVisitor& visitor) { }
void EmptyExpression::visit_children (NodeVisitor& visitor) { }
void Literal::visit_children (NodeVisitor& visitor) { }
void EmptyStatement::visit_children (NodeVisitor& visitor) { }

ImportDeclList::ImportDeclList (unsigned int line)
  : List (line)
{ }

TopLevelDeclList::TopLevelDeclList (unsigned int line)
  : List (line)
{ }

}
