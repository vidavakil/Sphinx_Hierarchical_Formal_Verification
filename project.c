#include "type.h"
#include "sphinx.h"

extern int		any_spec;
extern bdd_manager	*Bdd_Man;
extern bdd_t		*Reached;
extern int		MODE;
extern context_ptr	cur_cir; 
extern bdd_t 		*R;
extern bdd_t 		*signal_var[]; 

int Safe_Projection (gate_ptr *gpptr);
bdd_t *P_PPP (signal_ptr *spptr); 
int Is_Internally_Reachable (bdd_t *from, bdd_t *to, bdd_t *closure, array_t *Q, array_t *QQ);
bdd_t *Find_Transitive_Closure (gate_ptr *hidden, array_t **Q, array_t **QQ);
void Project_TR (gate_ptr *visible, gate_ptr *hidden, signal_ptr *proj_sig);
void Project_TR_Old (gate_ptr *visible, gate_ptr *hidden);
signal_ptr *Keep_Places (signal_ptr *spptr);
void Project_Env (gate_ptr *visible, gate_ptr *hidden);

/*******************************************************/
// Projection Functions
/*******************************************************/
int Safe_Projection (gate_ptr *gpptr)
{
  int i, j;
  gate_ptr gptr;
  char ch;
  array_t *quantify;
  signal_ptr sptr;
  bdd_t *temp, *temp2;
  
Again:
  quantify = array_alloc (bdd_tp, 1);
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    printf ("Gate %s\n", Gate_String (gptr));
    if (Gate_Ext_Out (gptr) == NULL)
      continue;
    printf ("Do you want to project the outputs of gate %s? (y/n)", Gate_String (gptr));
    while ((ch = tolower (getchar ())) != 'y' && ch != 'n');
    if (ch == 'y')
      for (j = 0; (sptr = Gate_Ext_Out (gptr)[j]) != NULL; j++)
        array_insert_last (bdd_tp, quantify, bdd_get_var (Bdd_Man, Signal_Order (sptr)));  
  }
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    Gate_PTR (gptr) = bdd_smooth (Gate_Ext_TR (gptr), (array_t *) quantify);
    temp = bdd_and (Gate_PTR (gptr), R, 1, 1);
    if (!bdd_equal (temp, Gate_Ext_TR (gptr))) {
      array_free (quantify);
      temp2 = bdd_xor (temp, Gate_Ext_TR (gptr));
      Print_Bdd (temp2, "Problem Spot ");
      bdd_free (temp);
      bdd_free (temp2);
      for (j = 0; j < i; j++)
        bdd_free (Gate_PTR (gpptr[i]));
      printf ("Not a safe projection for gate %s!\n", Gate_String (gptr));
      printf ("Do you want to try projection again?(y/n)");
      while ((ch = tolower (getchar ())) != 'y' && ch != 'n');
      if (ch == 'y')  
        goto Again;  
      return (0);
    }
    bdd_free (temp);
  }
  printf ("Safe Projection exists!\n");
  getchar ();
  array_free (quantify);      
}


/*******************************************************/
bdd_t *P_PPP (signal_ptr *spptr) 
{
  signal_ptr sptr;
  int i;
  bdd_t	 *temp0, *temp1, *temp2;
  
  temp0 = bdd_one (Bdd_Man);
  for (i = 0; (sptr = spptr[i]) != NULL; i++) {
    temp1 = bdd_xor (bdd_get_var (Bdd_Man, Signal_Order (sptr)),
    		     bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr)) );
    temp2 = bdd_and (temp0, temp1, 1, 0);
    bdd_free (temp1);
    bdd_free (temp0);
    temp0 = temp2;   		     
  }
  return (temp0);
}

/*******************************************************/

int Is_Internally_Reachable (bdd_t *from, bdd_t *to, bdd_t *closure, array_t *Q, array_t *QQ) 
{
  bdd_t *temp0, *temp1;

  // See if by a set of hidden signal transitions, every state in 'from' will always reach some 
  // state in 'to'
  // If an io signal is hidden, only if all of its changing places are also hidden we can consider
  // its transitions. 
  // Transitions of hidden places are taken into account only if there is a hidden io
  // all of its changing places are also hidden.
  // In all other cases, we simply project away any io signal or places.
  // To implement: we start with the from set and from it we fire some hidden signal.
  // If a state in to is reached, we exclude that state. Otherwise, from this state
  // and the rest of from, we fire another hidden signal (in a loop), and repeat until
  // either no state is remained in from (we update from each time), or no hidden transition
  // is enabled in any state in from.

  printf ("Transitive closure used\n");
  temp0 = bdd_and (from, closure, 1, 1);

  temp1 = bdd_smooth (temp0, Q);
  bdd_free (temp0);
  temp0 = bdd_substitute1 (temp1, QQ, Q);
//  bdd_free (temp1);
  
//  temp0 = Next_to_Current_Array (temp0, Q);

  temp1 = bdd_and (temp0, to, 1, 0);
  bdd_free (temp0);
  if (bdd_equal (temp1, bdd_zero (Bdd_Man))) {
    bdd_free (temp1);
    return (1);
  }
  bdd_free (temp1);
  return (0);    
}

/*******************************************************/
/*
void Prepare_Projection (gate_ptr *visible, gate_ptr *hidden, signal_ptr spptr)
{
  for (i = 0; (gptr = hidden[i]) 
}
*/

