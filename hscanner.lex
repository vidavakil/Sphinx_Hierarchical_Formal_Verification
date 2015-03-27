%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "hparser.h"

extern int vidaline_num;  
YYSTYPE vidalval;
#undef vidawrap

%}

delim	[ \t]+
letter	[A-Za-z_]
digit	[0-9]
id	{letter}({letter}|{digit})*
number	{digit}+(\.{digit}+)?([eE][+-]?{digit}+)?
comments {delim}*#.*
integer  {digit}+	

%%

{delim}	        {}     		
{comments}      {}  		
\.name|\.model	{ return NAME; }
\.inputs 	{ return INPUTS; }
\.outputs	{ return OUTPUTS; }
\.internal 	{ return INTERNALS; }
\.dummys        { return DUMMYS; }
\.graph   	{ return GRAPH; }
\.init_state    { return INIT_STATE; }
\.marking	{ return MARKING; }
\.end		{ return END; }
{integer}	{ vidalval.sval = strdup(vidatext);
		  return INTEGER; }
{id}	    	{ vidalval.sval = strdup(vidatext);
		  return ID; }

\n		{ vidaline_num ++; return vidatext[0];} 
. 		{ return vidatext[0]; }

%%

int vidawrap () 
{
    return 1;
}





