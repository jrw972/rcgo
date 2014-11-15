%option nounput
%option noyywrap
%option noinput

%{
#include "yyparse.h"
#include "parser.h"
#include "strtab.h"
#include "debug.h"
%}

%%

"action"               { return ACTION; }
"bind"                 { return BIND; }
"component"            { return COMPONENT; }
"instance"             { return INSTANCE; }
"port"                 { return PORT; }
"println"              { return PRINTLN; }
"reaction"             { return REACTION; }
"trigger"              { return TRIGGER; }
"type"                 { return TYPE; }
"var"                  { return VAR; }

[_a-zA-Z][_a-zA-Z0-9]* { yylval.identifier = enter (yytext);
                         return IDENTIFIER; }

"!"|";"|","|"="        { return *yytext; }
"$"|"."                { return *yytext; }
"{"|"}"                { return *yytext; }
"("|")"                { return *yytext; }

"&&"                   { return LOGIC_AND; }
"||"                   { return LOGIC_OR; }
"->"                   { return ARROW; }

\/\/.*                 /* Discard line comments. */

[ \t]+                 /* Discard whitespace */
[\n]                   { ++in_line; }
.                      { printf ("Unrecognized character: %s\n", yytext); }
