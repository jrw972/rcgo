/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

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
#define YYBISON_VERSION "3.0.2"

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
#include "ast.hpp"

using namespace ast;

#line 75 "parser.cpp" /* yacc.c:339  */

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
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 10 "parser.y" /* yacc.c:355  */
 ast::Node* node; ast::List* list; ast::Identifier* identifier; Mutability mutability; 

#line 181 "parser.cpp" /* yacc.c:355  */
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

/* Copy the second part of user declarations.  */

#line 210 "parser.cpp" /* yacc.c:358  */

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
#define YYLAST   681

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  78
/* YYNRULES -- Number of rules.  */
#define YYNRULES  192
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  398

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
       0,   160,   160,   163,   164,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   179,   181,   185,   188,   191,
     192,   193,   196,   197,   198,   201,   203,   207,   209,   213,
     215,   219,   223,   225,   229,   233,   235,   239,   241,   245,
     247,   251,   253,   257,   260,   261,   264,   266,   268,   271,
     274,   275,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   291,   292,   293,   294,   296,   299,   303,
     307,   311,   315,   317,   322,   323,   327,   329,   333,   335,
     338,   341,   342,   345,   346,   349,   351,   355,   357,   359,
     361,   365,   369,   371,   373,   375,   379,   381,   385,   387,
     389,   391,   393,   395,   399,   403,   405,   410,   411,   412,
     413,   414,   415,   416,   417,   420,   422,   424,   426,   428,
     430,   433,   436,   439,   442,   444,   447,   448,   450,   453,
     454,   455,   457,   461,   462,   467,   468,   471,   474,   476,
     480,   482,   486,   488,   490,   492,   494,   496,   498,   502,
     504,   506,   508,   510,   514,   516,   518,   520,   522,   524,
     526,   528,   532,   533,   534,   535,   536,   537,   538,   541,
     543,   545,   547,   550,   552,   554,   556,   558,   560,   562,
     564,   568,   569,   572,   573,   576,   577,   580,   581,   584,
     585,   587,   589
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
  "Init", "Getter", "Method", "Func", "Signature", "ParameterList",
  "Parameter", "optional_semicolon", "BindStatement", "Block",
  "StatementList", "Statement", "SimpleStatement", "EmptyStatement",
  "ActivateStatement", "ChangeStatement", "ForIotaStatement",
  "ReturnStatement", "IncrementStatement", "OptionalPushPortCallList",
  "PushPortCallList", "PushPortCall", "IndexExpression",
  "OptionalExpressionList", "OptionalTypeOrExpressionList",
  "ExpressionList", "TypeOrExpressionList", "ExpressionStatement",
  "VarStatement", "AssignmentStatement", "IfStatement", "WhileStatement",
  "IdentifierList", "TypeLitExpression", "ArrayType", "ElementType",
  "StructType", "SliceType", "MapType", "KeyType", "ComponentType",
  "PushPortType", "PullPortType", "HeapType", "PointerType", "TypeLit",
  "TypeName", "Type", "ArrayDimension", "FieldList", "OptionalExpression",
  "Expression", "OrExpression", "AndExpression", "CompareExpression",
  "AddExpression", "MultiplyExpression", "UnaryExpression",
  "PrimaryExpression", "LiteralValue", "ElementList", "Element", "Key",
  "Value", "OptionalComma", YY_NULLPTR
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

#define YYPACT_NINF -349

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-349)))

