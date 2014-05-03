#ifndef debug_h
#define debug_h

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define unimplemented do { fprintf (stderr, "%s:%d: unimplemented\n", __FILE__, __LINE__); abort (); } while (0);

#define bug(x) do { fprintf (stderr, "%s:%d: BUG %s\n", __FILE__, __LINE__, x); abort (); } while (0);

#endif /* debug_h */
