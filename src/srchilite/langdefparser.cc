/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse         langdef_parse
#define yylex           langdef_lex
#define yyerror         langdef_error
#define yylval          langdef_lval
#define yychar          langdef_char
#define yydebug         langdef_debug
#define yynerrs         langdef_nerrs
#define yylloc          langdef_lloc

/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "../../../lib/srchilite/langdefparser.yy"

/*
 * Copyright (C) 1999-2009 Lorenzo Bettini <http://www.lorenzobettini.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>

#include "stringdef.h"
#include "statelangelem.h"
#include "statestartlangelem.h"
#include "stringlistlangelem.h"
#include "delimitedlangelem.h"
#include "langelems.h"
#include "langdefparserfun.h"
#include "langdefscanner.h"
#include "vardefinitions.h"
#include "namedsubexpslangelem.h"
#include "parserexception.h"
#include "ioexception.h"

using std::cerr;
using std::string;

using namespace srchilite;

extern int langdef_lex() ;
extern FILE *langdef_in;
extern ParseStructPtr parsestruct;

static void yyerror( const char *s ) ;
static void yyerror( const string &s ) ;

/// the buffer for storing errors
static string errorBuffer;

/// this is where the parsed elements are store
LangElems *current_lang_elems = 0;

VarDefinitions *vardefinitions = 0;

/// used to record that the error is due to an included file not found
static bool includedFileNotFound = false;

#define UPDATE_REDEF(el, r) if (r == 1) { el->setRedef(); } else if (r == 2) { el->setSubst(); }
#define ADD_ELEMENT(elems, elem) \
  if (elem) { \
    if (elem->isRedef()) \
      elems->redef(elem); \
    else if (elem->isSubst()) \
      elems->subst(elem); \
    else \
      elems->add(elem); \
  }

struct Key : public ParserInfo
{
  const string *key;

  ~Key() { 
  	/* the string belongs to the string table so it is
  	   deallocated automatically by clearing the scanner */ 
  }
};

// this is a trick since ElementNames is a typedef and cannot
// be used in the union below
struct ElementNamesList : ElementNames {
};



/* Line 268 of yacc.c  */
#line 174 "../../../lib/srchilite/langdefparser.cc"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BEGIN_T = 258,
     END_T = 259,
     ENVIRONMENT_T = 260,
     STATE_T = 261,
     MULTILINE_T = 262,
     DELIM_T = 263,
     START_T = 264,
     ESCAPE_T = 265,
     NESTED_T = 266,
     EXIT_ALL = 267,
     EXIT_T = 268,
     VARDEF_T = 269,
     REDEF_T = 270,
     SUBST_T = 271,
     NONSENSITIVE_T = 272,
     WRONG_BACKREFERENCE = 273,
     LEVEL = 274,
     KEY = 275,
     STRINGDEF = 276,
     REGEXPNOPREPROC = 277,
     VARUSE = 278,
     BACKREFVAR = 279,
     WRONG_INCLUDE_FILE = 280,
     REGEXPDEF = 281
   };
#endif
/* Tokens.  */
#define BEGIN_T 258
#define END_T 259
#define ENVIRONMENT_T 260
#define STATE_T 261
#define MULTILINE_T 262
#define DELIM_T 263
#define START_T 264
#define ESCAPE_T 265
#define NESTED_T 266
#define EXIT_ALL 267
#define EXIT_T 268
#define VARDEF_T 269
#define REDEF_T 270
#define SUBST_T 271
#define NONSENSITIVE_T 272
#define WRONG_BACKREFERENCE 273
#define LEVEL 274
#define KEY 275
#define STRINGDEF 276
#define REGEXPNOPREPROC 277
#define VARUSE 278
#define BACKREFVAR 279
#define WRONG_INCLUDE_FILE 280
#define REGEXPDEF 281




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 95 "../../../lib/srchilite/langdefparser.yy"

  int tok ; /* command */
  bool booloption ;
  const std::string * string ; /* string : id, ... */
  class srchilite::StringDef *stringdef;
  class StringDefs *stringdefs;
  class LangElem *langelem;
  class StateLangElem *statelangelem;
  class StateStartLangElem *statestartlangelem;
  class LangElems *langelems;
  class NamedSubExpsLangElem *namedsubexpslangelem;
  struct Key *key;
  struct ElementNamesList *keys;
  int flag ;
  unsigned int level;



