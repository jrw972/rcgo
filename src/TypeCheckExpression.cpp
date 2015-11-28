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
  unimplemented;
  // typed_value_t tv = expr->typed_value;
  // if (!Type::Identical (target, tv.type) && tv.AssignableTo (target))
  //   {
  //     tv = tv.Convert (expr->location, target, *expr);
  //     expr = new ast_implicit_conversion_expr_t (expr->location.Line, expr);
  //     expr->typed_value = tv;
  //   }
  // return tv;
}

typed_value_t
ImplicitlyConvertToDefault (ast::Node*& expr)
{
  unimplemented;
  // typed_value_t tv = expr->typed_value;
  // const Type::Type* target = tv.type->DefaultType ();
  // if (!Type::Identical (target, tv.type))
  //   {
  //     if (!tv.AssignableTo (target))
  //       {
  //         error_at_line (-1, 0, expr->location.File.c_str (), expr->location.Line,
  //                        "cannot convert to real type (E68)");
  //       }
  //     tv = tv.Convert (expr->location, target, *expr);
  //     expr = new ast_implicit_conversion_expr_t (expr->location.Line, expr);
  //     expr->typed_value = tv;
  //   }
  // return tv;
}

static typed_value_t
insertImplicitDereference (ast::Node*& expr)
{
  unimplemented;
  // typed_value_t tv = expr->typed_value;
  // expr = new ast_implicit_dereference_expr_t (expr->location.Line, expr);
  // tv = typed_value_t::implicit_dereference (tv);
  // expr->typed_value = tv;
  // return tv;
}

typed_value_t
CheckAndImplicitlyDereferenceAndConvert (ast::Node*& expr, const Type::Type* type)
{
  unimplemented;
  // CheckAndImplicitlyDereference (expr);
  // ImplicitlyConvert (expr, type);
  // return expr->typed_value;
}

void
TypeCheckArgs (Node * node, TypedValueListType& tvlist)
{
  for (Node::Iterator child = node->Begin (), limit = node->End ();
       child != limit;
       ++child)
    {
      unimplemented;
      //tvlist.push_back (CheckAndImplicitlyDereference (*child));
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
      unimplemented;
      // typed_value_t parameter_tv = typed_value_t::make_ref ((*pos)->value);
      // typed_value_t argument_tv  = ImplicitlyConvert (argsnode->At (idx), parameter_tv.type);
      // if (argument_tv.component_state && type_contains_pointer (parameter_tv.type) && parameter_tv.dereference_mutability == MUTABLE)
      //   {
      //     component_state = true;
      //   }
      // check_assignment (parameter_tv, argument_tv, *argsnode->At (idx),
      //                   "incompatible types (%s) = (%s) (E116)",
      //                   "argument leaks mutable pointers (E117)");
    }

  // Set the return type.
  unimplemented;
  // node.typed_value = return_value;
  // node.typed_value.component_state = component_state && type_contains_pointer (return_value.type);
}

static Operation* check_array_index (Node* node,
                                     const Array* array_type)
{
  unimplemented;
  // typed_value_t index_tv = TypeCheckExpression (node);
  // index_tv.RequireReferenceOrValue (node->location);

  // if (!(index_tv.type->IsInteger () || index_tv.type->IsUntyped ()))
  //   {
  //     error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
  //                    "array index must be an integer (E144)");

  //   }
  // if (index_tv.value.present)
  //   {
  //     if (!index_tv.RepresentableBy (Type::Int::Instance ()))
  //       {
  //         error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
  //                        "array index cannot be represented by an int (E145)");
  //       }
  //     typed_value_t index_int_tv = index_tv.Convert (node->location, Type::Int::Instance (), *node);
  //     Type::Int::ValueType v = index_int_tv.value.ref (*Type::Int::Instance ());
  //     if (v < 0)
  //       {
  //         error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
  //                        "array index is negative (E146)");
  //       }
  //     if (v >= array_type->dimension)
  //       {
  //         error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
  //                        "array index is out of range (E147)");
  //       }
  //     return runtime::make_literal (v);
  //   }
  // else
  //   {
  //     Operation* o = new EvaluateNode (node);
  //     if (index_tv.IsReference ()) {
  //       o = new Load (o, index_tv.type);
  //     }
  //     return MakeConvertToInt (o, index_tv.type);
  //   }
}

