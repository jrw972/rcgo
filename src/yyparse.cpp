#include "yyparse.hpp"

#include <error.h>

#include "parser.hpp"
#include "location.hpp"
#include "source_file.hpp"

void
yyerror (YYLTYPE* loc, yyscan_t scanner, source::SourceFile* source_file, char const * message)
{
  error_at_line (-1, 0, source_file->path ().c_str (), *loc, "%s (E112)", message);
}
