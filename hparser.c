
/*  A Bison parser, made from hparser.yacc
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	ID	258
#define	INTEGER	259
#define	INPUTS	260
#define	OUTPUTS	261
#define	INTERNALS	262
#define	GRAPH	263
#define	END	264
#define	MARKING	265
#define	NAME	266
#define	INIT_STATE	267
#define	DUMMYS	268

#line 3 "hparser.yacc"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "basic.h"
#include "type.h"
#include "names.h"


int vidaerror (char*);
int vidalex ();

int vidaline_num = 1;
int i = 0;

extern array_t *trans_array;
extern array_t *st_var_array;

extern char vidatext[];
extern void Init_Signal(signal_ptr sptr, int init);
extern signal_ptr Enter_Signal_Chain (void);
extern void Enter_Trans(signal_ptr p1, transition_ptr t, signal_ptr p2);
extern signal_ptr Find_Signal (char *name, io_type io);
extern transition_ptr Find_Trans(char *name, char *idx, char polar);
extern signal_ptr get_implicit_place(transition_ptr t1, transition_ptr t2);
 

#line 31 "hparser.yacc"
typedef union {
	char *sval;
	//int ival;
	//gate_ptr gptr;
	signal_ptr sptr;
	transition_ptr tptr;
	name_ptr   nptr;
	//input_ptr  inptr;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		60
#define	YYFLAG		-32768
#define	YYNTBASE	23

#define YYTRANSLATE(x) ((unsigned)(x) <= 268 ? vidatranslate[x] : 37)

static const char vidatranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    14,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    15,    21,    17,     2,    16,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    20,
     2,    22,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    18,     2,    19,     2,     2,     2,     2,     2,
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
     6,     7,     8,     9,    10,    11,    12,    13
};

#if YYDEBUG != 0
static const short vidaprhs[] = {     0,
     0,     1,     4,     7,     9,    13,    16,    19,    22,    25,
    28,    31,    35,    37,    39,    42,    44,    47,    49,    52,
    54,    57,    59,    61,    63,    66,    68,    71,    74,    77,
    82,    85,    90,    92,    96,    99,   101,   107
};

static const short vidarhs[] = {    -1,
    23,    24,     0,    23,     9,     0,    14,     0,    11,     3,
    14,     0,    25,    14,     0,    26,    14,     0,    27,    14,
     0,    28,    14,     0,     8,    14,     0,    29,    14,     0,
    10,    34,    14,     0,     1,     0,     5,     0,    25,     3,
     0,     6,     0,    26,     3,     0,     7,     0,    27,     3,
     0,    13,     0,    28,     3,     0,    30,     0,    31,     0,
    33,     0,    30,    32,     0,    32,     0,    31,    32,     0,
    31,    33,     0,     3,    15,     0,     3,    15,    16,     4,
     0,     3,    17,     0,     3,    17,    16,     4,     0,     3,
     0,    18,    35,    19,     0,    35,    36,     0,    36,     0,
    20,    32,    21,    32,    22,     0,    33,     0
};

#endif

#if YYDEBUG != 0
static const short vidarline[] = { 0,
    54,    55,    56,    63,    64,    65,    66,    67,    68,    69,
    70,    72,    73,    80,    81,    89,    90,    98,    99,   105,
   106,   114,   117,   122,   127,   133,   138,   144,   150,   154,
   158,   162,   168,   186,   189,   190,   193,   197
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const vidatname[] = {   "$","error","$undefined.","ID","INTEGER",
"INPUTS","OUTPUTS","INTERNALS","GRAPH","END","MARKING","NAME","INIT_STATE","DUMMYS",
"'\\n'","'+'","'/'","'-'","'{'","'}'","'<'","','","'>'","input","line","inputs",
"outputs","internals","dummys","edges","edge_place","edge_trans","trans","place",
"marking","marking_set","mark", NULL
};
#endif

static const short vidar1[] = {     0,
    23,    23,    23,    24,    24,    24,    24,    24,    24,    24,
    24,    24,    24,    25,    25,    26,    26,    27,    27,    28,
    28,    29,    29,    30,    30,    31,    31,    31,    32,    32,
    32,    32,    33,    34,    35,    35,    36,    36
};

static const short vidar2[] = {     0,
     0,     2,     2,     1,     3,     2,     2,     2,     2,     2,
     2,     3,     1,     1,     2,     1,     2,     1,     2,     1,
     2,     1,     1,     1,     2,     1,     2,     2,     2,     4,
     2,     4,     1,     3,     2,     1,     5,     1
};

static const short vidadefact[] = {     1,
     0,    13,    33,    14,    16,    18,     0,     3,     0,     0,
    20,     4,     2,     0,     0,     0,     0,     0,    22,    23,
    26,    24,    29,    31,    10,     0,     0,     0,    15,     6,
    17,     7,    19,     8,    21,     9,    11,     0,    25,    27,
    28,     0,     0,    33,     0,    38,     0,    36,    12,     5,
    30,    32,     0,    34,    35,     0,     0,    37,     0,     0
};

static const short vidadefgoto[] = {     1,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    46,
    27,    47,    48
};

static const short vidapact[] = {-32768,
    33,-32768,    -7,-32768,-32768,-32768,   -10,-32768,    -9,     8,
-32768,-32768,-32768,     3,     4,     9,    10,    11,    12,    13,
-32768,-32768,    -2,    14,-32768,     2,    15,    17,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    -7,-32768,-32768,
-32768,    23,    24,-32768,    12,-32768,     0,-32768,-32768,-32768,
-32768,-32768,    27,-32768,-32768,    12,    28,-32768,    32,-32768
};

static const short vidapgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -19,     1,
-32768,-32768,   -12
};


#define	YYLAST		50


static const short vidatable[] = {    39,
    40,    22,    44,    25,    44,    29,    31,    23,    26,    24,
    28,    33,    35,    42,    38,     3,    30,    32,    54,    45,
    41,    45,    34,    36,    37,    53,    51,    52,    49,    43,
    50,    60,    59,     2,    55,     3,    57,     4,     5,     6,
     7,     8,     9,    10,     0,    11,    12,    56,     0,    58
};

static const short vidacheck[] = {    19,
    20,     1,     3,    14,     3,     3,     3,    15,    18,    17,
     3,     3,     3,    16,     3,     3,    14,    14,    19,    20,
    20,    20,    14,    14,    14,    45,     4,     4,    14,    16,
    14,     0,     0,     1,    47,     3,    56,     5,     6,     7,
     8,     9,    10,    11,    -1,    13,    14,    21,    -1,    22
};
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

#define vidaerrok		(vidaerrstatus = 0)
#define vidaclearin	(vidachar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto vidaerrlab1
/* Like YYERROR except do call vidaerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto vidaerrlab
#define YYRECOVERING()  (!!vidaerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (vidachar == YYEMPTY && vidalen == 1)				\
    { vidachar = (token), vidalval = (value);			\
      vidachar1 = YYTRANSLATE (vidachar);				\
      YYPOPSTACK;						\
      goto vidabackup;						\
    }								\
  else								\
    { vidaerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		vidalex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		vidalex(&vidalval, &vidalloc, YYLEX_PARAM)
#else
#define YYLEX		vidalex(&vidalval, &vidalloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		vidalex(&vidalval, YYLEX_PARAM)
#else
#define YYLEX		vidalex(&vidalval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	vidachar;			/*  the lookahead symbol		*/
YYSTYPE	vidalval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE vidalloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int vidanerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int vidadebug;			/*  nonzero means print parse trace	*/
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
int vidaparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __vida_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__vida_memcpy (to, from, count)
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
__vida_memcpy (char *to, char *from, int count)
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
   into vidaparse.  The argument should have type void *.
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
vidaparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int vidastate;
  register int vidan;
  register short *vidassp;
  register YYSTYPE *vidavsp;
  int vidaerrstatus;	/*  number of tokens to shift before error messages enabled */
  int vidachar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	vidassa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE vidavsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *vidass = vidassa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *vidavs = vidavsa;	/*  to allow vidaoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE vidalsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *vidals = vidalsa;
  YYLTYPE *vidalsp;

#define YYPOPSTACK   (vidavsp--, vidassp--, vidalsp--)
#else
#define YYPOPSTACK   (vidavsp--, vidassp--)
#endif

  int vidastacksize = YYINITDEPTH;

#ifdef YYPURE
  int vidachar;
  YYSTYPE vidalval;
  int vidanerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE vidalloc;
#endif
#endif

  YYSTYPE vidaval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int vidalen;

#if YYDEBUG != 0
  if (vidadebug)
    fprintf(stderr, "Starting parse\n");
#endif

  vidastate = 0;
  vidaerrstatus = 0;
  vidanerrs = 0;
  vidachar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  vidassp = vidass - 1;
  vidavsp = vidavs;
#ifdef YYLSP_NEEDED
  vidalsp = vidals;
#endif

/* Push a new state, which is found in  vidastate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
vidanewstate:

  *++vidassp = vidastate;

  if (vidassp >= vidass + vidastacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *vidavs1 = vidavs;
      short *vidass1 = vidass;
#ifdef YYLSP_NEEDED
      YYLTYPE *vidals1 = vidals;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = vidassp - vidass + 1;

#ifdef vidaoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if vidaoverflow is a macro.  */
      vidaoverflow("parser stack overflow",
		 &vidass1, size * sizeof (*vidassp),
		 &vidavs1, size * sizeof (*vidavsp),
		 &vidals1, size * sizeof (*vidalsp),
		 &vidastacksize);
