#include "node.hpp"

#include "node_visitor.hpp"
#include "node_cast.hpp"
#include "type.hpp"

namespace ast
{
using namespace decl;
using namespace semantic;

std::ostream&
operator<< (std::ostream& out, Node& node)
{
  struct visitor : public NodeVisitor
  {
    std::ostream& out;
    size_t indent;

    visitor (std::ostream& o) : out (o), indent (0) { }

    void print_indent (const Node& node)
    {
      for (size_t idx = 0; idx != indent; ++idx)
        {
          out << ' ';
        }
      out << node.location.line << ' ';
    }

    void print_children (Node& node)
    {
      size_t old_indent = indent;
      indent += 2;
      node.visit_children (*this);
      indent = old_indent;
    }

    void print_common (const Node& node)
    {
      if (node.eval.type != NULL)
        {
          out << ' ' << *node.eval.type;
          if (node.eval.value.present)
            {
              out << ' ' << ValuePrinter (node.eval.type, node.eval.value);
            }
          out << ' ';
          switch (node.eval.expression_kind)
            {
            case UnknownExpressionKind:
              out << "unknown";
              break;
            case ErrorExpressionKind:
              out << "error";
              break;
            case ValueExpressionKind:
              out << "value";
              break;
            case VariableExpressionKind:
              out << "variable";
              break;
            case VoidExpressionKind:
              out << "void";
              break;
            case TypeExpressionKind:
              out << "type";
              break;
            }
          out << ' ';
          switch (node.eval.intrinsic_mutability)
            {
            case Mutable:
              break;
            case Immutable:
              out << "const";
              break;
            case Foreign:
              out << "foreign";
              break;
            }
          out << ' ';
          switch (node.eval.indirection_mutability)
            {
            case Mutable:
              break;
            case Immutable:
              out << "+const";
              break;
            case Foreign:
              out << "+foreign";
              break;
            }
        }
      out << " receiver_state=" << node.eval.receiver_state;
      out << " receiver_access=";
      switch (node.eval.receiver_access)
        {
        case AccessNone:
          out << "none";
          break;
        case AccessRead:
          out << "read";
          break;
        case AccessWrite:
          out << "write";
          break;
        }
      out << '\n';
    }

    void visit (Identifier& node)
    {
      print_indent (node);
      out << "identifier " << node.identifier;
      print_common (node);
      print_children (node);
    }
    void visit (IdentifierList& node)
    {
      print_indent (node);
      out << "identifier_list";
      print_common (node);
      print_children (node);
    }
    void visit (Receiver& node)
    {
      print_indent (node);
      out << "receiver";
      print_common (node);
      print_children (node);
    }
    void visit (ArrayTypeSpec& node)
    {
      print_indent (node);
      out << "array_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (SliceTypeSpec& node)
    {
      print_indent (node);
      out << "slice_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (MapTypeSpec& node)
    {
      print_indent (node);
      out << "map_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (EmptyTypeSpec& node)
    {
      print_indent (node);
      out << "empty_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (FieldListTypeSpec& node)
    {
      print_indent (node);
      out << "field_list_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (HeapTypeSpec& node)
    {
      print_indent (node);
      out << "heap_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (IdentifierListTypeSpec& node)
    {
      print_indent (node);
      out << "identifier_list_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (IdentifierTypeSpec& node)
    {
      print_indent (node);
      out << "identifier_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (PointerTypeSpec& node)
    {
      print_indent (node);
      out << "pointer_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (PushPortTypeSpec& node)
    {
      print_indent (node);
      out << "push_port_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (PullPortTypeSpec& node)
    {
      print_indent (node);
      out << "pull_port_type_spec";
      print_common (node);
      print_children (node);
    }
    void visit (ParameterList& node)
    {
      print_indent (node);
      out << "parameter_list";
      print_common (node);
      print_children (node);
    }
    void visit (TypeExpression& node)
    {
      print_indent (node);
      out << "TypeExpression";
      print_common (node);
      print_children (node);
    }
    void visit (BinaryArithmeticExpr& node)
    {
      print_indent (node);
      out << "binary_arithmetic_expr";
      print_common (node);
      print_children (node);
    }
    void visit (AddressOfExpr& node)
    {
      print_indent (node);
      out << "address_of_expr";
      print_common (node);
      print_children (node);
    }
    void visit (CallExpr& node)
    {
      print_indent (node);
      out << "call_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ConversionExpr& node)
    {
      print_indent (node);
      out << "conversion_expr";
      print_common (node);
      print_children (node);
    }
    void visit (DereferenceExpr& node)
    {
      print_indent (node);
      out << "dereference_expr";
      print_common (node);
      print_children (node);
    }
    void visit (IdentifierExpr& node)
    {
      print_indent (node);
      out << "identifier_expr";
      print_common (node);
      print_children (node);
    }
    void visit (IndexExpr& node)
    {
      print_indent (node);
      out << "index_expr";
      print_common (node);
      print_children (node);
    }
    void visit (SliceExpr& node)
    {
      print_indent (node);
      out << "slice_expr";
      print_common (node);
      print_children (node);
    }
    void visit (EmptyExpr& node)
    {
      print_indent (node);
      out << "EmptyExpr";
      print_common (node);
      print_children (node);
    }
    void visit (IndexedPushPortCallExpr& node)
    {
      print_indent (node);
      out << "indexed_port_call_expr";
      print_common (node);
      print_children (node);
    }
    void visit (ListExpr& node)
    {
      print_indent (node);
      out << "list_expr";
      print_common (node);
      print_children (node);
    }
    void visit (LiteralExpr& node)
    {
      print_indent (node);
      out << "literal_expr";
      print_common (node);
      print_children (node);
    }
    void visit (UnaryArithmeticExpr& node)
    {
      print_indent (node);
      out << "unary_arithmetic_expr";
      print_common (node);
      print_children (node);
    }
    void visit (PushPortCallExpr& node)
    {
      print_indent (node);
      out << "push_port_call_expr";
      print_common (node);
      print_children (node);
    }
    void visit (SelectExpr& node)
    {
      print_indent (node);
      out << "select_expr";
      print_common (node);
      print_children (node);
    }
    void visit (EmptyStatement& node)
    {
      print_indent (node);
      out << "empty_statement";
      print_common (node);
      print_children (node);
    }
    void visit (AddAssignStatement& node)
    {
      print_indent (node);
      out << "add_assign_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ChangeStatement& node)
    {
      print_indent (node);
      out << "change_statement";
      print_common (node);
      print_children (node);
    }
    void visit (AssignStatement& node)
    {
      print_indent (node);
      out << "assign_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ExpressionStatement& node)
    {
      print_indent (node);
      out << "expression_statement";
      print_common (node);
      print_children (node);
    }
    void visit (IfStatement& node)
    {
      print_indent (node);
      out << "if_statement";
      print_common (node);
      print_children (node);
    }
    void visit (WhileStatement& node)
    {
      print_indent (node);
      out << "while_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ListStatement& node)
    {
      print_indent (node);
      out << "list_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ReturnStatement& node)
    {
      print_indent (node);
      out << "return_statement";
      print_common (node);
      print_children (node);
    }
    void visit (IncrementDecrementStatement& node)
    {
      print_indent (node);
      out << "increment_decrement_statement";
      print_common (node);
      print_children (node);
    }
    void visit (SubtractAssignStatement& node)
    {
      print_indent (node);
      out << "subtract_assign_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ActivateStatement& node)
    {
      print_indent (node);
      out << "activate_statement";
      print_common (node);
      print_children (node);
    }
    void visit (VarStatement& node)
    {
      print_indent (node);
      out << "var_statement";
      print_common (node);
      print_children (node);
    }
    void visit (BindPushPortStatement& node)
    {
      print_indent (node);
      out << "bind_push_port_statement";
      print_common (node);
      print_children (node);
    }
    void visit (BindPushPortParamStatement& node)
    {
      print_indent (node);
      out << "bind_push_port_param_statement";
      print_common (node);
      print_children (node);
    }
    void visit (BindPullPortStatement& node)
    {
      print_indent (node);
      out << "bind_pull_port_statement";
      print_common (node);
      print_children (node);
    }
    void visit (ForIotaStatement& node)
    {
      print_indent (node);
      out << "for_iota_statement";
      print_common (node);
      print_children (node);
    }
    void visit (Action& node)
    {
      print_indent (node);
      out << "action";
      print_common (node);
      print_children (node);
    }
    void visit (Const& node)
    {
      print_indent (node);
      out << "const";
      print_common (node);
      print_children (node);
    }
    void visit (DimensionedAction& node)
    {
      print_indent (node);
      out << "dimensioned_action";
      print_common (node);
      print_children (node);
    }
    void visit (Bind& node)
    {
      print_indent (node);
      out << "bind";
      print_common (node);
      print_children (node);
    }
    void visit (Function& node)
    {
      print_indent (node);
      out << "function";
      print_common (node);
      print_children (node);
    }
    void visit (Getter& node)
    {
      print_indent (node);
      out << "getter";
      print_common (node);
      print_children (node);
    }
    void visit (Initializer& node)
    {
      print_indent (node);
      out << "initializer";
      print_common (node);
      print_children (node);
    }
    void visit (Instance& node)
    {
      print_indent (node);
      out << "instance";
      print_common (node);
      print_children (node);
    }
    void visit (Method& node)
    {
      print_indent (node);
      out << "method";
      print_common (node);
      print_children (node);
    }
    void visit (Reaction& node)
    {
      print_indent (node);
      out << "reaction";
      print_common (node);
      print_children (node);
    }
    void visit (DimensionedReaction& node)
    {
      print_indent (node);
      out << "dimensioned_reaction";
      print_common (node);
      print_children (node);
    }
    void visit (Type& node)
    {
      print_indent (node);
      out << "Type";
      print_common (node);
      print_children (node);
    }
    void visit (SourceFile& node)
    {
      print_indent (node);
      out << "SourceFile";
      print_common (node);
      print_children (node);
    }
    void visit (ElementList& node)
    {
      print_indent (node);
      out << "element_list";
      print_common (node);
      print_children (node);
    }
    void visit (Element& node)
    {
      print_indent (node);
      out << "element";
      print_common (node);
      print_children (node);
    }
    void visit (CompositeLiteral& node)
    {
      print_indent (node);
      out << "composite_literal";
      print_common (node);
      print_children (node);
    }
  };

  visitor v (out);
  node.accept (v);
  return out;
}

#define ACCEPT(type) void                               \
  type::accept (NodeVisitor& visitor)                 \
  {                                                     \
    visitor.visit (*this);                              \
  }

ACCEPT (Identifier)
ACCEPT (IdentifierList)
ACCEPT (Receiver)
ACCEPT (ArrayTypeSpec)
ACCEPT (SliceTypeSpec)
ACCEPT (MapTypeSpec)
ACCEPT (EmptyTypeSpec)
ACCEPT (FieldListTypeSpec)
ACCEPT (HeapTypeSpec)
ACCEPT (IdentifierListTypeSpec)
ACCEPT (IdentifierTypeSpec)
ACCEPT (PointerTypeSpec)
ACCEPT (PushPortTypeSpec)
ACCEPT (PullPortTypeSpec)
ACCEPT (ParameterList)

ACCEPT (TypeExpression)
ACCEPT (BinaryArithmeticExpr)
ACCEPT (AddressOfExpr)
ACCEPT (CallExpr)
ACCEPT (ConversionExpr)
ACCEPT (DereferenceExpr)
ACCEPT (IdentifierExpr)
ACCEPT (IndexExpr)
ACCEPT (SliceExpr)
ACCEPT (EmptyExpr)
ACCEPT (IndexedPushPortCallExpr)
ACCEPT (ListExpr)
ACCEPT (LiteralExpr)
ACCEPT (UnaryArithmeticExpr)
ACCEPT (PushPortCallExpr)
ACCEPT (SelectExpr)

ACCEPT (EmptyStatement)
ACCEPT (AddAssignStatement)
ACCEPT (ChangeStatement)
ACCEPT (AssignStatement)
ACCEPT (ExpressionStatement)
ACCEPT (IfStatement)
ACCEPT (WhileStatement)
ACCEPT (ListStatement)
ACCEPT (ReturnStatement)
ACCEPT (IncrementDecrementStatement)
ACCEPT (SubtractAssignStatement)
ACCEPT (ActivateStatement)
ACCEPT (VarStatement)
ACCEPT (BindPushPortStatement)
ACCEPT (BindPushPortParamStatement)
ACCEPT (BindPullPortStatement)
ACCEPT (ForIotaStatement)

ACCEPT (Action)
ACCEPT (Const)
ACCEPT (DimensionedAction)
ACCEPT (Bind)
ACCEPT (Function)
ACCEPT (Getter)
ACCEPT (Initializer)
ACCEPT (Instance)
ACCEPT (Method)
ACCEPT (Reaction)
ACCEPT (DimensionedReaction)
ACCEPT (Type)
ACCEPT (SourceFile)
ACCEPT (ElementList)
ACCEPT (Element)
ACCEPT (CompositeLiteral)

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
  , temp (NULL)
  , operation (NULL)
{
  assert (location.line != 0);
}

LiteralExpr::LiteralExpr (unsigned int line, const ::type::Type* t, const Value& v)
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
  , this_identifier (this_id)
  , mutability (m)
  , indirection_mutability (dm)
  , is_pointer (isP)
  , type_identifier (type_id)
{ }

void
Receiver::visit_children (NodeVisitor& visitor)
{
  this_identifier->accept (visitor);
  type_identifier->accept (visitor);
}

ArrayTypeSpec::ArrayTypeSpec (unsigned int line, Node* dim, Node* base)
  : Node (line)
  , dimension (dim)
  , base_type (base)
{ }

void ArrayTypeSpec::visit_children (NodeVisitor& visitor)
{
  dimension->accept (visitor);
  base_type->accept (visitor);
}

EmptyTypeSpec::EmptyTypeSpec (unsigned int line)
  : Node (line)
{ }

FieldListTypeSpec::FieldListTypeSpec (unsigned int line)
  : List (line)
  , is_component (false)
{ }

HeapTypeSpec::HeapTypeSpec (unsigned int line, Node* child)
  : Unary (line, child)
{ }

IdentifierTypeSpec::IdentifierTypeSpec (unsigned int line, Identifier* child)
  : Unary (line, child)
{ }

IdentifierListTypeSpec::IdentifierListTypeSpec (unsigned int line,
    List* id_list,
    Mutability m,
    Mutability dm,
    Node* type_sp)
  : Node (line)
  , identifier_list (id_list)
  , mutability (m)
  , indirection_mutability (dm)
  , type_spec (type_sp)
{ }

void
IdentifierListTypeSpec::visit_children (NodeVisitor& visitor)
{
  identifier_list->accept (visitor);
  type_spec->accept (visitor);
}

PointerTypeSpec::PointerTypeSpec (unsigned int line, Node* child)
  : Unary (line, child)
{ }

SliceTypeSpec::SliceTypeSpec (unsigned int line, Node* child)
  : Unary (line, child)
{ }

MapTypeSpec::MapTypeSpec (unsigned int line, Node* k, Node* v)
  : Node (line)
  , key (k)
  , value (v)
{ }

void
MapTypeSpec::visit_children (NodeVisitor& visitor)
{
  key->accept (visitor);
  value->accept (visitor);
}

PushPortTypeSpec::PushPortTypeSpec (unsigned int line, Node* sig)
  : Node (line)
  , signature (sig)
{ }

void PushPortTypeSpec::visit_children (NodeVisitor& visitor)
{
  signature->accept (visitor);
}

PullPortTypeSpec::PullPortTypeSpec (unsigned int line,
                                    List* pl,
                                    List* rpl)
  : Node (line)
  , parameter_list (pl)
  , return_parameter_list (rpl)
{ }

void PullPortTypeSpec::visit_children (NodeVisitor& visitor)
{
  parameter_list->accept (visitor);
  return_parameter_list->accept (visitor);
}

ParameterList::ParameterList (unsigned int line)
  : List (line)
{ }

TypeExpression::TypeExpression (unsigned int line, Node* ts)
  : Node (line)
  , type_spec (ts)
{ }

void TypeExpression::visit_children (NodeVisitor& visitor)
{
  type_spec->accept (visitor);
}

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

BinaryArithmeticExpr::BinaryArithmeticExpr (unsigned int line, decl::PolymorphicFunction* a_temp, Node* left, Node* right)
  : Binary (line, left, right)
{
  temp = a_temp;
}

AddressOfExpr::AddressOfExpr (unsigned int line, Node* child)
  : Unary (line, child)
{ }

CallExpr::CallExpr (unsigned int line, Node* e, List* a)
  : Node (line)
  , expr (e)
  , args (a)
  , function_type (NULL)
  , push_port_type (NULL)
  , pull_port_type (NULL)
  , method_type (NULL)
  , initializer_type (NULL)
  , getter_type (NULL)
  , reaction_type (NULL)
  , parameter_list (NULL)
  , return_parameter_list (NULL)
{ }

void CallExpr::visit_children (NodeVisitor& visitor)
{
  expr->accept (visitor);
  args->accept (visitor);
}

ConversionExpr::ConversionExpr (unsigned int line, Node* te, Node* e)
  : Node (line)
  , type_expr (te)
  , expr (e)
{ }
void ConversionExpr::visit_children (NodeVisitor& visitor)
{
  type_expr->accept (visitor);
  expr->accept (visitor);
}

DereferenceExpr::DereferenceExpr (unsigned int line, Node* child)
  : Unary (line, child)
{ }

ListExpr::ListExpr (unsigned int line)
  : List (line)
{ }

IdentifierExpr::IdentifierExpr (unsigned int line, Identifier* c)
  : Unary (line, c)
{ }

IndexExpr::IndexExpr (unsigned int line, Node* b, Node* i)
  : Node (line)
  , base (b)
  , index (i)
  , array_type (NULL)
  , slice_type (NULL)
{ }

void IndexExpr::visit_children (NodeVisitor& visitor)
{
  base->accept (visitor);
  index->accept (visitor);
}

SliceExpr::SliceExpr (unsigned int line,
                      Node* b,
                      Node* l,
                      Node* h,
                      Node* m)
  : Node (line)
  , base (b)
  , low (l)
  , low_present (node_cast<EmptyExpr> (low) == NULL)
  , high (h)
  , high_present (node_cast<EmptyExpr> (high) == NULL)
  , max (m)
  , max_present (node_cast<EmptyExpr> (max) == NULL)
  , string_type (NULL)
  , pointer_to_array_type (NULL)
  , array_type (NULL)
  , slice_type (NULL)
{ }

void SliceExpr::visit_children (NodeVisitor& visitor)
{
  base->accept (visitor);
  low->accept (visitor);
  high->accept (visitor);
  max->accept (visitor);
}

EmptyExpr::EmptyExpr (unsigned int line)
  : Node (line)
{ }

UnaryArithmeticExpr::UnaryArithmeticExpr (unsigned int line, decl::PolymorphicFunction* a_temp, Node* child)
  : Unary (line, child)
{
  temp = a_temp;
}

PushPortCallExpr::PushPortCallExpr (unsigned int line, Identifier* id, List* a)
  : Node (line)
  , identifier (id)
  , args (a)
  , field (NULL)
  , push_port_type (NULL)
  , receiver_parameter (NULL)
{ }

void PushPortCallExpr::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  args->accept (visitor);
}

IndexedPushPortCallExpr::IndexedPushPortCallExpr (unsigned int line,
    Identifier* id,
    Node* idx,
    List* a)
  : Node (line)
  , identifier (id)
  , index (idx)
  , args (a)
  , field (NULL)
  , push_port_type (NULL)
  , receiver_parameter (NULL)
{ }

void IndexedPushPortCallExpr::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  index->accept (visitor);
  args->accept (visitor);
}

SelectExpr::SelectExpr (unsigned int line, Node* b, Identifier* id)
  : Node (line)
  , base (b)
  , identifier (id)
{ }

void SelectExpr::visit_children (NodeVisitor& visitor)
{
  base->accept (visitor);
  identifier->accept (visitor);
}

EmptyStatement::EmptyStatement (unsigned int line)
  : Node (line)
{ }

AddAssignStatement::AddAssignStatement (unsigned int line, Node* left, Node* right)
  : Binary (line, left, right)
{ }

AssignStatement::AssignStatement (unsigned int line, Node* left, Node* right)
  : Binary (line, left, right)
{ }

ChangeStatement::ChangeStatement (unsigned int line,
                                  Node * e,
                                  Identifier * id,
                                  Node * b)
  : Node (line)
  , expr (e)
  , identifier (id)
  , body (b)
{ }

void ChangeStatement::visit_children (NodeVisitor& visitor)
{
  expr->accept (visitor);
  identifier->accept (visitor);
  body->accept (visitor);
}

ExpressionStatement::ExpressionStatement (unsigned int line, Node* child)
  : Unary (line, child)
{ }

IfStatement::IfStatement (unsigned int line,
                          Node* s,
                          Node* c,
                          Node* tb,
                          Node* fb)
  : Node (line)
  , statement (s)
  , condition (c)
  , true_branch (tb)
  , false_branch (fb)
{ }

void IfStatement::visit_children (NodeVisitor& visitor)
{
  statement->accept (visitor);
  condition->accept (visitor);
  true_branch->accept (visitor);
  false_branch->accept (visitor);
}

WhileStatement::WhileStatement (unsigned int line, Node* c, Node* b)
  : Node (line)
  , condition (c)
  , body (b)
{ }

void WhileStatement::visit_children (NodeVisitor& visitor)
{
  condition->accept (visitor);
  body->accept (visitor);
}

ReturnStatement::ReturnStatement (unsigned int line, Node* child)
  : Unary (line, child)
  , return_symbol (NULL)
{ }

IncrementDecrementStatement::IncrementDecrementStatement (unsigned int line, Node* child, Kind a_kind)
  : Unary (line, child)
  , kind (a_kind)
{ }

ListStatement::ListStatement (unsigned int line)
  : List (line)
{ }

SubtractAssignStatement::SubtractAssignStatement (unsigned int line, Node* left, Node* right)
  : Binary (line, left, right)
{ }

ActivateStatement::ActivateStatement (unsigned int line, List * el, Node * b)
  : Node (line)
  , expr_list (el)
  , body (b)
  , memory_model (NULL)
  , in_action (false)
{ }

void ActivateStatement::visit_children (NodeVisitor& visitor)
{
  expr_list->accept (visitor);
  body->accept (visitor);
}

VarStatement::VarStatement (unsigned int line,
                            List* il,
                            Mutability m,
                            Mutability dm,
                            Node* ts,
                            List* el)
  : Node (line)
  , identifier_list (il)
  , mutability (m)
  , dereferenceMutability (dm)
  , type_spec (ts)
  , expression_list (el)
{ }

void VarStatement::visit_children (NodeVisitor& visitor)
{
  identifier_list->accept (visitor);
  type_spec->accept (visitor);
  expression_list->accept (visitor);
}

BindPushPortStatement::BindPushPortStatement (unsigned int line, Node* left, Node* right)
  : Binary (line, left, right)
{ }

BindPushPortParamStatement::BindPushPortParamStatement (unsigned int line,
    Node* l,
    Node* r,
    Node* p)
  : Node (line)
  , left (l)
  , right (r)
  , param (p)
{ }

void BindPushPortParamStatement::visit_children (NodeVisitor& visitor)
{
  left->accept (visitor);
  right->accept (visitor);
  param->accept (visitor);
}

BindPullPortStatement::BindPullPortStatement (unsigned int line, Node* left, Node* right)
  : Binary (line, left, right)
{ }

ForIotaStatement::ForIotaStatement (unsigned int line,
                                    Identifier* id,
                                    Node* lim,
                                    Node* b)
  : Node (line)
  , identifier (id)
  , limit_node (lim)
  , body (b)
{ }

void ForIotaStatement::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  limit_node->accept (visitor);
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
  , parameter_list (pl)
  , return_parameter_list (rpl)
  , body (b)
  , function (NULL)
{ }

void Function::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  parameter_list->accept (visitor);
  return_parameter_list->accept (visitor);
  body->accept (visitor);
}

Instance::Instance (unsigned int line,
                    Identifier* id,
                    Node* tn,
                    Identifier* init,
                    List* el)
  : Node (line)
  , identifier (id)
  , type_name (tn)
  , initializer (init)
  , expression_list (el)
{ }

void Instance::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  type_name->accept (visitor);
  initializer->accept (visitor);
  expression_list->accept (visitor);
}

Const::Const (unsigned int line,
              List* il,
              Node* ts,
              List* el)
  : Node (line)
  , identifier_list (il)
  , type_spec (ts)
  , expression_list (el)
  , done (false)
{ }

void Const::visit_children (NodeVisitor& visitor)
{
  identifier_list->accept (visitor);
  type_spec->accept (visitor);
  expression_list->accept (visitor);
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
  , parameter_list (a_parameter_list)
  , return_parameter_list (a_return_parameter_list)
  , body (b)
  , method (NULL)
{ }

void Method::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  identifier->accept (visitor);
  parameter_list->accept (visitor);
  return_parameter_list->accept (visitor);
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
  , parameter_list (pl)
  , return_parameter_list (rpl)
  , body (b)
  , getter (NULL)
{ }

void Getter::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  identifier->accept (visitor);
  parameter_list->accept (visitor);
  return_parameter_list->accept (visitor);
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
  , parameter_list (pl)
  , return_parameter_list (rpl)
  , body (b)
  , initializer (NULL)
{ }

void Initializer::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  identifier->accept (visitor);
  parameter_list->accept (visitor);
  return_parameter_list->accept (visitor);
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
  , parameter_list (pl)
  , return_parameter_list (new ParameterList (line))
  , body (b)
  , reaction (NULL)
{ }

void Reaction::visit_children (NodeVisitor& visitor)
{
  receiver->accept (visitor);
  identifier->accept (visitor);
  parameter_list->accept (visitor);
  return_parameter_list->accept (visitor);
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
  , parameter_list (pl)
  , return_parameter_list (new ParameterList (line))
  , body (b)
  , reaction (NULL)
{ }

void DimensionedReaction::visit_children (NodeVisitor& visitor)
{
  dimension->accept (visitor);
  receiver->accept (visitor);
  identifier->accept (visitor);
  parameter_list->accept (visitor);
  return_parameter_list->accept (visitor);
  body->accept (visitor);
}

Type::Type (unsigned int line, Identifier* i, Node* ts)
  : Node (line)
  , identifier (i)
  , type_spec (ts)
{ }

void Type::visit_children (NodeVisitor& visitor)
{
  identifier->accept (visitor);
  type_spec->accept (visitor);
}

SourceFile::SourceFile ()
  : List (-1)
{ }

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
  , literal_type (lt)
  , literal_value (lv)
{ }

void CompositeLiteral::visit_children (NodeVisitor& visitor)
{
  literal_type->accept (visitor);
  literal_value->accept (visitor);
}

void EmptyTypeSpec::visit_children (NodeVisitor& visitor) { }
void EmptyExpr::visit_children (NodeVisitor& visitor) { }
void LiteralExpr::visit_children (NodeVisitor& visitor) { }
void EmptyStatement::visit_children (NodeVisitor& visitor) { }

}
