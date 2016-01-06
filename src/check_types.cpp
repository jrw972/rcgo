#include "check_types.hpp"

#include <error.h>

#include "ast.hpp"
#include "ast_visitor.hpp"
#include "ast_cast.hpp"
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
#include "bind.hpp"

namespace semantic
{

using namespace ast;
using namespace type;
using namespace decl;

namespace
{

/*
  A reference has:
  - a type
  - a region
  - an intrinsic mutability
  - a dereference mutability
  - the value of the data to which this reference refers (possibly)

  A value has:
  - a type
  - an intrinsic mutability
  - a dereference mutability
  - an actual value (possibly)

  The four operations that contributed to the "const correctness" of
  the language are implicit dereference, dereference (*), address of
  (&), and assignment (=).

  Implicit Dereference
  --------------------

  This operation converts a reference to a value.  The type,
  intrinsic mutability, dereference mutability, and actual value of
  the reference are passed on to the value.

  | REFERENCE | VALUE |
  -----------------------+-----------+-------+
  Intrinsic Mutability |         A |     A |
  Dereference Mutability |         B |     B |


  Dereference (*)
  ---------------

  This operation converts a value to a reference and must be applied
  to a pointer.  The region of the reference is assumed to be the
  heap.  (This is points-to analysis which is undecidable in the
  general case.)  The intrinsic mutability and dereference
  mutability of the reference is determined by dereference
  mutability of the value.  The only actual values for a value with
  pointer type is nil but this could be improved.  Similarly, it is
  not defined how the actual value passes to the reference.

  | VALUE | REFERENCE |
  -----------------------+-------+-----------+
  Intrinsic Mutability |     A |         B |
  Dereference Mutability |     B |         B |


  Address Of (&)
  --------------

  This operation converts a reference to a pointer value.  The type
  and mutability of the pointer is determined by the mutability of
  the reference.

  | REFERENCE | VALUE |
  -----------------------+-----------+-------+
  Intrinsic Mutability |         A |     A |
  Dereference Mutability |         B |     B |

  Assignment (=)
  --------------

  Assignment must check if a value can be copied to the location
  indicated by a reference.  The two simple checks are (1) the types
  must be equivalent and (2) the reference should be mutable.  (All
  references to constant data should be immutable.)

  If the type being assigned does not contain a pointer, then no
  further checks are necessary as new references cannot be generated
  from the value.  However, if the type being assigned does contain
  a pointer, then we must be careful to ensure that any generated
  reference has the correct mutability.  To this end, we check the
  dereference mutability of the reference with the intrinsic
  mutability of the value.

  MUTABLE | IMMUTABLE | FOREIGN (VALUE)
  +---------+-----------+--------
  MUTABLE     |   OK    |     NO    |    NO
  IMMUTABLE   |   OK    |     OK    |    NO
  FOREIGN     |   OK    |     OK    |    OK
  (REFERENCE)

  If the value has FOREIGN dereference mutability then we require that
  the reference refer to the STACK.

  (This check is currently checked externally.)

  Port Calls and Getters
  ----------------------

  Getters (component methods) can only be defined on immutable
  components and can only be called in the immutable phase.  There
  is no restriction on the values going in but the return values
  must have FOREIGN intrinsic mutability.  This allows a component
  to return pointers but prevents them from being saved in the heap.

  Parameters to ports and reactions must have FOREIGN dereference
  mutability if the associated type contains a pointer.
*/

static void fix (Node& node)
{
  if (node.type->underlying_kind () == type::kStringU)
    {
      node.indirection_mutability = std::max (node.indirection_mutability, Immutable);
    }
}

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

static void convert (ast::Node* node, const type::Type* type)
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
  const type::Function* push_port_type = type::type_cast<type::Function> (port_node->type);

  if (push_port_type == NULL || push_port_type->function_kind != type::Function::PUSH_PORT)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "source of bind is not a port (E38)");
    }

  require_variable (port_node);

  const type::Method* reaction_type = type::type_cast<type::Method> (reaction_node->type);
  if (reaction_type == NULL || reaction_type->method_kind != type::Method::REACTION)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "target of bind is not a reaction (E39)");
    }

  require_variable (reaction_node);

  if (!identical (push_port_type->GetSignature (), reaction_type->signature))
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "cannot bind %s to %s (E40)", push_port_type->ToString ().c_str (), reaction_type->ToString ().c_str ());
    }

  return static_cast<const reaction_t*> (reaction_node->callable);
}

