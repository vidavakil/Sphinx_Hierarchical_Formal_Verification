#include "type.h"
#include "sphinx.h"

#define  Gate_MultiOut(x) ((Gate_Type(x)==_ME)||(Gate_Type(x)==_FF))

int 		Times = 0;
bdd_t		*Revisited, *Init_State;

extern int	MODE, cflag, xflag, sflag, oflag, iflag;
extern bdd_t 	*From, *To;
extern bdd_manager	*Bdd_Man;
extern context_ptr	cur_cir, complex_cir; 
extern bdd_t 		*R;
extern bdd_t 		*signal_var[]; 
 
void Reset_TR_Functions (void);
int Settle_Gate (gate_ptr gptr, signal_ptr sptr);
void Settle_Siblings (void);
void Settle_Internal (void);
void Find_Enabled_External (void);
int Is_An_IO (gate_ptr gptre, gate_ptr gptri);
bdd_t *Find_Internally_Unstable (void);
void Find_Revisited (void);
int Check_Safety (void);
void Update_TR_External (void);
int Update_Cycle (bdd_t *temp);
void Free_Internal_Bdds (void);
void Print_Internals (void);
bdd_t *Operate_Gate (gate_ptr gptr, int mode, sig_type Ext_Int, int *changed, signal_ptr schanged);
bdd_t *Find_IUS (bdd_t *ius, bdd_t *tr, signal_ptr *spptr1, signal_ptr *spptr2);

/*******************************************************/
// Abstraction functions
/*******************************************************/
void Initial_State (void)
{
  int i, j, g;
  signal_ptr sptr;
  bdd_t *bdd, *temp, *var;
  gate_ptr gptr, *gpptr;
  char	str[200];
	
  str[0] = 'S';
  bdd = bdd_one (Bdd_Man);

  // Jan 18, 99: The two loops combined into one!
  for (g = 0, gpptr = Ext_Gates; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Ext_Out (gptr) == NULL) // new
        continue;
      for (j = 0; (sptr = Gate_Ext_Out (gptr)[j]) != NULL; j++) {
        temp = bdd_and (bdd, var = bdd_get_var (Bdd_Man, Signal_Order (sptr)), 
        		    1, (Signal_Init (sptr) == 1) ? 1 : 0);
        bdd_free (bdd);
        bdd = temp;
      }  
    }
    if (g == 0)
      gpptr = Sibling_Gates;
  }

  for (i = 0; i < array_n (Ext_Place_Array); i++) {
    sptr = array_fetch (signal_ptr, Ext_Place_Array, i);
    temp = bdd_and (bdd, var = bdd_get_var (Bdd_Man, Signal_Order (sptr)), 
        	    1, (Signal_Init (sptr) == 1) ? 1 : 0);
    bdd_free (bdd);
    bdd = temp;
  }  
      
  From = bdd;
  Init_State = bdd_dup (bdd);
  Revisited = bdd_zero (Bdd_Man);
  R = bdd_zero (Bdd_Man);
  To = bdd_zero (Bdd_Man);

// Jan 18, 99: The two loops combined into one!
  for (g = 0, gpptr = DFS_List; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Int_Out (gptr) == NULL)
        continue;
      temp = bdd_one (Bdd_Man);
      for (j = 0; (sptr = Gate_Int_Out (gptr)[j]) != NULL; j++) {
        bdd = bdd_and (temp, var = bdd_get_var (Bdd_Man, Signal_Order (sptr)), 
   			1, (Signal_Init (sptr) == 1) ? 1 : 0);
        bdd_free (temp);
        temp = bdd;
      }
      bdd_free (Gate_Int_IUS (gptr)); // New   	
      Gate_Int_IUS (gptr) = bdd_and (From, temp, 1, 1);
      bdd_free (temp); 
      Gate_Reached (gptr) = bdd_zero (Bdd_Man);
//      Print_Bdd (Gate_Int_IUS (gptr), Gate_String (gptr));    					
    } 
    if (g == 0)
      gpptr = Sibling_Gates;
  }
  
  for (i = 0; (gptr = list_of_specs[i]) != NULL; i++) {
    temp = bdd_one (Bdd_Man);
    for (j = 0; (sptr = Gate_State_All_Places (gptr)[j]) != NULL; j++) {    
      bdd = bdd_and (temp, var = bdd_get_var (Bdd_Man, Signal_Order (sptr)), 
 			1, (Signal_Init (sptr) == 1) ? 1 : 0);
      bdd_free (temp);
      temp = bdd;
    }
    bdd_free (Gate_Int_IUS (gptr)); // New   	
    Gate_Int_IUS (gptr) = bdd_and (From, temp, 1, 1);
    bdd_free (temp); 
    Gate_Reached (gptr) = bdd_zero (Bdd_Man);
//    Print_Bdd (Gate_Int_IUS (gptr), Gate_String (gptr));
  }
/*  printf ("Initial States found\n");*/
}

/*******************************************************/
void Reset_TR_Functions (void)
{
  int i, g;
  gate_ptr gptr, *gpptr;
  
// Jan 18, 99: The two loops combined into one!
  for (g = 0, gpptr = list_of_gates; g < 2; g++) {  
    // VERY IMPORTANT: Better free them! In such a case, be careful with TRO/TRI which were not bdd_dups!
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      bdd_free (Gate_Int_TR (gptr));
      bdd_free (Gate_Int_TRFROM (gptr));
      bdd_free (Gate_Ext_TR (gptr));
      bdd_free (Gate_Ext_TRFROM (gptr));
      Gate_Int_TR (gptr) = bdd_zero (Bdd_Man);
      Gate_Int_TRFROM (gptr) = bdd_zero (Bdd_Man);
      Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
      Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);
    }
    if (g == 0)
      gpptr = Sibling_Gates;
  }      
}  

/*******************************************************/
void Test_TR_Functions (void)
{
  int i, g;
  gate_ptr gptr, *gpptr;
  bdd_t  *temp;
  
  temp = bdd_zero (Bdd_Man);
  
// Jan 18, 99: The two loops were combined into one! 
  for (g = 0, gpptr = list_of_gates; g < 2; g++) { 
    // VERY IMPORTANT: Better free them! In such a case, be careful with TRO/TRI which were not bdd_dups!
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (!bdd_equal (Gate_Int_TR (gptr), temp) || !bdd_equal (Gate_Int_TRFROM (gptr), temp) ||
          !bdd_equal (Gate_Ext_TR (gptr), temp) || !bdd_equal (Gate_Ext_TRFROM (gptr), temp) ) {
        printf ("Non-zero Bdd TR %s\n", Gate_String (gptr));
        exit (-1);
      }    
    }
    if (g == 0)
      gpptr = Sibling_Gates;
  }    
  bdd_free (temp);
}  

