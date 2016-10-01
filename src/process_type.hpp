#ifndef RC_SRC_PROCESS_TYPE_HPP
#define RC_SRC_PROCESS_TYPE_HPP

#include "types.hpp"

namespace semantic
{

// Process an array dimension.  Returns -1 on error.
long
process_array_dimension (util::ErrorReporter& er,
                         decl::Scope* scope,
                         ast::Node* ptr);

// Check that all parameters in a parameter list have foreign indirection mutability if necessary.
void
check_for_foreign_safe (util::ErrorReporter& er,
                        const decl::ParameterList* parameter_list);

// Process a receiver.
type::NamedType*
process_receiver (util::ErrorReporter& er,
                  decl::Scope* scope,
                  ast::Receiver* node,
                  decl::Parameter*& receiver_symbol,
                  bool require_component,
                  bool require_immutable_dereference_mutability);

// Process a signature.
const decl::ParameterList*
process_parameter_list (util::ErrorReporter& er,
                        decl::Scope* scope,
                        ast::ParameterList* node,
                        bool is_return);

void
process_signature (util::ErrorReporter& er,
                   decl::Scope* scope,
                   ast::ParameterList* parameter_list_node,
                   ast::ParameterList* return_parameter_list_node,
                   bool require_foreign_safe,
                   const decl::ParameterList*& parameter_list,
                   const decl::ParameterList*& return_parameter_list);

// Process a type specification.
// Pre:  node->eval.expression_kind == UnknownExpressionKind
// Post: node->eval.expression_kind is one of ErrorExpressionKind, TypeExpressionKind, or EmptyTypeExpressionKind
//       If TypeExpressionKind, then node->eval.type contains the type.
void
process_type (ast::Node* node,
              util::ErrorReporter& er,
              decl::Scope* scope,
              bool require_named_types_to_be_defined = true);

}

#endif // RC_SRC_PROCESS_TYPE_HPP
