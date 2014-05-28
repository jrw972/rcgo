%option nounput
%option noyywrap
%option noinput

%{
#include "yyparse.h"
#include "parser.h"
#include "strtab.h"
%}

%%

"action"               { return ACTION; }
"component"            { return COMPONENT; }
"instance"             { return INSTANCE; }
"print"                { return PRINT; }
"trigger"              { return TRIGGER; }
"type"                 { return TYPE; }
"var"                  { return VAR; }

[_a-zA-Z][_a-zA-Z0-9]* { yylval.identifier = enter (yytext);
                         return IDENTIFIER; }

"!"|";"|","|"="|"$"    { return *yytext; }
"{"|"}"                { return *yytext; }
"("|")"                { return *yytext; }

"->"                   { return ARROW; }
"&&"                   { return LOGIC_AND; }
"||"                   { return LOGIC_OR; }

\/\/.*                   /* Discard line comments. */

[ \t\n]+               /* Discard whitespace */

.                      { printf ("Unrecognized character: %s\n", yytext); }
