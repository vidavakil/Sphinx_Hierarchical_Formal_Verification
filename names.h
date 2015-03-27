#ifndef NAMESH
#define NAMESH
#include "struct.h"

#define UNDEFINED		0

#define NAME_MAX_ENTRIES        2000

typedef enum {
	GATE = 1,
	SIGNAL,
	SUBCIR,
	MODULE,
	TRANSITION
} item_type;	

typedef struct name_type {      /* A name entry consists of a   */
   	char	*string;   	
	item_type	type;      	/* signal/gate/SUBCIR/MODULE */
	union {
	  signal_ptr 	sptr;
	  transition_ptr tptr; // hoshik
	  gate_ptr 	gptr;
	  module_ptr    mptr;   
        } p;	
	struct  name_type *next;     /* pointer to the next name in  */
} Name_Type;  /* the same hash chain          */

#define  Name_String(x)		((x)->string)   /* Select the string */
#define  Name_Type(x)		((x)->type)
#define  Name_Gate(x)        	((x)->p.gptr)    
#define  Name_Signal(x)        	((x)->p.sptr)
#define  Name_Module(x)		((x)->p.mptr)    
#define  Name_Transition(x)     ((x)->p.tptr)
#define  Name_Next(x)        	((x)->next)     /* Select the next name */
#define  Name_Set_Gate(x)	((x)->type=GATE)
#define  Name_Set_Signal(x)	((x)->type=SIGNAL)

void     Name_Init_Table     ();
void     Name_Print_Stats    ();
name_ptr Name_Find_Str       (char *string);
name_ptr Name_Enter_Str      (char *string);
name_ptr Name_Find_Enter_Str (char *string);
void     Name_Debug_Graph    ();

#define NAME_NULL_PTR     ((name_ptr)NULL)


#endif
