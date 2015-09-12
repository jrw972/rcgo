#ifndef semantic_hpp
#define semantic_hpp

#include "types.hpp"
#include "ast.hpp"
#include <error.h>

/* Associate a symbol table with each node in the tree. */
void construct_symbol_table (ast_t * node);

/* Enter all symbols except vars and parameters. */
void enter_symbols (ast_t * node);

// Enter a symbol.
Symbol* enter_symbol (symtab_t* symtab, Symbol * symbol, symbol_holder& holder);

// Enter a signature.
void enter_signature (ast_t * node, const signature_type_t * type);

// Look up a symbol.  If it is not defined, process its definition.
template<typename T>
T* processAndLookup (ast_t * node, const std::string& identifier)
{
  Symbol *symbol = node->symtab->find (identifier);
  if (symbol == NULL)
    {
      error_at_line (-1, 0, node->location.file, node->location.line,
                     "%s was not declared in this scope", identifier.c_str ());
    }
  if (symbol->in_progress)
    {
      error_at_line (-1, 0, node->location.file, node->location.line,
                     "%s is defined recursively", identifier.c_str ());
    }
  if (!symbol->defined ())
    {
      // Process the definition.
      unimplemented;
    }

  return SymbolCast<T> (symbol);
}

// Look up a symbol.  Error if it is not defined.
Symbol * lookup_no_force (ast_t * node, const std::string& identifier);

// Extract an array dimension or error.
typed_value_t process_array_dimension (ast_t* ptr);

// Check that port and reaction sigantures have appropriate foreign attributes.
void check_port_reaction_signature (const signature_type_t* signature);

// Process a type specification.
const type_t * process_type_spec (ast_t * node, bool force_identifiers, bool is_component = false, named_type_t* named_type = NULL);

/* Process all declarations (non-code). */
void process_declarations (ast_t * node);

typed_value_t type_check_expr (ast_t* ptr);

void
check_assignment (typed_value_t left_tv,
                  typed_value_t right_tv,
                  const ast_t& node,
                  const char* conversion_message,
                  const char* leak_message,
                  const char* store_foreign_message);

/* Process all definitions (code). */
void process_definitions (ast_t * node);

void
enumerate_instances (ast_t* node, instance_table_t& instance_table);

void
allocate_stack_variables (ast_t* node);

Method*
get_current_method (const ast_t * node);

#endif /* semantic_hpp */
