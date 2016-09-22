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
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "parser.y" /* yacc.c:339  */

#include "scanner.hpp"
#include "yyparse.hpp"
#include "debug.hpp"
#include "node.hpp"
#include "semantic.hpp"

using namespace ast;

#line 76 "parser.cpp" /* yacc.c:339  */

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
  IDENTIFIER = 258,
  LITERAL = 259,
  STRING_LIT = 260,
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
  IMPORT = 280,
  INIT = 281,
  INSTANCE = 282,
  INTERFACE = 283,
  MAP = 284,
  PACKAGE_KW = 285,
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
#line 11 "parser.y" /* yacc.c:355  */
  ast::Node* node;
  ast::List* list;
  ast::Identifier* identifier;
  Mutability mutability;

#line 185 "parser.cpp" /* yacc.c:355  */
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


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_SRC_PARSER_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 216 "parser.cpp" /* yacc.c:358  */

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
#define YYLAST   706

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  90
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  88
/* YYNRULES -- Number of rules.  */
#define YYNRULES  209
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  405

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   321

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
  68,    69,    73,    80,    75,    81,    88,    84,     2,     2,
  2,     2,     2,     2,     2,     2,     2,     2,    89,    67,
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
  65,    70
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
  0,   170,   170,   173,   175,   177,   180,   181,   184,   185,
  188,   189,   192,   193,   194,   196,   199,   200,   204,   205,
  206,   207,   208,   209,   210,   211,   212,   213,   216,   218,
  222,   225,   228,   229,   230,   233,   234,   235,   238,   240,
  244,   246,   250,   252,   256,   260,   264,   268,   272,   276,
  278,   282,   284,   288,   293,   294,   296,   298,   301,   302,
  305,   307,   309,   312,   315,   316,   319,   320,   321,   322,
  323,   324,   325,   326,   327,   328,   329,   330,   333,   334,
  335,   336,   338,   341,   345,   349,   353,   357,   359,   364,
  365,   369,   371,   375,   377,   380,   383,   384,   387,   388,
  391,   393,   397,   399,   401,   403,   407,   411,   413,   415,
  417,   421,   423,   427,   429,   431,   433,   435,   437,   441,
  445,   447,   452,   453,   454,   455,   456,   457,   458,   459,
  462,   464,   466,   468,   470,   472,   475,   478,   481,   484,
  486,   489,   490,   492,   495,   496,   497,   499,   503,   504,
  509,   510,   513,   516,   518,   522,   524,   528,   530,   532,
  534,   536,   538,   540,   544,   546,   548,   550,   552,   556,
  558,   560,   562,   564,   566,   568,   570,   574,   575,   576,
  577,   578,   579,   580,   583,   585,   587,   589,   592,   594,
  596,   598,   600,   602,   604,   606,   610,   611,   614,   615,
  618,   619,   622,   623,   626,   627,   630,   631,   633,   635
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "LITERAL", "STRING_LIT",
  "ACTION", "ACTIVATE", "BIND", "BREAK", "CASE", "CHANGE", "COMPONENT",
  "CONST", "CONTINUE", "DEFAULT", "ELSE", "FALLTHROUGH", "FOR", "FOREIGN",
  "FUNC", "GETTER", "GOTO", "HEAP", "IF", "IMPORT", "INIT", "INSTANCE",
  "INTERFACE", "MAP", "PACKAGE_KW", "PULL", "PUSH", "RANGE", "REACTION",
  "RETURN", "STRUCT", "SWITCH", "TYPE", "VAR", "ADD_ASSIGN", "AND_ASSIGN",
  "AND_NOT", "AND_NOT_ASSIGN", "DECREMENT", "DIVIDE_ASSIGN", "DOTDOTDOT",
  "EQUAL", "INCREMENT", "LEFT_ARROW", "LEFT_SHIFT", "LEFT_SHIFT_ASSIGN",
  "LESS_EQUAL", "LOGIC_AND", "LOGIC_OR", "MODULUS_ASSIGN", "MORE_EQUAL",
  "MULTIPLY_ASSIGN", "NOT_EQUAL", "OR_ASSIGN", "RIGHT_ARROW",
  "RIGHT_SHIFT", "RIGHT_SHIFT_ASSIGN", "SHORT_ASSIGN", "SUBTRACT_ASSIGN",
  "XOR_ASSIGN", "'{'", "';'", "'('", "')'", "\".\"", "'='", "'$'", "'*'",
  "'}'", "','", "'['", "']'", "'<'", "'>'", "'+'", "'-'", "'|'", "'^'",
  "'/'", "'%'", "'&'", "'!'", "'.'", "':'", "$accept", "Top", "SourceFile",
  "PackageClause", "PackageName", "ImportDeclList", "ImportDecl",
  "ImportSpecList", "ImportSpec", "ImportPath", "TopLevelDeclList",
  "TopLevelDecl", "ConstDecl", "InstanceDecl", "TypeDecl", "Mutability",
  "DereferenceMutability", "Receiver", "ActionDecl", "ReactionDecl",
  "BindDecl", "InitDecl", "GetterDecl", "MethodDecl", "FunctionDecl",
  "ParameterList", "ParameterListInternal", "Parameter",
  "ReturnParameterList", "optional_semicolon", "BindStatement", "Block",
  "StatementList", "Statement", "SimpleStatement", "EmptyStatement",
  "Activate", "Change", "ForIota", "Return", "IncrementStatement",
  "OptionalPushPortCallList", "PushPortCallList", "PushPortCall",
  "Indexession", "OptionalExpressionList", "OptionalTypeOrExpressionList",
  "ExpressionList", "TypeOrExpressionList", "ExpressionStatement",
  "VarDecl", "AssignmentStatement", "If", "While", "IdentifierList",
  "TypeLitExpression", "Array", "ElementType", "StructType", "Slice",
  "Map", "KeyType", "ComponentType", "PushPort", "PullPort", "Heap",
  "Pointer", "TypeLit", "TypeName", "Type", "ArrayDimension", "FieldList",
  "OptionalExpression", "Expression", "OrExpression", "AndExpression",
  "CompareExpression", "AddExpression", "MultiplyExpression",
  "UnaryExpression", "PrimaryExpression", "Literal", "LiteralValue",
  "ElementList", "Element", "Key", "Value", "OptionalComma", YY_NULLPTR
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
  321,    61,    36,    42,   125,    44,    91,    93,    60,    62,
  43,    45,   124,    94,    47,    37,    38,    33,    46,    58
};
# endif

#define YYPACT_NINF -368

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-368)))

#define YYTABLE_NINF -206

