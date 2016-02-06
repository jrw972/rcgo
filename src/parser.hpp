/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_SRC_PARSER_HPP_INCLUDED
# define YY_YY_SRC_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
enum yytokentype
{
  IDENTIFIER = 258,
  LITERAL = 259,
  ACTION = 260,
  ACTIVATE = 261,
  BIND = 262,
  BREAK = 263,
  CASE = 264,
  CHANGE = 265,
  COMPONENT = 266,
  CONST = 267,
  CONTINUE = 268,
  DEFAULT = 269,
  ELSE = 270,
  FALLTHROUGH = 271,
  FOR = 272,
  FOREIGN = 273,
  FUNC = 274,
  GETTER = 275,
  GOTO = 276,
  HEAP = 277,
  IF = 278,
  INIT = 279,
  INSTANCE = 280,
  INTERFACE = 281,
  MAP = 282,
  PULL = 283,
  PUSH = 284,
  RANGE = 285,
  REACTION = 286,
  RETURN = 287,
  STRUCT = 288,
  SWITCH = 289,
  TYPE = 290,
  VAR = 291,
  ADD_ASSIGN = 292,
  AND_ASSIGN = 293,
  AND_NOT = 294,
  AND_NOT_ASSIGN = 295,
  DECREMENT = 296,
  DIVIDE_ASSIGN = 297,
  DOTDOTDOT = 298,
  EQUAL = 299,
  INCREMENT = 300,
  LEFT_ARROW = 301,
  LEFT_SHIFT = 302,
  LEFT_SHIFT_ASSIGN = 303,
  LESS_EQUAL = 304,
  LOGIC_AND = 305,
  LOGIC_OR = 306,
  MODULUS_ASSIGN = 307,
  MORE_EQUAL = 308,
  MULTIPLY_ASSIGN = 309,
  NOT_EQUAL = 310,
  OR_ASSIGN = 311,
  RIGHT_ARROW = 312,
  RIGHT_SHIFT = 313,
  RIGHT_SHIFT_ASSIGN = 314,
  SHORT_ASSIGN = 315,
  SUBTRACT_ASSIGN = 316,
  XOR_ASSIGN = 317
};
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 10 "parser.y" /* yacc.c:1909  */
  ast::Node* node;
  ast::List* list;
  ast::Identifier* identifier;
  Mutability mutability;

#line 120 "parser.hpp" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_SRC_PARSER_HPP_INCLUDED  */