/*******************************************************/
bdd_t *Find_Transitive_Closure (gate_ptr *hidden, array_t **Q, array_t **QQ)
{
// It should be emphasized that the correct way of projecting the TRs of visible external signals
// is to find the transitive closure of the disjuncted TR of internal signals, and finding out whether
// projecting transition sequences p+v (where p+ is a sequence of hidden transitions, and v is a single 
// visible signal transition) onto the visible signals can introduce any previously non-existing 
// visible signal transition from a reachable state.
// The result will not be the same had we tried projecting vp+ transitions, and in fact that method
// is not a correct implementation of projection. The reason is that there may be two states with the
// same visible coding (but different hidden signal values), such that from one of them v is eventually
// fireable, but from the other one w is eventually fireable. In such a case, although the behavior 
// may be actually projectable, the second method will always give negative results.
// The first method (p+v) will give the correct result, cause each of the two sequences (p+v and p+w)
// will be captured in their corresponding TRs (those of v and w). If there is a reachable state with 
// the same visible coding, all sequences starting from it and leading to any state from which only 
// visible signals can be fired are captured in the transitive closure of hidden external signals.
// Not all such states will be present if we use the second method. There may be a reachable state
// from which only w is fireable, and not even a hidden signal. Then, the second method will give a
// negative result since from that state a v transition was not possible.

  int i, j, k, index;
  gate_ptr gptr, gtemp, sgptr;
  bdd_t	   *disjunct, *temp0, *temp1, *disjunct0, *temp2;
  array_t  *P, *PP, *PPP;
  signal_ptr sptr, *spptr, sptr1;
  long start, end;  
  spec_index_ptr siptr;
  input_ptr iptr;
  

  if (hidden[0] == 0)
    return (bdd_zero (Bdd_Man));
  start = util_cpu_time ();

  // April 1
  P = array_alloc (bdd_tp, 1);
  PP = array_alloc (bdd_tp, 1);
  PPP = array_alloc (bdd_tp, 1);

// The following is just to prevent multiple insertions of places in the P arrays
  for (i = 0; (gtemp = hidden [i]) != NULL; i++) {
    if (Gate_Transitive (gtemp) == 1) {
      spptr = (Gate_Ext_Out (gtemp) ? Gate_Ext_Out (gtemp) : Gate_Int_Out (gtemp));
      for (j = 0; (sptr = spptr[j]) != NULL; j++) {
        for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
          index = Spec_Index_Index (siptr);
          sgptr = Spec_Index_Spec (siptr);
          iptr = &(Gate_State_Inputs (sgptr)[index]);
          if (array_n (Input_ChangingArray (iptr)) == 0)
            continue;
          for (k = 0; (sptr1 = Input_Changing (iptr)[k]) != NULL; k++)
            Signal_Visited (sptr1) = 0;
        }
      }      
    }
  }    
      
  for (i = 0; (gtemp = hidden [i]) != NULL; i++) {
    Gate_All_Ch (gtemp) = bdd_dup (Gate_Ext_Out (gtemp) ? Gate_Ext_Ch (gtemp) : Gate_Int_Ch (gtemp));
    if (Gate_Transitive (gtemp) == 0) {
      continue;
    }  
    spptr = (Gate_Ext_Out (gtemp) ? Gate_Ext_Out (gtemp) : Gate_Int_Out (gtemp));
    temp0 = bdd_zero (Bdd_Man);  
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      array_insert_last (bdd_tp, P, bdd_get_var (Bdd_Man, Signal_Order (sptr)));
      array_insert_last (bdd_tp, PP, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
      array_insert_last (bdd_tp, PPP, bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr)));
      for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        index = Spec_Index_Index (siptr);
        sgptr = Spec_Index_Spec (siptr);
        iptr = &(Gate_State_Inputs (sgptr)[index]);
        if (array_n (Input_ChangingArray (iptr)) == 0)
          continue;
        for (k = 0; (sptr1 = Input_Changing (iptr)[k]) != NULL; k++) {
          if (Signal_Visited (sptr1) == 0) {
            Signal_Visited (sptr1) = 1;  
            array_insert_last (bdd_tp, P, bdd_get_var (Bdd_Man, Signal_Order (sptr1)));
            array_insert_last (bdd_tp, PP, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr1)));
            array_insert_last (bdd_tp, PPP, bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr1)));             
          } 
          temp1 = bdd_xor (bdd_get_var (Bdd_Man, Signal_Order (sptr1)),
          		   bdd_get_var (Bdd_Man, Signal_Next_Order (sptr1)) );
          temp2 = bdd_or (temp1, temp0, 1, 1);
          bdd_free (temp1);
          bdd_free (temp0);
          temp0 = temp2;		    
        }
      }      
    }  
    temp1 = bdd_or (Gate_All_Ch (gtemp), temp0, 1, 1);
    bdd_free (temp0);
    bdd_free (Gate_All_Ch (gtemp));
    Gate_All_Ch (gtemp) = temp1;
  }

  disjunct = bdd_zero (Bdd_Man);
  for (i = 0; (gptr = hidden [i]) != NULL; i++) {
    // Make Gate_TRI_TRO incomplete
/*
    if (Gate_Transitive (gptr) == 0) { // Aug 23, 2000 
      continue;
    }  
*/
    temp0 = bdd_and (Gate_All_TR (gptr), 
    		     Gate_Int_Out (gptr) ? Gate_Int_Ch (gptr) : Gate_Ext_Ch (gptr), 1, 1); // Aug 23 2000
		    // Gate_Ext_Ch (gptr) ? Gate_Ext_Ch (gptr) : Gate_Int_Ch (gptr), 1, 1);
    // The following is the fastest way of creation of the disjunct bdd of hidden externals
    for (j = 0; (gtemp = hidden[j]) != NULL; j++) {
      if (i == j)
        continue;
      temp1 = bdd_and (temp0, Gate_All_Ch (gtemp), 1, 0);
      bdd_free (temp0);
      temp0 = temp1;
    }
    temp1 = temp0;

    temp0 = bdd_or (disjunct, temp1, 1, 1);
    bdd_free (disjunct);
    disjunct = temp0;
    bdd_free (temp1);
  }

  disjunct0 = bdd_dup (disjunct);
  for (;;) {
    temp0 = bdd_dup (disjunct);
    temp0 = bdd_substitute1 (temp0, PP, PPP);
    temp1 = bdd_substitute1 (temp0, P, PP);
    temp0 = bdd_and (disjunct, temp1, 1, 1);
    bdd_free (temp1);
    temp1 = bdd_smooth (temp0, PP);
    bdd_free (temp0);
    temp0 = bdd_substitute1 (temp1, PPP, PP);
    temp1 = bdd_or (disjunct0, temp0, 1, 1); // try to see if using disjunct instead of disjunct0 will speed up the process
    bdd_free (temp0);
    if (bdd_equal (disjunct, temp1)) {
      bdd_free (temp1);
      break;
    } 
    disjunct = temp1;
  }    
  *Q = P;
  *QQ = PP;
  array_free (PPP);
  bdd_free (disjunct0);
  for (i = 0; (gtemp = hidden [i]) != NULL; i++) 
    bdd_free (Gate_All_Ch (gtemp));
  end = util_cpu_time ();
  printf ("Transitive closure took %d time\n", end - start);
  return (disjunct);
}

