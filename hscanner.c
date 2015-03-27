#include <stdio.h>
#include <stdlib.h>
# define U(x) x
# define NLSTATE vidaprevious=YYNEWLINE
# define BEGIN vidabgin = vidasvec + 1 +
# define INITIAL 0
# define YYLERR vidasvec
# define YYSTATE (vidaestate-vidasvec-1)
# define YYOPTIM 1
# ifndef YYLMAX 
# define YYLMAX BUFSIZ
# endif 
#ifndef __cplusplus
# define output(c) (void)putc(c,vidaout)
#else
# define lex_output(c) (void)putc(c,vidaout)
#endif

#if defined(__cplusplus) || defined(__STDC__)

#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
	int vidaback(int *, int);
	int vidainput(void);
	int vidalook(void);
	void vidaoutput(int);
	int vidaracc(int);
	int vidareject(void);
	void vidaunput(int);
	int vidalex(void);
#ifdef YYLEX_E
	void vidawoutput(wchar_t);
	wchar_t vidawinput(void);
#endif
#ifndef vidaless
	int vidaless(int);
#endif
#ifndef vidawrap
	int vidawrap(void);
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
# define unput(c) {vidatchar= (c);if(vidatchar=='\n')vidalineno--;*vidasptr++=vidatchar;}
# define vidamore() (vidamorfg=1)
#ifndef __cplusplus
# define input() (((vidatchar=vidasptr>vidasbuf?U(*--vidasptr):getc(vidain))==10?(vidalineno++,vidatchar):vidatchar)==EOF?0:vidatchar)
#else
# define lex_input() (((vidatchar=vidasptr>vidasbuf?U(*--vidasptr):getc(vidain))==10?(vidalineno++,vidatchar):vidatchar)==EOF?0:vidatchar)
#endif
#define ECHO fprintf(vidaout, "%s",vidatext)
# define REJECT { nstr = vidareject(); goto vidafussy;}
int vidaleng;
#define YYISARRAY
char vidatext[YYLMAX];
int vidamorfg;
extern char *vidasptr, vidasbuf[];
int vidatchar;
FILE *vidain = {stdin}, *vidaout = {stdout};
extern int vidalineno;
struct vidasvf { 
	struct vidawork *vidastoff;
	struct vidasvf *vidaother;
	int *vidastops;};
struct vidasvf *vidaestate;
extern struct vidasvf vidasvec[], *vidabgin;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "hparser.h"

extern int vidaline_num;
YYSTYPE vidalval;
#undef vidawrap

# define YYNEWLINE 10
vidalex(){
int nstr; extern int vidaprevious;
#ifdef __cplusplus
/* to avoid CC and lint complaining vidafussy not being used ...*/
static int __lex_hack = 0;
if (__lex_hack) goto vidafussy;
#endif
while((nstr = vidalook()) >= 0)
vidafussy: switch(nstr){
case 0:
if(vidawrap()) return(0); break;
case 1:

# line 25 "hscanner.lex"
        {}
break;
case 2:

# line 26 "hscanner.lex"
     {}
break;
case 3:

# line 27 "hscanner.lex"
{ return NAME; }
break;
case 4:

# line 28 "hscanner.lex"
	{ return INPUTS; }
break;
case 5:

# line 29 "hscanner.lex"
{ return OUTPUTS; }
break;
case 6:

# line 30 "hscanner.lex"
	{ return INTERNALS; }
break;
case 7:

# line 31 "hscanner.lex"
       { return DUMMYS; }
break;
case 8:

# line 32 "hscanner.lex"
  	{ return GRAPH; }
break;
case 9:

# line 33 "hscanner.lex"
   { return INIT_STATE; }
break;
case 10:

# line 34 "hscanner.lex"
{ return MARKING; }
break;
case 11:

# line 35 "hscanner.lex"
	{ return END; }
break;
case 12:

# line 36 "hscanner.lex"
{ vidalval.sval = strdup(vidatext);
		  return INTEGER; }
break;
case 13:

# line 38 "hscanner.lex"
    	{ vidalval.sval = strdup(vidatext);
		  return ID; }
break;
case 14:

# line 41 "hscanner.lex"
	{ vidaline_num ++; return vidatext[0];}
break;
case 15:

# line 42 "hscanner.lex"
		{ return vidatext[0]; }
break;
case -1:
break;
default:
(void)fprintf(vidaout,"bad switch vidalook %d",nstr);
} return(0); }
/* end of vidalex */

