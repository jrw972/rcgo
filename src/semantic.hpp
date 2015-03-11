#ifndef semantic_h
#define semantic_h

#include "types.hpp"
#include "ast.hpp"

/* Associate a symbol table with each node in the tree. */
void construct_symbol_table (ast_t * node);

/* Enter all symbols except vars and parameters. */
void enter_symbols (ast_t * node);

// Enter a symbol.
symbol_t* enter_symbol (symtab_t* symtab, symbol_t * symbol, symbol_holder& holder);

// Enter a signature.
void enter_signature (ast_t * node, const signature_type_t * type);

// Look up a symbol.  If it is not defined, process its definition.
symbol_t * lookup_force (ast_t * node, const std::string& identifier);

// Look up a symbol.  Error if it is not defined.
symbol_t * lookup_no_force (ast_t * node, const std::string& identifier);

// Extract an array dimension or error.
size_t process_array_dimension (ast_t::iterator ptr);

// Check that port and reaction sigantures have appropriate foreign attributes.
void check_port_reaction_signature (const signature_type_t* signature);

// Process a type specification.
const type_t * process_type_spec (ast_t * node, bool force_identifiers, bool is_component = false);

/* Process all declarations (non-code). */
void process_declarations (ast_t * node);

typed_value_t type_check_expr (ast_t::iterator ptr);

/* Process all definitions (code). */
void process_definitions (ast_t * node);

void
enumerate_instances (ast_t* node, instance_table_t& instance_table);

void
allocate_stack_variables (ast_t* node);

method_t *
get_current_method (const ast_t * node);

#endif /* semantic_h */