/*******************************************************/
void Project_TR (gate_ptr *visible, gate_ptr *hidden, signal_ptr *proj_sig)
{
  long start, end;
  array_t *P, *PP, *PPP, *Q, *QQ, *R, *RR;
  int i, j, index, equal, already;
  input_ptr iptr;
  signal_ptr *spptr, sptr;
  spec_index_ptr siptr;
  gate_ptr  ogptr, gtemp, gptr;
  bdd_t *temp, *temp0, *temp1, *temp2, *temp3, *bdd, *tr_closure;
  

  start = util_cpu_time ();

/* // July 9! This will not happen
  if (hidden[0] == NULL && proj_sig[0] == NULL) { // was i == 0
    for (i = 0; (gptr = visible [i]) != NULL; i++) 
      Gate_Projected (gptr) = bdd_dup (Gate_TRI_TRO (gptr)); // Gate_TRI_TRO      
    return;
  } 
*/
  
  P = array_alloc (bdd_tp, 1);
  PP = array_alloc (bdd_tp, 1);
  PPP = array_alloc (bdd_tp, 1);
  
  printf ("Hidden signals: ");
  for (i = 0; (gtemp = hidden [i]) != NULL; i++) {
    spptr = (Gate_Ext_Out (gtemp) ? Gate_Ext_Out (gtemp) : Gate_Int_Out (gtemp));
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      array_insert_last (bdd_tp, P, bdd_get_var (Bdd_Man, Signal_Order (sptr)));
      array_insert_last (bdd_tp, PP, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
      array_insert_last (bdd_tp, PPP, bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr)));
      printf ("%s ", Signal_String (sptr));
    }  
  }

  for (i = 0; (sptr = proj_sig [i]) != NULL; i++) { // July 7
    array_insert_last (bdd_tp, P, bdd_get_var (Bdd_Man, Signal_Order (sptr)));
    array_insert_last (bdd_tp, PP, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
    array_insert_last (bdd_tp, PPP, bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr)));
    printf ("%s ", Signal_String (sptr));
  }  
  printf ("\n");

  for (i = 0; (gptr = hidden [i]) != NULL; i++) { // July 7
    if (Gate_Transitive (gptr) == 0)
      continue;
    Gate_All_Changing (gptr) = array_alloc (signal_ptr, 1); // all changing variables of this gate
    temp = bdd_dup (Gate_TRI_TRO (gptr));
    spptr = (Gate_Ext_Out (gptr) ? Gate_Ext_Out (gptr) : Gate_Int_Out (gptr));
    Append_ZArray_to_Array (Gate_All_Changing (gptr), spptr);  					
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        ogptr = Spec_Index_Spec (siptr);
        index = Spec_Index_Index (siptr);
        iptr = &(Gate_State_Inputs (ogptr)[index]);
        if (array_n (Input_ChangingArray (iptr)) == 0) 	
          continue;
        Append_ZArray_to_Array (Gate_All_Changing (gptr), Input_Changing (iptr));  					
        bdd = bdd_and (temp, Input_TR (iptr), 1, 1);         
        bdd_free (temp);
        temp = bdd;
      }
    }
    Gate_All_TR (gptr) = temp; // used for transitive closure computation
  }  

  already = 0;

  for (i = 0; (gptr = visible [i]) != NULL; i++) {
    Gate_All_Changing (gptr) = array_alloc (signal_ptr, 1); // all changing variables of this gate
    temp = bdd_and (Gate_TRI_TRO (gptr), Gate_Ext_Out (gptr) ? Gate_Ext_Ch (gptr) : Gate_Int_Ch (gptr), 1, 1); // incomplete
    spptr = (Gate_Ext_Out (gptr) ? Gate_Ext_Out (gptr) : Gate_Int_Out (gptr));
    Append_ZArray_to_Array (Gate_All_Changing (gptr), spptr);  					
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        ogptr = Spec_Index_Spec (siptr);
        index = Spec_Index_Index (siptr);
        iptr = &(Gate_State_Inputs (ogptr)[index]);
        if (array_n (Input_ChangingArray (iptr)) == 0) 	// July 7 ???????????? Shouldn't I also 
          continue;					// check to see if a changing is also external?
        Append_ZArray_to_Array (Gate_All_Changing (gptr), Input_Changing (iptr));  					
        bdd = bdd_and (temp, Input_TR (iptr), 1, 1); 
        bdd_free (temp);
        temp = bdd;
      }
    }
    temp1 = bdd_smooth (temp, PP);
    bdd_free (temp);
    temp = bdd_smooth (temp1, P);
    temp2 = bdd_and (Reached, temp, 1, 1);
    bdd_free (temp);
    temp3 = bdd_and (temp2, temp1, 1, 0);
    bdd_free (temp2);
    if (!(equal = bdd_equal (temp3, bdd_zero (Bdd_Man)))) {
      if (already == 0) {
        already = 1;
        tr_closure = Find_Transitive_Closure (hidden, &Q, &QQ);
      }
    }          
    temp3 = Exist_Quantify_Next_Array (temp3, Gate_All_Changing (gptr));
    temp1 = Exist_Quantify_Next_Array (temp1, Gate_All_Changing (gptr));
    array_free (Gate_All_Changing (gptr));
      
    if (equal || (hidden[0] != NULL && Is_Internally_Reachable (temp3, temp1, tr_closure, Q, QQ))) {
      // temp3 includes sets of reachable states from which the outputs of gptr are not enabled
      // but they are enabled in the projection of the TR of this gate.
      // We have to see if by a sequence of hidden signal transitions, we can reach from any
      // state in temp3 to a state in which the outputs of gptr are enabled.

      bdd_free (temp1);
      bdd_free (temp3);
      spptr = (Gate_Ext_Out (gptr) ? Gate_Ext_Out (gptr) : Gate_Int_Out (gptr));
      for (j = 0; (sptr = spptr[j]) != NULL; j++) {
        for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
          ogptr = Spec_Index_Spec (siptr);
          index = Spec_Index_Index (siptr);
          iptr = &(Gate_State_Inputs (ogptr)[index]);
          if (array_n (Input_ChangingArray (iptr)) == 0) {
            Input_Projected (iptr) = bdd_one (Bdd_Man); // July 9: used to be bdd_zero!
            continue;
          }  
          temp1 = bdd_smooth (Input_TR (iptr), P);
          Input_Projected (iptr) = bdd_smooth (temp1, PP); // The Input_Changing needs to be updated later when pushing this
          bdd_free (temp1);
        }
      }          
      temp1 = bdd_smooth (Gate_TRI_TRO (gptr), P);    
      temp2 = bdd_smooth (temp1, PP);
      Gate_Projected (gptr) = bdd_and (temp2, Gate_Ext_Out (gptr) ? Gate_Ext_Ch (gptr) : Gate_Int_Ch (gptr), 1, 1);
      bdd_free (temp2);
      bdd_free (temp1);     
      continue;
    } else {
      bdd_free (temp1);
      bdd_free (temp3);
      printf ("Error in projecting the TR of gate %s\n", Gate_String (gptr));
      exit (-1);
    }  
  } // for
  if (already) {
    array_free (Q);
    array_free (QQ);
    bdd_free (tr_closure);
  }  
  array_free (P);
  array_free (PP);
  array_free (PPP);
  for (i = 0; (gptr = hidden [i]) != NULL; i++) { // July 7
    if (Gate_Transitive (gptr) == 0)
      continue;
    array_free (Gate_All_Changing (gptr));
    bdd_free (Gate_All_TR (gptr));
  }  
  end = util_cpu_time ();
  printf ("PROJECT_TR took %d time\n", end - start);
}

