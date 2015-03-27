
/*  A Bison parser, made from parser.yac
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	ID	258
#define	ID1	259
#define	INTEGER	260
#define	UNKNOWN	261
#define	NETWORK	262
#define	END	263
#define	AND	264
#define	FF	265
#define	OR	266
#define	C_ELEMENT	267
#define	NOT	268
#define	BUFFER	269
#define	AOI12	270
#define	AOI22	271
#define	OAI12	272
#define	OAI22	273
#define	TABLE1	274
#define	SPEC	275
#define	HIDDEN	276
#define	ANY	277
#define	NAND	278
#define	NOR	279
#define	ME	280
#define	INIT	281
#define	EXTERNALS	282
#define	BDD_ORDER	283
#define	SUBCIRCUITS	284
#define	NONE	285
#define	FANOUTS	286
#define	GATE_LIST	287
#define	ENTITY	288
#define	INTERNAL_STATUS	289

#line 1 "parser.yac"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "basic.h"
#include "names.h"
#include "type.h"
#include "sphinx.h"

int yyerror (char*);
int yylex ();
int i;

extern int dflag;
extern char yytext[];
extern int  input_no, input_cur;
extern int vidaline_num;
extern input_ptr temp_input;
extern gate_ptr /* *list_of_gates */ gate_list, spec_list;
extern char Current_Module_Name[];
extern FILE *vidain;
char	Current_Module_Type[30];
extern char yytext[];
extern context_ptr	cur_cir;
extern array_t *place_array, *input_array, *output_array;


int line_num = 1;
name_ptr	nptr;
char		*str;

extern int debug_parse;
/*
extern name_ptr Name_Enter (char *Name, int type);
extern input_ptr Create_Input_ZArray_N (int no);
extern input_ptr Enter_Input_Signal_Chain (void);
extern signal_ptr Enter_Signal_Chain (void) ;
extern void Create_Subcircuits (void);
extern gate_ptr *Gate_Chain_to_ZArray (gate_ptr gptr, int num);
extern void Associate_Signal (char *formal, char *actual);
extern void Enter_Module (char *name, signal_ptr *spptr, sig_type mode);
extern void Set_Fanouts (void);
extern void Enter_Level_Externals (signal_ptr sptr);
extern void New_Specification (char *spec_name);
*/
#define GATE 	1
#define SIGNAL 	2


#line 51 "parser.yac"
typedef union {
	char *sval;
	component_type cval;
	int	ival;
	gate_ptr gptr;
	signal_ptr sptr;
	name_ptr   nptr;
	input_ptr  inptr;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		123
#define	YYFLAG		-32768
#define	YYNTBASE	41

#define YYTRANSLATE(x) ((unsigned)(x) <= 289 ? yytranslate[x] : 79)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,    39,     2,     2,     2,     2,     2,
     2,     2,    38,     2,    40,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    37,     2,    35,
     2,    36,     2,     2,     2,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     6,     8,    12,    14,    20,    25,    27,    28,    31,
    32,    36,    40,    42,    45,    47,    49,    53,    55,    58,
    62,    64,    66,    71,    75,    77,    80,    87,    89,    91,
    93,    95,    97,    99,   101,   103,   105,   107,   109,   111,
   113,   115,   117,   119,   123,   127,   131,   132,   134,   137,
   139,   142,   144,   146,   149,   152,   155,   160,   162,   165,
   168,   170,   173,   175,   178,   182,   187,   189,   192,   194,
   196
};

