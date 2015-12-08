#include "check_types.hpp"

#include <error.h>

#include "ast_visitor.hpp"
#include "template.hpp"
#include "callable.hpp"
#include "symbol_visitor.hpp"
#include "semantic.hpp"
#include "symbol.hpp"
#include "field.hpp"
#include "process_types_and_constants.hpp"
#include "symbol.hpp"
#include "action.hpp"
#include "reaction.hpp"

namespace semantic
{

using namespace ast;
using namespace Type;

namespace
{
static void require_value_or_variable (const Node* node)
{
  if (!(node->expression_kind == kValue ||
        node->expression_kind == kVariable))
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "required a value (E78)");
    }
}

static void require_variable (const Node* node)
{
  if (!(node->expression_kind == kVariable))
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "required a variable (E2)");
    }
}

// static void require_variable_list (const Node* node) {
//   for (Node::ConstIterator pos = node->Begin (), limit = node->End ();
//        pos != limit;
//        ++pos) {
//     require_variable (*pos);
//   }
// }

static void convert (ast::Node* node, const Type::Type* type)
{
  if (node->type != type)
    {
      if (node->value.present)
        {
          node->value.convert (node->type, type);
        }
      node->type = type;
    }
}

static const reaction_t* bind (Node& node, ast::Node* port_node, ast::Node* reaction_node)
{
  const Type::Function* push_port_type = Type::type_cast<Type::Function> (port_node->type);

  if (push_port_type == NULL || push_port_type->function_kind != Type::Function::PUSH_PORT)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "source of bind is not a port (E38)");
    }

  require_variable (port_node);

  const Type::Method* reaction_type = Type::type_cast<Type::Method> (reaction_node->type);
  if (reaction_type == NULL || reaction_type->method_kind != Type::Method::REACTION)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "target of bind is not a reaction (E39)");
    }

  require_variable (reaction_node);

  if (!type_is_equal (push_port_type->GetSignature (), reaction_type->signature))
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "cannot bind %s to %s (E40)", push_port_type->ToString ().c_str (), reaction_type->ToString ().c_str ());
    }

  return static_cast<const reaction_t*> (reaction_node->callable);
}

static void check_condition (Node* node)
{
  const Type::Type* condition = node->type;
  if (!(is_any_boolean (condition)))
    {
      error_at_line (-1, 0,
                     node->location.File.c_str (),
                     node->location.Line,
                     "condition is not boolean (E155)");
    }
  convert (node, condition->DefaultType ());
  require_value_or_variable (node);
}

// Select the type for == and !=.
// NULL means no suitable type.
static const Type::Type* comparable (const Type::Type* left_type, const Type::Type* right_type)
{
  switch (left_type->underlying_kind ())
    {
    case kNil:
    case kBoolean:
    case kString:
    case kBool:
    case kUint8:
    case kUint16:
    case kUint32:
    case kUint64:
    case kInt8:
    case kInt16:
    case kInt32:
    case kInt64:
    case kFloat32:
    case kFloat64:
    case kComplex64:
    case kComplex128:
    case kUint:
    case kInt:
    case kUintptr:
    case kStringU:
    case kPointer:
      if (left_type == right_type)
        {
          return left_type;
        }
      break;

    case kRune:
    case kInteger:
    case kFloat:
    case kComplex:
      if (is_untyped_numeric (right_type))
        {
          return Choose (left_type, right_type);
        }
      break;

    default:
      break;
    }

  return NULL;
}

static void process_comparable (const char* s, ast_binary_expr_t& node, void (*func) (value_t&, const Type::Type*, const value_t&, const value_t&))
{
  const Type::Type*& left_type = node.left ()->type;
  value_t& left_value = node.left ()->value;
  const Type::Type*& right_type = node.right ()->type;
  value_t& right_value = node.right ()->value;
  const Type::Type*& type = node.type;
  value_t& value = node.value;

  if (left_type->IsUntyped () &&
      right_type->IsUntyped ())
    {
      const Type::Type* t = comparable (left_type, right_type);
      if (t == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s cannot be applied to %s and %s (E206)",
                         s,
                         left_type->ToString ().c_str (),
                         right_type->ToString ().c_str ());
        }
      convert (node.left (), t);
      convert (node.right (), t);
      type = Boolean::Instance ();
      func (value, t, left_value, right_value);
      return;
    }

  if (!(assignable (left_type, left_value, right_type) ||
        assignable (right_type, right_value, left_type)))
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s cannot be applied to %s and %s (E207)",
                     s,
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  const Type::Type* in_type = Choose (left_type, right_type);
  convert (node.left (), in_type);
  convert (node.right (), in_type);

  if (comparable (in_type, in_type) == NULL)
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s cannot be applied to %s and %s (E124)",
                     s,
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  if (left_value.present &&
      right_value.present)
    {
      type = Boolean::Instance ();
      func (value, in_type, left_value, right_value);
    }
  else
    {
      node.type = Bool::Instance ();
    }
}

// Select the type for <, <=, >, and >=.
// NULL means no suitable type.
static const Type::Type* orderable (const Type::Type* left_type, const Type::Type* right_type)
{
  switch (left_type->underlying_kind ())
    {
    case kString:
    case kUint8:
    case kUint16:
    case kUint32:
    case kUint64:
    case kInt8:
    case kInt16:
    case kInt32:
    case kInt64:
    case kFloat32:
    case kFloat64:
    case kUint:
    case kInt:
    case kUintptr:
    case kStringU:
      if (left_type == right_type)
        {
          return left_type;
        }
      break;

    case kRune:
    case kInteger:
    case kFloat:
      if (right_type->underlying_kind () == kRune ||
          right_type->underlying_kind () == kInteger ||
          right_type->underlying_kind () == kFloat)
        {
          return Choose (left_type, right_type);
        }
      break;

    default:
      break;
    }

  return NULL;
}