#define yytable_value_is_error(Yytable_value) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
  -9,    38,    49,  -368,   -14,  -368,  -368,  -368,  -368,    35,
  52,    -5,   280,  -368,  -368,    62,    62,  -368,  -368,  -368,
  1,     1,    68,    21,     1,     1,    76,     1,    80,   533,
  20,  -368,  -368,  -368,  -368,  -368,  -368,  -368,  -368,  -368,
  -368,    13,    61,  -368,  -368,    95,    98,    99,  -368,   124,
  41,   103,   118,   132,   135,   136,   251,  -368,  -368,  -368,
  74,   251,    78,    41,    41,    91,   533,   533,   377,   533,
  533,   533,   533,   533,   -16,  -368,  -368,  -368,  -368,  -368,
  -368,  -368,  -368,   251,    81,   109,   111,    90,    69,   166,
  -368,   -20,  -368,  -368,     1,     1,  -368,   113,    77,   114,
  100,  -368,   251,   533,   251,   168,   398,  -368,  -368,  -368,
  -368,   106,    17,   576,    41,    41,    41,   180,    41,  -368,
  436,  -368,  -368,  -368,   251,   576,  -368,  -368,   117,  -368,
  116,   251,  -368,  -368,  -368,  -368,  -368,   533,  -368,  -368,
  -368,  -368,   533,   533,   533,   533,   533,   533,   533,   533,
  533,   533,   533,   533,   533,   533,   533,   533,   533,   533,
  533,   533,   533,   185,   187,   188,  -368,  -368,  -368,   126,
  533,  -368,  -368,   127,   129,  -368,  -368,  -368,   533,  -368,
  134,  -368,    32,    86,   100,  -368,   576,   576,   576,   137,
  100,  -368,   121,   123,   142,  -368,   133,  -368,     4,   146,
  -368,  -368,     6,  -368,   251,  -368,   156,   111,    90,    69,
  69,    69,    69,    69,    69,   166,   166,   166,   166,  -368,
  -368,  -368,  -368,  -368,  -368,  -368,   159,   154,   -16,  -368,
  141,   161,  -368,   163,    41,   115,    11,   164,   331,  -368,
  533,   129,    68,   167,   126,   251,   251,  -368,   100,   100,
  100,   533,  -368,   495,   171,   495,  -368,   592,   251,  -368,
  174,  -368,  -368,   533,   533,  -368,   533,   100,  -368,  -368,
  172,   243,   100,   104,   252,   189,   554,   533,   533,    68,
  -368,  -368,  -368,  -368,  -368,   191,  -368,  -368,  -368,  -368,
  -368,  -368,  -368,  -368,  -368,  -368,  -368,  -368,    32,   376,
  -368,  -368,  -368,   251,  -368,  -368,  -368,  -368,  -368,   190,
  129,  -368,  -368,  -368,  -368,  -368,   200,  -368,  -368,   -16,
  -368,   179,   181,   199,  -368,  -368,   202,  -368,   -30,   100,
  197,  -368,   533,   229,   100,   209,   225,  -368,    32,  -368,
  126,   533,  -368,  -368,   533,   533,   533,  -368,  -368,  -368,
  -368,   533,   100,  -368,   533,   533,   210,  -368,   252,   206,
  533,  -368,   533,   261,   126,   221,  -368,  -368,   239,  -368,
  212,  -368,   223,   217,   533,  -368,   292,   100,   100,    -8,
  630,   533,   533,  -368,  -368,  -368,   228,   233,  -368,   283,
  -368,  -368,   533,   234,   129,  -368,  -368,   100,    -8,   129,
  533,  -368,  -368,  -368,   129
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
  0,     0,     0,     2,     0,     5,     4,     1,     6,    16,
  0,     0,     3,    15,    10,     0,     0,     8,    14,     7,
  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  0,    18,    27,    19,    24,    25,    26,    22,    23,    21,
  20,     0,     0,    12,    13,     0,     0,     0,   120,     0,
  0,     0,     0,     0,     0,     0,     0,   188,   196,   197,
  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  0,     0,     0,     0,     0,   122,   123,   124,   125,   126,
  127,   128,   129,     0,     0,   152,   153,   155,   157,   164,
  169,   177,   184,    17,     0,     0,     9,     0,    32,     0,
  0,   143,     0,     0,     0,     0,     0,   142,   141,   145,
  144,     0,     0,    54,     0,     0,     0,     0,     0,    31,
  0,   186,   148,   139,     0,    54,   137,   148,     0,   182,
  0,     0,   178,   179,   181,   183,   180,     0,   185,   130,
  131,   147,     0,     0,     0,     0,     0,     0,     0,     0,
  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  0,    98,   150,     0,     0,     0,    11,    33,    34,    35,
  0,    64,    44,     0,    29,   100,   140,   121,     0,    49,
  58,    51,    32,     0,     0,    55,    54,    54,    54,     0,
  0,   198,   206,   207,   208,   200,     0,   203,     0,     0,
  135,   138,     0,   189,     0,   133,     0,   154,   156,   158,
  161,   163,   159,   160,   162,   165,   166,   167,   168,   176,
  173,   174,   170,   171,   172,   175,     0,    99,   103,   102,
  0,   151,   190,     0,     0,     0,     0,     0,    82,   146,
  0,    28,    59,     0,    35,     0,     0,    48,     0,     0,
  0,    96,    42,   209,     0,     0,   136,     0,     0,   132,
  0,   195,   194,     0,   150,   191,     0,     0,    36,    37,
  0,     0,     0,   188,    89,     0,     0,    82,     0,     0,
  63,    67,    68,    77,    71,     0,    66,    78,    70,    75,
  76,    72,    80,    79,    69,    81,    73,    74,    32,   106,
  101,    52,    50,     0,    56,    57,    47,    46,    45,     0,
  97,   201,   199,   206,   207,   202,     0,   134,   187,   105,
  104,     0,   151,     0,    43,    39,     0,    40,     0,     0,
  90,    91,     0,   188,     0,     0,   106,    86,    32,    65,
  35,     0,    88,    87,     0,     0,     0,    53,    30,   149,
  192,     0,     0,    38,    96,     0,     0,    83,     0,     0,
  0,   119,     0,   113,    35,     0,   112,    62,    60,   111,
  0,    41,     0,     0,    96,    92,     0,     0,     0,     0,
  0,     0,     0,   193,    94,    95,     0,     0,    85,   116,
  115,   114,     0,   108,   110,    61,    93,     0,     0,   109,
  0,    84,   118,   117,   107
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
  -368,  -368,  -368,  -368,   303,  -368,  -368,  -368,   266,   110,
  -368,  -368,    71,  -368,    72,  -179,  -240,     9,  -368,  -368,
  -368,  -368,  -368,  -368,  -368,   -21,  -368,    79,  -114,  -368,
  -368,    -6,  -368,  -368,    34,  -368,  -368,  -368,  -368,  -368,
  -368,  -368,  -368,   -45,  -368,  -339,  -368,  -102,  -368,  -368,
  -368,  -368,  -367,  -368,   -17,   -43,  -368,  -129,  -368,  -368,
  -368,  -368,  -368,  -368,  -368,  -368,  -368,  -368,   271,   -39,
  308,   205,    65,   -29,  -368,   198,   196,   230,    22,   -44,
  -368,  -368,   -66,  -368,    92,  -368,    93,  -368
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
  -1,     2,     3,     4,    16,     9,    11,    42,    17,    18,
  12,    30,    31,    32,    33,   169,   236,    46,    34,    35,
  36,    37,    38,    39,    40,   113,   180,   181,   184,   243,
  283,   172,   238,   285,   286,   287,   288,   289,   290,   291,
  292,   329,   330,   331,   356,   309,   226,   310,   227,   293,
  294,   295,   296,   297,   182,    74,    75,   139,    76,    77,
  78,   199,    79,    80,    81,    82,   108,   109,   110,   185,
  83,   198,   230,   175,    85,    86,    87,    88,    89,    90,
  91,    92,   121,   194,   195,   196,   197,   254
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
  84,   174,   205,   244,   303,    49,   107,    48,   138,    48,
  111,   201,   391,   107,   270,   372,   277,   119,   107,    94,
  48,     1,   123,   129,    50,   132,   133,   134,   135,   136,
  47,   403,    51,    52,    53,   386,    55,   128,   354,    84,
  107,     5,   125,   126,   140,   167,   355,    95,   161,     7,
  120,   168,   137,     8,   193,     5,   162,    13,   171,   107,
  10,   107,    19,   173,     5,   176,    13,    13,   163,    45,
  107,    48,   248,   249,   250,   260,   241,    84,   256,    54,
  259,   107,   107,    56,   271,   200,   179,    93,   107,    45,
  167,   192,   140,   186,   187,   188,   168,   190,    98,   245,
  365,    99,   100,   164,   165,   246,   114,   105,   206,   112,
  219,   220,   221,   222,   223,   224,   225,  -120,   228,   340,
  14,   115,    15,  -120,   380,    43,    44,   101,   268,   317,
  96,    15,   229,   231,   269,   116,    60,   144,   101,   118,
  122,   237,   145,   107,   107,   107,   146,    61,   147,   150,
  151,   152,   153,    62,   124,    63,    64,   127,   141,   364,
  65,   107,   138,   142,   143,   140,   171,  -120,   148,   149,
  120,   177,   215,   216,   217,   218,  -120,   178,   247,  -120,
  166,   257,   170,   189,   252,   257,   203,   193,   232,   314,
  233,   234,   102,   204,   318,   103,   239,   104,   235,   105,
  106,   242,   107,   107,   240,   251,   304,   305,   154,   299,
  -204,   300,  -205,   267,   107,   107,   155,   253,   316,   140,
  319,   298,   255,   258,   192,   261,   313,   156,   262,   263,
  264,   266,   284,   272,   320,   322,   302,   323,   265,   157,
  120,   325,   306,   307,   308,   312,   326,   334,   336,   337,
  158,   159,   160,   138,   101,   328,   350,   332,   339,   348,
  107,   324,   338,    60,   347,   341,   327,   349,   352,   342,
  351,   353,   358,   343,    61,   360,   362,   379,   374,   394,
  62,   376,    63,    64,   381,   382,    20,    65,    21,   383,
  399,   171,   384,    22,   385,   387,   346,   396,   404,   398,
  23,    24,   397,   359,     6,   400,    25,    26,    97,   281,
  282,   335,   366,   375,    27,   367,   368,   369,    28,   102,
  41,   301,   370,   357,   104,   117,   373,   106,   361,   321,
  363,   377,   202,   378,   273,    58,    59,   107,   274,   208,
  207,   393,   275,    60,    22,   311,   371,     0,   315,   276,
  0,     0,     0,   395,    61,   277,    29,     0,     0,     0,
  62,     0,    63,    64,     0,     0,   278,    65,     0,    28,
  279,   388,   389,   390,   209,   210,   211,   212,   213,   214,
  57,    58,    59,     0,     0,     0,     0,     0,     0,    60,
  0,   401,   402,     0,     0,     0,     0,   171,     0,    66,
  61,    57,    58,    59,    67,   280,    62,    68,    63,    64,
  60,    69,    70,    65,    71,     0,   341,    72,    73,     0,
  342,    61,     0,   130,   343,   344,     0,    62,     0,    63,
  64,     0,     0,     0,    65,     0,   345,     0,     0,    57,
  58,    59,     0,     0,     0,    66,     0,   346,    60,     0,
  67,     0,     0,    68,   131,     0,     0,    69,    70,    61,
  71,     0,     0,    72,    73,    62,    66,    63,    64,     0,
  0,    67,    65,     0,    68,   131,     0,     0,    69,    70,
  0,    71,     0,     0,    72,    73,     0,     0,     0,     0,
  0,     0,     0,     0,     0,     0,     0,     0,    57,    58,
  59,     0,   120,     0,    66,     0,     0,    60,     0,    67,
  191,     0,    68,     0,     0,     0,    69,    70,    61,    71,
  0,     0,    72,    73,    62,     0,    63,    64,     0,     0,
  0,    65,     0,     0,     0,     0,    57,    58,    59,     0,
  0,     0,     0,     0,     0,    60,     0,     0,     0,     0,
  0,     0,     0,     0,     0,     0,    61,   333,    58,    59,
  0,   120,    62,    66,    63,    64,    60,     0,    67,    65,
  0,    68,     0,     0,     0,    69,    70,    61,    71,   101,
  0,    72,    73,    62,     0,    63,    64,     0,    60,     0,
  65,     0,     0,     0,     0,   101,     0,     0,     0,    61,
  0,    66,     0,     0,    60,    62,    67,    63,    64,    68,
  0,     0,    65,    69,    70,    61,    71,     0,     0,    72,
  73,    62,    66,    63,    64,     0,     0,    67,    65,     0,
  68,     0,     0,   101,    69,    70,     0,    71,     0,     0,
  72,    73,    60,     0,   102,     0,     0,     0,   183,   104,
  0,     0,   106,    61,     0,     0,     0,     0,     0,    62,
  102,    63,    64,     0,     0,   104,    65,   105,   106,     0,
  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  0,     0,     0,     0,     0,     0,     0,     0,   102,     0,
  0,   392,     0,   104,     0,     0,   106
};