struct Predicate
{
  bool flag;
  Predicate () : flag (false) { }

  template <typename T>
  void operator() (const T& t)
  {
    flag = true;
  }
  void NotArithmetic (const Type::Type& t) { }
  void NotIntegral (const Type::Type& t) { }
};

template <template <typename S> class Visitor>
bool type_predicate (const Type::Type* type)
{
  Predicate p;
  Visitor<Predicate> visitor (p);
  type->Accept (visitor);
  return p.flag;
}

struct MultiplyValues
{
  template <typename T>
  T operator() (const Location& loc, T x, T y) const
  {
    return x * y;
  }
};

struct DivideValues
{
  template <typename T>
  T operator() (const Location& loc, T x, T y) const
  {
    if (y == 0)
      {
        error_at_line (-1, 0, loc.File.c_str (), loc.Line,
                       "division by zero (E147)");
      }
    return x / y;
  }
};

struct ModulusValues
{
  template <typename T>
  T operator() (const Location& loc, T x, T y) const
  {
    if (y == 0)
      {
        error_at_line (-1, 0, loc.File.c_str (), loc.Line,
                       "division by zero (E147)");
      }
    return x % y;
  }
};

template <typename F>
struct ApplyTypedValueBinary
{
  const Location& location;
  const typed_value_t& left;
  const typed_value_t& right;
  typed_value_t& out;

  ApplyTypedValueBinary (const Location& loc, const typed_value_t& l, const typed_value_t& r, typed_value_t& o) : location (loc), left (l), right (r), out (o) { }

  template <typename T>
  void operator() (const T& t) const
  {
    out.value.ref (t) = F () (location, left.value.ref (t), right.value.ref (t));
  }

  void NotArithmetic (const Type::Type& t)
  {
    type_not_reached (t);
  }

  void NotIntegral (const Type::Type& t)
  {
    type_not_reached (t);
  }
};

template<template <typename S> class Visitor, typename T>
static typed_value_t apply_typed_value_binary (const Location& loc, const typed_value_t& left, const typed_value_t& right)
{
  typed_value_t retval = left;
  retval.value.present = left.value.present && right.value.present;
  if (retval.value.present)
    {
      ApplyTypedValueBinary<T> op (loc, left, right, retval);
      Visitor<ApplyTypedValueBinary<T> > visitor (op);
      left.type->Accept (visitor);
    }
  return retval;
}

template <template <typename S> class Visitor, typename Op>
static void binary_arithmetic (ast_binary_arithmetic_expr_t& node,
                               typed_value_t& left,
                               typed_value_t& right,
                               const char* s)
{
  if (!(type_predicate<Visitor> (left.type) && type_predicate<Visitor> (right.type)))
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "%s cannot be applied to %s and %s (E45)", s, left.type->ToString ().c_str (), right.type->ToString ().c_str ());
    }
  if (!(left.AssignableTo (right.type) || right.AssignableTo (left.type)))
    {
      error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                     "%s cannot be applied to %s and %s (E45)", s, left.type->ToString ().c_str (), right.type->ToString ().c_str ());
    }
  // Determine the resulting type.
  if (left.type->Level () > right.type->Level ())
    {
      right = right.Convert (node.location, left.type, node);
    }
  else if (left.type->Level () < right.type->Level ())
    {
      left = left.Convert (node.location, right.type, node);
    }

  // unimplemented;n
  // node.typed_value = apply_typed_value_binary<Visitor, Op> (node.location, left, right);

  // if (node.typed_value.value.present && !node.typed_value.type->IsUntyped ()) {
  //   node.operation = make_literal (node.typed_value.type, node.typed_value.value);
  //   return;
  // }

  // node.operation = make_binary<Visitor, Op> (node.typed_value.type, node.location, new EvaluateNode (node.left ()), new EvaluateNode (node.right ()));
}

struct check_visitor : public ast::DefaultVisitor
{
  ast::Node* ptr;

  check_visitor (ast::Node* p) : ptr (p) { }