#else
      vidaoverflow("parser stack overflow",
		 &vidass1, size * sizeof (*vidassp),
		 &vidavs1, size * sizeof (*vidavsp),
		 &vidastacksize);
#endif

      vidass = vidass1; vidavs = vidavs1;
#ifdef YYLSP_NEEDED
      vidals = vidals1;
#endif
#else /* no vidaoverflow */
      /* Extend the stack our own way.  */
      if (vidastacksize >= YYMAXDEPTH)
	{
	  vidaerror("parser stack overflow");
	  return 2;
	}
      vidastacksize *= 2;
      if (vidastacksize > YYMAXDEPTH)
	vidastacksize = YYMAXDEPTH;
      vidass = (short *) alloca (vidastacksize * sizeof (*vidassp));
      __vida_memcpy ((char *)vidass, (char *)vidass1, size * sizeof (*vidassp));
      vidavs = (YYSTYPE *) alloca (vidastacksize * sizeof (*vidavsp));
      __vida_memcpy ((char *)vidavs, (char *)vidavs1, size * sizeof (*vidavsp));
#ifdef YYLSP_NEEDED
      vidals = (YYLTYPE *) alloca (vidastacksize * sizeof (*vidalsp));
      __vida_memcpy ((char *)vidals, (char *)vidals1, size * sizeof (*vidalsp));
