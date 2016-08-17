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

union YYSTYPE
{
#line 11 "parser.y" /* yacc.c:355  */
  ast::Node* node;
  ast::List* list;
  ast::Identifier* identifier;
  Mutability mutability;

#line 182 "parser.cpp" /* yacc.c:355  */
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

#line 213 "parser.cpp" /* yacc.c:358  */

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   694

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  79
/* YYNRULES -- Number of rules.  */
#define YYNRULES  191
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  392

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   317

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
  0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
  2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
  2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
  2,     2,     2,    83,     2,     2,    68,    81,    82,     2,
  66,    67,    69,    76,    71,    77,    84,    80,     2,     2,
  2,     2,     2,     2,     2,     2,     2,     2,    85,    65,
  74,    64,    75,     2,     2,     2,     2,     2,     2,     2,
  2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
  2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
  2,    72,     2,    73,    79,     2,     2,     2,     2,     2,
  2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
  2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
  2,     2,     2,    63,    78,    70,     2,     2,     2,     2,
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
  55,    56,    57,    58,    59,    60,    61,    62
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
  0,   162,   162,   165,   166,   169,   170,   171,   172,   173,
  174,   175,   176,   177,   178,   181,   183,   187,   190,   193,
  194,   195,   198,   199,   200,   203,   205,   209,   211,   215,
  217,   221,   225,   229,   233,   237,   241,   243,   247,   249,
  253,   258,   259,   262,   263,   266,   268,   270,   273,   276,
  277,   280,   281,   282,   283,   284,   285,   286,   287,   288,
  289,   290,   293,   294,   295,   296,   298,   301,   305,   309,
  313,   317,   319,   324,   325,   329,   331,   335,   337,   340,
  343,   344,   347,   348,   351,   353,   357,   359,   361,   363,
  367,   371,   373,   375,   377,   381,   383,   387,   389,   391,
  393,   395,   397,   401,   405,   407,   412,   413,   414,   415,
  416,   417,   418,   419,   422,   424,   426,   428,   430,   432,
  435,   438,   441,   444,   446,   449,   450,   452,   455,   456,
  457,   459,   463,   464,   469,   470,   473,   476,   478,   482,
  484,   488,   490,   492,   494,   496,   498,   500,   504,   506,
  508,   510,   512,   516,   518,   520,   522,   524,   526,   528,
  530,   534,   535,   536,   537,   538,   539,   540,   543,   545,
  547,   549,   552,   554,   556,   558,   560,   562,   564,   566,
  570,   571,   574,   575,   578,   579,   582,   583,   586,   587,
  589,   591
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "LITERAL", "ACTION",
  "ACTIVATE", "BIND", "BREAK", "CASE", "CHANGE", "COMPONENT", "CONST",
  "CONTINUE", "DEFAULT", "ELSE", "FALLTHROUGH", "FOR", "FOREIGN", "FUNC",
  "GETTER", "GOTO", "HEAP", "IF", "INIT", "INSTANCE", "INTERFACE", "MAP",
  "PULL", "PUSH", "RANGE", "REACTION", "RETURN", "STRUCT", "SWITCH",
  "TYPE", "VAR", "ADD_ASSIGN", "AND_ASSIGN", "AND_NOT", "AND_NOT_ASSIGN",
  "DECREMENT", "DIVIDE_ASSIGN", "DOTDOTDOT", "EQUAL", "INCREMENT",
  "LEFT_ARROW", "LEFT_SHIFT", "LEFT_SHIFT_ASSIGN", "LESS_EQUAL",
  "LOGIC_AND", "LOGIC_OR", "MODULUS_ASSIGN", "MORE_EQUAL",
  "MULTIPLY_ASSIGN", "NOT_EQUAL", "OR_ASSIGN", "RIGHT_ARROW",
  "RIGHT_SHIFT", "RIGHT_SHIFT_ASSIGN", "SHORT_ASSIGN", "SUBTRACT_ASSIGN",
  "XOR_ASSIGN", "'{'", "'='", "';'", "'('", "')'", "'$'", "'*'", "'}'",
  "','", "'['", "']'", "'<'", "'>'", "'+'", "'-'", "'|'", "'^'", "'/'",
  "'%'", "'&'", "'!'", "'.'", "':'", "$accept", "Top", "DefinitionList",
  "Definition", "Const", "Instance", "TypeDecl", "Mutability",
  "DereferenceMutability", "Receiver", "Action", "Reaction", "Bind",
  "Init", "Getter", "Method", "Func", "ParameterList",
  "ParameterListInternal", "Parameter", "ReturnParameterList",
  "optional_semicolon", "BindStatement", "Block", "StatementList",
  "Statement", "SimpleStatement", "EmptyStatement", "Activate", "Change",
  "ForIota", "Return", "IncrementStatement", "OptionalPushPortCallList",
  "PushPortCallList", "PushPortCall", "Indexession",
  "OptionalExpressionList", "OptionalTypeOrExpressionList",
  "ExpressionList", "TypeOrExpressionList", "ExpressionStatement", "Var",
  "AssignmentStatement", "If", "While", "IdentifierList",
  "TypeLitExpression", "Array", "ElementType", "StructType", "Slice",
  "Map", "KeyType", "ComponentType", "PushPort", "PullPort", "Heap",
  "Pointer", "TypeLit", "TypeName", "Type", "ArrayDimension", "FieldList",
  "OptionalExpression", "Expression", "OrExpression", "AndExpression",
  "CompareExpression", "AddExpression", "MultiplyExpression",
  "UnaryExpression", "PrimaryExpression", "LiteralValue", "ElementList",
  "Element", "Key", "Value", "OptionalComma", YY_NULLPTR
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
  315,   316,   317,   123,    61,    59,    40,    41,    36,    42,
  125,    44,    91,    93,    60,    62,    43,    45,   124,    94,
  47,    37,    38,    33,    46,    58
};
# endif

#define YYPACT_NINF -341

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-341)))

#define YYTABLE_NINF -188