static void check_condition (Node* node)
{
  const type::Type* condition = node->type;
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
static const type::Type* comparable (const type::Type* left_type, const type::Type* right_type)
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

static void process_comparable (const char* s, Binary& node, void (*func) (Value&, const type::Type*, const Value&, const Value&))
{
  const type::Type*& left_type = node.left->type;
  Value& left_value = node.left->value;
  const type::Type*& right_type = node.right->type;
  Value& right_value = node.right->value;
  const type::Type*& type = node.type;
  Value& value = node.value;

  if (left_type->IsUntyped () &&
      right_type->IsUntyped ())
    {
      const type::Type* t = comparable (left_type, right_type);
      if (t == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s cannot be applied to %s and %s (E206)",
                         s,
                         left_type->ToString ().c_str (),
                         right_type->ToString ().c_str ());
        }
      convert (node.left, t);
      convert (node.right, t);
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

  const type::Type* in_type = Choose (left_type, right_type);
  convert (node.left, in_type);
  convert (node.right, in_type);

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
static const type::Type* orderable (const type::Type* left_type, const type::Type* right_type)
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

static void process_orderable (const char* s, Binary& node, void (*func) (Value&, const type::Type*, const Value&, const Value&))
{
  const type::Type*& left_type = node.left->type;
  Value& left_value = node.left->value;
  const type::Type*& right_type = node.right->type;
  Value& right_value = node.right->value;
  const type::Type*& type = node.type;
  Value& value = node.value;

  if (left_type->IsUntyped () &&
      right_type->IsUntyped ())
    {
      const type::Type* t = orderable (left_type, right_type);
      if (t == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s cannot be applied to %s and %s (E208)",
                         s,
                         left_type->ToString ().c_str (),
                         right_type->ToString ().c_str ());
        }
      convert (node.left, t);
      convert (node.right, t);
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

  const type::Type* in_type = Choose (left_type, right_type);
  convert (node.left, in_type);
  convert (node.right, in_type);

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
static const type::Type* arithmetic (const type::Type* left_type, const type::Type* right_type)
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

static void process_arithmetic (const char* s, Binary& node, void (*func) (Value&, const type::Type*, const Value&, const Value&))
{
  const type::Type*& left_type = node.left->type;
  Value& left_value = node.left->value;
  const type::Type*& right_type = node.right->type;
  Value& right_value = node.right->value;
  const type::Type*& type = node.type;
  Value& value = node.value;

  if (left_type->IsUntyped () &&
      right_type->IsUntyped ())
    {
      const type::Type* t = arithmetic (left_type, right_type);
      if (t == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s cannot be applied to %s and %s (E210)",
                         s,
                         left_type->ToString ().c_str (),
                         right_type->ToString ().c_str ());
        }
      convert (node.left, t);
      convert (node.right, t);
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

  const type::Type* in_type = Choose (left_type, right_type);
  convert (node.left, in_type);
  convert (node.right, in_type);

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
static const type::Type* integral (const type::Type* left_type, const type::Type* right_type)
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

static void process_integral (const char* s, Binary& node, void (*func) (Value&, const type::Type*, const Value&, const Value&))
{
  const type::Type*& left_type = node.left->type;
  Value& left_value = node.left->value;
  const type::Type*& right_type = node.right->type;
  Value& right_value = node.right->value;
  const type::Type*& type = node.type;
  Value& value = node.value;

  if (left_type->IsUntyped () &&
      right_type->IsUntyped ())
    {
      const type::Type* t = integral (left_type, right_type);
      if (t == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "%s cannot be applied to %s and %s (E212)",
                         s,
                         left_type->ToString ().c_str (),
                         right_type->ToString ().c_str ());
        }
      convert (node.left, t);
      convert (node.right, t);
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

  const type::Type* in_type = Choose (left_type, right_type);
  convert (node.left, in_type);
  convert (node.right, in_type);

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
static void process_shift (Binary& node)
{
  if (!(is_typed_unsigned_integer (node.right->type) || is_untyped_numeric (node.right->type)))
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s is not integral (E65)",
                     node.right->type->ToString ().c_str ());
    }

  if (node.right->value.present)
    {
      if (!node.right->value.representable (node.right->type, type::Uint::Instance ()))
        {
          error_at_line (-1, 0, node.location.File.c_str (),
                         node.location.Line,
                         "shift amount %s is not a uint (E90)",
                         node.right->type->ToString ().c_str ());
        }
      node.right->value.convert (node.right->type, type::Uint::Instance ());
      node.right->type = type::Uint::Instance ();
    }

  if (!integral (node.left->type))
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "%s is not integral (E125)",
                     node.left->type->ToString ().c_str ());
    }

  node.type = node.left->type;

  if (node.left->value.present &&
      node.right->value.present)
    {
      node.value.present = true;
      switch (node.type->underlying_kind ())
        {
        case kUint8:
          node.value.uint8_value = T () (node.left->value.uint8_value, node.right->value.uint_value);
          break;
        case kUint16:
          node.value.uint16_value = T () (node.left->value.uint16_value, node.right->value.uint_value);
          break;
        case kUint32:
          node.value.uint32_value = T () (node.left->value.uint32_value, node.right->value.uint_value);
          break;
        case kUint64:
          node.value.uint64_value = T () (node.left->value.uint64_value, node.right->value.uint_value);
          break;
        case kInt8:
          node.value.int8_value = T () (node.left->value.int8_value, node.right->value.uint_value);
          break;
        case kInt16:
          node.value.int16_value = T () (node.left->value.int16_value, node.right->value.uint_value);
          break;
        case kInt32:
          node.value.int32_value = T () (node.left->value.int32_value, node.right->value.uint_value);
          break;
        case kInt64:
          node.value.int64_value = T () (node.left->value.int64_value, node.right->value.uint_value);
          break;
        case kUint:
          node.value.uint_value = T () (node.left->value.uint_value, node.right->value.uint_value);
          break;
        case kInt:
          node.value.int_value = T () (node.left->value.int_value, node.right->value.uint_value);
          break;
        case kUintptr:
          node.value.uintptr_value = T () (node.left->value.uintptr_value, node.right->value.uint_value);
          break;
        case kRune:
          node.value.rune_value = T () (node.left->value.rune_value, node.right->value.uint_value);
          break;
        case kInteger:
          node.value.integer_value = T () (node.left->value.integer_value, node.right->value.uint_value);
          break;
        default:
          TYPE_NOT_REACHED (*node.type);
        }
    }
}

static void process_logic_or (Binary& node)
{
  const type::Type*& left_type = node.left->type;
  Value& left_value = node.left->value;
  const type::Type*& right_type = node.right->type;
  Value& right_value = node.right->value;
  const type::Type*& type = node.type;
  Value& value = node.value;

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
      value.boolean_value = left_value.boolean_value || right_value.boolean_value;
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

  const type::Type* in_type = Choose (left_type, right_type);
  convert (node.left, in_type);
  convert (node.right, in_type);

  if (in_type->underlying_kind () != kBool)
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "|| cannot be applied to %s and %s (E179)",
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  if (left_value.present &&
      left_value.bool_value)
    {
      type = Boolean::Instance ();
      value.boolean_value = true;
    }
  else if (right_value.present)
    {
      type = Boolean::Instance ();
      value.boolean_value = right_value.bool_value;
    }
  else
    {
      node.type = Bool::Instance ();
    }
}

static void process_logic_and (Binary& node)
{
  const type::Type*& left_type = node.left->type;
  Value& left_value = node.left->value;
  const type::Type*& right_type = node.right->type;
  Value& right_value = node.right->value;
  const type::Type*& type = node.type;
  Value& value = node.value;

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
      value.boolean_value = left_value.boolean_value && right_value.boolean_value;
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

  const type::Type* in_type = Choose (left_type, right_type);
  convert (node.left, in_type);
  convert (node.right, in_type);

  if (in_type->underlying_kind () != kBool)
    {
      error_at_line (-1, 0, node.location.File.c_str (),
                     node.location.Line,
                     "&& cannot be applied to %s and %s (E182)",
                     left_type->ToString ().c_str (),
                     right_type->ToString ().c_str ());
    }

  if (left_value.present &&
      !left_value.bool_value)
    {
      type = Boolean::Instance ();
      value.boolean_value = false;
    }
  else if (right_value.present)
    {
      type = Boolean::Instance ();
      value.boolean_value = right_value.bool_value;
    }
  else
    {
      node.type = Bool::Instance ();
    }
}

// Determine the type of an expression.
struct Visitor : public ast::DefaultVisitor
{
  enum Context
  {
    Other,
    Action,
    Reaction,
    Initializer,
    Getter,
  };

  decl::SymbolTable& symtab;
  ParameterSymbol* receiver_parameter;
  Context context;
  bool in_mutable_phase;

  Visitor (decl::SymbolTable& st)
    : symtab (st)
    , receiver_parameter (NULL)
    , context (Other)
    , in_mutable_phase (false)
  { }

  void default_action (Node& node)
  {
    AST_NOT_REACHED (node);
  }

  void visit (CallExpr& node)
  {
    node.visit_children (*this);

    if (node.expr->expression_kind == kType)
      {
        // Conversion.
        if (node.args->size () != 1)
          {
            error_at_line (-1, 0, node.location.File.c_str (),
                           node.location.Line,
                           "conversion requires one argument (E8)");

          }

        conversion (node, node.expr, node.args->at (0));
        return;
      }

    // Collect the argument types.
    TypeList argument_types;
    List* args = node.args;
    for (List::ConstIterator pos = args->begin (), limit = args->end ();
         pos != limit;
         ++pos)
      {
        argument_types.push_back ((*pos)->type);
      }

    if (node.expr->temp != NULL)
      {
        node.callable = node.expr->temp->instantiate (argument_types);
        node.expr->type = node.callable->type ();
      }
    else
      {
        node.callable = node.expr->callable;
      }

    node.function_type = type_cast<type::Function> (node.expr->type);
    node.method_type = type_cast<type::Method> (node.expr->type);

    if (node.function_type)
      {
        switch (node.function_type->function_kind)
          {
          case type::Function::FUNCTION:
            // No restrictions on caller.
            break;

          case type::Function::PUSH_PORT:
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "push ports cannot be called (E202)");
            break;

          case type::Function::PULL_PORT:
            // Must be called from either a getter, an action, or reaction.
            if (!(context == Getter ||
                  context == Action ||
                  context == Reaction))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "pull ports may only be called from a getter, an action, or a reaction (E201)");
              }
            if (in_mutable_phase)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "cannot call pull port in mutable section (E198)");
              }
            break;
          }

        node.signature = node.function_type->GetSignature ();
        node.return_parameter = node.function_type->GetReturnParameter ();
      }
    else if (node.method_type)
      {
        switch (node.method_type->method_kind)
          {
          case type::Method::METHOD:
            // No restrictions on caller.
            break;
          case type::Method::INITIALIZER:
            // Caller must be an initializer.
            if (context != Initializer)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "initializers may only be called from initializers (E197)");
              }
            break;
          case type::Method::GETTER:
          {
            // Must be called from either a getter, action, reaction, or initializer.
            if (!(context == Getter ||
                  context == Action ||
                  context == Reaction ||
                  context == Initializer))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "getters may only be called from a getter, an action, a reaction, or an initializer (E196)");
              }
            if (in_mutable_phase)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "cannot call getter in mutable section (E34)");
              }
          }
          break;
          case type::Method::REACTION:
          {
            UNIMPLEMENTED;
          }
          break;
          }

        node.signature = node.method_type->signature;
        node.return_parameter = node.method_type->return_parameter;
      }
    else
      {
        NOT_REACHED;
      }

    if (node.callable != NULL)
      {
        require_value_or_variable (node.expr);
        // TODO:  Merge check_references into check_types.
        node.callable->check_types (args);
        node.callable->check_references (node.args);
        node.callable->check_mutability (node.args);
      }
    else
      {
        node.field = node.expr->field;
        check_types_arguments (args, node.signature);
        require_value_or_variable (node.expr);
        require_value_or_variable_list (node.args);
        check_mutability_arguments (node.args, node.signature);
      }

    node.type = node.return_parameter->type;
    node.expression_kind = kValue;
    node.intrinsic_mutability = Immutable;
    node.indirection_mutability = node.return_parameter->dereference_mutability;
    fix (node);
  }

  void conversion (Node& node,
                   Node* type_expr,
                   Node* expr)
  {
    require_type (type_expr);
    require_value_or_variable (expr);

    const type::Type* to = type_expr->type;
    const type::Type*& from = expr->type;
    Value& x = expr->value;

    if (x.present)
      {
        if (x.representable (from, to))
          {
            UNIMPLEMENTED;
          }
        else if (is_typed_float (from) && is_typed_float (to))
          {
            UNIMPLEMENTED;
          }
        else if (is_typed_integer (from) && is_any_string (to))
          {
            UNIMPLEMENTED;
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
        else if (identical (from->UnderlyingType (), to->UnderlyingType ()))
          {
            // Okay.
          }
        else if (from->Level () == type::Type::UNNAMED &&
                 to->Level () == type::Type::UNNAMED &&
                 from->underlying_kind () == kPointer &&
                 to->underlying_kind () == kPointer &&
                 identical (from->pointer_base_type (), to->pointer_base_type ()))
          {
            // Okay.
          }
        else if ((is_typed_integer (from) || is_typed_float (from)) &&
                 (is_typed_integer (to) || is_typed_float (to)))
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
    node.intrinsic_mutability = Immutable;
    node.indirection_mutability = expr->indirection_mutability;
    if (node.reset_mutability)
      {
        node.indirection_mutability = Mutable;
      }
    fix (node);
  }

  void visit (ConversionExpr& node)
  {
    node.visit_children (*this);
    conversion (node, node.type_expr, node.expr);
  }

  void visit (ListExpr& node)
  {
    node.visit_children (*this);
  }

  void visit (LiteralExpr& node)
  {
    assert (node.value.present);
    node.expression_kind = kValue;
    node.intrinsic_mutability = Immutable;
    node.indirection_mutability = Immutable;
    fix (node);
  }

  void visit (IdentifierExpr& node)
  {
    Identifier *identifier_node = node.child;
    const std::string& identifier = identifier_node->identifier;
    node.symbol = symtab.find_global_symbol (identifier);
    if (node.symbol == NULL)
      {
        error_at_line (-1, 0, identifier_node->location.File.c_str (),
                       identifier_node->location.Line, "%s is not defined (E214)",
                       identifier.c_str ());
      }

    struct visitor : public ConstSymbolVisitor
    {
      IdentifierExpr& node;

      visitor (IdentifierExpr& n)
        : node (n)
      { }

      void defaultAction (const Symbol& symbol)
      {
        NOT_REACHED;
      }

      void visit (const BuiltinFunction& symbol)
      {
        node.type = symbol.type ();
        node.callable = &symbol;
        node.expression_kind = kValue;
        node.intrinsic_mutability = Immutable;
        node.indirection_mutability = Immutable;
        fix (node);
      }

      void visit (const decl::Template& symbol)
      {
        node.type = symbol.type ();
        node.temp = &symbol;
        node.expression_kind = kValue;
        node.intrinsic_mutability = Immutable;
        node.indirection_mutability = Immutable;
        fix (node);
      }

      void visit (const decl::Function& symbol)
      {
        node.type = symbol.type ();
        node.callable = &symbol;
        node.expression_kind = kValue;
        node.intrinsic_mutability = Immutable;
        node.indirection_mutability = Immutable;
        fix (node);
      }

      void visit (const ParameterSymbol& symbol)
      {
        node.type = symbol.type;
        node.expression_kind = kVariable;
        node.intrinsic_mutability = symbol.intrinsic_mutability;
        node.indirection_mutability = symbol.dereference_mutability;
        fix (node);
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
        node.intrinsic_mutability = Immutable;
        node.indirection_mutability = Mutable;
        fix (node);
      }

      void visit (const VariableSymbol& symbol)
      {
        node.type = symbol.type;
        node.expression_kind = kVariable;
        node.intrinsic_mutability = symbol.intrinsic_mutability;
        node.indirection_mutability = symbol.dereference_mutability;
        fix (node);
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

  void visit (UnaryArithmeticExpr& node)
  {
    node.visit_children (*this);
    switch (node.arithmetic)
      {
      case LogicNot:
      {
        if (!(is_any_boolean (node.child->type)))
          {
            error_at_line (-1, 0, node.location.File.c_str (),
                           node.location.Line,
                           "! cannot be applied to %s (E158)",
                           node.child->type->ToString ().c_str ());
          }
        node.type = node.child->type;
        if (node.child->value.present)
          {
            node.value.present = true;
            switch (node.type->underlying_kind ())
              {
              case kBool:
                node.value.bool_value = !node.child->value.bool_value;
                break;
              case kBoolean:
                node.value.boolean_value = !node.child->value.boolean_value;
                break;
              default:
                NOT_REACHED;
              }
          }
      }
      break;
      case Posate:
        UNIMPLEMENTED;
      case Negate:
      {
        if (!(is_typed_numeric (node.child->type) ||
              is_untyped_numeric (node.child->type)))
          {
            error_at_line (-1, 0, node.location.File.c_str (),
                           node.location.Line,
                           "- cannot be applied to %s (E159)",
                           node.child->type->ToString ().c_str ());
          }
        node.type = node.child->type;
        if (node.child->value.present)
          {
            node.value.present = true;
            switch (node.type->underlying_kind ())
              {
              case kUint8:
                node.value.uint8_value = -node.child->value.uint8_value;
                break;
              case kUint16:
                node.value.uint16_value = -node.child->value.uint16_value;
                break;
              case kUint32:
                node.value.uint32_value = -node.child->value.uint32_value;
                break;
              case kUint64:
                node.value.uint32_value = -node.child->value.uint32_value;
                break;
              case kInt8:
                node.value.int8_value = -node.child->value.int8_value;
                break;
              case kInt16:
                node.value.int16_value = -node.child->value.int16_value;
                break;
              case kInt32:
                node.value.int32_value = -node.child->value.int32_value;
                break;
              case kInt64:
                node.value.int64_value = -node.child->value.int64_value;
                break;
              case kFloat32:
                node.value.float32_value = -node.child->value.float32_value;
                break;
              case kFloat64:
                node.value.float64_value = -node.child->value.float64_value;
                break;
              case kComplex64:
                node.value.complex64_value = -node.child->value.complex64_value;
                break;
              case kComplex128:
                node.value.complex128_value = -node.child->value.complex128_value;
                break;
              case kUint:
                node.value.uint_value = -node.child->value.uint_value;
                break;
              case kInt:
                node.value.int_value = -node.child->value.int_value;
                break;
              case kUintptr:
                node.value.uintptr_value = -node.child->value.uintptr_value;
                break;
              case kRune:
                node.value.rune_value = -node.child->value.rune_value;
                break;
              case kInteger:
                node.value.integer_value = -node.child->value.integer_value;
                break;
              case kFloat:
                node.value.float_value = -node.child->value.float_value;
                break;
              case kComplex:
                node.value.complex_value = -node.child->value.complex_value;
                break;
              default:
                NOT_REACHED;
              }
          }
      }
      break;
      case Complement:
        UNIMPLEMENTED;
      }

    require_value_or_variable (node.child);
    node.expression_kind = kValue;
    node.intrinsic_mutability = Immutable;
    node.indirection_mutability = Immutable;
    fix (node);
  }

  void visit (BinaryArithmeticExpr& node)
  {
    node.visit_children (*this);
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

    require_value_or_variable (node.left);
    require_value_or_variable (node.right);
    node.expression_kind = kValue;
    node.intrinsic_mutability = Immutable;
    node.indirection_mutability = Immutable;
    fix (node);
  }

  void visit (SourceFile& node)
  {
    node.visit_children (*this);
  }

  void visit (ast::Type& node)
  {
    // Do nothing.
  }

  void visit (Instance& node)
  {
    // Check the arguments.
    node.expression_list->accept (*this);
    check_types_arguments (node.expression_list, node.symbol->initializer->initializerType->signature);
    require_value_or_variable_list (node.expression_list);
    check_mutability_arguments (node.expression_list, node.symbol->initializer->initializerType->signature);
  }

  void visit (ast::Initializer& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.initializer->receiver_parameter ());
    symtab.enter_signature (node.initializer->signature ());
    symtab.enter_symbol (node.initializer->return_parameter ());
    Visitor v (*this);
    v.receiver_parameter = node.initializer->initializerType->receiver_parameter;
    v.context = Initializer;
    node.body->accept (v);
    symtab.close_scope ();
  }

  void visit (ast::Getter& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.getter->receiver_parameter ());
    symtab.enter_signature (node.getter->signature ());
    symtab.enter_symbol (node.getter->return_parameter ());
    Visitor v (*this);
    v.receiver_parameter = node.getter->getterType->receiver_parameter;
    v.context = Getter;
    node.body->accept (v);
    symtab.close_scope ();
  }

  void visit (ast::Action& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.action->receiver_parameter);
    Visitor v (*this);
    v.receiver_parameter = node.action->receiver_parameter;
    v.context = Action;
    node.precondition->accept (v);
    check_condition (node.precondition);
    node.body->accept (v);
    node.action->precondition = node.precondition;

    if (node.precondition->value.present)
      {
        if (node.precondition->value.bool_value)
          {
            node.action->precondition_kind = decl::Action::StaticTrue;
          }
        else
          {
            node.action->precondition_kind = decl::Action::StaticFalse;
          }
      }
    symtab.close_scope ();
  }

  void visit (DimensionedAction& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.action->receiver_parameter);
    symtab.enter_symbol (node.action->iota_parameter);
    Visitor v (*this);
    v.receiver_parameter = node.action->receiver_parameter;
    v.context = Action;
    node.precondition->accept (v);
    check_condition (node.precondition);
    node.body->accept (v);
    node.action->precondition = node.precondition;

    if (node.precondition->value.present)
      {
        if (node.precondition->value.bool_value)
          {
            node.action->precondition_kind = decl::Action::StaticTrue;
          }
        else
          {
            node.action->precondition_kind = decl::Action::StaticFalse;
          }
      }
    symtab.close_scope ();
  }

  void visit (ast::Reaction& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.reaction->receiver);
    symtab.enter_signature (node.reaction->signature ());
    // No return type.
    Visitor v (*this);
    v.receiver_parameter = node.reaction->reaction_type->receiver_parameter;
    v.context = Reaction;
    node.body->accept (v);
    symtab.close_scope ();
  }

  void visit (DimensionedReaction& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.reaction->receiver);
    symtab.enter_symbol (node.reaction->iota);
    symtab.enter_signature (node.reaction->signature ());

    // No return type.
    Visitor v (*this);
    v.receiver_parameter = node.reaction->reaction_type->receiver_parameter;
    v.context = Reaction;
    node.body->accept (v);
    symtab.close_scope ();
  }

  void visit (Bind& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.bind->receiver_parameter);
    node.body->accept (*this);
    symtab.close_scope ();
  }

  void visit (ast::Function& node)
  {
    symtab.open_scope ();
    symtab.enter_signature (node.function->signature ());
    symtab.enter_symbol (node.function->return_parameter ());
    node.body->accept (*this);
    symtab.close_scope ();
  }

  void visit (ast::Method& node)
  {
    symtab.open_scope ();
    symtab.enter_symbol (node.method->receiver_parameter ());
    symtab.enter_signature (node.method->signature ());
    symtab.enter_symbol (node.method->return_parameter ());
    Visitor v (*this);
    v.receiver_parameter = node.method->methodType->receiver_parameter;
    node.body->accept (v);
    symtab.close_scope ();
  }

  void visit (ListStatement& node)
  {
    symtab.open_scope ();
    node.visit_children (*this);
    symtab.close_scope ();
  }

  void visit (ExpressionStatement& node)
  {
    node.visit_children (*this);
    require_value_or_variable (node.child);
  }

  void visit (ReturnStatement& node)
  {
    // Check the expression.
    node.visit_children (*this);

    // Get the return symbol.
    node.return_symbol = SymbolCast<ParameterSymbol> (symtab.find_global_symbol (ReturnSymbol));
    assert (node.return_symbol != NULL);

    if (!assignable (node.child->type, node.child->value, node.return_symbol->type))
      {
        error_at_line (-1, 0, node.location.File.c_str (),
                       node.location.Line, "cannot convert %s to %s in return (E160)",
                       node.child->type->ToString ().c_str (), node.return_symbol->type->ToString ().c_str ());
      }
    convert (node.child, node.return_symbol->type);

    require_value_or_variable (node.child);

    if (type_contains_pointer (node.child->type) &&
        node.return_symbol->dereference_mutability < node.child->indirection_mutability)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "return casts away +const or +foreign (E149)");
      }
  }

  void visit (IfStatement& node)
  {
    symtab.open_scope ();
    node.visit_children (*this);
    check_condition (node.condition);
    symtab.close_scope ();
  }

  void visit (WhileStatement& node)
  {
    symtab.open_scope ();
    node.visit_children (*this);
    check_condition (node.condition);
    symtab.close_scope ();
  }

  void visit (ForIotaStatement& node)
  {
    const std::string& identifier = node.identifier->identifier;
    node.limit = process_array_dimension (node.limit_node, symtab);
    node.symbol = new VariableSymbol (identifier, node.identifier->location, Int::Instance (), Immutable, Immutable);
    symtab.open_scope ();
    symtab.enter_symbol (node.symbol);
    node.body->accept (*this);
    symtab.close_scope ();
  }

  void visit (ChangeStatement& node)
  {
    node.expr->accept (*this);

    const type::Type* root_type = type_change (node.expr->type);
    if (root_type == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot change expression of type %s (E96)", node.expr->type->ToString ().c_str ());
      }

    require_value_or_variable (node.expr);

    // Enter all parameters and variables in scope that are pointers as pointers to foreign.
    symtab.open_scope ();
    symtab.change ();

    // Enter the new heap root.
    const std::string& identifier = node.identifier->identifier;
    // Don't know dereference mutability yet.
    node.root_symbol = new VariableSymbol (identifier, node.location, root_type, Immutable, Foreign);
    node.root_symbol->dereference_mutability = node.expr->indirection_mutability;
    symtab.enter_symbol (node.root_symbol);

    // Check the body.
    node.body->accept (*this);
    symtab.close_scope ();
  }

  void visit (ActivateStatement& node)
  {
    if (!(context == Action ||
          context == Reaction))
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "activation outside of action or reaction (E53)");
      }

    if (in_mutable_phase)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "activations within activations are not allowed (E54)");
      }

    // Check the activations.
    node.expr_list->accept (*this);

    Visitor v (*this);
    v.in_mutable_phase = true;

    // Re-insert this as a pointer to mutable.
    symtab.open_scope ();
    symtab.activate ();
    // Check the body.
    node.body->accept (v);
    symtab.close_scope ();

    node.in_action = context == Action;
  }

  void visit (Const& node)
  {
    if (!node.done)
      {
        process_types_and_constants (&node, symtab);
      }
  }

  void visit (EmptyStatement& node)
  {
    // Do nothing.
  }

  void visit (VarStatement& node)
  {
    ast::List* identifier_list = node.identifier_list;
    ast::Node* type_spec = node.type_spec;
    ast::List* expression_list = node.expression_list;

    if (expression_list->size () != 0 &&
        identifier_list->size () != expression_list->size ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "wrong number of initializers (E184)");
      }

    // Process the type spec.
    const type::Type* type = process_type (type_spec, symtab, true);

    if (expression_list->size () == 0)
      {
        // Type, no expressions.

        if (type_cast<Void> (type) != NULL)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "missing type (E183)");

          }

        // Enter each symbol.
        for (List::ConstIterator id_pos = identifier_list->begin (),
             id_limit = identifier_list->end ();
             id_pos != id_limit;
             ++id_pos)
          {
            const std::string& name = ast_cast<Identifier> (*id_pos)->identifier;
            VariableSymbol* symbol = new VariableSymbol (name, (*id_pos)->location, type, node.mutability, node.dereferenceMutability);
            symtab.enter_symbol (symbol);
            node.symbols.push_back (symbol);
          }

        return;
      }

    if (type_cast<Void> (type) == NULL)
      {
        // Type, expressions.

        // Enter each symbol.
        for (List::ConstIterator id_pos = identifier_list->begin (),
             id_limit = identifier_list->end (),
             init_pos = expression_list->begin ();
             id_pos != id_limit;
             ++id_pos, ++init_pos)
          {
            Node* n = *init_pos;
            n->accept (*this);

            if (!assignable (n->type, n->value, type))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "cannot assign %s to %s in initialization (E62)", n->type->ToString ().c_str (), type->ToString ().c_str ());
              }
            convert (n, type);
            require_value_or_variable (n);

            const std::string& name = ast_cast<Identifier> (*id_pos)->identifier;
            VariableSymbol* symbol = new VariableSymbol (name, (*id_pos)->location, type, node.intrinsic_mutability, node.dereferenceMutability);
            symtab.enter_symbol (symbol);
            node.symbols.push_back (symbol);
          }

        goto done;
      }

    // No type, expressions.

    // Enter each symbol.
    for (List::ConstIterator id_pos = identifier_list->begin (),
         id_limit = identifier_list->end (),
         init_pos = expression_list->begin ();
         id_pos != id_limit;
         ++id_pos, ++init_pos)
      {
        Node* n = *init_pos;
        n->accept (*this);

        if (n->type->IsUntyped ())
          {
            n->value.convert (n->type, n->type->DefaultType ());
            n->type = n->type->DefaultType ();
          }
        require_value_or_variable (n);

        const std::string& name = ast_cast<Identifier> (*id_pos)->identifier;
        VariableSymbol* symbol = new VariableSymbol (name, (*id_pos)->location, n->type, node.intrinsic_mutability, node.dereferenceMutability);
        symtab.enter_symbol (symbol);
        node.symbols.push_back (symbol);
      }

