#include "type.h"
#include "sphinx.h"

extern gate_ptr	gate_list, spec_list;
extern bdd_manager	*Bdd_Man;
extern int MODE;
extern context_ptr	cur_cir; 
extern bdd_t 		*signal_var[]; 

void Make_Parent (signal_ptr spptr[], gate_ptr gptr); 
void Create_Input_Index (gate_ptr gptr);
void Enter_State (gate_ptr gptr, signal_ptr *spptr);
int Check_Inputs_Event (gate_ptr gptr, int mode, int Ext_Int);

/*******************************************************/
// Gate functions
/*******************************************************/
/*******************************************************/
void Make_Parent (signal_ptr spptr[], gate_ptr gptr) 
{
  int i;
  
  if (spptr == NULL)
    return;
  for (i = 0; spptr[i] != NULL; i++) {
//    printf ("parent of %s is %s\n", Signal_String(spptr[i]), Gate_String (gptr));
    Signal_Parent (spptr[i]) = gptr;
  }
} 

/*******************************************************/
gate_ptr *Gate_Chain_to_ZArray (gate_ptr gptr, int num)
{
// Generates a ZArray of gates from a chain of gates
  gate_ptr *gptrl;
  int i;
  
  gptrl = (gate_ptr *) malloc (sizeof (gate_ptr) * (num + 1));
  for (i = 0; i < num && gptr != NULL; gptr = Gate_Next (gptr), i++) 
    gptrl[i] = gptr;
  gptrl[i] = NULL;
  return (gptrl);
    
}

/*******************************************************/
void Print_Gates (gate_ptr *gpptr, char *str)
{
  int i;
  gate_ptr gptr;
  
  printf ("%s", str);
  for (i = 0; (gptr = gpptr[i]) != NULL; i++)
    printf ("%s ", Gate_String (gptr));
  printf ("\n");  
}

/*******************************************************/
void Push_Places (gate_ptr gptr, signal_ptr *spptr1)
{
  spec_place_ptr splptr;
  
  splptr = (spec_place_ptr) malloc (sizeof (spec_places));
  Spec_All_Places (splptr) = spptr1;
  Spec_Int_Places (splptr) = NULL;
  Spec_Ext_Places (splptr) = NULL;
  Spec_Next (splptr) = Gate_State_Places (gptr);
  Gate_State_Places (gptr) = splptr;
}

/*******************************************************/
void Create_Input_Index (gate_ptr gptr)
{
  int i;
  signal_ptr sptr;
  spec_index_ptr siptr;
  
  for (i = 0; (sptr = Input_Signal (Gate_State_Inputs (gptr)[i])) != NULL; i++) {
    siptr = (spec_index_ptr) malloc (sizeof (spec_index));
    Spec_Index_Spec (siptr) = gptr;
    Spec_Index_Index (siptr) = i;
    Spec_Index_Next (siptr) = Signal_SpecFanout (sptr);
    Signal_SpecFanout (sptr) = siptr;
  }   
}

/*******************************************************/
void Enter_State (gate_ptr gptr, signal_ptr *spptr)
{
  Gate_State (gptr) = (spec_ptr) malloc (sizeof (spec));
  Gate_State_Inputs (gptr) = Gate_Inputs (gptr);
  Gate_Inputs (gptr) = NULL;
  Gate_State_Places (gptr) = NULL;
  Push_Places (gptr, spptr);
  // The following, generates the index of this signal in the input-list for the corresponding signal_structure
  Create_Input_Index (gptr); 
}