/* Line 293 of yacc.c  */
#line 281 "../../../lib/srchilite/langdefparser.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 306 "../../../lib/srchilite/langdefparser.cc"

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
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
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
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   81

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  49
/* YYNRULES -- Number of states.  */
#define YYNSTATES  71

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   281

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      28,    29,     2,    31,    30,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    27,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     6,     9,    11,    15,    22,    27,
      29,    37,    41,    46,    52,    54,    58,    60,    63,    64,
      66,    69,    70,    72,    73,    75,    77,    79,    80,    82,
      84,    85,    87,    88,    90,    91,    95,    97,    99,   101,
     103,   105,   107,   111,   113,   115,   117,   119,   121,   125
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      33,     0,    -1,    -1,    34,    -1,    34,    35,    -1,    35,
      -1,    44,    36,    40,    -1,    44,    42,    36,     3,    34,
       4,    -1,    14,    20,    27,    47,    -1,    25,    -1,    37,
       8,    49,    48,    39,    43,    45,    -1,    37,     9,    49,
      -1,    37,    27,    47,    46,    -1,    28,    38,    29,    27,
      22,    -1,    20,    -1,    38,    30,    20,    -1,    20,    -1,
      10,    49,    -1,    -1,    12,    -1,    13,    41,    -1,    -1,
      19,    -1,    -1,     5,    -1,     6,    -1,     7,    -1,    -1,
      15,    -1,    16,    -1,    -1,    11,    -1,    -1,    17,    -1,
      -1,    47,    30,    49,    -1,    49,    -1,    26,    -1,    21,
      -1,    22,    -1,    23,    -1,    24,    -1,    48,    31,    48,
      -1,    18,    -1,    26,    -1,    21,    -1,    22,    -1,    23,
      -1,    49,    31,    49,    -1,    18,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   138,   138,   147,   150,   154,   160,   169,   177,   181,
     192,   199,   204,   210,   216,   223,   228,   235,   236,   239,
     240,   241,   244,   245,   248,   249,   252,   253,   256,   257,
     258,   261,   262,   265,   266,   269,   270,   275,   278,   281,
     284,   291,   295,   300,   310,   313,   316,   319,   326,   331
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "BEGIN_T", "END_T", "ENVIRONMENT_T",
  "STATE_T", "MULTILINE_T", "DELIM_T", "START_T", "ESCAPE_T", "NESTED_T",
  "EXIT_ALL", "EXIT_T", "VARDEF_T", "REDEF_T", "SUBST_T", "NONSENSITIVE_T",
  "WRONG_BACKREFERENCE", "LEVEL", "KEY", "STRINGDEF", "REGEXPNOPREPROC",
  "VARUSE", "BACKREFVAR", "WRONG_INCLUDE_FILE", "REGEXPDEF", "'='", "'('",
  "')'", "','", "'+'", "$accept", "allelements", "elemdefs", "elemdef",
  "complexelem", "key", "keys", "escapedef", "exitlevel", "level",
  "startnewenv", "multiline", "redefsubst", "nested", "nonsensitive",
  "stringdefs", "stringdefwreferences", "stringdef", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,    61,    40,    41,
      44,    43
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    32,    33,    33,    34,    34,    35,    35,    35,    35,
      36,    36,    36,    36,    37,    38,    38,    39,    39,    40,
      40,    40,    41,    41,    42,    42,    43,    43,    44,    44,
      44,    45,    45,    46,    46,    47,    47,    48,    48,    48,
      48,    48,    48,    48,    49,    49,    49,    49,    49,    49
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     2,     1,     3,     6,     4,     1,
       7,     3,     4,     5,     1,     3,     1,     2,     0,     1,
       2,     0,     1,     0,     1,     1,     1,     0,     1,     1,
       0,     1,     0,     1,     0,     3,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     1,     1,     1,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      30,     0,    28,    29,     9,     0,    30,     5,     0,     0,
       1,     4,    24,    25,    14,     0,    21,     0,     0,     0,
      16,     0,    19,    23,     6,     0,     0,     0,     0,    49,
      45,    46,    47,    44,     8,    36,     0,     0,    22,    20,
       0,    11,    34,    30,     0,     0,     0,    15,    43,    38,
      39,    40,    41,    37,    18,    33,    12,    30,    35,    48,
      13,     0,     0,    27,     7,    17,    42,    26,    32,    31,
      10
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     5,     6,     7,    16,    17,    21,    63,    24,    39,
      18,    68,     8,    70,    56,    34,    54,    35
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -25
static const yytype_int8 yypact[] =
{
       3,   -13,   -25,   -25,   -25,    31,     8,   -25,     6,    11,
     -25,   -25,   -25,   -25,   -25,    23,    -3,    21,   -15,    50,
     -25,    17,   -25,    16,   -25,    50,    50,    50,    47,   -25,
     -25,   -25,   -25,   -25,    25,    26,    29,    39,   -25,   -25,
      18,    26,    15,    38,    50,    50,    36,   -25,   -25,   -25,
     -25,   -25,   -25,   -25,    -4,   -25,   -25,     0,    26,    26,
     -25,    50,    43,    53,   -25,    26,    44,   -25,    51,   -25,
     -25
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -25,   -25,    27,    -6,    56,   -25,   -25,   -25,   -25,   -25,
     -25,   -25,   -25,   -25,   -25,    52,    19,   -24
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -4
static const yytype_int8 yytable[] =
{
      11,    40,    41,    -2,    64,    14,    61,     9,    -3,    22,
      23,    12,    13,    15,     1,     2,     3,     1,     2,     3,
      58,    59,     1,     2,     3,     4,    14,    62,     4,    25,
      26,    10,    55,     4,    15,    38,    48,    65,    19,    49,
      50,    51,    52,    20,    53,    44,    36,    37,    27,    45,
      43,    11,     1,     2,     3,    44,    46,    45,    60,    47,
      67,    48,    69,     4,    49,    50,    51,    52,    29,    53,
      57,    30,    31,    32,    28,    62,    33,     0,     0,    42,
       0,    66
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-25))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
       6,    25,    26,     0,     4,    20,    10,    20,     0,    12,
      13,     5,     6,    28,    14,    15,    16,    14,    15,    16,
      44,    45,    14,    15,    16,    25,    20,    31,    25,     8,
       9,     0,    17,    25,    28,    19,    18,    61,    27,    21,
      22,    23,    24,    20,    26,    30,    29,    30,    27,    31,
       3,    57,    14,    15,    16,    30,    27,    31,    22,    20,
       7,    18,    11,    25,    21,    22,    23,    24,    18,    26,
      43,    21,    22,    23,    18,    31,    26,    -1,    -1,    27,
      -1,    62
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    14,    15,    16,    25,    33,    34,    35,    44,    20,
       0,    35,     5,     6,    20,    28,    36,    37,    42,    27,
      20,    38,    12,    13,    40,     8,     9,    27,    36,    18,
      21,    22,    23,    26,    47,    49,    29,    30,    19,    41,
      49,    49,    47,     3,    30,    31,    27,    20,    18,    21,
      22,    23,    24,    26,    48,    17,    46,    34,    49,    49,
      22,    10,    31,    39,     4,    49,    48,     7,    43,    11,
      45
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
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
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
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

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

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

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 34: /* "elemdefs" */

