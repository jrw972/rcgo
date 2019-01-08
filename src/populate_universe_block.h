#ifndef SRC_POPULATE_UNIVERSE_BLOCK_H_
#define SRC_POPULATE_UNIVERSE_BLOCK_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/block.h"
#include "src/type.h"

namespace rcgo {

// Creates and enters symbols in the universe block including:
//
// Types:
//      bool byte complex64 complex128 error float32 float64
//      int int8 int16 int32 int64 rune string
//      uint uint8 uint16 uint32 uint64 uintptr
//
// Constants:
//      true false iota
//
// Zero value:
//      nil
//
// Functions:
//      append cap complex copy delete imag len
//      make new panic print println real recover
void PopulateUniverseBlock(type::Factory* type_factory, MutableBlock* block);

}  // namespace rcgo

#endif  // SRC_POPULATE_UNIVERSE_BLOCK_H_
