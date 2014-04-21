#include "compile.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <error.h>
#include <stdlib.h>
#include <errno.h>

void
compile (const char *infile, const char *outfile)
{
  char *command = NULL;
  int s = asprintf (&command, "gcc -g -c %s -o %s", infile, outfile);
  if (s == -1)
    {
      error (EXIT_FAILURE, errno, "could not allocate buffer");
    }

  int r = system (command);
  if (r != 0)
    {
      error (EXIT_FAILURE, errno, "could not execute %s", command);
    }
}
