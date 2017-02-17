/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 5 "../../src/parser.y" /* yacc.c:339  */

#include "yyparse.hpp"
#include "debug.hpp"
#include "node.hpp"
#include "source_file.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include "polymorphic_function.hpp"

  int yylex (YYSTYPE * yylval_param, YYLTYPE * yylloc, yyscan_t yyscanner, const source::SourceFile* source_file);

 source::Identifier idref (source::Identifier* id)
 {
   source::Identifier retval (*id);
   delete id;
   return retval;
 }

using namespace ast;

#line 87 "../../src/parser.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parser.hpp".  */
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
#line 31 "../../src/parser.y" /* yacc.c:355  */

  ast::Node* node;
  ast::List* list;
  ast::ParameterList* parameter_list;
  source::Identifier* identifier;
  ast::Receiver* receiver;
  ast::Literal* literal;
  Mutability mutability;
  ast::IdentifierList* identifier_list;
  ast::StatementList* statement_list;

#line 205 "../../src/parser.cpp" /* yacc.c:355  */
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

/* Copy the second part of user declarations.  */

#line 235 "../../src/parser.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   714

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  89
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  89
/* YYNRULES -- Number of rules.  */
#define YYNRULES  210
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  406

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   320

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    87,     2,     2,    72,    85,    86,     2,
      68,    69,    73,    80,    75,    81,    70,    84,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    88,    67,
      78,    71,    79,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    76,     2,    77,    83,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    66,    82,    74,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   199,   199,   202,   204,   206,   209,   210,   213,   214,
     218,   219,   221,   223,   226,   227,   228,   230,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   248,   250,
     254,   257,   260,   261,   262,   265,   266,   267,   270,   272,
     276,   278,   282,   284,   288,   292,   296,   300,   304,   308,
     310,   314,   316,   320,   325,   326,   328,   330,   333,   334,
     337,   339,   341,   344,   346,   349,   350,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   362,   363,   364,   367,
     368,   369,   370,   372,   375,   379,   383,   387,   391,   393,
     398,   399,   403,   405,   409,   411,   414,   417,   418,   421,
     422,   425,   427,   431,   433,   435,   437,   441,   445,   447,
     449,   451,   455,   457,   461,   463,   465,   467,   469,   471,
     475,   479,   481,   486,   487,   488,   489,   490,   491,   492,
     493,   496,   498,   500,   502,   504,   506,   509,   512,   515,
     518,   520,   523,   524,   526,   529,   530,   531,   533,   537,
     538,   543,   544,   547,   550,   552,   556,   558,   562,   564,
     566,   568,   570,   572,   574,   578,   580,   582,   584,   586,
     590,   592,   594,   596,   598,   600,   602,   604,   608,   609,
     610,   611,   612,   613,   614,   617,   619,   621,   623,   626,
     628,   630,   632,   634,   636,   638,   640,   644,   645,   648,
     649,   652,   653,   656,   657,   660,   661,   664,   665,   667,
     669
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "identifier", "LITERAL", "string_lit",
  "ACTION", "ACTIVATE", "BIND", "BREAK", "CASE", "CHANGE", "COMPONENT",
  "CONST", "CONTINUE", "DEFAULT", "ELSE", "FALLTHROUGH", "FOR", "FOREIGN",
  "FUNC", "GETTER", "GOTO", "HEAP", "IF", "import_kw", "INIT", "INSTANCE",
  "INTERFACE", "MAP", "package_kw", "PULL", "PUSH", "RANGE", "REACTION",
  "RETURN", "STRUCT", "SWITCH", "TYPE", "VAR", "ADD_ASSIGN", "AND_ASSIGN",
  "AND_NOT", "AND_NOT_ASSIGN", "DECREMENT", "DIVIDE_ASSIGN", "DOTDOTDOT",
  "EQUAL", "INCREMENT", "LEFT_ARROW", "LEFT_SHIFT", "LEFT_SHIFT_ASSIGN",
  "LESS_EQUAL", "LOGIC_AND", "LOGIC_OR", "MODULUS_ASSIGN", "MORE_EQUAL",
  "MULTIPLY_ASSIGN", "NOT_EQUAL", "OR_ASSIGN", "RIGHT_ARROW",
  "RIGHT_SHIFT", "RIGHT_SHIFT_ASSIGN", "SHORT_ASSIGN", "SUBTRACT_ASSIGN",
  "XOR_ASSIGN", "'{'", "';'", "'('", "')'", "'.'", "'='", "'$'", "'*'",
  "'}'", "','", "'['", "']'", "'<'", "'>'", "'+'", "'-'", "'|'", "'^'",
  "'/'", "'%'", "'&'", "'!'", "':'", "$accept", "Top", "SourceFile",
  "ImportDeclList", "TopLevelDeclList", "PackageClause", "PackageName",
  "ImportDecl", "ImportSpecList", "ImportSpec", "ImportPath",
  "TopLevelDecl", "ConstDecl", "InstanceDecl", "TypeDecl", "Mutability",
  "DereferenceMutability", "Receiver", "ActionDecl", "ReactionDecl",
  "BinderDecl", "InitDecl", "GetterDecl", "MethodDecl", "FunctionDecl",
  "ParameterList", "ParameterListInternal", "Parameter",
  "ReturnParameterList", "optional_semicolon", "BindStatement", "Body",
  "Block", "StatementList", "Statement", "SimpleStatement",
  "EmptyStatement", "Activate", "Change", "ForIota", "Return",
  "IncrementStatement", "OptionalPushPortCallList", "PushPortCallList",
  "PushPortCall", "Indexession", "OptionalExpressionList",
  "OptionalTypeOrExpressionList", "ExpressionList", "TypeOrExpressionList",
  "ExpressionStatement", "VarDecl", "AssignmentStatement", "If", "While",
  "IdentifierList", "TypeLitExpression", "Array", "ElementType",
  "StructType", "Slice", "Map", "KeyType", "ComponentType", "PushPort",
  "PullPort", "Heap", "Pointer", "TypeLit", "TypeName", "Type",
  "ArrayDimension", "FieldList", "OptionalExpression", "Expression",
  "OrExpression", "AndExpression", "CompareExpression", "AddExpression",
  "MultiplyExpression", "UnaryExpression", "PrimaryExpression", "Literal",
  "LiteralValue", "ElementList", "Element", "Key", "Value",
  "OptionalComma", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   123,    59,    40,    41,
      46,    61,    36,    42,   125,    44,    91,    93,    60,    62,
      43,    45,   124,    94,    47,    37,    38,    33,    58
};
# endif

#define YYPACT_NINF -347

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-347)))

