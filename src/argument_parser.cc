// Copyright 2018 The Contributors of rcgo
// All Contributions are owned by their respective authors.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/argument_parser.h"

#include <cstring>
#include <iostream>

#include "config.hpp"
#include "src/compile.h"
#include "src/import_location.h"
#include "src/package.h"
#include "src/paths.h"

namespace rcgo {

struct ExtraArgument : public Error {
  explicit ExtraArgument(const std::string& a_argument)
      : argument(a_argument) {}
  void Print(std::ostream& out) const override {
    out << "error: extra argument: " << argument << std::endl;
  }

  std::string const argument;
};

struct NoCommand : public Error {
  void Print(std::ostream& out) const override {
    out << "error: no command" << std::endl;
  }
};

struct UnknownCommand : public Error {
  explicit UnknownCommand(const std::string& a_command)
      : command(a_command) {}
  void Print(std::ostream& out) const override {
    out << "error: unknown command: " << command << std::endl;
  }

  std::string const command;
};

struct UnimplementedCommand : public Error {
  explicit UnimplementedCommand(const std::string& a_command)
      : command(a_command) {}
  void Print(std::ostream& out) const override {
    out << "error: unimplemented command: " << command << std::endl;
  }

  std::string const command;
};

struct UnknownFlag : public Error {
  explicit UnknownFlag(const std::string& a_flag)
      : flag(a_flag) {}
  void Print(std::ostream& out) const override {
    out << "error: unknown flag: " << flag << std::endl;
  }

  std::string const flag;
};

struct FlagRequiresParameter : public Error {
  explicit FlagRequiresParameter(const std::string& a_flag)
      : flag(a_flag) {}
  void Print(std::ostream& out) const override {
    out << "error: " << flag << " requires an argument" << std::endl;
  }

  std::string const flag;
};

struct ExpectedAnUnsignedInteger : public Error {
  explicit ExpectedAnUnsignedInteger(const std::string& a_argument)
      : argument(a_argument) {}
  void Print(std::ostream& out) const override {
    out << "error: cannot convert " << argument << " to an unsigned integer"
        << std::endl;
  }