/*******************************************************/
gate_ptr Enter_Gate (name_ptr nptr, component_type gtype, 
		     input_ptr in_ptr, signal_ptr out_ptr, signal_ptr hid_ptr, int mode)
{
  // mode : 1=out_ptr is a chain of signals, 0=out_ptr is NULL-ended array of signals
  gate_ptr   gptr;
	
  if (Name_Gate (nptr) != NULL) {
    printf ("gate already exists!\n");
    exit (-1);
  }

  gptr = Name_Gate (nptr) = (gate_ptr) malloc (sizeof (gate));
  if ((gtype == _AND) || (gtype == _OR) || (gtype == _NAND) || (gtype == _NOR) || (gtype == _BUFFER) || 
      (gtype == _NOT) || (gtype == _AOI12) || (gtype == _AOI22) || (gtype == _OAI12) || (gtype == _OAI22))
    Gate_Comb (gptr) = 1;
  else 
    Gate_Comb (gptr) = 0;
  Gate_Name (gptr) = nptr;
  if (gtype != _STATE)
    Gate_Num++;
  else
    Spec_Num++; // June 11  
  Gate_Type (gptr) = gtype;
  Gate_Inputs (gptr) = in_ptr;
  Gate_Complex_Inputs (gptr) = NULL; // April 20
  Gate_Outputs (gptr) = (mode) ? Signal_Chain_to_ZArray (out_ptr) : (signal_ptr *) out_ptr;
  Gate_Int_Out (gptr) = Gate_Ext_Out (gptr) = NULL;
  Make_Parent (Gate_Outputs (gptr), gptr);
  
  // For _STATE gates, hid_ptr is used to pass the ZArray of places
  if (Gate_Type (gptr) != _STATE) {
    Gate_Hidden (gptr) = Signal_Chain_to_ZArray (hid_ptr);
    Make_Parent (Gate_Hidden (gptr), gptr);
  }   
  Gate_Complex_Int_F (gptr) = Gate_Complex_Ext_F (gptr) = NULL;
  Gate_Subcircuit (gptr) = NULL;
  Push_Subcir (gptr, NULL, 0); // April 29
  Gate_Visited (gptr) = 0;
  if (Gate_Type (gptr) != _STATE) {
    Gate_Next (gptr) = gate_list;
    gate_list = gptr;		/* Global list of gates */
  } else { // June 10
    Gate_Next (gptr) = spec_list;
    spec_list = gptr;		/* Global list of gates */  
  }    
  Gate_Env_Stat (gptr) = NULL;
  Gate_TRI_TRO_List (gptr) = NULL;
  Gate_Uncle (gptr) = NULL;
  Gate_Niece (gptr) = NULL; // June 7
  Gate_Cones (gptr) = NULL;
  Gate_Is_Cone (gptr) = 0;
  Gate_Next_Cone (gptr) = NULL;
  Gate_HLevel (gptr) = 0;
  if (Gate_Type (gptr) == _STATE) // June 7
    Enter_State (gptr, (signal_ptr *) hid_ptr);  

  return (gptr);
}		     
 
/*******************************************************/
void End_Gate_Enter (void)
{
  gate_list = GATE_NULL_PTR; 
  spec_list = GATE_NULL_PTR; 
}

/*******************************************************/
// Gate operation functions
/*******************************************************/
int Check_Inputs_Event (gate_ptr gptr, int mode, int Ext_Int)
{
  int 		j;
  signal_ptr  	sptr;
  
  for (j = 0; (sptr = Input_Signal(Gate_Inputs(gptr)[j])) != NULL; j++) {
    if (Signal_Type (sptr) == EXTERNAL || mode == TOTAL) 
      return (1);
    if (Gate_Changed (Signal_Parent (sptr)))
      return (1);
  }    
  return (0);    
} 


/*******************************************************/
// Once the code for _STATE is in place, the rest of the code
// which were partly modified to include SPEC elements can be 
// reverse-modified. This will encapsulate all the effects
// of _STATEs!!
// Just that before anything else, make sure that this whole
// thing is doable not only for the flat case but also for 
// the different complexities associated with the hierarchichal
// mode, e.g., complex-gates, projections, subcircuit identification
// TRI-TROs, etc.


