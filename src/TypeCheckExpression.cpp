#include "semantic.hpp"
#include "AstVisitor.hpp"
using namespace ast;
#include "Type.hpp"
using namespace Type;
#include "runtime.hpp"
using namespace runtime;

typed_value_t
ImplicitlyConvert (ast::Node*& expr, const Type::Type* target)
{
  typed_value_t tv = expr->typed_value;
  if (!Type::Identitical (target, tv.type) && tv.AssignableTo (target))
    {
      tv = tv.Convert (expr->location, target, *expr);
      expr = new ast_implicit_conversion_expr_t (expr->location.Line, expr);
      expr->typed_value = tv;
    }
  return tv;
}

typed_value_t
ImplicitlyConvertToDefault (ast::Node*& expr)
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
      tv = tv.Convert (expr->location, target, *expr);
      expr = new ast_implicit_conversion_expr_t (expr->location.Line, expr);
      expr->typed_value = tv;
    }
  return tv;
}

static typed_value_t
insertImplicitDereference (ast::Node*& expr)
{
  typed_value_t tv = expr->typed_value;
  expr = new ast_implicit_dereference_expr_t (expr->location.Line, expr);
  tv = typed_value_t::implicit_dereference (tv);
  expr->typed_value = tv;
  return tv;
}

typed_value_t
CheckAndImplicitlyDereference (ast::Node*& expr)
{
  typed_value_t tv = TypeCheckExpression (expr);
  if (tv.IsReference ())
    {
      // Insert a dereference node.
      tv = insertImplicitDereference (expr);
    }
  return tv;
}

typed_value_t
CheckAndImplicitlyDereferenceAndConvert (ast::Node*& expr, const Type::Type* type)
{
  CheckAndImplicitlyDereference (expr);
  ImplicitlyConvert (expr, type);
  return expr->typed_value;
}

typed_value_t
CheckAndImplicitlyDereferenceAndConvertToDefault (ast::Node*& expr)
{
  CheckAndImplicitlyDereference (expr);
  ImplicitlyConvertToDefault (expr);
  return expr->typed_value;
}


static typed_value_t
insertExplicitDereference (ast::Node*& expr, typed_value_t tv)
{
  expr = new ast_dereference_expr_t (expr->location.Line, expr);
  tv = typed_value_t::dereference (tv);
  expr->typed_value = tv;
  return tv;
}

void
TypeCheckArgs (Node * node, TypedValueListType& tvlist)
{
  for (Node::Iterator child = node->Begin (), limit = node->End ();
       child != limit;
       ++child)
    {
      tvlist.push_back (CheckAndImplicitlyDereference (*child));
    }
}

void
TypeCheckCall (Node& node,
               const Type::Signature* signature,
               typed_value_t return_value,
               ast::Node* argsnode,
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

  bool component_state = false;
  size_t idx = 0;
  for (Type::Signature::const_iterator pos = signature->Begin (),
       limit = signature->End ();
       pos != limit;
       ++pos, ++idx)
    {
      typed_value_t parameter_tv = typed_value_t::make_ref ((*pos)->value);
      typed_value_t argument_tv  = ImplicitlyConvert (argsnode->At (idx), parameter_tv.type);
      if (argument_tv.component_state && type_contains_pointer (parameter_tv.type) && parameter_tv.dereference_mutability == MUTABLE)
        {
          component_state = true;
        }
      check_assignment (parameter_tv, argument_tv, *argsnode->At (idx),
                        "incompatible types (%s) = (%s) (E116)",
                        "argument leaks mutable pointers (E117)");
    }

  // Set the return type.
  node.typed_value = return_value;
  node.typed_value.component_state = component_state && type_contains_pointer (return_value.type);
}

struct check_visitor : public ast::DefaultVisitor
{
  ast::Node* ptr;

  check_visitor (ast::Node* p) : ptr (p) { }

  void default_action (Node& node)
  {
    ast_not_reached(node);
  }

  void visit (ast_list_expr_t& node)
  {
    for (Node::Iterator pos = node.Begin (), limit = node.End ();
         pos != limit;
         ++pos)
      {
        TypeCheckExpression (*pos);
      }
  }

  void visit (TypeExpression& node)
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

