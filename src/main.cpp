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
#include "instance.hpp"
#include "runtime.hpp"
#include "memory_model.hpp"
#include "instance_scheduler.hpp"
#include "partitioned_scheduler.hpp"

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
      static struct option long_options[] = {
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

  in_file = argv[optind];

  // Open the input file.
  yyin = fopen (in_file, "r");
  if (yyin == NULL)
    {
      error (EXIT_FAILURE, errno, "Could not open '%s'", in_file);
    }

  yylloc = 1;
  if (yyparse () != 0)
    {
      // TODO:  Error reporting.
      error (EXIT_FAILURE, 0, "yyparse failed");
    }
  assert (root != NULL);

  /* Check the semantics. */
  construct_symbol_table (root);
  enter_symbols (root);
  process_declarations (root);

  process_definitions (root);

  // Calculate the offsets of all stack variables.
  // Do this before checking composition so the receiver of binds has an offset.
  memory_model_t::stack_alignment = sizeof (void*);
  allocate_stack_variables (root);

  /* Check composition. */
  instance_table_t instance_table;
  enumerate_instances (root, instance_table);
  instance_table_enumerate_bindings (instance_table);
  instance_table_analyze_composition (instance_table);

  if (show_composition)
    {
      instance_table_dump (instance_table);
      return 0;
    }

  //typedef instance_scheduler_t SchedulerType;
  typedef partitioned_scheduler_t SchedulerType;

  SchedulerType scheduler;
  runtime::allocate_instances (instance_table);
  runtime::create_bindings (instance_table);
  scheduler.run (instance_table, 8 * 1024, thread_count);

  return 0;
}