/*******************************************************/
bdd_t *Include_Support_Info (gate_ptr *support, int mode)
{
  int i, j;
  gate_ptr sgptr, ogptr;
  signal_ptr sptr1;
  bdd_t *temp1, *temp3;
  spec_index_ptr siptr;
  
  // June 18: And in the internal state of any internal support gate of the Input_TR
  // and any _STATE gate in their Signal_SpecFanout
  temp1 = bdd_one (Bdd_Man);
  for (i = 0; (sgptr = support[i]) != NULL; i++) {
//    printf ("Support[%d] = %s", i, Gate_String (sgptr));
    if (Gate_Int_Out (sgptr) != NULL) {
      temp3 = bdd_and (mode ? Gate_Int_IS (sgptr) : Gate_Int_IUS (sgptr), temp1, 1, 1);
//      Print_Bdd (mode ? Gate_Int_IS (sgptr) : Gate_Int_IUS (sgptr), "Gate_Int_IUS");
      bdd_free (temp1);
      temp1 = temp3;
    }
    for (j = 0; (sptr1 = Gate_Outputs (sgptr)[j]) != NULL; j++)
      for (siptr = Signal_SpecFanout (sptr1); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        ogptr = Spec_Index_Spec (siptr);
        if (Gate_State_Int_Places (ogptr) != NULL) {
//          printf ("Spec_Support[%d] = %s", j, Gate_String (ogptr));
          temp3 = bdd_and (mode ? Gate_Int_IS (ogptr) : Gate_Int_IUS (ogptr), temp1, 1, 1);
//          Print_Bdd (mode ? Gate_Int_IS (ogptr) : Gate_Int_IUS (ogptr), "Gate_Int_IUS");
          bdd_free (temp1);
          temp1 = temp3;
        }
      }
  } // for i
  return (temp1);
}

/*******************************************************/
bdd_t *Disclude_Support_Info (gate_ptr *support, bdd_t *bdd, gate_ptr gptr, gate_ptr gptr1)
{
  int i, j;
  gate_ptr sgptr, ogptr;
  signal_ptr sptr1;
  bdd_t *temp1, *temp3;
  spec_index_ptr siptr;
  
  // June 18: And in the internal state of any internal support gate of the Input_TR
  // and any _STATE gate in their Signal_SpecFanout
  temp1 = bdd_dup (bdd);
  for (i = 0; (sgptr = support[i]) != NULL; i++) {
    if (sgptr != gptr && Gate_Int_Out (sgptr) != NULL) 
      temp1 = Exist_Quantify (temp1, Gate_Int_Out (sgptr));
    for (j = 0; (sptr1 = Gate_Outputs (sgptr)[j]) != NULL; j++)
      for (siptr = Signal_SpecFanout (sptr1); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        ogptr = Spec_Index_Spec (siptr);
        if (gptr1 != ogptr && Gate_State_Int_Places (ogptr) != NULL) 
          temp1 = Exist_Quantify (temp1, Gate_State_Int_Places (ogptr));
      }
  } // for i
  return (temp1);
}