#define YYTABLE_NINF -207

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      15,    16,    25,  -347,   -26,  -347,  -347,  -347,  -347,    27,
      44,   102,   -13,  -347,  -347,    54,    54,  -347,  -347,     7,
       7,    91,    20,     7,     7,   113,     7,   122,   520,    60,
    -347,  -347,  -347,  -347,  -347,  -347,  -347,  -347,  -347,  -347,
      12,  -347,   101,  -347,  -347,   127,   128,   129,  -347,   235,
      66,   134,   136,   138,   153,   155,   638,  -347,  -347,  -347,
      93,   638,    86,    66,    66,    97,   520,   520,   364,   520,
     520,   520,   520,   520,   -11,  -347,  -347,  -347,  -347,  -347,
    -347,  -347,  -347,   638,    88,   114,   116,   218,   205,   149,
    -347,    23,  -347,  -347,     7,     7,  -347,   109,    37,   104,
     111,  -347,   638,   520,   638,   176,   385,  -347,  -347,  -347,
    -347,   112,    10,   563,    66,    66,    66,   177,    66,  -347,
     423,  -347,  -347,  -347,   638,   563,  -347,  -347,   117,  -347,
     110,   638,  -347,  -347,  -347,  -347,  -347,   520,  -347,  -347,
    -347,  -347,   520,   520,   520,   520,   520,   520,   520,   520,
     520,   520,   520,   520,   520,   520,   520,   520,   520,   520,
     520,   520,   185,   520,   187,   191,  -347,  -347,  -347,   123,
     520,  -347,  -347,  -347,   135,   132,  -347,  -347,  -347,   520,
    -347,   130,  -347,    14,    94,   111,  -347,   563,   563,   563,
     140,   111,  -347,   121,   139,   141,  -347,   158,  -347,     8,
     147,  -347,  -347,    11,  -347,   638,  -347,   183,   116,   218,
     205,   205,   205,   205,   205,   205,   149,   149,   149,   149,
    -347,  -347,  -347,  -347,  -347,  -347,  -347,   186,   179,   -11,
    -347,  -347,   168,   182,   200,    66,   105,    13,   204,   318,
    -347,   520,   132,    91,   208,   123,   638,   638,  -347,   111,
     111,   111,   520,  -347,   482,   124,   482,  -347,   579,   638,
    -347,   209,  -347,  -347,   520,   520,  -347,   520,   111,  -347,
    -347,   214,   279,   111,   206,   286,   222,   541,   520,   520,
      91,  -347,  -347,  -347,  -347,  -347,   225,  -347,  -347,  -347,
    -347,  -347,  -347,  -347,  -347,  -347,  -347,  -347,  -347,    14,
     152,  -347,  -347,  -347,   638,  -347,  -347,  -347,  -347,  -347,
     224,   132,  -347,  -347,  -347,  -347,  -347,   227,  -347,  -347,
     -11,  -347,   221,   207,   231,  -347,  -347,   232,  -347,   -33,
     111,   229,  -347,   520,   256,   111,   240,   213,  -347,    14,
    -347,   123,   520,  -347,  -347,   520,   520,   520,  -347,  -347,
    -347,  -347,   520,   111,  -347,   520,   520,   241,  -347,   286,
     237,   520,  -347,   520,   297,   123,   252,  -347,  -347,   270,
    -347,   243,  -347,   257,   250,   520,  -347,   329,   111,   111,
     -15,   617,   520,   520,  -347,  -347,  -347,   265,   269,  -347,
     323,  -347,  -347,   520,   272,   132,  -347,  -347,   111,   -15,
     132,   520,  -347,  -347,  -347,   132
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     2,     0,     9,     8,     1,     4,     6,
       0,     3,     0,    17,    12,     0,     0,    10,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      18,    27,    19,    24,    25,    26,    22,    23,    21,    20,
       0,     5,     0,    15,    16,     0,     0,     0,   121,     0,
       0,     0,     0,     0,     0,     0,     0,   189,   197,   198,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   123,   124,   125,   126,   127,
     128,   129,   130,     0,     0,   153,   154,   156,   158,   165,
     170,   178,   185,     7,     0,     0,    11,     0,    32,     0,
       0,   144,     0,     0,     0,     0,     0,   143,   142,   146,
     145,     0,     0,    54,     0,     0,     0,     0,     0,    31,
       0,   187,   149,   140,     0,    54,   138,   149,     0,   183,
       0,     0,   179,   180,   182,   184,   181,     0,   186,   131,
     132,   148,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    99,     0,   151,     0,     0,    13,    33,    34,    35,
       0,    65,    44,    63,     0,    29,   101,   141,   122,     0,
      49,    58,    51,    32,     0,     0,    55,    54,    54,    54,
       0,     0,   199,   207,   208,   209,   201,     0,   204,     0,
       0,   136,   139,     0,   190,     0,   134,     0,   155,   157,
     159,   162,   164,   160,   161,   163,   166,   167,   168,   169,
     177,   174,   175,   171,   172,   173,   176,     0,   100,   104,
     103,   191,     0,   152,     0,     0,     0,     0,     0,    83,
     147,     0,    28,    59,     0,    35,     0,     0,    48,     0,
       0,     0,    97,    42,   210,     0,     0,   137,     0,     0,
     133,     0,   196,   195,     0,   151,   192,     0,     0,    36,
      37,     0,     0,     0,   189,    90,     0,     0,    83,     0,
       0,    64,    68,    69,    78,    72,     0,    67,    79,    71,
      76,    77,    73,    81,    80,    70,    82,    74,    75,    32,
     107,   102,    52,    50,     0,    56,    57,    47,    46,    45,
       0,    98,   202,   200,   207,   208,   203,     0,   135,   188,
     106,   105,     0,   152,     0,    43,    39,     0,    40,     0,
       0,    91,    92,     0,   189,     0,     0,   107,    87,    32,
      66,    35,     0,    89,    88,     0,     0,     0,    53,    30,
     150,   193,     0,     0,    38,    97,     0,     0,    84,     0,
       0,     0,   120,     0,   114,    35,     0,   113,    62,    60,
     112,     0,    41,     0,     0,    97,    93,     0,     0,     0,
       0,     0,     0,     0,   194,    95,    96,     0,     0,    86,
     117,   116,   115,     0,   109,   111,    61,    94,     0,     0,
     110,     0,    85,   119,   118,   108
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -347,  -347,  -347,  -347,  -347,  -347,   343,  -347,  -347,   303,
      55,  -347,   107,  -347,   119,  -179,  -239,     6,  -347,  -347,
    -347,  -347,  -347,  -347,  -347,   -42,  -347,   108,   -14,  -347,
    -347,  -154,  -232,  -347,  -347,    70,  -347,  -347,  -347,  -347,
    -347,  -347,  -347,  -347,    -7,  -347,  -331,  -347,  -102,  -347,
    -347,  -347,  -347,  -346,  -347,   -18,   -44,  -347,  -121,  -347,
    -347,  -347,  -347,  -347,  -347,  -347,  -347,  -347,  -347,   305,
     -41,   349,   234,    98,   -28,  -347,   220,   223,    96,    79,
      -5,  -347,  -347,   -72,  -347,   118,  -347,   115,  -347
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     9,    11,     4,    16,    12,    42,    17,
      18,    29,    30,    31,    32,   169,   237,    46,    33,    34,
      35,    36,    37,    38,    39,   113,   181,   182,   185,   244,
     284,   172,   173,   239,   286,   287,   288,   289,   290,   291,
     292,   293,   330,   331,   332,   357,   310,   227,   311,   228,
     294,   295,   296,   297,   298,   183,    74,    75,   139,    76,
      77,    78,   200,    79,    80,    81,    82,   108,   109,   110,
     186,    83,   199,   232,   176,    85,    86,    87,    88,    89,
      90,    91,    92,   121,   195,   196,   197,   198,   255
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      84,   175,   138,    49,   245,   107,   304,   285,   111,   278,
     206,    48,   107,    48,    48,   119,   271,   107,    94,     5,
     123,   125,   126,    50,   373,     7,    47,   167,    51,    52,
      53,   248,    55,   168,   392,   355,   325,   253,   128,   107,
      84,     8,   140,   356,   387,     1,    95,     5,   194,    13,
     167,   171,    10,   404,    41,   120,   168,   137,   107,    13,
     107,   174,   129,   177,   132,   133,   134,   135,   136,   107,
      43,    44,   187,   188,   189,    45,   191,   242,    84,   180,
     107,   107,   257,   201,   261,   260,   272,   107,    45,   105,
     140,   161,   193,   162,    48,   307,   308,   309,   358,   163,
     164,   165,   366,   362,     5,   364,    13,   246,    19,   207,
      20,   202,    14,   247,    15,    21,    54,   229,   269,   328,
     341,   372,    22,    23,   270,    56,   381,    93,    24,    25,
      98,    99,   100,   230,   112,   233,    26,   114,   318,   115,
      27,   116,   238,   107,   107,   107,   389,   390,   391,   220,
     221,   222,   223,   224,   225,   226,   101,   138,   118,   122,
     365,   107,   124,   127,   140,   141,   402,   403,   142,   143,
      96,    15,   170,   249,   250,   251,   166,   171,    28,   178,
     190,   258,   194,   179,   315,   258,   204,   205,   231,   319,
     234,   154,   342,   268,   235,   236,   343,   243,   313,   155,
     344,   345,   107,   107,   240,   305,   306,   241,   252,  -205,
     156,   300,   346,   301,   107,   107,   254,   317,   140,  -121,
     320,   299,   157,   347,   259,  -121,   193,  -206,   314,   216,
     217,   218,   219,   158,   159,   160,   321,   323,   101,   324,
     210,   211,   212,   213,   214,   215,   256,    60,   138,   335,
     337,   338,   262,   342,   264,   263,   265,   343,    61,   266,
     107,   344,   339,   348,    62,   144,    63,    64,   267,  -121,
     145,    65,   120,   273,   146,   120,   147,   303,  -121,   171,
     395,  -121,   327,   326,   347,   150,   151,   152,   153,   329,
     333,   400,   340,   349,   350,   352,   148,   149,   351,   405,
     353,   354,   361,   102,   359,   360,   103,   363,   104,   375,
     105,   106,   377,   380,   367,   382,   383,   368,   369,   370,
     384,   274,    58,    59,   371,   275,   385,   386,   374,   276,
      60,    21,   388,   378,   397,   379,   277,   107,   398,   399,
     394,    61,   278,   401,     6,    97,   282,    62,   336,    63,
      64,   302,   376,   279,    65,   396,    27,   280,   283,   117,
      40,   203,   208,   322,     0,     0,   209,    57,    58,    59,
       0,   316,   312,     0,     0,     0,    60,     0,     0,     0,
       0,     0,     0,     0,   171,     0,    66,    61,    57,    58,
      59,    67,   281,    62,    68,    63,    64,    60,    69,    70,
      65,    71,     0,     0,    72,    73,     0,     0,    61,     0,
     130,     0,     0,     0,    62,     0,    63,    64,     0,     0,
       0,    65,     0,     0,     0,     0,    57,    58,    59,     0,
       0,     0,    66,     0,     0,    60,     0,    67,     0,     0,
      68,   131,     0,     0,    69,    70,    61,    71,     0,     0,
      72,    73,    62,    66,    63,    64,     0,     0,    67,    65,
       0,    68,   131,     0,     0,    69,    70,     0,    71,     0,
       0,    72,    73,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    57,    58,    59,     0,   120,
       0,    66,     0,     0,    60,     0,    67,   192,     0,    68,
       0,     0,     0,    69,    70,    61,    71,     0,     0,    72,
      73,    62,     0,    63,    64,     0,     0,     0,    65,     0,
       0,     0,     0,    57,    58,    59,     0,     0,     0,     0,
       0,     0,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    61,   334,    58,    59,     0,   120,    62,
      66,    63,    64,    60,     0,    67,    65,     0,    68,     0,
       0,     0,    69,    70,    61,    71,   101,     0,    72,    73,
      62,     0,    63,    64,     0,    60,     0,    65,     0,     0,
       0,     0,   101,     0,     0,     0,    61,     0,    66,     0,
       0,    60,    62,    67,    63,    64,    68,     0,     0,    65,
      69,    70,    61,    71,     0,     0,    72,    73,    62,    66,
      63,    64,     0,     0,    67,    65,     0,    68,     0,     0,
     101,    69,    70,     0,    71,     0,     0,    72,    73,    60,
       0,   102,     0,     0,     0,   184,   104,     0,     0,   106,
      61,   101,     0,     0,     0,     0,    62,   102,    63,    64,
      60,     0,   104,    65,   105,   106,     0,     0,     0,     0,
       0,    61,     0,     0,     0,     0,     0,    62,     0,    63,
      64,     0,     0,     0,    65,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   102,     0,     0,   393,     0,
     104,     0,     0,   106,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   102,     0,     0,     0,
       0,   104,     0,     0,   106
};