#define YYTABLE_NINF -189

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -349,    10,    92,  -349,   -45,   -45,    14,    28,   -45,   -45,
      61,   -45,    62,   525,  -349,  -349,  -349,  -349,  -349,  -349,
    -349,  -349,  -349,  -349,  -349,    18,   116,   130,   131,  -349,
     216,    75,   149,   156,   158,   159,   172,   609,  -349,  -349,
      94,   609,   104,    75,    75,   120,   525,   525,   349,   525,
     525,   525,   525,   525,    32,  -349,  -349,  -349,  -349,  -349,
    -349,  -349,  -349,   609,    -6,   134,   137,   218,   132,    98,
    -349,    30,   -45,   -45,    12,   122,   128,  -349,   525,   609,
     609,   190,   431,  -349,  -349,  -349,  -349,   135,    25,    63,
      75,    75,    75,   191,    75,   140,   416,  -349,  -349,  -349,
     609,   146,  -349,  -349,   129,  -349,   142,   609,  -349,  -349,
    -349,  -349,  -349,   525,  -349,  -349,  -349,  -349,   525,   525,
     525,   525,   525,   525,   525,   525,   525,   525,   525,   525,
     525,   525,   525,   525,   525,   525,   525,   525,   525,   214,
     219,   223,  -349,  -349,   146,   525,  -349,  -349,   -10,  -349,
     161,  -349,  -349,   525,  -349,   168,  -349,     4,    36,   609,
    -349,    63,   146,    63,   155,   128,  -349,  -349,   151,   152,
     163,  -349,   169,  -349,     0,   175,  -349,   609,    16,  -349,
     609,  -349,   193,   137,   218,   132,   132,   132,   132,   132,
     132,    98,    98,    98,    98,  -349,  -349,  -349,  -349,  -349,
    -349,  -349,   196,   194,    32,  -349,   181,   195,  -349,   203,
      75,    24,   217,   334,  -349,   525,  -349,    44,    14,   222,
     146,  -349,  -349,   128,   609,  -349,   609,   609,  -349,   525,
    -349,   443,   200,   443,  -349,   228,   609,  -349,  -349,   220,
    -349,  -349,   525,   525,  -349,   525,   128,   231,   278,   128,
      50,   283,   224,   534,   513,   525,    14,  -349,  -349,  -349,
    -349,  -349,  -349,  -349,  -349,  -349,  -349,  -349,  -349,  -349,
    -349,  -349,  -349,  -349,  -349,     4,   101,  -349,  -349,  -349,
    -349,   609,  -349,   128,   128,   128,   238,   244,  -349,  -349,
    -349,  -349,  -349,   230,  -349,  -349,    32,  -349,   243,   232,
     252,  -349,  -349,   253,  -349,    56,   128,   250,  -349,   525,
     280,   128,   263,    -5,   265,     4,   146,   525,   266,   268,
     525,   525,   525,  -349,  -349,  -349,  -349,  -349,   269,  -349,
    -349,   525,   128,  -349,   525,   525,   270,  -349,   283,   271,
     525,  -349,   525,   324,  -349,   146,   287,   293,  -349,  -349,
     294,     1,   299,  -349,   292,  -349,   301,   296,   525,  -349,
     369,   128,   128,   -17,   321,   525,  -349,  -349,   525,  -349,
    -349,  -349,  -349,  -349,   306,   307,  -349,   360,  -349,  -349,
     525,   -12,    64,   314,  -349,   128,   -17,    78,   525,  -349,
    -349,  -349,  -349,  -349,  -349,  -349,    79,  -349
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     4,    14,    11,     5,     8,     9,
      10,     6,     7,    12,    13,     0,     0,     0,     0,   105,
       0,     0,     0,     0,     0,     0,     0,     0,   173,   169,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,   108,   109,   110,   111,
     112,   113,   114,     0,     0,   137,   138,   140,   142,   149,
     154,   162,     0,     0,    19,     0,     0,   128,     0,     0,
       0,     0,     0,   127,   126,   130,   129,     0,     0,    22,
       0,     0,     0,     0,     0,     0,     0,   171,   133,   124,
       0,    22,   122,   133,     0,   167,     0,     0,   163,   164,
     166,   168,   165,     0,   170,   115,   116,   132,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    83,   135,     0,
       0,     0,    20,    21,    22,     0,    50,    31,     0,    85,
       0,   125,   106,     0,    39,    44,    41,    19,     0,     0,
      37,    22,    22,    22,     0,     0,    18,   181,   189,   190,
     191,   183,     0,   186,     0,     0,   120,     0,     0,   174,
       0,   118,     0,   139,   141,   143,   146,   148,   144,   145,
     147,   150,   151,   152,   153,   161,   158,   159,   155,   156,
     157,   160,     0,    84,    88,    87,     0,   136,   175,     0,
       0,     0,     0,     0,    16,     0,   131,     0,    45,     0,
      22,    23,    24,     0,     0,    36,     0,     0,    33,    81,
      29,   192,     0,     0,   121,     0,     0,   123,   117,     0,
     180,   179,     0,   135,   176,     0,     0,     0,     0,     0,
     173,    74,     0,     0,     0,     0,     0,    67,    49,    62,
      61,    55,    51,    52,    63,    54,    59,    60,    56,    65,
      64,    53,    66,    57,    58,    19,     0,    86,    15,    42,
      40,     0,    38,     0,     0,     0,     0,    82,   184,   182,
     189,   190,   185,     0,   119,   172,    90,    89,     0,   136,
       0,    30,    26,     0,    27,     0,     0,    75,    76,     0,
     173,     0,     0,     0,     0,    19,    22,     0,     0,     0,
       0,     0,     0,    91,    43,    35,    34,    32,     0,   134,
     177,     0,     0,    25,    81,     0,     0,    68,     0,     0,
       0,   104,     0,    98,    71,    22,     0,     0,    73,    72,
       0,     0,     0,    17,     0,    28,     0,     0,    81,    77,
       0,     0,     0,     0,     0,     0,    97,    48,     0,    46,
      96,   178,    79,    80,     0,     0,    70,   101,   100,    99,
       0,     0,     0,     0,    78,     0,     0,     0,     0,    93,
      95,    47,    69,   103,   102,    94,     0,    92
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -349,  -349,  -349,  -349,   167,  -349,  -349,  -155,   -81,    34,
    -349,  -349,  -349,  -349,  -349,  -349,  -349,   -18,  -349,   165,
    -349,  -349,   -60,  -349,  -349,   127,  -349,  -349,  -349,  -349,
    -349,  -349,  -349,  -349,    46,  -349,  -321,  -349,   -74,  -349,
    -349,  -349,  -349,  -348,  -349,     3,   -29,  -349,   -96,  -349,
    -349,  -349,  -349,  -349,  -349,  -349,  -349,  -349,  -349,   351,
     -23,   386,   286,   148,   -13,  -349,   276,   277,   154,   184,
      38,  -349,   -49,  -349,   164,  -349,   174,  -349
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    14,    15,    16,    17,   144,   159,    27,
      18,    19,    20,    21,    22,    23,    24,    89,   155,   156,
     219,   260,   147,   213,   262,   263,   264,   265,   266,   267,
     268,   269,   306,   307,   308,   336,   286,   202,   287,   203,
     270,   271,   272,   273,   274,   157,    54,    55,   115,    56,
      57,    58,   175,    59,    60,    61,    62,    84,    85,    86,
     116,    63,   174,   206,   149,    65,    66,    67,    68,    69,
      70,    71,    97,   170,   171,   172,   173,   232
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      64,    83,   220,    29,   148,   114,   254,    87,    83,    30,
       3,   181,    83,   356,    95,   379,   142,    29,    99,    29,
     177,    26,   143,    72,   142,   101,   102,   247,    29,   160,
     143,    31,   317,   104,    83,    64,   318,   374,   394,    28,
     319,    32,    33,    34,   368,    36,   146,   169,   221,    73,
      83,    83,   388,   389,   222,   214,   150,   151,   146,   322,
     323,   215,  -105,   211,    35,    37,   369,   117,  -105,    64,
     234,    83,   161,   162,   163,    81,   165,   176,    83,   217,
     224,   226,   227,   168,   239,   105,   238,   108,   109,   110,
     111,   112,   154,   248,    26,    96,   137,     4,   113,     5,
     182,   225,   138,   228,     6,   230,   140,   141,   204,   278,
    -105,     7,     8,    96,   139,   215,     9,    10,  -105,    74,
     316,  -105,   334,    11,   205,   207,   146,    12,   335,   390,
      83,   158,   212,    75,    76,   215,   223,   130,   317,   281,
     294,    88,   318,   395,   397,   131,   319,   320,    83,   215,
     215,    83,    90,   261,   237,   114,   132,    98,   321,    91,
     345,    92,    77,   282,    13,   322,   323,   133,   195,   196,
     197,   198,   199,   200,   201,    94,   100,   235,   134,   135,
     136,   235,   169,   103,   291,   118,   301,   119,   145,   304,
     295,   146,   246,   152,   164,    83,   179,    83,    83,   153,
     276,   283,   277,   284,   285,   166,    83,    83,   126,   127,
     128,   129,   293,   296,   158,   180,   275,   208,   168,    77,
     290,   229,   209,   325,   326,   327,   210,    40,   216,   297,
     299,    77,   300,   218,   231,   346,  -187,  -188,    41,    40,
     311,   313,   314,    42,    43,    44,   337,   114,   236,    45,
      41,   341,    83,   343,   233,    42,    43,    44,   324,   315,
     240,    45,   120,   241,   364,   242,   243,   121,   244,   245,
     289,   122,   355,   123,   185,   186,   187,   188,   189,   190,
      78,   303,    79,    96,   249,    80,   305,    81,    82,   280,
     309,   382,   124,   125,    79,   329,   339,    80,   302,    81,
      82,   376,   377,   378,   347,   328,   387,   350,   351,   352,
     191,   192,   193,   194,   396,   215,   330,   331,   354,   332,
     333,   338,   357,   340,    77,   392,   393,   361,   342,   362,
     344,   348,    40,   349,   353,    83,   358,   250,    39,   363,
     251,   381,   360,    41,   252,    40,     6,   365,    42,    43,
      44,   253,    38,    39,    45,   383,    41,   254,   366,   367,
      40,    42,    43,    44,   370,   371,   255,    45,   372,   373,
     256,    41,   375,   384,   385,   386,    42,    43,    44,   391,
     259,   312,    45,   279,   359,   380,    93,    79,    25,   178,
      80,   298,   106,    82,   183,   288,   184,   146,     0,   257,
      46,     0,     0,    47,   258,     0,    48,   292,     0,     0,
      49,    50,     0,    51,     0,    46,    52,    53,    47,    38,
      39,    48,   107,     0,     0,    49,    50,    40,    51,     0,
       0,    52,    53,     0,    38,    39,     0,     0,    41,     0,
       0,     0,    40,    42,    43,    44,    38,    39,     0,    45,
       0,     0,     0,    41,    40,     0,     0,     0,    42,    43,
      44,     0,     0,     0,    45,    41,     0,     0,     0,     0,
      42,    43,    44,     0,     0,     0,    45,     0,     0,    96,
       0,     0,    46,     0,     0,    47,   167,     0,    48,     0,
       0,     0,    49,    50,     0,    51,     0,    46,    52,    53,
      47,     0,     0,    48,   107,     0,    96,    49,    50,    46,
      51,     0,    47,    52,    53,    48,    38,    39,     0,    49,
      50,     0,    51,     0,    40,    52,    53,     0,    38,    39,
       0,     0,     0,     0,     0,    41,    40,   310,    39,     0,
      42,    43,    44,     0,     0,    40,    45,    41,     0,     0,
       0,     0,    42,    43,    44,     0,    41,     0,    45,     0,
       0,    42,    43,    44,     0,     0,     0,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   257,    46,
       0,     0,    47,     0,     0,    48,     0,     0,     0,    49,
      50,    46,    51,     0,    47,    52,    53,    48,     0,     0,
      46,    49,    50,    47,    51,     0,    48,    52,    53,     0,
      49,    50,    77,    51,     0,     0,    52,    53,     0,     0,
      40,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    41,     0,     0,     0,     0,    42,    43,    44,     0,
       0,     0,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    79,     0,     0,    80,     0,
       0,    82
};

