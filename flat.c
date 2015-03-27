#include "type.h"
#include "sphinx.h"

bdd_t *New_From;

extern int	Max_External;
extern int 	oflag, kflag;
extern bdd_t 	*From, *To, *init_state;
extern bdd_manager	*Bdd_Man;
extern context_ptr	cur_cir; 
extern bdd_t 		*R;
extern bdd_t 		*signal_var[]; 

void For_Loop_And_Array_Initial (bdd_t **bptr, gate_ptr *gpptr, int mode);
void For_Loop_And_Array_Initial_Spec (bdd_t **bptr, gate_ptr *gpptr);
int Fire_All (gate_ptr gptr, int Int_Ext /**/, signal_ptr schanged /**/);

/*******************************************************/
// Flat verification functions
/*******************************************************/

/*******************************************************/
void For_Loop_And_Array_Initial (bdd_t **bptr, gate_ptr *gpptr, int mode)
{
  int i, j, k;
  bdd_t *temp, *var;
  gate_ptr gptr;
  signal_ptr *spptr, sptr;
  
  
  if (gpptr == NULL)
    return;
  
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
//    printf ("Gate %s being initialized\n", Gate_String (gptr));
    for (k = 0, spptr = Gate_Int_Out (gptr); k < 2; k++, spptr = Gate_Ext_Out (gptr)) {
      if (spptr == NULL)
        continue;
      for (j = 0; (sptr = spptr[j]) != NULL; j++) {
//        printf ("Signal %s being initialized\n", Signal_String (sptr));
        var = bdd_get_var (Bdd_Man, Signal_Order (sptr));
        temp = bdd_and (*bptr, var, 1, Signal_Init (sptr) ? 1 : 0);
        bdd_free (*bptr);
        *bptr = temp;
      }  
    }  
  }    
}

/*******************************************************/
void For_Loop_And_Array_Initial_Spec (bdd_t **bptr, gate_ptr *gpptr)
{
  int i, j;
  bdd_t *temp, *var;
  gate_ptr gptr;
  signal_ptr *spptr, sptr;
    
  if (gpptr == NULL)
    return;
  
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
//    printf ("Gate %s being initialized\n", Gate_String (gptr));
    spptr = Gate_State_All_Places (gptr);
    if (spptr == NULL)
      continue;
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
//        printf ("Signal %s being initialized\n", Signal_String (sptr));
      var = bdd_get_var (Bdd_Man, Signal_Order (sptr));
      temp = bdd_and (*bptr, var, 1, Signal_Init (sptr) ? 1 : 0);
      bdd_free (*bptr);
      *bptr = temp;
    }  
  }    
}


/*******************************************************/
int Check_Fixedpoint (void)
{
  bdd_t *temp;
  int   retval = 0;
  
/*
// May 17: To implement Chaining
  temp = bdd_or (R, New_From, 1, 1);
  bdd_free (R);
  R = temp;
  bdd_free (New_From);
  bdd_free (From);

*/
 // May 17
  temp = bdd_or (R, From, 1, 1);
  bdd_free (R);
  R = temp;
  bdd_free (From);
  
  
  From = bdd_and (To, R, 1, 0);
  bdd_free (To);
  To = bdd_zero (Bdd_Man);
  if (bdd_equal (From, bdd_zero (Bdd_Man)))
    return (1);
    
  // May 17: To implement Chaining
//  New_From = bdd_dup (From);
  
  return (0);  

}

/*******************************************************/
int Fire_All (gate_ptr gptr, int Int_Ext /**/, signal_ptr schanged /**/) /* fixed */
{
  int 		i, index;
  bdd_t     	*temp0, *temp2, *TRi, *T1i, *T2ij;
  signal_ptr 	sptr, *spptr;
  input_ptr 	iptr; 
  array_t 	*s_array; 
  spec_index_ptr siptr;
  gate_ptr gptro, *gpptr;

  // If sptr is an external signal (output of this subcircuit), then
  // check its TR. In any case, check the input_hazard of any fanout
  // gate in this same subcircuit 

  Find_Enabled (gptr, TOTAL, Int_Ext, /* NULL */ schanged);
  
  // May 4
  temp0 = (Int_Ext == EXTERNAL) ? Gate_Ext_TRFROM (gptr) : Gate_Int_TRFROM (gptr);
  if (bdd_equal (bdd_zero (Bdd_Man), temp0))
    return (1); 
  
  if (Check_Failure (gptr, Int_Ext))
    return (0);  
  
  TRi = temp0; // Should only have the next state of this signal and no places
  T1i = bdd_dup (TRi);
  s_array = array_alloc (signal_ptr, 1);	// the array version of spptr (see above!)
  spptr = (Int_Ext == INTERNAL) ? Gate_Int_Out (gptr) : Gate_Ext_Out (gptr);
  for (i = 0; (sptr = spptr[i]) != NULL; i++) {
    for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
      gptro = Spec_Index_Spec (siptr);
      index = Spec_Index_Index (siptr);
      iptr = &(Gate_State_Inputs (gptro)[index]);
      if (Input_Changing (iptr)[0] == NULL)
        continue;
      Append_ZArray_to_Array (s_array, Input_Changing (iptr)); // May 10: internal places better be excluded  

      T2ij = bdd_and (TRi, Input_TR (iptr), 1, 1); // T1ij
      temp2 = bdd_and (T1i, T2ij, 1, 1);
      bdd_free (T1i);
      T1i = temp2;
      temp2 = bdd_or (Input_Ext_TR (iptr), T2ij, 1, 1); // Has both signal and place transitions
      bdd_free (Input_Ext_TR (iptr));
      Input_Ext_TR (iptr) = temp2;
      bdd_free (T2ij);
    }
  }    
  spptr = (signal_ptr *) Array_to_ZArray (s_array);
  array_free (s_array);
      
  From_to_To (gptr, Int_Ext, T1i, spptr, 0); // SHALL WE CHANGE FROM?

  free (spptr);      
  return (1);
}

