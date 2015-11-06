%option nounput
%option noyywrap
%option noinput

%{
#include "yyparse.hpp"
#include "parser.hpp"
#include "debug.hpp"
#include "Type.hpp"
#include <error.h>

using namespace Type;

  static char* ParseInterprettedString (unsigned int line, char* pos, char* limit, char* out);
%}

%s IN_COMMENT

DECIMAL_DIGIT [0-9]
HEX_DIGIT [0-9a-fA-F]

DECIMAL1 {DECIMAL_DIGIT}+("."{DECIMAL_DIGIT}*)?
DECIMAL2 {DECIMAL_DIGIT}*"."{DECIMAL_DIGIT}+
DECIMAL_EXPONENT [Ee][+-]?{DECIMAL_DIGIT}+
DECIMAL_NUMBER ({DECIMAL1}|{DECIMAL2}){DECIMAL_EXPONENT}?

HEX1 {HEX_DIGIT}+("."{HEX_DIGIT}*)?
HEX2 {HEX_DIGIT}*"."{HEX_DIGIT}+
BINARY_EXPONENT [Pp][+-]?{DECIMAL_DIGIT}+
HEXADECIMAL_NUMBER (0x|0X)({HEX1}{BINARY_EXPONENT}|{HEX2}{BINARY_EXPONENT}?)

INFINITY ([Ii][Nn][Ff])|([Ii][Nn][Ff][Ii][Nn][Ii][Tt][Yy])
NAN [Nn][Aa][Nn]

FLOAT [+-]?({DECIMAL_NUMBER}|{HEXADECIMAL_NUMBER}|{INFINITY}|{NAN})

%%

\/\/.*                 /* Discard line comments. */
#.*                   /* Discard line comments. */

<INITIAL>{
  "/*"              BEGIN(IN_COMMENT);
}

<IN_COMMENT>{
  "*/"      BEGIN(INITIAL);
  [^*\n]+   // eat comment in chunks
  "*"       // eat the lone star
  \n        ++yylloc;
}

"action"               { return ACTION; }
"activate"             { return ACTIVATE; }
"bind"                 { return BIND; }
"break"                { return BREAK; }
"case"                 { return CASE; }
"change"               { return CHANGE; }
"component"            { return COMPONENT; }
"const"                { return CONST; }
"continue"             { return CONTINUE; }
"default"              { return DEFAULT; }
"else"                 { return ELSE; }
"enum"                 { return ENUM; }
"fallthrough"          { return FALLTHROUGH; }
"for"                  { return FOR; }
"foreign"              { return FOREIGN_KW; }
"func"                 { return FUNC; }
"getter"               { return GETTER; }
"goto"                 { return GOTO; }
"heap"                 { return HEAP; }
"if"                   { return IF; }
"init"                 { return INIT; }
"instance"             { return INSTANCE; }
"interface"            { return INTERFACE; }
"map"                  { return MAP; }
"pull"                 { return PULL; }
"push"                 { return PUSH; }
"range"                { return RANGE; }
"reaction"             { return REACTION; }
"return"               { return RETURN_KW; }
"struct"               { return STRUCT; }
"switch"               { return SWITCH; }
"type"                 { return TYPE; }
"var"                  { return VAR; }

"+"                    { return *yytext; }
"-"                    { return *yytext; }
"*"                    { return *yytext; }
"/"                    { return *yytext; }
"%"                    { return *yytext; }
"&"                    { return *yytext; }
"|"                    { return *yytext; }
"^"                    { return *yytext; }
"<<"                   { return LEFT_SHIFT; }
">>"                   { return RIGHT_SHIFT; }
"&^"                   { return AND_NOT; }
"+="                   { return ADD_ASSIGN; }
"-="                   { return SUBTRACT_ASSIGN; }
"*="                   { return MULTIPLY_ASSIGN; }
"/="                   { return DIVIDE_ASSIGN; }
"%="                   { return MODULUS_ASSIGN; }
"&="                   { return AND_ASSIGN; }
"|="                   { return OR_ASSIGN; }
"^="                   { return XOR_ASSIGN; }
"<<="                  { return LEFT_SHIFT_ASSIGN; }
">>="                  { return RIGHT_SHIFT_ASSIGN; }
"&^="                  { return AND_NOT_ASSIGN; }
"&&"                   { return LOGIC_AND; }
"||"                   { return LOGIC_OR; }
"<-"                   { return LEFT_ARROW; }
"->"                   { return RIGHT_ARROW; }
"++"                   { return INCREMENT; }
"--"                   { return DECREMENT; }
"=="                   { return EQUAL; }
"<"                    { return *yytext; }
">"                    { return *yytext; }
"="                    { return *yytext; }
"!"                    { return *yytext; }
"!="                   { return NOT_EQUAL; }
"<="                   { return LESS_EQUAL; }
">="                   { return MORE_EQUAL; }
":="                   { return SHORT_ASSIGN; }
"..."                  { return DOTDOTDOT; }
"("                    { return *yytext; }
"["                    { return *yytext; }
"{"                    { return *yytext; }
","                    { return *yytext; }
"."                    { return *yytext; }
")"                    { return *yytext; }
"]"                    { return *yytext; }
"}"                    { return *yytext; }
";"                    { return *yytext; }
":"                    { return *yytext; }

[_a-zA-Z][_a-zA-Z0-9]* { yylval.node = new ast_identifier_t (yylloc, yytext);
                         return IDENTIFIER; }

