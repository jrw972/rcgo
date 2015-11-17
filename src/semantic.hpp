#ifndef semantic_hpp
#define semantic_hpp

#include "types.hpp"
#include "Ast.hpp"
#include <error.h>
#include "SymbolVisitor.hpp"
#include "Composition.hpp"

/* Enter all symbols except vars and parameters. */
void enter_symbols (Ast::Node* node);

// Enter a symbol.
template <typename T>
T*
enter_symbol (Ast::Node& node, T* symbol)
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
      const Ast::Node* node = symbol->definingNode;
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "%s is already defined in this scope (E113)", identifier.c_str ());
    }
  return symbol;
}

// Enter a signature.
void enter_signature (Ast::Node& node, const Type::Signature * type);

// Look up a symbol.  If it is not defined, process its definition.
template<typename T>
T* processAndLookup (Ast::Node * node, const std::string& identifier)
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
typed_value_t process_array_dimension (Ast::Node*& ptr);

// Check that a signature has +foreign where needed.
void CheckForForeignSafe (const Type::Signature* signature, const parameter_t* return_parameter);

// Process a type specification.
const Type::Type * process_type_spec (Ast::Node* node, bool force_identifiers, bool is_component = false, Type::NamedType* named_type = NULL);

/* Process all declarations (non-code). */
void ProcessDeclarations (Ast::Node* node);

// Type check the expression and insert an implicit dereference if necessary.
typed_value_t
CheckAndImplicitlyDereference (Ast::Node*& expr);

// Type check the expression, insert an implicit dereference if necessary, and convert to the given type if necessary.
typed_value_t
CheckAndImplicitlyDereferenceAndConvert (Ast::Node*& expr, const Type::Type* type);

typed_value_t
CheckAndImplicitlyDereferenceAndConvertToDefault (Ast::Node*& expr);

// Type check the expression expecting a reference.
typed_value_t CheckExpectReference (Ast::Node* expr);


typed_value_t
type_check_expr (Ast::Node* ptr);

void
check_assignment (typed_value_t left_tv,
                  typed_value_t right_tv,
                  const Ast::Node& node,
                  const char* conversion_message,
                  const char* leak_message);

/* Process all definitions (code). */
void process_definitions (Ast::Node* node);

void
enumerate_instances (Ast::Node* node, Composition::Composer& instance_table);

void
allocate_stack_variables (Ast::Node* node);

Method*
get_current_method (const Ast::Node * node);

ReceiverAccess ComputeReceiverAccess (const Ast::Node* node);

#endif /* semantic_hpp */
