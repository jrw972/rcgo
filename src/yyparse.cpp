#include "yyparse.hpp"
#include "parser.hpp"
#include <error.h>
#include "debug.hpp"

ast_t *root = NULL;

void
yyerror (char const *message)
{
   error_at_line (-1, 0, Location::StaticFile.c_str (), yylloc, "%s", message);
}