  std::string const argument;
};

void Build::Execute() {
  if (paths.empty()) {
    // Build current directory.
    paths.push_back(".");
  }

  Paths path_list;

  for (const auto& path : paths) {
    Compile(path, &path_list, m_error_reporter, std::cout);
  }
}

CommandI* ArgumentParser::CommandLine() {
  // Consume the name of the program.
  m_argument_scanner.Consume();
  CommandI* command = Command();
  const char* token = NULL;
  while ((token = m_argument_scanner.Peek())) {
    m_error_reporter->Insert(ExtraArgument(token));
  }
  return command;
}

CommandI* ArgumentParser::Command() {
  const char* token = m_argument_scanner.Consume();
  if (token == NULL) {
    m_error_reporter->Insert(NoCommand());
  } else if (strcmp(token, "bug") == 0) {
    return Bug();
  } else if (strcmp(token, "build") == 0) {
    return Build();
  } else if (strcmp(token, "clean") == 0) {
    return Clean();
  } else if (strcmp(token, "doc") == 0) {
    return Doc();
  } else if (strcmp(token, "env") == 0) {
    return Env();
  } else if (strcmp(token, "fix") == 0) {
    return Fix();
  } else if (strcmp(token, "fmt") == 0) {
    return Fmt();
  } else if (strcmp(token, "generate") == 0) {
    return Generate();
  } else if (strcmp(token, "get") == 0) {
    return Get();
  } else if (strcmp(token, "help") == 0) {
    return Help();
  } else if (strcmp(token, "install") == 0) {
    return Install();
  } else if (strcmp(token, "list") == 0) {
    return List();
  } else if (strcmp(token, "mod") == 0) {
    return Mod();
  } else if (strcmp(token, "run") == 0) {
    return Run();
  } else if (strcmp(token, "test") == 0) {
    return Test();
  } else if (strcmp(token, "tool") == 0) {
    return Tool();
  } else if (strcmp(token, "version") == 0) {
    return Version();
  } else if (strcmp(token, "vet") == 0) {
    return Vet();
  } else {
    m_error_reporter->Insert(UnknownCommand(token));
  }
  return &m_no_op;
}

CommandI* ArgumentParser::Bug() {
  m_error_reporter->Insert(UnimplementedCommand("bug"));
  return &m_no_op;
}

CommandI* ArgumentParser::Build() {
  OptionalBuildArguments();
  OptionalPaths();
  return &m_build;
}

void ArgumentParser::BuildFlag() {
  const char* token = m_argument_scanner.Consume();
  if (strcmp(token, "-a") == 0) {
    m_build_flags.force_rebuild = true;
  } else if (strcmp(token, "-n") == 0) {
    m_build_flags.print_commands = true;
  } else if (strcmp(token, "-p") == 0) {
    const char* p = m_argument_scanner.Peek();
    if (p != NULL) {
      m_build_flags.parallelism = UnsignedInteger(0);
    } else {
      m_error_reporter->Insert(FlagRequiresParameter("-p"));
    }
  } else if (strcmp(token, "-v") == 0) {
    m_build_flags.print_packages = true;
  } else if (strcmp(token, "-work") == 0) {
    m_build_flags.print_and_keep_working_directory = true;
  } else {
    m_error_reporter->Insert(UnknownFlag(token));
  }
}

CommandI* ArgumentParser::Clean() {
  m_error_reporter->Insert(UnimplementedCommand("clean"));
  return &m_no_op;
}

CommandI* ArgumentParser::Doc() {
  m_error_reporter->Insert(UnimplementedCommand("doc"));
  return &m_no_op;
}

CommandI* ArgumentParser::Env() {
  m_error_reporter->Insert(UnimplementedCommand("env"));
  return &m_no_op;
}

CommandI* ArgumentParser::Fix() {
  m_error_reporter->Insert(UnimplementedCommand("fix"));
  return &m_no_op;
}

CommandI* ArgumentParser::Fmt() {
  m_error_reporter->Insert(UnimplementedCommand("fmt"));
  return &m_no_op;
}

CommandI* ArgumentParser::Generate() {
  m_error_reporter->Insert(UnimplementedCommand("generate"));
  return &m_no_op;
}

CommandI* ArgumentParser::Get() {
  m_error_reporter->Insert(UnimplementedCommand("get"));
  return &m_no_op;
}

CommandI* ArgumentParser::Help() {
  m_error_reporter->Insert(UnimplementedCommand("help"));
  return &m_no_op;
}

CommandI* ArgumentParser::Install() {
  m_error_reporter->Insert(UnimplementedCommand("install"));
  return &m_no_op;
}

CommandI* ArgumentParser::List() {
  m_error_reporter->Insert(UnimplementedCommand("list"));
  return &m_no_op;
}

CommandI* ArgumentParser::Mod() {
  m_error_reporter->Insert(UnimplementedCommand("mod"));
  return &m_no_op;
}

void ArgumentParser::OptionalBuildArguments() {
  const char* token;
  while ((token = m_argument_scanner.Peek())) {
    if (token[0] != '-') return;
    if (strcmp(token, "-o") == 0) {
      m_argument_scanner.Consume();
      const char* output = m_argument_scanner.Consume();
      if (output != NULL) {
        m_build.output_file = output;
      } else {
        m_error_reporter->Insert(FlagRequiresParameter("-o"));
      }
    } else if (strcmp(token, "-i") == 0) {
      m_argument_scanner.Consume();
      m_build.install_dependencies = true;
    } else {
      BuildFlag();
    }
  }
}

void ArgumentParser::OptionalPaths() {
  const char* token;
  while ((token = m_argument_scanner.Consume())) {
    m_build.paths.push_back(token);
  }
}

CommandI* ArgumentParser::Run() {
  m_error_reporter->Insert(UnimplementedCommand("run"));
  return &m_no_op;
}

CommandI* ArgumentParser::Test() {
  m_error_reporter->Insert(UnimplementedCommand("test"));
  return &m_no_op;
}

CommandI* ArgumentParser::Tool() {
  m_error_reporter->Insert(UnimplementedCommand("tool"));
  return &m_no_op;
}

unsigned int ArgumentParser::UnsignedInteger(unsigned int default_value) {
  const char* token = m_argument_scanner.Peek();
  assert(token != NULL);
  char* end = NULL;
  unsigned int value = strtoul(token, &end, 0);
  if (end != NULL && *end == 0) {
    return value;
  } else {
    m_error_reporter->Insert(ExpectedAnUnsignedInteger(token));
    return default_value;
  }
}

CommandI* ArgumentParser::Version() {
  m_error_reporter->Insert(UnimplementedCommand("version"));
  return &m_no_op;
}

CommandI* ArgumentParser::Vet() {
  m_error_reporter->Insert(UnimplementedCommand("vet"));
  return &m_no_op;
}


}  // namespace rcgo
