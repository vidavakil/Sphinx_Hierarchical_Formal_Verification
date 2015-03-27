 /* The format of the input file need to be such that first the inputs/outputs/internal/dummys are read in and the transitions are given and no toggle change is allowed. */

%{
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
 
%}

%union {
	char *sval;
	//int ival;
	//gate_ptr gptr;
	signal_ptr sptr;
	transition_ptr tptr;
	name_ptr   nptr;
	//input_ptr  inptr;
}; 
 
 //%pure_parser
 
%token <sval> ID 
%token <sval> INTEGER 
 //%token <sval> UNKNOWN 
%token INPUTS OUTPUTS INTERNALS GRAPH END MARKING NAME INIT_STATE DUMMYS
%type <sptr> place
%type <tptr> trans
%type <sptr> edge_place
%type <tptr> edge_trans


%%
input		: 
		| input line
		| input END 
	{
// Feb 10		Enter_Signal_Chain(); // new		
	}
		;


line		: '\n'	
		| NAME ID '\n'
		| inputs '\n'
	    	| outputs '\n'
        	| internals '\n'
		| dummys '\n'
		| GRAPH '\n'	
		| edges	'\n' 
//		| INIT_STATE init_state '\n' 
		| MARKING marking '\n'
		| error
	{ 
		printf ("Parsing Error at line %d\n", vidaline_num);
		printf ("       %s\n", vidatext);
	}
		;

inputs		: INPUTS // ID // ????? what is ID?
		| inputs ID
	{
		Find_Signal ((char*)$2, OUTPUT_NODE);
//		printf ("input %s\n", Name_String (Signal_Name (Find_Signal ((char*)$2, OUTPUT_NODE))));
		// May 26: was INPUT_NODE
	}
		;

outputs		: OUTPUTS
		| outputs ID
	{
		Find_Signal ((char*)$2, INPUT_NODE);
//		printf ("output %s\n", Name_String (Signal_Name (Find_Signal ((char*)$2, INPUT_NODE))));
		// May 26: was OUTPUT_NODE
	}
		;

internals	: INTERNALS
		| internals ID
	{
	    	Find_Signal ((char*)$2, INTERNAL_NODE);
	}
		;

dummys		: DUMMYS
		| dummys ID

	{
		printf("found dummy\n");
	    	Find_Signal ((char*)$2, DUMMY_NODE);
	}
		;

edges		: edge_place
	{
	}
		| edge_trans
	{
	}		
		;

edge_place 	: place
	{
//		printf ("edge-place first\n");
		$$ = $1;
	}
		| edge_place trans
	{
//		printf ("edge-place trans\n");
		Enter_Trans((signal_ptr)$1, (transition_ptr)$2, SIG_NULL_PTR);
	}

edge_trans 	: trans
	{
//		printf ("edge-trans first\n");
		$$ = $1;
	}
		| edge_trans trans
	{
//		printf ("edge-trans trans\n");
		Enter_Trans(SIG_NULL_PTR, (transition_ptr)$1, get_implicit_place((transition_ptr)$1, (transition_ptr)$2));
		Enter_Trans(get_implicit_place((transition_ptr)$1, (transition_ptr)$2), (transition_ptr)$2, SIG_NULL_PTR); 
	}
		| edge_trans place
	{
//		printf ("edge-trans place\n");
		Enter_Trans(SIG_NULL_PTR, (transition_ptr)$1, (signal_ptr)$2);
	}

trans		: ID '+'	
	{
		$$ = Find_Trans ((char*)$1, "", '+');
	}
		| ID '+' '/' INTEGER
	{
		$$ = Find_Trans ((char*)$1, (char*)$4, '+');
	}
		| ID '-'
	{
	    	$$ = Find_Trans ((char*)$1, "", '-'); 
	}
		| ID '-' '/' INTEGER
	{
		$$ = Find_Trans ((char*)$1, (char*)$4, '-');
	}
		;

place		: ID
	{
		$$ = Find_Signal ((char*)$1, PLACE_NODE);
//		printf ("place %s\n", Name_String (Signal_Name ($$)));
	}
		;
/*
init_state 	: '{' init_state_val '}'
		;

init_state_val	: 
		| init_state_val INTEGER
	{
		Init_Signal((signal_ptr)array_fetch(signal_ptr, st_var_array, i++), (int) atoi((char *)$2));
	}
		;
*/

marking		: '{' marking_set '}'
		;

marking_set	: marking_set mark
		| mark
		;
	
mark		: '<' trans ',' trans '>'
	{
	        Enter_Mark (get_implicit_place((transition_ptr)$2, (transition_ptr)$4));
	}
		| place 
	{
	    	Enter_Mark ($1);
	}
		;
	 



%%

vidaerror (char *s)
{
    printf ("%s at line number %d reading %s\n", s, vidaline_num, vidatext);
}


