#include "ast.hpp"
#include "Symbol.hpp"
#include <error.h>
#include "semantic.hpp"
#include "Activation.hpp"
#include "action.hpp"
#include "field.hpp"
#include "parameter.hpp"
#include "Callable.hpp"

using namespace Type;

typed_value_t
ImplicitlyConvert (ast_t*& expr, const Type::Type* target)
{
  typed_value_t tv = expr->typed_value;
  if (!Type::Identitical (target, tv.type) && tv.AssignableTo (target))
    {
      tv = tv.Convert (expr->location, target);
      expr = new ast_implicit_conversion_expr_t (expr->location.Line, expr);
      expr->typed_value = tv;
    }
  return tv;
}

typed_value_t
ImplicitlyConvertToDefault (ast_t*& expr)
{
  typed_value_t tv = expr->typed_value;
  const Type::Type* target = tv.type->DefaultType ();
  if (!Type::Identitical (target, tv.type))
    {
      if (!tv.AssignableTo (target))
        {
          error_at_line (-1, 0, expr->location.File.c_str (), expr->location.Line,
                         "cannot convert to real type (E68)");
        }
      tv = tv.Convert (expr->location, target);
      expr = new ast_implicit_conversion_expr_t (expr->location.Line, expr);
      expr->typed_value = tv;
    }
  return tv;
}

void
check_assignment (typed_value_t left_tv,
                  typed_value_t right_tv,
                  const ast_t& node,
                  const char* conversion_message,
                  const char* leak_message)
{
  assert (left_tv.type != NULL);
  assert (right_tv.type != NULL);

  if (left_tv.kind != typed_value_t::REFERENCE)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "target of assignment is not an lvalue (E48)");
    }

  if (left_tv.intrinsic_mutability != MUTABLE)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "target of assignment is not mutable (E13)");
    }

  if (right_tv.kind != typed_value_t::VALUE)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "source of assignment is not an rvalue (E132)");
    }

  if (!(
        type_is_equal (left_tv.type, right_tv.type) ||
        (Type::type_cast<Type::Pointer> (type_strip(left_tv.type)) && right_tv.type == Type::Nil::Instance ())
      ))
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     conversion_message, left_tv.type->ToString ().c_str (), right_tv.type->ToString ().c_str ());
    }

  if (type_contains_pointer (left_tv.type))
    {
      if (left_tv.dereference_mutability < right_tv.dereference_mutability)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "%s", leak_message);
        }
    }
}

static void checkArgs (ast_t * node, TypedValueListType& tvlist);

static void checkCall (ast_t& node,
                       const Type::Signature* signature,
                       typed_value_t return_value,
                       ast_t* argsnode,
                       const TypedValueListType& args)
{
  size_t argument_count = args.size ();
  size_t parameter_count = signature->Arity ();
  if (argument_count != parameter_count)
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "call expects %zd arguments but given %zd (E26)",
                     parameter_count, argument_count);
    }

  size_t idx = 0;
  for (Type::Signature::const_iterator pos = signature->Begin (),
       limit = signature->End ();
       pos != limit;
       ++pos, ++idx)
    {
      typed_value_t parameter_tv = typed_value_t::make_ref ((*pos)->value);
      typed_value_t argument_tv  = ImplicitlyConvert (argsnode->at (idx), parameter_tv.type);
      check_assignment (parameter_tv, argument_tv, *argsnode->at (idx),
                        "incompatible types (%s) = (%s) (E116)",
                        "argument leaks mutable pointers (E117)");
    }

  // Set the return type.
  node.typed_value = return_value;
}

static bool
in_mutable_section (const ast_t* node)
{
  return node->GetAction () != NULL &&
         node->GetActivation () != NULL;
}

static typed_value_t
insertImplicitDereference (ast_t*& expr, typed_value_t tv)
{
  expr = new ast_implicit_dereference_expr_t (expr->location.Line, expr);
  tv = typed_value_t::implicit_dereference (tv);
  expr->typed_value = tv;
  return tv;
}

typed_value_t
CheckAndImplicitlyDereference (ast_t*& expr)
{
  typed_value_t tv = type_check_expr (expr);
  if (tv.isReference ())
    {
      // Insert a dereference node.
      tv = insertImplicitDereference (expr, tv);
    }
  return tv;
}

typed_value_t
CheckAndImplicitlyDereferenceAndConvert (ast_t*& expr, const Type::Type* type)
{
  CheckAndImplicitlyDereference (expr);
  ImplicitlyConvert (expr, type);
  return expr->typed_value;
}

typed_value_t
CheckAndImplicitlyDereferenceAndConvertToDefault (ast_t*& expr)
{
  CheckAndImplicitlyDereference (expr);
  ImplicitlyConvertToDefault (expr);
  return expr->typed_value;
}


static typed_value_t
insertExplicitDereference (ast_t*& expr, typed_value_t tv)
{
  expr = new ast_dereference_expr_t (expr->location.Line, expr);
  tv = typed_value_t::dereference (tv);
  expr->typed_value = tv;
  return tv;
}

typed_value_t
CheckExpectReference (ast_t* expr)
{
  typed_value_t tv = type_check_expr (expr);
  if (!tv.isReference ())
    {
      error_at_line (EXIT_FAILURE, 0, expr->location.File.c_str (), expr->location.Line, "expected reference (E14)");
    }
  return tv;
}

