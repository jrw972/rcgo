%option nounput
%option noyywrap
%option noinput

%{
#include "yyparse.h"
#include "parser.h"
#include "strtab.h"
#include "debug.h"
#include "type.h"
%}

%%

"action"               { return ACTION; }
"bind"                 { return BIND; }
"component"            { return COMPONENT; }
"else"                 { return ELSE; }
"func"                 { return FUNC; }
"if"                   { return IF; }
"instance"             { return INSTANCE; }
"new"                  { return NEW; }
"port"                 { return PORT; }
"println"              { return PRINTLN; }
"reaction"             { return REACTION; }
"return"               { return RETURN; }
"struct"               { return STRUCT; }
"trigger"              { return TRIGGER; }
"type"                 { return TYPE; }
"var"                  { return VAR; }

"&&"                   { return LOGIC_AND; }
"||"                   { return LOGIC_OR; }
"++"                   { return INCREMENT; }
"=="                   { return EQUAL; }
"!="                   { return NOT_EQUAL; }
"->"                   { return ARROW; }

"{"|"}"                { return *yytext; }
"("|")"                { return *yytext; }
"<"|">"                { return *yytext; }
"!"|";"|","|"="        { return *yytext; }
"$"|"@"|"."|"&"        { return *yytext; }

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

                         yylval.literal = typed_value_make_string (type_make_untyped_string (), rtstring_make (begin, destination - begin));
                         return LITERAL; }

[0]                    { yylval.literal = typed_value_make_integer (0);
                         return LITERAL; }
[1-9][0-9]*            { yylval.literal = typed_value_make_integer (atoi (yytext));
                         return LITERAL; }

\/\/.*                 /* Discard line comments. */

[ \t]+                 /* Discard whitespace */
[\n]                   { ++in_line; }
.                      { printf ("Unrecognized character: %s\n", yytext); }