  void default_action (Node& node)
  {
    ast_not_reached(node);
  }

  void visit (TypeExpression& node)
  {
    unimplemented;
    // const Type::Type* type = process_type_spec (node.type_spec (), true);
    // node.typed_value = typed_value_t (type);
  }

  void visit (ast_indexed_port_call_expr_t& node)
  {
    const std::string& port_identifier = ast_get_identifier (node.identifier ());
    const Type::Type* this_type = node.GetReceiverType ();
    const Type::Type* type = type_select (this_type, port_identifier);

    if (type == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E15)", port_identifier.c_str ());
      }

    const Type::Array* array_type = Type::type_cast<Type::Array> (type);
    if (array_type == NULL)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "%s is not an array of ports (E16)", port_identifier.c_str ());
      }

    const Type::Function* push_port_type = Type::type_cast<Type::Function> (array_type->Base ());

    if (push_port_type == NULL || push_port_type->function_kind != Type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "%s is not an array of ports (E17)", port_identifier.c_str ());
      }

    node.index_op = check_array_index (node.index (), array_type);

    Node *args = node.args ();
    TypedValueListType tvlist;
    TypeCheckArgs (args, tvlist);
    unimplemented;
    // TypeCheckCall (node, push_port_type->GetSignature (), push_port_type->GetReturnParameter ()->value, args, tvlist);
    // node.field = type_select_field (this_type, port_identifier);
    // node.array_type = array_type;
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

    unimplemented;
    // struct visitor : public ConstSymbolVisitor
    // {
    //   ast_identifier_expr_t& node;

    //   visitor (ast_identifier_expr_t& n)
    //     : node (n)
    //   { }

    //   void defaultAction (const Symbol& symbol)
    //   {
    //     not_reached;
    //   }

    //   void visit (const BuiltinFunction& symbol)
    //   {
    //     node.typed_value = symbol.value ();
    //   }

    //   void visit (const ::Template& symbol)
    //   {
    //     node.typed_value = symbol.value ();
    //   }

    //   void visit (const ::Function& symbol)
    //   {
    //     node.typed_value = symbol.value ();
    //   }

    //   void visit (const ParameterSymbol& symbol)
    //   {
    //     unimplemented;
    //     //node.typed_value = symbol.value;
    //   }

    //   void visit (const TypeSymbol& symbol)
    //   {
    //     node.typed_value = typed_value_t (symbol.type);
    //   }

    //   void visit (const ConstantSymbol& symbol)
    //   {
    //     node.typed_value = symbol.value;
    //   }

    //   void visit (const VariableSymbol& symbol)
    //   {
    //     node.typed_value = symbol.value;
    //   }

    //   void visit (const HiddenSymbol& symbol)
    //   {
    //     error_at_line (-1, 0, node.location.File.c_str (),
    //                    node.location.Line, "%s is not accessible in this scope (E19)",
    //                    symbol.identifier.c_str ());
    //   }
    // };
    // visitor v (node);
    // symbol->accept (v);

    // node.symbol = symbol;
  }

  void visit (ast_literal_expr_t& node)
  {
    // Do nothing.
  }

  void check_address_of (ast_address_of_expr_t& node)
  {
    ast::Node* expr = node.child ();
    unimplemented;
    // typed_value_t in = expr->typed_value;
    // typed_value_t out = typed_value_t::address_of (in);
    // if (out.IsError ())
    //   {
    //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                    "incompatible types: %s (E22)", in.type->ToString ().c_str ());
    //   }
    // node.typed_value = out;
    // node.address_of_dereference = ast_cast<ast_dereference_expr_t> (expr) != NULL;
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
    unimplemented;
    //node.typed_value = out;
  }

  void visit (ast_unary_arithmetic_expr_t& node)
  {
    unimplemented;
    // typed_value_t in = CheckAndImplicitlyDereference (node.child_ref ());
    // switch (node.arithmetic)
    //   {
    //   case LogicNot:
    //     node.typed_value = in.LogicNot (node.location);
    //     return;
    //   case Negate:
    //     node.typed_value = in.Negate (node.location);
    //     return;
    //   }
    // not_reached;
  }

  void visit (ast_binary_arithmetic_expr_t& node)
  {
    unimplemented;
    // typed_value_t left = CheckAndImplicitlyDereference (node.left_ref ());
    // typed_value_t right = CheckAndImplicitlyDereference (node.right_ref ());
    // switch (node.arithmetic)
    //   {
    //   case Multiply:
    //     binary_arithmetic<Type::ArithmeticVisitor, MultiplyValues> (node, left, right, "*");
    //     return;
    //   case Divide:
    //     binary_arithmetic<Type::ArithmeticVisitor, DivideValues> (node, left, right, "/");
    //     return;
    //   case Modulus:
    //     binary_arithmetic<Type::IntegralVisitor, ModulusValues> (node, left, right, "%");
    //     return;
    //   case LeftShift:
    //     if (left.value.present && !right.value.present)
    //       {
    //         left = ImplicitlyConvertToDefault (node.left_ref ());
    //       }
    //     if (right.value.present && !left.value.present)
    //       {
    //         right = ImplicitlyConvertToDefault (node.right_ref ());
    //       }
    //     unimplemented;
    //     //node.typed_value = typed_value_t::LeftShift (node.location, left, right);
    //     return;
    //   case RightShift:
    //     if (left.value.present && !right.value.present)
    //       {
    //         left = ImplicitlyConvertToDefault (node.left_ref ());
    //       }
    //     if (right.value.present && !left.value.present)
    //       {
    //         right = ImplicitlyConvertToDefault (node.right_ref ());
    //       }
    //     unimplemented;
    //     //node.typed_value = typed_value_t::RightShift (node.location, left, right);
    //     return;
    //   case BitAnd:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::BitAnd (node.location, left, right);
    //     return;
    //   case BitAndNot:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::BitAndNot (node.location, left, right);
    //     return;
    //   case Add:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::Add (node.location, left, right);
    //     return;
    //   case Subtract:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::Subtract (node.location, left, right);
    //     return;
    //   case BitOr:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::BitOr (node.location, left, right);
    //     return;
    //   case BitXor:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::BitXor (node.location, left, right);
    //     return;
    //   case Equal:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::Equal (node.location, left, right);
    //     return;
    //   case NotEqual:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::NotEqual (node.location, left, right);
    //     return;
    //   case LessThan:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::LessThan (node.location, left, right);
    //     return;
    //   case LessEqual:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::LessEqual (node.location, left, right);
    //     return;
    //   case MoreThan:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::MoreThan (node.location, left, right);
    //     return;
    //   case MoreEqual:
    //     left = ImplicitlyConvert (node.left_ref (), right.type);
    //     right = ImplicitlyConvert (node.right_ref (), left.type);
    //     unimplemented;
    //     //node.typed_value = typed_value_t::MoreEqual (node.location, left, right);
    //     return;
    //   case ::LogicOr:
    //     {
    //       if (!(type_is_boolean (left.type) &&
    //             type_is_boolean (right.type))) {
    //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                        "|| cannot be applied to %s and %s (E45)", left.type->ToString ().c_str (), right.type->ToString ().c_str ());
    //       }
    //       if (!(left.AssignableTo (right.type) ||
    //             right.AssignableTo (left.type))) {
    //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                        "|| cannot be applied to %s and %s (E45)", left.type->ToString ().c_str (), right.type->ToString ().c_str ());
    //       }
    //       // Determine the resulting type.
    //       const Type::Type* type = Type::Choose (left.type, right.type);

    //       unimplemented;
    //       //node.typed_value = typed_value_t::make_value (type, IMMUTABLE, IMMUTABLE, false);

    //       if (left.value.present && left.value.ref (*Bool::Instance ())) {
    //         unimplemented;
    //         // node.typed_value.value.present = true;
    //         // node.typed_value.value.ref (*Bool::Instance ()) = true;
    //         // node.operation = make_literal (true);
    //         // return;
    //       }

    //       if (left.value.present && right.value.present) {
    //         unimplemented;
    //         // node.typed_value.value.present = true;
    //         // node.typed_value.value.ref (*Bool::Instance ()) = right.value.ref (*Bool::Instance ());
    //         // node.operation = make_literal (right.value.ref (*Bool::Instance ()));
    //         // return;
    //       }

    //       node.operation = new runtime::LogicOr (new EvaluateNode (node.left ()), new EvaluateNode (node.right ()));
    //       return;
    //     }
    //   case ::LogicAnd:
    //     {
    //       if (!(type_is_boolean (left.type) &&
    //             type_is_boolean (right.type))) {
    //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                        "&& cannot be applied to %s and %s (E45)", left.type->ToString ().c_str (), right.type->ToString ().c_str ());
    //       }
    //       if (!(left.AssignableTo (right.type) ||
    //             right.AssignableTo (left.type))) {
    //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                        "&& cannot be applied to %s and %s (E45)", left.type->ToString ().c_str (), right.type->ToString ().c_str ());
    //       }
    //       // Determine the resulting type.
    //       const Type::Type* type = Type::Choose (left.type, right.type);

    //       unimplemented;
    //       // node.typed_value = typed_value_t::make_value (type, IMMUTABLE, IMMUTABLE, false);

    //       // if (left.value.present && !left.value.ref (*Bool::Instance ())) {
    //       //   node.typed_value.value.present = true;
    //       //   node.typed_value.value.ref (*Bool::Instance ()) = false;
    //       //   node.operation = make_literal (false);
    //       //   return;
    //       // }

    //       // if (left.value.present && right.value.present) {
    //       //   node.typed_value.value.present = true;
    //       //   node.typed_value.value.ref (*Bool::Instance ()) = right.value.ref (*Bool::Instance ());
    //       //   node.operation = make_literal (right.value.ref (*Bool::Instance ()));
    //       //   return;
    //       // }

    //       // node.operation = new runtime::LogicAnd (new EvaluateNode (node.left ()), new EvaluateNode (node.right ()));
    //       return;
    //     }
    //   }
    // not_reached;
  }

  void visit (ast_call_expr_t& node)
  {
    unimplemented;
    // // Analyze the args.
    // TypedValueListType tvlist;
    // TypeCheckArgs (node.args (), tvlist);

    // // Analyze the callee.
    // // Expecting a value.
    // typed_value_t expr_tv = CheckAndImplicitlyDereference (node.expr_ref ());

    // node.original_expr_tv = expr_tv;

    // if (expr_tv.kind == typed_value_t::TYPE)
    //   {
    //     // Conversion.
    //     if (tvlist.size () != 1)
    //       {
    //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                        "conversion requires exactly one argument (E69)");
    //       }

    //     node.typed_value = tvlist[0].Convert (node.location, expr_tv.type, node);
    //     node.IsCall = false;
    //     return;
    //   }

    // node.IsCall = true;

    // const Type::Template* tt = Type::type_strip_cast <Type::Template> (expr_tv.type);
    // if (tt != NULL)
    //   {
    //     ::Template* t = expr_tv.value.template_value ();
    //     expr_tv = t->instantiate (tvlist);
    //     node.expr ()->typed_value = expr_tv;
    //   }

    // struct visitor : public Type::DefaultVisitor
    // {
    //   check_visitor& rvalue_visitor;
    //   ast_call_expr_t& node;
    //   TypedValueListType& tvlist;

    //   visitor (check_visitor& rv,
    //            ast_call_expr_t& n,
    //            TypedValueListType& tvl)
    //     : rvalue_visitor (rv)
    //     , node (n)
    //     , tvlist (tvl)
    //   { }

    //   void default_action (const Type::Type& type)
    //   {
    //     error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                    "cannot call %s (E27)", type.ToString ().c_str ());
    //   }

    //   void visit (const Type::Function& type)
    //   {
    //     Node::Context context = node.GetContext ();

    //     switch (type.kind)
    //       {
    //       case Type::Function::FUNCTION:
    //         // No restrictions on caller.
    //         unimplemented;
    //         //TypeCheckCall (node, type.GetSignature (), type.GetReturnParameter ()->value, node.args (), tvlist);
    //         break;

    //       case Type::Function::PUSH_PORT:
    //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                        "push ports cannot be called (E28)");

    //         break;

    //       case Type::Function::PULL_PORT:
    //         // Must be called from either a getter, an action, or reaction.
    //         if (!(context == Node::Getter ||
    //               context == Node::Action ||
    //               context == Node::Reaction))
    //           {
    //             error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                            "pull ports may only be called from a getter, an action, or a reaction (E29)");
    //           }

    //         unimplemented;
    //         //TypeCheckCall (node, type.GetSignature (), type.GetReturnParameter ()->value, node.args (), tvlist);
    //         if (node.GetInMutableSection ())
    //           {
    //             error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                            "cannot call pull port in mutable section (E30)");
    //           }
    //         break;
    //       }
    //   }

    //   void visit (const Type::Method& type)
    //   {
    //     Node::Context context = node.GetContext ();

    //     // Convert to a function call.
    //     // Move the receiver to the args.
    //     ast::Node* receiver = node.expr ()->At (0)->At (0);
    //     if (type_dereference (type.receiver_type) != NULL)
    //       {
    //         // Method expects a pointer.  Insert address of.
    //         ast_address_of_expr_t* e = new ast_address_of_expr_t (node.location.Line, receiver);
    //         rvalue_visitor.check_address_of (*e);
    //         receiver = e;
    //       }
    //     else
    //       {
    //         insertImplicitDereference (receiver);
    //       }
    //     node.args ()->Prepend (receiver);
    //     tvlist.insert (tvlist.begin (), receiver->typed_value);
    //     // Reset the expression to a literal.
    //     typed_value_t method_tv = node.expr ()->typed_value;
    //     method_tv.type = type.function_type;
    //     node.expr_ref () = new ast_literal_expr_t (node.location.Line, method_tv);

    //     unimplemented;
    //     //TypeCheckCall (node, type.function_type->GetSignature (), type.return_parameter->value, node.args (), tvlist);

    //     switch (type.kind)
    //       {
    //       case Type::Method::METHOD:
    //         // No restrictions on caller.
    //         break;
    //       case Type::Method::INITIALIZER:
    //         // Caller must be an initializer.
    //         if (context != Node::Initializer)
    //           {
    //             error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                            "initializers may only be called from initializers (E31)");
    //           }
    //         break;
    //       case Type::Method::GETTER:
    //       {
    //         // Must be called from either a getter, action, reaction, or initializer.
    //         if (!(context == Node::Getter ||
    //               context == Node::Action ||
    //               context == Node::Reaction ||
    //               context == Node::Initializer))
    //           {
    //             error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                            "getters may only be called from a getter, an action, a reaction, or an initializer (E32)");
    //           }

    //         if (node.GetInMutableSection ())
    //           {
    //             error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
    //                            "cannot call getter in mutable section (E33)");
    //           }
    //       }
    //       break;
    //       case Type::Method::REACTION:
    //       {
    //         unimplemented;
    //       }
    //       break;
    //       }
    //   }
    // };

    // visitor v (*this, node, tvlist);
    // expr_tv.type->Accept (v);
  }

  void visit (ast_push_port_call_expr_t& node)
  {
    Node *expr = node.identifier ();
    Node *args = node.args ();
    const std::string& port_identifier = ast_get_identifier (expr);
    const Type::Type *this_type = node.GetReceiverType ();
    const Type::Function *push_port_type = Type::type_cast<Type::Function> (type_select (this_type, port_identifier));
    if (push_port_type == NULL || push_port_type->function_kind != Type::Function::PUSH_PORT)
      {
        error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                       "no port named %s (E34)", port_identifier.c_str ());
      }
    TypedValueListType tvlist;
    TypeCheckArgs (args, tvlist);
    unimplemented;
    // TypeCheckCall (node, push_port_type->GetSignature (), push_port_type->GetReturnParameter ()->value, args, tvlist);
    // node.field = type_select_field (this_type, port_identifier);
  }

