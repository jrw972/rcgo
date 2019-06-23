#ifndef SRC_CHECK_TYPES_H_
#define SRC_CHECK_TYPES_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/block.h"
#include "src/error_reporter.h"
#include "src/type.h"

namespace rcgo {

void CheckTypes(
    ast::Node* source_file, Block* file_block, type::Factory* type_factory,
    ErrorList* error_list);

}  // namespace rcgo

#endif  // SRC_CHECK_TYPES_H_
