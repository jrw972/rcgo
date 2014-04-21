#include "yyparse.h"
#include <stdio.h>

node_t *root = NULL;

void
yyerror (char const *message)
{
  printf ("error %s\n", message);
}