static const yytype_int16 yycheck[] =
{
      28,   103,    74,    21,   183,    49,   245,   239,    49,    24,
     131,     3,    56,     3,     3,    56,     3,    61,     6,     3,
      61,    63,    64,     3,   355,     0,    20,    13,    22,    23,
      24,   185,    26,    19,   380,    68,   268,   191,    66,    83,
      68,    67,    83,    76,   375,    30,    34,     3,   120,     5,
      13,    66,    25,   399,    67,    66,    19,    68,   102,     5,
     104,   102,    67,   104,    69,    70,    71,    72,    73,   113,
      15,    16,   114,   115,   116,    68,   118,   179,   106,    69,
     124,   125,    74,   124,   205,    74,    73,   131,    68,    75,
     131,    68,   120,    70,     3,   249,   250,   251,   330,    76,
      94,    95,   341,   335,     3,   337,     5,    13,     6,   137,
       8,   125,    68,    19,    70,    13,     3,   161,    13,   273,
     299,   353,    20,    21,    19,     3,   365,    67,    26,    27,
       3,     3,     3,   161,    68,   163,    34,     3,   259,     3,
      38,     3,   170,   187,   188,   189,   378,   379,   380,   154,
     155,   156,   157,   158,   159,   160,     3,   229,     3,    66,
     339,   205,    76,    66,   205,    77,   398,   399,    54,    53,
      69,    70,    68,   187,   188,   189,    67,    66,    76,     3,
       3,   199,   254,    71,   256,   203,    69,    77,     3,   261,
       3,    42,    40,   235,     3,    72,    44,    67,    74,    50,
      48,    49,   246,   247,    69,   246,   247,    75,    68,    88,
      61,   239,    60,   241,   258,   259,    75,   258,   259,    13,
     264,   239,    73,    71,    77,    19,   254,    88,   256,   150,
     151,   152,   153,    84,    85,    86,   264,   265,     3,   267,
     144,   145,   146,   147,   148,   149,    88,    12,   320,   277,
     278,   279,    69,    40,    75,    69,    88,    44,    23,    77,
     304,    48,   280,   304,    29,    47,    31,    32,    68,    63,
      52,    36,    66,    69,    56,    66,    58,    69,    72,    66,
     382,    75,     3,    69,    71,    80,    81,    82,    83,     3,
      68,   393,    67,    69,    67,    88,    78,    79,    77,   401,
      69,    69,    46,    68,    75,   333,    71,    67,    73,    68,
      75,    76,    75,    16,   342,    63,    46,   345,   346,   347,
      77,     3,     4,     5,   352,     7,    69,    77,   356,    11,
      12,    13,     3,   361,    69,   363,    18,   381,    69,    16,
     381,    23,    24,    71,     1,    42,   239,    29,   278,    31,
      32,   243,   359,    35,    36,   383,    38,    39,   239,    54,
      11,   127,   142,   265,    -1,    -1,   143,     3,     4,     5,
      -1,   256,   254,    -1,    -1,    -1,    12,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    -1,    68,    23,     3,     4,
       5,    73,    74,    29,    76,    31,    32,    12,    80,    81,
      36,    83,    -1,    -1,    86,    87,    -1,    -1,    23,    -1,
      46,    -1,    -1,    -1,    29,    -1,    31,    32,    -1,    -1,
      -1,    36,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
      -1,    -1,    68,    -1,    -1,    12,    -1,    73,    -1,    -1,
      76,    77,    -1,    -1,    80,    81,    23,    83,    -1,    -1,
      86,    87,    29,    68,    31,    32,    -1,    -1,    73,    36,
      -1,    76,    77,    -1,    -1,    80,    81,    -1,    83,    -1,
      -1,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,    66,
      -1,    68,    -1,    -1,    12,    -1,    73,    74,    -1,    76,
      -1,    -1,    -1,    80,    81,    23,    83,    -1,    -1,    86,
      87,    29,    -1,    31,    32,    -1,    -1,    -1,    36,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,    -1,    -1,    -1,
      -1,    -1,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    23,     3,     4,     5,    -1,    66,    29,
      68,    31,    32,    12,    -1,    73,    36,    -1,    76,    -1,
      -1,    -1,    80,    81,    23,    83,     3,    -1,    86,    87,
      29,    -1,    31,    32,    -1,    12,    -1,    36,    -1,    -1,
      -1,    -1,     3,    -1,    -1,    -1,    23,    -1,    68,    -1,
      -1,    12,    29,    73,    31,    32,    76,    -1,    -1,    36,
      80,    81,    23,    83,    -1,    -1,    86,    87,    29,    68,
      31,    32,    -1,    -1,    73,    36,    -1,    76,    -1,    -1,
       3,    80,    81,    -1,    83,    -1,    -1,    86,    87,    12,
      -1,    68,    -1,    -1,    -1,    72,    73,    -1,    -1,    76,
      23,     3,    -1,    -1,    -1,    -1,    29,    68,    31,    32,
      12,    -1,    73,    36,    75,    76,    -1,    -1,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    29,    -1,    31,
      32,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,    71,    -1,
      73,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,    -1,
      -1,    73,    -1,    -1,    76
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    30,    90,    91,    94,     3,    95,     0,    67,    92,
      25,    93,    96,     5,    68,    70,    95,    98,    99,     6,
       8,    13,    20,    21,    26,    27,    34,    38,    76,   100,
     101,   102,   103,   107,   108,   109,   110,   111,   112,   113,
     160,    67,    97,    99,    99,    68,   106,   106,     3,   144,
       3,   106,   106,   106,     3,   106,     3,     3,     4,     5,
      12,    23,    29,    31,    32,    36,    68,    73,    76,    80,
      81,    83,    86,    87,   145,   146,   148,   149,   150,   152,
     153,   154,   155,   160,   163,   164,   165,   166,   167,   168,
     169,   170,   171,    67,     6,    34,    69,    98,     3,     3,
       3,     3,    68,    71,    73,    75,    76,   145,   156,   157,
     158,   159,    68,   114,     3,     3,     3,   158,     3,   159,
      66,   172,    66,   159,    76,   114,   114,    66,   163,   169,
      46,    77,   169,   169,   169,   169,   169,    68,   172,   147,
     159,    77,    54,    53,    47,    52,    56,    58,    78,    79,
      80,    81,    82,    83,    42,    50,    61,    73,    84,    85,
      86,    68,    70,    76,   106,   106,    67,    13,    19,   104,
      68,    66,   120,   121,   159,   137,   163,   159,     3,    71,
      69,   115,   116,   144,    72,   117,   159,   114,   114,   114,
       3,   114,    74,   163,   172,   173,   174,   175,   176,   161,
     151,   159,   117,   161,    69,    77,   147,   163,   165,   166,
     167,   167,   167,   167,   167,   167,   168,   168,   168,   168,
     169,   169,   169,   169,   169,   169,   169,   136,   138,   145,
     163,     3,   162,   163,     3,     3,    72,   105,   163,   122,
      69,    75,   137,    67,   118,   104,    13,    19,   120,   117,
     117,   117,    68,   120,    75,   177,    88,    74,   144,    77,
      74,   147,    69,    69,    75,    88,    77,    68,   114,    13,
      19,     3,    73,    69,     3,     7,    11,    18,    24,    35,
      39,    74,   101,   103,   119,   121,   123,   124,   125,   126,
     127,   128,   129,   130,   139,   140,   141,   142,   143,   144,
     163,   163,   116,    69,   105,   159,   159,   120,   120,   120,
     135,   137,   174,    74,   163,   172,   176,   159,   147,   172,
     145,   163,   162,   163,   163,   121,    69,     3,   120,     3,
     131,   132,   133,    68,     3,   163,   124,   163,   163,   144,
      67,   104,    40,    44,    48,    49,    60,    71,   159,    69,
      67,    77,    88,    69,    69,    68,    76,   134,   121,    75,
     163,    46,   121,    67,   121,   104,   105,   163,   163,   163,
     163,   163,   121,   135,   163,    68,   133,    75,   163,   163,
      16,   105,    63,    46,    77,    69,    77,   135,     3,   121,
     121,   121,   142,    71,   159,   137,   163,    69,    69,    16,
     137,    71,   121,   121,   142,   137
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    89,    90,    91,    92,    92,    93,    93,    94,    95,
      96,    96,    97,    97,    98,    98,    98,    99,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   101,   101,
     102,   103,   104,   104,   104,   105,   105,   105,   106,   106,
     107,   107,   108,   108,   109,   110,   111,   112,   113,   114,
     114,   115,   115,   116,   117,   117,   117,   117,   118,   118,
     119,   119,   119,   120,   121,   122,   122,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   124,
     124,   124,   124,   125,   126,   127,   128,   129,   130,   130,
     131,   131,   132,   132,   133,   133,   134,   135,   135,   136,
     136,   137,   137,   138,   138,   138,   138,   139,   140,   140,
     140,   140,   141,   141,   142,   142,   142,   142,   142,   142,
     143,   144,   144,   145,   145,   145,   145,   145,   145,   145,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   157,   158,   159,   159,   159,   160,   161,
     161,   162,   162,   163,   164,   164,   165,   165,   166,   166,
     166,   166,   166,   166,   166,   167,   167,   167,   167,   167,
     168,   168,   168,   168,   168,   168,   168,   168,   169,   169,
     169,   169,   169,   169,   169,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   171,   171,   172,
     172,   173,   173,   174,   174,   175,   175,   176,   176,   177,
     177
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     4,     0,     3,     0,     3,     2,     1,
       2,     4,     0,     3,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     4,
       7,     3,     0,     1,     1,     0,     2,     2,     7,     6,
       7,     8,     5,     6,     4,     6,     6,     6,     5,     2,
       4,     1,     3,     4,     0,     1,     3,     3,     0,     1,
       3,     5,     3,     1,     3,     0,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     3,     7,     5,     2,     2,     2,
       0,     1,     1,     3,     5,     4,     3,     0,     1,     0,
       1,     1,     3,     1,     1,     3,     3,     1,     7,     5,
       6,     5,     3,     3,     3,     5,     5,     5,     7,     7,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     4,     3,     5,     1,     4,     2,     3,
       2,     2,     1,     1,     1,     1,     1,     3,     3,     0,
       4,     0,     1,     1,     1,     3,     1,     3,     1,     3,
       3,     3,     3,     3,     3,     1,     3,     3,     3,     3,
       1,     3,     3,     3,     3,     3,     3,     3,     1,     2,
       2,     2,     2,     2,     2,     1,     2,     2,     5,     1,
       3,     3,     4,     6,     8,     4,     4,     1,     1,     2,
       4,     1,     3,     3,     1,     1,     1,     1,     1,     0,
       1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, scanner, source_file, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, scanner, source_file); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, yyscan_t scanner, source::SourceFile* source_file)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (source_file);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, yyscan_t scanner, source::SourceFile* source_file)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, source_file);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, yyscan_t scanner, source::SourceFile* source_file)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , scanner, source_file);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, scanner, source_file); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, yyscan_t scanner, source::SourceFile* source_file)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (source_file);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 100: /* TopLevelDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1511 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 101: /* ConstDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1517 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 102: /* InstanceDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1523 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 103: /* TypeDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1529 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 107: /* ActionDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1535 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 108: /* ReactionDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1541 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 109: /* BinderDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1547 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 110: /* InitDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1553 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 111: /* GetterDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1559 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 112: /* MethodDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1565 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 113: /* FunctionDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1571 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 116: /* Parameter  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1577 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 119: /* BindStatement  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1583 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 120: /* Body  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1589 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 123: /* Statement  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1595 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 124: /* SimpleStatement  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1601 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 125: /* EmptyStatement  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1607 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 126: /* Activate  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1613 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 127: /* Change  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1619 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 128: /* ForIota  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1625 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 129: /* Return  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1631 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 130: /* IncrementStatement  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1637 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 133: /* PushPortCall  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1643 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 134: /* Indexession  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1649 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 139: /* ExpressionStatement  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1655 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 140: /* VarDecl  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1661 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 141: /* AssignmentStatement  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1667 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 142: /* If  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1673 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 143: /* While  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1679 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 145: /* TypeLitExpression  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1685 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 146: /* Array  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1691 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 147: /* ElementType  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1697 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 148: /* StructType  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1703 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 149: /* Slice  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1709 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 150: /* Map  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1715 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 151: /* KeyType  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1721 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 152: /* ComponentType  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1727 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 153: /* PushPort  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1733 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 154: /* PullPort  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1739 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 155: /* Heap  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1745 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 156: /* Pointer  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1751 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 157: /* TypeLit  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1757 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 158: /* TypeName  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1763 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 159: /* Type  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1769 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 160: /* ArrayDimension  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1775 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 162: /* OptionalExpression  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1781 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 163: /* Expression  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1787 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 164: /* OrExpression  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1793 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 165: /* AndExpression  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1799 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 166: /* CompareExpression  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1805 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 167: /* AddExpression  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1811 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 168: /* MultiplyExpression  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1817 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 169: /* UnaryExpression  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1823 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 170: /* PrimaryExpression  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1829 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 171: /* Literal  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1835 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 174: /* Element  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1841 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 175: /* Key  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1847 "../../src/parser.cpp" /* yacc.c:1257  */
        break;

    case 176: /* Value  */
