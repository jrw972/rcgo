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
  virtual void Execute(ErrorList* a_error_list) = 0;
};

struct NoOp : public CommandI {
  void Execute(ErrorList* a_error_list) override {}
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

  explicit Build(const BuildFlags& a_build_flags)
      : install_dependencies(false), build_flags(a_build_flags) {}

  void Execute(ErrorList* a_error_list) override;
};

struct ArgumentParser {
  ArgumentParser(int argc, char** argv)
      : m_argument_scanner(argc, argv),
        m_build(m_build_flags) {}

  CommandI* CommandLine(ErrorList* a_error_list);

 private:
  ArgumentScanner m_argument_scanner;
  NoOp m_no_op;
  BuildFlags m_build_flags;
  rcgo::Build m_build;

  CommandI* Bug(ErrorList* a_error_list);
  CommandI* Build(ErrorList* a_error_list);
  void BuildFlag(ErrorList* a_error_list);
  CommandI* Clean(ErrorList* a_error_list);
  CommandI* Command(ErrorList* a_error_list);
  CommandI* Doc(ErrorList* a_error_list);
  CommandI* Env(ErrorList* a_error_list);
  CommandI* Fix(ErrorList* a_error_list);
  CommandI* Fmt(ErrorList* a_error_list);
  CommandI* Generate(ErrorList* a_error_list);
  CommandI* Get(ErrorList* a_error_list);
  CommandI* Help(ErrorList* a_error_list);
  CommandI* Install(ErrorList* a_error_list);
  CommandI* List(ErrorList* a_error_list);
  CommandI* Mod(ErrorList* a_error_list);
  void OptionalBuildArguments(ErrorList* a_error_list);
  void OptionalPaths();
  CommandI* Run(ErrorList* a_error_list);
  CommandI* Test(ErrorList* a_error_list);
  CommandI* Tool(ErrorList* a_error_list);
  unsigned int UnsignedInteger(unsigned int default_value,
                               ErrorList* a_error_list);
  CommandI* Version(ErrorList* a_error_list);
  CommandI* Vet(ErrorList* a_error_list);
};

}  // namespace rcgo

#endif  // SRC_ARGUMENT_PARSER_H_
