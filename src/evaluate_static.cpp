#include "evaluate_static.hpp"

#include "type.hpp"
#include "node.hpp"
#include "node_visitor.hpp"
#include "symbol.hpp"

namespace semantic
{

using namespace type;
using namespace ast;

std::ostream&
operator<< (std::ostream& o,
            const static_Value& v)
{
  switch (v.kind)
    {
    case static_Value::STACK_ADDRESS:
      o << "STACK " << v.offset;
      break;
    case static_Value::ABSOLUTE_ADDRESS:
      o << "ABSOLUTE " << v.address;
      break;
    case static_Value::VALUE:
      o << "VALUE " << v.value;
      break;
    }
  return o;
}

static_Value
EvaluateStatic (const ast::Node* node, const static_memory_t& memory)
{
  UNIMPLEMENTED;
  // struct visitor : public ast::DefaultConstVisitor
  // {
  //   const static_memory_t& memory;
  //   static_Value result;

  //   visitor (const static_memory_t& m) : memory (m) { }

  //   void default_action (const Node& node)
  //   {
  //     AST_NOT_REACHED (node);
  //   }

  //   void visit (const BinaryArithmeticExpr& node)
  //   {
  //     switch (node.arithmetic)
  //       {
  //       case Multiply:
  //       case Divide:
  //       case Modulus:
  //       case LeftShift:
  //       case RightShift:
  //       case BitAnd:
  //       case BitAndNot:
  //         UNIMPLEMENTED;

  //       case Add:
  //       case Subtract:
  //       case BitOr:
  //       case BitXor:
  //         UNIMPLEMENTED;

  //       case Equal:
  //         UNIMPLEMENTED;
  //       case NotEqual:
  //       {
  //         static_Value left = EvaluateStatic (node.left (), memory);
  //         static_Value right = EvaluateStatic (node.right (), memory);
  //         result = static_Value::make_value (left.value != right.value);
  //       }
  //       break;
  //       case LessThan:
  //       case LessEqual:
  //       case MoreThan:
  //       case MoreEqual:
  //         UNIMPLEMENTED;
  //       case LogicAnd:
  //         UNIMPLEMENTED;
  //       case LogicOr:
  //         UNIMPLEMENTED;
  //       }
  //   }

  //   void visit (const LiteralExpr& node)
  //   {
  //     typed_Value tv = node.typed_value;
  //     struct visitor : public Type::DefaultVisitor
  //     {
  //       typed_Value tv;
  //       static_Value result;

  //       visitor (typed_Value t) : tv (t) { }

  //       void default_action (const Type::Type& type)
  //       {
  //         NOT_REACHED;
  //       }

  //       void visit (const Int& type)
  //       {
  //         UNIMPLEMENTED;
  //         //result = static_Value::make_value (tv.int_value);
  //       }
  //     };
  //     visitor v (tv);
  //     tv.type->Accept (v);
  //     result = v.result;
  //   }

  //   void visit (const IndexExpr& node)
  //   {
  //     static_Value base = EvaluateStatic (node.base (), memory);
  //     static_Value index = EvaluateStatic (node.index (), memory);
  //     typed_Value base_tv = node.base ()->typed_value;
  //     const Array* array_type = type_cast<Array> (base_tv.type);
  //     result = static_Value::index (base, array_type, index);
  //   }

  //   void visit (const SelectExpr& node)
  //   {
  //     typed_Value tv = node.typed_value;
  //     assert (tv.has_offset);
  //     static_Value v = EvaluateStatic (node.base (), memory);
  //     result = static_Value::select (v, tv.offset);
  //   }

  //   void visit (const DereferenceExpr& node)
  //   {
  //     static_Value v = EvaluateStatic (node.child (), memory);
  //     result = static_Value::dereference (v);
  //   }

  //   void visit (const IdentifierExpr& node)
  //   {
  //     ptrdiff_t offset = node.symbol->offset ();
  //     result = static_Value::make_stack_offset (offset);
  //   }

  //   void visit (const AddressOfExpr& node)
  //   {
  //     static_Value v = EvaluateStatic (node.child (), memory);
  //     result = static_Value::address_of (v);
  //   }
  // };
  // visitor v (memory);
  // node->Accept (v);
  // return v.result;
}

}