/*******************************************************/
bdd_t *Set_FF_Function (gate_ptr gptr)
{
          signal_ptr sptr1, sptr2;
          bdd_t *var1, *var2, *out1, *out2, *next1, *next2;
	  bdd_t *zz, *oo, *xor1, *xor2, *xor3, *xor4;
          bdd_t *temp1, *temp0, *temp2, *temp3;
          
	  sptr1 = Input_Signal(Gate_Inputs (gptr)[0]);
	  var1 = bdd_get_var (Bdd_Man, Signal_Order (sptr1));
	  out1 = bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs (gptr)[0]));
	  next1 = bdd_get_var (Bdd_Man, Signal_Next_Order (Gate_Outputs (gptr)[0]));  
	  sptr2 = Input_Signal(Gate_Inputs (gptr)[1]);						
	  var2 = bdd_get_var (Bdd_Man, Signal_Order (sptr2));
	  out2 = bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs(gptr)[1])); 		
	  next2 = bdd_get_var (Bdd_Man, Signal_Next_Order (Gate_Outputs(gptr)[1]));   

	  oo = bdd_and (var1, var2, 1, 1);		// 11XX00 
	  zz = bdd_and (next1, next2, 0, 0);
	  temp0 = bdd_and (oo, zz, 1, 1);
	  bdd_free (oo);
	  
	  xor1 = bdd_xor (var1, var2);			// 100100,011000 
	  xor2 = bdd_xor (out1, out2);
	  xor3 = bdd_xor (var1, out1);
	  temp1 = bdd_and (xor1, xor2, 1, 1);
	  temp2 = bdd_and (xor3, zz, 1, 1);
	  temp3 = bdd_and (temp1, temp2, 1, 1);
	  free_ite (zz, temp1, temp2);
	  temp1 = bdd_or (temp0, temp3, 1, 1);		
	  free_ite (xor3, temp0, temp3);
	  
	  zz = bdd_and (var1, var2, 0, 0);		// 001010,000101 
	  xor3 = bdd_xor (next1, next2);
	  xor4 = bdd_xor (out1, next1);
	  temp0 = bdd_and (xor3, xor2, 1, 1);
	  temp3 = bdd_and (xor4, zz, 0, 1);
	  temp2 = bdd_and (temp0, temp3, 1, 1);
	  free_ite (temp0, temp3, zz);
	  temp0 = bdd_or (temp1, temp2, 1, 1);
	  free_ite (xor4, temp1, temp2);
	  
	  zz = bdd_and (out1, out2, 0, 0);		// 010001,100010 
	  xor4 = bdd_xor (var1, next1);
	  temp1 = bdd_and (xor1, xor3, 1, 1);
	  temp2 = bdd_and (zz, xor4, 1, 0);
	  temp3 = bdd_and (temp1, temp2, 1, 1);
	  free_ite (temp1, temp2, NULL);
//	  Print_Bdd (temp3, "CHECK1 ");
	  temp1 = bdd_or (temp3, temp0, 1, 1);
	  free_ite (temp3, temp0, NULL);
	  
	  temp3 = bdd_and (xor1, xor2, 1, 1);		// 010101,101010 
	  temp0 = bdd_and (xor4, xor3, 0, 1);
	  free_ite (xor1, xor2, xor3);
	  xor1 = bdd_xor (var1, out1);
	  temp2 = bdd_and (temp3, temp0, 1, 1);
	  free_ite (temp3, temp0, xor4);
	  temp3 = bdd_and (temp2, xor1, 1, 0);
	  temp0 = bdd_or (temp1, temp3, 1, 1);
//	  Print_Bdd (temp3, "CHECK2 ");
	  free_ite (temp1, temp3, xor1);
	  bdd_free (temp2);

	  temp1 = bdd_and (var1, var2, 0, 0);		// 0000XX // // Just for completeness 
	  temp3 = bdd_and (temp1, zz, 1, 1);
	  temp2 = bdd_or (temp3, temp0, 1, 1);
	  free_ite (temp1, temp0, zz);
	  bdd_free (temp3);	  
	  
	  temp1 = bdd_and (out1, out2, 1, 1);		// XX11XX
	  temp0 = bdd_or (temp1, temp2, 1, 1);
	  free_ite (temp1, temp2, NULL);
	  	  	  