static const short yyrhs[] = {    56,
    69,    76,    73,     8,     0,     6,     0,    33,    57,     8,
     0,    58,     0,    45,    43,    55,    44,     8,     0,    46,
    69,    55,     8,     0,    69,     0,     0,    34,    21,     0,
     0,    20,    48,    50,     0,    48,    49,    50,     0,    42,
     0,    47,    42,     0,     3,     0,     3,     0,    35,    51,
    36,     0,    52,     0,    51,    52,     0,    53,    37,    54,
     0,     3,     0,     3,     0,    28,    35,    65,    36,     0,
     7,    47,     8,     0,    58,     0,    57,    58,     0,    60,
    61,    59,    62,    63,    64,     0,     5,     0,     3,     0,
     9,     0,    11,     0,    12,     0,    13,     0,    14,     0,
    25,     0,    24,     0,    23,     0,    10,     0,    15,     0,
    16,     0,    17,     0,    18,     0,    19,     0,    35,    66,
    36,     0,    35,    65,    36,     0,    35,    65,    36,     0,
     0,    67,     0,    65,    67,     0,    68,     0,    66,    68,
     0,     3,     0,    30,     0,    38,     3,     0,    39,     3,
     0,    40,     3,     0,    26,    35,    70,    36,     0,    71,
     0,    70,    71,     0,     3,    72,     0,     5,     0,    27,
    74,     0,    75,     0,    74,    75,     0,    35,    77,    36,
     0,    28,    35,    77,    36,     0,    78,     0,    77,    78,
     0,    22,     0,     3,     0,     4,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    89,    90,    95,    98,    99,   120,   131,   132,   135,   139,
   145,   172,   199,   200,   203,   210,   217,   222,   223,   226,
   231,   236,   241,   247,   256,   257,   260,   266,   273,   279,
   283,   287,   291,   295,   299,   303,   307,   311,   315,   319,
   323,   327,   331,   338,   344,   350,   354,   360,   361,   364,
   365,   368,   373,   377,   381,   385,   390,   395,   396,   399,
   405,   422,   428,   429,   432,   439,   446,   447,   448,   451,
   455
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","ID","ID1",
"INTEGER","UNKNOWN","NETWORK","END","AND","FF","OR","C_ELEMENT","NOT","BUFFER",
"AOI12","AOI22","OAI12","OAI22","TABLE1","SPEC","HIDDEN","ANY","NAND","NOR",
"ME","INIT","EXTERNALS","BDD_ORDER","SUBCIRCUITS","NONE","FANOUTS","GATE_LIST",
"ENTITY","INTERNAL_STATUS","'<'","'>'","':'","'+'","'#'","'-'","input","Module",
"Initialize_SPEC","Mode","Specification_Module","Nested_Module","Module_List",
"Mod_Name","Mod_Type","Formal_Actual_List","Formal_Actual","FA_Item","Formal_Name",
"Actual_Name","BDD_Order","Network","Gate_List","Gate","In_Num","Gate_Name",
"Gate_Type","Input_List","Output_List","Internal_List","Signal_List","Input_Signal_List",
"Signal","Input_Signal","Initialize","Initial_State","Signal_State","BIT","External_Signals",
"List_of_Signal_List","List_of_Signal","BDD_Ordering","Item_List","Item", NULL
};
#endif

static const short yyr1[] = {     0,
    41,    41,    41,    42,    42,    42,    43,    43,    44,    44,
    45,    46,    47,    47,    48,    49,    50,    51,    51,    52,
    53,    54,    55,    56,    57,    57,    58,    59,    60,    61,
    61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
    61,    61,    61,    62,    63,    64,    64,    65,    65,    66,
    66,    67,    67,    68,    68,    68,    69,    70,    70,    71,
    72,    73,    74,    74,    75,    76,    77,    77,    77,    78,
    78
};

static const short yyr2[] = {     0,
     5,     1,     3,     1,     5,     4,     1,     0,     2,     0,
     3,     3,     1,     2,     1,     1,     3,     1,     2,     3,
     1,     1,     4,     3,     1,     2,     6,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     3,     3,     3,     0,     1,     2,     1,
     2,     1,     1,     2,     2,     2,     4,     1,     2,     2,
     1,     2,     1,     2,     3,     4,     1,     2,     1,     1,
     1
};

static const short yydefact[] = {     0,
     2,     0,     0,     0,    29,     0,    13,     8,     0,     0,
     0,     4,     0,    29,     0,    25,     0,     0,    15,     0,
     0,     7,     0,    24,    14,    16,     0,    30,    38,    31,
    32,    33,    34,    39,    40,    41,    42,    43,    37,    36,
    35,     0,     3,    26,     0,     0,     0,     0,    11,     0,
    10,     0,    12,    28,     0,     0,     0,    58,     0,     0,
     0,    21,     0,    18,     0,     0,     0,     0,     6,     0,
     0,    61,    60,    57,    59,    70,    71,    69,     0,    67,
     0,    62,    63,     1,    17,    19,     0,    52,    53,     0,
    48,     9,     5,     0,     0,     0,     0,    50,     0,    47,
    66,    68,     0,    64,    22,    20,    23,    49,    54,    55,
    56,    44,    51,     0,     0,    27,    65,    45,     0,    46,
     0,     0,     0
};