# line 45 "hscanner.lex"

int vidawrap ()
{
    return 1;
}





int vidavstop[] = {
0,

15,
0, 

1,
15,
0, 

14,
0, 

2,
15,
0, 

15,
0, 

12,
15,
0, 

13,
15,
0, 

1,
0, 

2,
0, 

12,
0, 

13,
0, 

11,
0, 

3,
0, 

8,
0, 

7,
0, 

4,
0, 

10,
0, 

5,
0, 

6,
0, 

9,
0, 
0};
# define YYTYPE unsigned char
struct vidawork { YYTYPE verify, advance; } vidacrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	6,11,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
4,10,	6,11,	6,0,	11,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	4,10,	
1,6,	2,6,	4,11,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	1,7,	
2,7,	1,8,	0,0,	0,0,	
6,11,	8,19,	8,19,	8,19,	
8,19,	8,19,	8,19,	8,19,	
8,19,	8,19,	8,19,	0,0,	
0,0,	0,0,	1,9,	0,0,	
0,0,	6,11,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
41,50,	0,0,	17,27,	23,31,	
58,62,	7,12,	7,13,	22,30,	
7,14,	26,36,	7,15,	34,43,	
35,44,	36,45,	7,16,	7,17,	
7,18,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	12,21,	
13,22,	14,23,	15,24,	18,28,	
21,29,	25,35,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
27,37,	28,38,	29,39,	31,40,	
9,20,	32,41,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
9,20,	9,20,	9,20,	9,20,	
16,25,	24,32,	33,42,	37,46,	
38,47,	39,48,	40,49,	42,51,	
24,33,	43,52,	44,53,	45,46,	
24,34,	47,54,	16,26,	48,55,	
50,56,	51,57,	52,58,	53,59,	
54,60,	56,61,	59,63,	60,64,	
61,65,	62,66,	65,67,	67,68,	
0,0};
struct vidasvf vidasvec[] = {
0,	0,	0,
vidacrank+-1,	0,		0,	
vidacrank+-2,	vidasvec+1,	0,	
vidacrank+0,	0,		vidavstop+1,
vidacrank+3,	0,		vidavstop+3,
vidacrank+0,	0,		vidavstop+6,
vidacrank+-4,	0,		vidavstop+8,
vidacrank+1,	0,		vidavstop+11,
vidacrank+5,	0,		vidavstop+13,
vidacrank+65,	0,		vidavstop+16,
vidacrank+0,	vidasvec+4,	vidavstop+19,
vidacrank+-5,	vidasvec+6,	vidavstop+21,
vidacrank+6,	0,		0,	
vidacrank+14,	0,		0,	
vidacrank+11,	0,		0,	
vidacrank+16,	0,		0,	
vidacrank+91,	0,		0,	
vidacrank+1,	0,		0,	
vidacrank+10,	0,		0,	
vidacrank+0,	vidasvec+8,	vidavstop+23,
vidacrank+0,	vidasvec+9,	vidavstop+25,
vidacrank+19,	0,		0,	
vidacrank+3,	0,		0,	
vidacrank+2,	0,		0,	
vidacrank+84,	0,		0,	
vidacrank+15,	0,		0,	
vidacrank+5,	0,		0,	
vidacrank+47,	0,		0,	
vidacrank+41,	0,		0,	
vidacrank+49,	0,		0,	
vidacrank+0,	0,		vidavstop+27,
vidacrank+47,	0,		0,	
vidacrank+45,	0,		0,	
vidacrank+73,	0,		0,	
vidacrank+6,	0,		0,	
vidacrank+1,	0,		0,	
vidacrank+8,	0,		0,	
vidacrank+90,	0,		0,	
vidacrank+80,	0,		0,	
vidacrank+72,	0,		0,	
vidacrank+90,	0,		0,	
vidacrank+1,	0,		0,	
vidacrank+79,	0,		0,	
vidacrank+83,	0,		0,	
vidacrank+93,	0,		0,	
vidacrank+91,	0,		0,	
vidacrank+0,	0,		vidavstop+29,
vidacrank+84,	0,		0,	
vidacrank+88,	0,		0,	
vidacrank+0,	0,		vidavstop+31,
vidacrank+89,	0,		0,	
vidacrank+90,	0,		0,	
vidacrank+96,	0,		0,	
vidacrank+97,	0,		0,	
vidacrank+92,	0,		0,	
vidacrank+0,	0,		vidavstop+33,
vidacrank+93,	0,		0,	
vidacrank+0,	0,		vidavstop+35,
vidacrank+3,	0,		0,	
vidacrank+107,	0,		0,	
vidacrank+96,	0,		0,	
vidacrank+115,	0,		0,	
vidacrank+105,	0,		0,	
vidacrank+0,	0,		vidavstop+37,
vidacrank+0,	0,		vidavstop+39,
vidacrank+98,	0,		0,	
vidacrank+0,	0,		vidavstop+41,
vidacrank+114,	0,		0,	
vidacrank+0,	0,		vidavstop+43,
0,	0,	0};
struct vidawork *vidatop = vidacrank+215;
struct vidasvf *vidabgin = vidasvec+1;
char vidamatch[] = {
  0,   1,   1,   1,   1,   1,   1,   1, 
  1,   9,  10,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  9,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
 48,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,   1,   1,   1,   1,   1,   1, 
  1,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,   1,   1,   1,   1,  65, 
  1,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,  65,  65,  65,  65,  65, 
 65,  65,  65,   1,   1,   1,   1,   1, 
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
char vidaextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*	Copyright (c) 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#pragma ident	"@(#)ncform	6.11	97/01/06 SMI"

int vidalineno =1;
# define YYU(x) x
# define NLSTATE vidaprevious=YYNEWLINE
struct vidasvf *vidalstate [YYLMAX], **vidalsp, **vidaolsp;
char vidasbuf[YYLMAX];
char *vidasptr = vidasbuf;
int *vidafnd;
extern struct vidasvf *vidaestate;
int vidaprevious = YYNEWLINE;
#if defined(__cplusplus) || defined(__STDC__)
int vidalook(void)
#else
vidalook()
#endif
{
	register struct vidasvf *vidastate, **lsp;
	register struct vidawork *vidat;
	struct vidasvf *vidaz;
	int vidach, vidafirst;
	struct vidawork *vidar;
# ifdef LEXDEBUG
	int debug;
# endif
	char *vidalastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	vidafirst=1;
	if (!vidamorfg)
		vidalastch = vidatext;
	else {
		vidamorfg=0;
		vidalastch = vidatext+vidaleng;
		}
	for(;;){
		lsp = vidalstate;
		vidaestate = vidastate = vidabgin;
		if (vidaprevious==YYNEWLINE) vidastate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(vidaout,"state %d\n",vidastate-vidasvec-1);
# endif
			vidat = vidastate->vidastoff;
			if(vidat == vidacrank && !vidafirst){  /* may not be any transitions */
				vidaz = vidastate->vidaother;
				if(vidaz == 0)break;
				if(vidaz->vidastoff == vidacrank)break;
				}
#ifndef __cplusplus
			*vidalastch++ = vidach = input();
#else
			*vidalastch++ = vidach = lex_input();
#endif
#ifdef YYISARRAY
			if(vidalastch > &vidatext[YYLMAX]) {
				fprintf(vidaout,"Input string too long, limit %d\n",YYLMAX);
				exit(1);
			}
#else
			if (vidalastch >= &vidatext[ vidatextsz ]) {
				int	x = vidalastch - vidatext;

				vidatextsz += YYTEXTSZINC;
				if (vidatext == vida_tbuf) {
				    vidatext = (char *) malloc(vidatextsz);
				    memcpy(vidatext, vida_tbuf, sizeof (vida_tbuf));
				}
				else
				    vidatext = (char *) realloc(vidatext, vidatextsz);
				if (!vidatext) {
				    fprintf(vidaout,
					"Cannot realloc vidatext\n");
				    exit(1);
				}
				vidalastch = vidatext + x;
			}
#endif
			vidafirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(vidaout,"char ");
				allprint(vidach);
				putchar('\n');
				}
# endif
			vidar = vidat;
			if ( (int)vidat > (int)vidacrank){
				vidat = vidar + vidach;
				if (vidat <= vidatop && vidat->verify+vidasvec == vidastate){
					if(vidat->advance+vidasvec == YYLERR)	/* error transitions */
						{unput(*--vidalastch);break;}
					*lsp++ = vidastate = vidat->advance+vidasvec;
					if(lsp > &vidalstate[YYLMAX]) {
						fprintf(vidaout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)vidat < (int)vidacrank) {		/* r < vidacrank */
				vidat = vidar = vidacrank+(vidacrank-vidat);
# ifdef LEXDEBUG
				if(debug)fprintf(vidaout,"compressed state\n");
# endif
				vidat = vidat + vidach;
				if(vidat <= vidatop && vidat->verify+vidasvec == vidastate){
					if(vidat->advance+vidasvec == YYLERR)	/* error transitions */
						{unput(*--vidalastch);break;}
					*lsp++ = vidastate = vidat->advance+vidasvec;
					if(lsp > &vidalstate[YYLMAX]) {
						fprintf(vidaout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				vidat = vidar + YYU(vidamatch[vidach]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(vidaout,"try fall back character ");
					allprint(YYU(vidamatch[vidach]));
					putchar('\n');
					}
# endif
				if(vidat <= vidatop && vidat->verify+vidasvec == vidastate){
					if(vidat->advance+vidasvec == YYLERR)	/* error transition */
						{unput(*--vidalastch);break;}
					*lsp++ = vidastate = vidat->advance+vidasvec;
					if(lsp > &vidalstate[YYLMAX]) {
						fprintf(vidaout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
			if ((vidastate = vidastate->vidaother) && (vidat= vidastate->vidastoff) != vidacrank){
# ifdef LEXDEBUG
				if(debug)fprintf(vidaout,"fall back to state %d\n",vidastate-vidasvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--vidalastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(vidaout,"state %d char ",vidastate-vidasvec-1);
				allprint(vidach);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(vidaout,"stopped at %d with ",*(lsp-1)-vidasvec-1);
			allprint(vidach);
			putchar('\n');
			}
# endif
		while (lsp-- > vidalstate){
			*vidalastch-- = 0;
			if (*lsp != 0 && (vidafnd= (*lsp)->vidastops) && *vidafnd > 0){
				vidaolsp = lsp;
				if(vidaextra[*vidafnd]){		/* must backup */
					while(vidaback((*lsp)->vidastops,-*vidafnd) != 1 && lsp > vidalstate){
						lsp--;
						unput(*vidalastch--);
						}
					}
				vidaprevious = YYU(*vidalastch);
				vidalsp = lsp;
				vidaleng = vidalastch-vidatext+1;
				vidatext[vidaleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(vidaout,"\nmatch ");
					sprint(vidatext);
					fprintf(vidaout," action %d\n",*vidafnd);
					}
# endif
				return(*vidafnd++);
				}
			unput(*vidalastch);
			}
		if (vidatext[0] == 0  /* && feof(vidain) */)
			{
			vidasptr=vidasbuf;
			return(0);
			}
#ifndef __cplusplus
		vidaprevious = vidatext[0] = input();
		if (vidaprevious>0)
			output(vidaprevious);
#else
		vidaprevious = vidatext[0] = lex_input();
		if (vidaprevious>0)
			lex_output(vidaprevious);
#endif
		vidalastch=vidatext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
#if defined(__cplusplus) || defined(__STDC__)
int vidaback(int *p, int m)
#else
vidaback(p, m)
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
int vidainput(void)
#else
vidainput()
#endif
{
#ifndef __cplusplus
	return(input());
#else
	return(lex_input());
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void vidaoutput(int c)
#else
vidaoutput(c)
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
void vidaunput(int c)
#else
vidaunput(c)
   int c; 
#endif
{
	unput(c);
	}