static const yytype_int16 yycheck[] =
{
  29,   103,   131,   182,   244,    22,    49,     3,    74,     3,
  49,   125,   379,    56,     3,   354,    24,    56,    61,     6,
  3,    30,    61,    67,     3,    69,    70,    71,    72,    73,
  21,   398,    23,    24,    25,   374,    27,    66,    68,    68,
  83,     3,    63,    64,    83,    13,    76,    34,    68,     0,
  66,    19,    68,    67,   120,     3,    76,     5,    66,   102,
  25,   104,    67,   102,     3,   104,     5,     5,    88,    68,
  113,     3,   186,   187,   188,   204,   178,   106,    74,     3,
  74,   124,   125,     3,    73,   124,    69,    67,   131,    68,
  13,   120,   131,   114,   115,   116,    19,   118,     3,    13,
  340,     3,     3,    94,    95,    19,     3,    75,   137,    68,
  154,   155,   156,   157,   158,   159,   160,    13,   161,   298,
  68,     3,    70,    19,   364,    15,    16,     3,    13,   258,
  69,    70,   161,   162,    19,     3,    12,    47,     3,     3,
  66,   170,    52,   186,   187,   188,    56,    23,    58,    80,
  81,    82,    83,    29,    76,    31,    32,    66,    77,   338,
  36,   204,   228,    54,    53,   204,    66,    63,    78,    79,
  66,     3,   150,   151,   152,   153,    72,    71,   184,    75,
  67,   198,    68,     3,   190,   202,    69,   253,     3,   255,
  3,     3,    68,    77,   260,    71,    69,    73,    72,    75,
  76,    67,   245,   246,    75,    68,   245,   246,    42,   238,
  89,   240,    89,   234,   257,   258,    50,    75,   257,   258,
  263,   238,    89,    77,   253,    69,   255,    61,    69,    75,
  89,    68,   238,    69,   263,   264,    69,   266,    77,    73,
  66,    69,   248,   249,   250,    74,     3,   276,   277,   278,
  84,    85,    86,   319,     3,     3,    77,    68,    67,    69,
  303,   267,   279,    12,   303,    40,   272,    67,    69,    44,
  89,    69,    75,    48,    23,    46,    67,    16,    68,   381,
  29,    75,    31,    32,    63,    46,     6,    36,     8,    77,
  392,    66,    69,    13,    77,     3,    71,    69,   400,    16,
  20,    21,    69,   332,     1,    71,    26,    27,    42,   238,
  238,   277,   341,   358,    34,   344,   345,   346,    38,    68,
  12,   242,   351,   329,    73,    54,   355,    76,   334,   264,
  336,   360,   127,   362,     3,     4,     5,   380,     7,   143,
  142,   380,    11,    12,    13,   253,   352,    -1,   255,    18,
  -1,    -1,    -1,   382,    23,    24,    76,    -1,    -1,    -1,
  29,    -1,    31,    32,    -1,    -1,    35,    36,    -1,    38,
  39,   377,   378,   379,   144,   145,   146,   147,   148,   149,
  3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    12,
  -1,   397,   398,    -1,    -1,    -1,    -1,    66,    -1,    68,
  23,     3,     4,     5,    73,    74,    29,    76,    31,    32,
  12,    80,    81,    36,    83,    -1,    40,    86,    87,    -1,
  44,    23,    -1,    46,    48,    49,    -1,    29,    -1,    31,
  32,    -1,    -1,    -1,    36,    -1,    60,    -1,    -1,     3,
  4,     5,    -1,    -1,    -1,    68,    -1,    71,    12,    -1,
  73,    -1,    -1,    76,    77,    -1,    -1,    80,    81,    23,
  83,    -1,    -1,    86,    87,    29,    68,    31,    32,    -1,
  -1,    73,    36,    -1,    76,    77,    -1,    -1,    80,    81,
  -1,    83,    -1,    -1,    86,    87,    -1,    -1,    -1,    -1,
  -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
  5,    -1,    66,    -1,    68,    -1,    -1,    12,    -1,    73,
  74,    -1,    76,    -1,    -1,    -1,    80,    81,    23,    83,
  -1,    -1,    86,    87,    29,    -1,    31,    32,    -1,    -1,
  -1,    36,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
  -1,    -1,    -1,    -1,    -1,    12,    -1,    -1,    -1,    -1,
  -1,    -1,    -1,    -1,    -1,    -1,    23,     3,     4,     5,
  -1,    66,    29,    68,    31,    32,    12,    -1,    73,    36,
  -1,    76,    -1,    -1,    -1,    80,    81,    23,    83,     3,
  -1,    86,    87,    29,    -1,    31,    32,    -1,    12,    -1,
  36,    -1,    -1,    -1,    -1,     3,    -1,    -1,    -1,    23,
  -1,    68,    -1,    -1,    12,    29,    73,    31,    32,    76,
  -1,    -1,    36,    80,    81,    23,    83,    -1,    -1,    86,
  87,    29,    68,    31,    32,    -1,    -1,    73,    36,    -1,
  76,    -1,    -1,     3,    80,    81,    -1,    83,    -1,    -1,
  86,    87,    12,    -1,    68,    -1,    -1,    -1,    72,    73,
  -1,    -1,    76,    23,    -1,    -1,    -1,    -1,    -1,    29,
  68,    31,    32,    -1,    -1,    73,    36,    75,    76,    -1,
  -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
  -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
  -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,
  -1,    71,    -1,    73,    -1,    -1,    76
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
  0,    30,    91,    92,    93,     3,    94,     0,    67,    95,
  25,    96,   100,     5,    68,    70,    94,    98,    99,    67,
  6,     8,    13,    20,    21,    26,    27,    34,    38,    76,
  101,   102,   103,   104,   108,   109,   110,   111,   112,   113,
  114,   160,    97,    99,    99,    68,   107,   107,     3,   144,
  3,   107,   107,   107,     3,   107,     3,     3,     4,     5,
  12,    23,    29,    31,    32,    36,    68,    73,    76,    80,
  81,    83,    86,    87,   145,   146,   148,   149,   150,   152,
  153,   154,   155,   160,   163,   164,   165,   166,   167,   168,
  169,   170,   171,    67,     6,    34,    69,    98,     3,     3,
  3,     3,    68,    71,    73,    75,    76,   145,   156,   157,
  158,   159,    68,   115,     3,     3,     3,   158,     3,   159,
  66,   172,    66,   159,    76,   115,   115,    66,   163,   169,
  46,    77,   169,   169,   169,   169,   169,    68,   172,   147,
  159,    77,    54,    53,    47,    52,    56,    58,    78,    79,
  80,    81,    82,    83,    42,    50,    61,    73,    84,    85,
  86,    68,    76,    88,   107,   107,    67,    13,    19,   105,
  68,    66,   121,   159,   137,   163,   159,     3,    71,    69,
  116,   117,   144,    72,   118,   159,   115,   115,   115,     3,
  115,    74,   163,   172,   173,   174,   175,   176,   161,   151,
  159,   118,   161,    69,    77,   147,   163,   165,   166,   167,
  167,   167,   167,   167,   167,   168,   168,   168,   168,   169,
  169,   169,   169,   169,   169,   169,   136,   138,   145,   163,
  162,   163,     3,     3,     3,    72,   106,   163,   122,    69,
  75,   137,    67,   119,   105,    13,    19,   121,   118,   118,
  118,    68,   121,    75,   177,    89,    74,   144,    77,    74,
  147,    69,    69,    75,    89,    77,    68,   115,    13,    19,
  3,    73,    69,     3,     7,    11,    18,    24,    35,    39,
  74,   102,   104,   120,   121,   123,   124,   125,   126,   127,
  128,   129,   130,   139,   140,   141,   142,   143,   144,   163,
  163,   117,    69,   106,   159,   159,   121,   121,   121,   135,
  137,   174,    74,   163,   172,   176,   159,   147,   172,   145,
  163,   162,   163,   163,   121,    69,     3,   121,     3,   131,
  132,   133,    68,     3,   163,   124,   163,   163,   144,    67,
  105,    40,    44,    48,    49,    60,    71,   159,    69,    67,
  77,    89,    69,    69,    68,    76,   134,   121,    75,   163,
  46,   121,    67,   121,   105,   106,   163,   163,   163,   163,
  163,   121,   135,   163,    68,   133,    75,   163,   163,    16,
  106,    63,    46,    77,    69,    77,   135,     3,   121,   121,
  121,   142,    71,   159,   137,   163,    69,    69,    16,   137,
  71,   121,   121,   142,   137
};

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
  0,    90,    91,    92,    93,    94,    95,    95,    96,    96,
  97,    97,    98,    98,    98,    99,   100,   100,   101,   101,
  101,   101,   101,   101,   101,   101,   101,   101,   102,   102,
  103,   104,   105,   105,   105,   106,   106,   106,   107,   107,
  108,   108,   109,   109,   110,   111,   112,   113,   114,   115,
  115,   116,   116,   117,   118,   118,   118,   118,   119,   119,
  120,   120,   120,   121,   122,   122,   123,   123,   123,   123,
  123,   123,   123,   123,   123,   123,   123,   123,   124,   124,
  124,   124,   125,   126,   127,   128,   129,   130,   130,   131,
  131,   132,   132,   133,   133,   134,   135,   135,   136,   136,
  137,   137,   138,   138,   138,   138,   139,   140,   140,   140,
  140,   141,   141,   142,   142,   142,   142,   142,   142,   143,
  144,   144,   145,   145,   145,   145,   145,   145,   145,   145,
  146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
  156,   157,   157,   158,   159,   159,   159,   160,   161,   161,
  162,   162,   163,   164,   164,   165,   165,   166,   166,   166,
  166,   166,   166,   166,   167,   167,   167,   167,   167,   168,
  168,   168,   168,   168,   168,   168,   168,   169,   169,   169,
  169,   169,   169,   169,   170,   170,   170,   170,   170,   170,
  170,   170,   170,   170,   170,   170,   171,   171,   172,   172,
  173,   173,   174,   174,   175,   175,   176,   176,   177,   177
};

/* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
  0,     2,     1,     4,     2,     1,     0,     3,     2,     4,
  0,     3,     2,     2,     1,     1,     0,     3,     1,     1,
  1,     1,     1,     1,     1,     1,     1,     1,     5,     4,
  7,     3,     0,     1,     1,     0,     2,     2,     7,     6,
  7,     8,     5,     6,     4,     6,     6,     6,     5,     2,
  4,     1,     3,     4,     0,     1,     3,     3,     0,     1,
  3,     5,     3,     3,     0,     3,     1,     1,     1,     1,
  1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
  1,     1,     0,     3,     7,     5,     2,     2,     2,     0,
  1,     1,     3,     5,     4,     3,     0,     1,     0,     1,
  1,     3,     1,     1,     3,     3,     1,     7,     5,     6,
  5,     3,     3,     3,     5,     5,     5,     7,     7,     3,
  1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
  2,     1,     4,     3,     5,     1,     4,     2,     3,     2,
  2,     1,     1,     1,     1,     1,     3,     3,     0,     4,
  0,     1,     1,     1,     3,     1,     3,     1,     3,     3,
  3,     3,     3,     3,     1,     3,     3,     3,     3,     1,
  3,     3,     3,     3,     3,     3,     3,     1,     2,     2,
  2,     2,     2,     2,     1,     2,     2,     5,     1,     3,
  3,     4,     6,     8,     4,     4,     1,     1,     2,     4,
  1,     3,     3,     1,     1,     1,     1,     1,     0,     1
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
      yyerror (YY_("syntax error: cannot back up")); \
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
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
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
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
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
do_not_strip_quotes:
      ;
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
    case 4: /* LITERAL  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1482 "parser.cpp" /* yacc.c:1257  */
      break;

    case 5: /* STRING_LIT  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1488 "parser.cpp" /* yacc.c:1257  */
      break;

    case 92: /* SourceFile  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1494 "parser.cpp" /* yacc.c:1257  */
      break;

    case 101: /* TopLevelDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1500 "parser.cpp" /* yacc.c:1257  */
      break;

    case 102: /* ConstDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1506 "parser.cpp" /* yacc.c:1257  */
      break;

    case 103: /* InstanceDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1512 "parser.cpp" /* yacc.c:1257  */
      break;

    case 104: /* TypeDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1518 "parser.cpp" /* yacc.c:1257  */
      break;

    case 107: /* Receiver  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1524 "parser.cpp" /* yacc.c:1257  */
      break;

    case 108: /* ActionDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1530 "parser.cpp" /* yacc.c:1257  */
      break;

    case 109: /* ReactionDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1536 "parser.cpp" /* yacc.c:1257  */
      break;

    case 110: /* BindDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1542 "parser.cpp" /* yacc.c:1257  */
      break;

    case 111: /* InitDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1548 "parser.cpp" /* yacc.c:1257  */
      break;

    case 112: /* GetterDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1554 "parser.cpp" /* yacc.c:1257  */
      break;

    case 113: /* MethodDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1560 "parser.cpp" /* yacc.c:1257  */
      break;

    case 114: /* FunctionDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1566 "parser.cpp" /* yacc.c:1257  */
      break;

    case 117: /* Parameter  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1572 "parser.cpp" /* yacc.c:1257  */
      break;

    case 120: /* BindStatement  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1578 "parser.cpp" /* yacc.c:1257  */
      break;

    case 121: /* Block  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1584 "parser.cpp" /* yacc.c:1257  */
      break;

    case 123: /* Statement  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1590 "parser.cpp" /* yacc.c:1257  */
      break;

    case 124: /* SimpleStatement  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1596 "parser.cpp" /* yacc.c:1257  */
      break;

    case 125: /* EmptyStatement  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1602 "parser.cpp" /* yacc.c:1257  */
      break;

    case 126: /* Activate  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1608 "parser.cpp" /* yacc.c:1257  */
      break;

    case 127: /* Change  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1614 "parser.cpp" /* yacc.c:1257  */
      break;

    case 128: /* ForIota  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1620 "parser.cpp" /* yacc.c:1257  */
      break;

    case 129: /* Return  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1626 "parser.cpp" /* yacc.c:1257  */
      break;

    case 130: /* IncrementStatement  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1632 "parser.cpp" /* yacc.c:1257  */
      break;

    case 133: /* PushPortCall  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1638 "parser.cpp" /* yacc.c:1257  */
      break;

    case 134: /* Indexession  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1644 "parser.cpp" /* yacc.c:1257  */
      break;

    case 139: /* ExpressionStatement  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1650 "parser.cpp" /* yacc.c:1257  */
      break;

    case 140: /* VarDecl  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1656 "parser.cpp" /* yacc.c:1257  */
      break;

    case 141: /* AssignmentStatement  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1662 "parser.cpp" /* yacc.c:1257  */
      break;

    case 142: /* If  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1668 "parser.cpp" /* yacc.c:1257  */
      break;

    case 143: /* While  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1674 "parser.cpp" /* yacc.c:1257  */
      break;

    case 145: /* TypeLitExpression  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1680 "parser.cpp" /* yacc.c:1257  */
      break;

    case 146: /* Array  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1686 "parser.cpp" /* yacc.c:1257  */
      break;

    case 147: /* ElementType  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1692 "parser.cpp" /* yacc.c:1257  */
      break;

    case 148: /* StructType  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1698 "parser.cpp" /* yacc.c:1257  */
      break;

    case 149: /* Slice  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1704 "parser.cpp" /* yacc.c:1257  */
      break;

    case 150: /* Map  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1710 "parser.cpp" /* yacc.c:1257  */
      break;

    case 151: /* KeyType  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1716 "parser.cpp" /* yacc.c:1257  */
      break;

    case 152: /* ComponentType  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1722 "parser.cpp" /* yacc.c:1257  */
      break;

    case 153: /* PushPort  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1728 "parser.cpp" /* yacc.c:1257  */
      break;

    case 154: /* PullPort  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1734 "parser.cpp" /* yacc.c:1257  */
      break;

    case 155: /* Heap  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1740 "parser.cpp" /* yacc.c:1257  */
      break;

    case 156: /* Pointer  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1746 "parser.cpp" /* yacc.c:1257  */
      break;

    case 157: /* TypeLit  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1752 "parser.cpp" /* yacc.c:1257  */
      break;

    case 158: /* TypeName  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1758 "parser.cpp" /* yacc.c:1257  */
      break;

    case 159: /* Type  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1764 "parser.cpp" /* yacc.c:1257  */
      break;

    case 160: /* ArrayDimension  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1770 "parser.cpp" /* yacc.c:1257  */
      break;

    case 162: /* OptionalExpression  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1776 "parser.cpp" /* yacc.c:1257  */
      break;

    case 163: /* Expression  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1782 "parser.cpp" /* yacc.c:1257  */
      break;

    case 164: /* OrExpression  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1788 "parser.cpp" /* yacc.c:1257  */
      break;

    case 165: /* AndExpression  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1794 "parser.cpp" /* yacc.c:1257  */
      break;

    case 166: /* CompareExpression  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1800 "parser.cpp" /* yacc.c:1257  */
      break;

    case 167: /* AddExpression  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1806 "parser.cpp" /* yacc.c:1257  */
      break;

    case 168: /* MultiplyExpression  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1812 "parser.cpp" /* yacc.c:1257  */
      break;

    case 169: /* UnaryExpression  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1818 "parser.cpp" /* yacc.c:1257  */
      break;

    case 170: /* PrimaryExpression  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1824 "parser.cpp" /* yacc.c:1257  */
      break;

    case 171: /* Literal  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1830 "parser.cpp" /* yacc.c:1257  */
      break;

    case 174: /* Element  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1836 "parser.cpp" /* yacc.c:1257  */
      break;

    case 175: /* Key  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1842 "parser.cpp" /* yacc.c:1257  */
      break;

    case 176: /* Value  */