/*******************************************************/
bdd_t *Operate_Gate (gate_ptr gptr, int mode, sig_type Ext_Int, int *changed, signal_ptr schanged) // fixed
/* 
     mode : TOTAL : Use the From set for obtaining the set of values for the inputs.
          PARTIAL : Use the Signal_IUS=Signal_TR set for internal inputs, and the
          	    From set for the external inputs to obntain their values.
     Ext_Int : Specifies the Int or Ext outputs of the gate to be evaluated		
*/ 
{

  int 		i, j;
  component_type type;
  bdd_t      	*temp0, *temp1, *temp2, *temp3, *temp4, *temp5, *bdd;
  signal_ptr 	sptr, *spptr;
  gate_ptr      gtemp;
        		      		
      if (!Gate_MultiOut (gptr))
        sptr = Gate_Outputs (gptr)[0];

      if (Gate_Env (gptr) && Gate_TRO_T (gptr)) {
        // A SPEC has TRs in Gate_Int_F & Gate_Ext_F
        // June 18
        if (mode == PARTIAL) {
  	  temp1 = Include_Support_Info (Gate_Support (gptr), 0);
  	  bdd = bdd_and (temp1, From, 1, 1);
  	  bdd_free (temp1);
	} else
	  bdd = bdd_dup (From);  

        // And with the appropriate output function of the SPEC
        if (Ext_Int == INTERNAL) // ??????????????
          temp1 = bdd_and (bdd, Gate_TRI (gptr), 1, 1);
        else 
          temp1 = bdd_and (bdd, Gate_TRO (gptr), 1, 1);
        bdd_free (bdd);  

        // Smooth away all internal inputs
        if (mode == PARTIAL) { // June 18
          temp4 = Disclude_Support_Info (Gate_Support (gptr), temp1, gptr, NULL);
          bdd_free (temp1);
          temp1 = temp4;
        }  
        return (temp1);
      }

      if (!(*changed)) {
        if (Check_Inputs_Event (gptr, mode, Ext_Int))
          *changed = 1;
        else {
          *changed = 0;  
          return (NULL);
        }  
      }
      
      type = Gate_Type (gptr);
      if (sflag && type != _OUT && type != _STATE) // NOTICE! New!!!!!
        type = _SPEC;
      switch (type) {
	case _SPEC  : {
        // A SPEC has TRs in Gate_Int_F & Gate_Ext_F
          
/*
          if (!strcmp (Gate_String (gptr), "and1"))
            Print_Bdd (From, "From of and1:");
*/
	// And all inputs of the gate
          bdd = bdd_one (Bdd_Man);
          for (j = 0, i = 0; (sptr = Input_Signal(Gate_Inputs(gptr)[j])) != NULL; j++) {
          // NOTICE!!!!! Very important: I am assuming that it is not the case that more than
          // one outputs of a multioutput gate enter a SPEC with any of them inverted         
            if (Signal_Type (sptr) == EXTERNAL || mode == TOTAL) {
              if (!i) {
	        temp0 = From;
	        i++;
	      } else
	        continue;
	    } else 
	      temp0 = Gate_Int_IUS (Signal_Parent (sptr)); // ??????	        
	    temp1 = bdd_and (bdd, temp0, 1, 1);
	    bdd_free (bdd);
	    bdd = temp1;
	  }

          // And in all internal outputs of the SPEC, since its new outputs may depend on them  
	  if (Gate_Int_Out (gptr) != NULL && mode == PARTIAL) { // April 13: GATE_COMB REMOVED BECAUSE OF CIRCULAR _SPECS
	    temp1 = bdd_and (bdd, Gate_Int_IUS (gptr), 1, 1);
	    bdd_free (bdd);
	    bdd = temp1;
	  }  

/*          
          if (!strcmp (Gate_String (gptr), "and1"))
            Print_Bdd (bdd, "IO state of and1:");
*/
	  // And with the appropriate output function of the SPEC
	  if (Ext_Int == INTERNAL) 
	    temp1 = bdd_and (bdd, Gate_Int_F (gptr), 1, 1);
	  else
	    temp1 = bdd_and (bdd, Gate_Ext_F (gptr), 1, 1);
	  bdd_free (bdd);  

/*
          if (!strcmp (Gate_String (gptr), "and1"))
            Print_Bdd (temp1, "Next state of and1:");
*/
	  // Smooth away all internal inputs
	  if (mode == PARTIAL) 
            for (j = 0; (sptr = Input_Signal(Gate_Inputs(gptr)[j])) != NULL; j++) {
              if (Signal_Type (sptr) == EXTERNAL)
                continue;
              temp1 = Exist_Quantify (temp1, Gate_Int_Out (Signal_Parent (sptr)));              
            }

/*
          if (!strcmp (Gate_String (gptr), "and1")) 
            Print_Bdd (temp1, "Return BDD for and1:");
*/

/*	// June 15 ?????????????????
          if (mode == PARTIAL && (!Gate_Comb (gptr) || (MODE == COMPLEX && Gate_Type (gptr) == _SPEC)) 
              && Gate_Int_Out (gptr) != NULL) // April 13: GATE_COMB REMOVED BECAUSE OF CIRCULAR _SPECS
            temp1 = Exist_Quantify (temp1, Gate_Int_Out (gptr));                           
*/
          return (temp1);
        }
        
        case _OUT : {
          gate_ptr ugptr, sgptr, ogptr;
	  array_t	*quantify;
	  input_ptr	iptr;

	  quantify = array_alloc (bdd_tp, 1);
          temp1 = bdd_dup ((Ext_Int == EXTERNAL || mode == TOTAL) ? From : Gate_Int_IUS (gptr));
          ugptr = Gate_Uncle (gptr);
          iptr = &(Gate_State_Inputs (ugptr)[Gate_Out_Index (gptr)]); // June 7
          if (mode == PARTIAL && Gate_State_Int_Places (ugptr) != NULL) {
            temp2 = bdd_and (temp1, Gate_Int_IUS (ugptr), 1, 1); // Thus the returned value will contain *all* internal places!
            bdd_free (temp1);
            temp1 = temp2;
          } // temp1 = Rp
          // Gate_TR has the next vars of only the places which may change for 
          // a transition of this signal. But it has the current value of all the places!

	  if (mode == PARTIAL) {
	    temp2 = Include_Support_Info (Input_Support (iptr), 0);
	    temp3 = bdd_and (temp1, temp2, 1, 1);
	    bdd_free (temp1);
	    bdd_free (temp2);
	    temp1 = temp3;
	  }  
          
          temp2 = bdd_and (temp1, Input_TR (iptr), 1, 1);
          bdd_free (temp1);
/*
          Print_Bdd (Input_TR (iptr), Gate_String (gptr));
          getchar (); 
*/
          temp5 = bdd_and (temp2, (Ext_Int == INTERNAL) ? Gate_Int_Ch (gptr) : Gate_Ext_Ch (gptr), 1, 1);
          bdd_free (temp2);
          temp5 = Exist_Quantify_Next (temp5, Input_Changing (iptr)); 
          if (mode == PARTIAL) {
            temp5 = Exist_Quantify (temp5, Gate_State_Int_Places (ugptr));
            temp4 = Disclude_Support_Info (Input_Support (iptr), temp5, gptr, NULL);
            bdd_free (temp5);
            temp5 = temp4;
          }  
//          Print_Bdd (temp5, "OpGate:");  
	  return (temp5); // temp5 only contains the transitions of the outputs of this gate and no internal places
        }

        default :
          printf ("Other gates to be implemented");
          exit (-1);
          break;
      }
}

/*******************************************************/
int Settle_Gate (gate_ptr gptr, signal_ptr sptr)
{
  int changed, i;
  bdd_t  *s, *us;
  bdd_t *temp1, *temp2;
  input_ptr iptr;
  int index;
  gate_ptr gptro;
  signal_ptr sptr1;
  spec_index_ptr siptr;
  
  us = Gate_Int_IUS (gptr);
  if (bdd_equal (us, bdd_zero (Bdd_Man))) {
    printf ("Gate_Int_IUS of %s was empty in Settle_Gate\n", Gate_String (gptr));
    getchar ();
  }
  if (cflag)
    changed = 0;
  else
    changed = 1;  
  s = Operate_Gate (gptr, PARTIAL, INTERNAL, &changed, sptr);   // Contains only the output transitions and no place transitions

  temp1 = bdd_and (s, Gate_Int_Ch (gptr), 1, 1);
  bdd_free (s);
  s = temp1;
  if (!changed) {
    Gate_Changed (gptr) = 0;
    return (-1);
  }
  
  if (bdd_equal (s, bdd_zero (Bdd_Man))) {
    Gate_Changed (gptr) = 0; 
    bdd_free (s);
    return (1);
  }  
  
  for (i = 0; (sptr1 = Gate_Int_Out (gptr)[i]) != NULL; i++) {
    for (siptr = Signal_SpecFanout (sptr1); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
      gptro = Spec_Index_Spec (siptr);
      if (Gate_State_Int_Places (gptro) == NULL)
        continue;
      index = Spec_Index_Index (siptr);
      iptr = &(Gate_State_Inputs (gptro)[index]);
      temp2 = bdd_and (Gate_Int_IUS (gptro), s, 1, 1);
      temp1 = bdd_and (temp2, Input_TR (iptr), 1, 1); // T1ij
      bdd_free (temp2);
      temp2 = Find_IUS (Gate_Int_IUS (gptro), temp1, Input_Changing (iptr), Gate_Int_Out (gptr));
      bdd_free (temp1);
      bdd_free (Gate_Int_IUS (gptro));
      Gate_Int_IUS (gptro) = temp2;
      Gate_Changed (gptro) = 1;
    }
  }    
  temp1 = Find_IUS (Gate_Int_IUS (gptr), s, Gate_Int_Out (gptr), NULL);
  bdd_free (s);
  bdd_free (Gate_Int_IUS (gptr));
  Gate_Int_IUS (gptr) = temp1;
  Gate_Changed (gptr) = 1;

  return (0);
}