static void process_orderable (const char* s, ast_binary_expr_t& node, void (*func) (value_t&, const Type::Type*, const value_t&, const value_t&))
{
  const Type::Type*& left_type = node.left ()->type;
  value_t& left_value = node.left ()->value;
  const Type::Type*& right_type = node.right ()->type;
  value_t& right_value = node.right ()->value;
  const Type::Type*& type = node.type;
  value_t& value = node.value;

  if (left_type->IsUntyped () &&
      right_type->IsUntyped ())
    {
      const Type::Type* t = orderable (left_type, right_type);
      if (t == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s cannot be applied to %s and %s (E208)",
                         s,
                         left_type->ToString ().c_str (),
                         right_type->ToString ().c_str ());
        }
      convert (node.left (), t);
      convert (node.right (), t);
      type = Boolean::Instance ();
      func (value, t, left_value, right_value);
      return;
    }

  if (!(assignable (left_type, left_value, right_type) ||
        assignable (right_type, right_value, left_type)))
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s cannot be applied to %s and %s (E209)",
                     s,
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  const Type::Type* in_type = Choose (left_type, right_type);
  convert (node.left (), in_type);
  convert (node.right (), in_type);

  if (orderable (in_type, in_type) == NULL)
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s cannot be applied to %s and %s (E170)",
                     s,
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  if (left_value.present &&
      right_value.present)
    {
      type = Boolean::Instance ();
      func (value, in_type, left_value, right_value);
    }
  else
    {
      node.type = Bool::Instance ();
    }
}

// Select the type for *, /, +, -.
// NULL means no suitable type.
static const Type::Type* arithmetic (const Type::Type* left_type, const Type::Type* right_type)
{
  switch (left_type->underlying_kind ())
    {
    case kUint8:
    case kUint16:
    case kUint32:
    case kUint64:
    case kInt8:
    case kInt16:
    case kInt32:
    case kInt64:
    case kFloat32:
    case kFloat64:
    case kComplex64:
    case kComplex128:
    case kUint:
    case kInt:
    case kUintptr:
      if (left_type == right_type)
        {
          return left_type;
        }
      break;

    case kRune:
    case kInteger:
    case kFloat:
    case kComplex:
      if (is_untyped_numeric (right_type))
        {
          return Choose (left_type, right_type);
        }
      break;

    default:
      break;
    }

  return NULL;
}

static void process_arithmetic (const char* s, ast_binary_expr_t& node, void (*func) (value_t&, const Type::Type*, const value_t&, const value_t&))
{
  const Type::Type*& left_type = node.left ()->type;
  value_t& left_value = node.left ()->value;
  const Type::Type*& right_type = node.right ()->type;
  value_t& right_value = node.right ()->value;
  const Type::Type*& type = node.type;
  value_t& value = node.value;

  if (left_type->IsUntyped () &&
      right_type->IsUntyped ())
    {
      const Type::Type* t = arithmetic (left_type, right_type);
      if (t == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s cannot be applied to %s and %s (E210)",
                         s,
                         left_type->ToString ().c_str (),
                         right_type->ToString ().c_str ());
        }
      convert (node.left (), t);
      convert (node.right (), t);
      type = t;
      func (value, t, left_value, right_value);
      return;
    }

  if (!(assignable (left_type, left_value, right_type) ||
        assignable (right_type, right_value, left_type)))
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s cannot be applied to %s and %s (E211)",
                     s,
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  const Type::Type* in_type = Choose (left_type, right_type);
  convert (node.left (), in_type);
  convert (node.right (), in_type);

  if (arithmetic (in_type, in_type) == NULL)
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s cannot be applied to %s and %s (E171)",
                     s,
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  if (left_value.present &&
      right_value.present)
    {
      type = in_type;
      func (value, in_type, left_value, right_value);
    }
  else
    {
      node.type = in_type;
    }
}

// Select the type for %.
// NULL means no suitable type.
static const Type::Type* integral (const Type::Type* left_type, const Type::Type* right_type)
{
  switch (left_type->underlying_kind ())
    {
    case kUint8:
    case kUint16:
    case kUint32:
    case kUint64:
    case kInt8:
    case kInt16:
    case kInt32:
    case kInt64:
    case kUint:
    case kInt:
    case kUintptr:
      if (left_type == right_type)
        {
          return left_type;
        }
      break;

    case kRune:
    case kInteger:
      if (right_type->underlying_kind () == kRune ||
          right_type->underlying_kind () == kInteger)
        {
          return Choose (left_type, right_type);
        }
      break;

    default:
      break;
    }

  return NULL;
}

static void process_integral (const char* s, ast_binary_expr_t& node, void (*func) (value_t&, const Type::Type*, const value_t&, const value_t&))
{
  const Type::Type*& left_type = node.left ()->type;
  value_t& left_value = node.left ()->value;
  const Type::Type*& right_type = node.right ()->type;
  value_t& right_value = node.right ()->value;
  const Type::Type*& type = node.type;
  value_t& value = node.value;

  if (left_type->IsUntyped () &&
      right_type->IsUntyped ())
    {
      const Type::Type* t = integral (left_type, right_type);
      if (t == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s cannot be applied to %s and %s (E212)",
                         s,
                         left_type->ToString ().c_str (),
                         right_type->ToString ().c_str ());
        }
      convert (node.left (), t);
      convert (node.right (), t);
      type = t;
      func (value, t, left_value, right_value);
      return;
    }

  if (!(assignable (left_type, left_value, right_type) ||
        assignable (right_type, right_value, left_type)))
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s cannot be applied to %s and %s (E213)",
                     s,
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  const Type::Type* in_type = Choose (left_type, right_type);
  convert (node.left (), in_type);
  convert (node.right (), in_type);

  if (integral (in_type, in_type) == NULL)
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s cannot be applied to %s and %s (E172)",
                     s,
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  if (left_value.present &&
      right_value.present)
    {
      type = in_type;
      func (value, in_type, left_value, right_value);
    }
  else
    {
      node.type = in_type;
    }
}