#line 96 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1848 "parser.cpp" /* yacc.c:1257  */
      break;


    default:
      break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
    ;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
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
      yychar = yylex ();
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
    case 2:
#line 170 "parser.y" /* yacc.c:1646  */
      {
        root = (yyvsp[0].node);
      }
#line 2136 "parser.cpp" /* yacc.c:1646  */
      break;

    case 3:
#line 173 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new SourceFile ((yylsp[-3]), (yyvsp[-3].identifier), (yyvsp[-1].list), (yyvsp[0].list));
      }
#line 2142 "parser.cpp" /* yacc.c:1646  */
      break;

    case 4:
#line 175 "parser.y" /* yacc.c:1646  */
      {
        (yyval.identifier) = (yyvsp[0].identifier);
      }
#line 2148 "parser.cpp" /* yacc.c:1646  */
      break;

    case 5:
#line 177 "parser.y" /* yacc.c:1646  */
      {
        (yyval.identifier) = (yyvsp[0].identifier);
      }
#line 2154 "parser.cpp" /* yacc.c:1646  */
      break;

    case 6:
#line 180 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ImportDeclList (yyloc);
      }
#line 2160 "parser.cpp" /* yacc.c:1646  */
      break;

    case 7:
#line 181 "parser.y" /* yacc.c:1646  */
      {
        UNIMPLEMENTED;
      }
#line 2166 "parser.cpp" /* yacc.c:1646  */
      break;

    case 8:
#line 184 "parser.y" /* yacc.c:1646  */
      {
        UNIMPLEMENTED;
      }
#line 2172 "parser.cpp" /* yacc.c:1646  */
      break;

    case 9:
#line 185 "parser.y" /* yacc.c:1646  */
      {
        UNIMPLEMENTED;
      }
#line 2178 "parser.cpp" /* yacc.c:1646  */
      break;

    case 10:
#line 188 "parser.y" /* yacc.c:1646  */
      {
        UNIMPLEMENTED;
      }
#line 2184 "parser.cpp" /* yacc.c:1646  */
      break;

    case 11:
#line 189 "parser.y" /* yacc.c:1646  */
      {
        UNIMPLEMENTED;
      }
#line 2190 "parser.cpp" /* yacc.c:1646  */
      break;

    case 12:
#line 192 "parser.y" /* yacc.c:1646  */
      {
        UNIMPLEMENTED;
      }
#line 2196 "parser.cpp" /* yacc.c:1646  */
      break;

    case 13:
#line 193 "parser.y" /* yacc.c:1646  */
      {
        UNIMPLEMENTED;
      }
#line 2202 "parser.cpp" /* yacc.c:1646  */
      break;

    case 14:
#line 194 "parser.y" /* yacc.c:1646  */
      {
        UNIMPLEMENTED;
      }
#line 2208 "parser.cpp" /* yacc.c:1646  */
      break;

    case 15:
#line 196 "parser.y" /* yacc.c:1646  */
      {
        UNIMPLEMENTED;
      }
#line 2214 "parser.cpp" /* yacc.c:1646  */
      break;

    case 16:
#line 199 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new TopLevelDeclList (yyloc);
      }
#line 2220 "parser.cpp" /* yacc.c:1646  */
      break;

    case 17:
#line 200 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[-1].node));
      }
#line 2226 "parser.cpp" /* yacc.c:1646  */
      break;

    case 18:
#line 204 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2232 "parser.cpp" /* yacc.c:1646  */
      break;

    case 19:
#line 205 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2238 "parser.cpp" /* yacc.c:1646  */
      break;

    case 20:
#line 206 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2244 "parser.cpp" /* yacc.c:1646  */
      break;

    case 21:
#line 207 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2250 "parser.cpp" /* yacc.c:1646  */
      break;

    case 22:
#line 208 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2256 "parser.cpp" /* yacc.c:1646  */
      break;

    case 23:
#line 209 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2262 "parser.cpp" /* yacc.c:1646  */
      break;

    case 24:
#line 210 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2268 "parser.cpp" /* yacc.c:1646  */
      break;

    case 25:
#line 211 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2274 "parser.cpp" /* yacc.c:1646  */
      break;

    case 26:
#line 212 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2280 "parser.cpp" /* yacc.c:1646  */
      break;

    case 27:
#line 213 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2286 "parser.cpp" /* yacc.c:1646  */
      break;

    case 28:
#line 217 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ConstDecl ((yylsp[-4]), (yyvsp[-3].list), (yyvsp[-2].node), (yyvsp[0].list));
      }
#line 2292 "parser.cpp" /* yacc.c:1646  */
      break;

    case 29:
#line 219 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ConstDecl ((yylsp[-3]), (yyvsp[-2].list), new EmptyType ((yylsp[-3])), (yyvsp[0].list));
      }
#line 2298 "parser.cpp" /* yacc.c:1646  */
      break;

    case 30:
#line 223 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new InstanceDecl ((yylsp[-6]), (yyvsp[-5].identifier), (yyvsp[-4].node), (yyvsp[-3].identifier), (yyvsp[-1].list));
      }
#line 2304 "parser.cpp" /* yacc.c:1646  */
      break;

    case 31:
#line 225 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::TypeDecl ((yylsp[-2]), (yyvsp[-1].identifier), (yyvsp[0].node));
      }
#line 2310 "parser.cpp" /* yacc.c:1646  */
      break;

    case 32:
#line 228 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Mutable;
      }
#line 2316 "parser.cpp" /* yacc.c:1646  */
      break;

    case 33:
#line 229 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Immutable;
      }
#line 2322 "parser.cpp" /* yacc.c:1646  */
      break;

    case 34:
#line 230 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Foreign;
      }
#line 2328 "parser.cpp" /* yacc.c:1646  */
      break;

    case 35:
#line 233 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Mutable;
      }
#line 2334 "parser.cpp" /* yacc.c:1646  */
      break;

    case 36:
#line 234 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Immutable;
      }
#line 2340 "parser.cpp" /* yacc.c:1646  */
      break;

    case 37:
#line 235 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Foreign;
      }
#line 2346 "parser.cpp" /* yacc.c:1646  */
      break;

    case 38:
#line 239 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Receiver ((yylsp[-6]), (yyvsp[-5].identifier), (yyvsp[-4].mutability), (yyvsp[-3].mutability), true, (yyvsp[-1].identifier));
      }
#line 2352 "parser.cpp" /* yacc.c:1646  */
      break;

    case 39:
#line 241 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Receiver ((yylsp[-5]), (yyvsp[-4].identifier), (yyvsp[-3].mutability), (yyvsp[-2].mutability), false, (yyvsp[-1].identifier));
      }
#line 2358 "parser.cpp" /* yacc.c:1646  */
      break;

    case 40:
#line 245 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ActionDecl ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2364 "parser.cpp" /* yacc.c:1646  */
      break;

    case 41:
#line 247 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new DimensionedActionDecl ((yylsp[-7]), (yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2370 "parser.cpp" /* yacc.c:1646  */
      break;

    case 42:
#line 251 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ReactionDecl ((yylsp[-4]), (yyvsp[-3].node), (yyvsp[-2].identifier), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2376 "parser.cpp" /* yacc.c:1646  */
      break;

    case 43:
#line 253 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new DimensionedReactionDecl ((yylsp[-4]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].identifier), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2382 "parser.cpp" /* yacc.c:1646  */
      break;

    case 44:
#line 257 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new BindDecl ((yylsp[-3]), (yyvsp[-2].node), (yyvsp[-1].identifier), (yyvsp[0].node));
      }
#line 2388 "parser.cpp" /* yacc.c:1646  */
      break;

    case 45:
#line 261 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new InitDecl ((yylsp[-5]), (yyvsp[-4].node), (yyvsp[-3].identifier), (yyvsp[-2].list), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2394 "parser.cpp" /* yacc.c:1646  */
      break;

    case 46:
#line 265 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new GetterDecl ((yylsp[-5]), (yyvsp[-4].node), (yyvsp[-3].identifier), (yyvsp[-2].list), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2400 "parser.cpp" /* yacc.c:1646  */
      break;

    case 47:
#line 269 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new MethodDecl ((yylsp[-5]), (yyvsp[-4].node), (yyvsp[-3].identifier), (yyvsp[-2].list), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2406 "parser.cpp" /* yacc.c:1646  */
      break;

    case 48:
#line 273 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new FunctionDecl ((yylsp[-4]), (yyvsp[-3].identifier), (yyvsp[-2].list), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2412 "parser.cpp" /* yacc.c:1646  */
      break;

    case 49:
#line 277 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ParameterList (yyloc);
      }
#line 2418 "parser.cpp" /* yacc.c:1646  */
      break;

    case 50:
#line 279 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list);
      }
#line 2424 "parser.cpp" /* yacc.c:1646  */
      break;

    case 51:
#line 283 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ParameterList ((yylsp[0])))->append ((yyvsp[0].node));
      }
#line 2430 "parser.cpp" /* yacc.c:1646  */
      break;

    case 52:
#line 285 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node));
      }
#line 2436 "parser.cpp" /* yacc.c:1646  */
      break;

    case 53:
#line 289 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new VariableList ((yylsp[-3]), (yyvsp[-3].list), (yyvsp[-2].mutability), (yyvsp[-1].mutability), (yyvsp[0].node));
      }
#line 2442 "parser.cpp" /* yacc.c:1646  */
      break;

    case 54:
#line 293 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ParameterList (yyloc);
      }
#line 2448 "parser.cpp" /* yacc.c:1646  */
      break;

    case 55:
#line 295 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ParameterList (yyloc))->append (new VariableList ((yylsp[0]), (new IdentifierList ((yylsp[0])))->append (new Identifier ((yylsp[0]), "")), Mutable, Mutable, (yyvsp[0].node)));
      }
#line 2454 "parser.cpp" /* yacc.c:1646  */
      break;

    case 56:
#line 297 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ParameterList (yyloc))->append (new VariableList ((yylsp[-2]), (new IdentifierList ((yylsp[-2])))->append (new Identifier ((yylsp[-2]), "")), Mutable, Immutable, (yyvsp[0].node)));
      }
#line 2460 "parser.cpp" /* yacc.c:1646  */
      break;

    case 57:
#line 299 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ParameterList (yyloc))->append (new VariableList ((yylsp[-2]), (new IdentifierList ((yylsp[-2])))->append (new Identifier ((yylsp[-2]), "")), Mutable, Foreign, (yyvsp[0].node)));
      }
#line 2466 "parser.cpp" /* yacc.c:1646  */
      break;

    case 60:
#line 306 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new BindPushPort ((yylsp[-2]), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2472 "parser.cpp" /* yacc.c:1646  */
      break;

    case 61:
#line 308 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new BindPushPortParameter ((yylsp[-4]), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2478 "parser.cpp" /* yacc.c:1646  */
      break;

    case 62:
#line 310 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new BindPullPort ((yylsp[-2]), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2484 "parser.cpp" /* yacc.c:1646  */
      break;

    case 63:
#line 312 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].list);
      }
#line 2490 "parser.cpp" /* yacc.c:1646  */
      break;

    case 64:
#line 315 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new StatementList (yyloc);
      }
#line 2496 "parser.cpp" /* yacc.c:1646  */
      break;

    case 65:
#line 316 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[-1].node));
      }
#line 2502 "parser.cpp" /* yacc.c:1646  */
      break;

    case 66:
#line 319 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2508 "parser.cpp" /* yacc.c:1646  */
      break;

    case 67:
#line 320 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2514 "parser.cpp" /* yacc.c:1646  */
      break;

    case 68:
#line 321 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2520 "parser.cpp" /* yacc.c:1646  */
      break;

    case 69:
#line 322 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2526 "parser.cpp" /* yacc.c:1646  */
      break;

    case 70:
#line 323 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2532 "parser.cpp" /* yacc.c:1646  */
      break;

    case 71:
#line 324 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2538 "parser.cpp" /* yacc.c:1646  */
      break;

    case 72:
#line 325 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2544 "parser.cpp" /* yacc.c:1646  */
      break;

    case 73:
#line 326 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2550 "parser.cpp" /* yacc.c:1646  */
      break;

    case 74:
#line 327 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2556 "parser.cpp" /* yacc.c:1646  */
      break;

    case 75:
#line 328 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2562 "parser.cpp" /* yacc.c:1646  */
      break;

    case 76:
#line 329 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2568 "parser.cpp" /* yacc.c:1646  */
      break;

    case 77:
#line 330 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2574 "parser.cpp" /* yacc.c:1646  */
      break;

    case 78:
#line 333 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2580 "parser.cpp" /* yacc.c:1646  */
      break;

    case 79:
#line 334 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2586 "parser.cpp" /* yacc.c:1646  */
      break;

    case 80:
#line 335 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2592 "parser.cpp" /* yacc.c:1646  */
      break;

    case 81:
#line 336 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2598 "parser.cpp" /* yacc.c:1646  */
      break;

    case 82:
#line 338 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new EmptyStatement (yyloc);
      }
#line 2604 "parser.cpp" /* yacc.c:1646  */
      break;

    case 83:
#line 342 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Activate ((yylsp[-2]), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2610 "parser.cpp" /* yacc.c:1646  */
      break;

    case 84:
#line 346 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Change ((yylsp[-6]), (yyvsp[-4].node), (yyvsp[-2].identifier), (yyvsp[0].node));
      }
#line 2616 "parser.cpp" /* yacc.c:1646  */
      break;

    case 85:
#line 350 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ForIota ((yylsp[-4]), (yyvsp[-3].identifier), (yyvsp[-1].node), (yyvsp[0].node));
      }
#line 2622 "parser.cpp" /* yacc.c:1646  */
      break;

    case 86:
#line 354 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Return ((yylsp[-1]), (yyvsp[0].node));
      }
#line 2628 "parser.cpp" /* yacc.c:1646  */
      break;

    case 87:
#line 358 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IncrementDecrement ((yylsp[-1]), (yyvsp[-1].node), IncrementDecrement::Increment);
      }
#line 2634 "parser.cpp" /* yacc.c:1646  */
      break;

    case 88:
#line 360 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IncrementDecrement ((yylsp[-1]), (yyvsp[-1].node), IncrementDecrement::Decrement);
      }
#line 2640 "parser.cpp" /* yacc.c:1646  */
      break;

    case 89:
#line 364 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ExpressionList (yyloc);
      }
#line 2646 "parser.cpp" /* yacc.c:1646  */
      break;

    case 90:
#line 366 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[0].list);
      }
#line 2652 "parser.cpp" /* yacc.c:1646  */
      break;

    case 91:
#line 370 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ExpressionList ((yylsp[0])))->append ((yyvsp[0].node));
      }
#line 2658 "parser.cpp" /* yacc.c:1646  */
      break;

    case 92:
#line 372 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node));
      }
#line 2664 "parser.cpp" /* yacc.c:1646  */
      break;

    case 93:
#line 376 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IndexedPushPortCall ((yylsp[-4]), (yyvsp[-4].identifier), (yyvsp[-3].node), (yyvsp[-1].list));
      }
#line 2670 "parser.cpp" /* yacc.c:1646  */
      break;

    case 94:
#line 378 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new PushPortCall ((yylsp[-3]), (yyvsp[-3].identifier), (yyvsp[-1].list));
      }