/*	  	  
	  Print_Bdd (temp2, "FF BDD: ");
*/
	  bdd_free (Gate_Function (gptr));
	  Gate_Function (gptr) = temp0;
	  
/*	// Dec 29	  
	  // NOTICE! Assuming that both outputs are either internal or external!
	  if (Gate_Ext_Out (gptr) != NULL) {
	    bdd_free (Gate_Ext_F (gptr));
	    Gate_Ext_F (gptr) = bdd_dup (temp0);
	  }  
	  else {
	    bdd_free (Gate_Int_F (gptr));
	    Gate_Int_F (gptr) = bdd_dup (temp0);  
	  }  
*/
	temp0 = bdd_dup (temp0); // Dec 29
	return (temp0);		 // Dec 29	  
}

/*******************************************************/
bdd_t *Set_ME_Function (gate_ptr gptr)
{
          signal_ptr sptr1, sptr2;
          bdd_t *var1, *var2, *out1, *out2, *next1, *next2, *zz, *var1_var2_xor;
          bdd_t *var1_equal_next1, *var2_equal_next2, *out1_equal_next1, *out2_equal_next2;
          bdd_t *temp1, *temp0, *temp2, *temp3, *temp4, *choice, *no_choice, *var11_var21, *out1_out2_xor, *next1_next2_xor;
          
	  sptr1 = Input_Signal(Gate_Inputs (gptr)[0]);
	  var1 = bdd_get_var (Bdd_Man, Signal_Order (sptr1));
	  out1 = bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs (gptr)[0]));
	  next1 = bdd_get_var (Bdd_Man, Signal_Next_Order (Gate_Outputs (gptr)[0])); 

          sptr2 = Input_Signal(Gate_Inputs (gptr)[1]);
	  var2 = bdd_get_var (Bdd_Man, Signal_Order (sptr2));
	  out2 = bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs(gptr)[1])); 
	  next2 = bdd_get_var (Bdd_Man, Signal_Next_Order (Gate_Outputs(gptr)[1])); 

	  var1_equal_next1 = bdd_xor (var1, next1);
	  var2_equal_next2 = bdd_xor (var2, next2);
	  out1_equal_next1 = bdd_xor (out1, next1);
	  out2_equal_next2 = bdd_xor (out2, next2);
	  var11_var21 = bdd_and (var1, var2, 1, 1);
	  out1_out2_xor = bdd_xor (out1, out2);
	  next1_next2_xor = bdd_xor (next1, next2);
	  var1_var2_xor = bdd_xor (var1, var2);
	  
	  temp0 = bdd_and (var1, out1, 0, 0);			/* 000100,010001,010101 */
	  temp1 = bdd_and (var2_equal_next2, next1, 0, 0);
	  temp3 = bdd_and (temp0, temp1, 1, 1); /* r1 = 0 */
	  free_ite (temp0, temp1, NULL);

	  temp0 = bdd_and (var2, out2, 0, 0);			/* 001000,100010,101010 */
	  temp1 = bdd_and (var1_equal_next1, next2, 0, 0);
	  temp4 = bdd_and (temp0, temp1, 1, 1);
	  bdd_free (temp0);

	  temp0 = bdd_or (temp3, temp4, 1, 1);
	  free_ite (temp1, temp3, temp4);
	  
	  temp1 = bdd_and (var11_var21, next1_next2_xor, 1, 1);	/* 110010,110001 */
	  choice = bdd_and (temp1, out1_out2_xor, 1, 0); 
	  temp3 = bdd_or (temp0, choice, 1, 1);
	  free_ite (temp0, choice, NULL);

          /* Same choice part is not needed, cause it causes no transitions! */	  
	  temp0 = bdd_and (out1_equal_next1, out1_out2_xor, 0, 1);	/* 111010,110101 */
	  no_choice = bdd_and (temp0, temp1, 1, 1);
	  temp4 = bdd_or (temp3, no_choice, 1, 1);
	  free_ite (no_choice, temp0, temp1);
	  bdd_free (temp3);
	  temp0 = temp4;
	  
	  zz = bdd_and (next1, next2, 0, 0);			/* 011000,100100 */ 
	  temp1 = bdd_and (var1_var2_xor, out1_out2_xor, 1, 1);
	  temp3 = bdd_xor (var1, out1);
	  temp4 = bdd_and (temp3, zz, 1, 1);
	  bdd_free (zz);
	  zz = bdd_and (temp4, temp1, 1, 1);
	  bdd_free (temp4);
	  temp4 = bdd_or (temp0, zz, 1, 1);
	  free_ite (temp0, zz, NULL);

	  temp0 = temp4; 			/* because of the removal of the above */
	  free_ite (temp1, temp3, NULL);	/* because of the removal of the above */
	  	  
	  temp1 = bdd_and (var1, var2, 0, 0);	/* 000000 */
	  zz = bdd_and (out1, out2, 0, 0);
	  temp4 = bdd_and (temp1, zz, 1, 1);
	  free_ite (temp1, zz, NULL);
	  temp3 = bdd_and (next1, next2, 0, 0);
	  temp1 = bdd_and (temp3, temp4, 1, 1);
	  zz = bdd_or (temp0, temp1, 1, 1);
	  free_ite (temp0, temp1, temp3);
	  bdd_free (temp4);
	  
	  temp0 = bdd_and (out1, out2, 1, 1);	/* XX11XX */
	  temp1 = bdd_or (zz, temp0, 1, 1);
	  free_ite (zz, temp0, NULL);
	  
