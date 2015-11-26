#include <stdlib.h>
#include <assert.h>
#include <error.h>
#include <stdbool.h>
#include <getopt.h>
#include <errno.h>

#include "config.h"
#include "scanner.hpp"
#include "yyparse.hpp"
#include "parser.hpp"
#include "semantic.hpp"
#include "debug.hpp"
#include "runtime.hpp"
#include "MemoryModel.hpp"
#include "instance_scheduler.hpp"
#include "partitioned_scheduler.hpp"
#include "generate_code.hpp"
#include "check_types.hpp"
#include "check_references.hpp"
#include "check_mutability.hpp"
#include "check_constants.hpp"
#include "check_control.hpp"
#include "compute_receiver_access.hpp"
#include "process_types_and_constants.hpp"
#include "process_functions_and_methods.hpp"

static void
print_version (void)
{
  puts (PACKAGE_STRING);
  puts ("Copyright (C) 2014 Justin R. Wilson");
  puts
  ("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.");
  puts ("This is free software: you are free to change and redistribute it.");
  puts ("There is NO WARRANTY, to the extent permitted by law.");
}

static void
try_help (void)
{
  fprintf (stderr, "Try `%s --help' for more information.\n",
           program_invocation_short_name);
  exit (EXIT_FAILURE);
}

int
main (int argc, char **argv)
{
  int show_composition = 0;
  int thread_count = 2;

  while (true)
    {
      static struct option long_options[] =
      {
        {"composition", no_argument, &show_composition, 1},
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {"thread", required_argument, NULL, 't'},
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
        case 'v':
          print_version ();
          exit (EXIT_SUCCESS);
          break;
        case 'h':
          printf ("Usage: %s OPTION... FILE \n",
                  program_invocation_short_name);
          puts ("Compile " PACKAGE_NAME " source code.\n" "\n"
                "  -c, --composition print composition analysis and exit\n"
                "  --help      display this help and exit\n"
                "  --version   display version information and exit\n" "\n"
                "Report bugs to: " PACKAGE_BUGREPORT);
          exit (EXIT_SUCCESS);
          break;
        case 't':
          thread_count = atoi (optarg);
          break;
        default:
          try_help ();
          break;
        }
    }

  if (optind + 1 != argc)
    {
      fprintf (stderr, "No input file\n");
      try_help ();
    }

  if (thread_count < 0 || thread_count > 64)
    {
      error (EXIT_FAILURE, 0, "Illegal thread count: %d", thread_count);
    }

  Location::StaticFile = argv[optind];

  // Open the input file.
  yyin = fopen (Location::StaticFile.c_str (), "r");
  if (yyin == NULL)
    {
      error (EXIT_FAILURE, errno, "Could not open '%s'", Location::StaticFile.c_str ());
    }

  yylloc = 1;
  if (yyparse () != 0)
    {
      // TODO:  Error reporting.
      error (EXIT_FAILURE, 0, "yyparse failed");
    }
  assert (root != NULL);

  MemoryModel::StackAlignment = sizeof (void*);

  enter_symbols (root);
  semantic::process_types_and_constants (root);
  semantic::process_functions_and_methods (root);
  semantic::check_types (root);
  semantic::check_references (root);
  semantic::check_mutability (root);
  semantic::check_control (root);
  semantic::compute_receiver_access (root);

  // Calculate the offsets of all stack variables.
  // Do this so we can execute some code statically when checking composition.
  allocate_stack_variables (root);

  // Generate code.
  code::generate_code (root);

  // Check composition.
  Composition::Composer instance_table;
  enumerate_instances (root, instance_table);
  instance_table.ElaborateComposition ();
  if (show_composition)
    {
      instance_table.DumpGraphviz ();
      return 0;
    }
  instance_table.AnalyzeComposition ();

  //typedef instance_scheduler_t SchedulerType;
  typedef partitioned_scheduler_t SchedulerType;

  SchedulerType scheduler;
  runtime::allocate_instances (instance_table);
  runtime::create_bindings (instance_table);
  scheduler.run (instance_table, 8 * 1024, thread_count);

  return 0;
}
