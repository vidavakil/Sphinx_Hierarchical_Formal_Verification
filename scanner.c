#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "parser.h"

# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# ifndef YYLMAX 
# define YYLMAX BUFSIZ
# endif 
#ifndef __cplusplus
# define output(c) (void)putc(c,yyout)
#else
# define lex_output(c) (void)putc(c,yyout)
#endif

#if defined(__cplusplus) || defined(__STDC__)

#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
	int yyback(int *, int);
	int yyinput(void);
	int yylook(void);
	void yyoutput(int);
	int yyracc(int);
	int yyreject(void);
	void yyunput(int);
	int yylex(YYSTYPE *);
#ifdef YYLEX_E
	void yywoutput(wchar_t);
	wchar_t yywinput(void);
#endif
#ifndef yyless
	int yyless(int);
#endif
#ifndef yywrap
	int yywrap(void);
#endif
#ifdef LEXDEBUG
	void allprint(char);
	void sprint(char *);
#endif
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
	void exit(int);
#ifdef __cplusplus
}
#endif

#endif
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
#ifndef __cplusplus
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#else
# define lex_input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#endif
#define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng;
#define YYISARRAY
char yytext[YYLMAX];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
     /* need this for the call to atof() below */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

# line 8 "scanner.lex"

extern int line_num;
YYSTYPE *yylval; /* Aug 23 2000*/

# line 19 "scanner.lex"
/*assign		:=
{assign}		{ return ASSIGN; }*/

#undef yywrap
int file_done = 0;
extern char *file2;