/*******************************************************/
void Settle_Siblings (void)
{
  int i, settled, flag;
  gate_ptr   gptr;
    
// I am assuming that ony the gates with TRI_TRO have interleaving semantics!
// With this, there is no need to make TRs complete!
// QUESTION: What happens if two TRI_TRO signals are enabled, but firing one of them
// messes up with the computation of the other?! It seems that because of this we MAY
// need to make TRs complete, not sure though, Jan 18, 99

  settled = 0;
  while (settled == 0) {
    settled = 1;
    for (i = 0; (gptr = Sibling_Gates[i]) != NULL; i++) { 
      if (Gate_Int_Out (gptr) == NULL)
        continue;
      flag = Settle_Gate (gptr, NULL);
      if (flag == -1)
        continue;
      if (flag == 0) { 
        settled = 0;
      }          
    } /* for */
  }  
}

/*******************************************************/
void Settle_Internal (void) /* fixed */
{

// This function still needs some work to fit the case of having circuit elements with
// internal state variables. In such a case, we need to settle gates with state
// variables first, not gates with TRI_TROs!!!

  int i, g, settled, flag;
  gate_ptr   gptr, *gpptr;
    
  Settle_Siblings ();
  settled = 0;
  while (settled == 0) {
    settled = 1;
    for (i = 0; (gptr = DFS_List[i]) != NULL; i++) {   
    // Feb 5: DFS_List should not include any _STATE gates. 
    // _STATE gates are operated by calling Settle_Following_State whenever 
    // an input of them changes, and by Operate_Gate whenever an output of them changes. 
    
      flag = Settle_Gate (gptr, NULL);
      if (flag == -1)
        continue;
      if (flag == 0) { 
        settled = 0;
        Settle_Siblings (); // this was called even before Jan 18, for !flag
      }
    } /* for */
  } /* while */
  
  for (g = 0, gpptr = DFS_List; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Int_Out (gptr) == NULL)
        continue;
      bdd_free (Gate_Int_IS (gptr));
      Gate_Int_IS (gptr) = bdd_dup (Gate_Int_IUS (gptr)); // notice
    }
    if (g == 0)
      gpptr = Sibling_Gates;
  }

  for (i = 0; (gptr = list_of_specs[i]) != NULL; i++) {
    if (Gate_State_Int_Places (gptr) == NULL)
      continue;
    bdd_free (Gate_Int_IS (gptr));
    Gate_Int_IS (gptr) = bdd_dup (Gate_Int_IUS (gptr)); // notice
  }
  
  Times++; 
  printf ("t");
}	     
	
/*******************************************************/
/*
int Settle_Non_Deterministic (void)
{
  int i, flag;
  gate_ptr gptr;

      for (i = 0; (gptr = DFS_List[i]) != NULL; i++) {   
      // Feb 5: DFS_List should not include any _STATE gates. 
      // _STATE gates are operated by calling Settle_Following_State whenever 
      // an input of them changes, and by Operate_Gate whenever an output of them changes. 
    
        if (Gate_Type (gptr) == _STATE)
          continue; // Feb 8         
        if (Gate_NDT (gptr)) // Non-Deterministic Gate
          continue;           
        flag = Semi_Settle_Gate (gptr, NULL); 	// fills a local bdd without changin Int_IUS.
        					// by later comparing it to Int_IUS 
        					// we can decide if the following states or Sibling_Gates
        					// need to be updated
        if (flag == -1)
          continue;
        if (flag == 0) { 
          settled = 0;
          Settle_Siblings (); // this was called even before Jan 18, for !flag
        }
      } // for 
  
}
*/

/*******************************************************/
/*
void Settle_Internal_New (void) 
{

// This function still needs some work to fit the case of having circuit elements with
// internal state variables. In such a case, we need to settle gates with state
// variables first, not gates with TRI_TROs!!!

  int i, g, settled, flag;
  gate_ptr   gptr, *gpptr;
    
  Settle_Siblings ();
  ND_settled = 0;
  while (ND_settled == 0)
    settled = 0;
    while (settled == 0) {
      settled = 1;
      for (i = 0; (gptr = DFS_List[i]) != NULL; i++) {   
      // Feb 5: DFS_List should not include any _STATE gates. 
      // _STATE gates are operated by calling Settle_Following_State whenever 
      // an input of them changes, and by Operate_Gate whenever an output of them changes. 
    
        if (Gate_Type (gptr) == _STATE)
          continue; // Feb 8
        if (Gate_NDT (gptr)) // Non-Deterministic Gate
          continue;           

// Jan 18, 99: used Settle_Gate and Settle_Following_State 
        flag = Settle_Gate (gptr, NULL);
        if (flag == -1)
          continue;
        if (flag == 0) { 
          settled = 0;
          Settle_Siblings (); // this was called even before Jan 18, for !flag
        }
      } // for 
    } // while 
    ND_settled = Settle_Non_Deterministic ();
  }  
  
// Jan 18, 99: Two loops combined into one
  for (g = 0, gpptr = DFS_List; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Int_Out (gptr) == NULL)
        continue;
      bdd_free (Gate_Int_IS (gptr));
      Gate_Int_IS (gptr) = bdd_dup (Gate_Int_IUS (gptr)); // notice
    }
    if (g == 0)
      gpptr = Sibling_Gates;
  } 
//
  Times++; 
//  printf ("Internally Settled\n");
}	     
*/
	
