#ifndef debug_h
#define debug_h

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>

#define unimplemented do { fprintf (stderr, "%s:%d: unimplemented\n", __FILE__, __LINE__); abort (); } while (0);

#define bug(x) do { fprintf (stderr, "%s:%d: BUG %s\n", __FILE__, __LINE__, x); abort (); } while (0);

#define not_reached bug("code designated unreachable has been reached")

/* Flag indicating we are debugging. */
extern int debug;

extern const char *in_file;

#endif /* debug_h */