/* Line 1391 of yacc.c  */
#line 130 "../../../lib/srchilite/langdefparser.yy"
	{
	//std::cout << "freeing discarded symbol" << std::endl;
	delete (yyvaluep->langelems);
};

/* Line 1391 of yacc.c  */
#line 1323 "../../../lib/srchilite/langdefparser.cc"
	break;
      case 35: /* "elemdef" */

/* Line 1391 of yacc.c  */
#line 130 "../../../lib/srchilite/langdefparser.yy"
	{
	//std::cout << "freeing discarded symbol" << std::endl;
	delete (yyvaluep->langelem);
};

/* Line 1391 of yacc.c  */
#line 1335 "../../../lib/srchilite/langdefparser.cc"
	break;
      case 37: /* "key" */

/* Line 1391 of yacc.c  */
#line 130 "../../../lib/srchilite/langdefparser.yy"
	{
	//std::cout << "freeing discarded symbol" << std::endl;
	delete (yyvaluep->key);
};

/* Line 1391 of yacc.c  */
#line 1347 "../../../lib/srchilite/langdefparser.cc"
	break;
      case 38: /* "keys" */

/* Line 1391 of yacc.c  */
#line 130 "../../../lib/srchilite/langdefparser.yy"
	{
	//std::cout << "freeing discarded symbol" << std::endl;
	delete (yyvaluep->keys);
};