struct check_visitor : public ast_visitor_t
{
  ast_t* ptr;

  check_visitor (ast_t* p) : ptr (p) { }

  void default_action (ast_t& node)
  {
    ast_not_reached(node);
  }

  void visit (ast_type_expr_t& node)
  {
    const Type::Type* type = process_type_spec (node.type_spec (), true);
    node.typed_value = typed_value_t (type);
  }

  void visit (ast_indexed_port_call_expr_t& node)
  {
    const std::string& port_identifier = ast_get_identifier (node.identifier ());
    const Type::Type *this_type = node.GetReceiverType ();
    const Type::Type *type = type_select (this_type, port_identifier);

    if (type == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E15)", port_identifier.c_str ());
      }

    const Type::Array* array_type = Type::type_cast<Type::Array> (type);
    if (!array_type)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "%s is not an array of ports (E16)", port_identifier.c_str ());
      }

    const Type::Function* push_port_type = Type::type_cast<Type::Function> (array_type->Base ());

    if (push_port_type == NULL || push_port_type->kind != Type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "%s is not an array of ports (E17)", port_identifier.c_str ());
      }

    typed_value_t index_tv = CheckAndImplicitlyDereference (node.index_ref ());

    typed_value_t::index (node.index ()->location, typed_value_t::make_ref (array_type, IMMUTABLE, IMMUTABLE), index_tv);

    ast_t *args = node.args ();
    TypedValueListType tvlist;
    checkArgs (args, tvlist);
    checkCall (node, push_port_type->GetSignature (), push_port_type->GetReturnParameter ()->value, args, tvlist);
    node.field = type_select_field (this_type, port_identifier);
    node.array_type = array_type;
  }

  void visit (ast_identifier_expr_t& node)
  {
    ast_t *identifier_node = node.child ();
    const std::string& identifier = ast_get_identifier (identifier_node);
    Symbol *symbol = node.FindSymbol (identifier);
    if (symbol == NULL)
      {
        error_at_line (-1, 0, identifier_node->location.File.c_str (),
                       identifier_node->location.Line, "%s is not defined (E18)",
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
        node.typed_value = symbol.value ();
      }

      void visit (const ::Template& symbol)
      {
        node.typed_value = symbol.value ();
      }

      void visit (const ::Function& symbol)
      {
        node.typed_value = symbol.value ();
      }

      void visit (const ParameterSymbol& symbol)
      {
        node.typed_value = symbol.value;
      }

      void visit (const TypeSymbol& symbol)
      {
        node.typed_value = typed_value_t (symbol.type);
      }

      void visit (const TypedConstantSymbol& symbol)
      {
        node.typed_value = symbol.value;
      }

      void visit (const VariableSymbol& symbol)
      {
        node.typed_value = symbol.value;
      }

      void visit (const HiddenSymbol& symbol)
      {
        error_at_line (-1, 0, node.location.File.c_str (),
                       node.location.Line, "%s is not accessible in this scope (E19)",
                       symbol.identifier.c_str ());
      }
    };
    visitor v (node);
    symbol->accept (v);

    node.symbol = symbol;
  }

  void visit (ast_select_expr_t& node)
  {
    const std::string& identifier = ast_get_identifier (node.identifier ());
    typed_value_t in = type_check_expr (node.base ());
    assert (in.type != NULL);

    if (in.isReference () && type_dereference (in.type))
      {
        // Pointer reference.
        // Insert an implicit dereference.
        in = insertImplicitDereference (node.base_ref (), in);
      }

    if (in.IsValue () && type_dereference (in.type))
      {
        // Pointer value.
        // Insert an explicit dereference.
        in = insertExplicitDereference (node.base_ref (), in);
      }

    if (in.isReference ())
      {
        typed_value_t out = typed_value_t::select (in, identifier);
        if (out.IsError ())
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "cannot select %s from expression of type %s (E20)",
                           identifier.c_str (), in.type->ToString ().c_str ());
          }
        node.typed_value = out;
      }
    else if (in.IsValue ())
      {
        unimplemented;
      }
  }

  void visit (ast_dereference_expr_t& node)
  {
    typed_value_t in = CheckAndImplicitlyDereference (node.child_ref ());
    typed_value_t out = typed_value_t::dereference (in);
    if (out.IsError ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "incompatible types: %s (E21)", in.type->ToString ().c_str ());
      }
    node.typed_value = out;
  }

  void visit (ast_literal_expr_t& node)
  {
    // Do nothing.
  }

  void check_address_of (ast_address_of_expr_t& node)
  {
    ast_t* expr = node.child ();
    typed_value_t in = expr->typed_value;
    typed_value_t out = typed_value_t::address_of (in);
    if (out.IsError ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "incompatible types: %s (E22)", in.type->ToString ().c_str ());
      }
    node.typed_value = out;
  }

  void visit (ast_address_of_expr_t& node)
  {
    typed_value_t in = CheckExpectReference (node.child ());
    typed_value_t out = typed_value_t::address_of (in);
    if (out.IsError ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "E45: incompatible types: %s (E23)", in.type->ToString ().c_str ());
      }
    node.typed_value = out;
  }

  void visit (ast_unary_arithmetic_expr_t& node)
  {
    typed_value_t in = CheckAndImplicitlyDereference (node.child_ref ());
    switch (node.arithmetic)
      {
      case LogicNot:
        node.typed_value = in.LogicNot (node.location);
        return;
      case Negate:
        node.typed_value = in.Negate (node.location);
        return;
      }
    not_reached;
  }

  void visit (ast_binary_arithmetic_expr_t& node)
  {
    typed_value_t left = CheckAndImplicitlyDereference (node.left_ref ());
    typed_value_t right = CheckAndImplicitlyDereference (node.right_ref ());
    switch (node.arithmetic)
      {
      case Multiply:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::Multiply (node.location, left, right);
        return;
      case Divide:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::Divide (node.location, left, right);
        return;
      case Modulus:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::Modulus (node.location, left, right);
        return;
      case LeftShift:
        if (left.value.present && !right.value.present)
          {
            left = ImplicitlyConvertToDefault (node.left_ref ());
          }
        if (right.value.present && !left.value.present)
          {
            right = ImplicitlyConvertToDefault (node.right_ref ());
          }
        node.typed_value = typed_value_t::LeftShift (node.location, left, right);
        return;
      case RightShift:
        if (left.value.present && !right.value.present)
          {
            left = ImplicitlyConvertToDefault (node.left_ref ());
          }
        if (right.value.present && !left.value.present)
          {
            right = ImplicitlyConvertToDefault (node.right_ref ());
          }
        node.typed_value = typed_value_t::RightShift (node.location, left, right);
        return;
      case BitAnd:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::BitAnd (node.location, left, right);
        return;
      case BitAndNot:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::BitAndNot (node.location, left, right);
        return;
      case Add:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::Add (node.location, left, right);
        return;
      case Subtract:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::Subtract (node.location, left, right);
        return;
      case BitOr:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::BitOr (node.location, left, right);
        return;
      case BitXor:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::BitXor (node.location, left, right);
        return;
      case Equal:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::Equal (node.location, left, right);
        return;
      case NotEqual:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::NotEqual (node.location, left, right);
        return;
      case LessThan:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::LessThan (node.location, left, right);
        return;
      case LessEqual:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::LessEqual (node.location, left, right);
        return;
      case MoreThan:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::MoreThan (node.location, left, right);
        return;
      case MoreEqual:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::MoreEqual (node.location, left, right);
        return;
      case LogicOr:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::LogicOr (node.location, left, right);
        return;
      case LogicAnd:
        left = ImplicitlyConvert (node.left_ref (), right.type);
        right = ImplicitlyConvert (node.right_ref (), left.type);
        node.typed_value = typed_value_t::LogicAnd (node.location, left, right);
        return;
      }
    not_reached;
  }

  void visit (ast_call_expr_t& node)
  {
    // Analyze the args.
    TypedValueListType tvlist;
    checkArgs (node.args (), tvlist);

    // Analyze the callee.
    // Expecting a value.
    typed_value_t expr_tv = CheckAndImplicitlyDereference (node.expr_ref ());

    if (expr_tv.kind == typed_value_t::TYPE)
      {
        // Conversion.
        if (tvlist.size () != 1)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "conversion requires exactly one argument (E69)");
          }

        node.typed_value = tvlist[0].Convert (node.location, expr_tv.type);
        node.IsCall = false;
        return;
      }

    node.IsCall = true;

    const Type::Template* tt = Type::type_strip_cast <Type::Template> (expr_tv.type);
    if (tt != NULL)
      {
        ::Template* t = expr_tv.value.template_value ();
        expr_tv = t->instantiate (tvlist);
        node.expr ()->typed_value = expr_tv;
      }

    struct visitor : public Type::Visitor
    {
      check_visitor& rvalue_visitor;
      ast_call_expr_t& node;
      const TypedValueListType& tvlist;

      visitor (check_visitor& rv,
               ast_call_expr_t& n,
               const TypedValueListType& tvl)
        : rvalue_visitor (rv)
        , node (n)
        , tvlist (tvl)
      { }

      void default_action (const Type::Type& type)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "cannot call %s (E27)", type.ToString ().c_str ());
      }

      void visit (const Type::Function& type)
      {
        switch (type.kind)
          {
          case Type::Function::FUNCTION:
            // No restrictions on caller.
            checkCall (node, type.GetSignature (), type.GetReturnParameter ()->value, node.args (), tvlist);
            break;

          case Type::Function::PUSH_PORT:
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "push ports cannot be called (E28)");

            break;

          case Type::Function::PULL_PORT:
            // Must be called from either a getter, an action, or reaction.
            if (node.GetGetter () == NULL &&
                node.GetAction () == NULL)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "pull ports may only be called from a getter, an action, or a reaction (E29)");
              }

            checkCall (node, type.GetSignature (), type.GetReturnParameter ()->value, node.args (), tvlist);
            if (in_mutable_section (&node))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "cannot call pull port in mutable section (E30)");
              }
            break;
          }
      }

      void visit (const Type::Method& type)
      {
        switch (type.kind)
          {
          case Type::Method::METHOD:
          {
            // No restrictions on caller.
            checkCall (node, type.signature, type.return_parameter->value, node.args (), tvlist);

            if (type_dereference (type.receiver_type) != NULL)
              {
                // Method expects a pointer.  Insert address of.
                // Strip off implicit deref and select.
                ast_t* receiver_select_expr = node.expr ()->at (0)->at (0);
                ast_address_of_expr_t* e = new ast_address_of_expr_t (node.location.Line, receiver_select_expr);
                rvalue_visitor.check_address_of (*e);
                node.expr ()->at (0)->at(0) = e;
              }

            typed_value_t argument_tv = node.expr ()->at (0)->at (0)->typed_value;
            typed_value_t parameter_tv = typed_value_t::make_ref (type.this_parameter->value);
            check_assignment (parameter_tv, argument_tv, node,
                              "call expects %s but given %s (E118)",
                              "argument leaks mutable pointers (E119)");
          }
          break;
          case Type::Method::INITIALIZER:
          {
            // Caller must be an initializer.
            Initializer* initializer = node.GetInitializer ();

            if (initializer == NULL)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "initializers may only be called from initializers (E31)");
              }

            checkCall (node, type.signature, type.return_parameter->value, node.args (), tvlist);

            assert (type_dereference (type.receiver_type) != NULL);

            // Method expects a pointer.  Insert address of.
            // Strip off implicit deref and select.
            ast_t* receiver_select_expr = node.expr ()->at (0)->at (0);
            ast_address_of_expr_t* e = new ast_address_of_expr_t (node.location.Line, receiver_select_expr);
            rvalue_visitor.check_address_of (*e);
            node.expr ()->at (0)->at (0) = e;

            typed_value_t argument_tv = node.expr ()->at (0)->at (0)->typed_value;
            typed_value_t parameter_tv = typed_value_t::make_ref (type.this_parameter->value);
            check_assignment (parameter_tv, argument_tv, node,
                              "call expects %s but given %s (E120)",
                              "argument leaks mutable pointers (E121)");
          }
          break;

          case Type::Method::GETTER:
          {
            unimplemented;
            // // Must be called from either a getter, an action, or reaction.
            // if (get_current_getter (&node) == NULL &&
            //         get_current_action (&node) == NULL)
            //     {
            //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
            //                        "getters may only be called from a getter, an action, or a reaction (E32)");
            //     }

            // rvalue_visitor.checkCall (node, type.signature, type.return_parameter->value, node.args ());
            // if (in_mutable_section (&node))
            //     {
            //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
            //                        "cannot call getter in mutable section (E33)");
            //     }
          }
          break;
          case Type::Method::REACTION:
          {
            unimplemented;
          }
          break;
          }
      }
    };

    visitor v (*this, node, tvlist);
    expr_tv.type->Accept (v);
  }

  void visit (ast_push_port_call_expr_t& node)
  {
    ast_t *expr = node.identifier ();
    ast_t *args = node.args ();
    const std::string& port_identifier = ast_get_identifier (expr);
    const Type::Type *this_type = node.GetReceiverType ();
    const Type::Function *push_port_type = Type::type_cast<Type::Function> (type_select (this_type, port_identifier));
    if (push_port_type == NULL || push_port_type->kind != Type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E34)", port_identifier.c_str ());
      }
    TypedValueListType tvlist;
    checkArgs (args, tvlist);
    checkCall (node, push_port_type->GetSignature (), push_port_type->GetReturnParameter ()->value, args, tvlist);
    node.field = type_select_field (this_type, port_identifier);
  }

  void visit (ast_index_expr_t& node)
  {
    typed_value_t base_tv = CheckExpectReference (node.base_ref ());
    typed_value_t idx_tv = CheckAndImplicitlyDereference (node.index_ref ());
    typed_value_t result = typed_value_t::index (node.location, base_tv, idx_tv);
    if (result.IsError ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "incompatible types (%s)[%s] (E35)",
                       base_tv.type->ToString ().c_str (),
                       idx_tv.type->ToString ().c_str ());
      }
    node.typed_value = result;
  }

  void visit (ast_slice_expr_t& node)
  {
    typed_value_t base_tv = CheckExpectReference (node.base_ref ());
    typed_value_t low_tv = CheckAndImplicitlyDereference (node.low_ref ());
    typed_value_t high_tv = CheckAndImplicitlyDereference (node.high_ref ());
    typed_value_t result = typed_value_t::slice (node.location, base_tv, low_tv, high_tv);
    if (result.IsError ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "incompatible types (%s)[%s : %s] (E36)",
                       base_tv.type->ToString ().c_str (),
                       low_tv.type->ToString ().c_str (),
                       high_tv.type->ToString ().c_str ());
      }
    node.typed_value = result;
  }
};

