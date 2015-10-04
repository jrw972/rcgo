#include "ast.hpp"
#include "semantic.hpp"
#include <error.h>
#include "Symbol.hpp"
#include "action.hpp"
#include "bind.hpp"
#include "parameter.hpp"
#include "Callable.hpp"

static named_type_t* process_receiver (ast_t* n, parameter_t*& this_parameter, bool requireComponent, bool requireImmutableDereferenceMutability)
{
    ast_receiver_t* node = ast_cast<ast_receiver_t> (n);
    assert (node != NULL);

    ast_t *type_identifier_node = node->type_identifier ();
    const std::string& type_identifier = ast_get_identifier (type_identifier_node);
    TypeSymbol* symbol = processAndLookup<TypeSymbol> (type_identifier_node, type_identifier);
    if (symbol == NULL)
        {
            error_at_line (-1, 0, type_identifier_node->location.File.c_str (), type_identifier_node->location.Line,
                           "E53: %s does not refer to a type",
                           type_identifier.c_str ());
        }

    named_type_t *type = symbol->type;

    if (requireComponent && type_cast<component_type_t> (type_strip (type)) == NULL)
        {
            error_at_line (-1, 0, type_identifier_node->location.File.c_str (), type_identifier_node->location.Line,
                           "E54: %s does not refer to a component",
                           type_identifier.c_str ());
        }

    if (requireImmutableDereferenceMutability && node->dereferenceMutability < IMMUTABLE)
        {
            error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                           "E55: receiver must be declared +const or +foreign");

        }

    // Insert "this" into the symbol table of the parent.
    ast_t *this_identifier_node = node->this_identifier ();
    const std::string& this_identifier = ast_get_identifier (this_identifier_node);
    typed_value_t this_value =
        typed_value_t::make_value (pointer_type_t::make (type),
                                   typed_value_t::STACK,
                                   node->mutability,
                                   node->dereferenceMutability);

    this_parameter = new parameter_t (this_identifier_node,
                                      this_identifier,
                                      this_value,
                                      true);

    enter_symbol (*node->parent (),
                  ParameterSymbol::makeReceiver (this_parameter),
                  node->this_symbol);

    return type;
}