    if (!(index_tv.type->IsInteger () || index_tv.type->IsUntyped ()))
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "push array index must be an integer (E144)");

      }
    if (index_tv.value.present)
      {
        if (!index_tv.RepresentableBy (Type::Int::Instance ()))
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "push array index cannot be represented by an integer (E145)");
          }
        typed_value_t index_int_tv = index_tv.Convert (node.location, Type::Int::Instance (), node);
        Type::Int::ValueType v = index_int_tv.value.ref (*Type::Int::Instance ());
        if (v < 0)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "push array index is negative (E146)");
          }
        if (v >= array_type->dimension)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "push array index is out of range (E147)");
          }
        node.index_op = new runtime::Int (v);
      }
    else
      {
        node.index_op = MakeConvertToInt (new EvaluateNode (node.index ()), index_tv.type);
      }

    Node *args = node.args ();
    TypedValueListType tvlist;
    TypeCheckArgs (args, tvlist);
    TypeCheckCall (node, push_port_type->GetSignature (), push_port_type->GetReturnParameter ()->value, args, tvlist);
    node.field = type_select_field (this_type, port_identifier);
    node.array_type = array_type;
  }

  void visit (ast_identifier_expr_t& node)
  {
    Node *identifier_node = node.child ();
    const std::string& identifier = ast_get_identifier (identifier_node);
    Symbol *symbol = node.FindGlobalSymbol (identifier);
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
    typed_value_t in = TypeCheckExpression (node.base ());
    assert (in.type != NULL);

    if (in.IsReference () && type_dereference (in.type))
      {
        // Pointer reference.
        // Insert an implicit dereference.
        in = insertImplicitDereference (node.base_ref ());
      }

    if (in.IsValue () && type_dereference (in.type))
      {
        // Pointer value.
        // Insert an explicit dereference.
        in = insertExplicitDereference (node.base_ref (), in);
      }

    if (in.IsReference ())
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
    ast::Node* expr = node.child ();
    typed_value_t in = expr->typed_value;
    typed_value_t out = typed_value_t::address_of (in);
    if (out.IsError ())
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "incompatible types: %s (E22)", in.type->ToString ().c_str ());
      }
    node.typed_value = out;
    node.address_of_dereference = ast_cast<ast_dereference_expr_t> (expr) != NULL;
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
    TypeCheckArgs (node.args (), tvlist);

    // Analyze the callee.
    // Expecting a value.
    typed_value_t expr_tv = CheckAndImplicitlyDereference (node.expr_ref ());

    node.original_expr_tv = expr_tv;

    if (expr_tv.kind == typed_value_t::TYPE)
      {
        // Conversion.
        if (tvlist.size () != 1)
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "conversion requires exactly one argument (E69)");
          }

        node.typed_value = tvlist[0].Convert (node.location, expr_tv.type, node);
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

    struct visitor : public Type::DefaultVisitor
    {
      check_visitor& rvalue_visitor;
      ast_call_expr_t& node;
      TypedValueListType& tvlist;

      visitor (check_visitor& rv,
               ast_call_expr_t& n,
               TypedValueListType& tvl)
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
        Node::Context context = node.GetContext ();

        switch (type.kind)
          {
          case Type::Function::FUNCTION:
            // No restrictions on caller.
            TypeCheckCall (node, type.GetSignature (), type.GetReturnParameter ()->value, node.args (), tvlist);
            break;

          case Type::Function::PUSH_PORT:
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "push ports cannot be called (E28)");

            break;

          case Type::Function::PULL_PORT:
            // Must be called from either a getter, an action, or reaction.
            if (!(context == Node::Getter ||
                  context == Node::Action ||
                  context == Node::Reaction))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "pull ports may only be called from a getter, an action, or a reaction (E29)");
              }

            TypeCheckCall (node, type.GetSignature (), type.GetReturnParameter ()->value, node.args (), tvlist);
            if (node.GetInMutableSection ())
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "cannot call pull port in mutable section (E30)");
              }
            break;
          }
      }

      void visit (const Type::Method& type)
      {
        Node::Context context = node.GetContext ();

        // Convert to a function call.
        // Move the receiver to the args.
        ast::Node* receiver = node.expr ()->At (0)->At (0);
        if (type_dereference (type.receiver_type) != NULL)
          {
            // Method expects a pointer.  Insert address of.
            ast_address_of_expr_t* e = new ast_address_of_expr_t (node.location.Line, receiver);
            rvalue_visitor.check_address_of (*e);
            receiver = e;
          }
        else
          {
            insertImplicitDereference (receiver);
          }
        node.args ()->Prepend (receiver);
        tvlist.insert (tvlist.begin (), receiver->typed_value);
        // Reset the expression to a literal.
        typed_value_t method_tv = node.expr ()->typed_value;
        method_tv.type = type.function_type;
        node.expr_ref () = new ast_literal_expr_t (node.location.Line, method_tv);

        TypeCheckCall (node, type.function_type->GetSignature (), type.return_parameter->value, node.args (), tvlist);

        switch (type.kind)
          {
          case Type::Method::METHOD:
            // No restrictions on caller.
            break;
          case Type::Method::INITIALIZER:
            // Caller must be an initializer.
            if (context != Node::Initializer)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "initializers may only be called from initializers (E31)");
              }
            break;
          case Type::Method::GETTER:
          {
            // Must be called from either a getter, action, reaction, or initializer.
            if (!(context == Node::Getter ||
                  context == Node::Action ||
                  context == Node::Reaction ||
                  context == Node::Initializer))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "getters may only be called from a getter, an action, a reaction, or an initializer (E32)");
              }

            if (node.GetInMutableSection ())
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "cannot call getter in mutable section (E33)");
              }
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
    Node *expr = node.identifier ();
    Node *args = node.args ();
    const std::string& port_identifier = ast_get_identifier (expr);
    const Type::Type *this_type = node.GetReceiverType ();
    const Type::Function *push_port_type = Type::type_cast<Type::Function> (type_select (this_type, port_identifier));
    if (push_port_type == NULL || push_port_type->kind != Type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E34)", port_identifier.c_str ());
      }
    TypedValueListType tvlist;
    TypeCheckArgs (args, tvlist);
    TypeCheckCall (node, push_port_type->GetSignature (), push_port_type->GetReturnParameter ()->value, args, tvlist);
    node.field = type_select_field (this_type, port_identifier);
  }

  void visit (ast_index_expr_t& node)
  {
    typed_value_t base_tv = TypeCheckExpression (node.base ());
    typed_value_t index_tv = CheckAndImplicitlyDereference (node.index_ref ());
    base_tv.RequireReferenceOrValue (node.base ()->location);
    index_tv.RequireReferenceOrValue (node.index ()->location);

    struct visitor : public Type::DefaultVisitor
    {
      ast_index_expr_t& node;
      visitor (ast_index_expr_t& n) : node (n) { }

      void default_action (const Type::Type& type)
      {
        type_not_reached (type);
      }

      void visit (const Array& type)
      {
        Operation* index_op;

        typed_value_t base_tv = node.base ()->typed_value;
        typed_value_t index_tv = node.index ()->typed_value;
        if (!(index_tv.type->IsInteger () || index_tv.type->IsUntyped ()))
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "array index must be an integer (E101)");

          }
        if (index_tv.value.present)
          {
            if (!index_tv.RepresentableBy (Type::Int::Instance ()))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "array index cannot be represented by an integer (E121)");
              }
            typed_value_t index_int_tv = index_tv.Convert (node.location, Type::Int::Instance (), node);
            Type::Int::ValueType v = index_int_tv.value.ref (*Type::Int::Instance ());
            if (v < 0)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "array index is negative (E141)");
              }
            if (v >= type.dimension)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "array index is out of range (E142)");
              }

            if (base_tv.value.present)
              {
                // Get the value from the array.
                unimplemented;
              }
            index_op = new runtime::Int (v);
          }
        else
          {
            index_op = MakeConvertToInt (new EvaluateNode (node.index ()), index_tv.type);
          }

        Operation* base_op = new EvaluateNode (node.base ());

        if (base_tv.IsReference ())
          {
            node.typed_value =
              typed_value_t::make_ref (type.Base (),
                                       base_tv.intrinsic_mutability,
                                       base_tv.dereference_mutability,
                                       base_tv.component_state);
            node.operation = new IndexArrayReference (node.location, base_op, index_op, type);
          }
        else if (base_tv.IsValue ())
          {
            node.typed_value =
              typed_value_t::make_value (type.Base (),
                                         base_tv.intrinsic_mutability,
                                         base_tv.dereference_mutability,
                                         base_tv.component_state);
            node.operation = new IndexArrayValue (node.location, base_op, index_op, type);
          }
        else
          {
            not_reached;
          }
      }

      void visit (const Slice& type)
      {
        Operation* index_op;

        typed_value_t base_tv = node.base ()->typed_value;
        typed_value_t index_tv = node.index ()->typed_value;

        if (!(index_tv.type->IsInteger () || index_tv.type->IsUntyped ()))
          {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "slice index must be an integer (E97)");

          }
        if (index_tv.value.present)
          {
            if (!index_tv.RepresentableBy (Type::Int::Instance ()))
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "slice index cannot be represented by an integer (E98)");
              }
            typed_value_t index_int_tv = index_tv.Convert (node.location, Type::Int::Instance (), node);
            Type::Int::ValueType v = index_int_tv.value.ref (*Type::Int::Instance ());
            if (v < 0)
              {
                error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                               "slice index is negative (E99)");
              }
            if (base_tv.value.present)
              {
                // Get the value from the slice.
                unimplemented;
              }
            index_op = new runtime::Int (v);
          }
        else
          {
            index_op = MakeConvertToInt (new EvaluateNode (node.index ()), index_tv.type);
          }

        node.typed_value =
          typed_value_t::make_ref (type.Base (),
                                   base_tv.intrinsic_mutability,
                                   base_tv.dereference_mutability,
                                   base_tv.component_state);

        Operation* base_op = new EvaluateNode (node.base ());
        if (base_tv.kind == typed_value_t::REFERENCE)
          {
            base_op = new LoadSlice (base_op);
          }
        node.operation = new IndexSlice (node.location, base_op, index_op, type);
      }
    } v (node);
    base_tv.type->Accept (v);
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
TypeCheckExpression (ast::Node* ptr)
{
  check_visitor check_lvalue_visitor (ptr);
  ptr->Accept (check_lvalue_visitor);
  return ptr->typed_value;
}