//	  Print_Bdd (temp1, Gate_String (gptr));
	  
	  free_ite (var1_equal_next1, var2_equal_next2, out1_equal_next1);
	  free_ite (out2_equal_next2, out1_out2_xor, next1_next2_xor);
	  bdd_free (var1_var2_xor);
	  bdd_free (Gate_Function (gptr));
	  Gate_Function (gptr) = temp1;


	temp1 = bdd_dup (temp1); // Dec 29
	return (temp1);		 // Dec 29
}


/*******************************************************/
bdd_t *Set_Gate_Function (gate_ptr gptr) // fixed
{

  int 		j, invert;
  bdd_t		*bdd, *temp0, *temp1, *temp2;
  signal_ptr    sptr, sptr1;  
      
      invert = 0;

      sptr = Gate_Outputs (gptr)[0];
      switch (Gate_Type (gptr)) {
        case _NOT : invert = 1;
        case _BUFFER : {
          bdd = bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[0]))); 
          if ((Input_Invert(Gate_Inputs (gptr)[0]) == '-' && !invert) ||
              (Input_Invert(Gate_Inputs (gptr)[0]) == '+' && invert) )
            bdd = bdd_not (bdd);
          else
            bdd = bdd_dup (bdd);          
          break;
        }
        case _NAND : invert = 1;
        case _AND : {           
          bdd = bdd_one (Bdd_Man);
          for (j = 0; (sptr = Input_Signal(Gate_Inputs(gptr)[j])) != NULL; j++) {
            temp1 = bdd_and (bdd, bdd_get_var (Bdd_Man, Signal_Order (sptr)), 1, 
              		(Input_Invert(Gate_Inputs(gptr)[j])=='+') ? 1 : 0);
            bdd_free (bdd);
            bdd = temp1; 
          }   		
          if (invert) {
            temp1 = bdd_not (bdd);
            bdd_free (bdd);
            bdd = temp1;
          }
          break;
        }    
          
        case _NOR : invert = 1;
        case _OR : {        
	  bdd = bdd_zero (Bdd_Man);
          for (j = 0; (sptr = Input_Signal(Gate_Inputs(gptr)[j])) != NULL; j++) {
            temp1 = bdd_or (bdd, bdd_get_var (Bdd_Man, Signal_Order (sptr)), 1, 
              		(Input_Invert(Gate_Inputs(gptr)[j])=='+') ? 1 : 0);
            bdd_free (bdd);
            bdd = temp1; 
          }   		
          if (invert) {
            temp1 = bdd_not (bdd);
            bdd_free (bdd);
            bdd = temp1;
          }
          break;
        } 
        
        case _AOI12 :
          temp1 = bdd_and (bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[1]))), 
          		 bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[2]))), 
          		 (Input_Invert(Gate_Inputs(gptr)[1])=='+') ? 1 : 0, 
          		 (Input_Invert(Gate_Inputs(gptr)[2])=='+') ? 1 : 0);
          bdd = bdd_or (temp1, bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[0]))), 1, 
          		 (Input_Invert(Gate_Inputs(gptr)[0])=='+') ? 1 : 0);
          bdd_free (temp1);
          temp1 = bdd_not (bdd);
          bdd_free (bdd);
          bdd = temp1;
          break;
          
        case _AOI22 :
          temp1 = bdd_and (bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[0]))), 
          		 bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[1]))), 
          		 (Input_Invert(Gate_Inputs(gptr)[0])=='+') ? 1 : 0, 
          		 (Input_Invert(Gate_Inputs(gptr)[1])=='+') ? 1 : 0);          		 
          temp2 = bdd_and (bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[2]))), 
          		 bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[3]))), 
          		 (Input_Invert(Gate_Inputs(gptr)[2])=='+') ? 1 : 0, 
          		 (Input_Invert(Gate_Inputs(gptr)[3])=='+') ? 1 : 0);          		 
          bdd = bdd_or (temp1, temp2, 1, 1);
          bdd_free (temp1);
          bdd_free (temp2);
          temp1 = bdd_not (bdd);
          bdd_free (bdd);
          bdd = temp1;
          break;
          
        case _OAI12 :
          temp1 = bdd_or (bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[1]))), 
          		 bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[2]))), 
          		 (Input_Invert(Gate_Inputs(gptr)[1])=='+') ? 1 : 0, 
          		 (Input_Invert(Gate_Inputs(gptr)[2])=='+') ? 1 : 0);
          bdd = bdd_and (temp1, bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[0]))), 1,
          		 (Input_Invert(Gate_Inputs(gptr)[0])=='+') ? 1 : 0);
          bdd_free (temp1);
          temp1 = bdd_not (bdd);
          bdd_free (bdd);
          bdd = temp1;
          break;
          
        case _OAI22 :
          temp1 = bdd_or (bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[0]))), 
          		 bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[1]))), 
          		 (Input_Invert(Gate_Inputs(gptr)[0])=='+') ? 1 : 0, 
          		 (Input_Invert(Gate_Inputs(gptr)[1])=='+') ? 1 : 0);          		 
          temp2 = bdd_or (bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[2]))), 
          		 bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[3]))), 
          		 (Input_Invert(Gate_Inputs(gptr)[2])=='+') ? 1 : 0, 
          		 (Input_Invert(Gate_Inputs(gptr)[3])=='+') ? 1 : 0);          		 
          bdd = bdd_and (temp1, temp2, 1, 1);
          bdd_free (temp1);
          bdd_free (temp2);
          temp1 = bdd_not (bdd);
          bdd_free (bdd);
          bdd = temp1;
          break;
                    
        case _C_ELEMENT : {          
          bdd_t *bdd_if, *bdd_else, *var, *temp1, *temp2, *temp3, *if_term, *else_term;
          
          bdd_if = bdd_one (Bdd_Man);
          bdd_else = bdd_one (Bdd_Man);
          sptr = Gate_Outputs(gptr)[0];
          for (j = 0; (sptr1 = Input_Signal(Gate_Inputs(gptr)[j])) != NULL; j++) {
            temp1 = bdd_and (bdd_if, var = bdd_get_var (Bdd_Man, Signal_Order (sptr1)), 1, 
              		(Input_Invert(Gate_Inputs(gptr)[j])=='+') ? 1 : 0);
            bdd_free (bdd_if);
            bdd_if = temp1; 
            temp1 = bdd_and (bdd_else, var, 1, 
              		(Input_Invert(Gate_Inputs(gptr)[j])=='+') ? 0 : 1);
            bdd_free (bdd_else);
            bdd_else = temp1;             
          }
    	  sptr = Gate_Outputs (gptr)[0];
          var = bdd_get_var (Bdd_Man, Signal_Order (sptr));
          temp2 = bdd_and (bdd_else, var, 0, 1);
          temp3 = bdd_and (bdd_if, var, 0, 0);
          if_term = bdd_or (bdd_if, temp2, 1, 1);
          else_term = bdd_or (bdd_else, temp3, 1, 1);
          free_ite (temp2, temp3, NULL);
          free_ite (bdd_if, bdd_else, NULL);
          temp1 = bdd_ite (bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)), if_term, else_term, 1, 1, 1);
          free_ite (if_term, else_term, NULL);
          return (temp1); 
        }
        
        case _TABLE1 : { // ?????????????????????????
          int i;
          bdd_t *var = bdd_get_var (Bdd_Man, Signal_Order (sptr));
          temp0 = bdd_one (Bdd_Man); // Notice
          for (i = 1; (sptr = Input_Signal (Gate_Inputs (gptr)[i])) != NULL; i++) {
            temp1 = bdd_and (temp0, bdd_get_var (Bdd_Man, Signal_Order (sptr)), 1, 1);
            bdd_free (temp0);
            temp0 = temp1;
          }
          bdd = bdd_or (temp0, bdd_get_var (Bdd_Man, Signal_Order (Input_Signal (Gate_Inputs (gptr)[0]))), 1, 0);
          bdd_free (temp0);
          break;          
	}

	case _ME : 
	  return (Set_ME_Function (gptr));

	case _FF : 
	  return (Set_FF_Function (gptr));