/*******************************************************/
void Find_Enabled (gate_ptr gptr, int mode, sig_type Int_Ext/**/, signal_ptr schanged /**/) /* fixed */
{
  int 	 i, changed = 1, cmode;  // NOTICE change = 1;
  bdd_t  *s;
  signal_ptr sptr;
  gate_ptr cgptr;
    
  s = Operate_Gate (gptr, mode, Int_Ext, &changed, /* NULL */ schanged);
/*
  if (!strcmp (Gate_String (gptr), "and1")) 
    Print_Bdd (s, "and1 Operated: ");  
*/

/* with change initially set to 1, it will stay at 1!
  if (!changed && mode != TOTAL) {
    Gate_Changed (gptr) = 0;
    return;
  }
*/
/*
  Print_Bdd (s, Gate_String (gptr));
  getchar ();
*/
  if (Int_Ext == EXTERNAL) {
    Gate_Changed (gptr) = 1;
    bdd_free (Gate_Ext_TRFROM (gptr));
    Gate_Ext_TRFROM (gptr) = bdd_and (s, Gate_Ext_Ch (gptr), 1, 1);
/*
    if (bdd_equal (Gate_Ext_Ch (gptr), bdd_zero (Bdd_Man))) 
      printf ("Zero Ext_Ch!!!!!!!!!!@@@@@@@\n");
    if (!strcmp (Gate_String (gptr), "and1")) 
      Print_Bdd (Gate_Ext_TRFROM (gptr), "TR_FROM of and1: ");  
*/
  } else {
    Gate_Changed (gptr) = 1; 
    bdd_free (Gate_Int_TRFROM (gptr));
    Gate_Int_TRFROM (gptr) = bdd_and (s, Gate_Int_Ch (gptr), 1, 1);
  }
  bdd_free (s);
    
/* redundant
  if (mode == TOTAL)
    Gate_Changed (gptr) = 1;  
*/
}

/*******************************************************/
void Find_Enabled_External (void) /* fixed */
{
  int i, g;
  gate_ptr gptr, *gpptr;

// Jan 18, 99: The two loops combined into one!
  for (g = 0, gpptr = Ext_Gates; g < 2; g++) { 
    // Feb 5: Ext_Gates should not include any _STATE gates  Feb 25: Why???????
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Ext_Out (gptr) == NULL) // new
        continue;
      Find_Enabled (gptr, PARTIAL, EXTERNAL, NULL);
      Gate_Changed (gptr) = 1; // New; adopted from the newly commented code in Find_Internally_Unstable  
    } // for 
    if (g == 0)
      gpptr = Sibling_Gates;
  }    
}

/*******************************************************/
bdd_t *Find_IUS (bdd_t *ius, bdd_t *tr, signal_ptr *spptr1, signal_ptr *spptr2)
{
  bdd_t *bdd, *temp, *temp1;
  
  temp = bdd_dup (tr);
  temp = Exist_Quantify_Next (temp, spptr1);
  temp = Exist_Quantify_Next (temp, spptr2);
  bdd = bdd_and (ius, temp, 1, 0);
  bdd_free (temp);
  temp = bdd_dup (tr);
  temp = Next_to_Current (temp, spptr1);
  temp = Next_to_Current (temp, spptr2);
  temp1 = bdd_or (bdd, temp, 1, 1);
  bdd_free (temp);
  bdd_free (bdd);
  return (temp1);
  
}

/*******************************************************/
int Is_An_IO (gate_ptr gptre, gate_ptr gptri)
{
  int i;
  signal_ptr sptr;
  spec_index_ptr siptr;
  
  for (i = 0; (sptr = Gate_Ext_Out (gptre)[i]) != NULL; i++)
    for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr))
      if (Spec_Index_Spec (siptr) == gptri)
        return (1);
  return (0);       
}