/*******************************************************/
void Project_TR_Old (gate_ptr *visible, gate_ptr *hidden)
{

// It should be emphasized that the correct way of projecting the TRs of visible external signals
// is to find the transitive closure of the disjuncted TR of internal signals, and finding out whether
// projecting transition sequences p+v (where p+ is a sequence of hidden transitions, and v is a single 
// visible signal transition) onto the visible signals can introduce any previously non-existing 
// visible signal transition from a reachable state.
// The result will not be the same had we tried projecting vp+ transitions, and in fact that method
// is not a correct implementation of projection. The reason is that there may be two states with the
// same visible coding (but different hidden signal values), such that from one of them v is eventually
// fireable, but from the other one w is eventually fireable. In such a case, although the behavior 
// may be actually projectable, the second method will always give negative results.
// The first method (p+v) will give the correct result, cause each of the two sequences (p+v and p+w)
// will be captured in their corresponding TRs (those of v and w). If there is a reachable state with 
// the same visible coding, all sequences starting from it and leading to any state from which only 
// visible signals can be fired are captured in the transitive closure of hidden external signals.
// Not all such states will be present if we use the second method. There may be a reachable state
// from which only w is fireable, and not even a hidden signal. Then, the second method will give a
// negative result since from that state a v transition was not possible.

  int i, j;
  gate_ptr gptr, gtemp;
  bdd_t	   *disjunct, *temp0, *temp1, *temp2, *TRWP, *ETRWPP, *xor, *HiddenStable, 
  	   *disjunct0 = NULL, *disjunct1 = NULL, *disjunct2 = NULL;
  array_t  *P, *PP, *PPP, *V, *VV, *VVV;
  signal_ptr sptr, *spptr;
  long start, end;  

  start = util_cpu_time ();

  if (hidden[0] == NULL) { // was i == 0
    for (i = 0; (gptr = visible [i]) != NULL; i++) 
      Gate_Projected (gptr) = bdd_dup (Gate_TRI_TRO (gptr)); /*Gate_TRI_TRO */     
    return;
  }    
  
  // April 1
  P = array_alloc (bdd_tp, 1);
  PP = array_alloc (bdd_tp, 1);
  PPP = array_alloc (bdd_tp, 1);
  
  temp0 = bdd_one (Bdd_Man);
  for (i = 0; (gtemp = hidden [i]) != NULL; i++) {

  // This version has P and PPP
    temp2 = P_PPP (Gate_Int_Out (gtemp) ? Gate_Int_Out (gtemp) : Gate_Ext_Out (gtemp));
    temp1 = bdd_and (temp0, temp2, 1, 1);
/*
    // This version has P and PP. It has to be used with the first version of projection below
    temp2 = bdd_dup (Gate_Int_Out (gtemp) ? Gate_Int_Ch (gtemp) : Gate_Ext_Ch (gtemp));
    temp1 = bdd_and (temp0, temp2, 1, 0);
*/
    bdd_free (temp0);
    bdd_free (temp2);
    temp0 = temp1;
    spptr = (Gate_Ext_Out (gtemp) ? Gate_Ext_Out (gtemp) : Gate_Int_Out (gtemp));
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      array_insert_last (bdd_tp, P, bdd_get_var (Bdd_Man, Signal_Order (sptr)));
      array_insert_last (bdd_tp, PP, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
      array_insert_last (bdd_tp, PPP, bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr)));
    }  
  }
  HiddenStable = temp0;

  disjunct = bdd_zero (Bdd_Man);
  for (i = 0; (gptr = hidden [i]) != NULL; i++) {
    // Make Gate_TRI_TRO incomplete
    temp0 = bdd_and (Gate_TRI_TRO (gptr), 
    		     Gate_Int_Out (gptr) ? Gate_Int_Ch (gptr) : Gate_Ext_Ch (gptr), 1, 1);

    // The following is the fastest way of creation of the disjunct bdd of hidden externals
    for (j = 0; (gtemp = hidden[j]) != NULL; j++) {
      if (i == j)
        continue;
      temp1 = bdd_and (temp0, Gate_Int_Out (gtemp) ? Gate_Int_Ch (gtemp) : Gate_Ext_Ch (gtemp), 1, 0);
      bdd_free (temp0);
      temp0 = temp1;
    }
    temp1 = temp0;

/*  
    // The following will only slow down the creation of the disjunct bdd of hidden externals
    temp1 = bdd_dup (HiddenStable);
    temp1 = Exist_Quantify (temp1, Gate_Int_Out (gptr) ? Gate_Int_Out (gptr) : Gate_Ext_Out (gptr));
    temp1 = Exist_Quantify_Next (temp1, Gate_Int_Out (gptr) ? Gate_Int_Out (gptr) : Gate_Ext_Out (gptr));
    temp2 = bdd_and (temp0, temp1, 1, 1);
    bdd_free (temp0);
    bdd_free (temp1);
    temp1 = temp2;    
*/    
    temp0 = bdd_or (disjunct, temp1, 1, 1);
    bdd_free (disjunct);
    disjunct = temp0;
    bdd_free (temp1);
  }

  disjunct0 = bdd_dup (disjunct);
  for (;;) {
    temp0 = bdd_dup (disjunct);
    temp0 = bdd_substitute1 (temp0, PP, PPP);
    temp1 = bdd_substitute1 (temp0, P, PP);
    temp0 = bdd_and (disjunct, temp1, 1, 1);
    bdd_free (temp1);
    temp1 = bdd_smooth (temp0, PP);
    bdd_free (temp0);
    temp0 = bdd_substitute1 (temp1, PPP, PP);
    temp1 = bdd_or (disjunct0, temp0, 1, 1); // try to see if using disjunct instead of disjunct0 will speed up the process
    bdd_free (temp0);
    if (bdd_equal (disjunct, temp1)) {
      bdd_free (temp1);
      break;
    } 
    disjunct = temp1;
  }    