template <typename T>
static void process_shift (ast_binary_expr_t& node)
{
  if (!(is_typed_unsigned_integer (node.right ()->type) || is_untyped_numeric (node.right ()->type)))
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s is not integral (E65)",
                     node.right ()->type->ToString ().c_str ());
    }

  if (node.right ()->value.present)
    {
      if (!node.right ()->value.representable (node.right ()->type, Type::Uint::Instance ()))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "shift amount %s is not a uint (E90)",
                         node.right ()->type->ToString ().c_str ());
        }
      node.right ()->value.convert (node.right ()->type, Type::Uint::Instance ());
      node.right ()->type = Type::Uint::Instance ();
    }

  if (!integral (node.left ()->type))
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s is not integral (E125)",
                     node.left ()->type->ToString ().c_str ());
    }

  node.type = node.left ()->type;

  if (node.left ()->value.present &&
      node.right ()->value.present)
    {
      node.value.present = true;
      switch (node.type->underlying_kind ())
        {
        case kUint8:
          node.value.uint8_value_ = T () (node.left ()->value.uint8_value_, node.right ()->value.uint_value_);
          break;
        case kUint16:
          node.value.uint16_value_ = T () (node.left ()->value.uint16_value_, node.right ()->value.uint_value_);
          break;
        case kUint32:
          node.value.uint32_value_ = T () (node.left ()->value.uint32_value_, node.right ()->value.uint_value_);
          break;
        case kUint64:
          node.value.uint64_value_ = T () (node.left ()->value.uint64_value_, node.right ()->value.uint_value_);
          break;
        case kInt8:
          node.value.int8_value_ = T () (node.left ()->value.int8_value_, node.right ()->value.uint_value_);
          break;
        case kInt16:
          node.value.int16_value_ = T () (node.left ()->value.int16_value_, node.right ()->value.uint_value_);
          break;
        case kInt32:
          node.value.int32_value_ = T () (node.left ()->value.int32_value_, node.right ()->value.uint_value_);
          break;
        case kInt64:
          node.value.int64_value_ = T () (node.left ()->value.int64_value_, node.right ()->value.uint_value_);
          break;
        case kUint:
          node.value.uint_value_ = T () (node.left ()->value.uint_value_, node.right ()->value.uint_value_);
          break;
        case kInt:
          node.value.int_value_ = T () (node.left ()->value.int_value_, node.right ()->value.uint_value_);
          break;
        case kUintptr:
          node.value.uintptr_value_ = T () (node.left ()->value.uintptr_value_, node.right ()->value.uint_value_);
          break;
        case kRune:
          node.value.rune_value_ = T () (node.left ()->value.rune_value_, node.right ()->value.uint_value_);
          break;
        case kInteger:
          node.value.integer_value_ = T () (node.left ()->value.integer_value_, node.right ()->value.uint_value_);
          break;
        default:
          type_not_reached (*node.type);
        }
    }
}

static void process_logic_or (ast_binary_expr_t& node)
{
  const Type::Type*& left_type = node.left ()->type;
  value_t& left_value = node.left ()->value;
  const Type::Type*& right_type = node.right ()->type;
  value_t& right_value = node.right ()->value;
  const Type::Type*& type = node.type;
  value_t& value = node.value;

  if (left_type->IsUntyped () &&
      right_type->IsUntyped ())
    {
      if (!(is_untyped_boolean (left_type) && is_untyped_boolean (right_type)))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "|| cannot be applied to %s and %s (E63)",
                         left_type->ToString ().c_str (),
                         right_type->ToString ().c_str ());
        }
      type = Boolean::Instance ();
      value.present = true;
      value.boolean_value_ = left_value.boolean_value_ || right_value.boolean_value_;
      return;
    }

  if (!(assignable (left_type, left_value, right_type) ||
        assignable (right_type, right_value, left_type)))
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "|| cannot be applied to %s and %s (E178)",
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  const Type::Type* in_type = Choose (left_type, right_type);
  convert (node.left (), in_type);
  convert (node.right (), in_type);

  if (in_type->underlying_kind () != kBool)
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "|| cannot be applied to %s and %s (E179)",
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  if (left_value.present &&
      left_value.bool_value_)
    {
      type = Boolean::Instance ();
      value.boolean_value_ = true;
    }
  else if (right_value.present)
    {
      type = Boolean::Instance ();
      value.boolean_value_ = right_value.bool_value_;
    }
  else
    {
      node.type = Bool::Instance ();
    }
}

static void process_logic_and (ast_binary_expr_t& node)
{
  const Type::Type*& left_type = node.left ()->type;
  value_t& left_value = node.left ()->value;
  const Type::Type*& right_type = node.right ()->type;
  value_t& right_value = node.right ()->value;
  const Type::Type*& type = node.type;
  value_t& value = node.value;

  if (left_type->IsUntyped () &&
      right_type->IsUntyped ())
    {
      if (!(is_untyped_boolean (left_type) && is_untyped_boolean (right_type)))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "&& cannot be applied to %s and %s (E180)",
                         left_type->ToString ().c_str (),
                         right_type->ToString ().c_str ());
        }
      type = Boolean::Instance ();
      value.present = true;
      value.boolean_value_ = left_value.boolean_value_ && right_value.boolean_value_;
      return;
    }

  if (!(assignable (left_type, left_value, right_type) ||
        assignable (right_type, right_value, left_type)))
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "&& cannot be applied to %s and %s (E181)",
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  const Type::Type* in_type = Choose (left_type, right_type);
  convert (node.left (), in_type);
  convert (node.right (), in_type);

  if (in_type->underlying_kind () != kBool)
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "&& cannot be applied to %s and %s (E182)",
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  if (left_value.present &&
      !left_value.bool_value_)
    {
      type = Boolean::Instance ();
      value.boolean_value_ = false;
    }
  else if (right_value.present)
    {
      type = Boolean::Instance ();
      value.boolean_value_ = right_value.bool_value_;
    }
  else
    {
      node.type = Bool::Instance ();
    }
}

// Determine the type of an expression.
struct Visitor : public ast::DefaultVisitor
{
  ParameterSymbol* receiver_parameter;

  Visitor () : receiver_parameter (NULL) { }

  void default_action (Node& node)
  {
    ast_not_reached (node);
  }

