#include "ast.hpp"
#include "Symbol.hpp"
#include <error.h>
#include "semantic.hpp"
#include "trigger.hpp"
#include "action.hpp"
#include "field.hpp"
#include "parameter.hpp"
#include "Callable.hpp"

Symbol*
enter_symbol (ast_t& node, Symbol * symbol)
{
    // Check if the symbol is defined locally.
    const std::string& identifier = symbol->identifier;
    Symbol *s = node.FindSymbolCurrent (identifier);
    if (s == NULL)
        {
            node.EnterSymbol (symbol);
        }
    else
        {
            const ast_t* node = symbol->definingNode;
            error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                           "%s is already defined in this scope", identifier.c_str ());
        }
    return symbol;
}

void
check_assignment (typed_value_t left_tv,
                  typed_value_t right_tv,
                  const ast_t& node,
                  const char* conversion_message,
                  const char* leak_message,
                  const char* store_foreign_message)
{
    assert (left_tv.type != NULL);
    assert (left_tv.kind == typed_value_t::REFERENCE);
    assert (right_tv.type != NULL);
    assert (right_tv.kind == typed_value_t::VALUE);

    if (left_tv.intrinsic_mutability != MUTABLE)
        {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           "E9: target of assignment is not mutable");
        }

    if (!(
                type_is_equal (left_tv.type, right_tv.type) ||
                (type_cast<pointer_type_t> (type_strip(left_tv.type)) && right_tv.type == nil_type_t::instance())
            ))
        {
            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                           conversion_message, left_tv.type->to_string ().c_str (), right_tv.type->to_string ().c_str ());
        }

    if (type_contains_pointer (left_tv.type))
        {
            if (left_tv.dereference_mutability < right_tv.intrinsic_mutability ||
                    left_tv.dereference_mutability < right_tv.dereference_mutability)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "%s", leak_message);
                }

            if (right_tv.intrinsic_mutability == FOREIGN &&
                    left_tv.region != typed_value_t::STACK)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "%s", store_foreign_message);
                }
        }
}

static void check_rvalue_list (ast_t * node);