"`"[^`]*"`"            { char* begin = yytext + 1;
                         // Skip over the first and last `
                         char* pos = begin;
                         char* limit = yytext + yyleng - 1;
                         // Set the destination.
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

                         size_t size = destination - begin;
                         const Slice* type = Uint8::Instance ()->GetSlice ();
                         Slice::ValueType v;
                         v.ptr = malloc (size);
                         memcpy (v.ptr, begin, size);
                         v.length = size;
                         v.capacity = size;

                         yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (type, v));
                         return LITERAL; }

\"(\\.|[^\"])*\" {
  char* begin = yytext + 1;
  char* pos = begin;
  char* limit = yytext + yyleng - 1;
  char* end = ParseInterprettedString (yylloc, pos, limit, pos);
  size_t size = end - begin;
  const Slice* type = Uint8::Instance ()->GetSlice ();
  Slice::ValueType v;
  v.ptr = malloc (size);
  memcpy (v.ptr, begin, size);
  v.length = size;
  v.capacity = size;

  yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (type, v));
  return LITERAL;
}

[1-9][0-9]* {
  yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (Integer::Instance (), strtol (yytext, NULL, 10)));
  return LITERAL;
}

0[0-7]* {
  yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (Integer::Instance (), strtol (yytext, NULL, 8)));
  return LITERAL;
}

0[xX][0-9a-fA-F]+ {
  yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (Integer::Instance (), strtol (yytext, NULL, 16)));
  return LITERAL;
}

{FLOAT}f(32|64) {
  // TODO:  Fix format to match strtod.

  const char* ptr = yytext + yyleng;
  for (; *ptr != 'f'; --ptr) ;;

  if (strcmp (ptr, "f32") == 0)
    {
      yytext[yyleng - 2] = 0;
      yyleng -= 3;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (Float32::Instance (), strtof (yytext, NULL)));
    }
  else if (strcmp (ptr, "f64") == 0)
    {
      yytext[yyleng - 3] = 0;
      yyleng -= 3;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (Float64::Instance (), strtod (yytext, NULL)));
    }
  else
    {
      not_reached;
    }
  return LITERAL;
}

[ \t]+                 /* Discard whitespace */
[\n]                   { ++yylloc; }
. {
error_at_line (-1, 0, Location::StaticFile.c_str (), yylloc,
                   "unrecognized character '%s' (E66)", yytext);
}

%%

static char* ParseInterprettedString (unsigned int line, char* pos, char* limit, char* out)
{
  enum Mode {
    Normal,
    Escaped,
    Octal,
    Hex
  };

  Mode mode = Normal;
  int value;
  int remainingDigits;

  while (true) {
    switch (mode) {
    case Normal:
      {
        if (pos == limit) {
          return out;
        }
        char c = *pos;
        if (c == '\n')
          {
            error_at_line (-1, 0, Location::StaticFile.c_str (), line,
                           "newline in interpretted string literal (E67)");
          }
        else if (c == '\\')
          {
            ++pos;
            mode = Escaped;
          }
        else
          {
            *out++ = *pos++;
          }
      }
      break;
    case Escaped:
      {
        if (pos == limit) {
          bug ("No character after escape character in interpretted string");
        }

        char c = *pos++;
        switch (c)
          {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
            // Octal
            value = c - '0';
            remainingDigits = 3;
            mode = Octal;
            break;
          case 'x':
            // Hex
            value = 0;
            remainingDigits = 1;
            mode = Hex;
            break;
          case 'a':
            *out++ = '\a';
            mode = Normal;
            break;
          case 'b':
            *out++ = '\b';
            mode = Normal;
            break;
          case 'f':
            *out++ = '\f';
            mode = Normal;
            break;
          case 'n':
            *out++ = '\n';
            mode = Normal;
            break;
          case 'r':
            *out++ = '\r';
            mode = Normal;
            break;
          case 't':
            *out++ = '\t';
            mode = Normal;
            break;
          case 'v':
            *out++ = '\v';
            mode = Normal;
            break;
          case '\\':
            *out++ = '\\';
            mode = Normal;
            break;
          case '"':
            *out++ = '\"';
            mode = Normal;
            break;
          default:
            error_at_line (-1, 0, Location::StaticFile.c_str (), line,
                           "illegal escape sequence '%c' (E68)", c);
          }
      }
      break;
    case Octal:
      {
        if (pos == limit) {
          error_at_line (-1, 0, Location::StaticFile.c_str (), line,
                         "end of string while parsing octal escape sequence (E69)");
        }

        char c = *pos++;
        switch (c)
          {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
            value = (value << 3) | (c - '0');
            --remainingDigits;
            break;
          default:
            error_at_line (-1, 0, Location::StaticFile.c_str (), line,
                           "unexpected character in octal escape sequence '%c' (E70)", c);
          }

        if (remainingDigits == 0) {
          *out++ = value;
          mode = Normal;
        }
      }
      break;
    case Hex:
      {
        if (pos == limit) {
          error_at_line (-1, 0, Location::StaticFile.c_str (), line,
                         "end of string while parsing hex escape sequence (E71)");
        }

        char c = *pos;
        switch (c)
          {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            value = (value << 4) | (c - '0');
            --remainingDigits;
            break;
          case 'a':
          case 'b':
          case 'c':
          case 'd':
          case 'e':
          case 'f':
            value = (value << 4) | (c - 'a' + 10);
            --remainingDigits;
            break;
          case 'A':
          case 'B':
          case 'C':
          case 'D':
          case 'E':
          case 'F':
            value = (value << 4) | (c - 'A' + 10);
            --remainingDigits;
            break;
          default:
            error_at_line (-1, 0, Location::StaticFile.c_str (), line,
                           "unexpected character in hex escape sequence '%c' (E72)", c);
          }

        if (remainingDigits == 0) {
          *out++ = value;
          mode = Normal;
        }
      }
      break;
    }
  }

  not_reached;
}
