#include "type.h"
#include "sphinx.h"

extern bdd_manager	*Bdd_Man;
extern int MODE;
extern array_t  *place_array, *input_array, *output_array;
extern bdd_t 	*signal_var[];  
extern context_ptr	cur_cir; 


input_ptr Signal_Array_to_Input_ZArray (array_t *sptr);
bdd_t *Sig_Cur_Next (gate_ptr gptr, int cur, int next);
bdd_t *Mul_Cur (gate_ptr gptr, int mode);
bdd_t *Sig_Next (gate_ptr gptr, int mode);
bdd_t *Set_Hazards (gate_ptr gptr);
void Set_Gate_Output_Change (gate_ptr gptr, sig_type Int_Ext);


/*******************************************************/
input_ptr Signal_Array_to_Input_ZArray (array_t *sptr)
{
  int i;
  input_ptr iptr;
  
  iptr = (input_ptr) malloc (sizeof (input) * (array_n (sptr) + 1));
  for (i = 0; i < array_n (sptr); i++) {
    Input_Signal (iptr[i]) = (signal_ptr) array_fetch (signal_ptr, sptr, i);    
    Input_Invert (iptr[i]) = '+';
    Input_Pushed (&(iptr[i])) = NULL;
    Push_Input (&(iptr[i]), 0);
    Input_Pushed_TR (&(iptr[i])) = NULL;
    Input_Pushed_TR (&(iptr[i])) = Push_Bdd (Input_Pushed_TR (&(iptr[i])), NULL); // No TR yet!
  }
  Input_Signal (iptr[i]) = NULL;
  return (iptr);
}

/*******************************************************/
// Specification Functions
/*******************************************************/

/*******************************************************/
void New_Specification (char *spec_name)
{

// IMPORTANT: The input arrays, spec_input_list, spec_place_list, spec_output_list are not freed by this routine

  int i, temp, input_no;
  name_ptr nptr;
  gate_ptr gptr, gtemp, gfirst;
  input_ptr input_array_signed, spec_input_array;
  signal_ptr sptr, sptr1, *spec_place_array, *spec_output_array;
  char name[100];
  array_t *temp_array;

// This function is called by the parser on the occasion of a Specification.
// It is assumed that the parser also calls appropriate functions which 
// create the data structures for keeping some representation of the specification
// before it is transformed to a BDD form.

  name[0] = 0;
  nptr = Name_Enter (strcat (name, "_g"), GATE);
  input_no = array_n (input_array);
  
  input_array_signed = Signal_Array_to_Input_ZArray (input_array);
  
  temp_array = array_dup (input_array);
  array_append (temp_array, output_array);
  spec_input_array = Signal_Array_to_Input_ZArray (temp_array); // Standard input list
  array_free (temp_array);
    
  spec_place_array = Array_to_ZArray (place_array);
  spec_output_array = Array_to_ZArray (output_array); // redundant! The loop below can be changed!

  // VERY IMPORTANT: if I ever want to free up the inout/output lists of gates, 
  // I should be careful since I need to free the input list of the _OUT gates 
  // only once, since they share the same list.
  gptr = Enter_Gate (nptr, _STATE, spec_input_array, 
  		     (signal_ptr) spec_output_array, (signal_ptr) spec_place_array, 0); 
  		     
  Gate_State_All_Places_Array (gptr) = place_array;
  Gate_Inputs (gptr) = input_array_signed;
  		      		     
  for (i = 0; (sptr = spec_output_array [i]) != NULL; i++) {
    strcpy (name, Signal_String (sptr) /*[strlen (spec_name)+1]*/);
    nptr = Name_Enter (strcat (name, "_g"), GATE);
    Signal_Next (sptr) = NULL; // for the list of outputs of gtemp!
    gtemp = Enter_Gate (nptr, _OUT, NULL, sptr, NULL, 1); // ????????
    Gate_Uncle (gtemp) = gptr; // the state gate for this output gate
    if (i == 0)
      gfirst = gtemp;
    Gate_Out_Index (gtemp) = i + input_no; // June 7: This is the index of this _OUT gate in the output list of the _STATE gate
  } 
  Gate_Niece (gptr) = Gate_Chain_to_ZArray (gfirst, i); // list of the output_gates for the state_gate
}