static const yytype_int16 yycheck[] =
{
      13,    30,   157,     3,    78,    54,    23,    30,    37,     6,
       0,   107,    41,   334,    37,   363,    12,     3,    41,     3,
     101,    66,    18,     5,    12,    43,    44,     3,     3,    89,
      18,     3,    37,    46,    63,    48,    41,   358,   386,     5,
      45,     7,     8,     9,    43,    11,    63,    96,    12,    31,
      79,    80,    64,    65,    18,    65,    79,    80,    63,    64,
      65,    71,    12,   144,     3,     3,    65,    73,    18,    82,
      70,   100,    90,    91,    92,    71,    94,   100,   107,   153,
     161,   162,   163,    96,   180,    47,    70,    49,    50,    51,
      52,    53,    67,    69,    66,    63,    66,     5,    66,     7,
     113,   161,    72,   163,    12,   165,    72,    73,   137,    65,
      60,    19,    20,    63,    84,    71,    24,    25,    68,     3,
     275,    71,    66,    31,   137,   138,    63,    35,    72,    65,
     159,    68,   145,     3,     3,    71,   159,    39,    37,   220,
     236,    66,    41,    65,    65,    47,    45,    46,   177,    71,
      71,   180,     3,   213,   177,   204,    58,    63,    57,     3,
     315,     3,     3,   223,    72,    64,    65,    69,   130,   131,
     132,   133,   134,   135,   136,     3,    72,   174,    80,    81,
      82,   178,   231,    63,   233,    51,   246,    50,    66,   249,
     239,    63,   210,     3,     3,   224,    67,   226,   227,    64,
     213,   224,   215,   226,   227,    65,   235,   236,    76,    77,
      78,    79,   235,   242,    68,    73,   213,     3,   231,     3,
     233,    66,     3,   283,   284,   285,     3,    11,    67,   242,
     243,     3,   245,    65,    71,   316,    85,    85,    22,    11,
     253,   254,   255,    27,    28,    29,   306,   296,    73,    33,
      22,   311,   281,   313,    85,    27,    28,    29,   281,   256,
      67,    33,    44,    67,   345,    71,    85,    49,    73,    66,
      70,    53,   332,    55,   120,   121,   122,   123,   124,   125,
      64,     3,    66,    63,    67,    69,     3,    71,    72,    67,
      66,   365,    74,    75,    66,    65,   309,    69,    67,    71,
      72,   361,   362,   363,   317,    67,   380,   320,   321,   322,
     126,   127,   128,   129,   388,    71,    73,    85,   331,    67,
      67,    71,   335,    43,     3,   385,   386,   340,    65,   342,
      65,    65,    11,    65,    65,   364,    66,     3,     4,    15,
       6,   364,    71,    22,    10,    11,    12,    60,    27,    28,
      29,    17,     3,     4,    33,   368,    22,    23,    65,    65,
      11,    27,    28,    29,    65,    73,    32,    33,    67,    73,
      36,    22,     3,    67,    67,    15,    27,    28,    29,    65,
     213,   254,    33,   218,   338,    64,    35,    66,     2,   103,
      69,   243,    43,    72,   118,   231,   119,    63,    -1,    65,
      66,    -1,    -1,    69,    70,    -1,    72,   233,    -1,    -1,
      76,    77,    -1,    79,    -1,    66,    82,    83,    69,     3,
       4,    72,    73,    -1,    -1,    76,    77,    11,    79,    -1,
      -1,    82,    83,    -1,     3,     4,    -1,    -1,    22,    -1,
      -1,    -1,    11,    27,    28,    29,     3,     4,    -1,    33,
      -1,    -1,    -1,    22,    11,    -1,    -1,    -1,    27,    28,
      29,    -1,    -1,    -1,    33,    22,    -1,    -1,    -1,    -1,
      27,    28,    29,    -1,    -1,    -1,    33,    -1,    -1,    63,
      -1,    -1,    66,    -1,    -1,    69,    70,    -1,    72,    -1,
      -1,    -1,    76,    77,    -1,    79,    -1,    66,    82,    83,
      69,    -1,    -1,    72,    73,    -1,    63,    76,    77,    66,
      79,    -1,    69,    82,    83,    72,     3,     4,    -1,    76,
      77,    -1,    79,    -1,    11,    82,    83,    -1,     3,     4,
      -1,    -1,    -1,    -1,    -1,    22,    11,     3,     4,    -1,
      27,    28,    29,    -1,    -1,    11,    33,    22,    -1,    -1,
      -1,    -1,    27,    28,    29,    -1,    22,    -1,    33,    -1,
      -1,    27,    28,    29,    -1,    -1,    -1,    33,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,
      -1,    -1,    69,    -1,    -1,    72,    -1,    -1,    -1,    76,
      77,    66,    79,    -1,    69,    82,    83,    72,    -1,    -1,
      66,    76,    77,    69,    79,    -1,    72,    82,    83,    -1,
      76,    77,     3,    79,    -1,    -1,    82,    83,    -1,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    22,    -1,    -1,    -1,    -1,    27,    28,    29,    -1,
      -1,    -1,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,    69,    -1,
      -1,    72
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    87,    88,     0,     5,     7,    12,    19,    20,    24,
      25,    31,    35,    72,    89,    90,    91,    92,    96,    97,
      98,    99,   100,   101,   102,   147,    66,    95,    95,     3,
     131,     3,    95,    95,    95,     3,    95,     3,     3,     4,
      11,    22,    27,    28,    29,    33,    66,    69,    72,    76,
      77,    79,    82,    83,   132,   133,   135,   136,   137,   139,
     140,   141,   142,   147,   150,   151,   152,   153,   154,   155,
     156,   157,     5,    31,     3,     3,     3,     3,    64,    66,
      69,    71,    72,   132,   143,   144,   145,   146,    66,   103,
       3,     3,     3,   145,     3,   146,    63,   158,    63,   146,
      72,   103,   103,    63,   150,   156,    43,    73,   156,   156,
     156,   156,   156,    66,   158,   134,   146,    73,    51,    50,
      44,    49,    53,    55,    74,    75,    76,    77,    78,    79,
      39,    47,    58,    69,    80,    81,    82,    66,    72,    84,
      95,    95,    12,    18,    93,    66,    63,   108,   124,   150,
     146,   146,     3,    64,    67,   104,   105,   131,    68,    94,
     108,   103,   103,   103,     3,   103,    65,    70,   150,   158,
     159,   160,   161,   162,   148,   138,   146,    94,   148,    67,
      73,   134,   150,   152,   153,   154,   154,   154,   154,   154,
     154,   155,   155,   155,   155,   156,   156,   156,   156,   156,
     156,   156,   123,   125,   132,   150,   149,   150,     3,     3,
       3,    94,   150,   109,    65,    71,    67,   124,    65,   106,
      93,    12,    18,   146,    94,   108,    94,    94,   108,    66,
     108,    71,   163,    85,    70,   131,    73,   146,    70,   134,
      67,    67,    71,    85,    73,    66,   103,     3,    69,    67,
       3,     6,    10,    17,    23,    32,    36,    65,    70,    90,
     107,   108,   110,   111,   112,   113,   114,   115,   116,   117,
     126,   127,   128,   129,   130,   131,   150,   150,    65,   105,
      67,    94,   108,   146,   146,   146,   122,   124,   160,    70,
     150,   158,   162,   146,   134,   158,   132,   150,   149,   150,
     150,   108,    67,     3,   108,     3,   118,   119,   120,    66,
       3,   150,   111,   150,   150,   131,    93,    37,    41,    45,
      46,    57,    64,    65,   146,   108,   108,   108,    67,    65,
      73,    85,    67,    67,    66,    72,   121,   108,    71,   150,
      43,   108,    65,   108,    65,    93,    94,   150,    65,    65,
     150,   150,   150,    65,   150,   108,   122,   150,    66,   120,
      71,   150,   150,    15,    94,    60,    65,    65,    43,    65,
      65,    73,    67,    73,   122,     3,   108,   108,   108,   129,
      64,   146,   124,   150,    67,    67,    15,   124,    64,    65,
      65,    65,   108,   108,   129,    65,   124,    65
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    86,    87,    88,    88,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    90,    90,    91,    92,    93,
      93,    93,    94,    94,    94,    95,    95,    96,    96,    97,
      97,    98,    99,    99,   100,   101,   101,   102,   102,   103,
     103,   104,   104,   105,   106,   106,   107,   107,   107,   108,
     109,   109,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   111,   111,   111,   111,   112,   113,   114,
     115,   116,   117,   117,   118,   118,   119,   119,   120,   120,
     121,   122,   122,   123,   123,   124,   124,   125,   125,   125,
     125,   126,   127,   127,   127,   127,   128,   128,   129,   129,
     129,   129,   129,   129,   130,   131,   131,   132,   132,   132,
     132,   132,   132,   132,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   144,   145,   146,
     146,   146,   147,   148,   148,   149,   149,   150,   151,   151,
     152,   152,   153,   153,   153,   153,   153,   153,   153,   154,
     154,   154,   154,   154,   155,   155,   155,   155,   155,   155,
     155,   155,   156,   156,   156,   156,   156,   156,   156,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   158,   158,   159,   159,   160,   160,   161,   161,   162,
     162,   163,   163
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     6,     5,     8,     4,     0,
       1,     1,     0,     2,     2,     7,     6,     7,     8,     5,
       6,     4,     7,     5,     7,     7,     5,     4,     6,     2,
       4,     1,     3,     4,     0,     1,     4,     6,     4,     3,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     7,
       5,     3,     3,     3,     0,     1,     1,     3,     5,     4,
       3,     0,     1,     0,     1,     1,     3,     1,     1,     3,
       3,     2,     8,     6,     7,     6,     4,     4,     3,     5,
       5,     5,     7,     7,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     4,     3,     5,
       1,     4,     2,     4,     2,     2,     1,     1,     1,     1,
       1,     3,     3,     0,     4,     0,     1,     1,     1,     3,
       1,     3,     1,     3,     3,     3,     3,     3,     3,     1,
       3,     3,     3,     3,     1,     3,     3,     3,     3,     3,
       3,     3,     1,     2,     2,     2,     2,     2,     2,     1,
       2,     2,     5,     1,     3,     3,     4,     6,     8,     4,
       4,     2,     4,     1,     3,     3,     1,     1,     1,     1,
       1,     0,     1
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
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1462 "parser.cpp" /* yacc.c:1257  */
        break;

    case 89: /* Definition  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1468 "parser.cpp" /* yacc.c:1257  */
        break;

    case 90: /* Const  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1474 "parser.cpp" /* yacc.c:1257  */
        break;

    case 91: /* Instance  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1480 "parser.cpp" /* yacc.c:1257  */
        break;

    case 92: /* TypeDecl  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1486 "parser.cpp" /* yacc.c:1257  */
        break;

    case 95: /* Receiver  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1492 "parser.cpp" /* yacc.c:1257  */
        break;

    case 96: /* Action  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1498 "parser.cpp" /* yacc.c:1257  */
        break;

    case 97: /* Reaction  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1504 "parser.cpp" /* yacc.c:1257  */
        break;

    case 98: /* Bind  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1510 "parser.cpp" /* yacc.c:1257  */
        break;

    case 99: /* Init  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1516 "parser.cpp" /* yacc.c:1257  */
        break;

    case 100: /* Getter  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1522 "parser.cpp" /* yacc.c:1257  */
        break;

    case 101: /* Method  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1528 "parser.cpp" /* yacc.c:1257  */
        break;

    case 102: /* Func  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1534 "parser.cpp" /* yacc.c:1257  */
        break;

    case 103: /* Signature  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1540 "parser.cpp" /* yacc.c:1257  */
        break;

    case 105: /* Parameter  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1546 "parser.cpp" /* yacc.c:1257  */
        break;

    case 107: /* BindStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1552 "parser.cpp" /* yacc.c:1257  */
        break;

    case 108: /* Block  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1558 "parser.cpp" /* yacc.c:1257  */
        break;

    case 110: /* Statement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1564 "parser.cpp" /* yacc.c:1257  */
        break;

    case 111: /* SimpleStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1570 "parser.cpp" /* yacc.c:1257  */
        break;

    case 112: /* EmptyStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1576 "parser.cpp" /* yacc.c:1257  */
        break;

    case 113: /* ActivateStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1582 "parser.cpp" /* yacc.c:1257  */
        break;

    case 114: /* ChangeStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1588 "parser.cpp" /* yacc.c:1257  */
        break;

    case 115: /* ForIotaStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1594 "parser.cpp" /* yacc.c:1257  */
        break;

    case 116: /* ReturnStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1600 "parser.cpp" /* yacc.c:1257  */
        break;

    case 117: /* IncrementStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1606 "parser.cpp" /* yacc.c:1257  */
        break;

    case 120: /* PushPortCall  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1612 "parser.cpp" /* yacc.c:1257  */
        break;

    case 121: /* IndexExpression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1618 "parser.cpp" /* yacc.c:1257  */
        break;

    case 126: /* ExpressionStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1624 "parser.cpp" /* yacc.c:1257  */
        break;

    case 127: /* VarStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1630 "parser.cpp" /* yacc.c:1257  */
        break;

    case 128: /* AssignmentStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1636 "parser.cpp" /* yacc.c:1257  */
        break;

    case 129: /* IfStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1642 "parser.cpp" /* yacc.c:1257  */
        break;

    case 130: /* WhileStatement  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1648 "parser.cpp" /* yacc.c:1257  */
        break;

    case 132: /* TypeLitExpression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1654 "parser.cpp" /* yacc.c:1257  */
        break;

    case 133: /* ArrayType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1660 "parser.cpp" /* yacc.c:1257  */
        break;

    case 134: /* ElementType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1666 "parser.cpp" /* yacc.c:1257  */
        break;

    case 135: /* StructType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1672 "parser.cpp" /* yacc.c:1257  */
        break;

    case 136: /* SliceType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1678 "parser.cpp" /* yacc.c:1257  */
        break;

    case 137: /* MapType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1684 "parser.cpp" /* yacc.c:1257  */
        break;

    case 138: /* KeyType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1690 "parser.cpp" /* yacc.c:1257  */
        break;

    case 139: /* ComponentType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1696 "parser.cpp" /* yacc.c:1257  */
        break;

    case 140: /* PushPortType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1702 "parser.cpp" /* yacc.c:1257  */
        break;

    case 141: /* PullPortType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1708 "parser.cpp" /* yacc.c:1257  */
        break;

    case 142: /* HeapType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1714 "parser.cpp" /* yacc.c:1257  */
        break;

    case 143: /* PointerType  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1720 "parser.cpp" /* yacc.c:1257  */
        break;

    case 144: /* TypeLit  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1726 "parser.cpp" /* yacc.c:1257  */
        break;

    case 145: /* TypeName  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1732 "parser.cpp" /* yacc.c:1257  */
        break;

    case 146: /* Type  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1738 "parser.cpp" /* yacc.c:1257  */
        break;

    case 147: /* ArrayDimension  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1744 "parser.cpp" /* yacc.c:1257  */
        break;

    case 149: /* OptionalExpression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1750 "parser.cpp" /* yacc.c:1257  */
        break;

    case 150: /* Expression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1756 "parser.cpp" /* yacc.c:1257  */
        break;

    case 151: /* OrExpression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1762 "parser.cpp" /* yacc.c:1257  */
        break;

    case 152: /* AndExpression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1768 "parser.cpp" /* yacc.c:1257  */
        break;

    case 153: /* CompareExpression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1774 "parser.cpp" /* yacc.c:1257  */
        break;

    case 154: /* AddExpression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1780 "parser.cpp" /* yacc.c:1257  */
        break;

    case 155: /* MultiplyExpression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1786 "parser.cpp" /* yacc.c:1257  */
        break;

    case 156: /* UnaryExpression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1792 "parser.cpp" /* yacc.c:1257  */
        break;

    case 157: /* PrimaryExpression  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1798 "parser.cpp" /* yacc.c:1257  */
        break;

    case 160: /* Element  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1804 "parser.cpp" /* yacc.c:1257  */
        break;

    case 161: /* Key  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1810 "parser.cpp" /* yacc.c:1257  */
        break;

    case 162: /* Value  */
#line 88 "parser.y" /* yacc.c:1257  */
      { /* Free the node ($$). */ }
#line 1816 "parser.cpp" /* yacc.c:1257  */
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
#line 160 "parser.y" /* yacc.c:1646  */
    { root = (yyvsp[0].list); }
#line 2104 "parser.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 163 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = new SourceFile (); }
#line 2110 "parser.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 164 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-1].list)->append ((yyvsp[0].node));   }
#line 2116 "parser.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 167 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2122 "parser.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 168 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2128 "parser.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 169 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2134 "parser.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 170 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2140 "parser.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 171 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2146 "parser.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 172 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2152 "parser.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 173 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2158 "parser.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 174 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2164 "parser.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 175 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2170 "parser.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 176 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2176 "parser.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 180 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Const ((yylsp[-5]), (yyvsp[-4].list), (yyvsp[-3].node), (yyvsp[-1].list)); }
#line 2182 "parser.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 182 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Const ((yylsp[-4]), (yyvsp[-3].list), new EmptyTypeSpec ((yylsp[-4])), (yyvsp[-1].list)); }
#line 2188 "parser.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 186 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Instance ((yylsp[-7]), (yyvsp[-6].identifier), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-2].list)); }
#line 2194 "parser.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 188 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ast::Type ((yylsp[-3]), (yyvsp[-2].identifier), (yyvsp[-1].node)); }
#line 2200 "parser.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 191 "parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Mutable;   }
#line 2206 "parser.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 192 "parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Immutable; }
#line 2212 "parser.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 193 "parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Foreign;   }
#line 2218 "parser.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 196 "parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Mutable;   }
#line 2224 "parser.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 197 "parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Immutable; }
#line 2230 "parser.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 198 "parser.y" /* yacc.c:1646  */
    { (yyval.mutability) = Foreign;   }
#line 2236 "parser.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 202 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Receiver ((yylsp[-6]), (yyvsp[-5].identifier), (yyvsp[-4].mutability), (yyvsp[-3].mutability), true, (yyvsp[-1].identifier));  }
#line 2242 "parser.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 204 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Receiver ((yylsp[-5]), (yyvsp[-4].identifier), (yyvsp[-3].mutability), (yyvsp[-2].mutability), false, (yyvsp[-1].identifier)); }
#line 2248 "parser.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 208 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Action ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2254 "parser.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 210 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new DimensionedAction ((yylsp[-7]), (yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2260 "parser.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 214 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Reaction ((yylsp[-4]), (yyvsp[-3].node), (yyvsp[-2].identifier), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2266 "parser.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 216 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new DimensionedReaction ((yylsp[-4]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].identifier), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2272 "parser.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 220 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Bind ((yylsp[-3]), (yyvsp[-2].node), (yyvsp[-1].identifier), (yyvsp[0].node)); }
#line 2278 "parser.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 224 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Initializer ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-3].node), (yyvsp[-2].mutability), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2284 "parser.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 226 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Initializer ((yylsp[-4]), (yyvsp[-3].node), (yyvsp[-2].identifier), (yyvsp[-1].node), Immutable, new EmptyTypeSpec ((yylsp[-4])), (yyvsp[0].node)); }
#line 2290 "parser.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 230 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Getter ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-3].node), (yyvsp[-2].mutability), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2296 "parser.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 234 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Method ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-4].identifier), (yyvsp[-3].node), (yyvsp[-2].mutability), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2302 "parser.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 236 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Method ((yylsp[-4]), (yyvsp[-3].node), (yyvsp[-2].identifier), (yyvsp[-1].node), Immutable, new EmptyTypeSpec ((yylsp[-4])), (yyvsp[0].node)); }
#line 2308 "parser.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 240 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Function ((yylsp[-3]), (yyvsp[-2].identifier), (yyvsp[-1].node), Immutable, new EmptyTypeSpec ((yylsp[-3])), (yyvsp[0].node)); }
#line 2314 "parser.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 242 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Function ((yylsp[-5]), (yyvsp[-4].identifier), (yyvsp[-3].node), (yyvsp[-2].mutability), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2320 "parser.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 246 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new SignatureTypeSpec (yyloc); }
#line 2326 "parser.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 248 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-2].list); }
#line 2332 "parser.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 252 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new SignatureTypeSpec ((yylsp[0])))->append ((yyvsp[0].node)); }
#line 2338 "parser.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 254 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node)); }
#line 2344 "parser.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 258 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IdentifierListTypeSpec ((yylsp[-3]), (yyvsp[-3].list), (yyvsp[-2].mutability), (yyvsp[-1].mutability), (yyvsp[0].node)); }
#line 2350 "parser.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 265 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BindPushPortStatement ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2356 "parser.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 267 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BindPushPortParamStatement ((yylsp[-5]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2362 "parser.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 269 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BindPullPortStatement ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2368 "parser.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 271 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].list); }
#line 2374 "parser.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 274 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = new ListStatement (yyloc); }
#line 2380 "parser.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 275 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-1].list)->append ((yyvsp[0].node)); }
#line 2386 "parser.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 278 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2392 "parser.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 279 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2398 "parser.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 280 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2404 "parser.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 281 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2410 "parser.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 282 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2416 "parser.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 283 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2422 "parser.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 284 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2428 "parser.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 285 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2434 "parser.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 286 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2440 "parser.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 287 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2446 "parser.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 288 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2452 "parser.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 291 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2458 "parser.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 292 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2464 "parser.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 293 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2470 "parser.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 294 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2476 "parser.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 296 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new EmptyStatement (yyloc); }
#line 2482 "parser.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 300 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ActivateStatement ((yylsp[-2]), (yyvsp[-1].list), (yyvsp[0].node)); }
#line 2488 "parser.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 304 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ChangeStatement ((yylsp[-6]), (yyvsp[-4].node), (yyvsp[-2].identifier), (yyvsp[0].node)); }
#line 2494 "parser.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 308 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ForIotaStatement ((yylsp[-4]), (yyvsp[-3].identifier), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2500 "parser.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 312 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ReturnStatement ((yylsp[-2]), (yyvsp[-1].node)); }
#line 2506 "parser.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 316 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IncrementDecrementStatement ((yylsp[-2]), (yyvsp[-2].node), IncrementDecrementStatement::Increment); }
#line 2512 "parser.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 318 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IncrementDecrementStatement ((yylsp[-2]), (yyvsp[-2].node), IncrementDecrementStatement::Decrement); }
#line 2518 "parser.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 322 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = new ListExpr (yyloc); }
#line 2524 "parser.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 324 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[0].list); }
#line 2530 "parser.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 328 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new ListExpr ((yylsp[0])))->append ((yyvsp[0].node)); }
#line 2536 "parser.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 330 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node)); }
#line 2542 "parser.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 334 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IndexedPushPortCallExpr ((yylsp[-4]), (yyvsp[-4].identifier), (yyvsp[-3].node), (yyvsp[-1].list)); }
#line 2548 "parser.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 336 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new PushPortCallExpr ((yylsp[-3]), (yyvsp[-3].identifier), (yyvsp[-1].list)); }
#line 2554 "parser.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 338 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 2560 "parser.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 341 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = new ListExpr (yyloc); }
#line 2566 "parser.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 342 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[0].list); }
#line 2572 "parser.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 345 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = new ListExpr (yyloc); }
#line 2578 "parser.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 346 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[0].list); }
#line 2584 "parser.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 350 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new ListExpr ((yylsp[0])))->append ((yyvsp[0].node)); }
#line 2590 "parser.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 352 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node)); }
#line 2596 "parser.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 356 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new ListExpr ((yylsp[0])))->append ((yyvsp[0].node)); }
#line 2602 "parser.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 358 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new ListExpr ((yylsp[0])))->append (new TypeExpression ((yylsp[0]), (yyvsp[0].node))); }
#line 2608 "parser.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 360 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node)); }
#line 2614 "parser.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 362 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append (new TypeExpression ((yylsp[-2]), (yyvsp[0].node))); }
#line 2620 "parser.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 366 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ExpressionStatement ((yylsp[-1]), (yyvsp[-1].node)); }
#line 2626 "parser.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 370 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new VarStatement ((yylsp[-7]), (yyvsp[-6].list), (yyvsp[-5].mutability), (yyvsp[-4].mutability), (yyvsp[-3].node), (yyvsp[-1].list)); }
#line 2632 "parser.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 372 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new VarStatement ((yylsp[-5]), (yyvsp[-4].list), (yyvsp[-3].mutability), (yyvsp[-2].mutability), (yyvsp[-1].node), new ListExpr ((yylsp[-5]))); }
#line 2638 "parser.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 374 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new VarStatement ((yylsp[-6]), (yyvsp[-5].list), (yyvsp[-4].mutability), (yyvsp[-3].mutability), new EmptyTypeSpec ((yylsp[-6])), (yyvsp[-1].list)); }
#line 2644 "parser.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 376 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new VarStatement ((yylsp[-5]), (yyvsp[-5].list), (yyvsp[-4].mutability), (yyvsp[-3].mutability), new EmptyTypeSpec ((yylsp[-5])), (yyvsp[-1].list)); }
#line 2650 "parser.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 380 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new AssignStatement ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2656 "parser.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 382 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new AddAssignStatement ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2662 "parser.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 386 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IfStatement ((yylsp[-2]), new EmptyStatement ((yylsp[-2])), (yyvsp[-1].node), (yyvsp[0].node), new ListStatement ((yylsp[-2]))); }
#line 2668 "parser.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 388 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IfStatement ((yylsp[-4]), new EmptyStatement ((yylsp[-4])), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2674 "parser.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 390 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IfStatement ((yylsp[-4]), new EmptyStatement ((yylsp[-4])), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2680 "parser.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 392 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IfStatement ((yylsp[-4]), (yyvsp[-3].node), (yyvsp[-1].node), (yyvsp[0].node), new ListStatement ((yylsp[-4]))); }
#line 2686 "parser.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 394 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IfStatement ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2692 "parser.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 396 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IfStatement ((yylsp[-6]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2698 "parser.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 400 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new WhileStatement ((yylsp[-2]), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2704 "parser.cpp" /* yacc.c:1646  */
    break;

  case 105:
#line 404 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new IdentifierList ((yylsp[0])))->append ((yyvsp[0].identifier)); }
#line 2710 "parser.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 406 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].identifier)); }
#line 2716 "parser.cpp" /* yacc.c:1646  */
    break;

  case 107:
#line 410 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2722 "parser.cpp" /* yacc.c:1646  */
    break;

  case 108:
#line 411 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2728 "parser.cpp" /* yacc.c:1646  */
    break;

  case 109:
#line 412 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2734 "parser.cpp" /* yacc.c:1646  */
    break;

  case 110:
#line 413 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2740 "parser.cpp" /* yacc.c:1646  */
    break;

  case 111:
#line 414 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2746 "parser.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 415 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2752 "parser.cpp" /* yacc.c:1646  */
    break;

  case 113:
#line 416 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2758 "parser.cpp" /* yacc.c:1646  */
    break;

  case 114:
#line 417 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2764 "parser.cpp" /* yacc.c:1646  */
    break;

  case 115:
#line 420 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ArrayTypeSpec ((yylsp[-1]), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2770 "parser.cpp" /* yacc.c:1646  */
    break;

  case 116:
#line 422 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2776 "parser.cpp" /* yacc.c:1646  */
    break;

  case 117:
#line 424 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].list); }
#line 2782 "parser.cpp" /* yacc.c:1646  */
    break;

  case 118:
#line 426 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new SliceTypeSpec ((yylsp[-2]), (yyvsp[0].node)); }
#line 2788 "parser.cpp" /* yacc.c:1646  */
    break;

  case 119:
#line 428 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new MapTypeSpec ((yylsp[-4]), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2794 "parser.cpp" /* yacc.c:1646  */
    break;

  case 120:
#line 430 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2800 "parser.cpp" /* yacc.c:1646  */
    break;

  case 121:
#line 434 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].list); static_cast<FieldListTypeSpec*> ((yyvsp[-1].list))->is_component = true; }
#line 2806 "parser.cpp" /* yacc.c:1646  */
    break;

  case 122:
#line 436 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new PushPortTypeSpec ((yylsp[-1]), (yyvsp[0].node)); }
#line 2812 "parser.cpp" /* yacc.c:1646  */
    break;

  case 123:
#line 440 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new PullPortTypeSpec ((yylsp[-3]), (yyvsp[-2].node), (yyvsp[-1].mutability), (yyvsp[0].node)); }
#line 2818 "parser.cpp" /* yacc.c:1646  */
    break;

  case 124:
#line 442 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new HeapTypeSpec ((yylsp[-1]), (yyvsp[0].node)); }
#line 2824 "parser.cpp" /* yacc.c:1646  */
    break;

  case 125:
#line 444 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new PointerTypeSpec ((yylsp[-1]), (yyvsp[0].node)); }
#line 2830 "parser.cpp" /* yacc.c:1646  */
    break;

  case 126:
#line 447 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2836 "parser.cpp" /* yacc.c:1646  */
    break;

  case 127:
#line 448 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2842 "parser.cpp" /* yacc.c:1646  */
    break;

  case 128:
#line 450 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IdentifierTypeSpec ((yylsp[0]), (yyvsp[0].identifier)); }
#line 2848 "parser.cpp" /* yacc.c:1646  */
    break;

  case 129:
#line 453 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2854 "parser.cpp" /* yacc.c:1646  */
    break;

  case 130:
#line 454 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2860 "parser.cpp" /* yacc.c:1646  */
    break;

  case 131:
#line 455 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 2866 "parser.cpp" /* yacc.c:1646  */
    break;

  case 132:
#line 457 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 2872 "parser.cpp" /* yacc.c:1646  */
    break;

  case 133:
#line 461 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = new FieldListTypeSpec (yyloc); }
#line 2878 "parser.cpp" /* yacc.c:1646  */
    break;

  case 134:
#line 463 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-3].list)->append (new IdentifierListTypeSpec ((yylsp[-3]), (yyvsp[-2].list), Mutable, Mutable, (yyvsp[-1].node))); }
#line 2884 "parser.cpp" /* yacc.c:1646  */
    break;

  case 135:
#line 467 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new EmptyExpr (yyloc); }
#line 2890 "parser.cpp" /* yacc.c:1646  */
    break;

  case 136:
#line 469 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2896 "parser.cpp" /* yacc.c:1646  */
    break;

  case 137:
#line 471 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2902 "parser.cpp" /* yacc.c:1646  */
    break;

  case 138:
#line 475 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2908 "parser.cpp" /* yacc.c:1646  */
    break;

  case 139:
#line 477 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), LogicOr, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2914 "parser.cpp" /* yacc.c:1646  */
    break;

  case 140:
#line 481 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2920 "parser.cpp" /* yacc.c:1646  */
    break;

  case 141:
#line 483 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), LogicAnd, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2926 "parser.cpp" /* yacc.c:1646  */
    break;

  case 142:
#line 487 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2932 "parser.cpp" /* yacc.c:1646  */
    break;

  case 143:
#line 489 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), Equal, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2938 "parser.cpp" /* yacc.c:1646  */
    break;

  case 144:
#line 491 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), NotEqual, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2944 "parser.cpp" /* yacc.c:1646  */
    break;

  case 145:
#line 493 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), LessThan, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2950 "parser.cpp" /* yacc.c:1646  */
    break;

  case 146:
#line 495 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), LessEqual, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2956 "parser.cpp" /* yacc.c:1646  */
    break;

  case 147:
#line 497 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), MoreThan, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2962 "parser.cpp" /* yacc.c:1646  */
    break;

  case 148:
#line 499 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), MoreEqual, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2968 "parser.cpp" /* yacc.c:1646  */
    break;

  case 149:
#line 503 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2974 "parser.cpp" /* yacc.c:1646  */
    break;

  case 150:
#line 505 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), Add, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2980 "parser.cpp" /* yacc.c:1646  */
    break;

  case 151:
#line 507 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), Subtract, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2986 "parser.cpp" /* yacc.c:1646  */
    break;

  case 152:
#line 509 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), BitOr, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2992 "parser.cpp" /* yacc.c:1646  */
    break;

  case 153:
#line 511 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), BitXor, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2998 "parser.cpp" /* yacc.c:1646  */
    break;

  case 154:
#line 515 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3004 "parser.cpp" /* yacc.c:1646  */
    break;

  case 155:
#line 517 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), Multiply, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3010 "parser.cpp" /* yacc.c:1646  */
    break;

  case 156:
#line 519 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), Divide, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3016 "parser.cpp" /* yacc.c:1646  */
    break;

  case 157:
#line 521 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), Modulus, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3022 "parser.cpp" /* yacc.c:1646  */
    break;

  case 158:
#line 523 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), LeftShift, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3028 "parser.cpp" /* yacc.c:1646  */
    break;

  case 159:
#line 525 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), RightShift, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3034 "parser.cpp" /* yacc.c:1646  */
    break;

  case 160:
#line 527 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), BitAnd, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3040 "parser.cpp" /* yacc.c:1646  */
    break;

  case 161:
#line 529 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new BinaryArithmeticExpr ((yylsp[-2]), BitAndNot, (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3046 "parser.cpp" /* yacc.c:1646  */
    break;

  case 162:
#line 532 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3052 "parser.cpp" /* yacc.c:1646  */
    break;

  case 163:
#line 533 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new UnaryArithmeticExpr ((yylsp[-1]), Posate, (yyvsp[0].node)); }
#line 3058 "parser.cpp" /* yacc.c:1646  */
    break;

  case 164:
#line 534 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new UnaryArithmeticExpr ((yylsp[-1]), Negate, (yyvsp[0].node)); }
#line 3064 "parser.cpp" /* yacc.c:1646  */
    break;

  case 165:
#line 535 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new UnaryArithmeticExpr ((yylsp[-1]), LogicNot, (yyvsp[0].node)); }
#line 3070 "parser.cpp" /* yacc.c:1646  */
    break;

  case 166:
#line 536 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new UnaryArithmeticExpr ((yylsp[-1]), Complement, (yyvsp[0].node)); }
#line 3076 "parser.cpp" /* yacc.c:1646  */
    break;

  case 167:
#line 537 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new DereferenceExpr ((yylsp[-1]), (yyvsp[0].node)); }
#line 3082 "parser.cpp" /* yacc.c:1646  */
    break;

  case 168:
#line 538 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new AddressOfExpr ((yylsp[-1]), (yyvsp[0].node)); }
#line 3088 "parser.cpp" /* yacc.c:1646  */
    break;

  case 169:
#line 542 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3094 "parser.cpp" /* yacc.c:1646  */
    break;

  case 170:
#line 544 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new CompositeLiteral ((yylsp[-1]), (yyvsp[-1].node), (yyvsp[0].list)); }
#line 3100 "parser.cpp" /* yacc.c:1646  */
    break;

  case 171:
#line 546 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new CompositeLiteral ((yylsp[-1]), new IdentifierTypeSpec ((yylsp[-1]), (yyvsp[-1].identifier)), (yyvsp[0].list)); }
#line 3106 "parser.cpp" /* yacc.c:1646  */
    break;

  case 172:
#line 548 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new CompositeLiteral ((yylsp[-4]), new ArrayTypeSpec ((yylsp[-4]), new EmptyExpr ((yylsp[-4])), (yyvsp[-1].node)), (yyvsp[0].list)); }
#line 3112 "parser.cpp" /* yacc.c:1646  */
    break;

  case 173:
#line 551 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IdentifierExpr ((yylsp[0]), (yyvsp[0].identifier)); }
#line 3118 "parser.cpp" /* yacc.c:1646  */
    break;

  case 174:
#line 553 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 3124 "parser.cpp" /* yacc.c:1646  */
    break;

  case 175:
#line 555 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new SelectExpr ((yylsp[-2]), (yyvsp[-2].node), (yyvsp[0].identifier)); }
#line 3130 "parser.cpp" /* yacc.c:1646  */
    break;

  case 176:
#line 557 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new IndexExpr ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 3136 "parser.cpp" /* yacc.c:1646  */
    break;

  case 177:
#line 559 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new SliceExpr ((yylsp[-5]), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node), new EmptyExpr (yyloc)); }
#line 3142 "parser.cpp" /* yacc.c:1646  */
    break;

  case 178:
#line 561 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new SliceExpr ((yylsp[-7]), (yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 3148 "parser.cpp" /* yacc.c:1646  */
    break;

  case 179:
#line 563 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new CallExpr ((yylsp[-3]), (yyvsp[-3].node), (yyvsp[-1].list)); }
#line 3154 "parser.cpp" /* yacc.c:1646  */
    break;

  case 180:
#line 565 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new ConversionExpr ((yylsp[-3]), new TypeExpression ((yylsp[-3]), (yyvsp[-3].node)), (yyvsp[-1].node)); }
#line 3160 "parser.cpp" /* yacc.c:1646  */
    break;

  case 181:
#line 568 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = new ElementList ((yylsp[-1])); }
#line 3166 "parser.cpp" /* yacc.c:1646  */
    break;

  case 182:
#line 569 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list);                          }
#line 3172 "parser.cpp" /* yacc.c:1646  */
    break;

  case 183:
#line 572 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (new ElementList ((yylsp[0])))->append ((yyvsp[0].node)); }
#line 3178 "parser.cpp" /* yacc.c:1646  */
    break;

  case 184:
#line 573 "parser.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[-2].list)->append ((yyvsp[0].node)); }
#line 3184 "parser.cpp" /* yacc.c:1646  */
    break;

  case 185:
#line 576 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Element ((yylsp[-2]), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3190 "parser.cpp" /* yacc.c:1646  */
    break;

  case 186:
#line 577 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = new Element ((yylsp[0]), new EmptyExpr ((yylsp[0])), (yyvsp[0].node)); }
#line 3196 "parser.cpp" /* yacc.c:1646  */
    break;

  case 187:
#line 580 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3202 "parser.cpp" /* yacc.c:1646  */
    break;

  case 188:
#line 581 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].list); }
#line 3208 "parser.cpp" /* yacc.c:1646  */
    break;

  case 189:
#line 584 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 3214 "parser.cpp" /* yacc.c:1646  */
    break;

  case 190:
#line 585 "parser.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].list); }
#line 3220 "parser.cpp" /* yacc.c:1646  */
    break;


#line 3224 "parser.cpp" /* yacc.c:1646  */
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
#line 591 "parser.y" /* yacc.c:1906  */

