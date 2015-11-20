#ifndef semantic_hpp
#define semantic_hpp

#include "types.hpp"
#include "ast.hpp"
#include <error.h>
#include "SymbolVisitor.hpp"
#include "Composition.hpp"

/* Enter all symbols except vars and parameters. */
void enter_symbols (ast::Node* node);

// Enter a symbol.
template <typename T>
T*
enter_symbol (ast::Node& node, T* symbol)
{
  // Check if the symbol is defined locally.
  const std::string& identifier = symbol->identifier;
  Symbol *s = node.FindLocalSymbol (identifier);
  if (s == NULL)
    {
      node.EnterSymbol (symbol);
    }
  else
    {
      const ast::Node* node = symbol->definingNode;
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "%s is already defined in this scope (E113)", identifier.c_str ());
    }
  return symbol;
}

// Enter a signature.
void enter_signature (ast::Node& node, const Type::Signature * type);

// Look up a symbol.  If it is not defined, process its definition.
template<typename T>
T* processAndLookup (ast::Node * node, const std::string& identifier)
{
  Symbol *symbol = node->FindGlobalSymbol (identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "%s was not declared in this scope (E114)", identifier.c_str ());
    }
  if (symbol->inProgress)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "%s is defined recursively (E115)", identifier.c_str ());
    }
  if (!symbol->defined ())
    {
      // Process the definition.
      unimplemented;
    }

  return SymbolCast<T> (symbol);
}

// Extract an array dimension or error.
typed_value_t process_array_dimension (ast::Node*& ptr);

// Check that a signature has +foreign where needed.
void CheckForForeignSafe (const Type::Signature* signature, const parameter_t* return_parameter);

// Process a type specification.
const Type::Type * process_type_spec (ast::Node* node, bool force_identifiers, bool is_component = false, Type::NamedType* named_type = NULL);

/* Process all declarations (non-code). */
void ProcessDeclarations (ast::Node* node);

// Type check the expression and insert an implicit dereference if necessary.
typed_value_t
CheckAndImplicitlyDereference (ast::Node*& expr);

// Type check the expression, insert an implicit dereference if necessary, and convert to the given type if necessary.
typed_value_t
CheckAndImplicitlyDereferenceAndConvert (ast::Node*& expr, const Type::Type* type);

typed_value_t
CheckAndImplicitlyDereferenceAndConvertToDefault (ast::Node*& expr);

// Type check the expression expecting a reference.
typed_value_t CheckExpectReference (ast::Node* expr);

typed_value_t
TypeCheckExpression (ast::Node* ptr);

// TODO:  Move this into TypeCheckCall.
void
TypeCheckArgs (ast::Node * node, TypedValueListType& tvlist);

void
TypeCheckCall (ast::Node& node,
               const Type::Signature* signature,
               typed_value_t return_value,
               ast::Node* argsnode,
               const TypedValueListType& args);

void
check_assignment (typed_value_t left_tv,
                  typed_value_t right_tv,
                  const ast::Node& node,
                  const char* conversion_message,
                  const char* leak_message);

/* Process all definitions (code). */
void process_definitions (ast::Node* node);

void
enumerate_instances (ast::Node* node, Composition::Composer& instance_table);

void
allocate_stack_variables (ast::Node* node);

Method*
get_current_method (const ast::Node * node);

ReceiverAccess ComputeReceiverAccess (const ast::Node* node);

#endif /* semantic_hpp */