static const short yydefgoto[] = {   121,
     7,    21,    68,     8,     9,    10,    11,    27,    49,    63,
    64,    65,   106,    51,     4,    15,    12,    55,    13,    42,
    71,   100,   116,    90,    97,    91,    98,    18,    57,    58,
    73,    61,    82,    83,    47,    79,    80
};

static const short yypact[] = {    10,
-32768,    27,    12,    20,    22,    63,-32768,    20,    20,    11,
    67,-32768,    39,-32768,    18,-32768,    36,    44,-32768,    38,
    46,-32768,    46,-32768,-32768,-32768,    38,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    70,-32768,-32768,    73,    42,    51,    76,-32768,    45,
    47,    74,-32768,-32768,    48,    79,     3,-32768,     6,    50,
    78,-32768,     5,-32768,    52,    15,    66,    80,-32768,    21,
    55,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    -3,-32768,
     6,    50,-32768,-32768,-32768,-32768,    88,-32768,-32768,    -1,
-32768,-32768,-32768,    89,    90,    91,    29,-32768,    15,    60,
-32768,-32768,     0,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,     2,    15,-32768,-32768,-32768,     4,-32768,
    96,    97,-32768
};

static const short yypgoto[] = {-32768,
    92,-32768,-32768,-32768,-32768,-32768,    93,-32768,    71,-32768,
    37,-32768,-32768,    81,-32768,-32768,     9,-32768,-32768,-32768,
-32768,-32768,-32768,   -88,-32768,   -77,     8,    14,-32768,    49,
-32768,-32768,-32768,    19,-32768,    26,   -59
};


#define	YYLAST		107


static const short yytable[] = {    76,
    77,    88,    76,    77,    88,    56,    88,    62,    76,    77,
   114,    16,   108,     5,    14,     1,     2,    88,    24,   102,
    14,    22,    23,    44,   -15,    43,   119,    78,    89,     5,
     6,    89,   101,    89,   107,   117,   108,   118,    74,   120,
    85,   108,     3,   102,    89,    17,     6,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    94,    95,
    96,    39,    40,    41,   112,    19,    94,    95,    96,    26,
    45,    46,    48,    50,    54,    56,    59,    60,    62,    66,
    67,    69,    70,    72,    81,    84,    92,    93,    87,    99,
   105,   109,   110,   111,   115,   122,   123,    53,    20,    86,
   104,    25,     0,    52,   113,    75,   103
};

