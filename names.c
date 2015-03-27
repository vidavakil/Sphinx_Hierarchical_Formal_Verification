#include <stdio.h>
#include <stdlib.h>
//#include "basic.h"
#include "hash.h"
//#include "gate.h"

#include "names.h"

	    /* Name table: headers of collision chains (buckets?) */
name_ptr name_table [NAME_MAX_ENTRIES];

/* Counters to keep track of interesting statistics. */
int name_stats_fcall = 0;     /* Total number of calls to Name_Find_Str */
int name_stats_ecall = 0;     /* Total number of calls to Name_Enter_Str */
int name_stats_comps = 0;     /* Number of string comparisons */
int name_stats_occup = 0;     /* Number of active chains (buckets) */
int name_stats_succs = 0;     /* Number of successful searches */
int name_stats_fails = 0;     /* Number of unsuccessful searches */

/*
 *  Name_Table_Init_Table: Initializes the name hash table by
 *       setting all entries to NULL_NAME, then enters all
 *       keywords (upper and lower case) and special literal: 
 *       TRUE, FALSE, NIL.
 */
void Name_Init_Table ()

   {int inx;

    for (inx = 0;  inx < NAME_MAX_ENTRIES;  inx ++)
        name_table [inx] = NAME_NULL_PTR;

   }


int      last_hash_entry;
name_ptr last_name_ptr;

/* This function searches for string in the name table by hashing its name
 * and then comparing it against all strings in the same chain. If the
 * string is found, the name structure is returned; otherwise it returns
 * NAME_NULL_PTR. variables last_hash_entry and last_name_ptr are used
 * to remember the hash_entry (chain) searched and the last member of
 * the chain. This variables should be used in Name_Enter_Str to avoid
 * searching twice.
 * 
 * 		YOU HAVE TO WRITE THIS FUNCTION
 */
name_ptr Name_Find_Str (char *string)

   {name_ptr nptr;

    last_hash_entry = Hash_Get_Map (string, NAME_MAX_ENTRIES);
    last_name_ptr   = NAME_NULL_PTR;

    name_stats_fcall ++;
    for (nptr = name_table [last_hash_entry];  nptr != NAME_NULL_PTR;
         nptr = Name_Next (nptr))
       {last_name_ptr = nptr;
        name_stats_comps ++;
        if (! strcmp (Name_String (nptr), string) )
           {name_stats_succs ++;
            return (nptr);
	   }
       }

    name_stats_fails ++;
    return (NAME_NULL_PTR);
   }


/*
 * This function takes a string and a token type and enters them in
 * the name table. Variables last_hash_entry and last_name_ptr should
 * be used to avoid searching again. This function is only called when
 * we know that the string is NOT in the table. A pointer to the new
 * name structure should be returned. 
 * 
 * 		YOU HAVE TO WRITE THIS FUNCTION
 */ 
name_ptr Name_Enter_Str (char *string)

   {name_ptr tmp;

    name_stats_ecall ++;
    tmp = (name_ptr) malloc (1 * sizeof (Name_Type));
    Name_String(tmp) = (char *) malloc(strlen(string) + 1);
    *(Name_String(tmp)) ='\0' ;
    strcat(Name_String(tmp),string);
    Name_Type(tmp) = UNDEFINED;
    Name_Gate(tmp) = NULL;
    Name_Next(tmp) = NAME_NULL_PTR;

    if (last_name_ptr == NAME_NULL_PTR)
       {name_stats_occup ++;
        name_table [last_hash_entry] = tmp;
       }
    else
        Name_Next (last_name_ptr) = tmp;

    return (tmp);
   }


/* 
 * This function is called only when we know that a string is not in
 * the name table. It calls Name_Find_Str and Name_Enter_Str.
 */
name_ptr Name_Find_Enter_Str (char *string)

   {Name_Find_Str (string);
    return (Name_Enter_Str (string));
   }

void Name_Debug_Graph ()
{
    return ;
/*
    int i;
    name_ptr nptr;

    for (i=0; i < NAME_MAX_ENTRIES; i++) {
	if ( (nptr = name_table[i]) == NAME_NULL_PTR ) continue;

	Print_Gate ( Name_Node (nptr));
     	
	for ( nptr = Name_Next (nptr);	
	      nptr != NAME_NULL_PTR; 
	      nptr = Name_Next (nptr))
	     Print_Gate ( Name_Node (nptr) );
    }
*/
}

    


/* 
 * This function prints simple statistics about the use of the name table
 */
/*
void Name_Print_Stats ()

   {name_ptr nptr;
    int inx;

    if (1)
       {printf ("\n");
        printf ("Pos   Type String\n");
        printf ("=================\n");
        for (inx = 0;  inx < NAME_MAX_ENTRIES;  inx ++)
            if ((nptr = name_table [inx]) != NAME_NULL_PTR)
	       {
	        printf ("%03d\t%01d  %s\n", inx, Gate_Type (Name_Node(nptr))
                             ,  Name_String (nptr));
                for (nptr  = Name_Next (nptr);  
                     nptr != NAME_NULL_PTR;
                     nptr  = Name_Next (nptr))
	            printf ("\t%01d  %s\n", Gate_Type (Name_Node(nptr)),
                                            Name_String (nptr));
	       }
       }

    return;
    printf ("\n");
    printf ("\t\tName Table Statistics\n");
    printf ("\t\t=====================\n\n");

    printf ("Name_Find_Str called   = %5d\n", name_stats_fcall);
    printf ("   Number of successes = %5d (%6.4f)\n", name_stats_succs,
                (float) name_stats_succs / (float) name_stats_fcall);
    printf ("   Number of failures  = %5d (%6.4f)\n", name_stats_fails,
                (float) name_stats_fails / (float) name_stats_fcall);
    printf ("   Probes per call     = %7.4f\n", 
                (float) name_stats_comps / (float) name_stats_fcall);
    printf ("\n");
    printf ("Name_Enter_Str called  = %5d\n", name_stats_ecall);
    printf ("   Occupancy ratio     = %7.4f\n", 
                (float) name_stats_occup / (float) NAME_MAX_ENTRIES);
    printf ("   Entries per chain   = %7.4f\n",
                (float) name_stats_ecall / (float) name_stats_occup);
    printf ("\n");
   }
*/