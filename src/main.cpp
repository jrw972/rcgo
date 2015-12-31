#include <error.h>
#include <getopt.h>
#include <errno.h>

#include <cstdlib>
#include <cassert>

#include "config.h"
#include "scanner.hpp"
#include "yyparse.hpp"
#include "parser.hpp"
#include "semantic.hpp"
#include "debug.hpp"
#include "runtime.hpp"
#include "arch.hpp"
#include "instance_scheduler.hpp"
#include "partitioned_scheduler.hpp"
#include "generate_code.hpp"
#include "check_types.hpp"
#include "compute_receiver_access.hpp"
#include "process_types_and_constants.hpp"
#include "process_functions_and_methods.hpp"
#include "scheduler.hpp"
#include "symbol_table.hpp"

static void
print_version (void)
{
  std::cout <<
            PACKAGE_STRING "\n"
            "Copyright (C) 2014 Justin R. Wilson\n"
            "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n"
            "This is free software: you are free to change and redistribute it.\n"
            "There is NO WARRANTY, to the extent permitted by law.\n";
}

static void
try_help (void)
{
  std::cerr << "Try `" << program_invocation_short_name << " --help' for more information.\n";
  exit (EXIT_FAILURE);
}

int
main (int argc, char **argv)
{
  int show_composition = 0;
  int thread_count = 2;
  std::string scheduler_type = "partitioned";

  const char* s = getenv ("RC_SCHEDULER");
  if (s != NULL)
    {
      scheduler_type = s;
    }

  while (true)
    {
      static struct option long_options[] =
      {
        {"composition", no_argument, &show_composition, 1},
        {"help", no_argument, NULL, 'h'},
        {"scheduler", required_argument, NULL, 's'},
        {"threads", required_argument, NULL, 't'},
        {"version", no_argument, NULL, 'v'},
        {0, 0, 0, 0}
      };

      int c = getopt_long (argc, argv, "chvt:", long_options, NULL);

      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          break;
        case 'c':
          show_composition = 1;
          break;
        case 'h':
          std::cout << "Usage: " << program_invocation_short_name << " OPTION... FILE\n"
                    <<
                    "Compile " PACKAGE_NAME " source code.\n"
                    "\n"
                    "  -c, --composition            print composition analysis and exit\n"
                    "  -s SCHED, --scheduler=SCHED  select a scheduler (instance, partitioned)\n"
                    "  -t NUM, --threads=NUM        use NUM threads\n"
                    "  --help                       display this help and exit\n"
                    "  --version                    display version information and exit\n"
                    "\n"
                    "Report bugs to: " PACKAGE_BUGREPORT "\n";
          exit (EXIT_SUCCESS);
          break;
        case 's':
          scheduler_type = optarg;
          break;
        case 't':
          thread_count = atoi (optarg);
          break;
        case 'v':
          print_version ();
          exit (EXIT_SUCCESS);
          break;
        default:
          try_help ();
          break;
        }
    }

  if (optind + 1 != argc)
    {
      std::cerr << "No input file\n";
      try_help ();
    }

  if (thread_count < 0 || thread_count > 64)
    {
      error (EXIT_FAILURE, 0, "Illegal thread count: %d", thread_count);
    }

  util::Location::StaticFile = argv[optind];

  // Open the input file.
  yyin = fopen (util::Location::StaticFile.c_str (), "r");
  if (yyin == NULL)
    {
      error (EXIT_FAILURE, errno, "Could not open '%s'", util::Location::StaticFile.c_str ());
    }

  yylloc = 1;
  if (yyparse () != 0)
    {
      error (EXIT_FAILURE, 0, "yyparse failed");
    }
  assert (root != NULL);

  arch::set_stack_alignment (sizeof (void*));

  decl::SymbolTable symtab;
  symtab.open_scope ();
  semantic::enter_symbols (symtab);
  semantic::process_types_and_constants (root, symtab);
  semantic::process_functions_and_methods (root, symtab);
  semantic::check_types (root, symtab);
  semantic::compute_receiver_access (root);

  // Calculate the offsets of all stack variables.
  // Do this so we can execute some code statically when checking composition.
  semantic::allocate_stack_variables (root);

  // Generate code.
  code::generate_code (root);

  // Check composition.
  composition::Composer instance_table;
  instance_table.enumerate_instances (root);
  instance_table.elaborate ();
  if (show_composition)
    {
      instance_table.dump_graphviz ();
      return 0;
    }
  instance_table.analyze ();

  runtime::allocate_instances (instance_table);
  runtime::create_bindings (instance_table);

  runtime::Scheduler* scheduler;
  if (scheduler_type == "partitioned")
    {
      scheduler = new runtime::partitioned_scheduler_t ();
    }
  else if (scheduler_type == "instance")
    {
      scheduler = new runtime::instance_scheduler_t ();
    }
  else
    {
      error (EXIT_FAILURE, 0, "unknown scheduler type '%s'", scheduler_type.c_str ());
    }

  scheduler->run (instance_table, 8 * 1024, thread_count);

  return 0;
}