/*
  temp0 = bdd_dup (disjunct);
  temp0 = bdd_substitute1 (temp0, PP, PPP); 
  temp0 = bdd_substitute1 (temp0, P, PP);
  disjunct1 = temp0; // P, PPP

//

  // The following is to compute the transitive closure of the hidden signal transitions
  // by using the double-transitive-closure method
  temp0 = bdd_dup (disjunct);
  temp0 = bdd_substitute1 (temp0, P, PPP);
  disjunct2 = temp0; // PPP, PP
  
  // The following is to take care of Odd sequences of projected signal transitions
  // while finding the transitive closure of the TR of projected signals using 
  // dual transitions
  temp0 = bdd_and (disjunct, disjunct1, 1, 1);
  temp1 = bdd_smooth (temp0, PP);
  temp1 = bdd_substitute1 (temp1, PPP, PP);
  bdd_free (temp0);
  temp0 = bdd_or (disjunct, temp1, 1, 1);
//  disjunct0 = disjunct; // P, PP
  bdd_free (disjunct);
  bdd_free (temp1);
  disjunct = temp0;

  for (;;) {
    temp0 = bdd_and (disjunct, disjunct1, 1, 1);
    temp1 = bdd_smooth (temp0, PP);
    bdd_free (temp0);
    temp0 = bdd_and (temp1, disjunct2, 1, 1);
    temp1 = bdd_smooth (temp0, PPP);
    bdd_free (temp0);
    temp0 = bdd_or (disjunct, temp1, 1, 1);
    bdd_free (temp1);
    if (bdd_equal (disjunct, temp0)) {
      bdd_free (temp0);
      break;
    }
    bdd_free (disjunct);
    disjunct = temp0;
  }    
// 
*/

/*
  // The following is to compute the transitive closure of the hidden signal transitions
  // by using the single-transitive-closure method, which does not require disjunct2.
  // This would be slower than the double-transitive-closure method
  for (;;) {
    temp0 = bdd_and (disjunct, disjunct1, 1, 1);
    temp1 = bdd_smooth (temp0, PP);
    bdd_free (temp0);
    temp1 = bdd_substitute1 (temp1, PPP, PP);
    temp0 = bdd_or (disjunct, temp1, 1, 1);
    bdd_free (temp1);
    if (bdd_equal (disjunct, temp0)) {
      bdd_free (temp0);
      break;
    }
    bdd_free (disjunct);
    disjunct = temp0;
  }    
*/  

/*
  // The first version of projection. It has to be used with the P/PP version of HiddenStable above
  // Together, they are the slowest version! 
  for (i = 0; (gptr = visible [i]) != NULL; i++) {
    V = array_alloc (bdd_tp, 1);
    VV = array_alloc (bdd_tp, 1);
    VVV = array_alloc (bdd_tp, 1);
    spptr = (Gate_Ext_Out (gptr) ? Gate_Ext_Out (gptr) : Gate_Int_Out (gptr));
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      array_insert_last (bdd_tp, V, bdd_get_var (Bdd_Man, Signal_Order (sptr)));
      array_insert_last (bdd_tp, VV, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
      array_insert_last (bdd_tp, VVV, bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr)));
    }  

    // Make Gate_TRI_TRO incomplete
    temp1 = bdd_and (Gate_TRI_TRO (gptr), 
    		     Gate_Int_Out (gptr) ? Gate_Int_Ch (gptr) : Gate_Ext_Ch (gptr), 1, 1); // V!=VV
    Gate_Projected (gptr) = bdd_and (temp1, HiddenStable, 1, 1); // it has P=PP and V!=VV
    bdd_free (temp1); 
    
    temp0 = bdd_dup (Gate_Projected (gptr));
    temp0 = bdd_substitute1 (temp0, PP, PPP);   // P=PPP V!=VV
    temp0 = bdd_substitute1 (temp0, P, PP);   // PP=PPP V!=VV

    temp1 = bdd_substitute1 (temp0, VV, VVV);
    temp0 = bdd_substitute1 (temp1, V, VV); // VV!=VVV
    
    temp1 = bdd_and (disjunct, Gate_Int_Out (gptr) ? Gate_Int_Ch (gptr) : Gate_Ext_Ch (gptr), 1, 0); // P!=PP, V=VV

    temp2 = bdd_and (temp0, temp1, 1, 1); // P!=PP=PPP,V=VV!=VVV
    bdd_free (temp1);
    bdd_free (temp0);
    temp0 = bdd_smooth (temp2, PP); // P!=PPP
    bdd_free (temp2);
    temp0 = bdd_substitute1 (temp0, PPP, PP);    
    temp1 = bdd_smooth (temp0, VV); // V!=VVV
    bdd_free (temp0);
    temp0 = bdd_substitute1 (temp1, VVV, VV); // V!=VV
    
    temp1 = bdd_or (Gate_Projected (gptr), temp0, 1, 1);
    bdd_free (temp0);
    ETRWPP = bdd_smooth (temp1, PP); // Will have no PPP; just P
    bdd_free (temp1);
    TRWP = bdd_smooth (ETRWPP, P);
    temp0 = bdd_and (TRWP, Reached, 1, 1);
    if (!bdd_equal (temp0, ETRWPP)) {
      printf ("No safe transition relation for the projected behavior of Gate %s!\n", Gate_String (gptr));
      Print_Bdd (temp0, "TRWP_Reached: ");
      Print_Bdd (ETRWPP, "ETRWPP: ");
      xor = bdd_xor (temp0, ETRWPP);
      Print_Bdd (xor, "XOR: ");
      exit (-1);
    }
    array_free (V);
    array_free (VV);
    array_free (VVV);
    free_ite (Gate_Projected (gptr), temp0, ETRWPP);
    Gate_Projected (gptr) = TRWP;

//    printf ("Projection of gate %s was Okay!\n", Gate_String (gptr));
//        Print_Bdd (TRWP, "Projection: ");
  }
*/

