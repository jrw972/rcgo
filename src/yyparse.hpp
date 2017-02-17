#ifndef RC_SRC_YYPARSE_HPP
#define RC_SRC_YYPARSE_HPP

#include "types.hpp"

typedef void* yyscan_t;

#define YYLTYPE size_t

#define YY_DECL int yylex \
  (YYSTYPE * yylval_param, YYLTYPE * yylloc_param, yyscan_t yyscanner, const source::SourceFile* source_file)

#define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current)   = YYRHSLOC (Rhs, 1);        \
        }                                                               \
      else                                                              \
        {                                                               \
         (Current) =  YYRHSLOC (Rhs, 0);                                \
        }                                                               \
    while (0)

void yyerror (YYLTYPE* loc, yyscan_t scanner, source::SourceFile* source_file, char const * message);

#endif // RC_SRC_YYPARSE_HPP