#line 2676 "parser.cpp" /* yacc.c:1646  */
      break;

    case 95:
#line 380 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].node);
      }
#line 2682 "parser.cpp" /* yacc.c:1646  */
      break;

    case 96:
#line 383 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ExpressionList (yyloc);
      }
#line 2688 "parser.cpp" /* yacc.c:1646  */
      break;

    case 97:
#line 384 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[0].list);
      }
#line 2694 "parser.cpp" /* yacc.c:1646  */
      break;

    case 98:
#line 387 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ExpressionList (yyloc);
      }
#line 2700 "parser.cpp" /* yacc.c:1646  */
      break;

    case 99:
#line 388 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[0].list);
      }
#line 2706 "parser.cpp" /* yacc.c:1646  */
      break;

    case 100:
#line 392 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ExpressionList ((yylsp[0])))->append ((yyvsp[0].node));
      }
#line 2712 "parser.cpp" /* yacc.c:1646  */
      break;

    case 101:
#line 394 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node));
      }
#line 2718 "parser.cpp" /* yacc.c:1646  */
      break;

    case 102:
#line 398 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ExpressionList ((yylsp[0])))->append ((yyvsp[0].node));
      }
#line 2724 "parser.cpp" /* yacc.c:1646  */
      break;

    case 103:
#line 400 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ExpressionList ((yylsp[0])))->append (new TypeExpression ((yylsp[0]), (yyvsp[0].node)));
      }
#line 2730 "parser.cpp" /* yacc.c:1646  */
      break;

    case 104:
#line 402 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node));
      }
#line 2736 "parser.cpp" /* yacc.c:1646  */
      break;

    case 105:
#line 404 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append (new TypeExpression ((yylsp[-2]), (yyvsp[0].node)));
      }
#line 2742 "parser.cpp" /* yacc.c:1646  */
      break;

    case 106:
#line 408 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ExpressionStatement ((yylsp[0]), (yyvsp[0].node));
      }
#line 2748 "parser.cpp" /* yacc.c:1646  */
      break;

    case 107:
#line 412 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Var ((yylsp[-6]), (yyvsp[-5].list), (yyvsp[-4].mutability), (yyvsp[-3].mutability), (yyvsp[-2].node), (yyvsp[0].list));
      }
#line 2754 "parser.cpp" /* yacc.c:1646  */
      break;

    case 108:
#line 414 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Var ((yylsp[-4]), (yyvsp[-3].list), (yyvsp[-2].mutability), (yyvsp[-1].mutability), (yyvsp[0].node), new ExpressionList ((yylsp[-4])));
      }
#line 2760 "parser.cpp" /* yacc.c:1646  */
      break;

    case 109:
#line 416 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Var ((yylsp[-5]), (yyvsp[-4].list), (yyvsp[-3].mutability), (yyvsp[-2].mutability), new EmptyType ((yylsp[-5])), (yyvsp[0].list));
      }
#line 2766 "parser.cpp" /* yacc.c:1646  */
      break;

    case 110:
#line 418 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Var ((yylsp[-4]), (yyvsp[-4].list), (yyvsp[-3].mutability), (yyvsp[-2].mutability), new EmptyType ((yylsp[-4])), (yyvsp[0].list));
      }
#line 2772 "parser.cpp" /* yacc.c:1646  */
      break;

    case 111:
#line 422 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Assign ((yylsp[-2]), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2778 "parser.cpp" /* yacc.c:1646  */
      break;

    case 112:
#line 424 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new AddAssign ((yylsp[-2]), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2784 "parser.cpp" /* yacc.c:1646  */
      break;

    case 113:
#line 428 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-2]), new EmptyStatement ((yylsp[-2])), (yyvsp[-1].node), (yyvsp[0].node), new StatementList ((yylsp[-2])));
      }
#line 2790 "parser.cpp" /* yacc.c:1646  */
      break;

    case 114:
#line 430 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-4]), new EmptyStatement ((yylsp[-4])), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2796 "parser.cpp" /* yacc.c:1646  */
      break;

    case 115:
#line 432 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-4]), new EmptyStatement ((yylsp[-4])), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2802 "parser.cpp" /* yacc.c:1646  */
      break;

    case 116:
#line 434 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-4]), (yyvsp[-3].node), (yyvsp[-1].node), (yyvsp[0].node), new StatementList ((yylsp[-4])));
      }
#line 2808 "parser.cpp" /* yacc.c:1646  */
      break;

    case 117:
#line 436 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2814 "parser.cpp" /* yacc.c:1646  */
      break;

    case 118:
#line 438 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2820 "parser.cpp" /* yacc.c:1646  */
      break;

    case 119:
#line 442 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new While ((yylsp[-2]), (yyvsp[-1].node), (yyvsp[0].node));
      }
#line 2826 "parser.cpp" /* yacc.c:1646  */
      break;

    case 120:
#line 446 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new IdentifierList ((yylsp[0])))->append ((yyvsp[0].identifier));
      }
#line 2832 "parser.cpp" /* yacc.c:1646  */
      break;

    case 121:
#line 448 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].identifier));
      }
#line 2838 "parser.cpp" /* yacc.c:1646  */
      break;

    case 122:
#line 452 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2844 "parser.cpp" /* yacc.c:1646  */
      break;

    case 123:
#line 453 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2850 "parser.cpp" /* yacc.c:1646  */
      break;

    case 124:
#line 454 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2856 "parser.cpp" /* yacc.c:1646  */
      break;

    case 125:
#line 455 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2862 "parser.cpp" /* yacc.c:1646  */
      break;

    case 126:
#line 456 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2868 "parser.cpp" /* yacc.c:1646  */
      break;

    case 127:
#line 457 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2874 "parser.cpp" /* yacc.c:1646  */
      break;

    case 128:
#line 458 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2880 "parser.cpp" /* yacc.c:1646  */
      break;

    case 129:
#line 459 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2886 "parser.cpp" /* yacc.c:1646  */
      break;

    case 130:
#line 462 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Array ((yylsp[-1]), (yyvsp[-1].node), (yyvsp[0].node));
      }
#line 2892 "parser.cpp" /* yacc.c:1646  */
      break;

    case 131:
#line 464 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2898 "parser.cpp" /* yacc.c:1646  */
      break;

    case 132:
#line 466 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].list);
      }
#line 2904 "parser.cpp" /* yacc.c:1646  */
      break;

    case 133:
#line 468 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Slice ((yylsp[-2]), (yyvsp[0].node));
      }
#line 2910 "parser.cpp" /* yacc.c:1646  */
      break;

    case 134:
#line 470 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Map ((yylsp[-4]), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2916 "parser.cpp" /* yacc.c:1646  */
      break;

    case 135:
#line 472 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2922 "parser.cpp" /* yacc.c:1646  */
      break;

    case 136:
#line 476 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].list);
        static_cast<FieldList*> ((yyvsp[-1].list))->is_component = true;
      }
#line 2928 "parser.cpp" /* yacc.c:1646  */
      break;

    case 137:
#line 478 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new PushPort ((yylsp[-1]), (yyvsp[0].list));
      }
#line 2934 "parser.cpp" /* yacc.c:1646  */
      break;

    case 138:
#line 482 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new PullPort ((yylsp[-2]), (yyvsp[-1].list), (yyvsp[0].list));
      }
#line 2940 "parser.cpp" /* yacc.c:1646  */
      break;

    case 139:
#line 484 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Heap ((yylsp[-1]), (yyvsp[0].node));
      }
#line 2946 "parser.cpp" /* yacc.c:1646  */
      break;

    case 140:
#line 486 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Pointer ((yylsp[-1]), (yyvsp[0].node));
      }
#line 2952 "parser.cpp" /* yacc.c:1646  */
      break;

    case 141:
#line 489 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2958 "parser.cpp" /* yacc.c:1646  */
      break;

    case 142:
#line 490 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2964 "parser.cpp" /* yacc.c:1646  */
      break;

    case 143:
#line 492 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IdentifierType ((yylsp[0]), (yyvsp[0].identifier));
      }
#line 2970 "parser.cpp" /* yacc.c:1646  */
      break;

    case 144:
#line 495 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2976 "parser.cpp" /* yacc.c:1646  */
      break;

    case 145:
#line 496 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2982 "parser.cpp" /* yacc.c:1646  */
      break;

    case 146:
#line 497 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].node);
      }
#line 2988 "parser.cpp" /* yacc.c:1646  */
      break;

    case 147:
#line 499 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].node);
      }
#line 2994 "parser.cpp" /* yacc.c:1646  */
      break;

    case 148:
#line 503 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new FieldList (yyloc);
      }
#line 3000 "parser.cpp" /* yacc.c:1646  */
      break;

    case 149:
#line 505 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-3].list)->append (new VariableList ((yylsp[-3]), (yyvsp[-2].list), Mutable, Mutable, (yyvsp[-1].node)));
      }
