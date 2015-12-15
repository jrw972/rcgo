#ifndef rc_src_debug_hpp
#define rc_src_debug_hpp

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <iostream>

#define unimplemented do { fprintf (stderr, "%s:%d: unimplemented\n", __FILE__, __LINE__); abort (); } while (0);

#define bug(x) do { fprintf (stderr, "%s:%d: BUG %s\n", __FILE__, __LINE__, x); abort (); } while (0);

#define not_reached bug("code designated unreachable has been reached")

#endif // rc_src_debug_hpp
