#ifndef SRC_TEST_HELPERS_H_
#define SRC_TEST_HELPERS_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/error_reporter.h"
#include "src/import_location.h"
#include "src/symbol.h"

namespace rcgo {
namespace test {

template<typename V, typename T>
struct AcceptTestVisitor : public V {
  T* t;

  AcceptTestVisitor() : t(NULL) {}

  void DefaultAction(T* t) override { this->t = t; }
};

struct AbortHandler : public TerminationHandlerI {
  void Terminate() override { abort(); }
};

static AbortHandler abort_handler;
static Location location = Location::Make("undefined", 1);
static ImportLocation import_location("path", Location::Make("undefined", 1));

struct TestSymbol : public Symbol {
  TestSymbol()
      : Symbol("identifier", Location::Make("undefined", 1), NULL, NULL) {}
  void Accept(SymbolVisitor* visitor) {}
};

static TestSymbol test_symbol;

}  // namespace test
}  // namespace rcgo

#endif  // SRC_TEST_HELPERS_H_
