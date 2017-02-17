%option reentrant
%option bison-bridge
%option bison-locations
%option nounput
%option noyywrap
%option noinput

%{
#include "yyparse.hpp"
#include "parser.hpp"
#include "debug.hpp"
#include "type.hpp"
#include "location.hpp"
#include "node.hpp"
#include "identifier.hpp"
#include "source_file.hpp"
#include <error.h>

  using namespace type;
  using namespace ast;
  using namespace semantic;
  using namespace source;

  static int octalvalue (char c) {
    return c - '0';
  }

  static int hexvalue (char c) {
    if (c >= '0' && c <= '9') {
      return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
      return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
      return c - 'A' + 10;
    }
    NOT_REACHED;
  }

  static void validUnicode (const source::Location& location, int u) {
    if (u > 0x10FFFF) {
      error_at_line (-1, 0, location.file ()->path ().c_str (), location.line (),
                     "Unicode value is out of range (E135)");
    }

    if (u >= 0xD800 && u <= 0xDFFF) {
      error_at_line (-1, 0, location.file ()->path ().c_str (), location.line (),
                     "illegal surrogate pair (E136)");
    }
  }

  static int parseOctal (const source::Location& location, const char* s) {
    int x =
      (octalvalue (s[0]) << 6) |
      (octalvalue (s[1]) << 3) |
      (octalvalue (s[2]) << 0);
    if (x > 255) {
      error_at_line (-1, 0, location.file ()->path ().c_str (), location.line (),
                     "octal escape is out of range (E134)");
    }
    return x;
  }

  static int parseHex (const char* s) {
    return
      (hexvalue (s[0]) << 4) |
      (hexvalue (s[1]) << 0);
  }

  static int parseU4 (const source::Location& location, const char* s) {
    int x =
      (hexvalue (s[0]) << 12) |
      (hexvalue (s[1]) << 8) |
      (hexvalue (s[2]) << 4) |
      (hexvalue (s[3]) << 0);
    validUnicode (location, x);
    return x;
  }

  static int parseU8 (const source::Location& location, const char* s) {
    int x =
      (hexvalue (s[0]) << 28) |
      (hexvalue (s[1]) << 24) |
      (hexvalue (s[2]) << 20) |
      (hexvalue (s[3]) << 16) |
      (hexvalue (s[4]) << 12) |
      (hexvalue (s[5]) << 8) |
      (hexvalue (s[6]) << 4) |
      (hexvalue (s[7]) << 0);
    validUnicode (location, x);
    return x;
  }

 static char* parseInterprettedString (const source::Location& location, char* pos, char* limit, char* out);

 static char* writeUtf8 (int x, char* out) {
   if (x < 128) {
     *out++ = x;
   } else {
     char u[6];
     size_t idx = 0;
     while (x != 0) {
       u[idx++] = 0x80 | (x & 0x3F);
       x >>= 6;
     }
     switch (idx) {
     case 2:
       u[1] |= 0xC0;
       break;
     case 3:
       u[2] |= 0xE0;
       break;
     case 4:
       u[3] |= 0xF0;
       break;
     case 5:
       u[4] |= 0xF8;
       break;
     case 6:
       u[5] |= 0xFC;
       break;
     }
     while (idx != 0) {
       *out++ = u[idx-- - 1];
     }
   }
   return out;
 }

%}

%s IN_COMMENT

OCTAL_DIGIT [0-7]
HEXADECIMAL_DIGIT [0-9a-fA-F]

DECIMALS [0-9]+
EXPONENT [eE][+-]?{DECIMALS}

FLOAT {DECIMALS}"."{DECIMALS}?{EXPONENT}?|{DECIMALS}{EXPONENT}|"."{DECIMALS}{EXPONENT}?