typed_value_t
type_check_expr (ast_t* ptr)
{
  check_visitor check_lvalue_visitor (ptr);
  ptr->accept (check_lvalue_visitor);
  return ptr->typed_value;
}

static void
checkArgs (ast_t * node, TypedValueListType& tvlist)
{
  for (ast_t::iterator child = node->begin (), limit = node->end ();
       child != limit;
       ++child)
    {
      tvlist.push_back (CheckAndImplicitlyDereference (*child));
    }
}

static typed_value_t
check_condition (ast_t*& condition_node)
{
  typed_value_t tv = CheckAndImplicitlyDereferenceAndConvertToDefault (condition_node);
  if (!type_is_boolean (tv.type))
    {
      error_at_line (-1, 0, condition_node->location.File.c_str (),
                     condition_node->location.Line,
                     "cannot convert (%s) to boolean expression in condition (E37)", tv.type->ToString ().c_str ());
    }
  return tv;
}

static void
type_check_statement (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {
    void default_action (ast_t& node)
    {
      ast_not_reached (node);
    }

    void visit (ast_const_t& node)
    {
      ProcessDeclarations (&node);
    }

    void visit (ast_empty_statement_t& node)
    { }

    typed_value_t bind (ast_t& node, ast_t* port_node, ast_t*& reaction_node)
    {
      CheckExpectReference (port_node);
      CheckAndImplicitlyDereference (reaction_node);

      typed_value_t port_tv = port_node->typed_value;
      typed_value_t reaction_tv = reaction_node->typed_value;

      const Type::Function *push_port_type = Type::type_cast<Type::Function> (port_tv.type);

      if (push_port_type == NULL || push_port_type->kind != Type::Function::PUSH_PORT)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "source of bind is not a port (E38)");
        }

      const Type::Method* reaction_type = Type::type_cast<Type::Method> (reaction_tv.type);
      if (reaction_type == NULL || reaction_type->kind != Type::Method::REACTION)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "target of bind is not a reaction (E39)");
        }

      if (!type_is_equal (push_port_type->GetSignature (), reaction_type->signature))
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "cannot bind %s to %s (E40)", push_port_type->ToString ().c_str (), reaction_type->ToString ().c_str ());
        }

      return reaction_tv;
    }

    void visit (ast_bind_push_port_statement_t& node)
    {
      bind (node, node.left (), node.right_ref ());
    }

    void visit (ast_bind_push_port_param_statement_t& node)
    {
      typed_value_t reaction_tv = bind (node, node.left (), node.right_ref ());
      typed_value_t param_tv = CheckAndImplicitlyDereference (node.param_ref ());
      assert (reaction_tv.value.present);
      reaction_t* reaction = reaction_tv.value.reaction_value ();
      if (!reaction->has_dimension ())
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "parameter specified for non-parameterized reaction (E41)");
        }
      typed_value_t dimension = reaction->dimension ();
      typed_value_t::index (node.location, typed_value_t::make_ref (reaction->reaction_type->GetArray (dimension.integral_value ()), IMMUTABLE, IMMUTABLE), param_tv);
    }

    void visit (ast_bind_pull_port_statement_t& node)
    {
      typed_value_t pull_port_tv = CheckExpectReference (node.left ());
      typed_value_t getter_tv = CheckAndImplicitlyDereference (node.right_ref ());

      const Type::Function* pull_port_type = type_cast<Type::Function> (pull_port_tv.type);

      if (pull_port_type == NULL || pull_port_type->kind != Type::Function::PULL_PORT)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "target of bind is not a pull port (E42)");
        }

      const Type::Method* getter_type = type_cast<Type::Method> (getter_tv.type);

      if (getter_type == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "source of bind is not a getter (E43)");
        }

      Type::Function g (Type::Function::FUNCTION, getter_type->signature, getter_type->return_parameter);
      if (!type_is_equal (pull_port_type, &g))
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "cannot bind %s to %s (E44)", pull_port_type->ToString ().c_str (), getter_type->ToString ().c_str ());
        }
    }

    void visit (ast_for_iota_statement_t& node)
    {
      const std::string& identifier = ast_get_identifier (node.identifier ());
      typed_value_t limit = process_array_dimension (node.limit_node_ref ());
      typed_value_t zero = limit;
      zero.zero ();
      Symbol* symbol = new VariableSymbol (identifier, node.identifier (), typed_value_t::make_ref (typed_value_t::make_range (zero, limit, IMMUTABLE, IMMUTABLE)));
      node.symbol = enter_symbol (node, symbol);
      type_check_statement (node.body ());
      node.limit = limit;
    }

    static typed_value_t
    check_assignment_target (ast_t* left)
    {
      typed_value_t tv = CheckExpectReference (left);
      if (tv.intrinsic_mutability != MUTABLE)
        {
          error_at_line (-1, 0, left->location.File.c_str (), left->location.Line,
                         "cannot assign to read-only location of type %s (E45)", tv.type->ToString ().c_str ());
        }

      return tv;
    }

    static void arithmetic_assign (ast_binary_t* node, const char* symbol)
    {
      typed_value_t left_tv = check_assignment_target (node->left ());
      typed_value_t right_tv = CheckAndImplicitlyDereference (node->right_ref ());
      if (!type_is_equal (left_tv.type, right_tv.type))
        {
          error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                         "incompatible types (%s) %s (%s) (E46)", left_tv.type->ToString ().c_str (), symbol, right_tv.type->ToString ().c_str ());
        }

      struct visitor : public Visitor
      {
        ast_t* node;
        const char* symbol;

        visitor (ast_t* n, const char* s) : node (n), symbol (s) { }

        void visit (const NamedType& type)
        {
          type.UnderlyingType ()->Accept (*this);
        }

        void visit (const Int& type)
        {
          // Okay.
        }

        void visit (const Uint& type)
        {
          // Okay.
        }

        void default_action (const Type::Type& type)
        {
          error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                         "incompatible types (%s) %s (%s) (E47)", type.ToString ().c_str (), symbol, type.ToString ().c_str ());
        }
      };
      visitor v (node, symbol);
      left_tv.type->Accept (v);
    }

    void visit (ast_assign_statement_t& node)
    {
      typed_value_t left_tv = check_assignment_target (node.left ());
      typed_value_t right_tv = CheckAndImplicitlyDereferenceAndConvert (node.right_ref (), left_tv.type);
      check_assignment (left_tv, right_tv, node,
                        "incompatible types (%s) = (%s) (E122)",
                        "assignment leaks mutable pointers (E123)");
    }

    void visit (ast_change_statement_t& node)
    {
      // Process the expression.
      typed_value_t tv = CheckAndImplicitlyDereference (node.expr_ref ());
      tv = typed_value_t::change (node.location, tv);

      // Enter the new heap root.
      const std::string& identifier = ast_get_identifier (node.identifier ());
      Symbol* symbol = new VariableSymbol (identifier, &node, typed_value_t::make_ref (tv));
      node.root_symbol = enter_symbol (node, symbol);

      // Enter all parameters and variables in scope that are pointers as pointers to foreign.
      node.Change ();

      // Check the body.
      type_check_statement (node.body ());
    }

    void visit (ast_expression_statement_t& node)
    {
      CheckAndImplicitlyDereference (node.child_ref ());
    }

    void visit (ast_if_statement_t& node)
    {
      check_condition (node.condition_ref ());
      type_check_statement (node.true_branch ());
      type_check_statement (node.false_branch ());
    }

    void visit (ast_while_statement_t& node)
    {
      check_condition (node.condition_ref ());
      type_check_statement (node.body ());
    }

    void visit (ast_add_assign_statement_t& node)
    {
      arithmetic_assign (&node, "+=");
    }

    void visit (ast_subtract_assign_statement_t& node)
    {
      arithmetic_assign (&node, "-=");
    }

    void visit (ast_list_statement_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          type_check_statement (*pos);
        }
    }

    void visit (ast_return_statement_t& node)
    {
      // Get the return symbol.
      node.return_symbol = node.GetReturnSymbol ();
      assert (node.return_symbol != NULL);

      // Check the expression.
      typed_value_t expr_tv = CheckAndImplicitlyDereferenceAndConvert (node.child_ref (), node.return_symbol->value.type);

      // Check that it matches with the return type.
      check_assignment (node.return_symbol->value, expr_tv, node,
                        "cannot convert to (%s) from (%s) in return (E124)",
                        "return leaks mutable pointers (E125)");
    }

    void visit (ast_increment_statement_t& node)
    {
      ast_t* expr = node.child ();
      check_assignment_target (expr);
      struct visitor : public Visitor
      {
        ast_t& node;

        visitor (ast_t& n) : node (n) { }

        void default_action (const Type::Type& type)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "cannot increment location of type %s (E50)", type.ToString ().c_str ());
        }

        void visit (const NamedType& type)
        {
          type.UnderlyingType ()->Accept (*this);
        }

        void visit (const Int& type)
        {
          // Okay.
        }

        void visit (const Uint& type)
        {
          // Okay.
        }
      };
      visitor v (node);
      expr->typed_value.type->Accept (v);
    }

    void visit (ast_decrement_statement_t& node)
    {
      unimplemented;
    }

    void visit (ast_activate_statement_t& node)
    {
      action_reaction_base_t *action = node.GetAction ();
      ast_t *expression_list_node = node.expr_list ();
      ast_t *body_node = node.body ();

      Activation *activation = new Activation (node);
      action->add_activation (activation);

      /* Check the activations. */
      TypedValueListType tvlist;
      checkArgs (expression_list_node, tvlist);

      /* Re-insert this as a pointer to mutable. */
      node.this_symbol = node.Activate ();

      /* Check the body. */
      type_check_statement (body_node);

      node.activation = activation;
    }

    void visit (ast_var_statement_t& node)
    {
      ast_t* identifier_list = node.identifier_list ();
      ast_t* type_spec = node.type_spec ();
      ast_t* expression_list = node.expression_list ();

      if (expression_list->size () != 0 &&
          identifier_list->size () != expression_list->size ())
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "wrong number of initializers (E51)");
        }

      // Process the type spec.
      const Type::Type* type = process_type_spec (type_spec, true);

      if (expression_list->size () == 0)
        {
          // Type, no expressions.

          if (type_cast<Void> (type) != NULL)
            {
              error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                             "missing type (E52)");

            }

          // Enter each symbol.
          typed_value_t left_tv = typed_value_t::make_ref (type, node.mutability, node.dereferenceMutability);
          for (ast_t::iterator id_pos = identifier_list->begin (),
               id_limit = identifier_list->end ();
               id_pos != id_limit;
               ++id_pos)
            {
              const std::string& name = ast_get_identifier (*id_pos);
              Symbol* symbol = new VariableSymbol (name, *id_pos, left_tv);
              node.symbols.push_back (enter_symbol (*node.parent (), symbol));
            }

          return;
        }

      if (type_cast<Void> (type) == NULL)
        {
          // Type, expressions.

          // Enter each symbol.
          for (ast_t::iterator id_pos = identifier_list->begin (),
               id_limit = identifier_list->end (),
               init_pos = expression_list->begin ();
               id_pos != id_limit;
               ++id_pos, ++init_pos)
            {
              // Assume left is mutable.
              typed_value_t left_tv = typed_value_t::make_ref (type, MUTABLE, node.dereferenceMutability);
              typed_value_t right_tv = CheckAndImplicitlyDereferenceAndConvert (*init_pos, left_tv.type);
              check_assignment (left_tv, right_tv, node,
                                "incompatible types (%s) = (%s) (E126)",
                                "assignment leaks mutable pointers (E127)");
              // Convert to specified mutability.
              left_tv.intrinsic_mutability = node.mutability;
              const std::string& name = ast_get_identifier (*id_pos);
              Symbol* symbol = new VariableSymbol (name, *id_pos, left_tv);
              node.symbols.push_back (enter_symbol (*node.parent (), symbol));
            }

          return;
        }

      // No type, expressions.

      // Enter each symbol.
      for (ast_t::iterator id_pos = identifier_list->begin (),
           id_limit = identifier_list->end (),
           init_pos = expression_list->begin ();
           id_pos != id_limit;
           ++id_pos, ++init_pos)
        {
          // Process the initializer.
          typed_value_t right_tv = CheckAndImplicitlyDereferenceAndConvertToDefault (*init_pos);
          typed_value_t left_tv = typed_value_t::make_ref (right_tv);
          left_tv.intrinsic_mutability = MUTABLE;
          left_tv.dereference_mutability = node.dereferenceMutability;
          check_assignment (left_tv, right_tv, node,
                            "incompatible types (%s) = (%s) (E128)",
                            "assignment leaks mutable pointers (E129)");
          // Convert to specified mutability.
          left_tv.intrinsic_mutability = node.mutability;
          const std::string& name = ast_get_identifier (*id_pos);
          Symbol* symbol = new VariableSymbol (name, *id_pos, left_tv);
          node.symbols.push_back (enter_symbol (*node.parent (), symbol));
        }
    }
  };

  visitor v;
  node->accept (v);
}