  void visit (ast_call_expr_t& node)
  {
    node.VisitChildren (*this);

    // Collect the argument types.
    TypeList argument_types;
    Node* args = node.args ();
    for (Node::ConstIterator pos = args->Begin (), limit = args->End ();
         pos != limit;
         ++pos)
      {
        argument_types.push_back ((*pos)->type);
      }

    if (node.expr ()->expression_kind == kType)
      {
        // Conversion.
        if (node.args ()->Size () != 1)
          {
            error_at_line (-1, 0, node.location.File.c_str (),
                           node.location.Line,
                           "conversion requires exactly one argument (E153)");
          }
        require_value_or_variable (node.args ()->At (0));

        const Type::Type* to = node.expr ()->type;
        const Type::Type*& from = node.args ()->At (0)->type;
        value_t& x = node.args ()->At (0)->value;

        if (x.present)
          {
            if (x.representable (from, to))
              {
                unimplemented;
              }
            else if (is_typed_float (from) && is_typed_float (to))
              {
                unimplemented;
              }
            else if (is_typed_integer (from) && is_any_string (to))
              {
                unimplemented;
                node.reset_mutability = true;
              }
            else if (is_any_string (from) && is_slice_of_bytes (to))
              {
                if (from->IsUntyped ())
                  {
                    x.convert (from, from->DefaultType ());
                    from = from->DefaultType ();
                  }
                node.reset_mutability = true;
              }
            else
              {
                error_at_line (-1, 0, node.location.File.c_str (),
                               node.location.Line,
                               "illegal conversion (E156)");
              }
          }
        else
          {
            if (assignable (from, x, to))
              {
                // Okay.
              }
            else if (Identical (from->UnderlyingType (), to->UnderlyingType ()))
              {
                // Okay.
              }
            else if (from->Level () == Type::Type::UNNAMED &&
                     to->Level () == Type::Type::UNNAMED &&
                     from->underlying_kind () == kPointer &&
                     to->underlying_kind () == kPointer &&
                     Identical (from->pointer_base_type (), to->pointer_base_type ()))
              {
                // Okay.
              }
            else if ((is_typed_integer (from) || is_typed_float (from)) &&
                     (is_typed_integer (to) || is_typed_float (from)))
              {
                // Okay.
              }
            else if (is_typed_complex (from) && is_typed_complex (to))
              {
                // Okay.
              }
            else if ((is_typed_integer (from) || is_slice_of_bytes (from) || is_slice_of_runes (from)) && is_typed_string (to))
              {
                // Okay.
                node.reset_mutability = true;
              }
            else if (is_typed_string (from) && (is_slice_of_bytes (to) || is_slice_of_runes (to)))
              {
                // Okay.
                node.reset_mutability = true;
              }
            else
              {
                error_at_line (-1, 0, node.location.File.c_str (),
                               node.location.Line,
                               "illegal conversion (E122)");
              }
          }

        node.type = to;
        node.expression_kind = kValue;
        return;
      }

    if (node.expr ()->temp != NULL)
      {
        node.callable = node.expr ()->temp->instantiate (argument_types);
        node.expr ()->type = node.callable->type ();
      }
    else
      {
        node.callable = node.expr ()->callable;
      }

    node.function_type = type_cast<Type::Function> (node.expr ()->type);
    node.method_type = type_cast<Type::Method> (node.expr ()->type);

    if (node.function_type)
      {
        node.signature = node.function_type->GetSignature ();
        node.return_parameter = node.function_type->GetReturnParameter ();
      }
    else if (node.method_type)
      {
        node.signature = node.method_type->signature;
        node.return_parameter = node.method_type->return_parameter;
      }
    else
      {
        not_reached;
      }

    if (node.callable != NULL)
      {
        require_value_or_variable (node.expr ());
        // TODO:  Merge check_references into check_types.
        node.callable->check_types (args);
        node.callable->check_references (node.args ());
      }
    else
      {
        node.field = node.expr ()->field;
        check_types_arguments (args, node.signature);
        require_value_or_variable (node.expr ());
        require_value_or_variable_list (node.args ());
      }

    node.type = node.return_parameter->type;
    node.expression_kind = kValue;
  }

