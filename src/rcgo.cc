// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>

#include "src/argument_parser.h"
#include "src/error_reporter.h"

namespace rcgo {

int _main(int argc, char** argv) {
  ErrorList error_list;
  ArgumentParser argument_parser(argc, argv);
  CommandI* command = argument_parser.CommandLine(&error_list);
  if (!error_list.empty()) {
    ReportErrors(std::cerr, error_list);
    return 1;
  }
  command->Execute(&error_list);
  if (!error_list.empty()) {
    ReportErrors(std::cerr, error_list);
    return 1;
  }
  return 0;
}

}  // namespace rcgo

int main(int argc, char** argv) {
  return rcgo::_main(argc, argv);
}