/*******************************************************/
bdd_t *Find_Internally_Unstable (void) /* fixed */
{
// This function finds the possible unstable states of all internal signals
// following any external signal transition. Also it finds the set of all 
// reachable external states due to such external signal transitions.
// The tricky part of this function include handling of internal states (places)
// of internal/external SPECifications due to an external I/O transition.

  int i, j, g, gg, index;
  bdd_t *temp0, *temp1, *temp2, *bdd, *temp3, *temp4;
  gate_ptr gptri, gptre, *gpptr, *ggpptr, gptro;
  signal_ptr sptr, *spptr;
  array_t	*s_array;
  spec_index_ptr siptr;
  bdd_t		*TRi, *T1i, *T2ij, *T3ij, *T4ij;
  input_ptr iptr;
  
// The transitions of external gates should be mirrored in the Int_IUS of internal gates.
// Here, the Int_IUS of internal gates are reset. 
  for (g = 0, gpptr = DFS_List; g < 2; g++) {
    for (i = 0; (gptri = gpptr[i]) != NULL; i++) { 
      if (Gate_Int_Out (gptri) == NULL)  
        continue;     
      bdd_free (Gate_Int_IUS (gptri));
      Gate_Int_IUS (gptri) = bdd_zero (Bdd_Man);
    }
    if (g == 0)
      gpptr = Sibling_Gates;
  }    

  for (i = 0; (gptri = list_of_specs[i]) != NULL; i++) {
    if (Gate_State_Int_Places (gptri) != NULL) {
      bdd_free (Gate_Int_IUS (gptri));
      Gate_Int_IUS (gptri) = bdd_zero (Bdd_Man);
    }
  }  
    
  bdd = bdd_zero (Bdd_Man); 	// depository of the reachable External states
  spptr = NULL;			// for each external gate gptre, spptr will contain 
  				// the list of all affected places in fanout _STATE gates gptro
  for (gg = 0, ggpptr = Ext_Gates; gg < 2; gg++) {
    for (j = 0; (gptre = ggpptr[j]) != NULL; j++) {
      if (Gate_Ext_Out (gptre) == NULL) 
        continue;

      if (bdd_equal (Gate_Ext_TRFROM (gptre), bdd_zero (Bdd_Man))) 
        continue;			// If an external gate has no transition, it won't affect Int_IUS
        				// of internal gates, or the reachable external states.
      TRi = Gate_Ext_TRFROM (gptre); // Should only have the next state of this signal and no places
      T1i = bdd_dup (TRi);
      s_array = array_alloc (signal_ptr, 1);	// the array version of spptr (see above!)
      for (i = 0; (sptr = Gate_Ext_Out (gptre)[i]) != NULL; i++) {
        for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
          gptro = Spec_Index_Spec (siptr);
          index = Spec_Index_Index (siptr);
          iptr = &(Gate_State_Inputs (gptro)[index]);
          Append_ZArray_to_Array (s_array, Input_Changing (iptr)); // May 10: internal places better be excluded  
          temp1 = (Gate_State_Int_Places (gptro) != NULL) ? Gate_Int_IS (gptro) : bdd_one (Bdd_Man);

/*
          printf ("New Input_Ext_TR %s at %s ", Signal_String (sptr), Gate_String (gptro));
          Print_Bdd (temp1, "temp1");
*/
          
          // July 13
          temp3 = Include_Support_Info (Input_Support (iptr), 1);
//          Print_Bdd (temp3, "temp3");
          temp4 = bdd_and (temp3, temp1, 1, 1);
          bdd_free (temp3);
          temp1 = temp4;
//          Print_Bdd (temp1, "and");
          

          temp2 = bdd_and (temp1, TRi, 1, 1);
          temp4 = bdd_and (temp2, Input_TR (iptr), 1, 1); // T1ij

                   
          // July 13
//          Print_Bdd (temp4, "before disclusion : ");
          temp3 = Disclude_Support_Info (Input_Support (iptr), temp4, gptre, gptro);
          bdd_free (temp4);
          temp4 = temp3;
//          Print_Bdd (temp4, "after disclusion : ");


          Input_Dummy (iptr) = temp4; 
          bdd_free (temp2);
          T2ij = bdd_dup (Input_Dummy (iptr)); 
          T2ij = Exist_Quantify (T2ij, Gate_State_Int_Places (gptro));
          T2ij = Exist_Quantify_Next (T2ij, Gate_State_Int_Places (gptro));          
          temp2 = bdd_and (T1i, T2ij, 1, 1);
          bdd_free (T1i);
          T1i = temp2;
          temp2 = bdd_or (Input_Ext_TR (iptr), T2ij, 1, 1);
          bdd_free (Input_Ext_TR (iptr));
          Input_Ext_TR (iptr) = temp2;
//          Print_Bdd (T2ij, "Finally: ");
          bdd_free (T2ij);
        }
      }    
/*
      temp2 = bdd_or (Gate_Ext_TR (gptre), T1i, 1, 1);
      bdd_free (Gate_Ext_TR (gptre));
      Gate_Ext_TR (gptre) = temp2;
*/

/*  // Updating of Ext_TRs will be performed in Update_TR_External  // June 16: commented
      bdd_free (Gate_Ext_TRFROM (gptre));
      Gate_Ext_TRFROM (gptre) = bdd_dup (T1i);
*/      
      
      spptr = (signal_ptr *) Array_to_ZArray (s_array);
      array_free (s_array);
      
      for (i = 0; (sptr = Gate_Ext_Out (gptre)[i]) != NULL; i++) {
        for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
          gptro = Spec_Index_Spec (siptr);
          index = Spec_Index_Index (siptr);
          iptr = &(Gate_State_Inputs (gptro)[index]);
          if (Gate_State_Int_Places (gptro) == NULL) {
            bdd_free (Input_Dummy (iptr));
            continue;
          } 
          T3ij = bdd_and (T1i, Input_Dummy (iptr), 1, 1);
          T4ij = Find_IUS (Gate_Int_IS (gptro), T3ij, spptr, Gate_Ext_Out (gptre));
          bdd_free (T3ij);
          temp2 = bdd_or (Gate_Int_IUS (gptro), T4ij, 1, 1);
          bdd_free (T4ij);
          bdd_free (Gate_Int_IUS (gptro));
          Gate_Int_IUS (gptro) = temp2;
          bdd_free (Input_Dummy (iptr));
/*
          printf ("Gate_Int_IUS (%s) : ", Gate_String (gptro));
          Print_Bdd (Gate_Int_IUS (gptro), "");
*/
        }
      }
      // So far, all the input/output _STATE gates of this signal have their Int_IUS and TRs updated.
      // Now, we have to update the Int_IUS of all other hidden gates.

      // Now that we have the transitions of an external signal with all involved place transitions 
      // in hand, we will find the new internally unstable state of all internal gates.
      for (g = 0, gpptr = DFS_List; g < 2; g++) {
        for (i = 0; (gptri = gpptr[i]) != NULL; i++) {
          if (Gate_Int_Out (gptri) == NULL)  
            continue;     

          temp0 = bdd_and (Gate_Int_IS (gptri), T1i, 1, 1);
          temp1 = Find_IUS (Gate_Int_IS (gptri), temp0, spptr, Gate_Ext_Out (gptre));
          bdd_free (temp0);
          temp2 = bdd_or (Gate_Int_IUS (gptri), temp1, 1, 1);
          bdd_free (Gate_Int_IUS (gptri));
          bdd_free (temp1);
	  Gate_Int_IUS (gptri) = temp2;
/*
          printf ("Gate_Int_IUS (%s) : ", Gate_String (gptri));
          Print_Bdd (Gate_Int_IUS (gptri), "");
*/
        } // for
        if (g == 0)
          gpptr = Sibling_Gates;
      } // for

      for (i = 0; (gptri = list_of_specs[i]) != NULL; i++) {
        if (Gate_State_Int_Places (gptri) == NULL || Is_An_IO (gptre, gptri))  
          continue;     

        temp0 = bdd_and (Gate_Int_IS (gptri), T1i, 1, 1);
        temp1 = Find_IUS (Gate_Int_IS (gptri), temp0, spptr, Gate_Ext_Out (gptre));
        bdd_free (temp0);
        temp2 = bdd_or (Gate_Int_IUS (gptri), temp1, 1, 1);
        bdd_free (Gate_Int_IUS (gptri));
        bdd_free (temp1);
        Gate_Int_IUS (gptri) = temp2;
/*
        printf ("Gate_Int_IUS (%s) : ", Gate_String (gptri));
        Print_Bdd (Gate_Int_IUS (gptri), "");
*/
      } // for

      T1i = Next_to_Current (T1i, spptr);
      T1i = Next_to_Current (T1i, Gate_Ext_Out (gptre));
      temp1 = bdd_or (bdd, T1i, 1, 1);
      bdd_free (bdd);
      bdd = temp1;
      bdd_free (T1i);
      free (spptr);    
      
    } // for
    if (gg == 0)
      ggpptr = Sibling_Gates;
  } // for

//  Print_Bdd (bdd, "New_From ");
  return (bdd); // the set of all reachable next external states
}

/*******************************************************/
void Find_Revisited (void)
{
  bdd_free (Revisited);
  Revisited = bdd_and (R, From, 1, 1);
}