done:
    if (!node.expression_list->empty ())
      {
        size_t idx = 0;
        for (List::ConstIterator pos = node.expression_list->begin (), limit = node.expression_list->end ();
             pos != limit;
             ++pos, ++idx)
          {
            Node* n = *pos;
            VariableSymbol* symbol = node.symbols[idx];

            if (type_contains_pointer (n->type) &&
                symbol->dereference_mutability < n->indirection_mutability)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "assignment casts away +const or +foreign (E92)");
              }
          }
      }
  }

  void visit (AssignStatement& node)
  {
    node.visit_children (*this);
    const type::Type* to = node.left->type;
    const type::Type*& from = node.right->type;
    Value& val = node.right->value;
    if (!assignable (from, val, to))
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot assign value of type %s to variable of type %s (E199)",
                       from->ToString ().c_str (),
                       to->ToString ().c_str ());
      }
    convert (node.right, to);
    require_variable (node.left);
    require_value_or_variable (node.right);

    if (node.left->intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "target of assignment is not mutable (E86)");
      }

    if (type_contains_pointer (node.right->type) &&
        node.left->indirection_mutability < node.right->indirection_mutability)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "assignment casts away +const or +foreign (E161)");
      }
  }

  void visit (AddAssignStatement& node)
  {
    node.visit_children (*this);

    const type::Type* to = node.left->type;
    const type::Type*& from = node.right->type;
    Value& val = node.right->value;
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
    convert (node.right, to);
    require_variable (node.left);
    require_value_or_variable (node.right);

    if (node.left->intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "target of assignment is not mutable (E15)");
      }
  }

  void visit (IncrementStatement& node)
  {
    node.visit_children (*this);
    if (!arithmetic (node.child->type))
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "++ cannot be applied to %s (E77)",
                       node.child->type->ToString ().c_str ());
      }
    require_variable (node.child);

    if (node.child->intrinsic_mutability != Mutable)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "target of increment is not mutable (E177)");
      }
  }

  void visit (BindPushPortStatement& node)
  {
    node.visit_children (*this);
    bind (node, node.left, node.right);
  }

  void visit (BindPushPortParamStatement& node)
  {
    node.visit_children (*this);
    const reaction_t* reaction = bind (node, node.left, node.right);
    if (!reaction->has_dimension ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "parameter specified for non-parameterized reaction (E41)");
      }
    type::Int::ValueType dimension = reaction->dimension ();
    check_array_index (reaction->reaction_type->GetArray (dimension), node.param, false);
  }

  void visit (BindPullPortStatement& node)
  {
    node.visit_children (*this);

    const type::Function* pull_port_type = type_cast<type::Function> (node.left->type);

    if (pull_port_type == NULL || pull_port_type->function_kind != type::Function::PULL_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "target of bind is not a pull port (E193)");
      }

    require_variable (node.left);

    const type::Method* getter_type = type_cast<type::Method> (node.right->type);

    if (getter_type == NULL || getter_type->method_kind != type::Method::GETTER)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "source of bind is not a getter (E192)");
      }

    require_variable (node.right);

    type::Function g (type::Function::FUNCTION, getter_type->signature, getter_type->return_parameter);
    if (!identical (pull_port_type, &g))
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot bind %s to %s (E191)", pull_port_type->ToString ().c_str (), getter_type->ToString ().c_str ());
      }
  }

  void visit (DereferenceExpr& node)
  {
    node.visit_children (*this);
    const type::Type* t = node.child->type;
    const Pointer* p = type_cast<Pointer> (t->UnderlyingType ());
    if (p == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "* cannot be applied to %s (E21)", t->ToString ().c_str ());
      }
    require_value_or_variable (node.child);
    node.type = p->Base ();
    node.expression_kind = kVariable;
    node.intrinsic_mutability = node.child->indirection_mutability;
    node.indirection_mutability = node.child->indirection_mutability;
    fix (node);
  }

  void visit (AddressOfExpr& node)
  {
    node.visit_children (*this);
    require_variable (node.child);
    node.type = node.child->type->GetPointer ();
    node.expression_kind = kValue;
    node.intrinsic_mutability = node.child->intrinsic_mutability;
    node.indirection_mutability = std::max (node.child->intrinsic_mutability, node.child->indirection_mutability);
    fix (node);
  }

  void visit (SelectExpr& node)
  {
    const std::string& identifier = node.identifier->identifier;
    node.base->accept (*this);
    const type::Type* base_type = node.base->type;
    require_value_or_variable (node.base);

    node.field = base_type->select_field (identifier);
    node.callable = base_type->select_callable (identifier);
    if (node.field != NULL)
      {
        node.type = node.field->type;
      }
    else if (node.callable != NULL)
      {
        node.type = node.callable->type ();
      }
    else
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot select %s from expression of type %s (E154)",
                       identifier.c_str (), base_type->ToString ().c_str ());
      }

    if (type_dereference (base_type))
      {
        // Selecting through pointer always yields a variable.
        node.expression_kind = kVariable;
        node.intrinsic_mutability = node.base->indirection_mutability;
        node.indirection_mutability = node.base->indirection_mutability;
        fix (node);
      }
    else
      {
        // Otherwise, use the base kind.
        node.expression_kind = node.base->expression_kind;
        node.intrinsic_mutability = node.base->intrinsic_mutability;
        node.indirection_mutability = node.base->indirection_mutability;
        fix (node);
      }
  }

  void check_array_index (const Array* array_type, Node* index, bool allow_equal)
  {
    const type::Type*& index_type = index->type;
    Value& index_value = index->value;

    if (is_untyped_numeric (index_type))
      {
        if (!index_value.representable (index_type, Int::Instance ()))
          {
            error_at_line (-1, 0, index->location.File.c_str (), index->location.Line,
                           "array index is not an integer (E20)");
          }
        index_value.convert (index_type, Int::Instance ());
        index_type = Int::Instance ();
        Int::ValueType idx = index_value.int_value;
        if (idx < 0)
          {
            error_at_line (-1, 0, index->location.File.c_str (), index->location.Line,
                           "array index is negative (E162)");
          }
        if (array_type)
          {
            if ((!allow_equal && idx >= array_type->dimension) ||
                (allow_equal && idx > array_type->dimension))
              {
                error_at_line (-1, 0, index->location.File.c_str (), index->location.Line,
                               "array index is out of bounds (E163)");
              }
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
            if (array_type)
              {
                if (idx >= array_type->dimension)
                  {
                    error_at_line (-1, 0, index->location.File.c_str (), index->location.Line,
                                   "array index is out of bounds (E165)");
                  }
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

  void visit (IndexExpr& node)
  {
    node.visit_children (*this);
    const type::Type* base_type = node.base->type;
    Node* index_node = node.index;
    Value& index_value = index_node->value;
    const type::Type*& index_type = node.index->type;

    node.array_type = type_cast<Array> (base_type->UnderlyingType ());
    if (node.array_type != NULL)
      {
        check_array_index (node.array_type, node.index, false);
        require_value_or_variable (node.base);
        node.type = node.array_type->Base ();
        node.expression_kind = node.base->expression_kind;
        node.intrinsic_mutability = node.base->intrinsic_mutability;
        node.indirection_mutability = node.base->indirection_mutability;
        fix (node);
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
            Int::ValueType idx = index_value.int_value;
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

        require_value_or_variable (node.base);
        require_value_or_variable (node.index);

        node.type = node.slice_type->Base ();
        node.expression_kind = kVariable;
        node.intrinsic_mutability = node.base->intrinsic_mutability;
        node.indirection_mutability = node.base->indirection_mutability;
        fix (node);
        return;
      }

    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                   "cannot index expression of type %s (E168)",
                   base_type->ToString ().c_str ());
  }

  void visit (SliceExpr& node)
  {
    Node* base = node.base;
    base->accept (*this);
    require_value_or_variable (base);
    const type::Type* base_type = node.base->type;

    Node* low_node = node.low;
    if (node.low_present)
      {
        low_node->accept (*this);
      }
    const type::Type*& low_type = low_node->type;
    Value& low_value = low_node->value;

    Node* high_node = node.high;
    if (node.high_present)
      {
        high_node->accept (*this);
      }
    const type::Type*& high_type = high_node->type;
    Value& high_value = high_node->value;

    Node* max_node = node.max;
    if (node.max_present)
      {
        max_node->accept (*this);
      }
    const type::Type*& max_type = max_node->type;
    Value& max_value = max_node->value;

    node.string_type = type_cast<String> (base_type->UnderlyingType ());
    if (node.string_type != NULL)
      {
        UNIMPLEMENTED;
        return;
      }

    node.pointer_to_array_type = pointer_to_array (base_type->UnderlyingType ());
    if (node.pointer_to_array_type)
      {
        UNIMPLEMENTED;
        return;
      }

    node.array_type = type_cast<Array> (base_type->UnderlyingType ());
    if (node.array_type != NULL)
      {
        if (node.low_present)
          {
            check_array_index (node.array_type, low_node, true);
          }
        if (node.high_present)
          {
            check_array_index (node.array_type, high_node, true);
          }
        if (node.max_present)
          {
            check_array_index (node.array_type, max_node, true);
          }

        node.type = node.array_type->Base ()->GetSlice ();
        node.expression_kind = kValue;
        node.intrinsic_mutability = Immutable;
        node.indirection_mutability = node.base->indirection_mutability;
        fix (node);
        goto done;
      }

    node.slice_type = type_cast<Slice> (base_type->UnderlyingType ());
    if (node.slice_type != NULL)
      {
        if (node.low_present)
          {
            check_array_index (NULL, low_node, true);
          }
        if (node.high_present)
          {
            check_array_index (NULL, high_node, true);
          }
        if (node.max_present)
          {
            check_array_index (NULL, max_node, true);
          }

        node.type = node.slice_type;
        node.expression_kind = kValue;
        node.intrinsic_mutability = Immutable;
        node.indirection_mutability = node.base->indirection_mutability;
        fix (node);
        goto done;
      }

    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                   "cannot slice expression of type %s (E225)",
                   base_type->ToString ().c_str ());

done:
    if (low_value.present && high_value.present)
      {
        if (!(low_value.to_int (low_type) <= high_value.to_int (high_type)))
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "slice indices are out of range (E224)");
          }
      }

    if (low_value.present && max_value.present)
      {
        if (!(low_value.to_int (low_type) <= max_value.to_int (max_type)))
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "slice indices are out of range (E6)");
          }
      }

    if (high_value.present && max_value.present)
      {
        if (!(high_value.to_int (high_type) <= max_value.to_int (max_type)))
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "slice indices are out of range (E7)");
          }
      }
  }

  void visit (TypeExpression& node)
  {
    node.type = process_type (node.type_spec, symtab, true);
    node.expression_kind = kType;
  }

  void visit (PushPortCallExpr& node)
  {
    node.receiver_parameter = receiver_parameter;
    const std::string& port_identifier = node.identifier->identifier;
    const type::Type* this_type = receiver_parameter->type;
    node.field = this_type->select_field (port_identifier);
    if (node.field == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E194)", port_identifier.c_str ());
      }
    const type::Function* push_port_type = type::type_cast<type::Function> (node.field->type);
    if (push_port_type == NULL || push_port_type->function_kind != type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E195)", port_identifier.c_str ());
      }

    node.args->accept (*this);
    check_types_arguments (node.args, push_port_type->GetSignature ());
    require_value_or_variable_list (node.args);
  }

  void visit (IndexedPushPortCallExpr& node)
  {
    node.receiver_parameter = receiver_parameter;
    const std::string& port_identifier = node.identifier->identifier;
    const type::Type* this_type = receiver_parameter->type;
    node.field = this_type->select_field (port_identifier);
    if (node.field == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E74)", port_identifier.c_str ());
      }
    node.array_type = type::type_cast<type::Array> (node.field->type);
    if (node.array_type == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "%s is not an array of ports (E16)", port_identifier.c_str ());
      }
    const type::Function* push_port_type = type::type_cast<type::Function> (node.array_type->Base ());
    if (push_port_type == NULL || push_port_type->function_kind != type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "%s is not an array of ports (E17)", port_identifier.c_str ());
      }

    node.index->accept (*this);
    check_array_index (node.array_type, node.index, false);

    node.args->accept (*this);
    check_types_arguments (node.args, push_port_type->GetSignature ());
    require_value_or_variable_list (node.args);
  }

  void visit (CompositeLiteral& node)
  {
    node.type = process_type (node.literal_type, symtab, true);
    node.expression_kind = kVariable;

    switch (node.type->underlying_kind ())
      {
      case kStruct:
      {
        for (List::ConstIterator pos = node.literal_value->begin (),
             limit = node.literal_value->end ();
             pos != limit;
             ++pos)
          {
            //Node* element = *pos;
            UNIMPLEMENTED;
          }
      }
      break;

      case kArray:
        UNIMPLEMENTED;
        break;

      case kSlice:
        UNIMPLEMENTED;
        break;

      default:
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot define composite literals for %s (E5)", node.type->ToString ().c_str ());
      }
  }
};
}