#define yytable_value_is_error(Yytable_value) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
  -341,    30,    92,  -341,   -53,   -53,    35,    29,   -53,   -53,
  36,   -53,    44,   504,  -341,  -341,  -341,  -341,  -341,  -341,
  -341,  -341,  -341,  -341,  -341,    31,    45,    46,    53,  -341,
  204,    -1,    60,    70,    84,    91,    95,   622,  -341,  -341,
  42,   622,    37,    -1,    -1,    55,   504,   504,   378,   504,
  504,   504,   504,   504,     9,  -341,  -341,  -341,  -341,  -341,
  -341,  -341,  -341,   622,    58,    85,    69,   138,   205,    68,
  -341,   -20,   -53,   -53,   110,    79,    83,  -341,   504,   622,
  622,   144,   286,  -341,  -341,  -341,  -341,   103,    19,    66,
  -1,    -1,    -1,   153,    -1,    94,   405,  -341,  -341,  -341,
  622,    90,  -341,  -341,   112,  -341,   104,   622,  -341,  -341,
  -341,  -341,  -341,   504,  -341,  -341,  -341,  -341,   504,   504,
  504,   504,   504,   504,   504,   504,   504,   504,   504,   504,
  504,   504,   504,   504,   504,   504,   504,   504,   504,   165,
  178,   186,  -341,  -341,   128,   504,  -341,  -341,   101,  -341,
  147,  -341,  -341,   504,  -341,   134,  -341,    25,   172,   622,
  83,    66,    66,    66,   152,    83,  -341,  -341,   149,   157,
  176,  -341,   163,  -341,     6,   181,  -341,  -341,     7,  -341,
  622,  -341,   182,    69,   138,   205,   205,   205,   205,   205,
  205,    68,    68,    68,    68,  -341,  -341,  -341,  -341,  -341,
  -341,  -341,   188,   187,     9,  -341,   174,   184,  -341,   201,
  -1,    13,   202,   314,  -341,   504,  -341,   123,    35,   207,
  128,  -341,  -341,  -341,  -341,    83,    83,    83,   504,  -341,
  437,   216,   437,  -341,   563,   622,  -341,   208,  -341,  -341,
  504,   504,  -341,   504,    83,   210,   269,    83,   102,   282,
  222,   536,   469,   504,    35,  -341,  -341,  -341,  -341,  -341,
  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,
  -341,  -341,  -341,    25,   179,  -341,  -341,  -341,  -341,   622,
  -341,  -341,  -341,   224,   221,  -341,  -341,  -341,  -341,  -341,
  229,  -341,  -341,     9,  -341,   223,   213,   228,  -341,  -341,
  232,  -341,   129,    83,   236,  -341,   504,   257,    83,   238,
  215,   246,    25,   128,   504,   262,   263,   504,   504,   504,
  -341,  -341,   265,  -341,  -341,   504,    83,  -341,   504,   504,
  243,  -341,   282,   251,   504,  -341,   504,   318,  -341,   128,
  274,   270,  -341,  -341,   273,    41,   275,  -341,   266,  -341,
  277,   272,   504,  -341,   345,    83,    83,    22,   595,   504,
  -341,  -341,   504,  -341,  -341,  -341,  -341,  -341,   284,   287,
  -341,   338,  -341,  -341,   504,   -36,   132,   291,  -341,    83,
  22,   133,   504,  -341,  -341,  -341,  -341,  -341,  -341,  -341,
  164,  -341
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
  3,     0,     2,     1,     0,     0,     0,     0,     0,     0,
  0,     0,     0,     0,     4,    14,    11,     5,     8,     9,
  10,     6,     7,    12,    13,     0,     0,     0,     0,   104,
  0,     0,     0,     0,     0,     0,     0,     0,   172,   168,
  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  0,     0,     0,     0,     0,   106,   107,   108,   109,   110,
  111,   112,   113,     0,     0,   136,   137,   139,   141,   148,
  153,   161,     0,     0,    19,     0,     0,   127,     0,     0,
  0,     0,     0,   126,   125,   129,   128,     0,     0,    22,
  0,     0,     0,     0,     0,     0,     0,   170,   132,   123,
  0,    22,   121,   132,     0,   166,     0,     0,   162,   163,
  165,   167,   164,     0,   169,   114,   115,   131,     0,     0,
  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  0,     0,     0,     0,     0,     0,     0,    82,   134,     0,
  0,     0,    20,    21,    22,     0,    49,    31,     0,    84,
  0,   124,   105,     0,    36,    43,    38,    19,     0,     0,
  0,    22,    22,    22,     0,     0,    18,   180,   188,   189,
  190,   182,     0,   185,     0,     0,   119,   122,     0,   173,
  0,   117,     0,   138,   140,   142,   145,   147,   143,   144,
  146,   149,   150,   151,   152,   160,   157,   158,   154,   155,
  156,   159,     0,    83,    87,    86,     0,   135,   174,     0,
  0,     0,     0,     0,    16,     0,   130,     0,    44,     0,
  22,    23,    24,    42,    35,     0,     0,     0,    80,    29,
  191,     0,     0,   120,     0,     0,   116,     0,   179,   178,
  0,   134,   175,     0,     0,     0,     0,     0,   172,    73,
  0,     0,     0,     0,     0,    66,    48,    61,    60,    54,
  50,    51,    62,    53,    58,    59,    55,    64,    63,    52,
  65,    56,    57,    19,     0,    85,    15,    39,    37,     0,
  34,    33,    32,     0,    81,   183,   181,   188,   189,   184,
  0,   118,   171,    89,    88,     0,   135,     0,    30,    26,
  0,    27,     0,     0,    74,    75,     0,   172,     0,     0,
  0,     0,    19,    22,     0,     0,     0,     0,     0,     0,
  90,    40,     0,   133,   176,     0,     0,    25,    80,     0,
  0,    67,     0,     0,     0,   103,     0,    97,    70,    22,
  0,     0,    72,    71,     0,     0,     0,    17,     0,    28,
  0,     0,    80,    76,     0,     0,     0,     0,     0,     0,
  96,    47,     0,    45,    95,   177,    78,    79,     0,     0,
  69,   100,    99,    98,     0,     0,     0,     0,    77,     0,
  0,     0,     0,    92,    94,    46,    68,   102,   101,    93,
  0,    91
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
  -341,  -341,  -341,  -341,   148,  -341,  -341,  -152,  -130,    16,
  -341,  -341,  -341,  -341,  -341,  -341,  -341,   -24,  -341,   139,
  -60,  -341,  -341,  -134,  -341,  -341,   108,  -341,  -341,  -341,
  -341,  -341,  -341,  -341,  -341,    32,  -341,  -310,  -341,   -72,
  -341,  -341,  -341,  -341,  -340,  -341,    -3,   -29,  -341,  -100,
  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,
  331,   -26,   365,   271,   135,   -13,  -341,   260,   256,   141,
  244,     8,  -341,   -52,  -341,   155,  -341,   156,  -341
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
  -1,     1,     2,    14,    15,    16,    17,   144,   159,    27,
  18,    19,    20,    21,    22,    23,    24,    89,   155,   156,
  160,   219,   258,   147,   213,   260,   261,   262,   263,   264,
  265,   266,   267,   303,   304,   305,   330,   283,   202,   284,
  203,   268,   269,   270,   271,   272,   157,    54,    55,   115,
  56,    57,    58,   175,    59,    60,    61,    62,    84,    85,
  86,   116,    63,   174,   206,   149,    65,    66,    67,    68,
  69,    70,    71,    97,   170,   171,   172,   173,   231
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
  64,    83,   114,    30,    87,   220,   148,   181,    83,    29,
  29,    95,    83,    26,   211,    99,   245,   373,   350,   101,
  102,    28,    29,    32,    33,    34,   224,    36,   382,   383,
  3,   229,    31,   104,    83,    64,    72,   142,    29,    35,
  388,   177,   368,   143,   169,   252,   137,    37,    74,    75,
  83,    83,   138,   150,   151,   105,    76,   108,   109,   110,
  111,   112,    73,    90,   139,    88,   161,   162,   163,    64,
  165,    83,    96,    91,   176,   113,   233,   236,    83,   259,
  237,   217,   246,   168,   362,   146,   154,    92,   140,   141,
  279,   280,   281,   282,    77,    26,    81,     4,    94,     5,
  182,   225,   226,   227,     6,    98,   363,   130,   204,   100,
  298,     7,     8,   301,  -104,   131,     9,    10,   103,   119,
  -104,   313,   142,    11,   205,   207,   132,    12,   143,   -41,
  83,   117,   212,   223,   158,   291,   118,   133,   195,   196,
  197,   198,   199,   200,   201,   145,   146,   152,   134,   135,
  136,    83,   114,   -41,   -41,   -41,   164,   -41,   158,   166,
  339,   -41,  -104,   -41,    13,    96,   214,   153,   208,   331,
  -104,   234,   215,  -104,   335,   234,   337,   180,   169,   179,
  288,   209,   120,   340,   221,   292,   244,   121,   276,   210,
  222,   122,   349,   123,   215,   328,   158,   384,   389,   218,
  274,   329,   275,   215,   215,    83,    83,    77,   290,   358,
  273,   293,   124,   125,   216,    40,   314,   168,   228,   287,
  315,   370,   371,   372,   316,   317,    41,   294,   296,   391,
  297,    42,    43,    44,  -186,   215,   318,    45,   308,   310,
  311,   114,  -187,   319,   320,   386,   387,   230,   232,   238,
  83,   312,   314,   321,   235,   239,   315,   242,   240,   241,
  316,   185,   186,   187,   188,   189,   190,   243,    78,   247,
  79,    96,   300,    80,   278,    81,    82,   299,   146,   319,
  320,   126,   127,   128,   129,   302,   286,   376,   306,    38,
  39,   322,   215,   333,   323,   326,   324,    40,   325,   327,
  334,   341,   381,   336,   344,   345,   346,   332,    41,   352,
  390,   338,   348,    42,    43,    44,   351,   248,    39,    45,
  249,   355,   354,   356,   250,    40,     6,   342,   343,    83,
  347,   251,   375,   357,   359,   360,    41,   252,   361,   365,
  364,    42,    43,    44,   366,   367,   253,    45,   369,   377,
  254,   378,    46,   380,   379,    47,   385,   277,    48,   107,
  309,   257,    49,    50,   353,    51,    93,    25,    52,    53,
  191,   192,   193,   194,   178,   184,   295,   146,   183,   255,
  46,    38,    39,    47,   256,   285,    48,     0,   289,    40,
  49,    50,     0,    51,     0,     0,    52,    53,     0,     0,
  41,     0,     0,     0,     0,    42,    43,    44,    38,    39,
  0,    45,     0,     0,     0,     0,    40,     0,     0,     0,
  0,   106,     0,     0,     0,     0,     0,    41,     0,     0,
  0,     0,    42,    43,    44,     0,     0,     0,    45,     0,
  38,    39,     0,     0,    46,     0,     0,    47,    40,     0,
  48,   107,     0,     0,    49,    50,     0,    51,     0,    41,
  52,    53,     0,     0,    42,    43,    44,     0,    96,     0,
  45,    46,    38,    39,    47,   167,     0,    48,     0,     0,
  40,    49,    50,     0,    51,     0,     0,    52,    53,     0,
  0,    41,     0,     0,     0,     0,    42,    43,    44,     0,
  96,     0,    45,    46,     0,     0,    47,    38,    39,    48,
  0,     0,     0,    49,    50,    40,    51,     0,     0,    52,
  53,     0,     0,     0,     0,     0,    41,     0,     0,     0,
  0,    42,    43,    44,   255,    46,     0,    45,    47,   307,
  39,    48,     0,     0,     0,    49,    50,    40,    51,     0,
  0,    52,    53,     0,     0,     0,     0,     0,    41,     0,
  0,     0,     0,    42,    43,    44,    77,     0,     0,    45,
  46,     0,     0,    47,    40,     0,    48,     0,     0,     0,
  49,    50,     0,    51,     0,    41,    52,    53,     0,     0,
  42,    43,    44,     0,     0,     0,    45,     0,    77,     0,
  0,     0,    46,     0,     0,    47,    40,     0,    48,     0,
  0,     0,    49,    50,     0,    51,     0,    41,    52,    53,
  0,     0,    42,    43,    44,    77,     0,     0,    45,    79,
  0,     0,    80,    40,    81,    82,     0,     0,     0,     0,
  0,     0,     0,     0,    41,     0,     0,     0,     0,    42,
  43,    44,     0,     0,     0,    45,     0,     0,     0,   374,
  0,    79,     0,     0,    80,     0,     0,    82,     0,     0,
  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  0,     0,     0,     0,     0,     0,     0,     0,    79,     0,
  0,    80,     0,     0,    82
};