/*******************************************************/
int Check_Safety (void) /* fixed */
{
  int i, g;
  bdd_t *bdd1, *bdd2, *bdd0, *temp1, *temp2;
  gate_ptr gptr, *gpptr;
  
  // Checking for USV
  // Jan 18, 99: The two loops combined into one!

  for (g = 0, gpptr = DFS_List; g < 3; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {      
      if (Gate_Type (gptr) != _STATE && Gate_Int_Out (gptr) == NULL)
        continue;
      if (Gate_Type (gptr) == _STATE && Gate_State_Int_Places (gptr) == NULL)
        continue;
      bdd1 = bdd_and (Gate_Reached (gptr), Revisited, 1, 1);
      bdd2 = bdd_and (Gate_Int_IS (gptr), Revisited, 1, 1);
      if (!bdd_equal (bdd_zero (Bdd_Man), bdd0 = bdd_xor (bdd1, bdd2)) /*&&
          bdd_equal (bdd_zero (Bdd_Man), bdd_and (bdd0, Init_State, 1, 1))*/ ) {
//      if (!bdd_equal (bdd1, bdd2)) {
        printf ("%s ", Gate_String (gptr));
        Print_Bdd (bdd0, "XOR USV");

        Print_Bdd (bdd1, "Reached");
        Print_Bdd (bdd2, "Anded Int_IS");
        Print_Bdd (Gate_Int_IS (gptr), "Int_IS");
        Print_Bdd (Revisited, "Revisited");

        bdd_free (bdd0);
        bdd_free (bdd1);
        bdd_free (bdd2);
        break;
      }
      bdd_free (bdd0);
      bdd_free (bdd1);
      bdd_free (bdd2);
    }
    if (gptr != NULL) // unsafe?!! Jan 18, 99
      return (0);
    if (g == 0)
      gpptr = Sibling_Gates;
    if (g == 1)
      gpptr = list_of_specs;
  }    

  if (gptr == NULL) // Was if (i == DFS_Count)
    return (1);  // safe
  else
    return (0);  // unsafe 

/* 
  // Checking for unique enabledness; incomplete and almost wrong, since 
  // the Gate_Ext_TRFROM of external gates have both Pv and Pv' in them!
  // Jan 18, 99: The two loops combined into one!
  for (g = 0, gpptr = Ext_Gates; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {      
      if (Gate_Ext_Out (gptr) == NULL)
        continue;     
//      if (Gate_Type (gptr) == _STATE)
//        continue;  
      bdd1 = bdd_and (Gate_Ext_TR (gptr), Revisited, 1, 1);

      if (Gate_Type (gptr) == _OUT && Gate_Ext_Out (Gate_Uncle (gptr)) != NULL) {
        bdd1 = Exist_Quantify (bdd1, Gate_Ext_Out (Gate_Uncle (gptr)));
        bdd1 = Exist_Quantify_Next (bdd1, Gate_Ext_Out (Gate_Uncle (gptr)));
      }  

      bdd2 = bdd_and (Gate_Ext_TRFROM (gptr), Revisited, 1, 1);

      if (Gate_Type (gptr) == _OUT && Gate_Ext_Out (Gate_Uncle (gptr)) != NULL) {
        bdd2 = Exist_Quantify (bdd2, Gate_Ext_Out (Gate_Uncle (gptr)));
        bdd2 = Exist_Quantify_Next (bdd2, Gate_Ext_Out (Gate_Uncle (gptr)));
      }  

      bdd0 = bdd_xor (bdd1, bdd2);

//      if (Gate_Type (gptr) == _OUT && Gate_Ext_Out (Gate_Uncle (gptr)) != NULL) {
//        bdd0 = Exist_Quantify (bdd0, Gate_Ext_Out (Gate_Uncle (gptr)));
//        bdd0 = Exist_Quantify_Next (bdd0, Gate_Ext_Out (Gate_Uncle (gptr)));
//      }  

      if (!bdd_equal (bdd_zero (Bdd_Man), bdd0) // && 
//          bdd_equal (bdd_zero (Bdd_Man), bdd_and (bdd0, Init_State, 1, 1))  
         ) {
        printf ("%s ", Gate_String (gptr));
        Print_Bdd (bdd0, "XOR UE");
	
	bdd0 = Exist_Quantify_Next (bdd0, Gate_Outputs (gptr));
	temp1 = bdd_and (bdd0, bdd1, 1, 1);
	temp2 = bdd_and (bdd0, bdd2, 1, 1);
	bdd_free (bdd1);
	bdd_free (bdd2);
	bdd1 = temp1;
	bdd2 = temp2;
        Print_Bdd (bdd1, "TR");
        Print_Bdd (bdd2, "TR_FROM");
//        Print_Bdd (Revisited, "Revisited");
        
        if (Gate_Type (gptr) == _OUT && Gate_Int_Out (Gate_Uncle (gptr)) != NULL) {
	  temp1 = bdd_and (bdd1, Gate_Reached (Gate_Uncle (gptr)), 1, 1);
	  Print_Bdd (temp1, "TR_Reached");
	  temp2 = bdd_and (bdd2, Gate_Int_IS (Gate_Uncle (gptr)), 1, 1);
	  Print_Bdd (temp2, "TRFROM_Reached");
	  bdd_free (temp1);
	  bdd_free (temp2);
        }

        bdd_free (bdd0);
        bdd_free (bdd1);
        bdd_free (bdd2);
        break;
      }
      bdd_free (bdd0);
      bdd_free (bdd1);
      bdd_free (bdd2);
    }
    if (gptr != NULL)
      return (0); // unsafe!
    if (g == 0)
      gpptr = Sibling_Gates;
  }    
*/
  if (gptr == NULL)
    return (1);  // safe 
  else
    return (0);  // unsafe 
}

/*******************************************************/
void Update_TR_External (void) /* fixed */
{
  int i, g;
  gate_ptr gptr, *gpptr;
  bdd_t *bdd1;
  
// Jan 18, 99: The two loops combined into one

  for (g = 0, gpptr = Ext_Gates; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Ext_Out (gptr) == NULL) // Nov. 1st, for Sibling_Gates
        continue;
          
      bdd1 = bdd_or (Gate_Ext_TR (gptr), Gate_Ext_TRFROM (gptr), 1, 1);
      
////    Gate_Changed (gptr) = bdd_equal (bdd1, Gate_Ext_TR (gptr)) ? 1 : 0;  // NOTICE ??????
      bdd_free (Gate_Ext_TR (gptr));
      Gate_Ext_TR (gptr) = bdd1;
/*
      printf ("Gate_Ext_TR (%s) :", Gate_String (gptr));
      Print_Bdd (bdd1, "");
*/
    }
    if (g == 0)
      gpptr = Sibling_Gates;
  }    
}