/*	  
	case _STATE : 
	case _OUT :
	  // implemented elsewhere!
*/
        default :
          printf ("Other gates to be implemented");
          exit (-1);
          break;
      }
    sptr = Gate_Outputs (gptr)[0];
    temp0 = bdd_not (bdd);  
    temp1 = bdd_ite (bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)), bdd, temp0, 1, 1, 1);
//    Print_Bdd (temp1, Gate_String (gptr));
    bdd_free (bdd);
    bdd_free (temp0);

    if (Gate_Type (gptr) == _TABLE1) {
      temp0 = Make_Bdd_Complete (temp1, gptr);
      bdd_free (temp1);
      temp1 = temp0;
//      Print_Bdd (temp1, "TABLE Function: ");
    }  

    return (temp1);
}

/*******************************************************/
void Set_Gate_Functions (void)
{
  int i;
  gate_ptr gptr;
  
  for (i = 0; i < Gate_Num; i++) {
    gptr = list_of_gates [i];
    if (Gate_Type (gptr) == _OUT)
      continue;
    if (Gate_Int_Out (gptr) != NULL) {
      bdd_free (Gate_Int_F (gptr));
      Gate_Int_F (gptr) = Set_Gate_Function (gptr);
    } else { // ????? What if a gate has both internal and external outputs?!!!!!
      bdd_free (Gate_Ext_F (gptr));
      Gate_Ext_F (gptr) = Set_Gate_Function (gptr);
    }      
  }
  for (i = 0; i < Spec_Num; i++)
    Hoshik_State (list_of_specs [i]);
}