/*******************************************************/
int Flat_Verify (void) /* fixed */
{
  int i, j, g, verified, k = 0;
  gate_ptr gptr, *gpptr;
  
  kflag = 1;
//  Test_TR_Functions ();
  printf ("\n____ FLAT VERIFICATION OF SUBCIRCUIT %s! ____\n", Subcir_Path);
  // Compute the initial From
  
//  Check_Fanouts ();
   
  From = bdd_one (Bdd_Man);
  For_Loop_And_Array_Initial (&From, DFS_List, INTERNAL);
  For_Loop_And_Array_Initial (&From, Ext_Gates, EXTERNAL);  
  For_Loop_And_Array_Initial (&From, Sibling_Gates, UNDEFINED);
  For_Loop_And_Array_Initial_Spec (&From, list_of_specs);
  
  init_state = bdd_dup (From);
//  New_From = bdd_dup (From); // May 17: To implement Chaining

//  Print_Bdd (From, "From");
  if (oflag) {
    printf ("Encoded initial state of the state graph: ");
    Print_A_State (From, TOTAL);
  }  

// Jan 18, 99: g added for the case that one of the lists is a NULL!
  for (g = 0, j = 0, gpptr = DFS_List; g < 3; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++, j++) {// ;
/*
      printf ("%s ", Gate_String (gptr));
      if (Gate_Env (gptr) && Gate_TRO_T (gptr)) {
        printf ("Gate_TRO, ");
      }    
      if (Gate_Env (gptr) && Gate_TRI_T (gptr)) {
        printf ("Gate_TRI, ");
      }    
      if (Gate_Env (gptr) && Gate_TRIO_T (gptr))
        printf ("Gate_TRIO, ");
*/
    }    
    if (g == 0)
      gpptr = Ext_Gates;
    else if (g == 1)
      gpptr = Sibling_Gates;
  }
  printf ("\n");
  if (j > Max_External)
    Max_External = j;        
      
  verified = 1;
/*
  Print_Bdd (From, "From :");
  printf ("The size of flat From BDD = %d\n", bdd_size (From));
*/
  R = bdd_dup (From);
  To = bdd_zero (Bdd_Man);
  for (j = 0;; j++) {
    for (i = 0; (gptr = DFS_List[i]) != NULL; i++) {
      if (!Fire_All (gptr, INTERNAL, NULL)) {
        verified = 0;
        break;
      }  
    }  
    if (verified == 0)
      break;     
    for (i = 0; (gptr = Ext_Gates[i]) != NULL; i++) {
      if (!Fire_All (gptr, EXTERNAL, NULL)) {
        verified = 0;
        break;
      }  
    }  
    if (verified == 0)
      break;     
    for (i = 0; (gptr = Sibling_Gates[i]) != NULL; i++) {
      if (Gate_Ext_Out (gptr)) { // new
        if (!Fire_All (gptr, EXTERNAL, NULL)) {
          verified = 0;
          break;
	}
      } else { // new
        if (!Fire_All (gptr, INTERNAL, NULL)) {
          verified = 0;
          break;
        }  
      }
    }
    if (verified == 0) 
      break;        
    if (Check_Fixedpoint ()) {
      if (k) 
        break;
      k++;
    }   
  }
//  Print_Bdd (R, "R = ");
  printf ("Flat subcircuit %s%s verified after %d iterations\n", Subcir_Path, verified ? "" : " not", j);
  printf ("The BDD size of Reached flat states = %d\n", bdd_size (R));
  Print_Graph (TOTAL);
  Print_Circuit_Graph (TOTAL);
  free_ite (From, To, R); // New
  
// sept 30
  for (i = 0; (gptr = DFS_List[i]) != NULL; i++) {
    bdd_free (Gate_Int_TR (gptr));
    Gate_Int_TR (gptr) = bdd_zero (Bdd_Man);
    bdd_free (Gate_Int_TRFROM (gptr));
    Gate_Int_TRFROM (gptr) = bdd_zero (Bdd_Man);
  }  
  for (i = 0; (gptr = Ext_Gates[i]) != NULL; i++) {
    bdd_free (Gate_Ext_TR (gptr));
    Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
    bdd_free (Gate_Ext_TRFROM (gptr));
    Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);
  }
  
  // new
  for (i = 0; (gptr = Sibling_Gates[i]) != NULL; i++) {
    if (Gate_Int_Out (gptr)) {
      bdd_free (Gate_Int_TR (gptr));
      Gate_Int_TR (gptr) = bdd_zero (Bdd_Man);
      bdd_free (Gate_Int_TRFROM (gptr));
      Gate_Int_TRFROM (gptr) = bdd_zero (Bdd_Man);
    } else {
      bdd_free (Gate_Ext_TR (gptr));
      Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
      bdd_free (Gate_Ext_TRFROM (gptr));
      Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);    
    }  
  }  
  
  kflag = 0;
  printf ("_______________________________________________________\n");
  return (verified);
}