/*******************************************************/
int Update_Cycle (bdd_t *temp) /* fixed */
{ /* NOTICE! Very tricky function! It assumes that the IUS of each signal/gate contains
     all possible newly reached states, once the corresponding variables of the gate
     are removed from the BDD */
     
  int g, i;
  bdd_t *bdd1, *bdd2, *bdd;
  gate_ptr gptr, *gpptr;  
  
  if (bdd_equal (From, Revisited)) {
    // May 13
    for (g = 0, gpptr = DFS_List; g < 3; g++) {
      for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
        if (Gate_Type (gptr) != _STATE && Gate_Int_Out (gptr) == NULL)
          continue;
        if (Gate_Type (gptr) == _STATE && Gate_State_Int_Places (gptr) == NULL)
          continue;
        bdd1 = bdd_and (Gate_Reached (gptr), Revisited, 1, 1);
        bdd2 = bdd_and (Gate_Int_IS (gptr), Revisited, 1, 1);        
        if (!bdd_equal (bdd1, bdd2)) {
          bdd_free (bdd1);
          bdd_free (bdd2);
          printf ("Semi-Fixedpoint\n");
          break;
        }
        bdd_free (bdd1);  
        bdd_free (bdd2);
      }
      if (gptr != NULL)
        break;
      if (g == 0)
        gpptr = Sibling_Gates;
      if (g == 1)
        gpptr = list_of_specs;  
    }
    if (gptr == NULL) {
      free_ite (From, Revisited, To);
      return (1); // Fixed point reached! 
    }  
  }

// Jan 18, 99: The two loops combined into one
  for (g = 0, gpptr = DFS_List; g < 3; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Type (gptr) != _STATE && Gate_Int_Out (gptr) == NULL)
        continue;
      if (Gate_Type (gptr) == _STATE && Gate_State_Int_Places (gptr) == NULL)
        continue;
      bdd = bdd_or (Gate_Reached (gptr), Gate_Int_IS (gptr), 1, 1);
      bdd_free (Gate_Reached (gptr));
      Gate_Reached (gptr) = bdd; 
    }
    if (g == 0)
      gpptr = Sibling_Gates;
    if (g == 1)
      gpptr = list_of_specs;
  }    
  bdd_free (To);
  To = temp;
  bdd = bdd_or (R, From, 1, 1);
  bdd_free (R);
  R = bdd;
  bdd_free (From);
  From = bdd_dup (To); // notice 
/*  
  Print_Bdd (From, "Updated From");
  getchar ();
*/  
  Update_TR_External ();
  // Fix Point condition 
  return (0);
}

/*******************************************************/
void Free_Internal_Bdds (void)
{
  int i, g;
  gate_ptr gptr, *gpptr;

// Jan 18, 99: tried to combine the two loops into one, 
// but they were not exactly the same. WHY?

  for (g = 0, gpptr = DFS_List; g < 3; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Type (gptr) != _STATE && Gate_Int_Out (gptr) == NULL)
        continue;
      if (Gate_Type (gptr) == _STATE && Gate_State_Int_Places (gptr) == NULL)
        continue;
      // Jan 18, 99: How come Sibling_Gates does not have the following line?
      bdd_free (Gate_Int_IUS (gptr)); 
      Gate_Int_IUS (gptr) = bdd_zero (Bdd_Man);
      bdd_free (Gate_Int_IS (gptr));
      Gate_Int_IS (gptr) = bdd_zero (Bdd_Man);
      bdd_free (Gate_Reached (gptr));
      Gate_Reached (gptr) = bdd_zero (Bdd_Man);
    }
    if (g == 0)
      gpptr = Sibling_Gates;
    if (g == 1)
      gpptr = list_of_specs;
  }      
}


/*******************************************************/
void Print_Internals (void)
{
  int i, g;
  gate_ptr *gpptr, gptr;
  
  if (!iflag)
    return;
  printf ("Printing internal status:\n");  
  for (g = 0, gpptr = DFS_List; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      printf ("Gate %s ", Gate_String (gptr));
      if (Gate_Int_Out (gptr)) {
        Print_Bdd (Gate_Int_IS (gptr), "BDD");
      }
      else printf ("\n");
    }
    if (g == 0)
      gpptr = Sibling_Gates;
  }
}

/*******************************************************/
int Find_Abstract (void)
{
  int i;
  context_ptr temp_cir;
  bdd_t	*new_from;
  
  if (xflag) {
    temp_cir = cur_cir;
    cur_cir = complex_cir;
    MODE = COMPLEX;
  }  
  printf ("\n@@@@ FINDING THE SAFE ABSTRACTION OF THE BEHAVIOR OF SUBCIRCUIT %s! @@@@\n", Subcir_Path);
//  Test_TR_Functions ();
  Initial_State ();
//  Print_Bdd (From, "From");
  if (oflag) {
    printf ("Encoded initial state of the state graph: ");
    Print_A_State (From, PARTIAL);
  }  
  printf ("Sett");
  while (!bdd_equal (From, bdd_zero (Bdd_Man))) { // Feb 26
    Settle_Internal ();
    Find_Enabled_External ();
    new_from = Find_Internally_Unstable ();
    Find_Revisited ();
    if (!Check_Safety ()) {
      printf ("A safe abstraction does not exist for subcircuit %s!\n", Subcir_Path); // NOTICE! restore the flat circuit!
      exit (-1);
    }  
    if (Update_Cycle (new_from))
      break;
  }
  printf ("led!\n");
  printf ("Safe Abstraction exists, after %d settlements!\n", Times);
//  Print_Bdd (R, "R :");
  printf ("The BDD size of the Reachable states = %d\n", bdd_size (R));

  Print_Internals ();
  Print_Graph (PARTIAL);
  Print_Circuit_Graph (TOTAL); // was PARTIAL!!

//  bdd_free (R);
// At this point only the BDDs of the Ext_Out are needed
  Free_Internal_Bdds ();
//  bdd_print_stats (Bdd_Man, stdout);
  if (xflag) {
    cur_cir = temp_cir;
    MODE = FLAT;
  }
  printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
  return (1);  
}