OCTAL   \\{OCTAL_DIGIT}{3}
HEX     \\x{HEXADECIMAL_DIGIT}{2}
LITTLEU \\u{HEXADECIMAL_DIGIT}{4}
BIGU    \\U{HEXADECIMAL_DIGIT}{8}
ESCAPED \\[abfnrtv\\\"]

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
"fallthrough"          { return FALLTHROUGH; }
"for"                  { return FOR; }
"foreign"              { return FOREIGN; }
"func"                 { return FUNC; }
"getter"               { return GETTER; }
"goto"                 { return GOTO; }
"heap"                 { return HEAP; }
"if"                   { return IF; }
"import"               { return import_kw; }
"init"                 { return INIT; }
"instance"             { return INSTANCE; }
"interface"            { return INTERFACE; }
"map"                  { return MAP; }
"package"              { return package_kw; }
"pull"                 { return PULL; }
"push"                 { return PUSH; }
"range"                { return RANGE; }
"reaction"             { return REACTION; }
"return"               { return RETURN; }
"struct"               { return STRUCT; }
"switch"               { return SWITCH; }
"type"                 { return TYPE; }
"var"                  { return VAR; }

"$"                    { return *yytext; }
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

[_a-zA-Z][_a-zA-Z0-9]* { yylval->identifier = new source::Identifier (yytext, source_file->location (*yylloc));
                         return identifier; }

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
                         runtime::String v;
                         void* o = malloc (size);
                         memcpy (o, begin, size);
                         v.ptr = o;
                         v.length = size;

                         ExpressionValue value;
                         value.kind = ExpressionValue::Constant;
                         value.type = UntypedString::instance ();
                         value.value.untyped_string_value = v;
                         yylval->literal = new Literal (source_file->location (*yylloc), value);
                         return string_lit; }

\"([^\"]|{OCTAL}|{HEX}|{LITTLEU}|{BIGU}|{ESCAPED})*\" {
  char* begin = yytext + 1;
  char* pos = begin;
  char* limit = yytext + yyleng - 1;
  char* end = parseInterprettedString (source_file->location (*yylloc), pos, limit, pos);
  size_t size = end - begin;
  runtime::String v;
  void* o = malloc (size);
  memcpy (o, begin, size);
  v.ptr = o;
  v.length = size;

  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedString::instance ();
  value.value.untyped_string_value = v;
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return string_lit;
}

[1-9][0-9]* {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedInteger::instance ();
  errno = 0;
  value.value.untyped_integer_value = strtoull (yytext, NULL, 10);
  if (errno == ERANGE) {
    error_at_line (-1, 0, source_file->path ().c_str (), *yylloc,
      "integer literal '%s' is out of range (E11)", yytext);
  }
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return LITERAL;
}

0{OCTAL_DIGIT}* {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedInteger::instance ();
  errno = 0;
  value.value.untyped_integer_value = strtoull (yytext, NULL, 8);
  if (errno == ERANGE) {
    error_at_line (-1, 0, source_file->path ().c_str (), *yylloc,
      "octal literal '%s' is out of range (E12)", yytext);
  }
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return LITERAL;
}

0[xX]{HEXADECIMAL_DIGIT}+ {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedInteger::instance ();
  errno = 0;
  value.value.untyped_integer_value = strtoull (yytext, NULL, 16);
  if (errno == ERANGE) {
    error_at_line (-1, 0, source_file->path ().c_str (), *yylloc,
      "hex literal '%s' is out of range (E13)", yytext);
  }
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return LITERAL;
}

{FLOAT} {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedFloat::instance ();
  value.value.untyped_float_value = strtod (yytext, NULL);
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return LITERAL;
}

({DECIMALS}|{FLOAT})i {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = type::UntypedComplex::instance ();
  value.value.untyped_complex_value = semantic::UntypedComplex::make (0, strtod (yytext, NULL));
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return LITERAL;
}

'[^'\n]' {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedRune::instance ();
  value.value.untyped_rune_value = yytext[1];
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return LITERAL;
}