/*
  // The second version of projection. It has to be used with the P/PPP version of HiddenStable above
  // Together, they are the next slowest version! 
  for (i = 0; (gptr = visible [i]) != NULL; i++) {
    V = array_alloc (bdd_tp, 1);
    VV = array_alloc (bdd_tp, 1);
    VVV = array_alloc (bdd_tp, 1);
    spptr = (Gate_Ext_Out (gptr) ? Gate_Ext_Out (gptr) : Gate_Int_Out (gptr));
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      array_insert_last (bdd_tp, V, bdd_get_var (Bdd_Man, Signal_Order (sptr)));
      array_insert_last (bdd_tp, VV, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
      array_insert_last (bdd_tp, VVV, bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr)));
    }  

    // Make Gate_TRI_TRO incomplete
    temp1 = bdd_and (Gate_TRI_TRO (gptr), 
    		     Gate_Int_Out (gptr) ? Gate_Int_Ch (gptr) : Gate_Ext_Ch (gptr), 1, 1); // V!=VV
    Gate_Projected (gptr) = bdd_and (temp1, HiddenStable, 1, 1); // it has P=PPP and V!=VV
    bdd_free (temp1); 
    
    temp0 = bdd_dup (Gate_Projected (gptr));
    temp0 = bdd_substitute1 (temp0, P, PP);   // PP=PPP V!=VV

    temp1 = bdd_substitute1 (temp0, VV, VVV);
    temp0 = bdd_substitute1 (temp1, V, VV); // VV!=VVV
    
    temp1 = bdd_and (disjunct, Gate_Int_Out (gptr) ? Gate_Int_Ch (gptr) : Gate_Ext_Ch (gptr), 1, 0); // P!=PP, V=VV

    temp2 = bdd_and (temp0, temp1, 1, 1); // P!=PP=PPP,V=VV!=VVV
    bdd_free (temp1);
    bdd_free (temp0);
    temp0 = bdd_smooth (temp2, PP); // P!=PPP
    bdd_free (temp2);    
    temp1 = bdd_smooth (temp0, VV); // V!=VVV
    bdd_free (temp0);
    temp0 = bdd_substitute1 (temp1, VVV, VV); // V!=VV
    
    temp1 = bdd_or (Gate_Projected (gptr), temp0, 1, 1);
    bdd_free (temp0);
    ETRWPP = bdd_smooth (temp1, PPP); // Will have no PPP; just P
    bdd_free (temp1);
    TRWP = bdd_smooth (ETRWPP, P);
    temp0 = bdd_and (TRWP, Reached, 1, 1);
    if (!bdd_equal (temp0, ETRWPP)) {
      printf ("No safe transition relation for the projected behavior of Gate %s!\n", Gate_String (gptr));
      Print_Bdd (temp0, "TRWP_Reached: ");
      Print_Bdd (ETRWPP, "ETRWPP: ");
      xor = bdd_xor (temp0, ETRWPP);
      Print_Bdd (xor, "XOR: ");
      exit (-1);
    }
    array_free (V);
    array_free (VV);
    array_free (VVV);
    free_ite (Gate_Projected (gptr), temp0, ETRWPP);
    Gate_Projected (gptr) = TRWP;

//    printf ("Projection of gate %s was Okay!\n", Gate_String (gptr));
//        Print_Bdd (TRWP, "Projection: ");

  }
*/

  // The third version of projection. It has to be used with the P/PPP version of HiddenStable above.
  // Together, they are the fastest version! 
  for (i = 0; (gptr = visible [i]) != NULL; i++) {
    V = array_alloc (bdd_tp, 1);
    VV = array_alloc (bdd_tp, 1);
    VVV = array_alloc (bdd_tp, 1);
    spptr = (Gate_Ext_Out (gptr) ? Gate_Ext_Out (gptr) : Gate_Int_Out (gptr));
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      array_insert_last (bdd_tp, V, bdd_get_var (Bdd_Man, Signal_Order (sptr)));
      array_insert_last (bdd_tp, VV, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
      array_insert_last (bdd_tp, VVV, bdd_get_var (Bdd_Man, Signal_Next2_Order (sptr)));
    }  

    // Make Gate_TRI_TRO incomplete
    temp1 = bdd_and (Gate_TRI_TRO (gptr), 
    		     Gate_Int_Out (gptr) ? Gate_Int_Ch (gptr) : Gate_Ext_Ch (gptr), 1, 1); // V!=VV
    temp1 = bdd_substitute1 (temp1, VV, VVV);
    temp1 = bdd_substitute1 (temp1, V, VV); // VV!=VVV
    Gate_Projected (gptr) = bdd_and (temp1, HiddenStable, 1, 1); // it has P=PPP and VV!=VVV
    bdd_free (temp1); 
    
    temp0 = bdd_dup (Gate_Projected (gptr));
    temp0 = bdd_substitute1 (temp0, P, PP);   // PP=PPP VV!=VVV
    
    temp1 = bdd_and (disjunct, Gate_Int_Out (gptr) ? Gate_Int_Ch (gptr) : Gate_Ext_Ch (gptr), 1, 0); // P!=PP, V=VV

    temp2 = bdd_and (temp0, temp1, 1, 1); // P!=PP=PPP,V=VV!=VVV
    bdd_free (temp1);
    bdd_free (temp0);
    temp0 = bdd_smooth (temp2, PP); // P!=PPP
    bdd_free (temp2);    
    temp1 = bdd_smooth (temp0, VV); // V!=VVV
    bdd_free (temp0);
    temp0 = bdd_substitute1 (temp1, V, VV); // VV!=VVV
    
    temp1 = bdd_or (Gate_Projected (gptr), temp0, 1, 1);
    bdd_free (temp0);
    temp1 = bdd_substitute1 (temp1, VV, V); // V!=VVV
    temp1 = bdd_substitute1 (temp1, VVV, VV); // V!=VV, P!=PPP
                
    ETRWPP = bdd_smooth (temp1, PPP); // Will have no PPP; just P
    bdd_free (temp1);
    TRWP = bdd_smooth (ETRWPP, P);
    temp0 = bdd_and (TRWP, Reached, 1, 1);
    if (!bdd_equal (temp0, ETRWPP)) {
      printf ("No safe transition relation for the projected behavior of Gate %s!\n", Gate_String (gptr));
      Print_Bdd (temp0, "TRWP_Reached: ");
      Print_Bdd (ETRWPP, "ETRWPP: ");
      xor = bdd_xor (temp0, ETRWPP);
      Print_Bdd (xor, "XOR: ");
      exit (-1);
    }
    array_free (V);
    array_free (VV);
    array_free (VVV);
    free_ite (Gate_Projected (gptr), temp0, ETRWPP);
    Gate_Projected (gptr) = TRWP;

//    printf ("Projection of gate %s was Okay!\n", Gate_String (gptr));
//        Print_Bdd (TRWP, "Projection: ");

  }

  array_free (P);
  array_free (PP);
  array_free (PPP);
  bdd_free (disjunct);
  if (disjunct0)
    bdd_free (disjunct0);
  if (disjunct1)
    bdd_free (disjunct1);
  if (disjunct2)
    bdd_free (disjunct2);
  bdd_free (HiddenStable);      
  end = util_cpu_time ();
  printf ("PROJECT_TR took %d time\n", end - start);

}

