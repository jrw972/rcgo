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
  struct ExitHandler : public TerminationHandlerI {
    void Terminate() override { exit(EXIT_FAILURE); }
  } exit_handler;

  ErrorReporter error_reporter(std::cerr, 0, &exit_handler);
  ArgumentParser argument_parser(argc, argv, &error_reporter);
  CommandI* command = argument_parser.CommandLine();
  if (!error_reporter.Empty()) {
    exit_handler.Terminate();
  }
  command->Execute();
  return error_reporter.Empty() ? 0 : 1;
}

}  // namespace rcgo

int main(int argc, char** argv) {
  return rcgo::_main(argc, argv);
}