static void
control_check_statement (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {
    bool in_activation_statement;

    visitor () : in_activation_statement (false) { }

    void visit (ast_change_statement_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (ast_if_statement_t& node)
    {
      node.true_branch ()->accept (*this);
      node.false_branch ()->accept (*this);
    }

    void visit (ast_while_statement_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          (*pos)->accept (*this);
        }
    }

    void visit (ast_return_statement_t& node)
    {
      // TODO: Maybe.
    }

    void visit (ast_activate_statement_t& node)
    {
      ast_t *body_node = node.body ();
      action_reaction_base_t *action = node.GetAction ();

      if (action == NULL)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "activation outside of action or reaction (E53)");
        }

      if (in_activation_statement)
        {
          error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                         "activations within activations are not allowed (E54)");
        }

      in_activation_statement = true;
      body_node->accept (*this);
      in_activation_statement = false;
    }
  };

  visitor v;
  node->accept (v);
}

/*
  Determine if each activation mutates the state of a component.
  This requires pointer analysis and could be improved.
  However, pointer analysis solves the halting problem so we must draw the line somewhere.

  The state of a component is only mutable in the body of a activation statement so we can exclude all other activity.
  The cases to check:
  1.  Assignment to an lvalue derived from the receiver.
  2.  Copying a pointer (to mutable) whose value is derived from the receiver.
      a.  In assignment
      a.  In a call

 */
