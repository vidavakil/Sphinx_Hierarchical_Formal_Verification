%{
     /* need this for the call to atof() below */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*
#include <math.h>
#include "basic.h"
#include "common.h"
#include "tran.h"
*/
#include "type.h"
#include "parser.h"

extern int line_num;
YYSTYPE *yylval; /* Aug 23 2000*/
/*assign		:=
{assign}		{ return ASSIGN; }*/

#undef yywrap
int file_done = 0;
extern char *file2;
%}

digit    	[0-9]
integer   	{digit}+
/*delim    	[ \t\n]+*/
delim    	[ \t]+
semicolon	;
colon		:
camma		\,
assign	        :=
letter		[A-Za-z"_""."]
id	 	$?{letter}({letter}|{digit})*
letter1		[A-Za-z"_"".""+""\-"]
id1	 	$?{letter}({letter1}|{digit})*
paren		["("")""<"">"]
sign 		["+""#""\-"]
comments 	{delim}*"//".*
speccomments {delim}*";".*

%%

{delim}				;	/* ignore whitespace */
{comments}			;	/* ignore comments */ 
{speccomments}			;	/* ignore comments */ 
NETWORK			{ /*printf ("NETWORK\n"); */return NETWORK; }
AND			{ return AND; }
OR			{ return OR; }
NOT			{ /*printf ("NOT = %d\n", NOT); */return NOT; }
BUFFER			{ return BUFFER; }
NAND			{ return NAND; }
NOR			{ return NOR; }
NOT			{ return NOT; }
ME			{ return ME; }
FF			{ return FF; }
C_ELEMENT		{ return C_ELEMENT; }
AOI12			{ return AOI12; }
AOI22			{ return AOI22; }
OAI12			{ return OAI12; }
OAI22			{ return OAI22; }
TABLE1			{ return TABLE1; }
INIT			{ return INIT; }
HIDDEN			{ return HIDDEN; }
EXTERNALS		{ /*printf ("EXTERNALS\n"); */return EXTERNALS; }
SUBCIRCUITS		{ /*printf ("SUBCIRCUITS\n"); */return SUBCIRCUITS; }
BDD_ORDER		{ /*printf ("BDD_ORDER\n"); */return BDD_ORDER; }
NONE			{ return NONE; }
FANOUTS			{ return FANOUTS; }
GATE_LIST		{ return GATE_LIST; }
END			{ return END; }
ENTITY			{ return ENTITY; }
SPEC		        { return SPEC; }
ANY			{ return ANY; }
INTERNAL_STATUS		{ return INTERNAL_STATUS; }
{delim}			;	/* ignore whitespace */
{paren}			{ return yytext[0]; }
{colon}			{ return yytext[0]; }
{sign}			{ return yytext[0]; }
{integer}			{ yylval->ival = (int) atoi(yytext); 
			  		return INTEGER; }. 
{id}	    		{ /* printf("scanned id %s\n",yytext);*/
				 yylval->sval = strdup(yytext); 
		 	  		return ID; }
{id1}	    		{ /* printf("scanned id %s\n",yytext);*/
				 yylval->sval = strdup(yytext); 
		 	  		return ID1; }
\n		   			{ line_num ++; /*return yytext[0];*/}
. 					{ yylval->sval = strdup(yytext); 
					return UNKNOWN; }
%%

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