  void visit (ast_list_expr_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (ast_literal_expr_t& node)
  {
    assert (node.value.present);
    node.expression_kind = kValue;
  }

  void visit (ast_identifier_expr_t& node)
  {
    Node *identifier_node = node.child ();
    const std::string& identifier = ast_get_identifier (identifier_node);
    node.symbol = node.FindGlobalSymbol (identifier);
    if (node.symbol == NULL)
      {
        error_at_line (-1, 0, identifier_node->location.File.c_str (),
                       identifier_node->location.Line, "%s is not defined (E214)",
                       identifier.c_str ());
      }

    struct visitor : public ConstSymbolVisitor
    {
      ast_identifier_expr_t& node;

      visitor (ast_identifier_expr_t& n)
        : node (n)
      { }

      void defaultAction (const Symbol& symbol)
      {
        not_reached;
      }

      void visit (const BuiltinFunction& symbol)
      {
        node.type = symbol.type ();
        node.callable = &symbol;
        node.expression_kind = kValue;
      }

      void visit (const ::Template& symbol)
      {
        node.type = symbol.type ();
        node.temp = &symbol;
        node.expression_kind = kValue;
      }

      void visit (const ::Function& symbol)
      {
        node.type = symbol.type ();
        node.callable = &symbol;
        node.expression_kind = kValue;
      }

      void visit (const ParameterSymbol& symbol)
      {
        node.type = symbol.type;
        node.expression_kind = kVariable;
      }

      void visit (const TypeSymbol& symbol)
      {
        node.type = symbol.type;
        node.expression_kind = kType;
      }

      void visit (const ConstantSymbol& symbol)
      {
        node.type = symbol.type;
        node.value = symbol.value;
        assert (node.value.present);
        node.expression_kind = kValue;
      }

      void visit (const VariableSymbol& symbol)
      {
        node.type = symbol.type;
        node.expression_kind = kVariable;
      }

      void visit (const HiddenSymbol& symbol)
      {
        error_at_line (-1, 0, node.location.File.c_str (),
                       node.location.Line, "%s is not accessible in this scope (E157)",
                       symbol.identifier.c_str ());
      }
    };
    visitor v (node);
    node.symbol->accept (v);
  }

  void visit (ast_unary_arithmetic_expr_t& node)
  {
    node.VisitChildren (*this);
    switch (node.arithmetic)
      {
      case LogicNot:
      {
        if (!(is_any_boolean (node.child ()->type)))
          {
            error_at_line (-1, 0, node.location.File.c_str (),
                           node.location.Line,
                           "! cannot be applied to %s (E158)",
                           node.child ()->type->ToString ().c_str ());
          }
        node.type = node.child ()->type;
        if (node.child ()->value.present)
          {
            node.value.present = true;
            switch (node.type->underlying_kind ())
              {
              case kBool:
                node.value.bool_value_ = !node.child ()->value.bool_value_;
                break;
              case kBoolean:
                node.value.boolean_value_ = !node.child ()->value.boolean_value_;
                break;
              default:
                not_reached;
              }
          }
      }
      break;
      case Negate:
      {
        if (!(is_typed_numeric (node.child ()->type) ||
              is_untyped_numeric (node.child ()->type)))
          {
            error_at_line (-1, 0, node.location.File.c_str (),
                           node.location.Line,
                           "- cannot be applied to %s (E159)",
                           node.child ()->type->ToString ().c_str ());
          }
        node.type = node.child ()->type;
        if (node.child ()->value.present)
          {
            node.value.present = true;
            switch (node.type->underlying_kind ())
              {
              case kUint8:
                node.value.uint8_value_ = -node.child ()->value.uint8_value_;
                break;
              case kUint16:
                node.value.uint16_value_ = -node.child ()->value.uint16_value_;
                break;
              case kUint32:
                node.value.uint32_value_ = -node.child ()->value.uint32_value_;
                break;
              case kUint64:
                node.value.uint32_value_ = -node.child ()->value.uint32_value_;
                break;
              case kInt8:
                node.value.int8_value_ = -node.child ()->value.int8_value_;
                break;
              case kInt16:
                node.value.int16_value_ = -node.child ()->value.int16_value_;
                break;
              case kInt32:
                node.value.int32_value_ = -node.child ()->value.int32_value_;
                break;
              case kInt64:
                node.value.int64_value_ = -node.child ()->value.int64_value_;
                break;
              case kFloat32:
                node.value.float32_value_ = -node.child ()->value.float32_value_;
                break;
              case kFloat64:
                node.value.float64_value_ = -node.child ()->value.float64_value_;
                break;
              case kComplex64:
                node.value.complex64_value_ = -node.child ()->value.complex64_value_;
                break;
              case kComplex128:
                node.value.complex128_value_ = -node.child ()->value.complex128_value_;
                break;
              case kUint:
                node.value.uint_value_ = -node.child ()->value.uint_value_;
                break;
              case kInt:
                node.value.int_value_ = -node.child ()->value.int_value_;
                break;
              case kUintptr:
                node.value.uintptr_value_ = -node.child ()->value.uintptr_value_;
                break;
              case kRune:
                node.value.rune_value_ = -node.child ()->value.rune_value_;
                break;
              case kInteger:
                node.value.integer_value_ = -node.child ()->value.integer_value_;
                break;
              case kFloat:
                node.value.float_value_ = -node.child ()->value.float_value_;
                break;
              case kComplex:
                node.value.complex_value_ = -node.child ()->value.complex_value_;
                break;
              default:
                not_reached;
              }
          }
      }
      break;
      }

    require_value_or_variable (node.child ());
    node.expression_kind = kValue;
  }

  void visit (ast_binary_arithmetic_expr_t& node)
  {
    node.VisitChildren (*this);
    switch (node.arithmetic)
      {
      case Multiply:
        process_arithmetic ("*", node, multiply);
        break;
      case Divide:
        process_arithmetic ("/", node, divide);
        break;
      case Modulus:
        process_integral ("%", node, modulus);
        break;
      case LeftShift:
        process_shift<LeftShifter> (node);
        break;
      case RightShift:
        process_shift<RightShifter> (node);
        break;
      case BitAnd:
        process_integral ("&", node, bit_and);
        break;
      case BitAndNot:
        process_integral ("&^", node, bit_and_not);
        break;
      case Add:
        process_arithmetic ("+", node, add);
        break;
      case Subtract:
        process_arithmetic ("-", node, subtract);
        break;
      case BitOr:
        process_integral ("|", node, bit_or);
        break;
      case BitXor:
        process_integral ("^", node, bit_xor);
        break;
      case Equal:
        process_comparable ("==", node, equal);
        break;
      case NotEqual:
        process_comparable ("!=", node, not_equal);
        break;
      case LessThan:
        process_orderable ("<", node, less_than);
        break;
      case LessEqual:
        process_orderable ("<=", node, less_equal);
        break;
      case MoreThan:
        process_orderable (">", node, more_than);
        break;
      case MoreEqual:
        process_orderable (">=", node, more_equal);
        break;
      case LogicOr:
        process_logic_or (node);
        break;
      case LogicAnd:
        process_logic_and (node);
        break;
      }

    require_value_or_variable (node.left ());
    require_value_or_variable (node.left ());
    node.expression_kind = kValue;
  }

  void visit (SourceFile& node)
  {
    node.VisitChildren (*this);
  }

  void visit (ast::Type& node)
  {
    // Do nothing.
  }

  void visit (ast_instance_t& node)
  {
    // Check the arguments.
    node.expression_list ()->Accept (*this);
    check_types_arguments (node.expression_list (), node.symbol->initializer->initializerType->signature);
    require_value_or_variable_list (node.expression_list ());
  }

  void visit (ast_initializer_t& node)
  {
    receiver_parameter = node.initializer->initializerType->receiver_parameter;
    node.body ()->Accept (*this);
  }

  void visit (ast_getter_t& node)
  {
    receiver_parameter = node.getter->getterType->receiver_parameter;
    node.body ()->Accept (*this);
  }

  void visit (ast_action_t& node)
  {
    receiver_parameter = node.receiver_symbol;
    node.precondition ()->Accept (*this);
    check_condition (node.precondition ());
    node.body ()->Accept (*this);
    node.action->precondition = node.precondition ();

    if (node.precondition ()->value.present)
      {
        if (node.precondition ()->value.bool_value_)
          {
            node.action->precondition_kind = decl::Action::StaticTrue;
          }
        else
          {
            node.action->precondition_kind = decl::Action::StaticFalse;
          }
      }
  }

  void visit (ast_dimensioned_action_t& node)
  {
    receiver_parameter = node.receiver_symbol;
    node.precondition ()->Accept (*this);
    check_condition (node.precondition ());
    node.body ()->Accept (*this);
    node.action->precondition = node.precondition ();

    if (node.precondition ()->value.present)
      {
        if (node.precondition ()->value.bool_value_)
          {
            node.action->precondition_kind = decl::Action::StaticTrue;
          }
        else
          {
            node.action->precondition_kind = decl::Action::StaticFalse;
          }
      }
  }

  void visit (ast_reaction_t& node)
  {
    receiver_parameter = node.reaction->reaction_type->receiver_parameter;
    node.body ()->Accept (*this);
  }

  void visit (ast_dimensioned_reaction_t& node)
  {
    receiver_parameter = node.reaction->reaction_type->receiver_parameter;
    node.body ()->Accept (*this);
  }

  void visit (ast_bind_t& node)
  {
    node.body ()->Accept (*this);
  }

  void visit (ast_function_t& node)
  {
    node.body ()->Accept (*this);
  }

  void visit (ast_method_t& node)
  {
    receiver_parameter = node.method->methodType->receiver_parameter;
    node.body ()->Accept (*this);
  }

  void visit (ast_list_statement_t& node)
  {
    node.VisitChildren (*this);
  }

  void visit (ast_expression_statement_t& node)
  {
    node.VisitChildren (*this);
    require_value_or_variable (node.child ());
  }

  void visit (ast_return_statement_t& node)
  {
    // Check the expression.
    node.VisitChildren (*this);

    // Get the return symbol.
    node.return_symbol = SymbolCast<ParameterSymbol> (node.FindGlobalSymbol (ReturnSymbol));
    assert (node.return_symbol != NULL);

    if (!assignable (node.child ()->type, node.child ()->value, node.return_symbol->type))
      {
        error_at_line (-1, 0, node.location.File.c_str (),
                       node.location.Line, "cannot convert %s to %s in return (E160)",
                       node.child ()->type->ToString ().c_str (), node.return_symbol->type->ToString ().c_str ());
      }
    convert (node.child (), node.return_symbol->type);

    require_value_or_variable (node.child ());
  }

  void visit (ast_if_statement_t& node)
  {
    node.VisitChildren (*this);
    check_condition (node.condition ());
  }

  void visit (ast_while_statement_t& node)
  {
    node.VisitChildren (*this);
    check_condition (node.condition ());
  }

  void visit (ast_for_iota_statement_t& node)
  {
    const std::string& identifier = ast_get_identifier (node.identifier ());
    node.limit = process_array_dimension (node.limit_node ());
    node.symbol = new VariableSymbol (identifier, node.identifier (), Int::Instance (), IMMUTABLE, IMMUTABLE);
    enter_symbol (node, node.symbol);
    node.body ()->Accept (*this);
  }

  void visit (ast_change_statement_t& node)
  {
    node.expr ()->Accept (*this);

    const Type::Type* root_type = type_change (node.expr ()->type);
    if (root_type == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot change expression of type %s (E96)", node.expr ()->type->ToString ().c_str ());
      }

    require_value_or_variable (node.expr ());

    // Enter all parameters and variables in scope that are pointers as pointers to foreign.
    node.Change ();

    // Enter the new heap root.
    const std::string& identifier = ast_get_identifier (node.identifier ());
    // Don't know dereference mutability yet.
    VariableSymbol* symbol = new VariableSymbol (identifier, &node, root_type, IMMUTABLE, FOREIGN);
    node.root_symbol = enter_symbol (node, symbol);

    // Check the body.
    node.body ()->Accept (*this);
  }

  void visit (ast_activate_statement_t& node)
  {
    // Check the activations.
    node.expr_list ()->Accept (*this);
    // Re-insert this as a pointer to mutable.
    node.Activate ();
    // Check the body.
    node.body ()->Accept (*this);
  }

  void visit (ast_const_t& node)
  {
    if (!node.done)
      {
        process_types_and_constants (&node);
      }
  }

  void visit (ast_empty_statement_t& node)
  {
    // Do nothing.
  }

  void visit (ast_var_statement_t& node)
  {
    ast::Node* identifier_list = node.identifier_list ();
    ast::Node* type_spec = node.type_spec ();
    ast::Node* expression_list = node.expression_list ();

    if (expression_list->Size () != 0 &&
        identifier_list->Size () != expression_list->Size ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "wrong number of initializers (E184)");
      }

    // Process the type spec.
    const Type::Type* type = process_type (type_spec, true);

    if (expression_list->Size () == 0)
      {
        // Type, no expressions.

        if (type_cast<Void> (type) != NULL)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "missing type (E183)");

          }

