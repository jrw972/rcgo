%option nounput
%option noyywrap
%option noinput

%{
#include "yyparse.h"
#include "parser.h"
#include "strtab.h"
%}

%%

[_a-zA-Z][_a-zA-Z0-9]* { yylval.identifier = enter (yytext);
                         return IDENTIFIER; }

"!"                    { return *yytext; }

"&&"                   { return LOGIC_AND; }
"||"                   { return LOGIC_OR; }

[ \t\n]+               /* Discard whitespace */

.                      { printf ("Unrecognized character: %s\n", yytext); }