/*******************************************************/
signal_ptr *Keep_Places (signal_ptr *spptr) 
{
  array_t *arr;
  int i, j;
  signal_ptr sptr;
  
  arr = array_alloc (signal_ptr, 1);
  for (i = 0, j = 0; (sptr = spptr [i]) != NULL; i++) 
    if (Signal_Proj (sptr) == 0) {
      array_insert_last (signal_ptr, arr, sptr);
      j++;
    }  

  spptr = (signal_ptr *) malloc (sizeof (signal_ptr) * (j + 1));
  for (i = 0; i < j; i++) {
    sptr = array_fetch (signal_ptr, arr, i);
    spptr[i] = sptr;
  }
  spptr[i] = NULL;
  return (spptr);   
}

/*******************************************************/
void Project_Env (gate_ptr *visible, gate_ptr *hidden)
{
  int i, j;
  gate_ptr gptr, sgptr;
  bdd_t *bdd;
  signal_ptr *spptr, sptr;
  spec_index_ptr siptr;
  input_ptr iptr;
  
  for (i = 0; (gptr = visible[i]) != NULL; i++) {
    // The following deals with pushing the Input_TRs of the affected Specs 
    spptr = (Gate_Ext_Out (gptr) != NULL) ? Gate_Ext_Out (gptr) : Gate_Int_Out (gptr);
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        sgptr = Spec_Index_Spec (siptr);
        Gate_Visited (sgptr) = 0;
      }
    }
  }

//  printf ("Pushing in Project_Env: ");
  for (i = 0; (gptr = visible[i]) != NULL; i++) {
//    Print_Bdd (Gate_Projected (gptr), Gate_String (gptr));
//    Print_Bdd (Gate_TRI_TRO (gptr), Gate_String (gptr));
    bdd = Make_Bdd_Complete (Gate_Projected (gptr), gptr);  //Oct 31!!!!!         
    Push ((rec_ptr *) &(Gate_TRI_TRO_List (gptr)), bdd, visible, 0);
    bdd_free (Gate_Projected (gptr));
    
    // The following deals with pushing the Input_TRs of the affected Specs 
    spptr = (Gate_Ext_Out (gptr) != NULL) ? Gate_Ext_Out (gptr) : Gate_Int_Out (gptr);
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        sgptr = Spec_Index_Spec (siptr);
        iptr = &(Gate_State_Inputs (sgptr)[Spec_Index_Index (siptr)]);
        Input_Pushed_TR (iptr) = Push_Bdd (Input_Pushed_TR (iptr), Input_Projected (iptr));
//        printf ("%s at %s: ", Signal_String (sptr), Gate_String (sgptr));
//        Print_Bdd (Input_Projected (iptr), "");
//        Print_Bdd (Input_TR (iptr), "");
        if (Gate_Visited (sgptr) == 0) {
          Push_Places (sgptr, Keep_Places (Gate_State_All_Places (sgptr)));
          Gate_Visited (sgptr) = 1;
        }  
      }
    }
  }
//  printf ("\n");      
}

/*******************************************************/
void Remove_Projection (gate_ptr *support)
{
  int i, j;
  gate_ptr gptr, sgptr;
  bdd_t *bdd;
  signal_ptr *spptr, sptr;
  spec_index_ptr siptr;
  input_ptr iptr;
  
  for (i = 0; (gptr = support[i]) != NULL; i++) {
    // The following deals with pushing the Input_TRs of the affected Specs 
    spptr = (Gate_Ext_Out (gptr) != NULL) ? Gate_Ext_Out (gptr) : Gate_Int_Out (gptr);
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        sgptr = Spec_Index_Spec (siptr);
        Gate_Visited (sgptr) = 0;
      }
    }
  }

//  printf ("Poping in Remove_Projection : ");
  for (i = 0; (gptr = support[i]) != NULL; i++) {
    bdd = Pop ((rec_ptr *) &(Gate_TRI_TRO_List (gptr)));
//    printf ("%s ", Gate_String (gptr));
    bdd_free (bdd);
    // The following deals with pushing the Input_TRs of the affected Specs 
    spptr = (Gate_Ext_Out (gptr) != NULL) ? Gate_Ext_Out (gptr) : Gate_Int_Out (gptr); 
    // ^^^^ this may be different than what it was when Pushing! Better use Gate_Outputs
    
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        sgptr = Spec_Index_Spec (siptr);
        iptr = &(Gate_State_Inputs (sgptr)[Spec_Index_Index (siptr)]);
        Input_Pushed_TR (iptr) = Pop_Bdd (Input_Pushed_TR (iptr));
        if (Gate_Visited (sgptr) == 0) {
          Pop_Places (sgptr);
          Gate_Visited (sgptr) = 1;
        }  
      }
    }
  }
//  printf ("\n");  
}

