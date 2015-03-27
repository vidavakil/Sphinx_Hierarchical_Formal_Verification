#include "type.h"
#include "sphinx.h"

extern int MODE;
extern bdd_t 	*From, *init_state;
extern context_ptr	cur_cir; 
extern bdd_manager	*Bdd_Man;
extern bdd_t 		*R;
extern bdd_t 		*signal_var[];  // was *2 before Feb 10

bdd_t *Reach (bdd_t *bdd, bdd_t *From, bdd_t *to, int mode);
int Check_Hazard_to_Gate (gate_ptr gptr, gate_ptr gptr1, signal_ptr sptr, bdd_t *tr);
int Check_Hazard_to_Spec (gate_ptr gptr, gate_ptr gptr1, signal_ptr sptr, bdd_t *tr, int index);
int Check_Hazard_to_Env (gate_ptr gptr, bdd_t *tr);
void Generate_Error_Trace (bdd_t *err_state, bdd_t *init_state, bdd_t *FReached);


/*******************************************************/
// Error Trace Functions
/*******************************************************/
bdd_t *Reach (bdd_t *bdd, bdd_t *From, bdd_t *to, int mode)
{
  int i, fixed = 0;
  gate_ptr *gpptr, gptr;
  bdd_t		*visited, *front, *temp0, *temp1, *temp2, *from;
  signal_ptr *spptr;
  
//  printf ("Reach called\n");
  temp0 = bdd_and (bdd, to, 1, 1);
  if (!bdd_equal (temp0, to)) {
    printf ("to must be in bdd\n");
    exit (-1);
  }
  from = bdd_dup (From);
  
  bdd_free (temp0);  
  visited = bdd_dup (from);
//  for (;;) {
    front = bdd_zero (Bdd_Man);
    for (gpptr = DFS_List; ;) {
      for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
        temp0 = bdd_and (from, (mode == BACKWARD) ? Gate_BWDTR (gptr) : Gate_FWDTR (gptr), 1, 1);
        if (bdd_equal (temp0, bdd_zero (Bdd_Man)))
          continue;
        spptr = (Gate_Int_Out (gptr)) ? Gate_Int_Out (gptr) : Gate_Ext_Out (gptr);
        temp0 = Exist_Quantify (temp0, spptr);
            // Shift the outputs
        temp0 = Next_to_Current (temp0, spptr);
        temp2 = bdd_or (front, temp0, 1, 1);
        bdd_free (front);
        bdd_free (temp0);
        front = temp2;
      }  
      if (gpptr == DFS_List) 
        gpptr = Ext_Gates;
      else if (gpptr == Ext_Gates)
        gpptr = Sibling_Gates;
      else {
        temp0 = bdd_and (to, front, 1, 1);

        gpptr = DFS_List;
        temp1 = bdd_and (front, bdd, 1, 1);
        bdd_free (from);
        from = bdd_and (temp1, visited, 1, 0);
        temp2 = bdd_or (visited, temp1, 1, 1);
        free_ite (visited, front, temp1);
        visited = temp2;        
        front = bdd_zero (Bdd_Man);          
        if (bdd_equal (temp0, to)) {
          bdd_free (temp0);    
          fixed = 1;
          // the init state is found
          break;
        } else {          
          bdd_free (temp0);
        }
      }    
    }  
//    if (fixed)
//      break;
//  }
  return (visited);    
}

/*******************************************************/
void Generate_Error_Trace (bdd_t *err_state, bdd_t *init_state, bdd_t *FReached)
{
  int i, j, g;
  gate_ptr *gpptr, gptr;
  array_t	*vars, *nextvars, *next2vars;
  bdd_t		*BReached, *bdd, *temp0, **bddtp;
  signal_ptr *spptr, sptr;
    
// Jan 18, 99: introduced g!
  for (g = 0, gpptr = DFS_List; g < 3; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      Gate_FWDTR (gptr) = Gate_Int_Out (gptr) ? Gate_Int_TR (gptr) : Gate_Ext_TR (gptr); // no duplication!
      Gate_BWDTR (gptr) = bdd_dup (Gate_FWDTR (gptr));
      if (bdd_equal (Gate_BWDTR (gptr), bdd_zero (Bdd_Man)))
        continue;
      spptr = (Gate_Int_Out (gptr)) ? Gate_Int_Out (gptr) : Gate_Ext_Out (gptr);
      vars = array_alloc (bdd_tp, 1);
      nextvars = array_alloc (bdd_tp, 1);
      next2vars = array_alloc (bdd_tp, 1);
      for (j = 0; (sptr = spptr[j]) != NULL; j++) {
        array_insert_last (bdd_tp, next2vars, bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr)));
        array_insert_last (bdd_tp, nextvars, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
        array_insert_last (bdd_tp, vars, bdd_get_var (Bdd_Man, Signal_Order (sptr)));      
      } 
      Gate_BWDTR (gptr) = bdd_substitute1 (Gate_BWDTR (gptr), nextvars, next2vars); 
      Gate_BWDTR (gptr) = bdd_substitute1 (Gate_BWDTR (gptr), vars, nextvars); 
      Gate_BWDTR (gptr) = bdd_substitute1 (Gate_BWDTR (gptr), next2vars, vars); 
      array_free (vars);
      array_free (nextvars);
      array_free (next2vars);
/*
      Print_Bdd (Gate_FWDTR (gptr), Gate_String (gptr));
      Print_Bdd (Gate_BWDTR (gptr), Gate_String (gptr));
*/
    }  
    if (g == 0) 
      gpptr = Ext_Gates;
    else if (g == 1)
      gpptr = Sibling_Gates;  
      
  }
  BReached = bdd_dup (err_state);
  FReached = bdd_dup (FReached);
