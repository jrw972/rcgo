#ifndef RC_SRC_DEBUG_HPP
#define RC_SRC_DEBUG_HPP

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <iostream>

#define UNIMPLEMENTED do { fprintf (stderr, "%s:%d: UNIMPLEMENTED\n", __FILE__, __LINE__); abort (); } while (0);

#define BUG(x) do { fprintf (stderr, "%s:%d: BUG %s\n", __FILE__, __LINE__, x); abort (); } while (0);

#define NOT_REACHED BUG("code designated unreachable has been reached")

#endif // RC_SRC_DEBUG_HPP
