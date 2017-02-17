/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
    identifier = 258,
    LITERAL = 259,
    string_lit = 260,
    ACTION = 261,
    ACTIVATE = 262,
    BIND = 263,
    BREAK = 264,
    CASE = 265,
    CHANGE = 266,
    COMPONENT = 267,
    CONST = 268,
    CONTINUE = 269,
    DEFAULT = 270,
    ELSE = 271,
    FALLTHROUGH = 272,
    FOR = 273,
    FOREIGN = 274,
    FUNC = 275,
    GETTER = 276,
    GOTO = 277,
    HEAP = 278,
    IF = 279,
    import_kw = 280,
    INIT = 281,
    INSTANCE = 282,
    INTERFACE = 283,
    MAP = 284,
    package_kw = 285,
    PULL = 286,
    PUSH = 287,
    RANGE = 288,
    REACTION = 289,
    RETURN = 290,
    STRUCT = 291,
    SWITCH = 292,
    TYPE = 293,
    VAR = 294,
    ADD_ASSIGN = 295,
    AND_ASSIGN = 296,
    AND_NOT = 297,
    AND_NOT_ASSIGN = 298,
    DECREMENT = 299,
    DIVIDE_ASSIGN = 300,
    DOTDOTDOT = 301,
    EQUAL = 302,
    INCREMENT = 303,
    LEFT_ARROW = 304,
    LEFT_SHIFT = 305,
    LEFT_SHIFT_ASSIGN = 306,
    LESS_EQUAL = 307,
    LOGIC_AND = 308,
    LOGIC_OR = 309,
    MODULUS_ASSIGN = 310,
    MORE_EQUAL = 311,
    MULTIPLY_ASSIGN = 312,
    NOT_EQUAL = 313,
    OR_ASSIGN = 314,
    RIGHT_ARROW = 315,
    RIGHT_SHIFT = 316,
    RIGHT_SHIFT_ASSIGN = 317,
    SHORT_ASSIGN = 318,
    SUBTRACT_ASSIGN = 319,
    XOR_ASSIGN = 320
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 31 "../../src/parser.y" /* yacc.c:1909  */

  ast::Node* node;
  ast::List* list;
  ast::ParameterList* parameter_list;
  source::Identifier* identifier;
  ast::Receiver* receiver;
  ast::Literal* literal;
  Mutability mutability;
  ast::IdentifierList* identifier_list;
  ast::StatementList* statement_list;

#line 132 "../../src/parser.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
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



int yyparse (yyscan_t scanner, source::SourceFile* source_file);

#endif /* !YY_YY_SRC_PARSER_HPP_INCLUDED  */