static const short yycheck[] = {     3,
     4,     3,     3,     4,     3,     3,     3,     3,     3,     4,
    99,     3,    90,     3,     3,     6,     7,     3,     8,    79,
     3,     8,     9,    15,     3,     8,   115,    22,    30,     3,
    20,    30,    36,    30,    36,    36,   114,    36,    36,    36,
    36,   119,    33,   103,    30,    26,    20,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    38,    39,
    40,    23,    24,    25,    36,     3,    38,    39,    40,     3,
    35,    28,    35,    28,     5,     3,    35,    27,     3,    35,
    34,     8,    35,     5,    35,     8,    21,     8,    37,    35,
     3,     3,     3,     3,    35,     0,     0,    27,     6,    63,
    82,    10,    -1,    23,    97,    57,    81
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/usc/gnu/bison/1.25/share/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/usr/usc/gnu/bison/1.25/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 2:
#line 91 "parser.yac"
{ 
	            printf ("Parsing Error at line %d\n", line_num);
/*                  printf ("       %s\n", $1);*/
		  ;
    break;}
case 5:
#line 102 "parser.yac"
{ 
		    signal_ptr *spptr ;
		    
		    if (yyvsp[-2].sptr != NULL) 
		      spptr = Signal_Chain_to_ZArray (yyvsp[-2].sptr);
		    else 
		      spptr = (signal_ptr *) Array_to_ZArray (place_array); 
		    sprintf (Current_Module_Name, ""); /* void the prefix */
	 	    Enter_Module (yyvsp[-4].sval, spptr, yyvsp[-1].ival); 

		    // June 11 
		    array_free (input_array); // input_array
		    array_free (output_array); // output_array
//		    array_free (place_array); // June 14
		    place_array = array_alloc(signal_ptr, 1); 
		    input_array = array_alloc(signal_ptr, 1); 
		    output_array = array_alloc(signal_ptr, 1);       
		  ;
    break;}
case 6:
#line 123 "parser.yac"
{
		    signal_ptr *spptr;
		    
		    spptr = Signal_Chain_to_ZArray (yyvsp[-1].sptr);
		    sprintf (Current_Module_Name, ""); /* void the prefix */ 
		    Enter_Module (yyvsp[-3].sval, spptr, UNDEFINED);
		  ;
    break;}
case 9:
#line 136 "parser.yac"
{
		yyval.ival = INTERNAL;
		;
    break;}
case 10:
#line 140 "parser.yac"
{
		yyval.ival = EXTERNAL;
		;
    break;}
case 11:
#line 146 "parser.yac"
{
		    extern FILE *yyin;
		    extern int line_num;
		    FILE *temp_file;
		    int line_no;
		    
//		    printf("Trying to open file %s\n", $2);
//		    temp_file = yyin;
//		    line_no = line_num;
		    vidaline_num = 1;
		    vidain = fopen (yyvsp[-1].sval, "r");
    		    if ( vidain == NULL ) {
      			printf ("Cannot open file %s\n", yyvsp[-1].sval);
      			exit (1);
    		    } 
    		    if (vidaparse () != 0) {
		      printf("Parsing error\n");
		      exit(-1);
		    }
		    fclose (vidain);
//		    yyin = temp_file;
//		    line_num = line_no;
		    New_Specification (yyvsp[-1].sval);
		    yyval.sval = yyvsp[-1].sval;    		    		    
		  ;
    break;}
case 12:
#line 173 "parser.yac"
{
		    extern FILE *yyin;
		    extern int line_num;
		    FILE *temp_file;
		    int line_no;
		    
//		    printf("Trying to open file %s\n", $2);
		    temp_file = yyin;
		    line_no = line_num;
		    line_num = 1;
		    yyin = fopen (yyvsp[-1].sval, "r");
    		    if ( yyin == NULL ) {
      			printf ("Cannot open file %s\n", yyvsp[-1].sval);
      			exit (1);
    		    } 
    		    if (yyparse () != 0) {
		      printf("Parsing error\n");
		      exit(-1);
		    }
		    fclose (yyin);
		    yyin = temp_file;
		    line_num = line_no;
		    yyval.sval = yyvsp[-2].sval;    		    		    
		  ;
    break;}
case 15:
#line 204 "parser.yac"
{ 
		    sprintf (Current_Module_Name, "%s", yyvsp[0].sval);
		    yyval.sval = yyvsp[0].sval;
		  ;
    break;}
case 16:
#line 211 "parser.yac"
{
		    sprintf (Current_Module_Type, "%s", yyvsp[0].sval);
		    yyval.sval = yyvsp[0].sval;
		  ;
    break;}
case 17:
#line 218 "parser.yac"
{
		    Enter_Signal_Chain ();
		  ;
    break;}
case 20:
#line 227 "parser.yac"
{
		    Associate_Signal (yyvsp[-2].sval, yyvsp[0].sval);
		  ;
    break;}
case 21:
#line 232 "parser.yac"
{ 
		    yyval.sval = yyvsp[0].sval;
		  ;
    break;}
case 22:
#line 237 "parser.yac"
{ 
		    yyval.sval = yyvsp[0].sval;
		  ;
    break;}
case 23:
#line 242 "parser.yac"
{
		    yyval.sptr = Enter_Signal_Chain ();
		  ;
    break;}
case 24:
#line 248 "parser.yac"
{
		    list_of_gates = Gate_Chain_to_ZArray (gate_list, Gate_Num);
		    list_of_specs = Gate_Chain_to_ZArray (spec_list, Spec_Num);
		    End_Gate_Enter ();
		    Set_Fanouts ();
		  ;
    break;}
case 27:
#line 261 "parser.yac"
{
		  	Enter_Gate (yyvsp[-5].nptr, yyvsp[-4].cval, yyvsp[-2].inptr, yyvsp[-1].sptr, yyvsp[0].sptr, 1);
		  ;
    break;}
case 28:
#line 267 "parser.yac"
{
		  	input_no = yyvsp[0].ival;		
		        input_cur = 0;
		  	temp_input = Create_Input_ZArray_N (yyvsp[0].ival);
		  ;
    break;}
case 29:
#line 274 "parser.yac"
{
		  	yyval.nptr = Name_Enter (yyvsp[0].sval, GATE);
		  ;
    break;}
case 30:
#line 280 "parser.yac"
{
		  	yyval.cval = _AND;
		  ;
    break;}
case 31:
#line 284 "parser.yac"
{
		  	yyval.cval = _OR;
		  ;
    break;}
case 32:
#line 288 "parser.yac"
{
		  	yyval.cval = _C_ELEMENT;
		  ;
    break;}
case 33:
#line 292 "parser.yac"
{
		  	yyval.cval = _NOT;
		  ;
    break;}
case 34:
#line 296 "parser.yac"
{
		  	yyval.cval = _BUFFER;
		  ;
    break;}
case 35:
#line 300 "parser.yac"
{
		  	yyval.cval = _ME;
		  ;
    break;}
case 36:
#line 304 "parser.yac"
{
		  	yyval.cval = _NOR;
		  ;
    break;}
case 37:
#line 308 "parser.yac"
{
		  	yyval.cval = _NAND;
		  ;
    break;}
case 38:
#line 312 "parser.yac"
{
		  	yyval.cval = _FF;
		  ;
    break;}
case 39:
#line 316 "parser.yac"
{
		  	yyval.cval = _AOI12;
		  ;
    break;}
case 40:
#line 320 "parser.yac"
{
		  	yyval.cval = _AOI22;
		  ;
    break;}
case 41:
#line 324 "parser.yac"
{
		  	yyval.cval = _OAI12;
		  ;
    break;}
case 42:
#line 328 "parser.yac"
{
		  	yyval.cval = _OAI22;
		  ;
    break;}
case 43:
#line 332 "parser.yac"
{
		  	yyval.cval = _TABLE1;
		  ;
    break;}
case 44:
#line 339 "parser.yac"
{
		  	yyval.inptr = Enter_Input_Signal_Chain ();
		  ;
    break;}
case 45:
#line 345 "parser.yac"
{
		  	yyval.sptr = Enter_Signal_Chain ();
		  ;
    break;}
case 46:
#line 351 "parser.yac"
{
		  	yyval.sptr = Enter_Signal_Chain ();
		  ;
    break;}
case 47:
#line 355 "parser.yac"
{
		   	yyval.sptr = Enter_Signal_Chain ();
		  ;
    break;}
case 52:
#line 369 "parser.yac"
{
		  // Since my code does not need io status, I pass NONE!
		  	Enter_Signal (yyvsp[0].sval, UNDEFINED); 
		  ;
    break;}
case 53:
#line 374 "parser.yac"
{
		  ;
    break;}
case 54:
#line 378 "parser.yac"
{
		  	Enter_Input_Signal ('+', yyvsp[0].sval);
		  ;
    break;}
case 55:
#line 382 "parser.yac"
{
		  	Enter_Input_Signal ('-', yyvsp[0].sval);
		  ;
    break;}
case 56:
#line 386 "parser.yac"
{
		  	Enter_Input_Signal ('-', yyvsp[0].sval);
		  ;
    break;}
case 57:
#line 391 "parser.yac"
{
			Enter_Signal_Chain ();
		  ;
    break;}
case 60:
#line 400 "parser.yac"
{
		  	Enter_Initial_State (yyvsp[-1].sval, yyvsp[0].ival);
		  ;
    break;}
case 61:
#line 406 "parser.yac"
{
		  	yyval.ival = yyvsp[0].ival;
		  	if (yyval.ival != 0 && yyval.ival != 1) {
		  	  printf ("Bad Initial Value %d\n", yyval.ival);
		  	  exit (-1);
		  	}  
		  ;
    break;}
case 62:
#line 423 "parser.yac"
{		  	
		  ;
    break;}
case 65:
#line 433 "parser.yac"
{
		  	Enter_Level_Externals (Enter_Signal_Chain ());		
		;
    break;}
case 66:
#line 440 "parser.yac"
{
		  	yyval.sptr = Enter_Signal_Chain ();
		  	BDD_Order (yyval.sptr);
		  ;
    break;}
case 70:
#line 452 "parser.yac"
{
		    Enter_Item (yyvsp[0].sval);
		  ;
    break;}
case 71:
#line 456 "parser.yac"
{
		    Enter_Item (yyvsp[0].sval);
		  ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/usc/gnu/bison/1.25/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 461 "parser.yac"


yyerror (char *s)
{
    printf ("%s at line number %d reading %s\n",s,line_num,yytext);
}


