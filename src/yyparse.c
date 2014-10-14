#include "yyparse.h"
#include <error.h>
#include "debug.h"

ast_t *root = NULL;

void
yyerror (char const *message)
{
  error_at_line (-1, 0, in_file, in_line, "%s", message);
}