#line 125 "../../src/parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1853 "../../src/parser.cpp" /* yacc.c:1257  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (yyscan_t scanner, source::SourceFile* source_file)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */
#line 27 "../../src/parser.y" /* yacc.c:1429  */
{
  yylloc = 1;
}

#line 1966 "../../src/parser.cpp" /* yacc.c:1429  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, scanner, source_file);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 202 "../../src/parser.y" /* yacc.c:1646  */
    { source_file->top_level_decl_list = (yyvsp[0].list); }
#line 2155 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 209 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = new ast::TopLevelDeclList (source_file->location (yyloc)); }
#line 2161 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 210 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[-1].node)); }
#line 2167 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 213 "../../src/parser.y" /* yacc.c:1646  */
    { source_file->package_name = (yyvsp[0].identifier); }
#line 2173 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 214 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.identifier) = (yyvsp[0].identifier); }
#line 2179 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 226 "../../src/parser.y" /* yacc.c:1646  */
    { source_file->add_import (source::Location (source_file, (yylsp[0])), source::Identifier ("", source::Location (source_file, (yylsp[0]))), (yyvsp[0].literal)->eval.value.untyped_string_value.to_string ()); }
#line 2185 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 227 "../../src/parser.y" /* yacc.c:1646  */
    { source_file->add_import (source::Location (source_file, (yylsp[-1])), source::Identifier (".", source_file->location ((yylsp[-1]))), (yyvsp[0].literal)->eval.value.untyped_string_value.to_string ()); }