static void
mutates_check_statement (ast_t * node)
{
  struct derived_visitor : public ast_visitor_t
  {
    bool derived_from_receiver;

    derived_visitor () : derived_from_receiver (false) { }

    void default_action (ast_t& node)
    {
      not_reached;
    }

    void visit (ast_implicit_dereference_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_address_of_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_index_expr_t& node)
    {
      node.base ()->accept (*this);
    }

    void visit (ast_select_expr_t& node)
    {
      node.base ()->accept (*this);
    }

    void visit (ast_dereference_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_identifier_expr_t& node)
    {
      ParameterSymbol* symbol = SymbolCast<ParameterSymbol> (node.symbol);
      if (symbol != NULL)
        {
          assert (symbol->kind != ParameterSymbol::Receiver);
          derived_from_receiver = symbol->kind == ParameterSymbol::ReceiverDuplicate;
        }
    }
  };

  struct check_visitor : public ast_visitor_t
  {
    bool mutates_receiver;

    check_visitor () : mutates_receiver (false) { }

    void default_action (ast_t& node)
    {
      ast_not_reached (node);
    }

    void check_for_pointer_copy (ast_t* node)
    {
      typed_value_t tv = node->typed_value;
      if (type_cast<Pointer> (tv.type))
        {
          derived_visitor v;
          node->accept (v);
          mutates_receiver = mutates_receiver || v.derived_from_receiver;
        }
    }

    void visit (ast_var_statement_t& node)
    {
      node.expression_list ()->accept (*this);
    }

    void visit (ast_slice_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_call_expr_t& node)
    {
      node.expr ()->accept (*this);

      if (type_cast<Type::Method> (node.expr ()->typed_value.type) != NULL)
        {
          // Invoking a method.
          check_for_pointer_copy (node.expr ()->at (0)->at (0));
        }

      for (ast_t::iterator pos = node.args ()->begin (), limit = node.args ()->end ();
           pos != limit;
           ++pos)
        {
          check_for_pointer_copy (*pos);
          (*pos)->accept (*this);
        }
    }

    void visit (ast_literal_expr_t& node)
    { }

    void visit (ast_identifier_expr_t& node)
    { }

    void visit (ast_implicit_dereference_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_address_of_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_dereference_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_index_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_list_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_select_expr_t& node)
    {
      node.base ()->accept (*this);
    }

    void visit (ast_unary_arithmetic_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_binary_arithmetic_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_implicit_conversion_expr_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_assign_statement_t& node)
    {
      {
        derived_visitor v;
        node.left ()->accept (v);
        mutates_receiver = mutates_receiver || v.derived_from_receiver;
      }

      check_for_pointer_copy (node.right ());

      node.left ()->accept (*this);
      node.right ()->accept (*this);
    }

    void visit (ast_add_assign_statement_t& node)
    {
      derived_visitor v;
      node.left ()->accept (v);
      mutates_receiver = mutates_receiver || v.derived_from_receiver;

      node.left ()->accept (*this);
      node.right ()->accept (*this);
    }

    void visit (ast_increment_statement_t& node)
    {
      derived_visitor v;
      node.child ()->accept (v);
      mutates_receiver = mutates_receiver || v.derived_from_receiver;

      node.child ()->accept (*this);
    }

    void visit (ast_change_statement_t& node)
    {
      node.expr ()->accept (*this);
      node.body ()->accept (*this);
    }

    void visit (ast_expression_statement_t& node)
    {
      node.child ()->accept (*this);
    }

    void visit (ast_if_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_while_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
           pos != limit;
           ++pos)
        {
          (*pos)->accept (*this);
          if (mutates_receiver)
            {
              break;
            }
        }
    }
  };

  struct visitor : public ast_visitor_t
  {
    void visit (ast_change_statement_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (ast_if_statement_t& node)
    {
      node.true_branch ()->accept (*this);
      node.false_branch ()->accept (*this);
    }

    void visit (ast_while_statement_t& node)
    {
      node.body ()->accept (*this);
    }

    void visit (ast_list_statement_t& node)
    {
      node.visit_children (*this);
    }

    void visit (ast_activate_statement_t& node)
    {
      check_visitor v;
      node.body ()->accept (v);
      if (v.mutates_receiver)
        {
          node.activation->mode = ACTIVATION_WRITE;
        }
    }
  };

  visitor v;
  node->accept (v);
}