static const yytype_int16 yycheck[] =
{
  13,    30,    54,     6,    30,   157,    78,   107,    37,     3,
  3,    37,    41,    66,   144,    41,     3,   357,   328,    43,
  44,     5,     3,     7,     8,     9,   160,    11,    64,    65,
  0,   165,     3,    46,    63,    48,     5,    12,     3,     3,
  380,   101,   352,    18,    96,    23,    66,     3,     3,     3,
  79,    80,    72,    79,    80,    47,     3,    49,    50,    51,
  52,    53,    31,     3,    84,    66,    90,    91,    92,    82,
  94,   100,    63,     3,   100,    66,    70,    70,   107,   213,
  180,   153,    69,    96,    43,    63,    67,     3,    72,    73,
  220,   225,   226,   227,     3,    66,    71,     5,     3,     7,
  113,   161,   162,   163,    12,    63,    65,    39,   137,    72,
  244,    19,    20,   247,    12,    47,    24,    25,    63,    50,
  18,   273,    12,    31,   137,   138,    58,    35,    18,    63,
  159,    73,   145,   159,    68,   235,    51,    69,   130,   131,
  132,   133,   134,   135,   136,    66,    63,     3,    80,    81,
  82,   180,   204,    63,    64,    65,     3,    67,    68,    65,
  312,    71,    60,    73,    72,    63,    65,    64,     3,   303,
  68,   174,    71,    71,   308,   178,   310,    73,   230,    67,
  232,     3,    44,   313,    12,   237,   210,    49,    65,     3,
  18,    53,   326,    55,    71,    66,    68,    65,    65,    65,
  213,    72,   215,    71,    71,   234,   235,     3,   234,   339,
  213,   240,    74,    75,    67,    11,    37,   230,    66,   232,
  41,   355,   356,   357,    45,    46,    22,   240,   241,    65,
  243,    27,    28,    29,    85,    71,    57,    33,   251,   252,
  253,   293,    85,    64,    65,   379,   380,    71,    85,    67,
  279,   254,    37,   279,    73,    67,    41,    73,    71,    85,
  45,   120,   121,   122,   123,   124,   125,    66,    64,    67,
  66,    63,     3,    69,    67,    71,    72,    67,    63,    64,
  65,    76,    77,    78,    79,     3,    70,   359,    66,     3,
  4,    67,    71,   306,    65,    67,    73,    11,    85,    67,
  43,   314,   374,    65,   317,   318,   319,    71,    22,    66,
  382,    65,   325,    27,    28,    29,   329,     3,     4,    33,
  6,   334,    71,   336,    10,    11,    12,    65,    65,   358,
  65,    17,   358,    15,    60,    65,    22,    23,    65,    73,
  65,    27,    28,    29,    67,    73,    32,    33,     3,   362,
  36,    67,    66,    15,    67,    69,    65,   218,    72,    73,
  252,   213,    76,    77,   332,    79,    35,     2,    82,    83,
  126,   127,   128,   129,   103,   119,   241,    63,   118,    65,
  66,     3,     4,    69,    70,   230,    72,    -1,   232,    11,
  76,    77,    -1,    79,    -1,    -1,    82,    83,    -1,    -1,
  22,    -1,    -1,    -1,    -1,    27,    28,    29,     3,     4,
  -1,    33,    -1,    -1,    -1,    -1,    11,    -1,    -1,    -1,
  -1,    43,    -1,    -1,    -1,    -1,    -1,    22,    -1,    -1,
  -1,    -1,    27,    28,    29,    -1,    -1,    -1,    33,    -1,
  3,     4,    -1,    -1,    66,    -1,    -1,    69,    11,    -1,
  72,    73,    -1,    -1,    76,    77,    -1,    79,    -1,    22,
  82,    83,    -1,    -1,    27,    28,    29,    -1,    63,    -1,
  33,    66,     3,     4,    69,    70,    -1,    72,    -1,    -1,
  11,    76,    77,    -1,    79,    -1,    -1,    82,    83,    -1,
  -1,    22,    -1,    -1,    -1,    -1,    27,    28,    29,    -1,
  63,    -1,    33,    66,    -1,    -1,    69,     3,     4,    72,
  -1,    -1,    -1,    76,    77,    11,    79,    -1,    -1,    82,
  83,    -1,    -1,    -1,    -1,    -1,    22,    -1,    -1,    -1,
  -1,    27,    28,    29,    65,    66,    -1,    33,    69,     3,
  4,    72,    -1,    -1,    -1,    76,    77,    11,    79,    -1,
  -1,    82,    83,    -1,    -1,    -1,    -1,    -1,    22,    -1,
  -1,    -1,    -1,    27,    28,    29,     3,    -1,    -1,    33,
  66,    -1,    -1,    69,    11,    -1,    72,    -1,    -1,    -1,
  76,    77,    -1,    79,    -1,    22,    82,    83,    -1,    -1,
  27,    28,    29,    -1,    -1,    -1,    33,    -1,     3,    -1,
  -1,    -1,    66,    -1,    -1,    69,    11,    -1,    72,    -1,
  -1,    -1,    76,    77,    -1,    79,    -1,    22,    82,    83,
  -1,    -1,    27,    28,    29,     3,    -1,    -1,    33,    66,
  -1,    -1,    69,    11,    71,    72,    -1,    -1,    -1,    -1,
  -1,    -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,    27,
  28,    29,    -1,    -1,    -1,    33,    -1,    -1,    -1,    64,
  -1,    66,    -1,    -1,    69,    -1,    -1,    72,    -1,    -1,
  -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
  -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,
  -1,    69,    -1,    -1,    72
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
  0,    87,    88,     0,     5,     7,    12,    19,    20,    24,
  25,    31,    35,    72,    89,    90,    91,    92,    96,    97,
  98,    99,   100,   101,   102,   148,    66,    95,    95,     3,
  132,     3,    95,    95,    95,     3,    95,     3,     3,     4,
  11,    22,    27,    28,    29,    33,    66,    69,    72,    76,
  77,    79,    82,    83,   133,   134,   136,   137,   138,   140,
  141,   142,   143,   148,   151,   152,   153,   154,   155,   156,
  157,   158,     5,    31,     3,     3,     3,     3,    64,    66,
  69,    71,    72,   133,   144,   145,   146,   147,    66,   103,
  3,     3,     3,   146,     3,   147,    63,   159,    63,   147,
  72,   103,   103,    63,   151,   157,    43,    73,   157,   157,
  157,   157,   157,    66,   159,   135,   147,    73,    51,    50,
  44,    49,    53,    55,    74,    75,    76,    77,    78,    79,
  39,    47,    58,    69,    80,    81,    82,    66,    72,    84,
  95,    95,    12,    18,    93,    66,    63,   109,   125,   151,
  147,   147,     3,    64,    67,   104,   105,   132,    68,    94,
  106,   103,   103,   103,     3,   103,    65,    70,   151,   159,
  160,   161,   162,   163,   149,   139,   147,   106,   149,    67,
  73,   135,   151,   153,   154,   155,   155,   155,   155,   155,
  155,   156,   156,   156,   156,   157,   157,   157,   157,   157,
  157,   157,   124,   126,   133,   151,   150,   151,     3,     3,
  3,    94,   151,   110,    65,    71,    67,   125,    65,   107,
  93,    12,    18,   147,   109,   106,   106,   106,    66,   109,
  71,   164,    85,    70,   132,    73,    70,   135,    67,    67,
  71,    85,    73,    66,   103,     3,    69,    67,     3,     6,
  10,    17,    23,    32,    36,    65,    70,    90,   108,   109,
  111,   112,   113,   114,   115,   116,   117,   118,   127,   128,
  129,   130,   131,   132,   151,   151,    65,   105,    67,    94,
  109,   109,   109,   123,   125,   161,    70,   151,   159,   163,
  147,   135,   159,   133,   151,   150,   151,   151,   109,    67,
  3,   109,     3,   119,   120,   121,    66,     3,   151,   112,
  151,   151,   132,    93,    37,    41,    45,    46,    57,    64,
  65,   147,    67,    65,    73,    85,    67,    67,    66,    72,
  122,   109,    71,   151,    43,   109,    65,   109,    65,    93,
  94,   151,    65,    65,   151,   151,   151,    65,   151,   109,
  123,   151,    66,   121,    71,   151,   151,    15,    94,    60,
  65,    65,    43,    65,    65,    73,    67,    73,   123,     3,
  109,   109,   109,   130,    64,   147,   125,   151,    67,    67,
  15,   125,    64,    65,    65,    65,   109,   109,   130,    65,
  125,    65
};

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
  0,    86,    87,    88,    88,    89,    89,    89,    89,    89,
  89,    89,    89,    89,    89,    90,    90,    91,    92,    93,
  93,    93,    94,    94,    94,    95,    95,    96,    96,    97,
  97,    98,    99,   100,   101,   102,   103,   103,   104,   104,
  105,   106,   106,   107,   107,   108,   108,   108,   109,   110,
  110,   111,   111,   111,   111,   111,   111,   111,   111,   111,
  111,   111,   112,   112,   112,   112,   113,   114,   115,   116,
  117,   118,   118,   119,   119,   120,   120,   121,   121,   122,
  123,   123,   124,   124,   125,   125,   126,   126,   126,   126,
  127,   128,   128,   128,   128,   129,   129,   130,   130,   130,
  130,   130,   130,   131,   132,   132,   133,   133,   133,   133,
  133,   133,   133,   133,   134,   135,   136,   137,   138,   139,
  140,   141,   142,   143,   144,   145,   145,   146,   147,   147,
  147,   148,   149,   149,   150,   150,   151,   152,   152,   153,
  153,   154,   154,   154,   154,   154,   154,   154,   155,   155,
  155,   155,   155,   156,   156,   156,   156,   156,   156,   156,
  156,   157,   157,   157,   157,   157,   157,   157,   158,   158,
  158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
  159,   159,   160,   160,   161,   161,   162,   162,   163,   163,
  164,   164
};

/* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
  0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
  1,     1,     1,     1,     1,     6,     5,     8,     4,     0,
  1,     1,     0,     2,     2,     7,     6,     7,     8,     5,
  6,     4,     6,     6,     6,     5,     2,     4,     1,     3,
  4,     0,     2,     0,     1,     4,     6,     4,     3,     0,
  2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
  1,     1,     1,     1,     1,     1,     1,     3,     7,     5,
  3,     3,     3,     0,     1,     1,     3,     5,     4,     3,
  0,     1,     0,     1,     1,     3,     1,     1,     3,     3,
  2,     8,     6,     7,     6,     4,     4,     3,     5,     5,
  5,     7,     7,     3,     1,     3,     1,     1,     1,     1,
  1,     1,     1,     1,     2,     1,     4,     3,     5,     1,
  4,     2,     3,     2,     2,     1,     1,     1,     1,     1,
  3,     3,     0,     4,     0,     1,     1,     1,     3,     1,
  3,     1,     3,     3,     3,     3,     3,     3,     1,     3,
  3,     3,     3,     1,     3,     3,     3,     3,     3,     3,
  3,     1,     2,     2,     2,     2,     2,     2,     1,     2,
  2,     5,     1,     3,     3,     4,     6,     8,     4,     4,
  2,     4,     1,     3,     3,     1,     1,     1,     1,     1,
  0,     1
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
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1466 "parser.cpp" /* yacc.c:1257  */
      break;

    case 89: /* Definition  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1472 "parser.cpp" /* yacc.c:1257  */
      break;

    case 90: /* Const  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1478 "parser.cpp" /* yacc.c:1257  */
      break;

    case 91: /* Instance  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1484 "parser.cpp" /* yacc.c:1257  */
      break;

    case 92: /* TypeDecl  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1490 "parser.cpp" /* yacc.c:1257  */
      break;

    case 95: /* Receiver  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1496 "parser.cpp" /* yacc.c:1257  */
      break;

    case 96: /* Action  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1502 "parser.cpp" /* yacc.c:1257  */
      break;

    case 97: /* Reaction  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1508 "parser.cpp" /* yacc.c:1257  */
      break;

    case 98: /* Bind  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1514 "parser.cpp" /* yacc.c:1257  */
      break;

    case 99: /* Init  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1520 "parser.cpp" /* yacc.c:1257  */
      break;

    case 100: /* Getter  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1526 "parser.cpp" /* yacc.c:1257  */
      break;

    case 101: /* Method  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1532 "parser.cpp" /* yacc.c:1257  */
      break;

    case 102: /* Func  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1538 "parser.cpp" /* yacc.c:1257  */
      break;

    case 105: /* Parameter  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1544 "parser.cpp" /* yacc.c:1257  */
      break;

    case 108: /* BindStatement  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1550 "parser.cpp" /* yacc.c:1257  */
      break;

    case 109: /* Block  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1556 "parser.cpp" /* yacc.c:1257  */
      break;

    case 111: /* Statement  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1562 "parser.cpp" /* yacc.c:1257  */
      break;

    case 112: /* SimpleStatement  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1568 "parser.cpp" /* yacc.c:1257  */
      break;

    case 113: /* EmptyStatement  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1574 "parser.cpp" /* yacc.c:1257  */
      break;

    case 114: /* Activate  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1580 "parser.cpp" /* yacc.c:1257  */
      break;

    case 115: /* Change  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1586 "parser.cpp" /* yacc.c:1257  */
      break;

    case 116: /* ForIota  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1592 "parser.cpp" /* yacc.c:1257  */
      break;

    case 117: /* Return  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1598 "parser.cpp" /* yacc.c:1257  */
      break;

    case 118: /* IncrementStatement  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1604 "parser.cpp" /* yacc.c:1257  */
      break;

    case 121: /* PushPortCall  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1610 "parser.cpp" /* yacc.c:1257  */
      break;

    case 122: /* Indexession  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1616 "parser.cpp" /* yacc.c:1257  */
      break;

    case 127: /* ExpressionStatement  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1622 "parser.cpp" /* yacc.c:1257  */
      break;

    case 128: /* Var  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1628 "parser.cpp" /* yacc.c:1257  */
      break;

    case 129: /* AssignmentStatement  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1634 "parser.cpp" /* yacc.c:1257  */
      break;

    case 130: /* If  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1640 "parser.cpp" /* yacc.c:1257  */
      break;

    case 131: /* While  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1646 "parser.cpp" /* yacc.c:1257  */
      break;

    case 133: /* TypeLitExpression  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1652 "parser.cpp" /* yacc.c:1257  */
      break;

    case 134: /* Array  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1658 "parser.cpp" /* yacc.c:1257  */
      break;

    case 135: /* ElementType  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1664 "parser.cpp" /* yacc.c:1257  */
      break;

    case 136: /* StructType  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1670 "parser.cpp" /* yacc.c:1257  */
      break;

    case 137: /* Slice  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1676 "parser.cpp" /* yacc.c:1257  */
      break;

    case 138: /* Map  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1682 "parser.cpp" /* yacc.c:1257  */
      break;

    case 139: /* KeyType  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1688 "parser.cpp" /* yacc.c:1257  */
      break;

    case 140: /* ComponentType  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1694 "parser.cpp" /* yacc.c:1257  */
      break;

    case 141: /* PushPort  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1700 "parser.cpp" /* yacc.c:1257  */
      break;

    case 142: /* PullPort  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1706 "parser.cpp" /* yacc.c:1257  */
      break;

    case 143: /* Heap  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1712 "parser.cpp" /* yacc.c:1257  */
      break;

    case 144: /* Pointer  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1718 "parser.cpp" /* yacc.c:1257  */
      break;

    case 145: /* TypeLit  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1724 "parser.cpp" /* yacc.c:1257  */
      break;

    case 146: /* TypeName  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1730 "parser.cpp" /* yacc.c:1257  */
      break;

    case 147: /* Type  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1736 "parser.cpp" /* yacc.c:1257  */
      break;

    case 148: /* ArrayDimension  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1742 "parser.cpp" /* yacc.c:1257  */
      break;

    case 150: /* OptionalExpression  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1748 "parser.cpp" /* yacc.c:1257  */
      break;

    case 151: /* Expression  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1754 "parser.cpp" /* yacc.c:1257  */
      break;

    case 152: /* OrExpression  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1760 "parser.cpp" /* yacc.c:1257  */
      break;

    case 153: /* AndExpression  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1766 "parser.cpp" /* yacc.c:1257  */
      break;

    case 154: /* CompareExpression  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1772 "parser.cpp" /* yacc.c:1257  */
      break;

    case 155: /* AddExpression  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1778 "parser.cpp" /* yacc.c:1257  */
      break;

    case 156: /* MultiplyExpression  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1784 "parser.cpp" /* yacc.c:1257  */
      break;

    case 157: /* UnaryExpression  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1790 "parser.cpp" /* yacc.c:1257  */
      break;

    case 158: /* PrimaryExpression  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1796 "parser.cpp" /* yacc.c:1257  */
      break;

    case 161: /* Element  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1802 "parser.cpp" /* yacc.c:1257  */
      break;

    case 162: /* Key  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1808 "parser.cpp" /* yacc.c:1257  */
      break;

    case 163: /* Value  */