void check_types_arguments (ast::List* args, const type::Signature* signature)
{
  if (args->size () != signature->Arity ())
    {
      error_at_line (-1, 0, args->location.File.c_str (), args->location.Line,
                     "call expects %lu arguments but given %lu (E150)", signature->Arity (), args->size ());
    }

  size_t i = 0;
  for (List::ConstIterator pos = args->begin (), limit = args->end ();
       pos != limit;
       ++pos, ++i)
    {
      const type::Type*& arg = (*pos)->type;
      Value& val = (*pos)->value;
      const type::Type* param = signature->At (i)->type;
      if (!type::assignable (arg, val, param))
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

void require_value_or_variable_list (const List* node)
{
  for (List::ConstIterator pos = node->begin (), limit = node->end ();
       pos != limit;
       ++pos)
    {
      require_value_or_variable (*pos);
    }
}

void check_types (ast::Node* root, decl::SymbolTable& symtab)
{
  Visitor visitor (symtab);
  root->accept (visitor);
}

void check_mutability_arguments (ast::Node* node, const type::Signature* signature)
{
  ListExpr* args = ast_cast<ListExpr> (node);

  size_t i = 0;
  for (List::ConstIterator pos = args->begin (), limit = args->end ();
       pos != limit;
       ++pos, ++i)
    {
      const type::Type* arg = (*pos)->type;
      if (type_contains_pointer (arg))
        {
          if (signature->At (i)->dereference_mutability < (*pos)->indirection_mutability)
            {
              error_at_line (-1, 0, (*pos)->location.File.c_str (), (*pos)->location.Line,
                             "argument %zd casts away +const or +foreign (E85)", i + 1);
            }
        }
    }
}

}