#endif
#endif /* no vidaoverflow */

      vidassp = vidass + size - 1;
      vidavsp = vidavs + size - 1;
#ifdef YYLSP_NEEDED
      vidalsp = vidals + size - 1;
#endif

#if YYDEBUG != 0
      if (vidadebug)
	fprintf(stderr, "Stack size increased to %d\n", vidastacksize);
#endif

      if (vidassp >= vidass + vidastacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (vidadebug)
    fprintf(stderr, "Entering state %d\n", vidastate);
#endif

  goto vidabackup;
 vidabackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* vidaresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  vidan = vidapact[vidastate];
  if (vidan == YYFLAG)
    goto vidadefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* vidachar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (vidachar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (vidadebug)
	fprintf(stderr, "Reading a token: ");
#endif
      vidachar = YYLEX;
    }

  /* Convert token to internal form (in vidachar1) for indexing tables with */

  if (vidachar <= 0)		/* This means end of input. */
    {
      vidachar1 = 0;
      vidachar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (vidadebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      vidachar1 = YYTRANSLATE(vidachar);

#if YYDEBUG != 0
      if (vidadebug)
	{
	  fprintf (stderr, "Next token is %d (%s", vidachar, vidatname[vidachar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, vidachar, vidalval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  vidan += vidachar1;
  if (vidan < 0 || vidan > YYLAST || vidacheck[vidan] != vidachar1)
    goto vidadefault;

  vidan = vidatable[vidan];

  /* vidan is what to do for this token type in this state.
     Negative => reduce, -vidan is rule number.
     Positive => shift, vidan is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (vidan < 0)
    {
      if (vidan == YYFLAG)
	goto vidaerrlab;
      vidan = -vidan;
      goto vidareduce;
    }
  else if (vidan == 0)
    goto vidaerrlab;

  if (vidan == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (vidadebug)
    fprintf(stderr, "Shifting token %d (%s), ", vidachar, vidatname[vidachar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (vidachar != YYEOF)
    vidachar = YYEMPTY;

  *++vidavsp = vidalval;
#ifdef YYLSP_NEEDED
  *++vidalsp = vidalloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (vidaerrstatus) vidaerrstatus--;

  vidastate = vidan;
  goto vidanewstate;

/* Do the default action for the current state.  */
vidadefault:

  vidan = vidadefact[vidastate];
  if (vidan == 0)
    goto vidaerrlab;

/* Do a reduction.  vidan is the number of a rule to reduce with.  */
vidareduce:
  vidalen = vidar2[vidan];
  if (vidalen > 0)
    vidaval = vidavsp[1-vidalen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (vidadebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       vidan, vidarline[vidan]);

      /* Print the symbols being reduced, and their result.  */
      for (i = vidaprhs[vidan]; vidarhs[i] > 0; i++)
	fprintf (stderr, "%s ", vidatname[vidarhs[i]]);
      fprintf (stderr, " -> %s\n", vidatname[vidar1[vidan]]);
    }
#endif


  switch (vidan) {

case 3:
#line 57 "hparser.yacc"
{
// Feb 10		Enter_Signal_Chain(); // new		
	;
    break;}
case 13:
#line 74 "hparser.yacc"
{ 
		printf ("Parsing Error at line %d\n", vidaline_num);
		printf ("       %s\n", vidatext);
	;
    break;}
case 15:
#line 82 "hparser.yacc"
{
		Find_Signal ((char*)vidavsp[0].sval, OUTPUT_NODE);
//		printf ("input %s\n", Name_String (Signal_Name (Find_Signal ((char*)$2, OUTPUT_NODE))));
		// May 26: was INPUT_NODE
	;
    break;}
case 17:
#line 91 "hparser.yacc"
{
		Find_Signal ((char*)vidavsp[0].sval, INPUT_NODE);
//		printf ("output %s\n", Name_String (Signal_Name (Find_Signal ((char*)$2, INPUT_NODE))));
		// May 26: was OUTPUT_NODE
	;
    break;}
case 19:
#line 100 "hparser.yacc"
{
	    	Find_Signal ((char*)vidavsp[0].sval, INTERNAL_NODE);
	;
    break;}
case 21:
#line 108 "hparser.yacc"
{
		printf("found dummy\n");
	    	Find_Signal ((char*)vidavsp[0].sval, DUMMY_NODE);
	;
    break;}
case 22:
#line 115 "hparser.yacc"
{
	;
    break;}
case 23:
#line 118 "hparser.yacc"
{
	;
    break;}
case 24:
#line 123 "hparser.yacc"
{
//		printf ("edge-place first\n");
		vidaval.sptr = vidavsp[0].sptr;
	;
    break;}
case 25:
#line 128 "hparser.yacc"
{
//		printf ("edge-place trans\n");
		Enter_Trans((signal_ptr)vidavsp[-1].sptr, (transition_ptr)vidavsp[0].tptr, SIG_NULL_PTR);
	;
    break;}
case 26:
#line 134 "hparser.yacc"
{
//		printf ("edge-trans first\n");
		vidaval.tptr = vidavsp[0].tptr;
	;
    break;}
case 27:
#line 139 "hparser.yacc"
{
//		printf ("edge-trans trans\n");
		Enter_Trans(SIG_NULL_PTR, (transition_ptr)vidavsp[-1].tptr, get_implicit_place((transition_ptr)vidavsp[-1].tptr, (transition_ptr)vidavsp[0].tptr));
		Enter_Trans(get_implicit_place((transition_ptr)vidavsp[-1].tptr, (transition_ptr)vidavsp[0].tptr), (transition_ptr)vidavsp[0].tptr, SIG_NULL_PTR); 
	;
    break;}
case 28:
#line 145 "hparser.yacc"
{
//		printf ("edge-trans place\n");
		Enter_Trans(SIG_NULL_PTR, (transition_ptr)vidavsp[-1].tptr, (signal_ptr)vidavsp[0].sptr);
	;
    break;}
case 29:
#line 151 "hparser.yacc"
{
		vidaval.tptr = Find_Trans ((char*)vidavsp[-1].sval, "", '+');
	;
    break;}
case 30:
#line 155 "hparser.yacc"
{
		vidaval.tptr = Find_Trans ((char*)vidavsp[-3].sval, (char*)vidavsp[0].sval, '+');
	;
    break;}
case 31:
#line 159 "hparser.yacc"
{
	    	vidaval.tptr = Find_Trans ((char*)vidavsp[-1].sval, "", '-'); 
	;
    break;}
case 32:
#line 163 "hparser.yacc"
{
		vidaval.tptr = Find_Trans ((char*)vidavsp[-3].sval, (char*)vidavsp[0].sval, '-');
	;
    break;}
case 33:
#line 169 "hparser.yacc"
{
		vidaval.sptr = Find_Signal ((char*)vidavsp[0].sval, PLACE_NODE);
//		printf ("place %s\n", Name_String (Signal_Name ($$)));
	;
    break;}
case 37:
#line 194 "hparser.yacc"
{
	        Enter_Mark (get_implicit_place((transition_ptr)vidavsp[-3].tptr, (transition_ptr)vidavsp[-1].tptr));
	;
    break;}
case 38:
#line 198 "hparser.yacc"
{
	    	Enter_Mark (vidavsp[0].sptr);
	;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/usc/gnu/bison/1.25/share/bison.simple"

  vidavsp -= vidalen;
  vidassp -= vidalen;
#ifdef YYLSP_NEEDED
  vidalsp -= vidalen;
#endif

#if YYDEBUG != 0
  if (vidadebug)
    {
      short *ssp1 = vidass - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != vidassp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++vidavsp = vidaval;

#ifdef YYLSP_NEEDED
  vidalsp++;
  if (vidalen == 0)
    {
      vidalsp->first_line = vidalloc.first_line;
      vidalsp->first_column = vidalloc.first_column;
      vidalsp->last_line = (vidalsp-1)->last_line;
      vidalsp->last_column = (vidalsp-1)->last_column;
      vidalsp->text = 0;
    }
  else
    {
      vidalsp->last_line = (vidalsp+vidalen-1)->last_line;
      vidalsp->last_column = (vidalsp+vidalen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  vidan = vidar1[vidan];

  vidastate = vidapgoto[vidan - YYNTBASE] + *vidassp;
  if (vidastate >= 0 && vidastate <= YYLAST && vidacheck[vidastate] == *vidassp)
    vidastate = vidatable[vidastate];
  else
    vidastate = vidadefgoto[vidan - YYNTBASE];

  goto vidanewstate;

vidaerrlab:   /* here on detecting error */

  if (! vidaerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++vidanerrs;

#ifdef YYERROR_VERBOSE
      vidan = vidapact[vidastate];

      if (vidan > YYFLAG && vidan < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -vidan if nec to avoid negative indexes in vidacheck.  */
	  for (x = (vidan < 0 ? -vidan : 0);
	       x < (sizeof(vidatname) / sizeof(char *)); x++)
	    if (vidacheck[x + vidan] == x)
	      size += strlen(vidatname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (vidan < 0 ? -vidan : 0);
		       x < (sizeof(vidatname) / sizeof(char *)); x++)
		    if (vidacheck[x + vidan] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, vidatname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      vidaerror(msg);
	      free(msg);
	    }
	  else
	    vidaerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	vidaerror("parse error");
    }

  goto vidaerrlab1;
vidaerrlab1:   /* here on error raised explicitly by an action */

  if (vidaerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (vidachar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (vidadebug)
	fprintf(stderr, "Discarding token %d (%s).\n", vidachar, vidatname[vidachar1]);
#endif

      vidachar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  vidaerrstatus = 3;		/* Each real token shifted decrements this */

  goto vidaerrhandle;

vidaerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  vidan = vidadefact[vidastate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (vidan) goto vidadefault;
#endif

vidaerrpop:   /* pop the current state because it cannot handle the error token */

  if (vidassp == vidass) YYABORT;
  vidavsp--;
  vidastate = *--vidassp;
#ifdef YYLSP_NEEDED
  vidalsp--;
#endif

#if YYDEBUG != 0
  if (vidadebug)
    {
      short *ssp1 = vidass - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != vidassp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

vidaerrhandle:

  vidan = vidapact[vidastate];
  if (vidan == YYFLAG)
    goto vidaerrdefault;

  vidan += YYTERROR;
  if (vidan < 0 || vidan > YYLAST || vidacheck[vidan] != YYTERROR)
    goto vidaerrdefault;

  vidan = vidatable[vidan];
  if (vidan < 0)
    {
      if (vidan == YYFLAG)
	goto vidaerrpop;
      vidan = -vidan;
      goto vidareduce;
    }
  else if (vidan == 0)
    goto vidaerrpop;

  if (vidan == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (vidadebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++vidavsp = vidalval;
#ifdef YYLSP_NEEDED
  *++vidalsp = vidalloc;
#endif

  vidastate = vidan;
  goto vidanewstate;
}
#line 206 "hparser.yacc"


vidaerror (char *s)
{
    printf ("%s at line number %d reading %s\n", s, vidaline_num, vidatext);
}