/*******************************************************/
// Hazard related functions
/*******************************************************/
/*******************************************************/
bdd_t *Sig_Cur_Next (gate_ptr gptr, int cur, int next)
{
  int j;
  bdd_t  *temp0, *temp1, *temp2, *var, *next_var;
  signal_ptr   sptr;
  
  temp0 = bdd_zero (Bdd_Man);
  for (j = 0; Input_Signal (Gate_Inputs(gptr)[j]) != NULL; j++) {
    sptr = Input_Signal (Gate_Inputs (gptr)[j]);
    var = bdd_get_var (Bdd_Man, Signal_Order (sptr));
    next_var = bdd_get_var (Bdd_Man, Signal_Next_Order (sptr));
    temp1 = bdd_and (var, next_var, 
    		((Input_Invert(Gate_Inputs(gptr)[j]) == '+') ? cur : 1-cur), 
    		((Input_Invert(Gate_Inputs(gptr)[j]) == '+') ? next : 1-next));
    temp2 = bdd_or (temp0, temp1, 1, 1);
    free_ite (temp1, temp0, NULL);
    temp0 = temp2;  	
  }
  return (temp0);
}

/*******************************************************/
bdd_t *Sig_Next (gate_ptr gptr, int mode)
{
  int j;
  bdd_t  *temp0, *temp2, *next_var;
  signal_ptr   sptr;
  
  temp0 = bdd_zero (Bdd_Man);
  for (j = 0; Input_Signal (Gate_Inputs(gptr)[j]) != NULL; j++) {
    sptr = Input_Signal (Gate_Inputs (gptr)[j]);
    next_var = bdd_get_var (Bdd_Man, Signal_Next_Order (sptr));
    temp2 = bdd_or (temp0, next_var, 1, ((Input_Invert(Gate_Inputs(gptr)[j]) == '+') ? mode : 1-mode));
    bdd_free (temp0);
    temp0 = temp2;  	
  }
  return (temp0);
}

/*******************************************************/
bdd_t *Mul_Cur (gate_ptr gptr, int mode)
{
  int j;
  bdd_t  *temp0, *temp2, *var;
  signal_ptr   sptr;
  
  temp0 = bdd_one (Bdd_Man);  /* NOTICE!!!!!! Was mistakenly a zero! */
  for (j = 0; Input_Signal(Gate_Inputs(gptr)[j]) != NULL; j++) {
    sptr = Input_Signal (Gate_Inputs (gptr)[j]);
    var = bdd_get_var (Bdd_Man, Signal_Order (sptr));
    temp2 = bdd_and (temp0, var, 1, ((Input_Invert(Gate_Inputs(gptr)[j]) == '+') ? mode : 1-mode));
    bdd_free (temp0);
    temp0 = temp2;  	
  }
  return (temp0);
}