static bool
in_mutable_section (const ast_t* node)
{
    return node->GetAction () != NULL &&
           node->GetTrigger () != NULL;
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
checkAndImplicitlyDereference (ast_t*& expr)
{
    typed_value_t tv = type_check_expr (expr);
    if (tv.isReference ())
        {
            // Insert a dereference node.
            tv = insertImplicitDereference (expr, tv);
        }
    return tv;
}

static typed_value_t
insertExplicitDerefence (ast_t*& expr, typed_value_t tv)
{
    expr = new ast_dereference_expr_t (expr->location.Line, expr);
    tv = typed_value_t::dereference (tv);
    expr->typed_value = tv;
    return tv;
}

static typed_value_t
checkExpectReference (ast_t* expr)
{
    typed_value_t tv = type_check_expr (expr);
    if (!tv.isReference ())
        {
            error_at_line (EXIT_FAILURE, 0, expr->location.File.c_str (), expr->location.Line, "E46: expected reference");
        }
    return tv;
}

typed_value_t
type_check_expr (ast_t* ptr)
{
    struct check_visitor : public ast_visitor_t
    {
        ast_t* ptr;

        check_visitor (ast_t* p) : ptr (p) { }

        void default_action (ast_t& node)
        {
            ast_not_reached(node);
        }

        void visit (ast_cast_expr_t& node)
        {
            const type_t* type = process_type_spec (node.type_spec (), true);
            typed_value_t tv = checkAndImplicitlyDereference (node.child_ref ());
            node.typed_value = typed_value_t::cast (node.location, type, tv);
        }

        void visit (ast_indexed_port_call_expr_t& node)
        {
            const std::string& port_identifier = ast_get_identifier (node.identifier ());
            const type_t *this_type = node.GetReceiverType ();
            const type_t *type = type_select (this_type, port_identifier);

            if (type == NULL)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "no port named %s", port_identifier.c_str ());
                }

            const array_type_t* array_type = type_cast<array_type_t> (type);
            if (!array_type)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "%s is not an array of ports", port_identifier.c_str ());
                }

            const push_port_type_t* push_port_type = type_cast<push_port_type_t> (array_type->base_type ());

            if (!push_port_type)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "%s is not an array of ports", port_identifier.c_str ());
                }

            typed_value_t index_tv = checkAndImplicitlyDereference (node.index_ref ());

            typed_value_t::index (node.index ()->location, typed_value_t::make_ref (array_type, typed_value_t::HEAP, IMMUTABLE, IMMUTABLE), index_tv);


            ast_t *args = node.args ();
            check_rvalue_list (args);
            typed_value_t tv = typed_value_t::make_value (void_type_t::instance (),
                               typed_value_t::STACK,
                               IMMUTABLE,
                               IMMUTABLE);
            check_call (node, push_port_type->signature (), tv, args);
            node.field = type_select_field (this_type, port_identifier);
            node.array_type = array_type;
        }

        void visit (ast_merge_expr_t& node)
        {
            typed_value_t in = checkAndImplicitlyDereference (node.child_ref ());
            typed_value_t out = typed_value_t::merge (in);
            if (out.type == NULL)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "cannot merge expression of type %s", in.type->to_string ().c_str ());
                }
            node.typed_value = out;
        }

        void visit (ast_move_expr_t& node)
        {
            typed_value_t in = checkAndImplicitlyDereference (node.child_ref ());
            typed_value_t out = typed_value_t::move (in);
            if (out.type == NULL)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "cannot move expression of type %s", in.type->to_string ().c_str ());
                }
            node.typed_value = out;
        }

        void visit (ast_new_expr_t& node)
        {
            ast_t* type_spec = node.child ();
            const type_t* t = process_type_spec (type_spec, false);
            const type_t* type = pointer_type_t::make (t);
            node.typed_value = typed_value_t::make_value (type, typed_value_t::HEAP, MUTABLE, MUTABLE);
        }

        void visit (ast_copy_expr_t& node)
        {
            node.typed_value = typed_value_t::copy (node.location, checkAndImplicitlyDereference (node.child_ref ()));
        }

        void visit (ast_identifier_expr_t& node)
        {
            ast_t *identifier_node = node.child ();
            const std::string& identifier = ast_get_identifier (identifier_node);
            Symbol *symbol = node.FindSymbol (identifier);
            if (symbol == NULL)
                {
                    error_at_line (-1, 0, identifier_node->location.File.c_str (),
                                   identifier_node->location.Line, "E57: %s is not defined",
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

                void visit (const Function& symbol)
                {
                    node.typed_value = symbol.value ();
                }

                void visit (const ParameterSymbol& symbol)
                {
                    node.typed_value = symbol.value;
                }

                void visit (const TypeSymbol& symbol)
                {
                    error_at_line (-1, 0, node.location.File.c_str (),
                                   node.location.Line, "%s is a type (and not an expression)",
                                   symbol.identifier.c_str ());
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
                    std::cout << symbol.identifier << '\n';
                    error_at_line (-1, 0, node.location.File.c_str (),
                                   node.location.Line, "E47: %s is not accessible in this scope",
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

            if (in.isValue () && type_dereference (in.type))
                {
                    // Pointer value.
                    // Insert an explicit dereference.
                    in = insertExplicitDerefence (node.base_ref (), in);
                }

            if (in.isReference ())
                {
                    typed_value_t out = typed_value_t::select (in, identifier);
                    if (out.isError ())
                        {
                            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                           "E23: cannot select %s from expression of type %s",
                                           identifier.c_str (), in.type->to_string ().c_str ());
                        }
                    node.typed_value = out;
                }
            else if (in.isValue ())
                {
                    unimplemented;
                }
        }

        void visit (ast_dereference_expr_t& node)
        {
            typed_value_t in = checkAndImplicitlyDereference (node.child_ref ());
            typed_value_t out = typed_value_t::dereference (in);
            if (out.isError ())
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E1: incompatible types: (%s)@", in.type->to_string ().c_str ());
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
            if (out.isError ())
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E2: incompatible types: (%s)&", in.type->to_string ().c_str ());
                }
            node.typed_value = out;
        }

        void visit (ast_address_of_expr_t& node)
        {
            typed_value_t in = checkExpectReference (node.child ());
            typed_value_t out = typed_value_t::address_of (in);
            if (out.isError ())
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E45: incompatible types: &(%s)", in.type->to_string ().c_str ());
                }
            node.typed_value = out;
        }

        void visit (ast_logic_not_expr_t& node)
        {
            typed_value_t in = checkAndImplicitlyDereference (node.child_ref ());
            typed_value_t out = typed_value_t::logic_not (in);
            if (out.isError ())
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E3: incompatible types (%s) !", in.type->to_string ().c_str ());
                }
            node.typed_value = out;
        }

        void visit (ast_binary_arithmetic_expr_t& node)
        {
            typed_value_t left = checkAndImplicitlyDereference (node.left_ref ());
            typed_value_t right = checkAndImplicitlyDereference (node.right_ref ());
            typed_value_t result = typed_value_t::binary (node.location, node.arithmetic, left, right);
            if (result.isError ())
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E4: incompatible types (%s) %s (%s)", left.type->to_string ().c_str (), binary_arithmetic_symbol (node.arithmetic), right.type->to_string ().c_str ());
                }
            node.typed_value = result;
        }

        void check_call (ast_expr_t& node, const signature_type_t* signature, typed_value_t return_value, ast_t* args)
        {
            size_t argument_count = args->size ();
            size_t parameter_count = signature->arity ();
            if (argument_count != parameter_count)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E51: call expects %zd arguments but given %zd",
                                   parameter_count, argument_count);
                }

            size_t idx = 0;
            for (signature_type_t::const_iterator pos = signature->begin (),
                    limit = signature->end ();
                    pos != limit;
                    ++pos, ++idx)
                {
                    ast_t *arg = args->at (idx);
                    typed_value_t argument_tv  = arg->typed_value;
                    typed_value_t parameter_tv = typed_value_t::make_ref ((*pos)->value);
                    check_assignment (parameter_tv, argument_tv, *arg,
                                      "E5: incompatible types (%s) = (%s)",
                                      "E19: argument leaks mutable pointers",
                                      "argument may store foreign pointer");
                }

            // Set the return type.
            node.typed_value = return_value;
        }

        void visit (ast_call_expr_t& node)
        {
            // Analyze the args.
            check_rvalue_list (node.args ());

            // Analyze the callee.
            // Expecting a value.
            typed_value_t expr_tv = checkAndImplicitlyDereference (node.expr_ref ());

            struct visitor : public const_type_visitor_t
            {
                check_visitor& rvalue_visitor;
                ast_call_expr_t& node;

                visitor (check_visitor& rv,
                         ast_call_expr_t& n) : rvalue_visitor (rv), node (n) { }

                void default_action (const type_t& type)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "cannot call %s", type.to_string ().c_str ());
                }

                void visit (const function_type_t& type)
                {
                    // No restrictions on caller.
                    rvalue_visitor.check_call (node, type.signature (), type.return_parameter ()->value, node.args ());
                }

                void visit (const method_type_t& type)
                {
                    // No restrictions on caller.
                    rvalue_visitor.check_call (node, type.signature, type.return_parameter ()->value, node.args ());

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
                                      "E48: call expects %s but given %s",
                                      "E18: argument leaks mutable pointers",
                                      "argument may store foreign pointer");
                }

                void visit (const initializer_type_t& type)
                {
                    // Caller must be an initializer.
                    Initializer* initializer = node.GetInitializer ();

                    if (initializer == NULL)
                        {
                            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                           "E25: initializers may only be called from initializers");
                        }

                    rvalue_visitor.check_call (node, type.signature, type.return_parameter ()->value, node.args ());

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
                                      "E49: call expects %s but given %s",
                                      "E18: argument leaks mutable pointers",
                                      "argument may store foreign pointer");
                }

                void visit (const getter_type_t& type)
                {
                    unimplemented;
                    // // Must be called from either a getter, an action, or reaction.
                    // if (get_current_getter (&node) == NULL &&
                    //         get_current_action (&node) == NULL)
                    //     {
                    //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                    //                        "E26: getters may only be called from a getter, an action, or a reaction");
                    //     }

                    // rvalue_visitor.check_call (node, type.signature, type.return_parameter->value, node.args ());
                    // if (in_mutable_section (&node))
                    //     {
                    //         error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                    //                        "cannot call getter in mutable section");
                    //     }
                }

                void visit (const pull_port_type_t& type)
                {
                    // Must be called from either a getter, an action, or reaction.
                    if (node.GetGetter () == NULL &&
                            node.GetAction () == NULL)
                        {
                            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                           "E26: pull ports may only be called from a getter, an action, or a reaction");
                        }

                    rvalue_visitor.check_call (node, type.signature (), type.return_parameter ()->value, node.args ());
                    if (in_mutable_section (&node))
                        {
                            error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                           "cannot call pull port in mutable section");
                        }
                }
            };

            visitor v (*this, node);
            expr_tv.type->accept (v);
            assert (expr_tv.type != NULL);
        }

        void visit (ast_push_port_call_expr_t& node)
        {
            ast_t *expr = node.identifier ();
            ast_t *args = node.args ();
            const std::string& port_identifier = ast_get_identifier (expr);
            const type_t *this_type = node.GetReceiverType ();
            const push_port_type_t *push_port_type = type_cast<push_port_type_t> (type_select (this_type, port_identifier));
            if (push_port_type == NULL)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E30: no port named %s", port_identifier.c_str ());
                }
            check_rvalue_list (args);
            typed_value_t tv = typed_value_t::make_value (void_type_t::instance (),
                               typed_value_t::STACK,
                               IMMUTABLE,
                               IMMUTABLE);
            check_call (node, push_port_type->signature (), tv, args);
            node.field = type_select_field (this_type, port_identifier);
        }

        void visit (ast_index_expr_t& node)
        {
            typed_value_t base_tv = checkExpectReference (node.base_ref ());
            typed_value_t idx_tv = checkAndImplicitlyDereference (node.index_ref ());
            typed_value_t result = typed_value_t::index (node.location, base_tv, idx_tv);
            if (result.isError ())
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E6: incompatible types (%s)[%s]",
                                   base_tv.type->to_string ().c_str (),
                                   idx_tv.type->to_string ().c_str ());
                }
            node.typed_value = result;
        }

        void visit (ast_slice_expr_t& node)
        {
            typed_value_t base_tv = checkExpectReference (node.base_ref ());
            typed_value_t low_tv = checkAndImplicitlyDereference (node.low_ref ());
            typed_value_t high_tv = checkAndImplicitlyDereference (node.high_ref ());
            typed_value_t result = typed_value_t::slice (node.location, base_tv, low_tv, high_tv);
            if (result.isError ())
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E37: incompatible types (%s)[%s : %s]",
                                   base_tv.type->to_string ().c_str (),
                                   low_tv.type->to_string ().c_str (),
                                   high_tv.type->to_string ().c_str ());
                }
            node.typed_value = result;
        }
    };
    check_visitor check_lvalue_visitor (ptr);
    ptr->accept (check_lvalue_visitor);
    return ptr->typed_value;
}