// TODO: Replace node with its symbol table.
void
enter_signature (ast_t& node, const Signature * type)
{
  for (Signature::ParametersType::const_iterator pos = type->Begin (), limit = type->End ();
       pos != limit; ++pos)
    {
      const parameter_t* parameter = *pos;
      // Check if the symbol is defined locally.
      const std::string& identifier = parameter->name;
      Symbol *s = node.FindSymbolCurrent (identifier);
      if (s == NULL)
        {
          if (parameter->is_receiver)
            {
              s = ParameterSymbol::makeReceiver (parameter);
            }
          else
            {
              s = ParameterSymbol::make (parameter);
            }
          node.EnterSymbol (s);
        }
      else
        {
          error_at_line (-1, 0, parameter->defining_node->location.File.c_str (), parameter->defining_node->location.Line,
                         "%s is already defined in this scope (E55)",
                         identifier.c_str ());
        }
    }
}

/* Check the semantics of all executable code. */
void
process_definitions (ast_t * node)
{
  struct visitor : public ast_visitor_t
  {
    void default_action (ast_t& node)
    {
      ast_not_reached (node);
    }

    void visit (ast_type_definition_t& node)
    { }

    void visit (ast_const_t& node)
    { }

    void visit (ast_action_t& node)
    {
      typed_value_t tv = check_condition (node.precondition_ref ());
      node.action->precondition = node.precondition ();
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);

      if (tv.value.present)
        {
          if (tv.value.ref (*Bool::Instance ()))
            {
              node.action->precondition_kind = action_t::STATIC_TRUE;
            }
          else
            {
              node.action->precondition_kind = action_t::STATIC_FALSE;
            }
        }
    }

    void visit (ast_dimensioned_action_t& node)
    {
      check_condition (node.precondition_ref ());
      node.action->precondition = node.precondition ();
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);
    }

    void visit (ast_bind_t& node)
    {
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_function_t& node)
    {
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_method_t& node)
    {
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_initializer_t& node)
    {
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_getter_t& node)
    {
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
    }

    void visit (ast_instance_t& node)
    {
      // Lookup the initialization function.
      InstanceSymbol* symbol = node.symbol;
      const NamedType* type = symbol->type;
      ast_t* initializer_node = node.initializer ();
      Initializer* initializer = type->GetInitializer (ast_get_identifier (initializer_node));
      if (initializer == NULL)
        {
          error_at_line (-1, 0, initializer_node->location.File.c_str (),
                         initializer_node->location.Line,
                         "no initializer named %s (E56)",
                         ast_get_identifier (initializer_node).c_str ());
        }

      // Check the call.
      TypedValueListType tvlist;
      checkArgs (node.expression_list (), tvlist);
      checkCall (node, initializer->initializerType->signature, initializer->initializerType->return_parameter->value, node.expression_list (), tvlist);
      symbol->initializer = initializer;
    }

    void visit (ast_reaction_t& node)
    {
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);
    }

    void visit (ast_dimensioned_reaction_t& node)
    {
      ast_t *body_node = node.body ();
      type_check_statement (body_node);
      control_check_statement (body_node);
      mutates_check_statement (body_node);
    }

    void visit (ast_top_level_list_t& node)
    {
      node.visit_children (*this);
    }
  };

  visitor v;
  node->accept (v);
}