#line 2191 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 228 "../../src/parser.y" /* yacc.c:1646  */
    { source_file->add_import (source::Location (source_file, (yylsp[-1])), idref ((yyvsp[-1].identifier)), (yyvsp[0].literal)->eval.value.untyped_string_value.to_string ()); }
#line 2197 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 230 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.literal) = (yyvsp[0].literal); }
#line 2203 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 236 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2209 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 237 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2215 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 238 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2221 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 239 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2227 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 240 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2233 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 241 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2239 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 242 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2245 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 243 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2251 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 244 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2257 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 245 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2263 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 249 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ConstDecl (source_file->location ((yylsp[-4])), (yyvsp[-3].identifier_list), (yyvsp[-2].node), (yyvsp[0].list)); }
#line 2269 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 251 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ConstDecl (source_file->location ((yylsp[-3])), (yyvsp[-2].identifier_list), new EmptyType (source_file->location ((yylsp[-3]))), (yyvsp[0].list)); }
#line 2275 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 255 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new InstanceDecl (source_file->location ((yylsp[-6])), idref ((yyvsp[-5].identifier)), (yyvsp[-4].node), idref ((yyvsp[-3].identifier)), (yyvsp[-1].list)); }
#line 2281 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 257 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ast::TypeDecl (source_file->location ((yylsp[-2])), idref ((yyvsp[-1].identifier)), (yyvsp[0].node)); }
#line 2287 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 260 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Mutable;   }
#line 2293 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 261 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Immutable; }
#line 2299 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 262 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Foreign;   }
#line 2305 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 265 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Mutable;   }
#line 2311 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 266 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Immutable; }
#line 2317 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 267 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Foreign;   }
#line 2323 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 271 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.receiver) = new Receiver (source_file->location ((yylsp[-6])), idref ((yyvsp[-5].identifier)), (yyvsp[-4].mutability), (yyvsp[-3].mutability), true, idref ((yyvsp[-1].identifier))); }
#line 2329 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 273 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.receiver) = new Receiver (source_file->location ((yylsp[-5])), idref ((yyvsp[-4].identifier)), (yyvsp[-3].mutability), (yyvsp[-2].mutability), false, idref ((yyvsp[-1].identifier))); }
#line 2335 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 277 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ActionDecl (source_file->location ((yylsp[-6])), new EmptyExpression (source_file->location ((yylsp[-6]))), (yyvsp[-5].receiver), idref ((yyvsp[-4].identifier)), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2341 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 279 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ActionDecl (source_file->location ((yylsp[-7])), (yyvsp[-7].node), (yyvsp[-5].receiver), idref ((yyvsp[-4].identifier)), (yyvsp[-2].node), (yyvsp[0].statement_list)); }
#line 2347 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 283 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ReactionDecl (source_file->location ((yylsp[-4])), new EmptyExpression (source_file->location ((yylsp[-4]))), (yyvsp[-3].receiver), idref ((yyvsp[-2].identifier)), (yyvsp[-1].parameter_list), (yyvsp[0].node)); }
#line 2353 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 285 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ReactionDecl (source_file->location ((yylsp[-4])), (yyvsp[-5].node), (yyvsp[-3].receiver), idref ((yyvsp[-2].identifier)), (yyvsp[-1].parameter_list), (yyvsp[0].statement_list)); }
#line 2359 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 289 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinderDecl (source_file->location ((yylsp[-3])), (yyvsp[-2].receiver), idref ((yyvsp[-1].identifier)), (yyvsp[0].node)); }
#line 2365 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 293 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new InitializerDecl (source_file->location ((yylsp[-5])), (yyvsp[-4].receiver), idref ((yyvsp[-3].identifier)), (yyvsp[-2].parameter_list), (yyvsp[-1].parameter_list), (yyvsp[0].node)); }
#line 2371 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 297 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new GetterDecl (source_file->location ((yylsp[-5])), (yyvsp[-4].receiver), idref ((yyvsp[-3].identifier)), (yyvsp[-2].parameter_list), (yyvsp[-1].parameter_list), (yyvsp[0].node)); }
#line 2377 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 301 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new MethodDecl (source_file->location ((yylsp[-5])), (yyvsp[-4].receiver), idref ((yyvsp[-3].identifier)), (yyvsp[-2].parameter_list), (yyvsp[-1].parameter_list), (yyvsp[0].node)); }
#line 2383 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 305 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new FunctionDecl (source_file->location ((yylsp[-4])), idref ((yyvsp[-3].identifier)), (yyvsp[-2].parameter_list), (yyvsp[-1].parameter_list), (yyvsp[0].node)); }
#line 2389 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 309 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.parameter_list) = new ParameterList (source_file->location (yyloc)); }
#line 2395 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 311 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.parameter_list) = (yyvsp[-2].parameter_list); }
#line 2401 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 315 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.parameter_list) = new ParameterList (source_file->location ((yylsp[0]))); (yyval.parameter_list)->append ((yyvsp[0].node)); }
#line 2407 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 317 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.parameter_list) = (yyvsp[-2].parameter_list); (yyval.parameter_list)->append ((yyvsp[0].node)); }
#line 2413 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 321 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new VariableList (source_file->location ((yylsp[-3])), (yyvsp[-3].identifier_list), (yyvsp[-2].mutability), (yyvsp[-1].mutability), (yyvsp[0].node)); }
#line 2419 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 325 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.parameter_list) = new ParameterList (source_file->location (yyloc)); }
#line 2425 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 327 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.parameter_list) = new ParameterList (source_file->location (yyloc)); (yyval.parameter_list)->append (new VariableList (source_file->location ((yylsp[0])), (new IdentifierList (source_file->location ((yylsp[0]))))->append (source::Identifier ("", source_file->location ((yylsp[0])))), Mutable, Mutable, (yyvsp[0].node))); }
#line 2431 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 329 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.parameter_list) = new ParameterList (source_file->location (yyloc)); (yyval.parameter_list)->append (new VariableList (source_file->location ((yylsp[-2])), (new IdentifierList (source_file->location ((yylsp[-2]))))->append (source::Identifier ("", source_file->location ((yylsp[-2])))), Mutable, Immutable, (yyvsp[0].node))); }
#line 2437 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 331 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.parameter_list) = new ParameterList (source_file->location (yyloc)); (yyval.parameter_list)->append (new VariableList (source_file->location ((yylsp[-2])), (new IdentifierList (source_file->location ((yylsp[-2]))))->append (source::Identifier ("", source_file->location ((yylsp[-2])))), Mutable, Foreign, (yyvsp[0].node))); }
#line 2443 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 338 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BindPushPort (source_file->location ((yylsp[-2])), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2449 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 340 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BindPushPortParameter (source_file->location ((yylsp[-4])), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2455 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 342 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BindPullPort (source_file->location ((yylsp[-2])), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2461 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 344 "../../src/parser.y" /* yacc.c:1646  */
    { (yyvsp[0].statement_list)->is_body = true; (yyval.node) = (yyvsp[0].statement_list); }