/*******************************************************/
bdd_t *Set_Hazards (gate_ptr gptr)
{
  int invert;
  bdd_t      *temp0, *temp1, *if_term, *else_term, *var;
  
      invert = 0;
      switch (Gate_Type (gptr)) {
        case _NOT : invert = 1;
        case _BUFFER :
          if_term = Sig_Cur_Next (gptr, 0, 1);
          temp0 = Mul_Cur (gptr, 1);
          temp1 = Sig_Next (gptr, 0);
          else_term = bdd_and (temp0, temp1, 1, 1);
          bdd_free (temp0);
          bdd_free (temp1);
          break;
          
        case _C_ELEMENT :
	  temp0 = Mul_Cur (gptr, 1);
	  temp1 = Sig_Next (gptr, 0);
	  else_term = bdd_and (temp0, temp1, 1, 1);
	  bdd_free (temp0);
	  bdd_free (temp1);
	  temp0 = Mul_Cur (gptr, 0);
	  temp1 = Sig_Next (gptr, 1);
	  if_term = bdd_and (temp0, temp1, 1, 1);
	  bdd_free (temp0);
	  bdd_free (temp1);
          break;
           
        case _ME : {
          bdd_t *r1, *r2, *r1_, *r2_, *q1, *q2, *zz, *xor1, *xor2, *xor3, *xor4, *oo, *temp0, *temp1, *temp2, *temp3;
          
          r1 = bdd_get_var (Bdd_Man, Signal_Order (Input_Signal(Gate_Inputs (gptr) [0])));
          r2 = bdd_get_var (Bdd_Man, Signal_Order (Input_Signal(Gate_Inputs (gptr) [1])));
          r1_ = bdd_get_var (Bdd_Man, Signal_Next_Order (Input_Signal(Gate_Inputs (gptr) [0])));
          r2_ = bdd_get_var (Bdd_Man, Signal_Next_Order (Input_Signal(Gate_Inputs (gptr) [1])));
          q1 = bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs (gptr) [0]));
          q2 = bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs (gptr) [1]));
          
          xor1 = bdd_xor (q1, q2);
          xor2 = bdd_xor (r1_, r2_);
          xor3 = bdd_xor (r1, r2);
          xor4 = bdd_xor (r1_, q1);
          temp1 = bdd_and (xor1, xor2, 1, 1);
          temp2 = bdd_and (xor3, xor4, 0, 0);
          temp0 = bdd_and (temp1, temp2, 1, 1);

/*          Print_Bdd (temp0, "\n");*/

          free_ite (temp1, temp2, xor4);
          xor4 = bdd_xor (r1, q1);
          temp1 = bdd_and (xor1, xor2, 1, 0);
          temp2 = bdd_and (xor3, xor4, 1, 1);
          temp3 = bdd_and (temp1, temp2, 1, 1);
          free_ite (temp1, temp2, xor4);

/*          Print_Bdd (temp3, "\n");*/
                    
          temp1 = bdd_or (temp0, temp3, 1, 1);
          
          free_ite (temp0, temp3, xor1);
          zz = bdd_and (r1_, r2_, 0, 0);
          temp0 = bdd_and (zz, xor3, 1, 1);
          free_ite (zz, xor3, NULL);
          zz = bdd_and (q1, q2, 0, 0);
          temp2 = bdd_and (temp0, zz, 1, 1);
          bdd_free (temp0);

/*          Print_Bdd (temp2, "\n");*/

          temp0 = bdd_or (temp1, temp2, 1, 1);
          free_ite (temp1, temp2, NULL);
          oo = bdd_and (r1, r2, 1, 1);
          temp1 = bdd_and (oo, zz, 1, 1);
          temp2 = bdd_and (temp1, xor2, 1, 1);

/*          Print_Bdd (temp2, "\n");*/

          free_ite (oo, zz, temp1);
          temp1 = bdd_or (temp0, temp2, 1, 1);
          free_ite (xor2, temp0, temp2);
          
          temp0 = bdd_and (q1, q2, 1, 1);
          temp2 = bdd_or (temp0, temp1, 1, 1);
          free_ite (temp0, temp1, NULL);

/*          Print_Bdd (temp2, "\n");*/

