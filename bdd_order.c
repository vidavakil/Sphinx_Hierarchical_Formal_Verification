#include "type.h"
#include "sphinx.h"

extern int rflag;
extern bdd_t 		*signal_var[];
extern bdd_manager	*Bdd_Man;
extern context_ptr	cur_cir; 
extern signal_ptr	order[]; 

void Print_Order_Signal_ZArray (signal_ptr *spptr);
void Change_Signal_Ord (signal_ptr *spptr, int size, int base, int mode);
int Signal_Compare (signal_ptr *sptr1, signal_ptr *sptr2);
void Sort_Signal_List (signal_ptr *spptr, int size);

/*******************************************************/
// BDD ordering functions
/*******************************************************/
/*******************************************************/
void Print_Order_Signal_ZArray (signal_ptr *spptr)
{
  int i;
  signal_ptr sptr;
  
  for (i = 0; (sptr = spptr[i]) != NULL; i++)
    printf ("%s %d ", Signal_String (sptr), Signal_Order (sptr));
  printf ("size = %d\n", i);  
}

/*******************************************************/
void Change_Signal_Ord (signal_ptr *spptr, int size, int base, int mode)
{
  int i, j;
  
  for (j = base, i = 0; i < size; i++) {
    Signal_Order (spptr[i]) = j+1; 
    Signal_Next_Order (spptr[i]) = j;
    Signal_Next2_Order (spptr[i]) = j+2;
    order[j] = order[j+1] = order[j+2] = spptr[i];
    j = j+3;
//      printf ("assigned Order %d to signal %s\n", j, 
//      		Signal_String (spptr[i]));
  }
}

/*******************************************************/
int Signal_Compare (signal_ptr *sptr1, signal_ptr *sptr2)
{
  if (Signal_Order (*sptr1) < Signal_Order (*sptr2))
    return (-1);
  else if (Signal_Order (*sptr1) > Signal_Order (*sptr2))
    return (1);  
  else
    return (0);  
}

/*******************************************************/
void Sort_Signal_List (signal_ptr *spptr, int size)
{
//  Print_Order_Signal_ZArray (spptr);
  qsort (spptr, size, sizeof (signal_ptr), Signal_Compare );
//  Print_Order_Signal_ZArray (spptr);
}

/*******************************************************/
void Change_Orders (void)
{
  if (rflag) {
    Sort_Signal_List (internal_list, Internal_Num);
    Change_Signal_Ord (internal_list, Internal_Num, 0, INTERNAL);
    Sort_Signal_List (external_list, External_Num);
    Change_Signal_Ord (external_list, External_Num, Internal_Num * 3, EXTERNAL); // April 6: 3 was 2
  } else {
  // The first Sig_Num IDs are reserved for next value of signals
  // the second Sig_Num IDs are reserved for the signals themselves 
    Sort_Signal_List (signal_array, Sig_Num);
    Change_Signal_Ord (signal_array, Sig_Num, 0, INTERNAL);  
  }  
}

/*******************************************************/
void BDD_Order (signal_ptr sptr)
{
  signal_ptr sptr1;
  int 	   i;
  
  for (i = 0, sptr1 = sptr; sptr1 != NULL; sptr1 = Signal_Next (sptr1), i++) {
    Signal_Next2_Order (sptr1) = i; 
    Signal_Next_Order (sptr1) = i + Sig_Num;
    Signal_Order (sptr1) = i + Sig_Num * 2;
    order[i] = order[i+Sig_Num] = order[i+Sig_Num*2] = sptr1;	// global variable 
/*
    printf ("assigned Order %d to signal %s\n", Signal_Order (sptr1), 
    		Signal_String (sptr1));
*/
  }  
}  

/*******************************************************/
void Set_Signal_Vars (void)
{
  int i, j;
  
  for (i = 0; i < Sig_Num; i++) {
    j = Signal_Order (signal_array [i]);
    signal_var[j] = bdd_get_variable (Bdd_Man, j); 
    j = Signal_Next_Order (signal_array [i]);
    signal_var[j] = bdd_get_variable (Bdd_Man, j);   
    j = Signal_Next2_Order (signal_array [i]);
    signal_var[j] = bdd_get_variable (Bdd_Man, j);
  }  
}
