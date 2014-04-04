#ifndef RCGO_UTEST_SPIES_HPP
#define RCGO_UTEST_SPIES_HPP

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ErrorReporter.hpp"
#include "Symbol.hpp"
#include "ImportLocation.hpp"

using namespace rcgo;

template<typename V, typename T>
struct AcceptTestVisitor : public V {
  T* t;

  AcceptTestVisitor ()
    : t (NULL)
  { }

  virtual void default_action (T& t) { this->t = &t; }
};

struct AbortHandler : public TerminationHandlerI {
  void terminate ()
  {
    abort ();
  }
};

static AbortHandler abortHandler;
static Location location = Location::make ("undefined", 1);
static ImportLocation import_location ("path", Location::make ("undefined", 1));

struct TestSymbol : public Symbol {
  TestSymbol ()
    : Symbol ("identifier", Location::make ("undefined", 1), NULL, NULL)
  { }
  void accept (SymbolVisitor& visitor) { }
};

static TestSymbol test_symbol;

#endif // RCGO_UTEST_SPIES_HPP