# line 29 "scanner.lex"
/*delim    	[ \t\n]+*/
# define YYNEWLINE 10
yylex(YYSTYPE *yylval){
int nstr; extern int yyprevious;
#ifdef __cplusplus
/* to avoid CC and lint complaining yyfussy not being used ...*/
static int __lex_hack = 0;
if (__lex_hack) goto yyfussy;
#endif
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:

# line 45 "scanner.lex"
			;
break;
case 2:

# line 46 "scanner.lex"
		;
break;
case 3:

# line 47 "scanner.lex"
		;
break;
case 4:

# line 48 "scanner.lex"
		{ /*printf ("NETWORK\n"); */return NETWORK; }
break;
case 5:

# line 49 "scanner.lex"
		{ return AND; }
break;
case 6:

# line 50 "scanner.lex"
		{ return OR; }
break;
case 7:

# line 51 "scanner.lex"
		{ /*printf ("NOT = %d\n", NOT); */return NOT; }
break;
case 8:

# line 52 "scanner.lex"
		{ return BUFFER; }
break;
case 9:

# line 53 "scanner.lex"
		{ return NAND; }
break;
case 10:

# line 54 "scanner.lex"
		{ return NOR; }
break;
case 11:

# line 55 "scanner.lex"
		{ return NOT; }
break;
case 12:

# line 56 "scanner.lex"
		{ return ME; }
break;
case 13:

# line 57 "scanner.lex"
		{ return FF; }
break;
case 14:

# line 58 "scanner.lex"
	{ return C_ELEMENT; }
break;
case 15:

# line 59 "scanner.lex"
		{ return AOI12; }
break;
case 16:

# line 60 "scanner.lex"
		{ return AOI22; }
break;
case 17:

# line 61 "scanner.lex"
		{ return OAI12; }
break;
case 18:

# line 62 "scanner.lex"
		{ return OAI22; }
break;
case 19:

# line 63 "scanner.lex"
		{ return TABLE1; }
break;
case 20:

# line 64 "scanner.lex"
		{ return INIT; }
break;
case 21:

# line 65 "scanner.lex"
		{ return HIDDEN; }
break;
case 22:

# line 66 "scanner.lex"
	{ /*printf ("EXTERNALS\n"); */return EXTERNALS; }
break;
case 23:

# line 67 "scanner.lex"
	{ /*printf ("SUBCIRCUITS\n"); */return SUBCIRCUITS; }
break;
case 24:

# line 68 "scanner.lex"
	{ /*printf ("BDD_ORDER\n"); */return BDD_ORDER; }
break;
case 25:

# line 69 "scanner.lex"
		{ return NONE; }
break;
case 26:

# line 70 "scanner.lex"
		{ return FANOUTS; }
break;
case 27:

# line 71 "scanner.lex"
	{ return GATE_LIST; }
break;
case 28:

# line 72 "scanner.lex"
		{ return END; }
break;
case 29:

# line 73 "scanner.lex"
		{ return ENTITY; }
break;
case 30:

# line 74 "scanner.lex"
	        { return SPEC; }
break;
case 31:

# line 75 "scanner.lex"
		{ return ANY; }
break;
case 32:

# line 76 "scanner.lex"
	{ return INTERNAL_STATUS; }
break;
case 33:

# line 77 "scanner.lex"
		;
break;
case 34:

# line 78 "scanner.lex"
		{ return yytext[0]; }
break;
case 35:

# line 79 "scanner.lex"
		{ return yytext[0]; }
break;
case 36:

# line 80 "scanner.lex"
		{ return yytext[0]; }
break;
case 37:

# line 81 "scanner.lex"
		{ yylval->ival = (int) atoi(yytext); 
			  		return INTEGER; }
break;
case 38:

# line 83 "scanner.lex"
    		{ /* printf("scanned id %s\n",yytext);*/
				 yylval->sval = strdup(yytext); 
		 	  		return ID; }
break;
case 39:

# line 86 "scanner.lex"
    		{ /* printf("scanned id %s\n",yytext);*/
				 yylval->sval = strdup(yytext); 
		 	  		return ID1; }
break;
case 40:

# line 89 "scanner.lex"
	   			{ line_num ++; /*return yytext[0];*/}
break;
case 41:

# line 90 "scanner.lex"
					{ yylval->sval = strdup(yytext); 
					return UNKNOWN; }
break;
case -1:
break;
default:
(void)fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

# line 93 "scanner.lex"

int yywrap ()
{
	if (file_done == 1)
	    return 1;
	else
	{
		file_done = 1;
		return 0;
	}
}

int yyvstop[] = {
0,

41,
0, 

1,
33,
41,
0, 

40,
0, 

34,
36,
38,
39,
41,
0, 

36,
41,
0, 

41,
0, 

34,
41,
0, 

38,
39,
41,
0, 

41,
0, 

37,
41,
0, 

35,
41,
0, 

3,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

38,
39,
41,
0, 

1,
33,
0, 

3,
0, 

38,
39,
0, 

39,
0, 

2,
0, 

37,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

13,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

12,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

6,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

5,
38,
39,
0, 

31,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

28,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

10,
38,
39,
0, 

7,
11,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

20,
38,
39,
0, 

38,
39,
0, 

9,
38,
39,
0, 

38,
39,
0, 

25,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

30,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

15,
38,
39,
0, 

16,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

17,
38,
39,
0, 

18,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

8,
38,
39,
0, 

38,
39,
0, 

29,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

21,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

19,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

26,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

4,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

24,
38,
39,
0, 

14,
38,
39,
0, 

22,
38,
39,
0, 

27,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

23,
38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

38,
39,
0, 

32,
38,
39,
0, 
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	0,0,	0,0,	4,28,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	1,6,	
1,7,	1,8,	4,28,	30,0,	
0,0,	1,9,	0,0,	0,0,	
1,7,	0,0,	0,0,	1,10,	
1,11,	1,12,	0,0,	0,0,	
0,0,	4,29,	0,0,	0,0,	
0,0,	0,0,	0,0,	1,13,	
1,14,	0,0,	0,0,	0,0,	
2,8,	4,30,	1,15,	1,16,	
1,17,	0,0,	1,18,	1,19,	
1,20,	1,21,	1,22,	2,11,	
11,33,	0,0,	1,23,	1,24,	
1,25,	58,79,	58,80,	0,0,	
1,26,	1,27,	2,13,	2,14,	
0,0,	0,0,	75,94,	75,95,	
0,0,	2,15,	2,16,	2,17,	
20,44,	2,18,	2,19,	2,20,	
2,21,	2,22,	21,45,	23,47,	
6,31,	2,23,	2,24,	2,25,	
15,35,	15,36,	22,46,	2,26,	
2,27,	6,32,	16,37,	6,32,	
6,31,	27,55,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
17,39,	19,42,	36,58,	16,38,	
37,59,	38,60,	19,43,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	26,53,	39,61,	41,64,	
42,65,	6,31,	26,54,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	6,31,	6,31,	6,31,	
6,31,	8,31,	12,34,	12,34,	
12,34,	12,34,	12,34,	12,34,	
12,34,	12,34,	12,34,	12,34,	
18,40,	8,31,	24,48,	40,62,	
44,66,	45,67,	24,49,	46,68,	
48,70,	49,71,	18,41,	50,72,	
51,75,	53,76,	54,77,	50,73,	
24,50,	50,74,	46,69,	40,63,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	55,78,	59,81,	
60,82,	61,83,	8,31,	63,84,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	8,31,	8,31,	
8,31,	8,31,	14,30,	25,51,	
35,56,	64,85,	65,86,	66,87,	
67,88,	68,89,	14,30,	14,0,	
69,90,	70,91,	71,92,	72,93,	
76,96,	77,97,	78,98,	79,99,	
25,52,	80,100,	81,101,	82,102,	
83,103,	35,57,	84,104,	85,105,	
86,106,	87,107,	88,108,	90,109,	
92,110,	94,111,	95,112,	14,30,	
14,30,	97,113,	32,32,	98,114,	
101,115,	14,30,	102,116,	103,117,	
14,30,	104,118,	105,119,	14,30,	
106,120,	14,30,	32,32,	107,121,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	108,122,	109,123,	
110,124,	113,125,	114,126,	115,127,	
117,128,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	119,129,	
120,130,	121,131,	123,132,	32,32,	
124,133,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	33,33,	
125,134,	127,135,	128,136,	129,137,	
131,138,	132,139,	134,140,	33,33,	
33,0,	135,141,	136,142,	137,143,	
138,144,	139,145,	140,146,	145,147,	
146,148,	147,149,	148,150,	149,151,	
151,152,	152,153,	153,154,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
33,33,	33,33,	0,0,	0,0,	
0,0,	0,0,	33,33,	0,0,	
0,0,	33,33,	0,0,	0,0,	
33,33,	0,0,	33,33,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-28,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+6,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+7,
yycrank+70,	0,		yyvstop+9,
yycrank+0,	0,		yyvstop+15,
yycrank+159,	0,		yyvstop+18,
yycrank+0,	0,		yyvstop+20,
yycrank+0,	yysvec+6,	yyvstop+23,
yycrank+29,	0,		yyvstop+27,
yycrank+146,	0,		yyvstop+29,
yycrank+0,	0,		yyvstop+32,
yycrank+-281,	0,		yyvstop+35,
yycrank+30,	yysvec+6,	yyvstop+38,
yycrank+46,	yysvec+6,	yyvstop+42,
yycrank+33,	yysvec+6,	yyvstop+46,
yycrank+126,	yysvec+6,	yyvstop+50,
yycrank+64,	yysvec+6,	yyvstop+54,
yycrank+31,	yysvec+6,	yyvstop+58,
yycrank+29,	yysvec+6,	yyvstop+62,
yycrank+32,	yysvec+6,	yyvstop+66,
yycrank+34,	yysvec+6,	yyvstop+70,
yycrank+141,	yysvec+6,	yyvstop+74,
yycrank+218,	yysvec+6,	yyvstop+78,
yycrank+81,	yysvec+6,	yyvstop+82,
yycrank+52,	yysvec+6,	yyvstop+86,
yycrank+0,	yysvec+4,	yyvstop+90,
yycrank+0,	yysvec+11,	0,	
yycrank+-29,	yysvec+14,	yyvstop+93,
yycrank+0,	yysvec+6,	yyvstop+95,
yycrank+284,	yysvec+6,	yyvstop+98,
yycrank+-406,	0,		yyvstop+100,
yycrank+0,	yysvec+12,	yyvstop+102,
yycrank+216,	yysvec+6,	yyvstop+104,
yycrank+57,	yysvec+6,	yyvstop+107,
yycrank+64,	yysvec+6,	yyvstop+110,
yycrank+63,	yysvec+6,	yyvstop+113,
yycrank+93,	yysvec+6,	yyvstop+116,
yycrank+139,	yysvec+6,	yyvstop+119,
yycrank+79,	yysvec+6,	yyvstop+122,
yycrank+86,	yysvec+6,	yyvstop+125,
yycrank+0,	yysvec+6,	yyvstop+128,
yycrank+124,	yysvec+6,	yyvstop+132,
yycrank+141,	yysvec+6,	yyvstop+135,
yycrank+138,	yysvec+6,	yyvstop+138,
yycrank+0,	yysvec+6,	yyvstop+141,
yycrank+134,	yysvec+6,	yyvstop+145,
yycrank+129,	yysvec+6,	yyvstop+148,
yycrank+137,	yysvec+6,	yyvstop+151,
yycrank+143,	yysvec+6,	yyvstop+154,
yycrank+0,	yysvec+6,	yyvstop+157,
yycrank+148,	yysvec+6,	yyvstop+161,
yycrank+152,	yysvec+6,	yyvstop+164,
yycrank+184,	yysvec+6,	yyvstop+167,
yycrank+0,	yysvec+6,	yyvstop+170,
yycrank+0,	yysvec+6,	yyvstop+174,
yycrank+32,	yysvec+6,	yyvstop+178,
yycrank+156,	yysvec+6,	yyvstop+181,
yycrank+182,	yysvec+6,	yyvstop+184,
yycrank+177,	yysvec+6,	yyvstop+187,
yycrank+0,	yysvec+6,	yyvstop+190,
yycrank+182,	yysvec+6,	yyvstop+194,
yycrank+216,	yysvec+6,	yyvstop+197,
yycrank+207,	yysvec+6,	yyvstop+200,
yycrank+218,	yysvec+6,	yyvstop+203,
yycrank+220,	yysvec+6,	yyvstop+206,
yycrank+205,	yysvec+6,	yyvstop+209,
yycrank+223,	yysvec+6,	yyvstop+212,
yycrank+225,	yysvec+6,	yyvstop+215,
yycrank+207,	yysvec+6,	yyvstop+218,
yycrank+226,	yysvec+6,	yyvstop+221,
yycrank+0,	yysvec+6,	yyvstop+224,
yycrank+0,	yysvec+6,	yyvstop+228,
yycrank+41,	yysvec+6,	yyvstop+233,
yycrank+229,	yysvec+6,	yyvstop+236,
yycrank+230,	yysvec+6,	yyvstop+239,
yycrank+222,	yysvec+6,	yyvstop+242,
yycrank+249,	yysvec+6,	yyvstop+245,
yycrank+251,	yysvec+6,	yyvstop+248,
yycrank+223,	yysvec+6,	yyvstop+251,
yycrank+234,	yysvec+6,	yyvstop+254,
yycrank+235,	yysvec+6,	yyvstop+257,
yycrank+222,	yysvec+6,	yyvstop+260,
yycrank+225,	yysvec+6,	yyvstop+263,
yycrank+223,	yysvec+6,	yyvstop+266,
yycrank+214,	yysvec+6,	yyvstop+269,
yycrank+241,	yysvec+6,	yyvstop+272,
yycrank+0,	yysvec+6,	yyvstop+275,
yycrank+229,	yysvec+6,	yyvstop+279,
yycrank+0,	yysvec+6,	yyvstop+282,
yycrank+233,	yysvec+6,	yyvstop+286,
yycrank+0,	yysvec+6,	yyvstop+289,
yycrank+263,	yysvec+6,	yyvstop+293,
yycrank+264,	yysvec+6,	yyvstop+296,
yycrank+0,	yysvec+6,	yyvstop+299,
yycrank+244,	yysvec+6,	yyvstop+303,
yycrank+250,	yysvec+6,	yyvstop+306,
yycrank+0,	yysvec+6,	yyvstop+309,
yycrank+0,	yysvec+6,	yyvstop+313,
yycrank+238,	yysvec+6,	yyvstop+317,
yycrank+240,	yysvec+6,	yyvstop+320,
yycrank+246,	yysvec+6,	yyvstop+323,
yycrank+236,	yysvec+6,	yyvstop+326,
yycrank+248,	yysvec+6,	yyvstop+329,
yycrank+244,	yysvec+6,	yyvstop+332,
yycrank+255,	yysvec+6,	yyvstop+335,
yycrank+264,	yysvec+6,	yyvstop+338,
yycrank+265,	yysvec+6,	yyvstop+341,
yycrank+262,	yysvec+6,	yyvstop+344,
yycrank+0,	yysvec+6,	yyvstop+347,
yycrank+0,	yysvec+6,	yyvstop+351,
yycrank+263,	yysvec+6,	yyvstop+355,
yycrank+297,	yysvec+6,	yyvstop+358,
yycrank+279,	yysvec+6,	yyvstop+361,
yycrank+0,	yysvec+6,	yyvstop+364,
yycrank+279,	yysvec+6,	yyvstop+368,
yycrank+0,	yysvec+6,	yyvstop+371,
yycrank+310,	yysvec+6,	yyvstop+375,
yycrank+293,	yysvec+6,	yyvstop+378,
yycrank+304,	yysvec+6,	yyvstop+381,
yycrank+0,	yysvec+6,	yyvstop+384,
yycrank+313,	yysvec+6,	yyvstop+388,
yycrank+305,	yysvec+6,	yyvstop+391,
yycrank+341,	yysvec+6,	yyvstop+394,
yycrank+0,	yysvec+6,	yyvstop+397,
yycrank+340,	yysvec+6,	yyvstop+401,
yycrank+332,	yysvec+6,	yyvstop+404,
yycrank+335,	yysvec+6,	yyvstop+407,
yycrank+0,	yysvec+6,	yyvstop+410,
yycrank+329,	yysvec+6,	yyvstop+414,
yycrank+337,	yysvec+6,	yyvstop+417,
yycrank+0,	yysvec+6,	yyvstop+420,
yycrank+329,	yysvec+6,	yyvstop+424,
yycrank+335,	yysvec+6,	yyvstop+427,
yycrank+334,	yysvec+6,	yyvstop+430,
yycrank+336,	yysvec+6,	yyvstop+433,
yycrank+336,	yysvec+6,	yyvstop+436,
yycrank+326,	yysvec+6,	yyvstop+439,
yycrank+349,	yysvec+6,	yyvstop+442,
yycrank+0,	yysvec+6,	yyvstop+445,
yycrank+0,	yysvec+6,	yyvstop+449,
yycrank+0,	yysvec+6,	yyvstop+453,
yycrank+0,	yysvec+6,	yyvstop+457,
yycrank+340,	yysvec+6,	yyvstop+461,
yycrank+340,	yysvec+6,	yyvstop+464,
yycrank+341,	yysvec+6,	yyvstop+467,
yycrank+343,	yysvec+6,	yyvstop+470,
yycrank+362,	yysvec+6,	yyvstop+473,
yycrank+0,	yysvec+6,	yyvstop+476,
yycrank+344,	yysvec+6,	yyvstop+480,
yycrank+344,	yysvec+6,	yyvstop+483,
yycrank+347,	yysvec+6,	yyvstop+486,
yycrank+0,	yysvec+6,	yyvstop+489,
0,	0,	0};
struct yywork *yytop = yycrank+454;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
  0,   1,   1,   1,   1,   1,   1,   1, 
  1,   9,  10,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  9,   1,  34,  35,   1,   1,   1,   1, 
 40,  40,   1,  43,   1,  43,  46,   1, 
 48,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,   1,   1,  40,   1,  40,   1, 
  1,  46,  46,  46,  46,  46,  46,  46, 
 46,  46,  46,  46,  46,  46,  46,  46, 
 46,  46,  46,  46,  46,  46,  46,  46, 
 46,  46,  46,   1,   1,   1,   1,  46, 
  1,  46,  46,  46,  46,  46,  46,  46, 
 46,  46,  46,  46,  46,  46,  46,  46, 
 46,  46,  46,  46,  46,  46,  46,  46, 
 46,  46,  46,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*	Copyright (c) 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#pragma ident	"@(#)ncform	6.11	97/01/06 SMI"

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
#if defined(__cplusplus) || defined(__STDC__)
int yylook(void)
#else
yylook()
#endif
{
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
#ifndef __cplusplus
			*yylastch++ = yych = input();
#else
			*yylastch++ = yych = lex_input();
#endif
#ifdef YYISARRAY
			if(yylastch > &yytext[YYLMAX]) {
				fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
				exit(1);
			}
#else
			if (yylastch >= &yytext[ yytextsz ]) {
				int	x = yylastch - yytext;

				yytextsz += YYTEXTSZINC;
				if (yytext == yy_tbuf) {
				    yytext = (char *) malloc(yytextsz);
				    memcpy(yytext, yy_tbuf, sizeof (yy_tbuf));
				}
				else
				    yytext = (char *) realloc(yytext, yytextsz);
				if (!yytext) {
				    fprintf(yyout,
					"Cannot realloc yytext\n");
				    exit(1);
				}
				yylastch = yytext + x;
			}
#endif
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
#ifndef __cplusplus
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
#else
		yyprevious = yytext[0] = lex_input();
		if (yyprevious>0)
			lex_output(yyprevious);
#endif
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
#if defined(__cplusplus) || defined(__STDC__)
int yyback(int *p, int m)
#else
yyback(p, m)
	int *p;
#endif
{
	if (p==0) return(0);
	while (*p) {
		if (*p++ == m)
			return(1);
	}
	return(0);
}
	/* the following are only used in the lex library */
#if defined(__cplusplus) || defined(__STDC__)
int yyinput(void)
#else
yyinput()
#endif
{
#ifndef __cplusplus
	return(input());
#else
	return(lex_input());
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyoutput(int c)
#else
yyoutput(c)
  int c; 
#endif
{
#ifndef __cplusplus
	output(c);
#else
	lex_output(c);
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyunput(int c)
#else
yyunput(c)
   int c; 
#endif
{
	unput(c);
	}
