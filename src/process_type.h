#ifndef SRC_PROCESS_TYPE_H_
#define SRC_PROCESS_TYPE_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/ast.h"
#include "src/block.h"
#include "src/error_reporter.h"
#include "src/type.h"

namespace rcgo {

// Returns a type descriptor for the type in ast if successful and an
// instance of ErrorType otherwise.
const type::Type* ProcessType(
    ast::Node* node, Block* block, type::Factory* type_factory,
    ErrorList* error_list);

// Like process_type but specifically for function types such as those
// in interfaces.
const type::Function* ProcessFunction(
    ast::Signature* signature, Block* block, type::Factory* type_factory,
    ErrorList* error_list);

}  // namespace rcgo

#endif  // SRC_PROCESS_TYPE_H_
