#ifndef SRC_ARGUMENT_PARSER_H_
#define SRC_ARGUMENT_PARSER_H_

// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/argument_scanner.h"

#include <string>
#include <vector>

#include "src/error_reporter.h"
#include "src/paths.h"

namespace rcgo {

struct CommandI {
  virtual void Execute() = 0;
};

struct NoOp : public CommandI {
  void Execute() override {}
};

struct BuildFlags {
  bool force_rebuild;
  bool print_commands;
  unsigned int parallelism;
  bool print_packages;
  bool print_and_keep_working_directory;

  BuildFlags()
      : force_rebuild(false), print_commands(false), parallelism(false),
        print_packages(false), print_and_keep_working_directory(false) {}
};

struct Build : public CommandI {
  Path output_file;
  bool install_dependencies;
  const BuildFlags& build_flags;
  Paths paths;

  explicit Build(const BuildFlags& a_build_flags,
                 ErrorReporter* a_error_reporter)
      : install_dependencies(false), build_flags(a_build_flags),
        m_error_reporter(a_error_reporter) {}

  void Execute() override;

 private:
  ErrorReporter* m_error_reporter;
};

struct ArgumentParser {
  ArgumentParser(int argc, char** argv, ErrorReporter* a_error_reporter)
      : m_argument_scanner(argc, argv), m_error_reporter(a_error_reporter),
        m_build(m_build_flags, a_error_reporter) {}

  CommandI* CommandLine();

 private:
  ArgumentScanner m_argument_scanner;
  ErrorReporter* m_error_reporter;
  NoOp m_no_op;
  BuildFlags m_build_flags;
  rcgo::Build m_build;

  CommandI* Bug();
  CommandI* Build();
  void BuildFlag();
  CommandI* Clean();
  CommandI* Command();
  CommandI* Doc();
  CommandI* Env();
  CommandI* Fix();
  CommandI* Fmt();
  CommandI* Generate();
  CommandI* Get();
  CommandI* Help();
  CommandI* Install();
  CommandI* List();
  CommandI* Mod();
  void OptionalBuildArguments();
  void OptionalPaths();
  CommandI* Run();
  CommandI* Test();
  CommandI* Tool();
  unsigned int UnsignedInteger(unsigned int default_value);
  CommandI* Version();
  CommandI* Vet();
};

}  // namespace rcgo

#endif  // SRC_ARGUMENT_PARSER_H_
