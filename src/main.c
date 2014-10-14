#define _GNU_SOURCE
#include <stdlib.h>
#include <assert.h>
#include <error.h>
#include <stdbool.h>
#include <getopt.h>
#include <errno.h>

#include "config.h"
#include "scanner.h"
#include "yyparse.h"
#include "semantic.h"
#include "semantics.h"
#include "codegen.h"
#include "compile.h"
#include "debug.h"

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
  const char *include = "/usr/include";
  const char *outfile = NULL;

  while (true)
    {
      static struct option long_options[] = {
	{"debug", no_argument, &debug, 1},
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'v'},
	{0, 0, 0, 0}
      };

      int c = getopt_long (argc, argv, "I:c:o:dhv", long_options, NULL);

      if (c == -1)
	break;

      switch (c)
	{
	case 0:
	  break;
	case 'I':
	  include = optarg;
	  break;
	case 'c':
	  in_file = optarg;
	  break;
	case 'o':
	  outfile = optarg;
	  break;
	case 'd':
	  debug = 1;
	  break;
	case 'v':
	  print_version ();
	  exit (EXIT_SUCCESS);
	  break;
	case 'h':
	  printf ("Usage: %s OPTION... -c FILE -o FILE\n",
		  program_invocation_short_name);
	  puts ("Compile " PACKAGE_NAME " source code.\n" "\n"
		"  -I PATH     specifies the path to the FBU headers\n"
		"  -c FILE     specifies the input file\n"
		"  -o FILE     specifies the output file\n"
		"  -d, --debug turn on debugging output\n"
		"  --help      display this help and exit\n"
		"  --version   display version information and exit\n" "\n"
		"Report bugs to: " PACKAGE_BUGREPORT);
	  exit (EXIT_SUCCESS);
	  break;
	default:
	  try_help ();
	  break;
	}
    }

  if (in_file == NULL)
    {
      fprintf (stderr, "No input file\n");
      try_help ();
    }

  if (outfile == NULL)
    {
      fprintf (stderr, "No output file\n");
      try_help ();
    }

  // Open the input file.
  yyin = fopen (in_file, "r");
  if (yyin == NULL)
    {
      error (EXIT_FAILURE, errno, "Could not open '%s'", in_file);
    }

  if (yyparse () != 0)
    {
      // TODO:  Error reporting.
      error (EXIT_FAILURE, 0, "yyparse failed");
    }
  assert (root != NULL);


  construct_symbol_table (root, NULL);
  enter_symbols (root);
  process_declarations (root);
  process_definitions (root);
  check_composition (root);

  /* char filename[] = "XXXXXX.s"; */
  /* int fd = mkstemps (filename, 2); */
  /* if (fd == -1) */
  /*   { */
  /*     error (EXIT_FAILURE, errno, "could not create temporary file"); */
  /*   } */

  /* FILE *file = fdopen (fd, "w"); */
  /* if (file == NULL) */
  /*   { */
  /*     error (EXIT_FAILURE, errno, "could not open temporary file"); */
  /*   } */

  /* if (generate_code (file, root) != 0) */
  /*   { */
  /*     // TODO:  Error reporting. */
  /*     error (EXIT_FAILURE, 0, "generate_code failed"); */
  /*   } */

  /* compile (include, filename, outfile); */

  /* if (debug) */
  /*   { */
  /*     printf ("Intermediate code in %s\n", filename); */
  /*   } */
  /* else */
  /*   { */
  /*     unlink (filename); */
  /*   } */

  return 0;
}
