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

namespace {

Error extraArgument(std::string const& token) {
  Error error;
  error.message << "error: extra argument: " << token << std::endl;
  return error;
}

Error noCommand() {
  Error error;
  error.message << "error: no command" << std::endl;
  return error;
}

Error unknownCommand(std::string const& command) {
  Error error;
  error.message << "error: unknown command: " << command << std::endl;
  return error;
}

Error unimplementedCommand(std::string const& command) {
  Error error;
  error.message << "error: unimplemented command: " << command << std::endl;
  return error;
}

Error optionRequiresArgument(std::string const& option) {
  Error error;
  error.message << "error: " << option << " requires an argument" << std::endl;
  return error;
}

Error unknownOption(std::string const& option) {
  Error error;
  error.message << "error: unknown option: " << option << std::endl;
  return error;
}

Error cannotConvert(std::string const& token, std::string const& message) {
  Error error;
  error.message << "error: cannot convert " << token << " to " << message << std::endl;
  return error;
}

}  // namespace

void Build::Execute(ErrorList* a_error_list) {
  if (paths.empty()) {
    // Build current directory.
    paths.push_back(".");
  }

  Paths path_list;

  for (const auto& path : paths) {
    Compile(path, &path_list, a_error_list, std::cout);
  }
}

CommandI* ArgumentParser::CommandLine(ErrorList* a_error_list) {
  // Consume the name of the program.
  m_argument_scanner.Consume();
  CommandI* command = Command(a_error_list);
  const char* token = NULL;
  while ((token = m_argument_scanner.Peek())) {
    a_error_list->push_back(extraArgument(token));
  }
  return command;
}

CommandI* ArgumentParser::Command(ErrorList* a_error_list) {
  const char* token = m_argument_scanner.Consume();
  if (token == NULL) {
    a_error_list->push_back(noCommand());
  } else if (strcmp(token, "bug") == 0) {
    return Bug(a_error_list);
  } else if (strcmp(token, "build") == 0) {
    return Build(a_error_list);
  } else if (strcmp(token, "clean") == 0) {
    return Clean(a_error_list);
  } else if (strcmp(token, "doc") == 0) {
    return Doc(a_error_list);
  } else if (strcmp(token, "env") == 0) {
    return Env(a_error_list);
  } else if (strcmp(token, "fix") == 0) {
    return Fix(a_error_list);
  } else if (strcmp(token, "fmt") == 0) {
    return Fmt(a_error_list);
  } else if (strcmp(token, "generate") == 0) {
    return Generate(a_error_list);
  } else if (strcmp(token, "get") == 0) {
    return Get(a_error_list);
  } else if (strcmp(token, "help") == 0) {
    return Help(a_error_list);
  } else if (strcmp(token, "install") == 0) {
    return Install(a_error_list);
  } else if (strcmp(token, "list") == 0) {
    return List(a_error_list);
  } else if (strcmp(token, "mod") == 0) {
    return Mod(a_error_list);
  } else if (strcmp(token, "run") == 0) {
    return Run(a_error_list);
  } else if (strcmp(token, "test") == 0) {
    return Test(a_error_list);
  } else if (strcmp(token, "tool") == 0) {
    return Tool(a_error_list);
  } else if (strcmp(token, "version") == 0) {
    return Version(a_error_list);
  } else if (strcmp(token, "vet") == 0) {
    return Vet(a_error_list);
  } else {
    a_error_list->push_back(unknownCommand(token));
  }
  return &m_no_op;
}

CommandI* ArgumentParser::Bug(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("bug"));

  return &m_no_op;
}

CommandI* ArgumentParser::Build(ErrorList* a_error_list) {
  OptionalBuildArguments(a_error_list);
  OptionalPaths();
  return &m_build;
}

void ArgumentParser::BuildFlag(ErrorList* a_error_list) {
  const char* token = m_argument_scanner.Consume();
  if (strcmp(token, "-a") == 0) {
    m_build_flags.force_rebuild = true;
  } else if (strcmp(token, "-n") == 0) {
    m_build_flags.print_commands = true;
  } else if (strcmp(token, "-p") == 0) {
    const char* p = m_argument_scanner.Peek();
    if (p != NULL) {
      m_build_flags.parallelism = UnsignedInteger(0, a_error_list);
    } else {
      a_error_list->push_back(optionRequiresArgument("-p"));
    }
  } else if (strcmp(token, "-v") == 0) {
    m_build_flags.print_packages = true;
  } else if (strcmp(token, "-work") == 0) {
    m_build_flags.print_and_keep_working_directory = true;
  } else {
    a_error_list->push_back(unknownOption(token));
  }
}

CommandI* ArgumentParser::Clean(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("clean"));
  return &m_no_op;
}

CommandI* ArgumentParser::Doc(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("doc"));
  return &m_no_op;
}

CommandI* ArgumentParser::Env(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("env"));
  return &m_no_op;
}

CommandI* ArgumentParser::Fix(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("fix"));
  return &m_no_op;
}

CommandI* ArgumentParser::Fmt(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("fmt"));
  return &m_no_op;
}

CommandI* ArgumentParser::Generate(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("generate"));
  return &m_no_op;
}

CommandI* ArgumentParser::Get(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("get"));
  return &m_no_op;
}

CommandI* ArgumentParser::Help(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("help"));
  return &m_no_op;
}

CommandI* ArgumentParser::Install(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("install"));
  return &m_no_op;
}

CommandI* ArgumentParser::List(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("list"));
  return &m_no_op;
}

CommandI* ArgumentParser::Mod(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("mod"));
  return &m_no_op;
}

void ArgumentParser::OptionalBuildArguments(ErrorList* a_error_list) {
  const char* token;
  while ((token = m_argument_scanner.Peek())) {
    if (token[0] != '-') return;
    if (strcmp(token, "-o") == 0) {
      m_argument_scanner.Consume();
      const char* output = m_argument_scanner.Consume();
      if (output != NULL) {
        m_build.output_file = output;
      } else {
        a_error_list->push_back(optionRequiresArgument("-o"));
      }
    } else if (strcmp(token, "-i") == 0) {
      m_argument_scanner.Consume();
      m_build.install_dependencies = true;
    } else {
      BuildFlag(a_error_list);
    }
  }
}

void ArgumentParser::OptionalPaths() {
  const char* token;
  while ((token = m_argument_scanner.Consume())) {
    m_build.paths.push_back(token);
  }
}

CommandI* ArgumentParser::Run(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("run"));
  return &m_no_op;
}

CommandI* ArgumentParser::Test(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("test"));
  return &m_no_op;
}

CommandI* ArgumentParser::Tool(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("tool"));
  return &m_no_op;
}

unsigned int ArgumentParser::UnsignedInteger(unsigned int default_value,
                                             ErrorList* a_error_list) {
  const char* token = m_argument_scanner.Peek();
  assert(token != NULL);
  char* end = NULL;
  unsigned int value = strtoul(token, &end, 0);
  if (end != NULL && *end == 0) {
    return value;
  } else {
    a_error_list->push_back(cannotConvert(token, "an unsigned integer"));
    return default_value;
  }
}

CommandI* ArgumentParser::Version(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("version"));
  return &m_no_op;
}

CommandI* ArgumentParser::Vet(ErrorList* a_error_list) {
  a_error_list->push_back(unimplementedCommand("vet"));
  return &m_no_op;
}


}  // namespace rcgo
