#include "semantic.hpp"

#include <error.h>

#include "debug.hpp"
#include "node.hpp"
#include "type.hpp"
#include "symbol.hpp"
#include "memory_model.hpp"
#include "callable.hpp"
#include "node_visitor.hpp"
#include "parameter_list.hpp"
#include "error_reporter.hpp"
#include "runtime.hpp"
#include "operation.hpp"

namespace semantic
{

using namespace ast;
using namespace decl;
using namespace util;
using namespace type;

ExpressionValueList collect_evals (ast::Node* node)
{
  struct visitor : public DefaultNodeVisitor
  {
    ExpressionValueList list;

    virtual void default_action (Node& node)
    {
      list.push_back (node.eval);
    }
  };

  visitor v;
  node->visit_children (v);
  return v.list;
}


void
allocate_symbol (runtime::MemoryModel& memory_model,
                 Symbol* symbol)
{
  struct visitor : public SymbolVisitor
  {
    runtime::MemoryModel& memory_model;

    visitor (runtime::MemoryModel& mm) : memory_model (mm) { }

    void defineAction (Symbol& symbol)
    {
      NOT_REACHED;
    }

    void visit (Parameter& symbol)
    {
      switch (symbol.kind)
        {
        case Parameter::Ordinary:
        case Parameter::Receiver:
        case Parameter::Return:
        {
          const type::Type* type = symbol.type;
          memory_model.arguments_push (type);
          static_cast<Symbol&> (symbol).offset (memory_model.arguments_offset ());
          if (symbol.kind == Parameter::Receiver)
            {
              memory_model.set_receiver_offset ();
            }
        }
        break;
        case Parameter::Receiver_Duplicate:
        case Parameter::Ordinary_Duplicate:
          break;
        }
    }

    void visit (Constant& symbol)
    {
      // No need to allocate.
    }

    void visit (Variable& symbol)
    {
      const type::Type* type = symbol.type;
      static_cast<Symbol&>(symbol).offset (memory_model.locals_offset ());
      memory_model.locals_push (type);
    }

    void visit (decl::Hidden& symbol)
    {
      // Do nothing.
    }
  };

  visitor v (memory_model);
  symbol->accept (v);
}

static void
allocate_statement_stack_variables (ast::Node* node, runtime::MemoryModel& memory_model)
{
  struct visitor : public DefaultNodeVisitor
  {
    runtime::MemoryModel& memory_model;

    visitor (runtime::MemoryModel& m) : memory_model (m) { }

    void default_action (Node& node)
    {
      AST_NOT_REACHED (node);
    }

    void visit (ConstDecl& node)
    {
      // Do nothing.
    }

    void visit (EmptyStatement& node)
    { }

    void visit (ForIota& node)
    {
      ptrdiff_t offset_before = memory_model.locals_offset ();
      allocate_symbol (memory_model, node.symbol);
      ptrdiff_t offset_after = memory_model.locals_offset ();
      allocate_statement_stack_variables (node.body, memory_model);
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
    }

    void visit (BindPushPort& node)
    {
      // Do nothing.
    }

    void visit (BindPushPortParameter& node)
    {
      // Do nothing.
    }

    void visit (BindPullPort& node)
    {
      // Do nothing.
    }

    void visit (Assign& node)
    {
      // Do nothing.
    }

    void visit (Change& node)
    {
      ptrdiff_t offset_before = memory_model.locals_offset ();
      allocate_symbol (memory_model, node.root_symbol);
      ptrdiff_t offset_after = memory_model.locals_offset ();
      allocate_statement_stack_variables (node.body, memory_model);
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
    }

    void visit (ExpressionStatement& node)
    {
      // Do nothing.
    }

    void visit (If& node)
    {
      allocate_statement_stack_variables (node.before, memory_model);
      allocate_statement_stack_variables (node.true_body, memory_model);
      allocate_statement_stack_variables (node.false_body, memory_model);
    }

    void visit (While& node)
    {
      allocate_statement_stack_variables (node.body, memory_model);
    }

    void visit (AddAssign& node)
    {
      // Do nothing.
    }

    void visit (SubtractAssign& node)
    {
      // Do nothing.
    }

    void visit (StatementList& node)
    {
      ptrdiff_t offset_before = memory_model.locals_offset ();
      for (List::ConstIterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          allocate_statement_stack_variables (*pos, memory_model);
        }
      ptrdiff_t offset_after = memory_model.locals_offset ();
      memory_model.locals_pop (offset_after - offset_before);
      assert (memory_model.locals_offset () == offset_before);
    }

    void visit (Return& node)
    {
      // Do nothing.
    }

    void visit (IncrementDecrement& node)
    {
      // Do nothing.
    }

    void visit (Activate& node)
    {
      allocate_statement_stack_variables (node.body, memory_model);
      node.memory_model = &memory_model;
    }

    void visit (Var& node)
    {
      for (Var::SymbolsType::const_iterator pos = node.symbols.begin (),
           limit = node.symbols.end ();
           pos != limit;
           ++pos)
        {
          allocate_symbol (memory_model, *pos);
        }
    }
  };
  visitor v (memory_model);
  node->accept (v);
}

void
allocate_stack_variables (ast::Node* node)
{
  struct visitor : public DefaultNodeVisitor
  {
    void visit (ast::ActionDecl& node)
    {
      allocate_symbol (node.action->memory_model, node.action->receiver_parameter ());
      if (node.action->dimension () != -1)
        {
          allocate_symbol (node.action->memory_model, node.action->iota_parameter ());
        }

      allocate_statement_stack_variables (node.body, node.action->memory_model);
      assert (node.action->memory_model.locals_empty ());
    }

    void visit (ast::BindDecl& node)
    {
      allocate_symbol (node.bind->memory_model, node.bind->receiver_parameter ());
      allocate_statement_stack_variables (node.body, node.bind->memory_model);
      assert (node.bind->memory_model.locals_empty ());
    }

    void visit (ast::FunctionDecl& node)
    {
      allocate_parameters (node.symbol->memory_model, node.symbol->parameter_list ());
      allocate_parameters (node.symbol->memory_model, node.symbol->return_parameter_list ());
      allocate_statement_stack_variables (node.body, node.symbol->memory_model);
      assert (node.symbol->memory_model.locals_empty ());
    }

    void visit (ast::MethodDecl& node)
    {
      allocate_parameters (node.method->memory_model, node.method->parameter_list ());
      allocate_parameters (node.method->memory_model, node.method->return_parameter_list ());
      allocate_statement_stack_variables (node.body, node.method->memory_model);
      assert (node.method->memory_model.locals_empty ());
    }

    void visit (ast::InitDecl& node)
    {
      allocate_parameters (node.initializer->memory_model, node.initializer->parameter_list ());
      allocate_parameters (node.initializer->memory_model, node.initializer->return_parameter_list ());
      allocate_statement_stack_variables (node.body, node.initializer->memory_model);
      assert (node.initializer->memory_model.locals_empty ());
    }

    void visit (ast::GetterDecl& node)
    {
      allocate_parameters (node.getter->memory_model, node.getter->parameter_list ());
      allocate_parameters (node.getter->memory_model, node.getter->return_parameter_list ());
      allocate_statement_stack_variables (node.body, node.getter->memory_model);
      assert (node.getter->memory_model.locals_empty ());
    }

    void visit (ast::ReactionDecl& node)
    {
      allocate_parameters (node.reaction->memory_model, node.reaction->parameter_list ());
      if (node.reaction->dimension () != -1)
        {
          allocate_symbol (node.reaction->memory_model, node.reaction->iota ());
        }
      allocate_statement_stack_variables (node.body, node.reaction->memory_model);
      assert (node.reaction->memory_model.locals_empty ());
    }

    void visit (SourceFile& node)
    {
      node.top_level_decl_list->accept (*this);
    }

    void visit (TopLevelDeclList& node)
    {
      node.visit_children (*this);
    }
  };

  visitor v;
  node->accept (v);
}

void
allocate_parameters (runtime::MemoryModel& memory_model,
                     const decl::ParameterList* signature)
{
  for (decl::ParameterList::const_reverse_iterator pos = signature->rbegin (), limit = signature->rend ();
       pos != limit;
       ++pos)
    {
      allocate_symbol (memory_model, *pos);
    }
}

bool
require_value_or_variable (ErrorReporter& er,
                           const Location& location,
                           ExpressionValue& result,
                           const ExpressionValue& arg)
{
  assert (!arg.is_unknown ());
  if (!arg.is_value_or_variable ())
    {
      er.requires_value_or_variable (location);
      result.expression_kind = ErrorExpressionKind;
      return false;
    }
  return true;
}

bool
require_type (ErrorReporter& er,
              const Location& location,
              ExpressionValue& result,
              const ExpressionValue& arg)
{
  assert (!arg.is_unknown ());
  if (!arg.is_type ())
    {
      er.expected_a_type (location);
      result.expression_kind = TypeExpressionKind;
      return false;
    }
  return true;
}

LogicNot::LogicNot (const util::Location& loc)
  : PolymorphicFunction ("!", loc)
{ }

void LogicNot::check (ErrorReporter& er,
                      const Location& location,
                      ExpressionValue& result,
                      ExpressionValueList& arguments) const
{
  assert (arguments.size () == 1);
  const ExpressionValue& arg = arguments.front ();

  if (!require_value_or_variable (er, location, result, arg))
    {
      return;
    }

  if (!(arg.type->is_any_boolean ()))
    {
      er.cannot_be_applied (location, "!", arg.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = arg.type;
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Immutable;

  if (arg.value.present)
    {
      result.value.present = true;
      switch (result.type->underlying_kind ())
        {
        case Bool_Kind:
          result.value.bool_value = !arg.value.bool_value;
          break;
        case Untyped_Boolean_Kind:
          result.value.untyped_boolean_value = !arg.value.untyped_boolean_value;
          break;
        default:
          NOT_REACHED;
        }
    }
}

runtime::Operation* LogicNot::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_val,
    runtime::ListOperation* arg_op) const
{
  return runtime::make_unary<LogicNotter> (result.type, arg_op);
}

Posate::Posate (const util::Location& loc)
  : PolymorphicFunction ("+", loc)
{ }

void Posate::check (ErrorReporter& er,
                    const util::Location& location,
                    ExpressionValue& result,
                    ExpressionValueList& arguments) const
{
  assert (arguments.size () == 1);
  const ExpressionValue& arg = arguments.front ();

  if (!require_value_or_variable (er, location, result, arg))
    {
      return;
    }

  if (!arg.type->is_numeric ())
    {
      er.cannot_be_applied (location, "+", arg.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = arg.type;
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Immutable;

  if (arg.value.present)
    {
      result.value = arg.value;
    }
}

Negate::Negate (const util::Location& loc)
  : PolymorphicFunction ("-", loc)
{ }

void Negate::check (ErrorReporter& er,
                    const util::Location& location,
                    ExpressionValue& result,
                    ExpressionValueList& arguments) const
{
  assert (arguments.size () == 1);
  const ExpressionValue& arg = arguments.front ();

  if (!require_value_or_variable (er, location, result, arg))
    {
      return;
    }

  if (!arg.type->is_numeric ())
    {
      er.cannot_be_applied (location, "-", arg.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = arg.type;
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Immutable;

  if (arg.value.present)
    {
      result.value.present = true;
      switch (result.type->underlying_kind ())
        {
        case Uint8_Kind:
          result.value.uint8_value = -arg.value.uint8_value;
          break;
        case Uint16_Kind:
          result.value.uint16_value = -arg.value.uint16_value;
          break;
        case Uint32_Kind:
          result.value.uint32_value = -arg.value.uint32_value;
          break;
        case Uint64_Kind:
          result.value.uint32_value = -arg.value.uint32_value;
          break;
        case Int8_Kind:
          result.value.int8_value = -arg.value.int8_value;
          break;
        case Int16_Kind:
          result.value.int16_value = -arg.value.int16_value;
          break;
        case Int32_Kind:
          result.value.int32_value = -arg.value.int32_value;
          break;
        case Int64_Kind:
          result.value.int64_value = -arg.value.int64_value;
          break;
        case Float32_Kind:
          result.value.float32_value = -arg.value.float32_value;
          break;
        case Float64_Kind:
          result.value.float64_value = -arg.value.float64_value;
          break;
        case Complex64_Kind:
          result.value.complex64_value = -arg.value.complex64_value;
          break;
        case Complex128_Kind:
          result.value.complex128_value = -arg.value.complex128_value;
          break;
        case Uint_Kind:
          result.value.uint_value = -arg.value.uint_value;
          break;
        case Int_Kind:
          result.value.int_value = -arg.value.int_value;
          break;
        case Uintptr_Kind:
          result.value.uintptr_value = -arg.value.uintptr_value;
          break;
        case Untyped_Rune_Kind:
          result.value.untyped_rune_value = -arg.value.untyped_rune_value;
          break;
        case Untyped_Integer_Kind:
          result.value.untyped_integer_value = -arg.value.untyped_integer_value;
          break;
        case Untyped_Float_Kind:
          result.value.untyped_float_value = -arg.value.untyped_float_value;
          break;
        case Untyped_Complex_Kind:
          result.value.untyped_complex_value = -arg.value.untyped_complex_value;
          break;
        default:
          NOT_REACHED;
        }
    }
}

Complement::Complement (const util::Location& loc)
  : PolymorphicFunction ("^", loc)
{ }

void Complement::check (ErrorReporter& er,
                        const Location& location,
                        ExpressionValue& result,
                        ExpressionValueList& arguments) const
{
  assert (arguments.size () == 1);
  const ExpressionValue& arg = arguments.front ();

  if (!require_value_or_variable (er, location, result, arg))
    {
      return;
    }

  if (!(arg.type->is_integral ()))
    {
      er.cannot_be_applied (location, "^", arg.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = arg.type;
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Immutable;

  if (arg.value.present)
    {
      result.value.present = true;
      switch (result.type->underlying_kind ())
        {
        case Uint8_Kind:
          result.value.uint8_value = ~arg.value.uint8_value;
          break;
        case Uint16_Kind:
          result.value.uint16_value = ~arg.value.uint16_value;
          break;
        case Uint32_Kind:
          result.value.uint32_value = ~arg.value.uint32_value;
          break;
        case Uint64_Kind:
          result.value.uint32_value = ~arg.value.uint32_value;
          break;
        case Int8_Kind:
          result.value.int8_value = ~arg.value.int8_value;
          break;
        case Int16_Kind:
          result.value.int16_value = ~arg.value.int16_value;
          break;
        case Int32_Kind:
          result.value.int32_value = ~arg.value.int32_value;
          break;
        case Int64_Kind:
          result.value.int64_value = ~arg.value.int64_value;
          break;
        case Uint_Kind:
          result.value.uint_value = ~arg.value.uint_value;
          break;
        case Int_Kind:
          result.value.int_value = ~arg.value.int_value;
          break;
        case Uintptr_Kind:
          result.value.uintptr_value = ~arg.value.uintptr_value;
          break;
        case Untyped_Rune_Kind:
          result.value.untyped_rune_value = ~arg.value.untyped_rune_value;
          break;
        case Untyped_Integer_Kind:
          result.value.untyped_integer_value = ~arg.value.untyped_integer_value;
          break;
        default:
          NOT_REACHED;
        }
    }
}

const type::Type* PassThroughPicker::pick (const type::Type* input_type,
    const ExpressionValue& left,
    const ExpressionValue& right)
{
  return input_type;
}

const type::Type* BooleanPicker::pick (const type::Type* input_type,
                                       const ExpressionValue& left,
                                       const ExpressionValue& right)
{
  if (left.value.present && right.value.present)
    {
      return UntypedBoolean::instance ();
    }
  else
    {
      return Bool::instance ();
    }
}

template <typename Op>
void BinaryValueComputer<Op>::compute (ExpressionValue& result,
                                       const type::Type* in_type,
                                       const ExpressionValue& left,
                                       const ExpressionValue& right)
{
  if (left.value.present &&
      right.value.present)
    {
      Op() (result.value, in_type, left.value, right.value);
    }
}

template <typename Op>
runtime::Operation* BinaryValueComputer<Op>::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops)
{
  return Op::generate_code (result, arg_vals, arg_ops);
}

void LogicOrComputer::compute (ExpressionValue& result,
                               const type::Type* in_type,
                               const ExpressionValue& left,
                               const ExpressionValue& right)
{
  if (left.value.present)
    {
      if (left.value.bool_value)
        {
          result.value.present = true;
          result.value.untyped_boolean_value = true;
        }
      else if (right.value.present)
        {
          result.value.present = true;
          result.value.untyped_boolean_value = right.value.bool_value;
        }
    }
}

runtime::Operation* LogicOrComputer::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops)
{
  return new runtime::LogicOr (arg_ops->list[0], arg_ops->list[1]);
}

void LogicAndComputer::compute (ExpressionValue& result,
                                const type::Type* in_type,
                                const ExpressionValue& left,
                                const ExpressionValue& right)
{
  if (left.value.present)
    {
      if (!left.value.bool_value)
        {
          result.value.present = true;
          result.value.untyped_boolean_value = false;
        }
      else if (right.value.present)
        {
          result.value.present = true;
          result.value.untyped_boolean_value = right.value.bool_value;
        }
    }
}

runtime::Operation* LogicAndComputer::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops)
{
  return new runtime::LogicAnd (arg_ops->list[0], arg_ops->list[1]);
}

static const char* binary_arithmetic_external_symbol (::BinaryArithmetic ba)
{
  switch (ba)
    {
    case ::Multiply:
      return "*";
    case ::Divide:
      return "/";
    case ::Modulus:
      return "%";
    case ::LeftShift:
      return "<<";
    case ::RightShift:
      return ">>";
    case ::BitAnd:
      return "&";
    case ::BitAndNot:
      return "&^";

    case ::Add:
      return "+";
    case ::Subtract:
      return "-";
    case ::BitOr:
      return "|";
    case ::BitXor:
      return "^";

    case ::Equal:
      return "==";
    case ::NotEqual:
      return "!=";
    case ::LessThan:
      return "<";
    case ::LessEqual:
      return "<=";
    case ::MoreThan:
      return ">";
    case ::MoreEqual:
      return ">=";

    case ::LogicOr:
      return "||";

    case ::LogicAnd:
      return "&&";
    }

  NOT_REACHED;
}

template <typename InputPicker, typename OutputPicker, typename Computer, ::BinaryArithmetic ba>
void
BinaryArithmetic<InputPicker, OutputPicker, Computer, ba>::check (ErrorReporter& er,
    const Location& location,
    ExpressionValue& result,
    ExpressionValueList& arguments) const
{
  assert (arguments.size () == 2);
  ExpressionValue& left = arguments[0];
  ExpressionValue& right = arguments[1];

  if (!require_value_or_variable (er, location, result, left) ||
      !require_value_or_variable (er, location, result, right))
    {
      return;
    }

  if (left.type->is_untyped () &&
      right.type->is_untyped ())
    {
      const type::Type* t = InputPicker::pick (left.type, right.type);
      if (t == NULL)
        {
          er.cannot_be_applied (location, binary_arithmetic_external_symbol (ba), left.type, right.type);
          result.expression_kind = ErrorExpressionKind;
          return;
        }
      left.convert (t);
      right.convert (t);

      result.expression_kind = ValueExpressionKind;
      result.type = OutputPicker::pick (t, left, right);
      Computer::compute (result, t, left, right);
      result.intrinsic_mutability = Immutable;
      result.indirection_mutability = Immutable;

      return;
    }

  if (!(are_assignable (left.type, left.value, right.type) ||
        are_assignable (right.type, right.value, left.type)))
    {
      er.cannot_be_applied (location, binary_arithmetic_external_symbol (ba), left.type, right.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  const type::Type* in_type = choose (left.type, right.type);
  left.convert (in_type);
  right.convert (in_type);

  if (InputPicker::pick (in_type, in_type) == NULL)
    {
      er.cannot_be_applied (location, binary_arithmetic_external_symbol (ba), left.type, right.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = OutputPicker::pick (in_type, left, right);
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Immutable;

  Computer::compute (result, in_type, left, right);
}

template <typename InputPicker, typename OutputPicker, typename Computer, ::BinaryArithmetic ba>
runtime::Operation*
BinaryArithmetic<InputPicker, OutputPicker, Computer, ba>::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return Computer::generate_code (result, arg_vals, arg_ops);
}

template <typename B, ::BinaryArithmetic ba>
void BinaryShift<B, ba>::check (ErrorReporter& er,
                                const Location& location,
                                ExpressionValue& result,
                                ExpressionValueList& arguments) const
{
  assert (arguments.size () == 2);
  ExpressionValue& left = arguments[0];
  ExpressionValue& right = arguments[1];

  if (!require_value_or_variable (er, location, result, left) ||
      !require_value_or_variable (er, location, result, right))
    {
      return;
    }

  if (!(right.type->is_typed_unsigned_integer () || right.type->is_untyped_numeric ()))
    {
      er.cannot_be_applied (location, binary_arithmetic_external_symbol (ba), left.type, right.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  if (right.value.present)
    {
      if (!right.value.representable (right.type, type::Uint::instance ()))
        {
          er.cannot_be_applied (location, binary_arithmetic_external_symbol (ba), left.type, right.type);
          result.expression_kind = ErrorExpressionKind;
          return;
        }
      right.value.convert (right.type, type::Uint::instance ());
      right.type = type::Uint::instance ();
    }

  if (!left.type->is_integral ())
    {
      er.cannot_be_applied (location, binary_arithmetic_external_symbol (ba), left.type, right.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = left.type;
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Immutable;

  if (left.value.present &&
      right.value.present)
    {
      result.value.present = true;
      switch (result.type->underlying_kind ())
        {
        case Uint8_Kind:
          result.value.uint8_value = B () (left.value.uint8_value, right.value.uint_value);
          break;
        case Uint16_Kind:
          result.value.uint16_value = B () (left.value.uint16_value, right.value.uint_value);
          break;
        case Uint32_Kind:
          result.value.uint32_value = B () (left.value.uint32_value, right.value.uint_value);
          break;
        case Uint64_Kind:
          result.value.uint64_value = B () (left.value.uint64_value, right.value.uint_value);
          break;
        case Int8_Kind:
          result.value.int8_value = B () (left.value.int8_value, right.value.uint_value);
          break;
        case Int16_Kind:
          result.value.int16_value = B () (left.value.int16_value, right.value.uint_value);
          break;
        case Int32_Kind:
          result.value.int32_value = B () (left.value.int32_value, right.value.uint_value);
          break;
        case Int64_Kind:
          result.value.int64_value = B () (left.value.int64_value, right.value.uint_value);
          break;
        case Uint_Kind:
          result.value.uint_value = B () (left.value.uint_value, right.value.uint_value);
          break;
        case Int_Kind:
          result.value.int_value = B () (left.value.int_value, right.value.uint_value);
          break;
        case Uintptr_Kind:
          result.value.uintptr_value = B () (left.value.uintptr_value, right.value.uint_value);
          break;
        case Untyped_Rune_Kind:
          result.value.untyped_rune_value = B () (left.value.untyped_rune_value, right.value.uint_value);
          break;
        case Untyped_Integer_Kind:
          result.value.untyped_integer_value = B () (left.value.untyped_integer_value, right.value.uint_value);
          break;
        default:
          TYPE_NOT_REACHED (*result.type);
        }
    }
}

template <typename B, ::BinaryArithmetic ba>
runtime::Operation* BinaryShift<B, ba>::generate_code (const ExpressionValue& result,
    const ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return B::generate_code (result, arg_vals, arg_ops);
}

template class BinaryArithmetic<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SMultiply>, ::Multiply>;
template class BinaryArithmetic<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SDivide>, ::Divide>;
template class BinaryArithmetic<type::Integral, PassThroughPicker, BinaryValueComputer<SModulus>, ::Modulus>;
template class BinaryShift <LeftShifter, ::LeftShift>;
template class BinaryShift <RightShifter, ::RightShift>;
template class BinaryArithmetic<type::Integral, PassThroughPicker, BinaryValueComputer<SBitAnd>, ::BitAnd>;
template class BinaryArithmetic<type::Integral, PassThroughPicker, BinaryValueComputer<SBitAndNot>, ::BitAndNot>;
template class BinaryArithmetic<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SAdd>, ::Add>;
template class BinaryArithmetic<type::Arithmetic, PassThroughPicker, BinaryValueComputer<SSubtract>, ::Subtract>;
template class BinaryArithmetic<type::Integral, PassThroughPicker, BinaryValueComputer<SBitOr>, ::BitOr>;
template class BinaryArithmetic<type::Integral, PassThroughPicker, BinaryValueComputer<SBitXor>, ::BitXor>;
template class BinaryArithmetic<type::Comparable, BooleanPicker, BinaryValueComputer<SEqual>, ::Equal>;
template class BinaryArithmetic<type::Comparable, BooleanPicker, BinaryValueComputer<SNotEqual>, ::NotEqual>;
template class BinaryArithmetic<type::Orderable, BooleanPicker, BinaryValueComputer<SLessThan>, ::LessThan>;
template class BinaryArithmetic<type::Orderable, BooleanPicker, BinaryValueComputer<SLessEqual>, ::LessEqual>;
template class BinaryArithmetic<type::Orderable, BooleanPicker, BinaryValueComputer<SMoreThan>, ::MoreThan>;
template class BinaryArithmetic<type::Orderable, BooleanPicker, BinaryValueComputer<SMoreEqual>, ::MoreEqual>;
template class BinaryArithmetic<type::Logical, BooleanPicker, LogicOrComputer, ::LogicOr>;
template class BinaryArithmetic<type::Logical, BooleanPicker, LogicAndComputer, ::LogicAnd>;

void SMultiply::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  multiply (out, type, left, right);
}

runtime::Operation*
SMultiply::generate_code (const ExpressionValue& result,
                          const ExpressionValueList& arg_vals,
                          runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<Multiplier> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

void SDivide::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  divide (out, type, left, right);
}

runtime::Operation*
SDivide::generate_code (const ExpressionValue& result,
                        const ExpressionValueList& arg_vals,
                        runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<Divider> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

void SModulus::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  modulus (out, type, left, right);
}

runtime::Operation*
SModulus::generate_code (const ExpressionValue& result,
                         const ExpressionValueList& arg_vals,
                         runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<Modulizer> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

runtime::Operation*
LeftShifter::generate_code (const ExpressionValue& result,
                            const ExpressionValueList& arg_vals,
                            runtime::ListOperation* arg_ops)
{
  return runtime::make_shift<LeftShifter> (result.type, arg_ops->list[0], MakeConvertToUint (arg_ops->list[1], arg_vals[1].type));
}

runtime::Operation*
RightShifter::generate_code (const ExpressionValue& result,
                             const ExpressionValueList& arg_vals,
                             runtime::ListOperation* arg_ops)
{
  return runtime::make_shift<RightShifter> (result.type, arg_ops->list[0], MakeConvertToUint (arg_ops->list[1], arg_vals[1].type));
}

void SBitAnd::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  bit_and (out, type, left, right);
}

runtime::Operation*
SBitAnd::generate_code (const ExpressionValue& result,
                        const ExpressionValueList& arg_vals,
                        runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<BitAnder> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

void SBitAndNot::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  bit_and_not (out, type, left, right);
}

runtime::Operation*
SBitAndNot::generate_code (const ExpressionValue& result,
                           const ExpressionValueList& arg_vals,
                           runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<BitAndNotter> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

void SAdd::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  add (out, type, left, right);
}

runtime::Operation*
SAdd::generate_code (const ExpressionValue& result,
                     const ExpressionValueList& arg_vals,
                     runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<Adder> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

void SSubtract::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  subtract (out, type, left, right);
}

runtime::Operation*
SSubtract::generate_code (const ExpressionValue& result,
                          const ExpressionValueList& arg_vals,
                          runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<Subtracter> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

void SBitOr::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  bit_or (out, type, left, right);
}

runtime::Operation*
SBitOr::generate_code (const ExpressionValue& result,
                       const ExpressionValueList& arg_vals,
                       runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<BitOrer> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

void SBitXor::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  bit_xor (out, type, left, right);
}

runtime::Operation*
SBitXor::generate_code (const ExpressionValue& result,
                        const ExpressionValueList& arg_vals,
                        runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_integral<BitXorer> (result.type, arg_ops->list[0], arg_ops->list[1]);
}

void SEqual::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  equal (out, type, left, right);
}

runtime::Operation*
SEqual::generate_code (const ExpressionValue& result,
                       const ExpressionValueList& arg_vals,
                       runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<Equalizer> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

void SNotEqual::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  not_equal (out, type, left, right);
}

runtime::Operation*
SNotEqual::generate_code (const ExpressionValue& result,
                          const ExpressionValueList& arg_vals,
                          runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<NotEqualizer> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

void SLessThan::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  less_than (out, type, left, right);
}

runtime::Operation*
SLessThan::generate_code (const ExpressionValue& result,
                          const ExpressionValueList& arg_vals,
                          runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<LessThaner> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

void SLessEqual::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  less_equal (out, type, left, right);
}

runtime::Operation*
SLessEqual::generate_code (const ExpressionValue& result,
                           const ExpressionValueList& arg_vals,
                           runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<LessEqualizer> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

void SMoreThan::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  more_than (out, type, left, right);
}

runtime::Operation*
SMoreThan::generate_code (const ExpressionValue& result,
                          const ExpressionValueList& arg_vals,
                          runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<MoreThaner> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

void SMoreEqual::operator() (Value& out, const type::Type* type, const Value& left, const Value& right) const
{
  more_equal (out, type, left, right);
}

runtime::Operation*
SMoreEqual::generate_code (const ExpressionValue& result,
                           const ExpressionValueList& arg_vals,
                           runtime::ListOperation* arg_ops)
{
  return runtime::make_binary_arithmetic<MoreEqualizer> (arg_vals[0].type, arg_ops->list[0], arg_ops->list[1]);
}

New::New (const Location& loc)
  : PolymorphicFunction ("new",
                         loc)
{ }

void
New::check (ErrorReporter& er,
            const Location& location,
            ExpressionValue& result,
            ExpressionValueList& arguments) const
{
  if (arguments.size () != 1)
    {
      er.func_expects_count (location, "new", 1, arguments.size ());
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  ExpressionValue& arg = arguments[0];

  if (!require_type (er, location, result, arg))
    {
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = arg.type->get_pointer ();
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Mutable;
}

void New::compute_receiver_access (const semantic::ExpressionValueList& args,
                                   ReceiverAccess& receiver_access,
                                   bool& flag) const
{
  receiver_access = AccessNone;
  flag = false;
}

runtime::Operation* New::generate_code (const semantic::ExpressionValue& result,
                                        const semantic::ExpressionValueList& arg_vals,
                                        runtime::ListOperation* arg_ops) const
{
  return new runtime::NewOp (arg_vals.front ().type);
}

Move::Move (const Location& loc)
  : PolymorphicFunction ("move",
                         loc)
{ }

void
Move::check (ErrorReporter& er,
             const Location& location,
             ExpressionValue& result,
             ExpressionValueList& arguments) const
{
  if (arguments.size () != 1)
    {
      er.func_expects_count (location, "move", 1, arguments.size ());
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  ExpressionValue& arg = arguments[0];

  const type::Type* in = arg.type;
  const type::Type* out = in->move ();
  if (out == NULL)
    {
      er.cannot_be_applied (location, "move", in);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  if (!require_value_or_variable (er, location, result, arg))
    {
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = out;
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Mutable;
}

void
Move::compute_receiver_access (const semantic::ExpressionValueList& args,
                               ReceiverAccess& receiver_access,
                               bool& flag) const
{
  // Check if a mutable pointer escapes.
  receiver_access = AccessNone;
  flag = false;
  for (ExpressionValueList::const_iterator pos = args.begin (),
       limit = args.end ();
       pos != limit;
       ++pos)
    {
      receiver_access = std::max (receiver_access, pos->receiver_access);
    }
}

runtime::Operation* Move::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return new runtime::MoveOp (arg_ops);
}

Merge::Merge (const Location& loc)
  : PolymorphicFunction ("merge",
                         loc)
{ }

void
Merge::check (ErrorReporter& er,
              const Location& location,
              ExpressionValue& result,
              ExpressionValueList& arguments) const
{
  if (arguments.size () != 1)
    {
      er.func_expects_count (location, "merge", 1, arguments.size ());
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  ExpressionValue& arg = arguments[0];

  const type::Type* in = arg.type;
  const type::Type* out = in->merge_change ();
  if (out == NULL)
    {
      er.cannot_be_applied (location, "merge", in);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  if (!require_value_or_variable (er, location, result, arg))
    {
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = out;
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Mutable;
}

void Merge::compute_receiver_access (const semantic::ExpressionValueList& args,
                                     ReceiverAccess& receiver_access,
                                     bool& flag) const
{
  // Check if a mutable pointer escapes.
  receiver_access = AccessNone;
  flag = false;
  for (ExpressionValueList::const_iterator pos = args.begin (),
       limit = args.end ();
       pos != limit;
       ++pos)
    {
      receiver_access = std::max (receiver_access, pos->receiver_access);
    }
}

runtime::Operation* Merge::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return new runtime::MergeOp (arg_ops);
}

Len::Len (const Location& loc)
  : PolymorphicFunction ("len",
                         loc)
{ }

void
Len::check (ErrorReporter& er,
            const Location& location,
            ExpressionValue& result,
            ExpressionValueList& arguments) const
{
  if (arguments.size () != 1)
    {
      er.func_expects_count (location, "len", 1, arguments.size ());
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  ExpressionValue& arg = arguments[0];

  if (!require_value_or_variable (er, location, result, arg))
    {
      return;
    }

  const type::Type* type = arg.type;
  if (type->underlying_kind () != Slice_Kind)
    {
      er.cannot_be_applied (location, "[:]", type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = &named_int;
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Immutable;
}

runtime::Operation* Len::generate_code (const semantic::ExpressionValue& result,
                                        const semantic::ExpressionValueList& arg_vals,
                                        runtime::ListOperation* arg_ops) const
{
  return new runtime::LenOp (arg_ops);
}

Append::Append (const Location& loc)
  : PolymorphicFunction ("append",
                         loc)
{ }

void
Append::check (ErrorReporter& er,
               const Location& location,
               ExpressionValue& result,
               ExpressionValueList& arguments) const
{
  if (arguments.size () != 2)
    {
      er.func_expects_count (location, "append", 2, arguments.size ());
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  ExpressionValue& slice = arguments[0];
  ExpressionValue& element = arguments[1];

  if (!require_value_or_variable (er, location, result, slice) ||
      !require_value_or_variable (er, location, result, element))
    {
      return;
    }

  if (slice.type->underlying_kind () != Slice_Kind)
    {
      er.cannot_be_applied (location, "append", slice.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  const type::Slice* st = slice.type->underlying_type ()->to_slice ();
  if (st != NULL &&
      !are_identical (st->base_type, element.type))
    {
      er.func_expects_arg (location, "append", 2, st->base_type, element.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = st;
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Mutable;
}

runtime::Operation* Append::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return make_append (arg_vals[0].type->underlying_type ()->to_slice (), arg_ops);
}

Copy::Copy (const Location& loc)
  : PolymorphicFunction ("copy",
                         loc)
{ }

void
Copy::check (ErrorReporter& er,
             const Location& location,
             ExpressionValue& result,
             ExpressionValueList& arguments) const
{
  if (arguments.size () != 1)
    {
      er.func_expects_count (location, "copy", 1, arguments.size ());
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  ExpressionValue& arg = arguments[0];

  if (!require_value_or_variable (er, location, result, arg))
    {
      return;
    }

  switch (arg.type->underlying_kind ())
    {
    case Slice_Kind:
    {
      const type::Slice* st = arg.type->underlying_type ()->to_slice ();
      if (st->base_type->contains_pointer ())
        {
          er.leaks_pointers (location);
          result.expression_kind = ErrorExpressionKind;
          return;
        }
    }
    break;
    case String_Kind:
      // Okay.
      break;
    default:
      er.cannot_be_applied (location, "copy", arg.type);
      result.expression_kind = ErrorExpressionKind;
      return;
    }

  result.expression_kind = ValueExpressionKind;
  result.type = arg.type;
  result.intrinsic_mutability = Immutable;
  result.indirection_mutability = Mutable;
}

void
Copy::compute_receiver_access (const semantic::ExpressionValueList& args,
                               ReceiverAccess& receiver_access,
                               bool& flag) const
{
  // Check if a mutable pointer escapes.
  receiver_access = AccessNone;
  flag = false;
  for (ExpressionValueList::const_iterator pos = args.begin (),
       limit = args.end ();
       pos != limit;
       ++pos)
    {
      receiver_access = std::max (receiver_access, pos->receiver_access);
    }
}

runtime::Operation* Copy::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return new runtime::CopyOp (arg_vals.front ().type, arg_ops);
}

Println::Println (const Location& loc)
  : PolymorphicFunction ("println",
                         loc)
{ }

void
Println::check (ErrorReporter& er,
                const Location& location,
                ExpressionValue& result,
                ExpressionValueList& arguments) const
{
  for (ExpressionValueList::iterator pos = arguments.begin (), limit = arguments.end ();
       pos != limit;
       ++pos)
    {
      pos->convert (pos->type->default_type ());
    }

  result.expression_kind = VoidExpressionKind;
}

void Println::compute_receiver_access (const semantic::ExpressionValueList& args,
                                       ReceiverAccess& receiver_access,
                                       bool& flag) const
{
  // Check if a mutable pointer escapes.
  receiver_access = AccessNone;
  flag = false;
  for (ExpressionValueList::const_iterator pos = args.begin (),
       limit = args.end ();
       pos != limit;
       ++pos)
    {
      receiver_access = std::max (receiver_access, pos->receiver_access);
    }
}

runtime::Operation* Println::generate_code (const semantic::ExpressionValue& result,
    const semantic::ExpressionValueList& arg_vals,
    runtime::ListOperation* arg_ops) const
{
  return new runtime::PrintlnOp (arg_vals, arg_ops);
}

Posate posate_temp ((util::Location ()));
Negate negate_temp ((util::Location ()));
LogicNot logic_not_temp ((util::Location ()));
Complement complement_temp ((util::Location ()));

Multiply multiply_temp ("*", (util::Location ()));
Divide divide_temp ("/", (util::Location ()));
Modulus modulus_temp ("%", (util::Location ()));
LeftShift left_shift_temp ("<<", (util::Location ()));
RightShift right_shift_temp (">>", (util::Location ()));
BitAnd bit_and_temp ("&", (util::Location ()));
BitAndNot bit_and_not_temp ("&^", (util::Location ()));
Add add_temp ("+", (util::Location ()));
Subtract subtract_temp ("-", (util::Location ()));
BitOr bit_or_temp ("|", (util::Location ()));
BitXor bit_xor_temp ("^", (util::Location ()));
Equal equal_temp ("==", (util::Location ()));
NotEqual not_equal_temp ("!=", (util::Location ()));
LessThan less_than_temp ("<", (util::Location ()));
LessEqual less_equal_temp ("<=", (util::Location ()));
MoreThan more_than_temp (">", (util::Location ()));
MoreEqual more_equal_temp (">=", (util::Location ()));
LogicOr logic_or_temp ("||", (util::Location ()));
LogicAnd logic_and_temp ("&&", (util::Location ()));

}