void
process_declarations (ast_t * node)
{
    struct visitor : public ast_visitor_t
    {
        void default_action (ast_t& node)
        {
            ast_not_reached (node);
        }

        void visit (ast_const_t& node)
        {
            typed_value_t right_tv = checkAndImplicitlyDereference (node.expr_ref ());

            if (!right_tv.value.present)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "expression is not constant");
                }

            const type_t* type = process_type_spec (node.type_spec (), true);
            typed_value_t left_tv = typed_value_t::make_ref (type, typed_value_t::STACK, MUTABLE, IMMUTABLE);

            check_assignment (left_tv, right_tv, node,
                              "E11: incompatible types (%s) = (%s)",
                              "argument leaks mutable pointers",
                              "argument may store foreign pointer");

            enter_symbol (*node.parent (),
                          new TypedConstantSymbol (ast_get_identifier (node.identifier ()), &node, right_tv),
                          node.symbol);
        }

        void visit (ast_action_t& node)
        {
            parameter_t* this_parameter;
            named_type_t* type = process_receiver (node.receiver (), this_parameter, true, true);
            action_t *action = new action_t (type, &node, node.body ());
            type->add_action (action);
            node.action = action;
        }

        void visit (ast_dimensioned_action_t& node)
        {
            typed_value_t dimension = process_array_dimension (node.dimension_ref ());
            parameter_t* this_parameter;
            named_type_t* type = process_receiver (node.receiver (), this_parameter, true, true);

            /* Insert "iota" into the symbol table. */
            typed_value_t zero = dimension;
            zero.zero ();

            typed_value_t iota_value = typed_value_t::make_range (zero, dimension, typed_value_t::STACK, IMMUTABLE, IMMUTABLE);

            parameter_t* iota_parameter = new parameter_t (node.dimension (),
                    "IOTA",
                    iota_value,
                    false);
            enter_symbol (node,
                          ParameterSymbol::make (iota_parameter),
                          node.iota_symbol);

            action_t *action = new action_t (type, &node, node.body (), dimension);
            type->add_action (action);
            node.action = action;
        }

        void visit (ast_bind_t& node)
        {
            parameter_t* this_parameter;
            named_type_t* type = process_receiver (node.receiver (), this_parameter, true, false);
            bind_t* bind = new bind_t (&node);
            type->add_bind (bind);
            node.bind = bind;
        }

        void visit (ast_function_t& node)
        {
            ast_t *signature_node = node.signature ();
            ast_t *return_type_node = node.return_type ();
            Function* function = SymbolCast<Function> (node.function_symbol.symbol ());

            /* Process the signature. */
            const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));

            /* Process the return type. */
            const type_t *return_type = process_type_spec (return_type_node, true);
            typed_value_t return_value = typed_value_t::make_value (return_type,
                                         typed_value_t::STACK,
                                         MUTABLE,
                                         MUTABLE);

            parameter_t* return_parameter = new parameter_t (return_type_node,
                    "0return",
                    return_value,
                    false);

            Symbol* return_symbol = ParameterSymbol::makeReturn (return_parameter);

            function->set (new function_type_t (signature, return_parameter), return_symbol);

            // Enter the return first as it is deeper on the stack.
            enter_symbol (node, return_symbol, node.return_symbol);
            enter_signature (node, signature);

            node.function = function;
        }

        void visit (ast_method_t& node)
        {
            // Process the return type first so that symbol will be entered first.
            ast_t *return_type_node = node.return_type ();
            const type_t *return_type = process_type_spec (node.return_type (), true);
            typed_value_t return_value = typed_value_t::make_value (return_type,
                                         typed_value_t::STACK,
                                         MUTABLE,
                                         node.return_dereference_mutability);
            parameter_t* return_parameter = new parameter_t (return_type_node,
                    "0return",
                    return_value,
                    false);
            Symbol* return_symbol = ParameterSymbol::makeReturn (return_parameter);
            enter_symbol (node, return_symbol, node.return_symbol);

            // Process the receiver.
            parameter_t* this_parameter;
            named_type_t* type = process_receiver (node.receiver (), this_parameter, false, false);
            ast_t *identifier_node = node.identifier ();
            const std::string& identifier = ast_get_identifier (identifier_node);
            const type_t *t = type_select (type, identifier);
            if (t != NULL)
                {
                    error_at_line (-1, 0, identifier_node->location.File.c_str (),
                                   identifier_node->location.Line,
                                   "component already contains a member named %s",
                                   identifier.c_str ());
                }

            if (type_strip_cast<component_type_t> (type) != NULL &&
                    type_dereference (this_parameter->value.type) == NULL)
                {
                    // Components must have pointer receivers.
                    error_at_line (-1, 0, node.location.File.c_str (),
                                   node.location.Line,
                                   "component methods must have pointer receiver");
                }

            // Process the signature.
            ast_t *signature_node = node.signature ();
            const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));
            enter_signature (node, signature);

            method_type_t* method_type = new method_type_t (type,
                    this_parameter,
                    signature,
                    return_parameter);
            Method* method = new Method (&node, identifier, method_type, return_symbol);

            type->add_method (method);
            node.method = method;
        }

        void visit (ast_initializer_t& node)
        {
            parameter_t* this_parameter;
            named_type_t* type = process_receiver (node.receiver (), this_parameter, true, false);
            ast_t *signature_node = node.signature ();
            ast_t *identifier_node = node.identifier ();
            const std::string& identifier = ast_get_identifier (identifier_node);
            const type_t *t = type_select (type, identifier);
            if (t != NULL)
                {
                    error_at_line (-1, 0, identifier_node->location.File.c_str (),
                                   identifier_node->location.Line,
                                   "component already contains a member named %s",
                                   identifier.c_str ());
                }

            /* Process the signature. */
            const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));

            const type_t *return_type = void_type_t::instance ();
            typed_value_t return_value = typed_value_t::make_value (return_type,
                                         typed_value_t::STACK,
                                         MUTABLE,
                                         IMMUTABLE);

            parameter_t* return_parameter = new parameter_t (&node,
                    "0return",
                    return_value,
                    false);

            initializer_type_t* initializer_type =
                new initializer_type_t (type,
                                        this_parameter,
                                        signature,
                                        return_parameter);

            Initializer* initializer = new Initializer (&node, identifier, initializer_type);

            enter_signature (node, signature);

            type->add_initializer (initializer);
            node.initializer = initializer;
        }

        void visit (ast_getter_t& node)
        {
            // Process the return type first so that symbol will be entered first.
            ast_t *return_type_node = node.return_type ();
            const type_t *return_type = process_type_spec (return_type_node, true);
            typed_value_t return_value = typed_value_t::make_value (return_type,
                                         typed_value_t::STACK,
                                         MUTABLE,
                                         node.dereferenceMutability);
            parameter_t* return_parameter = new parameter_t (return_type_node,
                    "0return",
                    return_value,
                    false);
            Symbol* return_symbol = ParameterSymbol::makeReturn (return_parameter);
            enter_symbol (node, return_symbol, node.return_symbol);

            // Process the receiver.
            parameter_t* this_parameter;
            named_type_t* type = process_receiver (node.receiver (), this_parameter, true, true);
            ast_t *identifier_node = node.identifier ();
            const std::string& identifier = ast_get_identifier (identifier_node);
            const type_t *t = type_select (type, identifier);
            if (t != NULL)
                {
                    error_at_line (-1, 0, identifier_node->location.File.c_str (),
                                   identifier_node->location.Line,
                                   "type already contains a member named %s",
                                   identifier.c_str ());
                }

            // Process the signature.
            ast_t *signature_node = node.signature ();
            const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));
            enter_signature (node, signature);

            getter_type_t* getter_type = new getter_type_t (type,
                    this_parameter,
                    signature,
                    return_parameter);

            Getter* getter = new Getter (&node, identifier, getter_type, return_symbol);

            type->add_getter (getter);
            node.getter = getter;
        }

        void visit (ast_instance_t& node)
        {
            ast_t *type_identifier_node = node.type_identifier ();
            const std::string& type_identifier = ast_get_identifier (type_identifier_node);
            TypeSymbol *symbol = processAndLookup<TypeSymbol> (type_identifier_node, ast_get_identifier (type_identifier_node));
            if (symbol == NULL)
                {
                    error_at_line (-1, 0, type_identifier_node->location.File.c_str (),
                                   type_identifier_node->location.Line,
                                   "%s does not refer to a type",
                                   type_identifier.c_str ());
                }
            named_type_t *type = symbol->type;
            if (type_cast<component_type_t> (type_strip (type)) == NULL)
                {
                    error_at_line (-1, 0, type_identifier_node->location.File.c_str (),
                                   type_identifier_node->location.Line,
                                   "%s does not refer to a component",
                                   type_identifier.c_str ());
                }
            SymbolCast<InstanceSymbol> (node.symbol.symbol ())->type = type;
        }

        void visit (ast_reaction_t& node)
        {
            parameter_t* this_parameter;
            named_type_t* type = process_receiver (node.receiver (), this_parameter, true, true);
            ast_t *signature_node = node.signature ();
            ast_t *identifier_node = node.identifier ();
            const std::string& identifier = ast_get_identifier (identifier_node);
            const type_t *t = type_select (type, identifier);
            if (t != NULL)
                {
                    error_at_line (-1, 0, identifier_node->location.File.c_str (),
                                   identifier_node->location.Line,
                                   "component already contains a member named %s",
                                   identifier.c_str ());
                }

            /* Process the signature. */
            const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));
            check_port_reaction_signature (signature);
            enter_signature (node, signature);

            reaction_t* reaction = new reaction_t (type, &node, node.body (), identifier, signature);
            type->add_reaction (reaction);
            node.reaction = reaction;
        }

        void visit (ast_dimensioned_reaction_t& node)
        {
            typed_value_t dimension = process_array_dimension (node.dimension_ref ());
            parameter_t* this_parameter;
            named_type_t* type = process_receiver (node.receiver (), this_parameter, true, true);
            ast_t *signature_node = node.signature ();
            ast_t *identifier_node = node.identifier ();
            const std::string& identifier = ast_get_identifier (identifier_node);
            const type_t *t = type_select (type, identifier);
            if (t != NULL)
                {
                    error_at_line (-1, 0, identifier_node->location.File.c_str (),
                                   identifier_node->location.Line,
                                   "component already contains a member named %s",
                                   identifier.c_str ());
                }

            /* Insert "iota" into the symbol table. */
            typed_value_t zero = dimension;
            zero.zero ();

            typed_value_t iota_value = typed_value_t::make_range (zero, dimension, typed_value_t::STACK, IMMUTABLE, IMMUTABLE);

            parameter_t* iota_parameter = new parameter_t (node.dimension (),
                    "IOTA",
                    iota_value,
                    false);

            enter_symbol (node,
                          ParameterSymbol::make (iota_parameter),
                          node.iota_symbol);

            /* Process the signature. */
            const signature_type_t *signature = type_cast<signature_type_t> (process_type_spec (signature_node, true));
            check_port_reaction_signature (signature);
            enter_signature (node, signature);

            reaction_t* reaction = new reaction_t (type, &node, node.body (), identifier, signature, dimension);
            type->add_reaction (reaction);
            node.reaction = reaction;
        }

        void visit (ast_top_level_list_t& node)
        {
            for (ast_t::const_iterator pos = node.begin (), limit = node.end ();
                    pos != limit;
                    ++pos)
                {
                    process_declarations (*pos);
                }
        }

        void visit (ast_type_definition_t& node)
        {
            Symbol *symbol = node.symbol.symbol ();
            const std::string& identifier = symbol->identifier;
            named_type_t *type = SymbolCast<TypeSymbol> (symbol)->type;

            if (symbol->defined ())
                return;

            if (symbol->inProgress)
                {
                    error_at_line (-1, 0, node.location.File.c_str (), node.location.Line,
                                   "%s is defined recursively", identifier.c_str ());
                }

            symbol->inProgress = true;
            ast_t *type_spec_node = node.type_spec ();
            const type_t *new_type = process_type_spec (type_spec_node, true, false, type);
            type->subtype (new_type);
            symbol->inProgress = false;
        }
    };

    visitor v;
    node->accept (v);
}

Symbol *
lookup_no_force (ast_t * node, const std::string& identifier)
{
    Symbol *symbol = node->FindSymbol (identifier);
    if (symbol == NULL)
        {
            error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                           "%s was not declared in this scope", identifier.c_str ());
        }
    return symbol;
}