/*******************************************************/
gate_ptr *Check_Projection (gate_ptr *externals, gate_ptr *must_vis, subcir_ptr sub_cir) 
{
  int i, j, k, index;
  array_t *vis, *proj, *hid;
  char ch;
  gate_ptr gptr, *visible, *hidden, sgptr;
  signal_ptr *proj_sig, sptr, sptr1;
  long	start, end;
  spec_index_ptr siptr;
  input_ptr iptr;
  
  if (externals[0] == NULL && (!any_spec || (array_n (Ext_Place_Array) == 0)))
    return (NULL);    
  
  printf ("\n!!!! SAFE PROJECTION OF THE BEHAVIOR OF THE PARENT SUBCIRCUIT TO VERIFY %s !!!!\n", Subcir_String (sub_cir));
/*
  if (any_spec) {
    printf ("Sorry! Safe projection is not yet supported for circuits/specification having internal variables!\n");
    printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return (NULL); // June 17
  }  
*/  
  if (externals[0] != NULL) {
    printf ("Gates outside subcircuit %s which may be projected away from the parent's behavior: \n", Subcir_String (sub_cir));
    Print_Gates (externals, ""); // March 31: externals is not correctly set!!
    printf ("Project away the outputs of any of the above gates to verify subcircuit? (y/n) ", Subcir_String (sub_cir));
    while ((ch = tolower (getchar ())) != 'y' && ch != 'n');
    if (ch == 'n') {
      printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      return (NULL);
    }  
  }  

  vis = array_alloc (gate_ptr, 1);
  hid = array_alloc (gate_ptr, 1);
  proj = array_alloc (signal_ptr, 1);
  
  if (must_vis != NULL)
    for (i = 0; (gptr = must_vis[i]) != NULL; i++)
      if (Gate_Ext_Out (gptr) != NULL) 
        array_insert_last (gate_ptr, vis, gptr);

  if (externals [0] != NULL)
    printf ("Specify if the outputs of a gate are to be projected away (y) or not (n).\n");
  for (i = 0; (gptr = externals[i]) != NULL; i++) {
    printf ("%s? (y/n) ", Gate_String (gptr));
    while ((ch = tolower (getchar ())) != 'y' && ch != 'n');
    array_insert_last (gate_ptr, (ch == 'y') ? hid : vis, gptr);        
  }

  if (array_n (Ext_Place_Array) != 0)
    printf ("\nSpecify if the internal variables of a specification are to be projected away (y) or not (n).\n");
  for (i = 0; i < array_n (Ext_Place_Array); i++) {
    sptr = array_fetch (signal_ptr, Ext_Place_Array, i);
    printf ("%s? (y/n) ", Signal_String (sptr));
    while ((ch = tolower (getchar ())) != 'y' && ch != 'n');
    if (ch == 'y') {
      array_insert_last (signal_ptr, proj, sptr);        
      Signal_Proj (sptr) = 1;
    } else
      Signal_Proj (sptr) = 0; // Keep it as a visible place
  }
  
  for (i = 0; i < array_n (hid); i++) {
    gptr = array_fetch (gate_ptr, hid, i); // August 23 2000, hid used to be vis! 
    Gate_Transitive (gptr) = 1;
    for (j = 0; (sptr = Gate_Ext_Out (gptr)[j]) != NULL && Gate_Transitive (gptr) == 1; j++)
      for (siptr = Signal_SpecFanout (sptr); siptr != NULL && Gate_Transitive (gptr) == 1; 
           siptr = Spec_Index_Next (siptr)) {
        index = Spec_Index_Index (siptr);
        sgptr = Spec_Index_Spec (siptr);
        iptr = &(Gate_State_Inputs (sgptr)[index]);
        for (k = 0; (sptr1 = Input_Changing (iptr)[k]) != NULL; k++) {
          if (Signal_Type (sptr1) == INTERNAL)
            continue;
          if (Signal_Proj (sptr1) == 0) {
            Gate_Transitive (gptr) = 0; // This gate, although hidden, will not be participated in the
            				// transitive closure computation for TR-Projections!
            break;
          }  
        } // for k
      } // for siptr
  } // for i    
            
// Any place which is projected away is automatically removed from the TRs of all
// visible gates, and the TRs of any affected Specification. It should also be removed
// from the corresponding Input_Changing arrays. Also, it should be removed from the
// set of External_Place_Array. The Signal_Proj of all such places are set to 1.            
        
  // The caller procedure, when pushing the new information onto stacks, can check 
  // the Signal_Proj state of signals and act accordingly.    
    
  visible = (gate_ptr *) Array_to_ZArray_Free (vis);

  if (array_n (hid) != 0 || array_n (proj) != 0) {  // July 9
    hidden = (gate_ptr *) Array_to_ZArray_Free (hid);
    proj_sig = (signal_ptr *) Array_to_ZArray_Free (proj);

/* // June 29
    Print_Gates (visible, "Visible gates: ");
    Print_Gates (hidden, "Hidden Gates: ");
*/
    Update_Sibling (externals, hidden);
//  Print_Gates (externals, "Siblings after hidden removal: ");  // June 29

    Project_TR (visible, hidden, proj_sig); // July 7

//  Project_TR_Old (visible, hidden); // July 7

    Project_Env (visible, hidden); // returns the new support_set, which all need to be poped!
    free (hidden);
    free (proj_sig); // July 9
    printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return (visible);
  } else {  
    printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return (NULL);
  }  
  
}

/*******************************************************/
void Project_Places (void)
{
  int i, g;
  gate_ptr gptr, *gpptr;
  signal_ptr *vars;
  bdd_t	*bdd1, *bdd2, *bdd3;
  char 	ch;  

  return; // June 17

  printf ("Do you want to project away all external places? (y/n)\n");
  while ((ch = tolower (getchar ())) != 'y' && ch != 'n');
  if (ch == 'n') {
    Ext_Places_Projected = 0;
    return;
  }
  else
    Ext_Places_Projected = 1;  
  Print_Array (Ext_Place_Array);    
  vars = (signal_ptr *) malloc (sizeof (signal_ptr) * (array_n (Ext_Place_Array) + 1));
  for (i = 0; i < array_n (Ext_Place_Array); i++)
    vars[i] = array_fetch (signal_ptr, Ext_Place_Array, i);
  vars[i] = NULL;  
  for (g = 0, gpptr = Ext_Gates; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Ext_Out (gptr) == NULL) 
        continue;
      if (Gate_Type (gptr) == _STATE) {
        bdd_free (Gate_Ext_TR (gptr));
        Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
      }    
      bdd1 = bdd_dup (Gate_Ext_TR (gptr));
      bdd3 = Exist_Quantify_Next (bdd1, vars);
      bdd1 = bdd_dup (bdd3);
      bdd1 = Exist_Quantify (bdd1, vars);
      bdd2 = bdd_and (bdd1, R, 1, 1); // R or Reached?????????
      if (!bdd_equal (bdd2, bdd3)) {
        printf ("Cannot project away all external places: problem at the TR of %s\n", Gate_String (gptr));
        exit (-1);
      }  
      bdd_free (bdd3);
      bdd_free (bdd2);
      bdd_free (Gate_Ext_TR (gptr));
      Gate_Ext_TR (gptr) = bdd1;
    }
    if (g == 0)
      gpptr = Sibling_Gates;
  }
  free (vars);
  Print_Graph (PARTIAL);
  
}
