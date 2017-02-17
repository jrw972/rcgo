#ifndef RCGO_SRC_DECL_HPP
#define RCGO_SRC_DECL_HPP

#include "types.hpp"

namespace decl
{

// Enter builtin types, constants, functions, etc. into the symbol table.
void populate_universe_block (SymbolTable& symbol_table);

  void generate_package_symbols (util::ErrorReporter& er,
                                 SymbolTable& symbol_table,
                                 ast::Node* node);

  void generate_and_lookup_symbols (util::ErrorReporter& er,
                                    SymbolTable& symbol_table,
                                    ast::Node* node);

}

#endif /* RCGO_SRC_DECL_HPP */
