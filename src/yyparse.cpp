#include "yyparse.hpp"

#include <error.h>

#include "parser.hpp"
#include "location.hpp"

ast::Node *root = NULL;

void
yyerror (char const *message)
{
  error_at_line (-1, 0, util::Location::StaticFile.c_str (), yylloc, "%s (E112)", message);
}