/* Line 1391 of yacc.c  */
#line 1359 "../../../lib/srchilite/langdefparser.cc"
	break;
      case 47: /* "stringdefs" */

/* Line 1391 of yacc.c  */
#line 130 "../../../lib/srchilite/langdefparser.yy"
	{
	//std::cout << "freeing discarded symbol" << std::endl;
	delete (yyvaluep->stringdefs);
};

/* Line 1391 of yacc.c  */
#line 1371 "../../../lib/srchilite/langdefparser.cc"
	break;
      case 49: /* "stringdef" */

/* Line 1391 of yacc.c  */
#line 130 "../../../lib/srchilite/langdefparser.yy"
	{
	//std::cout << "freeing discarded symbol" << std::endl;
	delete (yyvaluep->stringdef);
};

/* Line 1391 of yacc.c  */
#line 1383 "../../../lib/srchilite/langdefparser.cc"
	break;

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
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
  int yytoken;
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

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

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
      yychar = YYLEX;
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
  *++yyvsp = yylval;
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
     `$$ = $1'.

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

/* Line 1806 of yacc.c  */
#line 138 "../../../lib/srchilite/langdefparser.yy"
    {
          /* no definitions (i.e., empty a .lang file with no definition) */
          /* such as, default.lang */
          /* synthetize a normal elem that catches everything */
          current_lang_elems = new LangElems;
          StringDefs *defs = new StringDefs;
          defs->push_back (new StringDef("(?:.+)"));
          current_lang_elems->add(new StringListLangElem("normal", defs, false));
        }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 147 "../../../lib/srchilite/langdefparser.yy"
    { current_lang_elems = (yyvsp[(1) - (1)].langelems); }
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 150 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.langelems) = (yyvsp[(1) - (2)].langelems);
              ADD_ELEMENT((yyval.langelems), (yyvsp[(2) - (2)].langelem));
           }
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 154 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.langelems) = new LangElems;
              ADD_ELEMENT((yyval.langelems), (yyvsp[(1) - (1)].langelem));
          }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 161 "../../../lib/srchilite/langdefparser.yy"
    {
            (yyval.langelem) = (yyvsp[(2) - (3)].statestartlangelem);
            if ((yyvsp[(3) - (3)].tok) < 0)
              (yyvsp[(2) - (3)].statestartlangelem)->setExitAll();
            if ((yyvsp[(3) - (3)].tok) > 0)
              (yyvsp[(2) - (3)].statestartlangelem)->setExit((yyvsp[(3) - (3)].tok));
            UPDATE_REDEF((yyvsp[(2) - (3)].statestartlangelem), (yyvsp[(1) - (3)].tok));
          }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 170 "../../../lib/srchilite/langdefparser.yy"
    {
            StateLangElem *statelangelem = new StateLangElem((yyvsp[(3) - (6)].statestartlangelem)->getName(), (yyvsp[(3) - (6)].statestartlangelem), (yyvsp[(5) - (6)].langelems), !(yyvsp[(2) - (6)].booloption));
            (yyval.langelem) = statelangelem;
            (yyvsp[(3) - (6)].statestartlangelem)->setStateLangElem(statelangelem);
            
            UPDATE_REDEF((yyval.langelem), (yyvsp[(1) - (6)].tok));
          }
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 177 "../../../lib/srchilite/langdefparser.yy"
    {
            vardefinitions->add(*(yyvsp[(2) - (4)].string), (yyvsp[(4) - (4)].stringdefs));
            (yyval.langelem) = 0;
          }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 181 "../../../lib/srchilite/langdefparser.yy"
    {
            // this token is used by the scanner to signal an error
            // in opening an include file
            includedFileNotFound = true;
            yyerror("cannot open include file " + *(yyvsp[(1) - (1)].string));
            YYERROR;
            
            (yyval.langelem) = 0;
          }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 193 "../../../lib/srchilite/langdefparser.yy"
    {
                  (yyval.statestartlangelem) = new DelimitedLangElem(*((yyvsp[(1) - (7)].key)->key), (yyvsp[(3) - (7)].stringdef), (yyvsp[(4) - (7)].stringdef), (yyvsp[(5) - (7)].stringdef), (yyvsp[(6) - (7)].booloption), (yyvsp[(7) - (7)].booloption));
                  (yyval.statestartlangelem)->setParserInfo((yyvsp[(1) - (7)].key));
                  
                  delete (yyvsp[(1) - (7)].key);
                }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 199 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.statestartlangelem) = new DelimitedLangElem(*((yyvsp[(1) - (3)].key)->key), (yyvsp[(3) - (3)].stringdef), 0, 0, false, false);
              (yyval.statestartlangelem)->setParserInfo((yyvsp[(1) - (3)].key));
              delete (yyvsp[(1) - (3)].key);
            }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 204 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.statestartlangelem) = new StringListLangElem(*((yyvsp[(1) - (4)].key)->key), (yyvsp[(3) - (4)].stringdefs), (yyvsp[(4) - (4)].booloption));
              (yyval.statestartlangelem)->setParserInfo((yyvsp[(1) - (4)].key));
              
              delete (yyvsp[(1) - (4)].key);
            }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 210 "../../../lib/srchilite/langdefparser.yy"
    {
          		(yyval.statestartlangelem) = new NamedSubExpsLangElem((yyvsp[(2) - (5)].keys), new StringDef(*(yyvsp[(5) - (5)].string)));
                        (yyval.statestartlangelem)->setParserInfo(parsestruct->file_name, (yylsp[(1) - (5)]).first_line);
            }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 216 "../../../lib/srchilite/langdefparser.yy"
    {
    (yyval.key) = new Key;
    (yyval.key)->key = (yyvsp[(1) - (1)].string);
    (yyval.key)->setParserInfo(parsestruct->file_name, (yylsp[(1) - (1)]).first_line);
  }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 224 "../../../lib/srchilite/langdefparser.yy"
    {
        (yyval.keys) = (yyvsp[(1) - (3)].keys);
        (yyval.keys)->push_back(*(yyvsp[(3) - (3)].string));
    }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 229 "../../../lib/srchilite/langdefparser.yy"
    {
    	(yyval.keys) = new ElementNamesList;
    	(yyval.keys)->push_back(*(yyvsp[(1) - (1)].string));
    }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 235 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.stringdef) = (yyvsp[(2) - (2)].stringdef); }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 236 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.stringdef) = 0; }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 239 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.tok) = -1; }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 240 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.tok) = (yyvsp[(2) - (2)].level); }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 241 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.tok) = 0; }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 244 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.level) = (yyvsp[(1) - (1)].level); }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 245 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.level) = 1; }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 248 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.booloption) = true; }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 249 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.booloption) = false; }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 252 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.booloption) = true; }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 253 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.booloption) = false; }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 256 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.tok) = 1; }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 257 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.tok) = 2; }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 258 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.tok) = 0; }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 261 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.booloption) = true; }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 262 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.booloption) = false; }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 265 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.booloption) = true; }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 266 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.booloption) = false; }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 269 "../../../lib/srchilite/langdefparser.yy"
    { (yyval.stringdefs) = (yyvsp[(1) - (3)].stringdefs); (yyval.stringdefs)->push_back((yyvsp[(3) - (3)].stringdef)); }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 270 "../../../lib/srchilite/langdefparser.yy"
    {
                (yyval.stringdefs) = new StringDefs;
                (yyval.stringdefs)->push_back((yyvsp[(1) - (1)].stringdef)); }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 275 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.stringdef) = (yyvsp[(1) - (1)].stringdef);
            }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 278 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.stringdef) = new StringDef(*(yyvsp[(1) - (1)].string), true);
            }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 281 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.stringdef) = new StringDef(*(yyvsp[(1) - (1)].string));
            }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 284 "../../../lib/srchilite/langdefparser.yy"
    {
              if (! vardefinitions->contains(*(yyvsp[(1) - (1)].string))) {
                yyerror("undefined variable " + *(yyvsp[(1) - (1)].string));
                YYERROR;
              }
              (yyval.stringdef) = new StringDef(vardefinitions->getVar(*(yyvsp[(1) - (1)].string)));
            }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 291 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.stringdef) = new StringDef(*(yyvsp[(1) - (1)].string));
              (yyval.stringdef)->setBackRef(true);
            }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 295 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.stringdef) = StringDef::concat((yyvsp[(1) - (3)].stringdef), (yyvsp[(3) - (3)].stringdef));
              delete (yyvsp[(1) - (3)].stringdef);
              delete (yyvsp[(3) - (3)].stringdef);
            }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 300 "../../../lib/srchilite/langdefparser.yy"
    {
            // this token is used by the scanner to signal an error
            // in scanning a string with backreference
            yyerror("backreferences are allowed only inside ` `");
            YYERROR;
            
            (yyval.stringdef) = 0;
          }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 310 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.stringdef) = (yyvsp[(1) - (1)].stringdef);
            }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 313 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.stringdef) = new StringDef(*(yyvsp[(1) - (1)].string), true);
            }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 316 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.stringdef) = new StringDef(*(yyvsp[(1) - (1)].string));
            }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 319 "../../../lib/srchilite/langdefparser.yy"
    {
              if (! vardefinitions->contains(*(yyvsp[(1) - (1)].string))) {
                yyerror("undefined variable " + *(yyvsp[(1) - (1)].string));
                YYERROR;
              }
              (yyval.stringdef) = new StringDef(vardefinitions->getVar(*(yyvsp[(1) - (1)].string)));
            }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 326 "../../../lib/srchilite/langdefparser.yy"
    {
              (yyval.stringdef) = StringDef::concat((yyvsp[(1) - (3)].stringdef), (yyvsp[(3) - (3)].stringdef));
              delete (yyvsp[(1) - (3)].stringdef);
              delete (yyvsp[(3) - (3)].stringdef);
            }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 331 "../../../lib/srchilite/langdefparser.yy"
    {
            // this token is used by the scanner to signal an error
            // in scanning a string with backreference
            yyerror("backreferences are allowed only inside ` `");
            YYERROR;
            
            (yyval.stringdef) = 0;
          }
    break;



/* Line 1806 of yacc.c  */
#line 2165 "../../../lib/srchilite/langdefparser.cc"
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

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
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
  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

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

  *++yyvsp = yylval;

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

#if !defined(yyoverflow) || YYERROR_VERBOSE
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
  /* Do not reclaim the symbols of the rule which action triggered
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 341 "../../../lib/srchilite/langdefparser.yy"


extern int langdef_lex_destroy (void);

void
yyerror( const char *s )
{
  errorBuffer = s;
}

void
yyerror( const string &s )
{
  yyerror(s.c_str());
}

namespace srchilite {

LangElems *
parse_lang_def()
{
  return parse_lang_def("", "stdin");
}

LangElems *
parse_lang_def(const char *path, const char *name)
{
  current_lang_elems = 0;
  includedFileNotFound = false;
  vardefinitions = new VarDefinitions;
  parsestruct = ParseStructPtr(new ParseStruct(path, name));
  
  errorBuffer = "";
  int result = 1;
  bool fileNotFound = false;
  
  try {
  	if (strcmp(name, "stdin") != 0)
      open_file_to_scan(path, name);
  } catch (IOException &e) {
    errorBuffer = e.message;
    fileNotFound = true;
  }

  if (!fileNotFound)
     result = langdef_parse();

  delete vardefinitions;
  
  vardefinitions = 0;

  if (result != 0 && ! fileNotFound) {
  	  // make sure the input file is closed
	  close_langdefinputfile();
	  // close it before clearing the scanner
  }
  
  // release scanner memory
  clear_langdefscanner ();

  if (result != 0 || errorBuffer.size()) {
  	if (fileNotFound || includedFileNotFound) {
	  if (current_lang_elems) delete current_lang_elems;
	  throw ParserException(errorBuffer);
  	} else {
	  ParserException e(errorBuffer, parsestruct.get());
	  if (current_lang_elems) delete current_lang_elems;
	  throw e;
	}
  }

  return current_lang_elems;
}

}

