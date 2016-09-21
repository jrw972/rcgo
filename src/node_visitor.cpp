#include "node_visitor.hpp"

#include "node.hpp"

namespace ast
{

void DefaultNodeVisitor::visit (Identifier& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IdentifierList& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Receiver& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (Array& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Slice& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Map& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (EmptyType& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (FieldList& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Heap& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (VariableList& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IdentifierType& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Pointer& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (PushPort& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (PullPort& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ParameterList& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (TypeExpression& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (BinaryArithmetic& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (AddressOf& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Call& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Conversion& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Dereference& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IdentifierExpression& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Index& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IndexSlice& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (EmptyExpression& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IndexedPushPortCall& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ExpressionList& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Literal& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (UnaryArithmetic& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (PushPortCall& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Select& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (EmptyStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (AddAssign& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Change& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Assign& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ExpressionStatement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (If& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (While& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (StatementList& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Return& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (IncrementDecrement& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (SubtractAssign& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Activate& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Var& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (BindPushPort& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (BindPushPortParameter& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (BindPullPort& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (ForIota& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (Action& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Const& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (DimensionedAction& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Bind& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Function& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Getter& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Initializer& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Instance& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Method& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Reaction& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (DimensionedReaction& node)
{
  default_action (node);
}
void DefaultNodeVisitor::visit (Type& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (SourceFile& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (ElementList& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (Element& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (CompositeLiteral& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (ImportDeclList& node)
{
  default_action (node);
}

void DefaultNodeVisitor::visit (TopLevelDeclList& node)
{
  default_action (node);
}

void DefaultNodeVisitor::default_action (Node& node)
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
      out << node << '\n';
      print_children (node);
    }


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


}