#line 2467 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 346 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.statement_list) = (yyvsp[-1].statement_list); }
#line 2473 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 349 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.statement_list) = new StatementList (source_file->location (yyloc)); }
#line 2479 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 350 "../../src/parser.y" /* yacc.c:1646  */
    { (yyvsp[-2].statement_list)->append ((yyvsp[-1].node)); (yyval.statement_list) = (yyvsp[-2].statement_list); }
#line 2485 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 353 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2491 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 354 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2497 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 355 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2503 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 356 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2509 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 357 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2515 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 358 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].statement_list); }
#line 2521 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 359 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2527 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 360 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2533 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 361 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2539 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 362 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2545 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 363 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2551 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 364 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2557 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 367 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2563 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 368 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2569 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 369 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2575 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 370 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2581 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 372 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new EmptyStatement (source_file->location (yyloc)); }
#line 2587 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 376 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Activate (source_file->location ((yylsp[-2])), (yyvsp[-1].list), (yyvsp[0].statement_list)); }
#line 2593 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 380 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Change (source_file->location ((yylsp[-6])), (yyvsp[-4].node), idref ((yyvsp[-2].identifier)), (yyvsp[0].statement_list)); }
#line 2599 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 384 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ForIota (source_file->location ((yylsp[-4])), idref ((yyvsp[-3].identifier)), (yyvsp[-1].node), (yyvsp[0].statement_list)); }
#line 2605 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 388 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Return (source_file->location ((yylsp[-1])), (yyvsp[0].node)); }
#line 2611 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 392 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IncrementDecrement (source_file->location ((yylsp[-1])), (yyvsp[-1].node), IncrementDecrement::Increment); }
#line 2617 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 394 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IncrementDecrement (source_file->location ((yylsp[-1])), (yyvsp[-1].node), IncrementDecrement::Decrement); }
#line 2623 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 398 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = new ExpressionList (source_file->location (yyloc)); }
#line 2629 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 400 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[0].list); }
#line 2635 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 404 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new ExpressionList (source_file->location ((yylsp[0]))))->append ((yyvsp[0].node)); }
#line 2641 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 406 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node)); }
#line 2647 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 410 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IndexedPushPortCall (source_file->location ((yylsp[-4])), idref ((yyvsp[-4].identifier)), (yyvsp[-3].node), (yyvsp[-1].list)); }
#line 2653 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 412 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new PushPortCall (source_file->location ((yylsp[-3])), idref ((yyvsp[-3].identifier)), (yyvsp[-1].list)); }
#line 2659 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 414 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 2665 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 417 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = new ExpressionList (source_file->location (yyloc)); }
#line 2671 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 418 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[0].list); }
#line 2677 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 421 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = new ExpressionList (source_file->location (yyloc)); }
#line 2683 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 422 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[0].list); }
#line 2689 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 426 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new ExpressionList (source_file->location ((yylsp[0]))))->append ((yyvsp[0].node)); }
#line 2695 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 428 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node)); }
#line 2701 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 432 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new ExpressionList (source_file->location ((yylsp[0]))))->append ((yyvsp[0].node)); }
#line 2707 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 434 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new ExpressionList (source_file->location ((yylsp[0]))))->append (new TypeExpression (source_file->location ((yylsp[0])), (yyvsp[0].node))); }
#line 2713 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 105:
#line 436 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node)); }
#line 2719 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 438 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append (new TypeExpression (source_file->location ((yylsp[-2])), (yyvsp[0].node))); }
#line 2725 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 107:
#line 442 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ExpressionStatement (source_file->location ((yylsp[0])), (yyvsp[0].node)); }
#line 2731 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 108:
#line 446 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new VarDecl (source_file->location ((yylsp[-6])), (yyvsp[-5].identifier_list), (yyvsp[-4].mutability), (yyvsp[-3].mutability), (yyvsp[-2].node), (yyvsp[0].list)); }
#line 2737 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 109:
#line 448 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new VarDecl (source_file->location ((yylsp[-4])), (yyvsp[-3].identifier_list), (yyvsp[-2].mutability), (yyvsp[-1].mutability), (yyvsp[0].node), new ExpressionList (source_file->location ((yylsp[-4])))); }
#line 2743 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 110:
#line 450 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new VarDecl (source_file->location ((yylsp[-5])), (yyvsp[-4].identifier_list), (yyvsp[-3].mutability), (yyvsp[-2].mutability), new EmptyType (source_file->location ((yylsp[-5]))), (yyvsp[0].list)); }
#line 2749 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 111:
#line 452 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new VarDecl (source_file->location ((yylsp[-4])), (yyvsp[-4].identifier_list), (yyvsp[-3].mutability), (yyvsp[-2].mutability), new EmptyType (source_file->location ((yylsp[-4]))), (yyvsp[0].list)); }
#line 2755 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 456 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Assign (source_file->location ((yylsp[-2])), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2761 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 113:
#line 458 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new AddAssign (source_file->location ((yylsp[-2])), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2767 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 114:
#line 462 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new If (source_file->location ((yylsp[-2])), new EmptyStatement (source_file->location ((yylsp[-2]))), (yyvsp[-1].node), (yyvsp[0].statement_list), new StatementList (source_file->location ((yylsp[-2])))); }
#line 2773 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 115:
#line 464 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new If (source_file->location ((yylsp[-4])), new EmptyStatement (source_file->location ((yylsp[-4]))), (yyvsp[-3].node), (yyvsp[-2].statement_list), (yyvsp[0].node)); }
#line 2779 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 116:
#line 466 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new If (source_file->location ((yylsp[-4])), new EmptyStatement (source_file->location ((yylsp[-4]))), (yyvsp[-3].node), (yyvsp[-2].statement_list), (yyvsp[0].statement_list)); }
#line 2785 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 117:
#line 468 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new If (source_file->location ((yylsp[-4])), (yyvsp[-3].node), (yyvsp[-1].node), (yyvsp[0].statement_list), new StatementList (source_file->location ((yylsp[-4])))); }
#line 2791 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 118:
#line 470 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new If (source_file->location ((yylsp[-6])), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].statement_list), (yyvsp[0].node)); }
#line 2797 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 119:
#line 472 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new If (source_file->location ((yylsp[-6])), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].statement_list), (yyvsp[0].statement_list)); }
#line 2803 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 120:
#line 476 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new While (source_file->location ((yylsp[-2])), (yyvsp[-1].node), (yyvsp[0].statement_list)); }
#line 2809 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 121:
#line 480 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.identifier_list) = (new IdentifierList (source_file->location ((yylsp[0]))))->append (idref ((yyvsp[0].identifier))); }
#line 2815 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 122:
#line 482 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.identifier_list) = (yyvsp[-2].identifier_list)->append (idref ((yyvsp[0].identifier))); }
#line 2821 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 123:
#line 486 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2827 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 124:
#line 487 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2833 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 125:
#line 488 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2839 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 126:
#line 489 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2845 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 127:
#line 490 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2851 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 128:
#line 491 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2857 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 129:
#line 492 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2863 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 130:
#line 493 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2869 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 131:
#line 496 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Array (source_file->location ((yylsp[-1])), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2875 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 132:
#line 498 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2881 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 133:
#line 500 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].list); }
#line 2887 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 134:
#line 502 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Slice (source_file->location ((yylsp[-2])), (yyvsp[0].node)); }
#line 2893 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 135:
#line 504 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Map (source_file->location ((yylsp[-4])), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2899 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 136:
#line 506 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2905 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 137:
#line 510 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].list); static_cast<FieldList*> ((yyvsp[-1].list))->is_component = true; }
#line 2911 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 138:
#line 512 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new PushPort (source_file->location ((yylsp[-1])), (yyvsp[0].parameter_list)); }
#line 2917 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 139:
#line 516 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new PullPort (source_file->location ((yylsp[-2])), (yyvsp[-1].parameter_list), (yyvsp[0].parameter_list)); }
#line 2923 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 140:
#line 518 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Heap (source_file->location ((yylsp[-1])), (yyvsp[0].node)); }
#line 2929 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 141:
#line 520 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Pointer (source_file->location ((yylsp[-1])), (yyvsp[0].node)); }
#line 2935 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 142:
#line 523 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2941 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 143:
#line 524 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2947 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 144:
#line 526 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IdentifierType (source_file->location ((yylsp[0])), idref ((yyvsp[0].identifier))); }
#line 2953 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 145:
#line 529 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2959 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 146:
#line 530 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2965 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 147:
#line 531 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 2971 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 148:
#line 533 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 2977 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 149:
#line 537 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = new FieldList (source_file->location (yyloc)); }
#line 2983 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 150:
#line 539 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-3].list)->append (new VariableList (source_file->location ((yylsp[-3])), (yyvsp[-2].identifier_list), Mutable, Mutable, (yyvsp[-1].node))); }
#line 2989 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 151:
#line 543 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new EmptyExpression (source_file->location (yyloc)); }
#line 2995 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 152:
#line 545 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3001 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 153:
#line 547 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3007 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 154:
#line 551 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3013 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 155:
#line 553 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::logic_or, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3019 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 156:
#line 557 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3025 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 157:
#line 559 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::logic_and, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3031 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 158:
#line 563 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3037 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 159:
#line 565 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::equal, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3043 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 160:
#line 567 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::not_equal, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3049 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 161:
#line 569 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::less_than, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3055 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 162:
#line 571 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::less_equal, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3061 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 163:
#line 573 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::more_than, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3067 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 164:
#line 575 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::more_equal, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3073 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 165:
#line 579 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3079 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 166:
#line 581 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::add, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3085 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 167:
#line 583 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::subtract, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3091 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 168:
#line 585 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::bit_or, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3097 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 169:
#line 587 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::bit_xor, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3103 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 170:
#line 591 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3109 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 171:
#line 593 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::multiply, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3115 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 172:
#line 595 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::divide, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3121 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 173:
#line 597 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::modulus, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3127 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 174:
#line 599 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::left_shift, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3133 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 175:
#line 601 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::right_shift, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3139 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 176:
#line 603 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::bit_and, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3145 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 177:
#line 605 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_binary (source_file->location ((yylsp[-2])), &decl::bit_and_not, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3151 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 178:
#line 608 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3157 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 179:
#line 609 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_unary (source_file->location ((yylsp[-1])), &decl::posate, (yyvsp[0].node)); }
#line 3163 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 180:
#line 610 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_unary (source_file->location ((yylsp[-1])), &decl::negate, (yyvsp[0].node)); }
#line 3169 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 181:
#line 611 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_unary (source_file->location ((yylsp[-1])), &decl::logic_not, (yyvsp[0].node)); }
#line 3175 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 182:
#line 612 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = make_unary (source_file->location ((yylsp[-1])), &decl::complement, (yyvsp[0].node)); }
#line 3181 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 183:
#line 613 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Dereference (source_file->location ((yylsp[-1])), (yyvsp[0].node)); }
#line 3187 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 184:
#line 614 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new AddressOf (source_file->location ((yylsp[-1])), (yyvsp[0].node)); }
#line 3193 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 185:
#line 618 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3199 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 186:
#line 620 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new CompositeLiteral (source_file->location ((yylsp[-1])), (yyvsp[-1].node), (yyvsp[0].list)); }
#line 3205 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 187:
#line 622 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new CompositeLiteral (source_file->location ((yylsp[-1])), new IdentifierType (source_file->location ((yylsp[-1])), idref ((yyvsp[-1].identifier))), (yyvsp[0].list)); }
#line 3211 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 188:
#line 624 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new CompositeLiteral (source_file->location ((yylsp[-4])), new Array (source_file->location ((yylsp[-4])), new EmptyExpression (source_file->location ((yylsp[-4]))), (yyvsp[-1].node)), (yyvsp[0].list)); }
#line 3217 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 189:
#line 627 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IdentifierExpression (source_file->location ((yylsp[0])), idref ((yyvsp[0].identifier))); }
#line 3223 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 190:
#line 629 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 3229 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 191:
#line 631 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Select (source_file->location ((yylsp[-2])), (yyvsp[-2].node), idref ((yyvsp[0].identifier))); }
#line 3235 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 192:
#line 633 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Index (source_file->location ((yylsp[-3])), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 3241 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 193:
#line 635 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IndexSlice (source_file->location ((yylsp[-5])), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node), new EmptyExpression (source_file->location (yyloc))); }
#line 3247 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 194:
#line 637 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IndexSlice (source_file->location ((yylsp[-7])), (yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 3253 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 195:
#line 639 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Call (source_file->location ((yylsp[-3])), (yyvsp[-3].node), (yyvsp[-1].list)); }
#line 3259 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 196:
#line 641 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Conversion (source_file->location ((yylsp[-3])), new TypeExpression (source_file->location ((yylsp[-3])), (yyvsp[-3].node)), (yyvsp[-1].node)); }
#line 3265 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 197:
#line 644 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].literal); }
#line 3271 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 198:
#line 645 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].literal); }
#line 3277 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 199:
#line 648 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = new ElementList (source_file->location ((yylsp[-1]))); }
#line 3283 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 200:
#line 649 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list);                          }
#line 3289 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 201:
#line 652 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new ElementList (source_file->location ((yylsp[0]))))->append ((yyvsp[0].node)); }
#line 3295 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 202:
#line 653 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node)); }
#line 3301 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 203:
#line 656 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Element (source_file->location ((yylsp[-2])), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3307 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 204:
#line 657 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Element (source_file->location ((yylsp[0])), new EmptyExpression (source_file->location ((yylsp[0]))), (yyvsp[0].node)); }
#line 3313 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 205:
#line 660 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3319 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 206:
#line 661 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].list); }
#line 3325 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 207:
#line 664 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3331 "../../src/parser.cpp" /* yacc.c:1646  */
    break;

  case 208:
#line 665 "../../src/parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].list); }
#line 3337 "../../src/parser.cpp" /* yacc.c:1646  */
    break;


#line 3341 "../../src/parser.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, scanner, source_file, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, scanner, source_file, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, scanner, source_file);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, scanner, source_file);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, scanner, source_file, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, scanner, source_file);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, scanner, source_file);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 671 "../../src/parser.y" /* yacc.c:1906  */