//   void visit (ast_index_expr_t& node)
//   {
//     typed_value_t base_tv = TypeCheckExpression (node.base ());
//     typed_value_t index_tv = CheckAndImplicitlyDereference (node.index_ref ());
//     base_tv.RequireReferenceOrValue (node.base ()->location);
//     index_tv.RequireReferenceOrValue (node.index ()->location);

//     struct visitor : public Type::DefaultVisitor
//     {
//       ast_index_expr_t& node;
//       visitor (ast_index_expr_t& n) : node (n) { }

//       void default_action (const Type::Type& type)
//       {
//         type_not_reached (type);
//       }

//       void visit (const Array& type)
//       {
//         Operation* index_op;

//         typed_value_t base_tv = node.base ()->typed_value;
//         typed_value_t index_tv = node.index ()->typed_value;
//         if (!(index_tv.type->IsInteger () || index_tv.type->IsUntyped ()))
//           {
//             error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
//                            "array index must be an integer (E101)");

//           }
//         if (index_tv.value.present)
//           {
//             if (!index_tv.RepresentableBy (Type::Int::Instance ()))
//               {
//                 error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
//                                "array index cannot be represented by an integer (E121)");
//               }
//             typed_value_t index_int_tv = index_tv.Convert (node.location, Type::Int::Instance (), node);
//             Type::Int::ValueType v = index_int_tv.value.ref (*Type::Int::Instance ());
//             if (v < 0)
//               {
//                 error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
//                                "array index is negative (E141)");
//               }
//             if (v >= type.dimension)
//               {
//                 error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
//                                "array index is out of range (E142)");
//               }