#line 3006 "parser.cpp" /* yacc.c:1646  */
      break;

    case 150:
#line 509 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new EmptyExpression (yyloc);
      }
#line 3012 "parser.cpp" /* yacc.c:1646  */
      break;

    case 151:
#line 511 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3018 "parser.cpp" /* yacc.c:1646  */
      break;

    case 152:
#line 513 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3024 "parser.cpp" /* yacc.c:1646  */
      break;

    case 153:
#line 517 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3030 "parser.cpp" /* yacc.c:1646  */
      break;

    case 154:
#line 519 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::logic_or_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3036 "parser.cpp" /* yacc.c:1646  */
      break;

    case 155:
#line 523 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3042 "parser.cpp" /* yacc.c:1646  */
      break;

    case 156:
#line 525 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::logic_and_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3048 "parser.cpp" /* yacc.c:1646  */
      break;

    case 157:
#line 529 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3054 "parser.cpp" /* yacc.c:1646  */
      break;

    case 158:
#line 531 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::equal_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3060 "parser.cpp" /* yacc.c:1646  */
      break;

    case 159:
#line 533 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::not_equal_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3066 "parser.cpp" /* yacc.c:1646  */
      break;

    case 160:
#line 535 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::less_than_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3072 "parser.cpp" /* yacc.c:1646  */
      break;

    case 161:
#line 537 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::less_equal_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3078 "parser.cpp" /* yacc.c:1646  */
      break;

    case 162:
#line 539 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::more_than_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3084 "parser.cpp" /* yacc.c:1646  */
      break;

    case 163:
#line 541 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::more_equal_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3090 "parser.cpp" /* yacc.c:1646  */
      break;

    case 164:
#line 545 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3096 "parser.cpp" /* yacc.c:1646  */
      break;

    case 165:
#line 547 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::add_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3102 "parser.cpp" /* yacc.c:1646  */
      break;

    case 166:
#line 549 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::subtract_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3108 "parser.cpp" /* yacc.c:1646  */
      break;

    case 167:
#line 551 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::bit_or_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3114 "parser.cpp" /* yacc.c:1646  */
      break;

    case 168:
#line 553 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::bit_xor_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3120 "parser.cpp" /* yacc.c:1646  */
      break;

    case 169:
#line 557 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3126 "parser.cpp" /* yacc.c:1646  */
      break;

    case 170:
#line 559 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::multiply_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3132 "parser.cpp" /* yacc.c:1646  */
      break;

    case 171:
#line 561 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::divide_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3138 "parser.cpp" /* yacc.c:1646  */
      break;

    case 172:
#line 563 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::modulus_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3144 "parser.cpp" /* yacc.c:1646  */
      break;

    case 173:
#line 565 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::left_shift_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3150 "parser.cpp" /* yacc.c:1646  */
      break;

    case 174:
#line 567 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::right_shift_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3156 "parser.cpp" /* yacc.c:1646  */
      break;

    case 175:
#line 569 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::bit_and_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3162 "parser.cpp" /* yacc.c:1646  */
      break;

    case 176:
#line 571 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::bit_and_not_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3168 "parser.cpp" /* yacc.c:1646  */
      break;

    case 177:
#line 574 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3174 "parser.cpp" /* yacc.c:1646  */
      break;

    case 178:
#line 575 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::UnaryArithmetic ((yylsp[-1]), &semantic::posate_temp, (yyvsp[0].node));
      }
#line 3180 "parser.cpp" /* yacc.c:1646  */
      break;

    case 179:
#line 576 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::UnaryArithmetic ((yylsp[-1]), &semantic::negate_temp, (yyvsp[0].node));
      }
#line 3186 "parser.cpp" /* yacc.c:1646  */
      break;

    case 180:
#line 577 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::UnaryArithmetic ((yylsp[-1]), &semantic::logic_not_temp, (yyvsp[0].node));
      }
#line 3192 "parser.cpp" /* yacc.c:1646  */
      break;

    case 181:
#line 578 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::UnaryArithmetic ((yylsp[-1]), &semantic::complement_temp, (yyvsp[0].node));
      }
#line 3198 "parser.cpp" /* yacc.c:1646  */
      break;

    case 182:
#line 579 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Dereference ((yylsp[-1]), (yyvsp[0].node));
      }
#line 3204 "parser.cpp" /* yacc.c:1646  */
      break;

    case 183:
#line 580 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new AddressOf ((yylsp[-1]), (yyvsp[0].node));
      }
#line 3210 "parser.cpp" /* yacc.c:1646  */
      break;

    case 184:
#line 584 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3216 "parser.cpp" /* yacc.c:1646  */
      break;

    case 185:
#line 586 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new CompositeLiteral ((yylsp[-1]), (yyvsp[-1].node), (yyvsp[0].list));
      }
#line 3222 "parser.cpp" /* yacc.c:1646  */
      break;

    case 186:
#line 588 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new CompositeLiteral ((yylsp[-1]), new IdentifierType ((yylsp[-1]), (yyvsp[-1].identifier)), (yyvsp[0].list));
      }
#line 3228 "parser.cpp" /* yacc.c:1646  */
      break;

    case 187:
#line 590 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new CompositeLiteral ((yylsp[-4]), new Array ((yylsp[-4]), new EmptyExpression ((yylsp[-4])), (yyvsp[-1].node)), (yyvsp[0].list));
      }
#line 3234 "parser.cpp" /* yacc.c:1646  */
      break;

    case 188:
#line 593 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IdentifierExpression ((yylsp[0]), (yyvsp[0].identifier));
      }
#line 3240 "parser.cpp" /* yacc.c:1646  */
      break;

    case 189:
#line 595 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].node);
      }
#line 3246 "parser.cpp" /* yacc.c:1646  */
      break;

    case 190:
#line 597 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Select ((yylsp[-2]), (yyvsp[-2].node), (yyvsp[0].identifier));
      }
#line 3252 "parser.cpp" /* yacc.c:1646  */
      break;

    case 191:
#line 599 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Index ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node));
      }
#line 3258 "parser.cpp" /* yacc.c:1646  */
      break;

    case 192:
#line 601 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IndexSlice ((yylsp[-5]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node), new EmptyExpression (yyloc));
      }
#line 3264 "parser.cpp" /* yacc.c:1646  */
      break;

    case 193:
#line 603 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IndexSlice ((yylsp[-7]), (yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node));
      }
#line 3270 "parser.cpp" /* yacc.c:1646  */
      break;

    case 194:
#line 605 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Call ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].list));
      }
#line 3276 "parser.cpp" /* yacc.c:1646  */
      break;

    case 195:
#line 607 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Conversion ((yylsp[-3]), new TypeExpression ((yylsp[-3]), (yyvsp[-3].node)), (yyvsp[-1].node));
      }
#line 3282 "parser.cpp" /* yacc.c:1646  */
      break;

    case 196:
#line 610 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3288 "parser.cpp" /* yacc.c:1646  */
      break;

    case 197:
#line 611 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3294 "parser.cpp" /* yacc.c:1646  */
      break;

    case 198:
#line 614 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ElementList ((yylsp[-1]));
      }
#line 3300 "parser.cpp" /* yacc.c:1646  */
      break;

    case 199:
#line 615 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list);
      }
#line 3306 "parser.cpp" /* yacc.c:1646  */
      break;

    case 200:
#line 618 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ElementList ((yylsp[0])))->append ((yyvsp[0].node));
      }
#line 3312 "parser.cpp" /* yacc.c:1646  */
      break;

    case 201:
#line 619 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node));
      }
#line 3318 "parser.cpp" /* yacc.c:1646  */
      break;

    case 202:
#line 622 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Element ((yylsp[-2]), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3324 "parser.cpp" /* yacc.c:1646  */
      break;

    case 203:
#line 623 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Element ((yylsp[0]), new EmptyExpression ((yylsp[0])), (yyvsp[0].node));
      }
#line 3330 "parser.cpp" /* yacc.c:1646  */
      break;

    case 204:
#line 626 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3336 "parser.cpp" /* yacc.c:1646  */
      break;

    case 205:
#line 627 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].list);
      }
#line 3342 "parser.cpp" /* yacc.c:1646  */
      break;

    case 206:
#line 630 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3348 "parser.cpp" /* yacc.c:1646  */
      break;

    case 207:
#line 631 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].list);
      }
#line 3354 "parser.cpp" /* yacc.c:1646  */
      break;


#line 3358 "parser.cpp" /* yacc.c:1646  */
    default:
      break;
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
      yyerror (YY_("syntax error"));
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
        yyerror (yymsgp);
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
                      yytoken, &yylval, &yylloc);
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
                  yystos[yystate], yyvsp, yylsp);
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
  yyerror (YY_("memory exhausted"));
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
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp);
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
#line 637 "parser.y" /* yacc.c:1906  */