// void
// type_check_rvalue (ast_t::iterator ptr)
// {
//   struct check_rvalue_visitor_t : public ast_visitor_t
//   {

//     void
//     binary_arithmetic (ast_binary_expr_t& node, const char* operator_str)
//     {
//       type_check_rvalue (node.left_iter ());
//       type_check_rvalue (node.right_iter ());
//       typed_value_t left_tv = ast_get_typed_value (node.left ());
//       typed_value_t right_tv = ast_get_typed_value (node.right ());
//       typed_value_t tv = typed_value_binary_arithmetic (left_tv, right_tv);
//       if (!tv.type)
//         {
//           error_at_line (-1, 0, node.file, node.line,
//                          "incompatible types (%s) %s (%s)", left_tv.type->to_string ().c_str (), operator_str, right_tv.type->to_string ().c_str ());
//         }
//       node.set_type (tv);
//     }

//   };
//   check_rvalue_visitor_t check_rvalue_visitor (ptr);
//   (*ptr)->accept (check_rvalue_visitor);
// }

// TODO:  Fold into check_rvalue.
static void
check_rvalue_list (ast_t * node)
{
    struct check_rvalue_list_visitor_t : public ast_visitor_t
    {
        void default_action (ast_t& node)
        {
            not_reached;
        }

        void visit (ast_list_expr_t& node)
        {
            for (ast_t::iterator child = node.begin (), limit = node.end ();
                    child != limit;
                    ++child)
                {
                    checkAndImplicitlyDereference (*child);
                }
        }
    };

    check_rvalue_list_visitor_t check_rvalue_list_visitor;
    node->accept (check_rvalue_list_visitor);
}