//	  Set_ME_Function (gptr); // Dec 29
          return (temp2);
        }
        
	case _FF : {
          bdd_t *temp0, *temp1, *temp2, *temp3, *var1, *var2, *out1, *out2, *next1, *next2,
          	*oo, *zz1, *zz2, *zo1;
          
          var1 = bdd_get_var (Bdd_Man, Signal_Order (Input_Signal(Gate_Inputs (gptr) [0])));
          var2 = bdd_get_var (Bdd_Man, Signal_Order (Input_Signal(Gate_Inputs (gptr) [1])));
          next1 = bdd_get_var (Bdd_Man, Signal_Next_Order (Input_Signal(Gate_Inputs (gptr) [0])));
          next2 = bdd_get_var (Bdd_Man, Signal_Next_Order (Input_Signal(Gate_Inputs (gptr) [1])));
          out1 = bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs (gptr) [0]));
          out2 = bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs (gptr) [1]));
          
          zz1 = bdd_and (var1, var2, 0, 0);			/* 0000XX*/
          zz2 = bdd_and (out1, out2, 0, 0);
          temp2 = bdd_and (zz1, zz2, 1, 1);
          bdd_free (zz1);
          
          zo1 = bdd_and (var1, var2, 0, 1);			/* 01001X */
          temp0 = bdd_and (zz2, next1, 1, 1);
          temp1 = bdd_and (zo1, temp0, 1, 1);
          temp3 = bdd_or (temp2, temp1, 1, 1);
          free_ite (temp2, temp1, temp0);
          
          oo = bdd_and (out2, next2, 0, 0);			/* 01X0X0 */
          temp0 = bdd_and (oo, zo1, 1, 1);
          temp1 = bdd_or (temp3, temp0, 1, 1);
          free_ite (temp0, temp3, zo1);
          
          zo1 = bdd_and (var1, var2, 1, 0);			/* 1000X1 */
          temp0 = bdd_and (zz2, next2, 1, 1);
          temp2 = bdd_and (zo1, temp0, 1, 1);
          temp3 = bdd_or (temp2, temp1, 1, 1);
          free_ite (temp2, temp0, temp1);
          bdd_free (oo);
          
          oo = bdd_and (out1, next1, 0, 0);			/* 100X0X */
          temp0 = bdd_and (oo, zo1, 1, 1);
          temp1 = bdd_or (temp3, temp0, 1, 1);
          free_ite (temp0, temp3, zo1);
          
          zz1 = bdd_and (var1, var2, 1, 1);			/* 11010X */
          temp0 = bdd_and (oo, out2, 1, 1);
          temp3 = bdd_and (zz1, temp0, 1, 1);
          temp2 = bdd_or (temp3, temp1, 1, 1);
          free_ite (temp0, temp1, temp3);
          bdd_free (oo);
          
          oo = bdd_and (out2, next2, 0, 0);			/* 1110X0 */
          temp0 = bdd_and (oo, out1, 1, 1);
          temp3 = bdd_and (zz1, temp0, 1, 1);
          temp1 = bdd_or (temp2, temp3, 1, 1);
          free_ite (temp2, temp3, temp0);
          
          temp0 = bdd_and (out1, out2, 1, 1);			/* XX11XX */
          temp2 = bdd_or (temp1, temp0, 1, 1);
          free_ite (temp1, temp0, NULL);
          
/*  	  Print_Bdd (temp1, "FF Hazards ");*/
  	  
  	  free_ite (oo, zz1, zz2);
//	  Set_FF_Function (gptr); // Dec 29
  	  return (temp2); 
  	}        