'\\[abfnrtv\\']' {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedRune::instance ();
  switch (yytext[2]) {
  case 'a':
    value.value.untyped_rune_value = '\a';
    yylval->node = new Literal (source_file->location (*yylloc), value);
    break;
  case 'b':
    value.value.untyped_rune_value = '\b';
    yylval->node = new Literal (source_file->location (*yylloc), value);
    break;
  case 'f':
    value.value.untyped_rune_value = '\f';
    yylval->node = new Literal (source_file->location (*yylloc), value);
    break;
  case 'n':
    value.value.untyped_rune_value = '\n';
    yylval->node = new Literal (source_file->location (*yylloc), value);
    break;
  case 'r':
    value.value.untyped_rune_value = '\r';
    yylval->node = new Literal (source_file->location (*yylloc), value);
    break;
  case 't':
    value.value.untyped_rune_value = '\t';
    yylval->node = new Literal (source_file->location (*yylloc), value);
    break;
  case 'v':
    value.value.untyped_rune_value = '\v';
    yylval->node = new Literal (source_file->location (*yylloc), value);
    break;
  case '\\':
    value.value.untyped_rune_value = '\\';
    yylval->node = new Literal (source_file->location (*yylloc), value);
    break;
  case '\'':
    value.value.untyped_rune_value = '\'';
    yylval->node = new Literal (source_file->location (*yylloc), value);
    break;
  default:
    NOT_REACHED;
  }

  return LITERAL;
}

'\\{OCTAL_DIGIT}{3}' {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedRune::instance ();
  value.value.untyped_rune_value = parseOctal (source_file->location (*yylloc), yytext + 2);
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return LITERAL;
}

'\\x{HEXADECIMAL_DIGIT}{2}' {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedRune::instance ();
  value.value.untyped_rune_value = parseHex (yytext + 3);
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return LITERAL;
}

'\\u{HEXADECIMAL_DIGIT}{4}' {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedRune::instance ();
  value.value.untyped_rune_value = parseU4 (source_file->location (*yylloc), yytext + 3);
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return LITERAL;
}

'\\U{HEXADECIMAL_DIGIT}{8}' {
  ExpressionValue value;
  value.kind = ExpressionValue::Constant;
  value.type = UntypedRune::instance ();
  value.value.untyped_rune_value = parseU8 (source_file->location (*yylloc), yytext + 3);
  yylval->node = new Literal (source_file->location (*yylloc), value);
  return LITERAL;
}

[ \t]+                 /* Discard whitespace */
[\n]                   { ++*yylloc; }
. {
error_at_line (-1, 0, source_file->path ().c_str (), *yylloc,
                   "unrecognized character '%s' (E66)", yytext);
}

%%

static char* parseInterprettedString (const source::Location& location, char* pos, char* limit, char* out)
{
  while (pos != limit) {
    char c = *pos;
    if (c != '\\') {
      *out++ = *pos++;
    } else {
      ++pos;
      switch (*pos) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
        {
          int x = parseOctal (location, pos);
          pos += 3;
          *out++ = x;
        }
        break;
      case 'x':
        {
          ++pos;
          int x = parseHex (pos);
          pos += 2;
          *out++ = x;
        }
        break;
      case 'u':
        {
          ++pos;
          int x = parseU4 (location, pos);
          pos += 4;
          out = writeUtf8 (x, out);
        }
        break;
      case 'U':
        {
          ++pos;
          int x = parseU8 (location, pos);
          pos += 8;
          out = writeUtf8 (x, out);
        }
        break;
      case 'a':
        *out++ = '\a';
        ++pos;
        break;
      case 'b':
        *out++ = '\b';
        ++pos;
        break;
      case 'f':
        *out++ = '\f';
        ++pos;
        break;
      case 'n':
        *out++ = '\n';
        ++pos;
        break;
      case 'r':
        *out++ = '\r';
        ++pos;
        break;
      case 't':
        *out++ = '\t';
        ++pos;
        break;
      case 'v':
        *out++ = '\v';
        ++pos;
        break;
      case '\\':
        *out++ = '\\';
        ++pos;
        break;
      case '"':
        *out++ = '"';
        ++pos;
        break;
      }
    }
  }

  return out;
}
