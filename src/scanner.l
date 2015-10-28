%option nounput
%option noyywrap
%option noinput

%{
#include "yyparse.hpp"
#include "parser.hpp"
#include "debug.hpp"
#include "type.hpp"
#include <error.h>

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
^#.*                   /* Discard line comments. */

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
"change"               { return CHANGE; }
"component"            { return COMPONENT; }
"const"                { return CONST; }
"copy"                 { return COPY; }
"else"                 { return ELSE; }
"enum"                 { return ENUM; }
"for"                  { return FOR; }
"foreign"              { return FOREIGN_KW; }
"func"                 { return FUNC; }
"getter"               { return GETTER; }
"heap"                 { return HEAP; }
"if"                   { return IF; }
"init"                 { return INIT; }
"instance"             { return INSTANCE; }
"merge"                { return MERGE; }
"println"              { return PRINTLN; }
"pull"                 { return PULL; }
"push"                 { return PUSH; }
"reaction"             { return REACTION; }
"return"               { return RETURN_KW; }
"struct"               { return STRUCT; }
"type"                 { return TYPE; }
"var"                  { return VAR; }
"while"                { return WHILE; }

"+="                   { return ADD_ASSIGN; }
"&&"                   { return LOGIC_AND_TOKEN; }
"||"                   { return LOGIC_OR_TOKEN; }
"++"                   { return INCREMENT; }
"--"                   { return DECREMENT; }
"=="                   { return EQUAL_TOKEN; }
"!="                   { return NOT_EQUAL_TOKEN; }
"<="                   { return LESS_EQUAL_TOKEN; }
">="                   { return MORE_EQUAL_TOKEN; }
"->"                   { return RIGHT_ARROW; }
"<-"                   { return LEFT_ARROW; }
"<<"                   { return LEFT_SHIFT_TOKEN; }
">>"                   { return RIGHT_SHIFT_TOKEN; }
"&^"                   { return AND_NOT_TOKEN; }
".."                   { return DOTDOT; }

"{"|"}"                { return *yytext; }
"["|"]"                { return *yytext; }
"("|")"                { return *yytext; }
"<"|">"                { return *yytext; }
"*"|"/"|"%"|"&"        { return *yytext; }
"+"|"-"|"|"|"^"        { return *yytext; }
"!"|";"|","|"="        { return *yytext; }
"."|":"                { return *yytext; }

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
                         const slice_type_t* type = new slice_type_t (uint8_type_t::instance ());
                         slice_type_t::ValueType v;
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
  const slice_type_t* type = new slice_type_t (uint8_type_t::instance ());
  slice_type_t::ValueType v;
  v.ptr = malloc (size);
  memcpy (v.ptr, begin, size);
  v.length = size;
  v.capacity = size;

  yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (type, v));
  return LITERAL;
}

-?(0|([1-9][0-9]*)|(0x[0-9a-fA-F]+))i(8|16|32|64|128)? {
  int base = 10;
  if (yyleng >= 2 && yytext[0] == '0' && yytext[1] == 'x')
    {
      base = 16;
    }

  const char* ptr = yytext + yyleng;
  for (; *ptr != 'i'; --ptr) ;;

  if (strcmp (ptr, "i") == 0)
    {
      yytext[yyleng - 1] = 0;
      yyleng -= 1;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (int_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else if (strcmp (ptr, "i8") == 0)
    {
      yytext[yyleng - 2] = 0;
      yyleng -= 2;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (int8_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else if (strcmp (ptr, "i16") == 0)
    {
      yytext[yyleng - 3] = 0;
      yyleng -= 3;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (int16_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else if (strcmp (ptr, "i32") == 0)
    {
      yytext[yyleng - 3] = 0;
      yyleng -= 3;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (int32_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else if (strcmp (ptr, "i64") == 0)
    {
      yytext[yyleng - 3] = 0;
      yyleng -= 3;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (int64_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else if (strcmp (ptr, "i128") == 0)
    {
      yytext[yyleng - 4] = 0;
      yyleng -= 4;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (int128_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else
    {
      not_reached;
    }

  return LITERAL;
}

(0|([1-9][0-9]*)|(0x[0-9a-fA-F]+))u(8|16|32|64|128)? {
  // TODO:  Fix format to match strtol.
  int base = 10;
  if (yyleng >= 2 && yytext[0] == '0' && yytext[1] == 'x')
    {
      base = 16;
    }

  const char* ptr = yytext + yyleng;
  for (; *ptr != 'u'; --ptr) ;;

  if (strcmp (ptr, "u") == 0)
    {
      yytext[yyleng - 1] = 0;
      yyleng -= 1;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (uint_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else if (strcmp (ptr, "u8") == 0)
    {
      yytext[yyleng - 2] = 0;
      yyleng -= 2;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (uint8_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else if (strcmp (ptr, "u16") == 0)
    {
      yytext[yyleng - 3] = 0;
      yyleng -= 3;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (uint16_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else if (strcmp (ptr, "u32") == 0)
    {
      yytext[yyleng - 3] = 0;
      yyleng -= 3;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (uint32_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else if (strcmp (ptr, "u64") == 0)
    {
      yytext[yyleng - 3] = 0;
      yyleng -= 3;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (uint64_type_t::instance (), strtol (yytext, NULL, base)));
    }
  else if (strcmp (ptr, "u128") == 0)
    {
      yytext[yyleng - 4] = 0;
      yyleng -= 4;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (uint128_type_t::instance (), strtoll (yytext, NULL, base)));
    }
  else
    {
      not_reached;
    }

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
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (float32_type_t::instance (), strtof (yytext, NULL)));
    }
  else if (strcmp (ptr, "f64") == 0)
    {
      yytext[yyleng - 3] = 0;
      yyleng -= 3;
      yylval.node = new ast_literal_expr_t (yylloc, typed_value_t (float64_type_t::instance (), strtod (yytext, NULL)));
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
                   "unrecognized character: %s", yytext);
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
                           "E59: newline in interpretted string literal");
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
                           "E60: illegal escape sequence '%c'", c);
          }
      }
      break;
    case Octal:
      {
        if (pos == limit) {
          error_at_line (-1, 0, Location::StaticFile.c_str (), line,
                         "E61: end of string while parsing octal escape sequence");
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
                           "E62: unexpected character in octal escape sequence '%c'", c);
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
                         "E63: end of string while parsing hex escape sequence");
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
                           "E64: unexpected character in hex escape sequence '%c'", c);
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
