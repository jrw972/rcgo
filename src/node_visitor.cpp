#include "node_visitor.hpp"

#include "node.hpp"

#define VISIT(type) void \
DefaultNodeVisitor::visit (type& node) \
{ \
  default_action (node); \
}

namespace ast
{

VISIT (ActionDecl)
VISIT (Activate)
VISIT (AddAssign)
VISIT (AddressOf)
VISIT (Array)
VISIT (Assign)
VISIT (BinderDecl)
VISIT (BindPullPort)
VISIT (BindPushPort)
VISIT (BindPushPortParameter)
VISIT (Call)
VISIT (Change)
VISIT (CompositeLiteral)
VISIT (ConstDecl)
VISIT (Conversion)
VISIT (Dereference)
VISIT (Element)
VISIT (ElementList)
VISIT (EmptyExpression)
VISIT (EmptyStatement)
VISIT (EmptyType)
VISIT (ExpressionList)
VISIT (ExpressionStatement)
VISIT (FieldList)
VISIT (ForIota)
VISIT (FunctionDecl)
VISIT (GetterDecl)
VISIT (Heap)
VISIT (IdentifierExpression)
VISIT (IdentifierList)
VISIT (IdentifierType)
VISIT (If)
VISIT (IncrementDecrement)
VISIT (Index)
VISIT (IndexSlice)
VISIT (IndexedPushPortCall)
VISIT (InitializerDecl)
VISIT (InstanceDecl)
VISIT (Literal)
VISIT (Map)
VISIT (MethodDecl)
VISIT (ParameterList)
VISIT (Pointer)
VISIT (PullPort)
VISIT (PushPort)
VISIT (PushPortCall)
VISIT (ReactionDecl)
VISIT (Receiver)
VISIT (Return)
VISIT (Select)
VISIT (Slice)
VISIT (StatementList)
VISIT (SubtractAssign)
VISIT (TopLevelDeclList)
VISIT (TypeDecl)
VISIT (TypeExpression)
VISIT (VarDecl)
VISIT (VariableList)
VISIT (While)

void DefaultNodeVisitor::default_action (Node& node)
{ }

#ifndef COVERAGE
TreePrinter::TreePrinter (Node& a_node)
  : node (a_node)
{ }

void TreePrinter::print (std::ostream& out) const
{
  struct visitor : public DefaultNodeVisitor
  {
    std::ostream& out;
    size_t indent;

    visitor (std::ostream& o) : out (o), indent (0) { }

    virtual void default_action (Node& node)
    {
      print_indent (node);
      out << node << ' ' << node.eval << '\n';
      print_children (node);
    }


    void print_indent (const Node& node)
    {
      for (size_t idx = 0; idx != indent; ++idx)
        {
          out << ' ';
        }
      out << node.location.line () << ' ';
    }

    void print_children (Node& node)
    {
      size_t old_indent = indent;
      indent += 2;
      node.visit_children (*this);
      indent = old_indent;
    }

    // void print_common (const Node& node)
    // {
    //   if (node.eval.type != NULL)
    //     {
    //       out << ' ' << *node.eval.type;
    //       if (node.eval.value.present)
    //         {
    //           out << ' ' << ValuePrinter (node.eval.type, node.eval.value);
    //         }
    //       out << ' ';
    //       switch (node.eval.expression_kind)
    //         {
    //         case UnknownExpressionKind:
    //           out << "unknown";
    //           break;
    //         case ErrorExpressionKind:
    //           out << "error";
    //           break;
    //         case ValueExpressionKind:
    //           out << "value";
    //           break;
    //         case VariableExpressionKind:
    //           out << "variable";
    //           break;
    //         case VoidExpressionKind:
    //           out << "void";
    //           break;
    //         case TypeExpressionKind:
    //           out << "type";
    //           break;
    //         }
    //       out << ' ';
    //       switch (node.eval.intrinsic_mutability)
    //         {
    //         case Mutable:
    //           break;
    //         case Immutable:
    //           out << "const";
    //           break;
    //         case Foreign:
    //           out << "foreign";
    //           break;
    //         }
    //       out << ' ';
    //       switch (node.eval.indirection_mutability)
    //         {
    //         case Mutable:
    //           break;
    //         case Immutable:
    //           out << "+const";
    //           break;
    //         case Foreign:
    //           out << "+foreign";
    //           break;
    //         }
    //     }
    //   out << " receiver_state=" << node.eval.receiver_state;
    //   out << " receiver_access=";
    //   switch (node.eval.receiver_access)
    //     {
    //     case AccessNone:
    //       out << "none";
    //       break;
    //     case AccessRead:
    //       out << "read";
    //       break;
    //     case AccessWrite:
    //       out << "write";
    //       break;
    //     }
    //   out << '\n';
    // }
  };

  visitor v (out);
  node.accept (v);
}

std::ostream& operator<< (std::ostream& out,
                          const TreePrinter& printer)
{
  printer.print (out);
  return out;
}
#endif

}
