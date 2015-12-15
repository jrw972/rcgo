#include "evaluate_static.hpp"

#include "type.hpp"
#include "ast.hpp"
#include "ast_visitor.hpp"
#include "symbol.hpp"

namespace semantic
{

using namespace type;
using namespace ast;

std::ostream&
operator<< (std::ostream& o,
            const static_value_t& v)
{
  switch (v.kind)
    {
    case static_value_t::STACK_ADDRESS:
      o << "STACK " << v.offset;
      break;
    case static_value_t::ABSOLUTE_ADDRESS:
      o << "ABSOLUTE " << v.address;
      break;
    case static_value_t::VALUE:
      o << "VALUE " << v.value;
      break;
    }
  return o;
}

static_value_t
EvaluateStatic (const ast::Node* node, const static_memory_t& memory)
{
  UNIMPLEMENTED;
  // struct visitor : public ast::DefaultConstVisitor
  // {
  //   const static_memory_t& memory;
  //   static_value_t result;

  //   visitor (const static_memory_t& m) : memory (m) { }

  //   void default_action (const Node& node)
  //   {
  //     AST_NOT_REACHED (node);
  //   }

  //   void visit (const ast_binary_arithmetic_expr_t& node)
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
  //         static_value_t left = EvaluateStatic (node.left (), memory);
  //         static_value_t right = EvaluateStatic (node.right (), memory);
  //         result = static_value_t::make_value (left.value != right.value);
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

  //   void visit (const ast_literal_expr_t& node)
  //   {
  //     typed_value_t tv = node.typed_value;
  //     struct visitor : public Type::DefaultVisitor
  //     {
  //       typed_value_t tv;
  //       static_value_t result;

  //       visitor (typed_value_t t) : tv (t) { }

  //       void default_action (const Type::Type& type)
  //       {
  //         NOT_REACHED;
  //       }

  //       void visit (const Int& type)
  //       {
  //         UNIMPLEMENTED;
  //         //result = static_value_t::make_value (tv.int_value);
  //       }
  //     };
  //     visitor v (tv);
  //     tv.type->Accept (v);
  //     result = v.result;
  //   }

  //   void visit (const ast_index_expr_t& node)
  //   {
  //     static_value_t base = EvaluateStatic (node.base (), memory);
  //     static_value_t index = EvaluateStatic (node.index (), memory);
  //     typed_value_t base_tv = node.base ()->typed_value;
  //     const Array* array_type = type_cast<Array> (base_tv.type);
  //     result = static_value_t::index (base, array_type, index);
  //   }

  //   void visit (const ast_select_expr_t& node)
  //   {
  //     typed_value_t tv = node.typed_value;
  //     assert (tv.has_offset);
  //     static_value_t v = EvaluateStatic (node.base (), memory);
  //     result = static_value_t::select (v, tv.offset);
  //   }

  //   void visit (const ast_dereference_expr_t& node)
  //   {
  //     static_value_t v = EvaluateStatic (node.child (), memory);
  //     result = static_value_t::dereference (v);
  //   }

  //   void visit (const ast_implicit_dereference_expr_t& node)
  //   {
  //     static_value_t v = EvaluateStatic (node.child (), memory);
  //     result = static_value_t::implicit_dereference (v, memory);
  //   }

  //   void visit (const ast_identifier_expr_t& node)
  //   {
  //     ptrdiff_t offset = node.symbol->offset ();
  //     result = static_value_t::make_stack_offset (offset);
  //   }

  //   void visit (const ast_address_of_expr_t& node)
  //   {
  //     static_value_t v = EvaluateStatic (node.child (), memory);
  //     result = static_value_t::address_of (v);
  //   }
  // };
  // visitor v (memory);
  // node->Accept (v);
  // return v.result;
}

}