/*
  Print_Bdd (FReached, "R");
  Print_Bdd (err_state, "error_state ");
  Print_Bdd (init_state, "init_state ");
*/
  for (;;) {
    BReached = Reach (FReached, err_state, init_state, BACKWARD);
//    Print_Bdd (BReached, "BReached ");
    bdd_free (FReached);
    FReached = Reach (BReached, init_state, err_state, FORWARD);
//    Print_Bdd (FReached, "FReached ");
    if (bdd_equal (BReached, FReached)) {
      bdd_free (BReached);
      break;
    }
    bdd_free (BReached);  
  }
//  Print_Bdd (FReached, "Error Path Bdd :");
  
// Jan 18, 99: Introduced g!
  for (g = 0, gpptr = DFS_List; g < 3; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (bdd_equal (Gate_FWDTR (gptr), bdd_zero (Bdd_Man))) {
        continue;
      }  
      temp0 = bdd_and (Gate_BWDTR (gptr), FReached, 1, 1);
      bdd_free (Gate_BWDTR (gptr));
      Gate_BWDTR (gptr) = temp0;
      spptr = (Gate_Int_Out (gptr)) ? Gate_Int_Out (gptr) : Gate_Ext_Out (gptr);
      vars = array_alloc (bdd_tp, 1);
      nextvars = array_alloc (bdd_tp, 1);
      next2vars = array_alloc (bdd_tp, 1);
      for (j = 0; (sptr = spptr[j]) != NULL; j++) {
        array_insert_last (bdd_tp, next2vars, bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr)));
        array_insert_last (bdd_tp, nextvars, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
        array_insert_last (bdd_tp, vars, bdd_get_var (Bdd_Man, Signal_Order (sptr)));      
      } 
      Gate_BWDTR (gptr) = bdd_substitute1 (Gate_BWDTR (gptr), nextvars, next2vars); 
      Gate_BWDTR (gptr) = bdd_substitute1 (Gate_BWDTR (gptr), vars, nextvars); 
      Gate_BWDTR (gptr) = bdd_substitute1 (Gate_BWDTR (gptr), next2vars, vars); 
      array_free (vars);
      array_free (nextvars);
      array_free (next2vars);
/*
      Print_Bdd (Gate_FWDTR (gptr), Gate_String (gptr));
      Print_Bdd (Gate_BWDTR (gptr), Gate_String (gptr));
*/
      if (Gate_Int_Out (gptr)) {
        bdd_free (Gate_Int_TR (gptr));
        Gate_Int_TR (gptr) = Gate_BWDTR (gptr);
      } else {
        bdd_free (Gate_Ext_TR (gptr));
        Gate_Ext_TR (gptr) = Gate_BWDTR (gptr);
      }                  
    }  
    if (g == 0) 
      gpptr = Ext_Gates;
    else if (g == 1)
      gpptr = Sibling_Gates;   
  }
  bdd_free (FReached);
}


/*******************************************************/
/*******************************************************/
// Subcircuit verification functions
/*******************************************************/
/*******************************************************/
int Check_Hazard_to_Gate (gate_ptr gptr, gate_ptr gptr1, signal_ptr sptr, bdd_t *tr) 
{
  bdd_t *temp, *Front;

//  if (Gate_Subcir (gptr) == Subcir) { // Note: in the flat case, subcir is NULL!!!!!!
  if (Gate_Subcir (gptr) == Subcir) { // Note: in the flat case, subcir is NULL!!!!!!
   // Jan 18, 99
    temp = bdd_and (tr, Gate_Hazards (gptr), 1, 1);
    if (!bdd_equal (temp, bdd_zero (Bdd_Man))) {
      printf ("Hazard on signal %s at the input of gate %s\n",
  		  Signal_String (sptr), Gate_String (gptr) );
      Print_Bdd (temp, "\n");
      Print_Bdd (From, "From at Hazard: ");
      Print_Bdd (tr, "TRFROM");
      bdd_free (temp);	  

      return (1); // May 18
    
      // June 9: Front = From_to_To_Old (gptr1, Int_Ext, 1, 0, NULL, -1); // SHALL WE CHANGE FROM?
      Check_Fixedpoint ();           
//    Generate_Error_Trace (Front, init_state, R); // May 18
      bdd_free (Front); // Notice	  		
      return (1);      		
    }        		
    bdd_free (temp); 
  }
  return (0);

}