#line 90 "parser.y" /* yacc.c:1257  */
      {
        /* Free the node ($$). */
      }
#line 1814 "parser.cpp" /* yacc.c:1257  */
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
#line 162 "parser.y" /* yacc.c:1646  */
      {
        root = (yyvsp[0].list);
      }
#line 2102 "parser.cpp" /* yacc.c:1646  */
      break;

    case 3:
#line 165 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new SourceFile ();
      }
#line 2108 "parser.cpp" /* yacc.c:1646  */
      break;

    case 4:
#line 166 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-1].list)->append ((yyvsp[0].node));
      }
#line 2114 "parser.cpp" /* yacc.c:1646  */
      break;

    case 5:
#line 169 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2120 "parser.cpp" /* yacc.c:1646  */
      break;

    case 6:
#line 170 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2126 "parser.cpp" /* yacc.c:1646  */
      break;

    case 7:
#line 171 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2132 "parser.cpp" /* yacc.c:1646  */
      break;

    case 8:
#line 172 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2138 "parser.cpp" /* yacc.c:1646  */
      break;

    case 9:
#line 173 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2144 "parser.cpp" /* yacc.c:1646  */
      break;

    case 10:
#line 174 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2150 "parser.cpp" /* yacc.c:1646  */
      break;

    case 11:
#line 175 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2156 "parser.cpp" /* yacc.c:1646  */
      break;

    case 12:
#line 176 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2162 "parser.cpp" /* yacc.c:1646  */
      break;

    case 13:
#line 177 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2168 "parser.cpp" /* yacc.c:1646  */
      break;

    case 14:
#line 178 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2174 "parser.cpp" /* yacc.c:1646  */
      break;

    case 15:
#line 182 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Const ((yylsp[-5]), (yyvsp[-4].list), (yyvsp[-3].node), (yyvsp[-1].list));
      }
#line 2180 "parser.cpp" /* yacc.c:1646  */
      break;

    case 16:
#line 184 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Const ((yylsp[-4]), (yyvsp[-3].list), new EmptyType ((yylsp[-4])), (yyvsp[-1].list));
      }
#line 2186 "parser.cpp" /* yacc.c:1646  */
      break;

    case 17:
#line 188 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Instance ((yylsp[-7]), (yyvsp[-6].identifier), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-2].list));
      }
#line 2192 "parser.cpp" /* yacc.c:1646  */
      break;

    case 18:
#line 190 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::Type ((yylsp[-3]), (yyvsp[-2].identifier), (yyvsp[-1].node));
      }
#line 2198 "parser.cpp" /* yacc.c:1646  */
      break;

    case 19:
#line 193 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Mutable;
      }
#line 2204 "parser.cpp" /* yacc.c:1646  */
      break;

    case 20:
#line 194 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Immutable;
      }
#line 2210 "parser.cpp" /* yacc.c:1646  */
      break;

    case 21:
#line 195 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Foreign;
      }
#line 2216 "parser.cpp" /* yacc.c:1646  */
      break;

    case 22:
#line 198 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Mutable;
      }
#line 2222 "parser.cpp" /* yacc.c:1646  */
      break;

    case 23:
#line 199 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Immutable;
      }
#line 2228 "parser.cpp" /* yacc.c:1646  */
      break;

    case 24:
#line 200 "parser.y" /* yacc.c:1646  */
      {
        (yyval.mutability) = Foreign;
      }
#line 2234 "parser.cpp" /* yacc.c:1646  */
      break;

    case 25:
#line 204 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Receiver ((yylsp[-6]), (yyvsp[-5].identifier), (yyvsp[-4].mutability), (yyvsp[-3].mutability), true, (yyvsp[-1].identifier));
      }
#line 2240 "parser.cpp" /* yacc.c:1646  */
      break;

    case 26:
#line 206 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Receiver ((yylsp[-5]), (yyvsp[-4].identifier), (yyvsp[-3].mutability), (yyvsp[-2].mutability), false, (yyvsp[-1].identifier));
      }
#line 2246 "parser.cpp" /* yacc.c:1646  */
      break;

    case 27:
#line 210 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Action ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2252 "parser.cpp" /* yacc.c:1646  */
      break;

    case 28:
#line 212 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new DimensionedAction ((yylsp[-7]), (yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2258 "parser.cpp" /* yacc.c:1646  */
      break;

    case 29:
#line 216 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Reaction ((yylsp[-4]), (yyvsp[-3].node), (yyvsp[-2].identifier), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2264 "parser.cpp" /* yacc.c:1646  */
      break;

    case 30:
#line 218 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new DimensionedReaction ((yylsp[-4]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].identifier), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2270 "parser.cpp" /* yacc.c:1646  */
      break;

    case 31:
#line 222 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Bind ((yylsp[-3]), (yyvsp[-2].node), (yyvsp[-1].identifier), (yyvsp[0].node));
      }
#line 2276 "parser.cpp" /* yacc.c:1646  */
      break;

    case 32:
#line 226 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Initializer ((yylsp[-5]), (yyvsp[-4].node), (yyvsp[-3].identifier), (yyvsp[-2].list), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2282 "parser.cpp" /* yacc.c:1646  */
      break;

    case 33:
#line 230 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Getter ((yylsp[-5]), (yyvsp[-4].node), (yyvsp[-3].identifier), (yyvsp[-2].list), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2288 "parser.cpp" /* yacc.c:1646  */
      break;

    case 34:
#line 234 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Method ((yylsp[-5]), (yyvsp[-4].node), (yyvsp[-3].identifier), (yyvsp[-2].list), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2294 "parser.cpp" /* yacc.c:1646  */
      break;

    case 35:
#line 238 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Function ((yylsp[-4]), (yyvsp[-3].identifier), (yyvsp[-2].list), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2300 "parser.cpp" /* yacc.c:1646  */
      break;

    case 36:
#line 242 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ParameterList (yyloc);
      }
#line 2306 "parser.cpp" /* yacc.c:1646  */
      break;

    case 37:
#line 244 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list);
      }
#line 2312 "parser.cpp" /* yacc.c:1646  */
      break;

    case 38:
#line 248 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ParameterList ((yylsp[0])))->append ((yyvsp[0].node));
      }
#line 2318 "parser.cpp" /* yacc.c:1646  */
      break;

    case 39:
#line 250 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node));
      }
#line 2324 "parser.cpp" /* yacc.c:1646  */
      break;

    case 40:
#line 254 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new VariableList ((yylsp[-3]), (yyvsp[-3].list), (yyvsp[-2].mutability), (yyvsp[-1].mutability), (yyvsp[0].node));
      }
#line 2330 "parser.cpp" /* yacc.c:1646  */
      break;

    case 41:
#line 258 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ParameterList (yyloc);
      }
#line 2336 "parser.cpp" /* yacc.c:1646  */
      break;

    case 42:
#line 260 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ParameterList (yyloc))->append (new VariableList ((yylsp[-1]), (new IdentifierList ((yylsp[-1])))->append (new Identifier ((yylsp[-1]), "")), Mutable, (yyvsp[-1].mutability), (yyvsp[0].node)));
      }
#line 2342 "parser.cpp" /* yacc.c:1646  */
      break;

    case 45:
#line 267 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new BindPushPort ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node));
      }
#line 2348 "parser.cpp" /* yacc.c:1646  */
      break;

    case 46:
#line 269 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new BindPushPortParameter ((yylsp[-5]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node));
      }
#line 2354 "parser.cpp" /* yacc.c:1646  */
      break;

    case 47:
#line 271 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new BindPullPort ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node));
      }
#line 2360 "parser.cpp" /* yacc.c:1646  */
      break;

    case 48:
#line 273 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].list);
      }
#line 2366 "parser.cpp" /* yacc.c:1646  */
      break;

    case 49:
#line 276 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new StatementList (yyloc);
      }
#line 2372 "parser.cpp" /* yacc.c:1646  */
      break;

    case 50:
#line 277 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-1].list)->append ((yyvsp[0].node));
      }
#line 2378 "parser.cpp" /* yacc.c:1646  */
      break;

    case 51:
#line 280 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2384 "parser.cpp" /* yacc.c:1646  */
      break;

    case 52:
#line 281 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2390 "parser.cpp" /* yacc.c:1646  */
      break;

    case 53:
#line 282 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2396 "parser.cpp" /* yacc.c:1646  */
      break;

    case 54:
#line 283 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2402 "parser.cpp" /* yacc.c:1646  */
      break;

    case 55:
#line 284 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2408 "parser.cpp" /* yacc.c:1646  */
      break;

    case 56:
#line 285 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2414 "parser.cpp" /* yacc.c:1646  */
      break;

    case 57:
#line 286 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2420 "parser.cpp" /* yacc.c:1646  */
      break;

    case 58:
#line 287 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2426 "parser.cpp" /* yacc.c:1646  */
      break;

    case 59:
#line 288 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2432 "parser.cpp" /* yacc.c:1646  */
      break;

    case 60:
#line 289 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2438 "parser.cpp" /* yacc.c:1646  */
      break;

    case 61:
#line 290 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2444 "parser.cpp" /* yacc.c:1646  */
      break;

    case 62:
#line 293 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2450 "parser.cpp" /* yacc.c:1646  */
      break;

    case 63:
#line 294 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2456 "parser.cpp" /* yacc.c:1646  */
      break;

    case 64:
#line 295 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2462 "parser.cpp" /* yacc.c:1646  */
      break;

    case 65:
#line 296 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2468 "parser.cpp" /* yacc.c:1646  */
      break;

    case 66:
#line 298 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new EmptyStatement (yyloc);
      }
#line 2474 "parser.cpp" /* yacc.c:1646  */
      break;

    case 67:
#line 302 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Activate ((yylsp[-2]), (yyvsp[-1].list), (yyvsp[0].node));
      }
#line 2480 "parser.cpp" /* yacc.c:1646  */
      break;

    case 68:
#line 306 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Change ((yylsp[-6]), (yyvsp[-4].node), (yyvsp[-2].identifier), (yyvsp[0].node));
      }
#line 2486 "parser.cpp" /* yacc.c:1646  */
      break;

    case 69:
#line 310 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ForIota ((yylsp[-4]), (yyvsp[-3].identifier), (yyvsp[-1].node), (yyvsp[0].node));
      }
#line 2492 "parser.cpp" /* yacc.c:1646  */
      break;

    case 70:
#line 314 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Return ((yylsp[-2]), (yyvsp[-1].node));
      }
#line 2498 "parser.cpp" /* yacc.c:1646  */
      break;

    case 71:
#line 318 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IncrementDecrement ((yylsp[-2]), (yyvsp[-2].node), IncrementDecrement::Increment);
      }
#line 2504 "parser.cpp" /* yacc.c:1646  */
      break;

    case 72:
#line 320 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IncrementDecrement ((yylsp[-2]), (yyvsp[-2].node), IncrementDecrement::Decrement);
      }
#line 2510 "parser.cpp" /* yacc.c:1646  */
      break;

    case 73:
#line 324 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ExpressionList (yyloc);
      }
#line 2516 "parser.cpp" /* yacc.c:1646  */
      break;

    case 74:
#line 326 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[0].list);
      }
#line 2522 "parser.cpp" /* yacc.c:1646  */
      break;

    case 75:
#line 330 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ExpressionList ((yylsp[0])))->append ((yyvsp[0].node));
      }
#line 2528 "parser.cpp" /* yacc.c:1646  */
      break;

    case 76:
#line 332 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node));
      }
#line 2534 "parser.cpp" /* yacc.c:1646  */
      break;

    case 77:
#line 336 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IndexedPushPortCall ((yylsp[-4]), (yyvsp[-4].identifier), (yyvsp[-3].node), (yyvsp[-1].list));
      }
#line 2540 "parser.cpp" /* yacc.c:1646  */
      break;

    case 78:
#line 338 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new PushPortCall ((yylsp[-3]), (yyvsp[-3].identifier), (yyvsp[-1].list));
      }
#line 2546 "parser.cpp" /* yacc.c:1646  */
      break;

    case 79:
#line 340 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].node);
      }
#line 2552 "parser.cpp" /* yacc.c:1646  */
      break;

    case 80:
#line 343 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ExpressionList (yyloc);
      }
#line 2558 "parser.cpp" /* yacc.c:1646  */
      break;

    case 81:
#line 344 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[0].list);
      }
#line 2564 "parser.cpp" /* yacc.c:1646  */
      break;

    case 82:
#line 347 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ExpressionList (yyloc);
      }
#line 2570 "parser.cpp" /* yacc.c:1646  */
      break;

    case 83:
#line 348 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[0].list);
      }
#line 2576 "parser.cpp" /* yacc.c:1646  */
      break;

    case 84:
#line 352 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ExpressionList ((yylsp[0])))->append ((yyvsp[0].node));
      }
#line 2582 "parser.cpp" /* yacc.c:1646  */
      break;

    case 85:
#line 354 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node));
      }
#line 2588 "parser.cpp" /* yacc.c:1646  */
      break;

    case 86:
#line 358 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ExpressionList ((yylsp[0])))->append ((yyvsp[0].node));
      }
#line 2594 "parser.cpp" /* yacc.c:1646  */
      break;

    case 87:
#line 360 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ExpressionList ((yylsp[0])))->append (new TypeExpression ((yylsp[0]), (yyvsp[0].node)));
      }
#line 2600 "parser.cpp" /* yacc.c:1646  */
      break;

    case 88:
#line 362 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node));
      }
#line 2606 "parser.cpp" /* yacc.c:1646  */
      break;

    case 89:
#line 364 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append (new TypeExpression ((yylsp[-2]), (yyvsp[0].node)));
      }
#line 2612 "parser.cpp" /* yacc.c:1646  */
      break;

    case 90:
#line 368 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ExpressionStatement ((yylsp[-1]), (yyvsp[-1].node));
      }
#line 2618 "parser.cpp" /* yacc.c:1646  */
      break;

    case 91:
#line 372 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Var ((yylsp[-7]), (yyvsp[-6].list), (yyvsp[-5].mutability), (yyvsp[-4].mutability), (yyvsp[-3].node), (yyvsp[-1].list));
      }
#line 2624 "parser.cpp" /* yacc.c:1646  */
      break;

    case 92:
#line 374 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Var ((yylsp[-5]), (yyvsp[-4].list), (yyvsp[-3].mutability), (yyvsp[-2].mutability), (yyvsp[-1].node), new ExpressionList ((yylsp[-5])));
      }
#line 2630 "parser.cpp" /* yacc.c:1646  */
      break;

    case 93:
#line 376 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Var ((yylsp[-6]), (yyvsp[-5].list), (yyvsp[-4].mutability), (yyvsp[-3].mutability), new EmptyType ((yylsp[-6])), (yyvsp[-1].list));
      }
#line 2636 "parser.cpp" /* yacc.c:1646  */
      break;

    case 94:
#line 378 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Var ((yylsp[-5]), (yyvsp[-5].list), (yyvsp[-4].mutability), (yyvsp[-3].mutability), new EmptyType ((yylsp[-5])), (yyvsp[-1].list));
      }
#line 2642 "parser.cpp" /* yacc.c:1646  */
      break;

    case 95:
#line 382 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Assign ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node));
      }
#line 2648 "parser.cpp" /* yacc.c:1646  */
      break;

    case 96:
#line 384 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new AddAssign ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node));
      }
#line 2654 "parser.cpp" /* yacc.c:1646  */
      break;

    case 97:
#line 388 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-2]), new EmptyStatement ((yylsp[-2])), (yyvsp[-1].node), (yyvsp[0].node), new StatementList ((yylsp[-2])));
      }
#line 2660 "parser.cpp" /* yacc.c:1646  */
      break;

    case 98:
#line 390 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-4]), new EmptyStatement ((yylsp[-4])), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2666 "parser.cpp" /* yacc.c:1646  */
      break;

    case 99:
#line 392 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-4]), new EmptyStatement ((yylsp[-4])), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2672 "parser.cpp" /* yacc.c:1646  */
      break;

    case 100:
#line 394 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-4]), (yyvsp[-3].node), (yyvsp[-1].node), (yyvsp[0].node), new StatementList ((yylsp[-4])));
      }
#line 2678 "parser.cpp" /* yacc.c:1646  */
      break;

    case 101:
#line 396 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2684 "parser.cpp" /* yacc.c:1646  */
      break;

    case 102:
#line 398 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new If ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2690 "parser.cpp" /* yacc.c:1646  */
      break;

    case 103:
#line 402 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new While ((yylsp[-2]), (yyvsp[-1].node), (yyvsp[0].node));
      }
#line 2696 "parser.cpp" /* yacc.c:1646  */
      break;

    case 104:
#line 406 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new IdentifierList ((yylsp[0])))->append ((yyvsp[0].identifier));
      }
#line 2702 "parser.cpp" /* yacc.c:1646  */
      break;

    case 105:
#line 408 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].identifier));
      }
#line 2708 "parser.cpp" /* yacc.c:1646  */
      break;

    case 106:
#line 412 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2714 "parser.cpp" /* yacc.c:1646  */
      break;

    case 107:
#line 413 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2720 "parser.cpp" /* yacc.c:1646  */
      break;

    case 108:
#line 414 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2726 "parser.cpp" /* yacc.c:1646  */
      break;

    case 109:
#line 415 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2732 "parser.cpp" /* yacc.c:1646  */
      break;

    case 110:
#line 416 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2738 "parser.cpp" /* yacc.c:1646  */
      break;

    case 111:
#line 417 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2744 "parser.cpp" /* yacc.c:1646  */
      break;

    case 112:
#line 418 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2750 "parser.cpp" /* yacc.c:1646  */
      break;

    case 113:
#line 419 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2756 "parser.cpp" /* yacc.c:1646  */
      break;

    case 114:
#line 422 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Array ((yylsp[-1]), (yyvsp[-1].node), (yyvsp[0].node));
      }
#line 2762 "parser.cpp" /* yacc.c:1646  */
      break;

    case 115:
#line 424 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2768 "parser.cpp" /* yacc.c:1646  */
      break;

    case 116:
#line 426 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].list);
      }
#line 2774 "parser.cpp" /* yacc.c:1646  */
      break;

    case 117:
#line 428 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Slice ((yylsp[-2]), (yyvsp[0].node));
      }
#line 2780 "parser.cpp" /* yacc.c:1646  */
      break;

    case 118:
#line 430 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Map ((yylsp[-4]), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2786 "parser.cpp" /* yacc.c:1646  */
      break;

    case 119:
#line 432 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2792 "parser.cpp" /* yacc.c:1646  */
      break;

    case 120:
#line 436 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].list);
        static_cast<FieldList*> ((yyvsp[-1].list))->is_component = true;
      }
#line 2798 "parser.cpp" /* yacc.c:1646  */
      break;

    case 121:
#line 438 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new PushPort ((yylsp[-1]), (yyvsp[0].list));
      }
#line 2804 "parser.cpp" /* yacc.c:1646  */
      break;

    case 122:
#line 442 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new PullPort ((yylsp[-2]), (yyvsp[-1].list), (yyvsp[0].list));
      }
#line 2810 "parser.cpp" /* yacc.c:1646  */
      break;

    case 123:
#line 444 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Heap ((yylsp[-1]), (yyvsp[0].node));
      }
#line 2816 "parser.cpp" /* yacc.c:1646  */
      break;

    case 124:
#line 446 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Pointer ((yylsp[-1]), (yyvsp[0].node));
      }
#line 2822 "parser.cpp" /* yacc.c:1646  */
      break;

    case 125:
#line 449 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2828 "parser.cpp" /* yacc.c:1646  */
      break;

    case 126:
#line 450 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2834 "parser.cpp" /* yacc.c:1646  */
      break;

    case 127:
#line 452 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IdentifierType ((yylsp[0]), (yyvsp[0].identifier));
      }
#line 2840 "parser.cpp" /* yacc.c:1646  */
      break;

    case 128:
#line 455 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2846 "parser.cpp" /* yacc.c:1646  */
      break;

    case 129:
#line 456 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2852 "parser.cpp" /* yacc.c:1646  */
      break;

    case 130:
#line 457 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].node);
      }
#line 2858 "parser.cpp" /* yacc.c:1646  */
      break;

    case 131:
#line 459 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].node);
      }
#line 2864 "parser.cpp" /* yacc.c:1646  */
      break;

    case 132:
#line 463 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new FieldList (yyloc);
      }
#line 2870 "parser.cpp" /* yacc.c:1646  */
      break;

    case 133:
#line 465 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-3].list)->append (new VariableList ((yylsp[-3]), (yyvsp[-2].list), Mutable, Mutable, (yyvsp[-1].node)));
      }
#line 2876 "parser.cpp" /* yacc.c:1646  */
      break;

    case 134:
#line 469 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new EmptyExpression (yyloc);
      }
#line 2882 "parser.cpp" /* yacc.c:1646  */
      break;

    case 135:
#line 471 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2888 "parser.cpp" /* yacc.c:1646  */
      break;

    case 136:
#line 473 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2894 "parser.cpp" /* yacc.c:1646  */
      break;

    case 137:
#line 477 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2900 "parser.cpp" /* yacc.c:1646  */
      break;

    case 138:
#line 479 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::logic_or_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2906 "parser.cpp" /* yacc.c:1646  */
      break;

    case 139:
#line 483 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2912 "parser.cpp" /* yacc.c:1646  */
      break;

    case 140:
#line 485 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::logic_and_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2918 "parser.cpp" /* yacc.c:1646  */
      break;

    case 141:
#line 489 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2924 "parser.cpp" /* yacc.c:1646  */
      break;

    case 142:
#line 491 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::equal_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2930 "parser.cpp" /* yacc.c:1646  */
      break;

    case 143:
#line 493 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::not_equal_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2936 "parser.cpp" /* yacc.c:1646  */
      break;

    case 144:
#line 495 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::less_than_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2942 "parser.cpp" /* yacc.c:1646  */
      break;

    case 145:
#line 497 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::less_equal_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2948 "parser.cpp" /* yacc.c:1646  */
      break;

    case 146:
#line 499 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::more_than_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2954 "parser.cpp" /* yacc.c:1646  */
      break;

    case 147:
#line 501 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::more_equal_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2960 "parser.cpp" /* yacc.c:1646  */
      break;

    case 148:
#line 505 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2966 "parser.cpp" /* yacc.c:1646  */
      break;

    case 149:
#line 507 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::add_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2972 "parser.cpp" /* yacc.c:1646  */
      break;

    case 150:
#line 509 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::subtract_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2978 "parser.cpp" /* yacc.c:1646  */
      break;

    case 151:
#line 511 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::bit_or_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2984 "parser.cpp" /* yacc.c:1646  */
      break;

    case 152:
#line 513 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::bit_xor_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2990 "parser.cpp" /* yacc.c:1646  */
      break;

    case 153:
#line 517 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 2996 "parser.cpp" /* yacc.c:1646  */
      break;

    case 154:
#line 519 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::multiply_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3002 "parser.cpp" /* yacc.c:1646  */
      break;

    case 155:
#line 521 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::divide_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3008 "parser.cpp" /* yacc.c:1646  */
      break;

    case 156:
#line 523 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::modulus_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3014 "parser.cpp" /* yacc.c:1646  */
      break;

    case 157:
#line 525 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::left_shift_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3020 "parser.cpp" /* yacc.c:1646  */
      break;

    case 158:
#line 527 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::right_shift_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3026 "parser.cpp" /* yacc.c:1646  */
      break;

    case 159:
#line 529 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::bit_and_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3032 "parser.cpp" /* yacc.c:1646  */
      break;

    case 160:
#line 531 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::BinaryArithmetic ((yylsp[-2]), &semantic::bit_and_not_temp, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3038 "parser.cpp" /* yacc.c:1646  */
      break;

    case 161:
#line 534 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3044 "parser.cpp" /* yacc.c:1646  */
      break;

    case 162:
#line 535 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::UnaryArithmetic ((yylsp[-1]), &semantic::posate_temp, (yyvsp[0].node));
      }
#line 3050 "parser.cpp" /* yacc.c:1646  */
      break;

    case 163:
#line 536 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::UnaryArithmetic ((yylsp[-1]), &semantic::negate_temp, (yyvsp[0].node));
      }
#line 3056 "parser.cpp" /* yacc.c:1646  */
      break;

    case 164:
#line 537 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::UnaryArithmetic ((yylsp[-1]), &semantic::logic_not_temp, (yyvsp[0].node));
      }
#line 3062 "parser.cpp" /* yacc.c:1646  */
      break;

    case 165:
#line 538 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new ast::UnaryArithmetic ((yylsp[-1]), &semantic::complement_temp, (yyvsp[0].node));
      }
#line 3068 "parser.cpp" /* yacc.c:1646  */
      break;

    case 166:
#line 539 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Dereference ((yylsp[-1]), (yyvsp[0].node));
      }
#line 3074 "parser.cpp" /* yacc.c:1646  */
      break;

    case 167:
#line 540 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new AddressOf ((yylsp[-1]), (yyvsp[0].node));
      }
#line 3080 "parser.cpp" /* yacc.c:1646  */
      break;

    case 168:
#line 544 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3086 "parser.cpp" /* yacc.c:1646  */
      break;

    case 169:
#line 546 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new CompositeLiteral ((yylsp[-1]), (yyvsp[-1].node), (yyvsp[0].list));
      }
#line 3092 "parser.cpp" /* yacc.c:1646  */
      break;

    case 170:
#line 548 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new CompositeLiteral ((yylsp[-1]), new IdentifierType ((yylsp[-1]), (yyvsp[-1].identifier)), (yyvsp[0].list));
      }
#line 3098 "parser.cpp" /* yacc.c:1646  */
      break;

    case 171:
#line 550 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new CompositeLiteral ((yylsp[-4]), new Array ((yylsp[-4]), new EmptyExpression ((yylsp[-4])), (yyvsp[-1].node)), (yyvsp[0].list));
      }
#line 3104 "parser.cpp" /* yacc.c:1646  */
      break;

    case 172:
#line 553 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IdentifierExpression ((yylsp[0]), (yyvsp[0].identifier));
      }
#line 3110 "parser.cpp" /* yacc.c:1646  */
      break;

    case 173:
#line 555 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[-1].node);
      }
#line 3116 "parser.cpp" /* yacc.c:1646  */
      break;

    case 174:
#line 557 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Select ((yylsp[-2]), (yyvsp[-2].node), (yyvsp[0].identifier));
      }
#line 3122 "parser.cpp" /* yacc.c:1646  */
      break;

    case 175:
#line 559 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Index ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node));
      }
#line 3128 "parser.cpp" /* yacc.c:1646  */
      break;

    case 176:
#line 561 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IndexSlice ((yylsp[-5]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node), new EmptyExpression (yyloc));
      }
#line 3134 "parser.cpp" /* yacc.c:1646  */
      break;

    case 177:
#line 563 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new IndexSlice ((yylsp[-7]), (yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node));
      }
#line 3140 "parser.cpp" /* yacc.c:1646  */
      break;

    case 178:
#line 565 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Call ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].list));
      }
#line 3146 "parser.cpp" /* yacc.c:1646  */
      break;

    case 179:
#line 567 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Conversion ((yylsp[-3]), new TypeExpression ((yylsp[-3]), (yyvsp[-3].node)), (yyvsp[-1].node));
      }
#line 3152 "parser.cpp" /* yacc.c:1646  */
      break;

    case 180:
#line 570 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = new ElementList ((yylsp[-1]));
      }
#line 3158 "parser.cpp" /* yacc.c:1646  */
      break;

    case 181:
#line 571 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list);
      }
#line 3164 "parser.cpp" /* yacc.c:1646  */
      break;

    case 182:
#line 574 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (new ElementList ((yylsp[0])))->append ((yyvsp[0].node));
      }
#line 3170 "parser.cpp" /* yacc.c:1646  */
      break;

    case 183:
#line 575 "parser.y" /* yacc.c:1646  */
      {
        (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node));
      }
#line 3176 "parser.cpp" /* yacc.c:1646  */
      break;

    case 184:
#line 578 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Element ((yylsp[-2]), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 3182 "parser.cpp" /* yacc.c:1646  */
      break;

    case 185:
#line 579 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = new Element ((yylsp[0]), new EmptyExpression ((yylsp[0])), (yyvsp[0].node));
      }
#line 3188 "parser.cpp" /* yacc.c:1646  */
      break;

    case 186:
#line 582 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3194 "parser.cpp" /* yacc.c:1646  */
      break;

    case 187:
#line 583 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].list);
      }
#line 3200 "parser.cpp" /* yacc.c:1646  */
      break;

    case 188:
#line 586 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].node);
      }
#line 3206 "parser.cpp" /* yacc.c:1646  */
      break;

    case 189:
#line 587 "parser.y" /* yacc.c:1646  */
      {
        (yyval.node) = (yyvsp[0].list);
      }
#line 3212 "parser.cpp" /* yacc.c:1646  */
      break;


#line 3216 "parser.cpp" /* yacc.c:1646  */
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
#line 593 "parser.y" /* yacc.c:1906  */

