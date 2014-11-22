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
"getter"               { return GETTER; }
"instance"             { return INSTANCE; }
"port"                 { return PORT; }
"println"              { return PRINTLN; }
"reaction"             { return REACTION; }
"return"               { return RETURN; }
"trigger"              { return TRIGGER; }
"type"                 { return TYPE; }
"var"                  { return VAR; }

[_a-zA-Z][_a-zA-Z0-9]* { yylval.identifier = enter (yytext);
                         return IDENTIFIER; }

"`"[^`]*"`"            { char* begin = yytext + 1;
                         // Skip over the first and last `
                         char* pos = begin;
                         char* limit = yytext + yyleng - 1;
                         // Set the desintation.
                         char* destination = pos;
                         // Replace carriage returns.
                         while (pos != limit)
                           {
                             char c = *pos;
                             if (c != '\r')
                               {
                                 *destination++ = c;
                               }
                             ++pos;
                           }

                         yylval.literal = untyped_value_make_string (rtstring_make (begin, destination - begin));
                         return LITERAL; }

"!"|";"|","|"="        { return *yytext; }
"$"|"."                { return *yytext; }
"{"|"}"                { return *yytext; }
"("|")"                { return *yytext; }

"&&"                   { return LOGIC_AND; }
"||"                   { return LOGIC_OR; }
"++"                   { return INCREMENT; }
"->"                   { return ARROW; }

\/\/.*                 /* Discard line comments. */

[ \t]+                 /* Discard whitespace */
[\n]                   { ++in_line; }
.                      { printf ("Unrecognized character: %s\n", yytext); }