static typed_value_t
check_condition (ast_t*& condition_node)
{
    typed_value_t tv = checkAndImplicitlyDereference (condition_node);
    if (!type_is_boolean (tv.type))
        {
            error_at_line (-1, 0, condition_node->location.File.c_str (),
                           condition_node->location.Line,
                           "cannot convert (%s) to boolean expression in condition", tv.type->to_string ().c_str ());
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
            process_declarations (&node);
        }

        void visit (ast_empty_statement_t& node)
        { }

        typed_value_t bind (ast_t& node, ast_t* port_node, ast_t*& reaction_node)
        {
            checkExpectReference (port_node);
            checkAndImplicitlyDereference (reaction_node);

            typed_value_t port_tv = port_node->typed_value;
            typed_value_t reaction_tv = reaction_node->typed_value;

            const push_port_type_t *push_port_type = type_cast<push_port_type_t> (port_tv.type);

            if (push_port_type == NULL)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E28: source of bind is not a port");
                }

            const reaction_type_t *reaction_type = type_cast<reaction_type_t> (reaction_tv.type);
            if (reaction_type == NULL)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "target of bind is not a reaction");
                }

            if (!type_is_equal (push_port_type->signature (), reaction_type->signature ()))
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "cannot bind %s to %s", push_port_type->to_string ().c_str (), reaction_type->to_string ().c_str ());
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
            typed_value_t param_tv = checkAndImplicitlyDereference (node.param_ref ());
            assert (reaction_tv.value.present);
            reaction_t* reaction = reaction_tv.value.reaction_value ();
            if (!reaction->has_dimension ())
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "parameter specified for non-parameterized reaction");
                }
            typed_value_t dimension = reaction->dimension ();
            typed_value_t::index (node.location, typed_value_t::make_ref (new array_type_t (dimension.integral_value (), reaction->reaction_type), typed_value_t::CONSTANT, IMMUTABLE, IMMUTABLE), param_tv);
        }

        void visit (ast_bind_pull_port_statement_t& node)
        {
            typed_value_t pull_port_tv = checkExpectReference (node.left ());
            typed_value_t getter_tv = checkAndImplicitlyDereference (node.right_ref ());

            const pull_port_type_t* pull_port_type = type_cast<pull_port_type_t> (pull_port_tv.type);

            if (pull_port_type == NULL)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "target of bind is not a pull port");
                }

            const getter_type_t* getter_type = type_cast<getter_type_t> (getter_tv.type);

            if (getter_type == NULL)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "E29: source of bind is not a getter");
                }

            if (!type_is_equal (pull_port_type->bind_type (), getter_type->bind_type ))
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "cannot bind %s to %s", pull_port_type->to_string ().c_str (), getter_type->to_string ().c_str ());
                }
        }

        void visit (ast_for_iota_statement_t& node)
        {
            const std::string& identifier = ast_get_identifier (node.identifier ());
            typed_value_t limit = process_array_dimension (node.limit_node_ref ());
            typed_value_t zero = limit;
            zero.zero ();
            Symbol* symbol = new VariableSymbol (identifier, node.identifier (), typed_value_t::make_ref (typed_value_t::make_range (zero, limit, typed_value_t::STACK, IMMUTABLE, IMMUTABLE)));
            node.symbol = enter_symbol (node, symbol);
            type_check_statement (node.body ());
            node.limit = limit;
        }

        static typed_value_t
        check_assignment_target (ast_t* left)
        {
            typed_value_t tv = checkExpectReference (left);
            if (tv.intrinsic_mutability != MUTABLE)
                {
                    error_at_line (-1, 0, left->location.File.c_str (), left->location.Line,
                                   "E52: cannot assign to read-only location of type %s", tv.type->to_string ().c_str ());
                }

            return tv;
        }

        static void arithmetic_assign (ast_binary_t* node, const char* symbol)
        {
            typed_value_t left_tv = check_assignment_target (node->left ());
            typed_value_t right_tv = checkAndImplicitlyDereference (node->right_ref ());
            if (!type_is_equal (left_tv.type, right_tv.type))
                {
                    error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                                   "E7: incompatible types (%s) %s (%s)", left_tv.type->to_string ().c_str (), symbol, right_tv.type->to_string ().c_str ());
                }

            struct visitor : public const_type_visitor_t
            {
                ast_t* node;
                const char* symbol;

                visitor (ast_t* n, const char* s) : node (n), symbol (s) { }

                void visit (const named_type_t& type)
                {
                    type.subtype ()->accept (*this);
                }

                void visit (const uint_type_t& type)
                {
                    // Okay.
                }

                void default_action (const type_t& type)
                {
                    error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                                   "E8: incompatible types (%s) %s (%s)", type.to_string ().c_str (), symbol, type.to_string ().c_str ());
                }
            };
            visitor v (node, symbol);
            left_tv.type->accept (v);
        }

        void visit (ast_assign_statement_t& node)
        {
            typed_value_t left_tv = check_assignment_target (node.left ());
            typed_value_t right_tv = checkAndImplicitlyDereference (node.right_ref ());
            check_assignment (left_tv, right_tv, node,
                              "E31: incompatible types (%s) = (%s)",
                              "E32: assignment leaks mutable pointers",
                              "E33:assignment may store foreign pointer");
        }

        void visit (ast_change_statement_t& node)
        {
            const std::string& identifier = ast_get_identifier (node.identifier ());
            ast_t* type = node.type ();
            ast_t* body = node.body ();

            // Process the expression.
            typed_value_t tv = checkAndImplicitlyDereference (node.expr_ref ());

            const type_t* root_type = type_change (tv.type);
            if (root_type == NULL)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "cannot change expression of type %s", tv.type->to_string ().c_str ());
                }

            // Process the root variable.
            const type_t* proposed_root_type = process_type_spec (type, false);

            if (!type_is_equal (proposed_root_type, root_type))
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "cannot convert %s to %s in change", root_type->to_string ().c_str (), proposed_root_type->to_string ().c_str ());
                }

            // Enter the new heap root.
            Symbol* symbol = new VariableSymbol (identifier, &node, typed_value_t::make_ref (root_type, typed_value_t::STACK, MUTABLE, MUTABLE));
            node.root_symbol = enter_symbol (node, symbol);

            // Enter all parameters and variables in scope that are pointers as pointers to foreign.
            node.Change ();

            // Check the body.
            type_check_statement (body);
        }

        void visit (ast_expression_statement_t& node)
        {
            checkAndImplicitlyDereference (node.child_ref ());
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
            // Check the expression.
            typed_value_t expr_tv = checkAndImplicitlyDereference (node.child_ref ());

            // Check that it matches with the return type.
            node.return_symbol = node.GetReturnSymbol ();
            assert (node.return_symbol != NULL);

            check_assignment (SymbolCast<ParameterSymbol> (node.return_symbol)->value, expr_tv, node,
                              "cannot convert to (%s) from (%s) in return",
                              "return leaks mutable pointers",
                              "return may store foreign pointer");
        }

        void visit (ast_increment_statement_t& node)
        {
            ast_t* expr = node.child ();
            check_assignment_target (expr);
            struct visitor : public const_type_visitor_t
            {
                ast_t& node;

                visitor (ast_t& n) : node (n) { }

                void default_action (const type_t& type)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "cannot increment location of type %s", type.to_string ().c_str ());
                }

                void visit (const named_type_t& type)
                {
                    type.subtype ()->accept (*this);
                }

                void visit (const uint_type_t& type)
                {
                    // Okay.
                }
            };
            visitor v (node);
            expr->typed_value.type->accept (v);
        }

        void visit (ast_decrement_statement_t& node)
        {
            unimplemented;
        }

        void visit (ast_trigger_statement_t& node)
        {
            action_reaction_base_t *action = node.GetAction ();
            ast_t *expression_list_node = node.expr_list ();
            ast_t *body_node = node.body ();

            trigger_t *trigger = new trigger_t (node);
            action->add_trigger (trigger);

            /* Check the triggers. */
            check_rvalue_list (expression_list_node);

            /* Re-insert this as a pointer to mutable. */
            node.this_symbol = node.Trigger ();

            /* Check the body. */
            type_check_statement (body_node);

            node.trigger = trigger;
        }

        void visit (ast_var_statement_t& node)
        {
            ast_t* identifier_list = node.identifier_list ();
            ast_t* type_spec = node.type_spec ();

            // Process the type spec.
            const type_t* type = process_type_spec (type_spec, true);

            // Enter each symbol.
            for (ast_t::const_iterator pos = identifier_list->begin (),
                    limit = identifier_list->end ();
                    pos != limit;
                    ++pos)
                {
                    const std::string& name = ast_get_identifier (*pos);
                    Symbol* symbol = new VariableSymbol (name, *pos, typed_value_t::make_ref (type, typed_value_t::STACK, node.mutability, node.dereferenceMutability));
                    node.symbols.push_back (enter_symbol (*node.parent (), symbol));
                }
        }

        void visit (ast_var_type_init_statement_t& node)
        {
            ast_t* identifier_list = node.identifier_list ();
            ast_t* type_spec = node.type_spec ();
            ast_t* initializer_list = node.initializer_list ();

            // Process the type spec.
            const type_t* type = process_type_spec (type_spec, true);

            if (identifier_list->size () != initializer_list->size ())
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "wrong number of initializers");
                }

            typed_value_t left_tv = typed_value_t::make_ref (type, typed_value_t::STACK, node.mutability, node.dereferenceMutability);

            // Enter each symbol.
            for (ast_t::iterator id_pos = identifier_list->begin (),
                    id_limit = identifier_list->end (),
                    init_pos = initializer_list->begin ();
                    id_pos != id_limit;
                    ++id_pos, ++init_pos)
                {
                    // Process the initializer.
                    typed_value_t right_tv = checkAndImplicitlyDereference (*init_pos);

                    check_assignment (left_tv, right_tv, node,
                                      "E34: incompatible types (%s) = (%s)",
                                      "E35: assignment leaks mutable pointers",
                                      "E36: assignment may store foreign pointer");

                    const std::string& name = ast_get_identifier (*id_pos);
                    Symbol* symbol = new VariableSymbol (name, *id_pos, left_tv);
                    node.symbols.push_back (enter_symbol (*node.parent (), symbol));
                }
        }

        void visit (ast_println_statement_t& node)
        {
            check_rvalue_list (node.child ());
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
        bool in_trigger_statement;

        visitor () : in_trigger_statement (false) { }

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

        void visit (ast_trigger_statement_t& node)
        {
            ast_t *body_node = node.body ();
            action_reaction_base_t *action = node.GetAction ();

            if (action == NULL)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "trigger outside of action or reaction");
                }

            if (in_trigger_statement)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "triggers within triggers are not allowed");
                }

            in_trigger_statement = true;
            body_node->accept (*this);
            in_trigger_statement = false;
        }
    };

    visitor v;
    node->accept (v);
}