        // Enter each symbol.
        for (Node::Iterator id_pos = identifier_list->Begin (),
             id_limit = identifier_list->End ();
             id_pos != id_limit;
             ++id_pos)
          {
            const std::string& name = ast_get_identifier (*id_pos);
            VariableSymbol* symbol = new VariableSymbol (name, *id_pos, type, node.mutability, node.dereferenceMutability);
            node.symbols.push_back (enter_symbol (*node.GetParent (), symbol));
          }

        return;
      }

    if (type_cast<Void> (type) == NULL)
      {
        // Type, expressions.

        // Enter each symbol.
        for (Node::Iterator id_pos = identifier_list->Begin (),
             id_limit = identifier_list->End (),
             init_pos = expression_list->Begin ();
             id_pos != id_limit;
             ++id_pos, ++init_pos)
          {
            Node* n = *init_pos;
            n->Accept (*this);

            if (!assignable (n->type, n->value, type))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "cannot assign %s to %s in initialization (E62)", n->type->ToString ().c_str (), type->ToString ().c_str ());
              }
            convert (n, type);
            require_value_or_variable (n);

            const std::string& name = ast_get_identifier (*id_pos);
            VariableSymbol* symbol = new VariableSymbol (name, *id_pos, type, node.intrinsic_mutability, node.dereferenceMutability);
            node.symbols.push_back (enter_symbol (*node.GetParent (), symbol));
          }

        return;
      }

    // No type, expressions.

    // Enter each symbol.
    for (Node::Iterator id_pos = identifier_list->Begin (),
         id_limit = identifier_list->End (),
         init_pos = expression_list->Begin ();
         id_pos != id_limit;
         ++id_pos, ++init_pos)
      {
        Node* n = *init_pos;
        n->Accept (*this);

        if (n->type->IsUntyped ())
          {
            n->value.convert (n->type, n->type->DefaultType ());
            n->type = n->type->DefaultType ();
          }
        require_value_or_variable (n);

        const std::string& name = ast_get_identifier (*id_pos);
        VariableSymbol* symbol = new VariableSymbol (name, *id_pos, n->type, node.intrinsic_mutability, node.dereferenceMutability);
        node.symbols.push_back (enter_symbol (*node.GetParent (), symbol));
      }
  }

  void visit (ast_assign_statement_t& node)
  {
    node.VisitChildren (*this);
    const Type::Type* to = node.left ()->type;
    const Type::Type*& from = node.right ()->type;
    value_t& val = node.right ()->value;
    if (!assignable (from, val, to))
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot assign value of type %s to variable of type %s (E199)",
                       from->ToString ().c_str (),
                       to->ToString ().c_str ());
      }
    convert (node.right (), to);
    require_variable (node.left ());
    require_value_or_variable (node.right ());
  }

  void visit (ast_add_assign_statement_t& node)
  {
    node.VisitChildren (*this);

    const Type::Type* to = node.left ()->type;
    const Type::Type*& from = node.right ()->type;
    value_t& val = node.right ()->value;
    if (!arithmetic (to))
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "+= cannot be applied to %s (E200)",
                       to->ToString ().c_str ());
      }
    if (!assignable (from, val, to))
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot assign value of type %s to variable of type %s (E76)",
                       from->ToString ().c_str (),
                       to->ToString ().c_str ());
      }
    convert (node.right (), to);
    require_variable (node.left ());
    require_value_or_variable (node.right ());
  }

  void visit (ast_increment_statement_t& node)
  {
    node.VisitChildren (*this);
    if (!arithmetic (node.child ()->type))
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "++ cannot be applied to %s (E77)",
                       node.child ()->type->ToString ().c_str ());
      }
    require_variable (node.child ());
  }

  void visit (ast_bind_push_port_statement_t& node)
  {
    node.VisitChildren (*this);
    bind (node, node.left (), node.right_ref ());
  }

  void visit (ast_bind_push_port_param_statement_t& node)
  {
    node.VisitChildren (*this);
    const reaction_t* reaction = bind (node, node.left (), node.right_ref ());
    if (!reaction->has_dimension ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "parameter specified for non-parameterized reaction (E41)");
      }
    Type::Int::ValueType dimension = reaction->dimension ();
    check_array_index (reaction->reaction_type->GetArray (dimension), node.param (), false);
  }

  void visit (ast_bind_pull_port_statement_t& node)
  {
    node.VisitChildren (*this);

    const Type::Function* pull_port_type = type_cast<Type::Function> (node.left ()->type);

    if (pull_port_type == NULL || pull_port_type->function_kind != Type::Function::PULL_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "target of bind is not a pull port (E193)");
      }

    require_variable (node.left ());

    const Type::Method* getter_type = type_cast<Type::Method> (node.right ()->type);

    if (getter_type == NULL || getter_type->method_kind != Type::Method::GETTER)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "source of bind is not a getter (E192)");
      }

    require_variable (node.right ());

    Type::Function g (Type::Function::FUNCTION, getter_type->signature, getter_type->return_parameter);
    if (!type_is_equal (pull_port_type, &g))
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot bind %s to %s (E191)", pull_port_type->ToString ().c_str (), getter_type->ToString ().c_str ());
      }
  }

  void visit (ast_dereference_expr_t& node)
  {
    node.VisitChildren (*this);
    const Type::Type* t = node.child ()->type;
    const Pointer* p = type_cast<Pointer> (t->UnderlyingType ());
    if (p == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "* cannot be applied to %s (E21)", t->ToString ().c_str ());
      }
    require_value_or_variable (node.child ());
    node.type = p->Base ();
    node.expression_kind = kVariable;
  }

  void visit (ast_address_of_expr_t& node)
  {
    node.VisitChildren (*this);
    require_variable (node.child ());
    node.type = node.child ()->type->GetPointer ();
    node.expression_kind = kValue;
  }

  void visit (ast_select_expr_t& node)
  {
    const std::string& identifier = ast_get_identifier (node.identifier ());
    node.base ()->Accept (*this);
    const Type::Type* base_type = node.base ()->type;
    require_value_or_variable (node.base ());

    if (type_dereference (base_type))
      {
        // Selecting through pointer always yields a variable.
        node.expression_kind = kVariable;
      }
    else
      {
        // Otherwise, use the base kind.
        node.expression_kind = node.base ()->expression_kind;
      }

    node.field = base_type->select_field (identifier);
    node.callable = base_type->select_callable (identifier);
    if (node.field != NULL)
      {
        node.type = node.field->type;
      }
    else if (node.callable != NULL)
      {
        node.type = node.callable->type ();
        return;
      }
    else
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot select %s from expression of type %s (E154)",
                       identifier.c_str (), base_type->ToString ().c_str ());
      }
  }

  void check_array_index (const Array* array_type, Node* index, bool allow_equal)
  {
    const Type::Type*& index_type = index->type;
    value_t& index_value = index->value;

    if (is_untyped_numeric (index_type))
      {
        if (!index_value.representable (index_type, Int::Instance ()))
          {
            error_at_line (-1, 0, index->location.File.c_str (), index->location.Line,
                           "array index is not an integer (E20)");
          }
        index_value.convert (index_type, Int::Instance ());
        index_type = Int::Instance ();
        Int::ValueType idx = index_value.int_value_;
        if (idx < 0)
          {
            error_at_line (-1, 0, index->location.File.c_str (), index->location.Line,
                           "array index is negative (E162)");
          }
        if ((!allow_equal && idx >= array_type->dimension) ||
            (allow_equal && idx > array_type->dimension))
          {
            error_at_line (-1, 0, index->location.File.c_str (), index->location.Line,
                           "array index is out of bounds (E163)");
          }
      }
    else if (is_typed_integer (index_type))
      {
        if (index_value.present)
          {
            Int::ValueType idx = index_value.to_int (index_type);
            if (idx < 0)
              {
                error_at_line (-1, 0, index->location.File.c_str (), index->location.Line,
                               "array index is negative (E164)");
              }
            if (idx >= array_type->dimension)
              {
                error_at_line (-1, 0, index->location.File.c_str (), index->location.Line,
                               "array index is out of bounds (E165)");
              }
          }
      }
    else
      {
        error_at_line (-1, 0, index->location.File.c_str (), index->location.Line,
                       "array index is not an integer (E203)");
      }
    require_value_or_variable (index);
  }

  void visit (ast_index_expr_t& node)
  {
    node.VisitChildren (*this);
    const Type::Type* base_type = node.base ()->type;
    Node* index_node = node.index ();
    value_t& index_value = index_node->value;
    const Type::Type*& index_type = node.index ()->type;

    node.array_type = type_cast<Array> (base_type->UnderlyingType ());
    if (node.array_type != NULL)
      {
        check_array_index (node.array_type, node.index (), false);
        require_value_or_variable (node.base ());
        node.type = node.array_type->Base ();
        node.expression_kind = node.base ()->expression_kind;
        return;
      }

    node.slice_type = type_cast<Slice> (base_type->UnderlyingType ());
    if (node.slice_type != NULL)
      {
        if (is_untyped_numeric (index_type))
          {
            if (!index_value.representable (index_type, Int::Instance ()))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "slice index is not an integer (E204)");
              }
            index_value.convert (index_type, Int::Instance ());
            index_type = Int::Instance ();
            Int::ValueType idx = index_value.int_value_;
            if (idx < 0)
              {
                error_at_line (-1, 0, index_node->location.File.c_str (), index_node->location.Line,
                               "slice index is negative (E166)");
              }
          }
        else if (is_typed_integer (index_type))
          {
            if (index_value.present)
              {
                Int::ValueType idx = index_value.to_int (index_type);
                if (idx < 0)
                  {
                    error_at_line (-1, 0, index_node->location.File.c_str (), index_node->location.Line,
                                   "slice index is negative (E167)");
                  }
              }
          }
        else
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "slice index is not an integer (E205)");
          }

        require_value_or_variable (node.base ());
        require_value_or_variable (node.index ());

        node.type = node.slice_type->Base ();
        node.expression_kind = kVariable;
        return;
      }

    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                   "cannot index expression of type %s (E168)",
                   base_type->ToString ().c_str ());
  }

  void visit (ast_slice_expr_t& node)
  {
    node.VisitChildren (*this);
    Node* base = node.base ();
    const Type::Type* base_type = node.base ()->type;
    Node* low_node = node.low ();
    const Type::Type*& low_type = low_node->type;
    value_t& low_value = low_node->value;
    Node* high_node = node.high ();
    const Type::Type*& high_type = high_node->type;
    value_t& high_value = high_node->value;

    node.array_type = type_cast<Array> (base_type->UnderlyingType ());
    if (node.array_type != NULL)
      {
        check_array_index (node.array_type, low_node, true);
        check_array_index (node.array_type, high_node, true);
        if (low_value.present && high_value.present)
          {
            if (!(low_value.to_int (low_type) <= high_value.to_int (high_type)))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "slice indices are out of range (E224)");
              }
          }
        require_value_or_variable (base);
        require_value_or_variable (low_node);
        require_value_or_variable (high_node);
        node.type = node.array_type->Base ()->GetSlice ();
        node.expression_kind = kValue;
        return;
      }

    node.slice_type = type_cast<Slice> (base_type->UnderlyingType ());
    if (node.slice_type != NULL)
      {
        unimplemented;
        return;
      }

    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                   "cannot slice expression of type %s (E225)",
                   base_type->ToString ().c_str ());

    //     typed_value_t base_tv = CheckExpectReference (node.base_ref ());
    //     typed_value_t low_tv = CheckAndImplicitlyDereference (node.low_ref ());
    //     typed_value_t high_tv = CheckAndImplicitlyDereference (node.high_ref ());
    //     typed_value_t result = typed_value_t::slice (node.location, base_tv, low_tv, high_tv);
    //     if (result.IsError ())
    //       {
    //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                        "incompatible types (%s)[%s : %s] (E36)",
    //                        base_tv.type->ToString ().c_str (),
    //                        low_tv.type->ToString ().c_str (),
    //                        high_tv.type->ToString ().c_str ());
    //       }
    //     node.typed_value = result;
  }

  void visit (TypeExpression& node)
  {
    node.type = process_type (node.type_spec (), true);
    node.expression_kind = kType;
  }

  void visit (ast_push_port_call_expr_t& node)
  {
    node.receiver_parameter = receiver_parameter;
    const std::string& port_identifier = ast_get_identifier (node.identifier ());
    const Type::Type* this_type = receiver_parameter->type;
    node.field = this_type->select_field (port_identifier);
    if (node.field == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E194)", port_identifier.c_str ());
      }
    const Type::Function* push_port_type = Type::type_cast<Type::Function> (node.field->type);
    if (push_port_type == NULL || push_port_type->function_kind != Type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E195)", port_identifier.c_str ());
      }

    node.args ()->Accept (*this);
    check_types_arguments (node.args (), push_port_type->GetSignature ());
    require_value_or_variable_list (node.args ());
  }

  void visit (ast_indexed_port_call_expr_t& node)
  {
    node.receiver_parameter = receiver_parameter;
    const std::string& port_identifier = ast_get_identifier (node.identifier ());
    const Type::Type* this_type = receiver_parameter->type;
    node.field = this_type->select_field (port_identifier);
    if (node.field == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E74)", port_identifier.c_str ());
      }
    node.array_type = Type::type_cast<Type::Array> (node.field->type);
    if (node.array_type == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "%s is not an array of ports (E16)", port_identifier.c_str ());
      }
    const Type::Function* push_port_type = Type::type_cast<Type::Function> (node.array_type->Base ());
    if (push_port_type == NULL || push_port_type->function_kind != Type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "%s is not an array of ports (E17)", port_identifier.c_str ());
      }

    node.index ()->Accept (*this);
    check_array_index (node.array_type, node.index (), false);

    node.args ()->Accept (*this);
    check_types_arguments (node.args (), push_port_type->GetSignature ());
    require_value_or_variable_list (node.args ());
  }

};
}

