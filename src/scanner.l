%option nounput
%option noyywrap
%option noinput

%{
#include "yyparse.hpp"
#include "parser.hpp"
#include "strtab.hpp"
#include "debug.hpp"
#include "type.hpp"
%}

%%

"action"               { return ACTION; }
"bind"                 { return BIND; }
"change"               { return CHANGE; }
"component"            { return COMPONENT; }
"const"                { return CONST; }
"else"                 { return ELSE; }
"foreign"              { return FOREIGN; }
"func"                 { return FUNC; }
"heap"                 { return HEAP; }
"if"                   { return IF; }
"instance"             { return INSTANCE; }
"merge"                { return MERGE; }
"move"                 { return MOVE; }
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
"--"                   { return DECREMENT; }
"=="                   { return EQUAL; }
"!="                   { return NOT_EQUAL; }
"->"                   { return ARROW; }

".."                   { return DOTDOT; }

"{"|"}"                { return *yytext; }
"["|"]"                { return *yytext; }
"("|")"                { return *yytext; }
"<"|">"                { return *yytext; }
"!"|";"|","|"="|"?"    { return *yytext; }
"$"|"@"|"."|"&"|"+"    { return *yytext; }

[_a-zA-Z][_a-zA-Z0-9]* { yylval.node = ast_make_identifier (yylloc, enter (yytext));
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

                         yylval.node = ast_make_typed_literal (yylloc, typed_value_make_string (string_type_t::instance (), rtstring_make (begin, destination - begin)));
                         return LITERAL; }

[0]u? {
  std::string x (yytext);
  if (x.size () == 1)
    {
      yylval.node = ast_make_typed_literal (yylloc, typed_value_make_int (int_type_t::instance (), 0));
    }
  else
    {
      yylval.node = ast_make_typed_literal (yylloc, typed_value_make_uint (uint_type_t::instance (), 0));
    }
  return LITERAL;
}

[1-9][0-9]*u? {
  std::string x (yytext);
  if (*x.rbegin () == 'u')
    {
      yytext[x.size () - 1] = 0;
      yylval.node = ast_make_typed_literal (yylloc, typed_value_make_uint (uint_type_t::instance (), atoi (yytext)));
    }
  else
    {
      yylval.node = ast_make_typed_literal (yylloc, typed_value_make_int (int_type_t::instance (), atoi (yytext)));
    }
  return LITERAL;
}

\/\/.*                 /* Discard line comments. */

[ \t]+                 /* Discard whitespace */
[\n]                   { ++yylloc; }
.                      { printf ("Unrecognized character: %s\n", yytext); }