//             if (base_tv.value.present)
//               {
//                 // Get the value from the array.
//                 unimplemented;
//               }
//             index_op = make_literal (v);
//           }
//         else
//           {
//             index_op = MakeConvertToInt (new EvaluateNode (node.index ()), index_tv.type);
//           }

//         Operation* base_op = new EvaluateNode (node.base ());

//         if (base_tv.IsReference ())
//           {
//             node.typed_value =
//               typed_value_t::make_ref (type.Base (),
//                                        base_tv.intrinsic_mutability,
//                                        base_tv.dereference_mutability,
//                                        base_tv.component_state);
//             node.operation = new IndexArrayReference (node.location, base_op, index_op, type);
//           }
//         else if (base_tv.IsValue ())
//           {
//             node.typed_value =
//               typed_value_t::make_value (type.Base (),
//                                          base_tv.intrinsic_mutability,
//                                          base_tv.dereference_mutability,
//                                          base_tv.component_state);
//             node.operation = new IndexArrayValue (node.location, base_op, index_op, type);
//           }
//         else
//           {
//             not_reached;
//           }
//       }

//       void visit (const Slice& type)
//       {
//         Operation* index_op;

//         typed_value_t base_tv = node.base ()->typed_value;
//         typed_value_t index_tv = node.index ()->typed_value;