void check_types_arguments (ast::Node* node, const Type::Signature* signature)
{
  ast_list_expr_t* args = ast_cast<ast_list_expr_t> (node);

  if (args->Size () != signature->Arity ())
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "call expects %lu arguments but given %lu (E150)", signature->Arity (), args->Size ());
    }

  size_t i = 0;
  for (Node::ConstIterator pos = args->Begin (), limit = args->End ();
       pos != limit;
       ++pos, ++i)
    {
      const Type::Type*& arg = (*pos)->type;
      value_t& val = (*pos)->value;
      const Type::Type* param = signature->At (i)->type;
      if (!Type::assignable (arg, val, param))
        {
          error_at_line (-1, 0, (*pos)->location.File.c_str (), (*pos)->location.Line,
                         "cannot assign %s to %s in call (E151)", arg->ToString ().c_str (), param->ToString ().c_str ());
        }
      convert ((*pos), param);
    }
}

void require_type (const Node* node)
{
  if (!(node->expression_kind == kType))
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "required a type (E221)");
    }
}

void require_value_or_variable_list (const Node* node)
{
  for (Node::ConstIterator pos = node->Begin (), limit = node->End ();
       pos != limit;
       ++pos)
    {
      require_value_or_variable (*pos);
    }
}

void check_types (ast::Node* root)
{
  Visitor visitor;
  root->Accept (visitor);
}
}
