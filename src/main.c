#define _GNU_SOURCE
#include <stdlib.h>
#include <assert.h>
#include <error.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>

#include "config.h"
#include "scanner.h"
#include "yyparse.h"
#include "semantics.h"
#include "codegen.h"
#include "compile.h"

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
  const char *infile = NULL;
  const char *outfile = NULL;

  while (true)
    {
      static struct option long_options[] = {
	{"version", no_argument, 0, 'v'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
      };

      int c = getopt_long (argc, argv, "c:o:h", long_options, NULL);

      if (c == -1)
	break;

      switch (c)
	{
	case 'c':
	  infile = optarg;
	  break;
	case 'o':
	  outfile = optarg;
	  break;
	case 'v':
	  print_version ();
	  exit (EXIT_SUCCESS);
	  break;
	case 'h':
	  printf ("Usage: %s OPTION... -c FILE -o FILE\n",
		  program_invocation_short_name);
	  puts ("Compile " PACKAGE_NAME " source code.\n" "\n"
		"  -c FILE     specifies the input file\n"
		"  -o FILE     specifies the output file\n"
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

  if (infile == NULL)
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
  yyin = fopen (infile, "r");
  if (yyin == NULL)
    {
      error (EXIT_FAILURE, errno, "Could not open '%s'", infile);
    }

  if (yyparse () != 0)
    {
      // TODO:  Error reporting.
      error (EXIT_FAILURE, 0, "yyparse failed");
    }
  assert (root != NULL);

  /* Add a node that forces the result to a typed value. */
  /* This will be removed as this translator develops. */

  node_t *n = node_make_to_typed_value (root);
  root = n;

  if (check_semantics (root) != 0)
    {
      // TODO:  Error reporting.
      error (EXIT_FAILURE, 0, "check_semantics failed");
    }

  char filename[] = "XXXXXX.c";
  int fd = mkstemps (filename, 2);
  if (fd == -1)
    {
      error (EXIT_FAILURE, errno, "could not create temporary file");
    }

  FILE *file = fdopen (fd, "w");
  if (file == NULL)
    {
      error (EXIT_FAILURE, errno, "could open temporary file");
    }

  if (generate_code (root, file) != 0)
    {
      // TODO:  Error reporting.
      error (EXIT_FAILURE, 0, "generate_code failed");
    }

  compile (filename, outfile);
  unlink (filename);

  return 0;
}
