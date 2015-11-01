#ifndef semantic_hpp
#define semantic_hpp

#include "types.hpp"
#include "ast.hpp"
#include <error.h>
#include "SymbolVisitor.hpp"

/* Enter all symbols except vars and parameters. */
void enter_symbols (ast_t * node);

// Enter a symbol.
template <typename T>
T*
enter_symbol (ast_t& node, T* symbol)
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

// Enter a signature.
void enter_signature (ast_t& node, const Type::Signature * type);

// Look up a symbol.  If it is not defined, process its definition.
template<typename T>
T* processAndLookup (ast_t * node, const std::string& identifier)
{
  Symbol *symbol = node->FindSymbol (identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "%s was not declared in this scope", identifier.c_str ());
    }
  if (symbol->inProgress)
    {
      error_at_line (-1, 0, node->location.File.c_str (), node->location.Line,
                     "%s is defined recursively", identifier.c_str ());
    }
  if (!symbol->defined ())
    {
      // Process the definition.
      unimplemented;
    }

  return SymbolCast<T> (symbol);
}

// Extract an array dimension or error.
typed_value_t process_array_dimension (ast_t*& ptr);

// Check that a signature has +foreign where needed.
void CheckForForeignSafe (const Type::Signature* signature, const parameter_t* return_parameter);

// Process a type specification.
const Type::Type * process_type_spec (ast_t * node, bool force_identifiers, bool is_component = false, Type::NamedType* named_type = NULL);

/* Process all declarations (non-code). */
void ProcessDeclarations (ast_t * node);

// Type check the expression and insert an implicit dereference.
typed_value_t
checkAndImplicitlyDereference (ast_t*& expr);

typed_value_t
type_check_expr (ast_t* ptr);

void
check_assignment (typed_value_t left_tv,
                  typed_value_t right_tv,
                  const ast_t& node,
                  const char* conversion_message,
                  const char* leak_message);

/* Process all definitions (code). */
void process_definitions (ast_t * node);

void
enumerate_instances (ast_t* node, instance_table_t& instance_table);

void
allocate_stack_variables (ast_t* node);

Method*
get_current_method (const ast_t * node);

#endif /* semantic_hpp */
