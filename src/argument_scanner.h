#ifndef SRC_ARGUMENT_SCANNER_H_
#define SRC_ARGUMENT_SCANNER_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

namespace rcgo {

struct ArgumentScanner {
  ArgumentScanner(int argc, char** argv)
      : m_argc(argc), m_argv(argv), m_idx(0) {}
  const char* Peek(int a_offset = 0) {
    return (m_idx + a_offset < m_argc) ? m_argv[m_idx + a_offset] : 0;
  }
  const char* Consume() {
    return (m_idx < m_argc) ? m_argv[m_idx++] : 0;
  }

 private:
  int m_argc;
  char** m_argv;
  int m_idx;
};

}  // namespace rcgo

#endif  // SRC_ARGUMENT_SCANNER_H_