/*        
	case _XNOR : invert = 1;
        case _XOR :
          printf ("XOR/XNOR to be implemented");
          exit (-1);
*/

	case _STATE : 
	case _OUT : 
	  return (bdd_zero (Bdd_Man));  // Hazards of such gates are checked differently!

	case _AND :
	case _NAND :
        case _NOR :
        case _OR :
	case _TABLE1 :
	case _AOI12 :
	case _AOI22 :
	case _OAI12 :
	case _OAI22 : {
	  bdd_t *bdd, *temp2, *temp3, *temp4, *temp5, *temp6; 
	  int i;
	  signal_ptr sptr;
	  
	  bdd = (Gate_Int_Out (gptr) != NULL) ? Gate_Int_F (gptr) : Gate_Ext_F (gptr);
	  var = bdd_get_var (Bdd_Man, Signal_Next_Order (Gate_Outputs (gptr)[0]));
	  if_term = bdd_cofactor (bdd, var);
	  temp6 = bdd_one (Bdd_Man);
	  for (i = 0; (sptr = Input_Signal (Gate_Inputs (gptr)[i])) != NULL; i++) {
	    temp1 = bdd_xnor (bdd_get_var (Bdd_Man, Signal_Order (sptr)), 
    			      bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
    	    temp5 = bdd_and (temp6, temp1, 1, 1);
    	    bdd_free (temp1);
    	    bdd_free (temp6);
    	    temp6 = temp5;
    	  }  		     
    	  temp5 = bdd_and (if_term, temp6, 1, 1);
    	  temp1 = bdd_and (if_term, temp6, 0, 1);
    	  bdd_free (temp6);
	  temp3 = bdd_zero (Bdd_Man);
	  temp4 = bdd_zero (Bdd_Man);
	  for (i = 0; (sptr = Input_Signal (Gate_Inputs (gptr)[i])) != NULL; i++) {
	    temp0 = bdd_compose (temp1, bdd_get_var (Bdd_Man, Signal_Order (sptr)), 
	    				bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
	    temp2 = bdd_or (temp0, temp3, 1, 1);
	    bdd_free (temp0);
	    bdd_free (temp3);
	    temp3 = temp2;
	    temp0 = bdd_compose (temp5, bdd_get_var (Bdd_Man, Signal_Order (sptr)), 
	    				bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
	    temp2 = bdd_or (temp0, temp4, 1, 1);
	    bdd_free (temp0);
	    bdd_free (temp4);
	    temp4 = temp2;
	  }
//	  temp0 = bdd_compose (temp1, bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs (gptr)[0])), 
//	  			      bdd_get_var (Bdd_Man, Signal_Next_Order (Gate_Outputs (gptr)[0])));
//	  bdd_free (temp1);	  			      
	  temp1 = bdd_and (if_term, temp3, 1, 1);
	  temp2 = bdd_and (if_term, temp4, 0, 1);
	  bdd = bdd_ite (bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs (gptr)[0])), temp2, temp1, 1, 1, 1);
	  free_ite (temp3, temp1, temp2);
	  free_ite (if_term, temp4, NULL);
/*
	  if (Gate_Type (gptr) == _TABLE1)
	    Print_Bdd (bdd, "Hazards of TABLE1: ");
	  if (!strcmp ("oai5", Gate_String (gptr))) 
	    Print_Bdd (bdd, "Hazards of oai5: ");
*/
	  return (bdd); 
	}  
	
        default : 
          printf ("Other gates to be implemented");
          exit (-1);
          break;
      }
    var = bdd_get_var (Bdd_Man, Signal_Order (Gate_Outputs(gptr)[0]));
    temp0 = bdd_and (var, if_term, 1 - invert, 1);
    temp1 = bdd_and (var, else_term, invert, 1);    
    var = bdd_or (temp0, temp1, 1, 1);
    free_ite (temp0, temp1, if_term);
    bdd_free (else_term);
    return (var);
}

/*******************************************************/
void Set_Gate_Output_Change (gate_ptr gptr, sig_type Int_Ext)
{
  int i;
  signal_ptr 	*spptr, sptr;
  bdd_t		*var, *next_var, *temp1, *temp2, *temp0;
  
  spptr = (Int_Ext == INTERNAL) ? Gate_Int_Out (gptr) : Gate_Ext_Out (gptr);
  temp0 = bdd_zero (Bdd_Man);
  if (spptr != NULL)
    for (i = 0; (sptr = spptr[i]) != NULL; i++) {
      next_var = bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)); 
      var = bdd_get_var (Bdd_Man, Signal_Order (sptr));
      temp1 = bdd_xor (var, next_var);
      temp2 = bdd_or (temp0, temp1, 1, 1);
      bdd_free (temp1);
      bdd_free (temp0);
      temp0 = temp2;
    }
  if (Int_Ext == INTERNAL) {
    bdd_free (Gate_Int_Ch (gptr));
    Gate_Int_Ch (gptr) = temp0;
  }  
  else {
    bdd_free (Gate_Ext_Ch (gptr));
    Gate_Ext_Ch (gptr) = temp0;   
  }                
}

/*******************************************************/
void Set_Gate_Hazards (void) 
{
  int i;
  gate_ptr gptr;
  
  for (i = 0; i < Gate_Num; i++) {
    gptr = list_of_gates[i]; 
    Gate_Hazards (gptr) = Set_Hazards (gptr);
    if (Gate_Type (gptr) != _STATE) { // June 10 // June 11: not needed, since list_of_gates does not include any _STATE gates
      Set_Gate_Output_Change (gptr, INTERNAL);
      Set_Gate_Output_Change (gptr, EXTERNAL);
    }  
  }  
}