/*
  Determine if each trigger mutates the state of a component.
  This requires pointer analysis and could be improved.
  However, pointer analysis solves the halting problem so we must draw the line somewhere.

  The state of a component is only mutable in the body of a trigger statement so we can exclude all other activity.
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
            if (type_cast<pointer_type_t> (tv.type))
                {
                    derived_visitor v;
                    node->accept (v);
                    mutates_receiver = mutates_receiver || v.derived_from_receiver;
                }
        }

        void visit (ast_var_type_init_statement_t& node)
        {
            node.initializer_list ()->accept (*this);
        }

        void visit (ast_call_expr_t& node)
        {
            node.expr ()->accept (*this);

            if (type_cast<method_type_t> (node.expr ()->typed_value.type) != NULL)
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

        void visit (ast_logic_not_expr_t& node)
        {
            node.visit_children (*this);
        }

        void visit (ast_binary_arithmetic_expr_t& node)
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

        void visit (ast_println_statement_t& node)
        {
            node.child ()->accept (*this);
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

        void visit (ast_trigger_statement_t& node)
        {
            check_visitor v;
            node.body ()->accept (v);
            if (v.mutates_receiver)
                {
                    node.trigger->action = TRIGGER_WRITE;
                }
        }
    };

    visitor v;
    node->accept (v);
}

// TODO: Replace node with its symbol table.
void
enter_signature (ast_t& node, const signature_type_t * type)
{
    for (signature_type_t::ParametersType::const_iterator pos = type->begin (), limit = type->end ();
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
                                   "E58: %s is already defined in this scope",
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
                    if (tv.value.ref (*bool_type_t::instance ()))
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
            // Check the initialization function.
            Symbol* symbol = node.symbol;
            const named_type_t* type = SymbolCast<InstanceSymbol> (symbol)->type;
            ast_t* initializer_node = node.initializer ();
            Initializer* initializer = type->get_initializer (ast_get_identifier (initializer_node));
            if (initializer == NULL)
                {
                    error_at_line (-1, 0, initializer_node->location.File.c_str (),
                                   initializer_node->location.Line,
                                   "E21: no initializer named %s",
                                   ast_get_identifier (initializer_node).c_str ());
                }
            if (initializer->initializerType->signature->arity () != 0)
                {
                    error_at_line (-1, 0, initializer_node->location.File.c_str (),
                                   initializer_node->location.Line,
                                   "named method is not null-ary");
                }
            SymbolCast<InstanceSymbol> (symbol)->initializer = initializer;
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