/*******************************************************/
int Check_Hazard_to_Spec (gate_ptr gptr, gate_ptr gptr1, signal_ptr sptr, bdd_t *tr, int index) 
{
  bdd_t *temp, *Front, *temp1;
  input_ptr iptr;
    
  iptr = &(Gate_State_Inputs (gptr)[index]);
  temp1 = bdd_dup (Input_TR (iptr)); // !!!!!!!!!!!!!!!!!! Too expensive! This is performed each time!
  temp1 = Exist_Quantify_Next (temp1, Input_Changing (iptr));
  temp = bdd_and (tr, temp1, 1, 0);
  if (!bdd_equal (temp, bdd_zero (Bdd_Man))) {
    printf ("Hazard on signal %s at the input of spec %s\n",
		  Signal_String (sptr), Gate_String (gptr) );
    Print_Bdd (temp, "Choke: ");
    Print_Bdd (From, "From at Hazard: ");
    Print_Bdd (tr, "TRFROM: ");
    Print_Bdd (Input_TR (iptr), "Input_TR: ");
    bdd_free (temp);
    bdd_free (temp1);	  

    return (1); // May 18
   
    // June 9: Front = From_to_To_Old (gptr1, Int_Ext, 1, 0, NULL, -1); // SHALL WE CHANGE FROM?
    Check_Fixedpoint ();           
//  Generate_Error_Trace (Front, init_state, R); // May 18
    bdd_free (Front); // Notice	  		
    return (1);      		
  }        		
  bdd_free (temp); 
  bdd_free (temp1); 
  return (0);
}

/*******************************************************/
int Check_Hazard_to_Env (gate_ptr gptr, bdd_t *tr) /* fixed */
{
  bdd_t *temp1, *Front;  

  if (Gate_Env (gptr) && (Gate_TRI_T (gptr) || Gate_TRIO_T (gptr))) { 
    temp1 = bdd_and (tr, Gate_TRI (gptr), 1, 0); // June 14
    if (!bdd_equal (temp1, bdd_zero (Bdd_Man))) {
      printf ("Choke to the environment of gate %s\n", Gate_String (gptr) );
      Print_Bdd (temp1, "Choke: ");
      Print_Bdd (tr, "TR");
      Print_Bdd (Gate_TRI (gptr), "Gate_TRI:");
      bdd_free (temp1);
          
      return (1); // Feb 27
    
      // June 9: Front = From_to_To_Old (gptr, Int_Ext, 1, 0, NULL, -1); // SHALL WE CHANGE FROM? 
      // ^^^^ Feb 24, Not sure about the NULL!!!!!!!!!!!!!!!
      
      Check_Fixedpoint ();           
      Generate_Error_Trace (Front, init_state, R);
      bdd_free (Front); // Notice	  		
      return (1);
    }
    bdd_free (temp1);
  }           
  return (0);        
}  
  
/*******************************************************/
int Check_Failure (gate_ptr gptr1, sig_type Int_Ext) /* fixed */
{
  int j, i, index;
  gate_ptr gptr;
  bdd_t *temp, *Front, *tr;
  signal_ptr sptr, *spptr;
  spec_index_ptr    siptr;
  
  if (Int_Ext == EXTERNAL) {
    spptr = Gate_Ext_Out (gptr1);
    tr = Gate_Ext_TRFROM (gptr1);
  }  
  else {
    spptr = Gate_Int_Out (gptr1);  
    tr = Gate_Int_TRFROM (gptr1);
  }  
 
  if (Check_Hazard_to_Env (gptr1, tr))
    return (1);   

  if (Gate_Env (gptr1) && Gate_TRO_T (gptr1)) // June 28
    return (0);
    
  for (i = 0; (sptr = spptr[i]) != NULL; i++) {
    for (j = 0; (gptr = Signal_Fanout(sptr)[j]) != NULL; j++) { // if a signal only enters SPECs, this will be empty!
      if (Check_Hazard_to_Gate (gptr, gptr1, sptr, tr))
        return (1);
    }  
    for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
      gptr = Spec_Index_Spec (siptr);
      index = Spec_Index_Index (siptr);
      if (Check_Hazard_to_Spec (gptr, gptr1, sptr, tr, index))
        return (1);
    }
  }
  return (0);  
}  
