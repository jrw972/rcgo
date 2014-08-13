#ifndef debug_h
#define debug_h

#include <stdlib.h>
#include <assert.h>
#define _GNU_SOURCE
#include <stdio.h>

#define unimplemented do { fprintf (stderr, "%s:%d: unimplemented\n", __FILE__, __LINE__); abort (); } while (0);

#define bug(x) do { fprintf (stderr, "%s:%d: BUG %s\n", __FILE__, __LINE__, x); abort (); } while (0);

/* Flag indicating we are debugging. */
extern int debug;

extern const char* in_file;
extern unsigned int in_line;

#endif /* debug_h */
