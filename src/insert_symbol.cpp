/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Symbol.hpp"
#include "SymbolTable.hpp"
#include "Error.hpp"
#include "functions.hpp"

namespace rcgo {

  void insert_symbol (Block* block, Symbol* symbol, ErrorReporter& error_reporter)
  {
    Symbol* s = block->find_local_symbol (symbol->identifier);
    if (s == NULL)
      {
        block->insert (symbol);
      }
    else
      {
        error_reporter << DuplicateSymbol (symbol, s);
      }
  }

}
