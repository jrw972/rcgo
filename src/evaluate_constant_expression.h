#ifndef SRC_EVALUATE_CONSTANT_EXPRESSION_H_
#define SRC_EVALUATE_CONSTANT_EXPRESSION_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/error_reporter.h"
#include "src/type.h"

namespace rcgo {

// Evaluate the constant expression in ast.
Value EvaluateConstantExpression(
    ast::Node* ast, const Block& block, type::Factory* type_factory,
    ErrorReporter* error_reporter);

}

#endif  // SRC_EVALUATE_CONSTANT_EXPRESSION_H_
