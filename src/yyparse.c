#include "yyparse.h"
#include <error.h>
#include "debug.h"

node_t *root = NULL;

void
yyerror (char const *message)
{
  error_at_line (-1, 0, in_file, in_line, "%s", message);
}