//         if (!(index_tv.type->IsInteger () || index_tv.type->IsUntyped ()))
//           {
//             error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
//                            "slice index must be an integer (E97)");

//           }
//         if (index_tv.value.present)
//           {
//             if (!index_tv.RepresentableBy (Type::Int::Instance ()))
//               {
//                 error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
//                                "slice index cannot be represented by an integer (E98)");
//               }
//             typed_value_t index_int_tv = index_tv.Convert (node.location, Type::Int::Instance (), node);
//             Type::Int::ValueType v = index_int_tv.value.ref (*Type::Int::Instance ());
//             if (v < 0)
//               {
//                 error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
//                                "slice index is negative (E99)");
//               }
//             if (base_tv.value.present)
//               {
//                 // Get the value from the slice.
//                 unimplemented;
//               }
//             index_op = make_literal (v);
//           }
//         else
//           {
//             index_op = MakeConvertToInt (new EvaluateNode (node.index ()), index_tv.type);
//           }

//         node.typed_value =
//           typed_value_t::make_ref (type.Base (),
//                                    base_tv.intrinsic_mutability,
//                                    base_tv.dereference_mutability,
//                                    base_tv.component_state);

//         Operation* base_op = new EvaluateNode (node.base ());
//         if (base_tv.kind == typed_value_t::REFERENCE)
//           {
//             base_op = new Load (base_op, &type);
//           }
//         node.operation = new IndexSlice (node.location, base_op, index_op, type);
//       }
//     } v (node);
//     base_tv.type->Accept (v);
//   }

//   void visit (ast_slice_expr_t& node)
//   {
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
//   }
};
