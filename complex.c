#include "type.h"
#include "sphinx.h"

extern int	MODE;

extern int	cflag, lflag, tflag, Cflag;
extern gate_ptr	gate_list;
extern bdd_manager	*Bdd_Man;
extern context_ptr	flat_cir, complex_cir, cur_cir; 
extern bdd_t 		*signal_var[];  
extern signal_ptr	order[]; 


gate_ptr *Enter_Gate_List (void);
bdd_t *Compose_Gates (gate_ptr gptr, bdd_t **bdd);
input_ptr Create_Input_List (bdd_t *bdd, signal_ptr *spptr);
void Add_Complex_Signals (signal_ptr *spptr);
void Add_Complex_Inputs (input_ptr iptr);
void Copy_Flat_to_Complex (context_ptr sub_cir);
void Undo_Complex_Gates (gate_ptr *gpptr, int gnum);
input_ptr Merge_Input_Lists (input_ptr iptr1, input_ptr iptr2);


/*******************************************************/
// Complex gate functions
/*******************************************************/
/*******************************************************/
gate_ptr *Enter_Gate_List (void)
{
  gate_ptr gptr, *gpptr;
  int max_gate, i;

  for (max_gate = 0, gptr = gate_list; gptr != GATE_NULL_PTR; max_gate++, gptr = Gate_Next (gptr));
//    printf ("%s ", Gate_String (gptr));
  gpptr = (gate_ptr *) malloc (sizeof (gate_ptr) * (max_gate  + 1));
  for (i = 0, gptr = gate_list; gptr != GATE_NULL_PTR; i++, gptr = Gate_Next (gptr))
    gpptr[i] = gptr;
  gpptr[i] = GATE_NULL_PTR;
  return (gpptr);   
}

/*******************************************************/
void Set_Fanouts (void) // June 11
{
  int i, j;
  gate_ptr gptr;
  signal_ptr sptr;
  array_t *arr;
  
  for (i = 0; (sptr = signal_array[i]) != NULL; i++) {
    if (Signal_Fanout (sptr) && Signal_Fanout (sptr) != (gate_ptr *) 1)    // April 20
      free (Signal_Fanout (sptr));    
    if (Signal_IO (sptr) != PLACE_NODE) {
      Signal_Fanout (sptr) = (gate_ptr *) array_alloc (gate_ptr, 1);
      Signal_Visited (sptr) = -1;
    }  
  }  
  for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) {
    if (Gate_Type (gptr) == _OUT) // June 11
      continue;
    if (Gate_Env (gptr) && Gate_TRO_T (gptr)) // June 28: Exclude the inputs of gates which drive this subcircuit
      continue;   
    for (j = 0; (sptr = Input_Signal (Gate_Inputs (gptr)[j])) != NULL; j++) 
      if (Signal_Visited (sptr) != i) {
        Signal_Visited (sptr) = i;
        array_insert_last (gate_ptr, (array_t *) Signal_Fanout (sptr), gptr); 
      }
  }    
  for (i = 0; (sptr = signal_array[i]) != NULL; i++) 
    if (Signal_IO (sptr) != PLACE_NODE) {
//      printf ("Fanout gates of signal %s: ", Signal_String (sptr));
//      Print_Array ((array_t *) Signal_Fanout (sptr));
//      printf ("\n");
      arr = (array_t *) Signal_Fanout (sptr);
      Signal_Fanout (sptr) = Array_to_ZArray (arr); 
      array_free (arr);     
    }  
}

/*******************************************************/
void Check_Fanouts (void) 
{
  int i, j;
  gate_ptr gptr, *gpptr;
  signal_ptr sptr;
  
  for (j = 0; (sptr = signal_array[j]) != NULL; j++) {
//    printf ("Fanout list of signal %s: ", Signal_String (sptr));
    gpptr = Signal_Fanout (sptr);
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) 
      printf ("%s ", Gate_String (gptr));
    printf ("\n");
  }
}

/*******************************************************/
bdd_t *Compose_Gates (gate_ptr gptr, bdd_t **bdd)
{
  int j;
  signal_ptr sptr;
  gate_ptr pgptr;
  bdd_t *temp, *temp1, *var;
  array_t *quantify;

  if (Gate_Visited (gptr)) {
/*
    printf ("Internal combinational cycle detected including gate %s\n", Gate_String (gptr));
    printf ("Need to handle this situation\n"); 
*/
    // May need to specify that the output of this gate depends on its current ouput, thus we have a 
    // SPEC element at this node which is not combinational!

    // April 6
    Gate_Visible (gptr) = 1;
    bdd_free (*bdd);
    *bdd = bdd_zero (Bdd_Man);
    Gate_Visited (gptr) = 0;
    if (Cflag) printf ("\n\nGate %s made visible\n\n", Gate_String (gptr));
    return (*bdd);
    exit (-1);
  }
  Gate_Visited (gptr) = 1;  
  for (j = 0; (sptr = Input_Signal(Gate_Inputs(gptr)[j])) != NULL; j++) {
    pgptr = Signal_Parent (sptr);
    if (!(Signal_Type (sptr) == EXTERNAL || 
          Gate_Env (pgptr) || 
          Gate_Visible (pgptr) // This condition added on April 6
          /*&& Gate_TRO_T (Signal_Parent (sptr))*/ // Nov. 5th
          || Gate_Type (pgptr) == _OUT || Signal_SpecFanout (sptr) != NULL) // June 24
          && Gate_Comb (pgptr) ) {
      temp1 = bdd_dup (Gate_Int_F (pgptr));	  
      temp1 = Next_to_Current (temp1, Gate_Int_Out (pgptr));
      var = bdd_get_var (Bdd_Man, Signal_Order (sptr));
      temp = bdd_cofactor (temp1, var);
      bdd_free (temp1);
      temp1 = temp;
      temp = bdd_compose (*bdd, var, temp1);
      bdd_free (*bdd);
      bdd_free (temp1);
//    temp = Exist_Quantify (temp, Gate_Int_Out (pgptr));
          
      if (bdd_equal (temp, bdd_zero (Bdd_Man))) {
        printf ("Zero Function for gate %s\n", Gate_String (pgptr));
        exit (-1);
      }  
          
      *bdd = Compose_Gates (pgptr, &temp);
	  
      // April 6
      if (bdd_equal (*bdd, bdd_zero (Bdd_Man))) {
//      if (!Gate_Visible (pgptr))
//        printf ("Zero Function from a non-visible parent gate!\n");
        break;
      }  
    }
  }  
  Gate_Visited (gptr) = 0;    
  return (*bdd);
}

/*******************************************************/
input_ptr Create_Input_List (bdd_t *bdd, signal_ptr *spptr)
{
  var_set_t *vst;
  array_t  *arr;
  input_ptr iptr;
  int i, j, k;
  signal_ptr sptr;
  
  arr = array_alloc (signal_ptr, 1);
  vst = bdd_get_support (bdd);
  for (i = 0, j = 0; i < flat_cir->_Sig_Num * 3; i++) {
    if (Signal_Next2_Order (order[i]) == i) // April 6: a VVV variable
      continue;
    if (!var_set_get_elt (vst, i))
      continue;
    for (k = 0; (sptr = spptr[k]) != NULL; k++)
      if (order[i] == sptr) 
        break;
    if (sptr == NULL) {  
      array_insert_last (signal_ptr, arr, order[i]);    
      j++;
    }  
  }  
  
  iptr = (input_ptr) malloc (sizeof (input) * (j + 1));
  for (i = 0; i < j; i++) {
    Input_Signal (iptr[i]) = array_fetch (signal_ptr, arr, i);    
    Input_Invert (iptr[i]) = '+';
    if (Cflag) printf ("%s ", Signal_String (Input_Signal (iptr[i])));
  }
  if (Cflag) printf ("\n");
  Input_Signal (iptr[j]) = NULL;
  array_free (arr);
  var_set_free (vst);
  return (iptr);  
}

/*******************************************************/

/*******************************************************/
void Add_Complex_Signals (signal_ptr *spptr)
{
  int i; 
  signal_ptr sptr;

  for (i = 0; (sptr = spptr[i]) != NULL; i++)
    if (Signal_Complex_Fanout (sptr) == NULL) {
      Signal_Complex_Fanout (sptr) = (gate_ptr *) 1;
      complex_cir->_signal_array[complex_cir->_Sig_Num++] = sptr;
      complex_cir->_signal_array[complex_cir->_Sig_Num] = NULL;     
    }  
}

/*******************************************************/
void Add_Complex_Inputs (input_ptr iptr)
{
  int i; 
  signal_ptr sptr;

  for (i = 0; (sptr = Input_Signal (iptr[i])) != NULL; i++)
    if (Signal_Complex_Fanout (sptr) == NULL) {
      Signal_Complex_Fanout (sptr) = (gate_ptr *) 1;
      complex_cir->_signal_array[complex_cir->_Sig_Num++] = sptr;
      complex_cir->_signal_array[complex_cir->_Sig_Num] = NULL;     
    }  
}

/*******************************************************/
void Copy_Flat_to_Complex (context_ptr sub_cir)
{
  strcpy (sub_cir->_Subcir_Path, Subcir_Path);
  sub_cir->_Subcir = Subcir; // bad code w.r.t. object orientation  
  Copy_ZArray (sub_cir->_Sibling_Gates, Sibling_Gates);
//  sub_cir->_external_list = external_list; // external_list is correct only at level 0!!!! Commented on April 20
  sub_cir->_place_array = Place_Array;
  sub_cir->_ext_place_array = Ext_Place_Array;
  sub_cir->_Parent_Subcir = Parent_Subcir;
  sub_cir->_list_of_specs = list_of_specs;
}

/*******************************************************/
void Undo_Complex_Gates (gate_ptr *gpptr, int gnum)
{
  int i;
  gate_ptr gptr;
  signal_ptr *spptr, sptr;
  
  gate_list = NULL;
  complex_cir->_Gate_Num = 0;
  complex_cir->_Sig_Num = 0;
  spptr = complex_cir->_signal_array;
  for (i = 0; (sptr = spptr[i]) != NULL; i++) {
    if (Signal_Complex_Fanout (sptr) && Signal_Complex_Fanout (sptr) != (gate_ptr *) 1)
      free (Signal_Complex_Fanout (sptr));
    Signal_Complex_Fanout (sptr) = NULL; 
//    spptr[i] = NULL;
  }  
  spptr[0] = NULL; // April 20: moved from above
  
  if (gnum >= 0)
    for (i = 0; i <= gnum; i++) {
      if (Gate_Type (gpptr[i]) != _OUT && Gate_Complex_Inputs (gpptr[i]) && Gate_Complex_Inputs (gpptr[i]) != Gate_Inputs (gpptr[i]))
        free (Gate_Complex_Inputs (gpptr[i])); // June 24: added Gate_Type (gptr) != _OUT 
    }
  else
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Type (gptr) != _OUT && Gate_Complex_Inputs (gptr) && Gate_Complex_Inputs (gptr) != Gate_Inputs (gptr))
        free (Gate_Complex_Inputs (gptr)); // June 24: added Gate_Type (gptr) != _OUT
    }
      
}

/*******************************************************/
void Generate_Complex_Gates_NoSpec (void)
{ 
  // needs to be double checked w.r.t. internal combinational outputs which may have TRI_TROs

  int i, j, l, decomposable;
  gate_ptr gptr, pgptr;
  signal_ptr sptr;
  context_ptr temp_cir;
  
  
  printf ("\nXXXX COMPLEX GATE COMPUTATION FOR SUBCIRCUIT %s! XXXX\n", Subcir_Path);
  if (complex_cir)
    free (complex_cir);
  complex_cir = (context_ptr) malloc (sizeof (context));

  Undo_Complex_Gates (list_of_gates, -1);  // April 20
  
  for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) {
    if (Gate_Type (gptr) == _OUT) {
      Gate_Visible (gptr) = 1;
      continue;
    }  
    bdd_free (Gate_Complex_Int_F (gptr)); // April 8
    Gate_Complex_Int_F (gptr) = bdd_zero (Bdd_Man); // April 8: used to be NULL
    Gate_Visible (gptr) = 0;
    
    // June 24
    for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++)
      if (Signal_SpecFanout (sptr) != NULL) 
        break;
    if (sptr != NULL)
      Gate_Visible (gptr) = 1;
        
  }  
 
  l = 0;  
  while (l < 10) {
    l++;
    for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) {

      // June 24
      if (Gate_Type (gptr) == _OUT) {
        Gate_Complex_Type (gptr) = Gate_Type (gptr);
        Add_Complex_Signals (Gate_Outputs (gptr));
        Gate_Next (gptr) = gate_list;
        gate_list = gptr;
        complex_cir->_Gate_Num++;
	if (Cflag) printf ("Complex gate %s\n", Gate_String (gptr));
        continue; 
      }

      if (Gate_Comb (gptr) && Gate_Ext_Out (gptr) != NULL) {
        Add_Complex_Signals (Gate_Ext_Out (gptr));
        Gate_Next (gptr) = gate_list;
        gate_list = gptr;
        complex_cir->_Gate_Num++;
        Gate_Complex_Type (gptr) = _SPEC;
        bdd_free (Gate_Complex_Ext_F (gptr));
        Gate_Complex_Ext_F (gptr) = bdd_dup (Gate_Ext_F (gptr));
	if (Cflag) printf ("Complex gate %s ", Gate_String (gptr));
        if (bdd_equal (Gate_Complex_Ext_F (gptr) = Compose_Gates (gptr, &Gate_Complex_Ext_F (gptr)), bdd_zero (Bdd_Man))) {
          if (Cflag) printf ("\n");
          break;
        }  
	if (Cflag) printf ("Inputs: "); 
        Gate_Complex_Inputs (gptr) = Create_Input_List (Gate_Complex_Ext_F (gptr), Gate_Outputs (gptr)/*[0]*/); // Feb 26
//      Print_Bdd (Gate_Complex_Ext_F (gptr), Gate_String (gptr)); 
      }
      else if (Gate_Comb (gptr) && Gate_Visible (gptr)) {
        Add_Complex_Signals (Gate_Int_Out (gptr));
        Gate_Next (gptr) = gate_list;
        gate_list = gptr;
        complex_cir->_Gate_Num++;
        Gate_Complex_Type (gptr) = _SPEC;
        bdd_free (Gate_Complex_Int_F (gptr));
        Gate_Complex_Int_F (gptr) = bdd_dup (Gate_Int_F (gptr));
        if (Cflag) printf ("Complex gate %s ", Gate_String (gptr));
        Gate_Complex_Inputs (gptr) = NULL;
        if (bdd_equal (Gate_Complex_Int_F (gptr) = Compose_Gates (gptr, &Gate_Complex_Int_F (gptr)), bdd_zero (Bdd_Man))) {
          if (Cflag) printf ("\n");
          break;
        }  
        if (Cflag) printf ("Inputs: "); 
        // NOOOOTTTTIIICCCEEEEd on Feb 26: I'm making the wrong assumption that combinational 
        // gates have only one output!!!
        Gate_Complex_Inputs (gptr) = Create_Input_List (Gate_Complex_Int_F (gptr), Gate_Outputs (gptr)/*[0]*/); // Feb 26
//      Print_Bdd (Gate_Complex_Int_F (gptr), Gate_String (gptr)); 
      }

      else if (!Gate_Comb (gptr) || Gate_Env (gptr)) {
        Gate_Complex_Type (gptr) = Gate_Type (gptr);
        Gate_Complex_Inputs (gptr) = Gate_Inputs (gptr); 
        Add_Complex_Inputs (Gate_Inputs (gptr));
        Add_Complex_Signals (Gate_Outputs (gptr));
        if (Gate_Int_Out (gptr) != NULL) { // sept 30
          bdd_free (Gate_Complex_Int_F (gptr));
          Gate_Complex_Int_F (gptr) = bdd_dup (Gate_Int_F (gptr)); // bdd_dup added sept 30
        }  
        if (Gate_Ext_Out (gptr) != NULL) {// sept 30
          bdd_free (Gate_Complex_Ext_F (gptr));
          Gate_Complex_Ext_F (gptr) = bdd_dup (Gate_Ext_F (gptr)); // bdd_dup added sept 30
        }  
        Gate_Next (gptr) = gate_list;
        gate_list = gptr;
        complex_cir->_Gate_Num++;
        for (j = 0; (sptr = Input_Signal(Gate_Inputs(gptr)[j])) != NULL; j++)      
          if (Signal_Type (sptr) == INTERNAL && Gate_Comb (pgptr = Signal_Parent (sptr)) && !Gate_Visible (pgptr)) 
            if (bdd_equal (Gate_Complex_Int_F (pgptr), bdd_zero (Bdd_Man))) { // No redundancies // Used to be check for Nulity
              Gate_Next (pgptr) = gate_list;
              gate_list = pgptr;
              complex_cir->_Gate_Num++;
              Gate_Complex_Type (pgptr) = _SPEC;
              bdd_free (Gate_Complex_Int_F (pgptr));
    	      Gate_Complex_Int_F (pgptr) = bdd_dup (Gate_Int_F (pgptr));
              if (Cflag) printf ("Complex gate %s ", Gate_String (pgptr));
  	      if (bdd_equal (Gate_Complex_Int_F (pgptr) = Compose_Gates (pgptr, &Gate_Complex_Int_F (pgptr)), bdd_zero (Bdd_Man))) {
  	        if (Cflag) printf ("\n");
	        break;
	      }  
  	      if (Cflag) printf ("Inputs: "); 
              // NOOOOTTTTIIICCCEEEEd on Feb 26: I'm making the wrong assumption that combinational 
	      //  gates have only one output!!!
              Gate_Complex_Inputs (pgptr) = Create_Input_List (Gate_Complex_Int_F (pgptr), Gate_Outputs (pgptr) /*sptr*/); // Feb 26
//    	      Print_Bdd (Gate_Complex_Int_F (pgptr), Gate_String (pgptr)); 
	    }
      }  	      
    }
    if (gptr != NULL) { // combinational cycle detected, whatever has been done need to be undone
      Undo_Complex_Gates (list_of_gates, i);
      for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) {
        bdd_free (Gate_Complex_Int_F (gptr));
        Gate_Complex_Int_F (gptr) = bdd_zero (Bdd_Man);      
      }  
    
    } else
      break;
  } // while
  if (l == 10) {
    printf ("Convergence problem in Generate_Complex_Gates_Spec\n");
    exit (-1);
  } 
  
  complex_cir->_list_of_gates = Enter_Gate_List ();
  complex_cir->_All_Externals = All_Externals;
/*
  complex_cir->_Internal_Num = complex_cir->_Sig_Num - External_Num; // ??
  complex_cir->_External_Num = External_Num; // ??
  complex_cir->_external_list = external_list; // ??
  // What about signal_array? It is used in Set_Fanouts!!!
*/
  Copy_Flat_to_Complex (complex_cir);
  gate_list = NULL;
  temp_cir = cur_cir;
  cur_cir = complex_cir;
//printf ("gate Gate_Num = %d, Sig_Num = %d\n", Gate_Num, Sig_Num);  
  MODE = COMPLEX;
  Set_Fanouts ();
  if (!tflag && !lflag) 
    NoSort (list_of_gates);
  else  
    TopSort_Internals (list_of_gates);
  if (lflag)  
    Levelize_Internals (DFS_List);
//  printf ("Complex gate Gate_Num = %d, Sig_Num = %d\n", Gate_Num, Sig_Num);  
  cur_cir = temp_cir;
  MODE = FLAT;
  printf ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
}

/*******************************************************/
input_ptr Merge_Input_Lists (input_ptr iptr1, input_ptr iptr2)
{
// Only iptr2 is freed

  array_t  *arr;
  input_ptr iptr;
  int i, j, k;
  input     ip;

  if (iptr1 == NULL)
    return (iptr2);
  arr = array_alloc (signal_ptr, 1);
  for (i = 0; Input_Signal (iptr1[i]) != NULL; i++) 
    array_insert_last (signal_ptr, arr, Input_Signal (iptr1[i])); 
  for (k = i, i = 0; Input_Signal (iptr2[i]) != NULL; i++) {
    for (j = 0; Input_Signal (iptr1[j]) != NULL; j++)
      if (Input_Signal (iptr2[i]) == Input_Signal (iptr1[j]))
        break;
    if (Input_Signal (iptr1[j]) == NULL) {
      array_insert_last (signal_ptr, arr, Input_Signal (iptr2[i]));
      k++;
    }  
  }     
  iptr = (input_ptr) malloc (sizeof (input) * (k + 1));
  for (i = 0; i < k; i++) {
    Input_Signal (iptr[i]) = array_fetch (signal_ptr, arr, i);    
    Input_Invert (iptr[i]) = '+';
//  printf ("%s ", Signal_String (Input_Signal (iptr[i])));
  }
//  printf ("\n");
  Input_Signal (iptr[j]) = NULL;
  array_free (arr);
  free (iptr1); // April 20. This line may cause problem
  free (iptr2);
  return (iptr);  
}

/*******************************************************/
void Generate_Complex_Gates_Spec (void)
{
  int i, j, l;
  gate_ptr gptr, pgptr;
  signal_ptr sptr;
  context_ptr temp_cir;
  char ch;

// To simplify matters now: As soon as a combinational cycle is detected, the outputs of the 
// corresponding gate become visible w.r.t complex gate generation, and the generation is
// undone and then redone with the new information.  
  
  // April 6. All internal combinational gates are taken to be non-visible. They may become visible
  // when breaking combinational cycles.

  printf ("\nXXXX COMPLEX GATE COMPUTATION FOR SUBCIRCUIT %s! XXXX\n", Subcir_Path);
  for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) {
    if (Gate_Type (gptr) == _OUT) {
      Gate_Visible (gptr) = 1;
      continue;
    }  
    bdd_free (Gate_Complex_Int_F (gptr)); // April 8
    Gate_Complex_Int_F (gptr) = bdd_zero (Bdd_Man); // April 8: used to be NULL
    Gate_Visible (gptr) = 0;
    
    // June 24
    for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++)
      if (Signal_SpecFanout (sptr) != NULL) 
        break;
    if (sptr != NULL)
      Gate_Visible (gptr) = 1;        
  }  
 
/* // June 24
  for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) {
      Gate_Visible (gptr) = 0;
      printf ("Do you want to make gate %s visible? (y/n)", Gate_String (gptr));
      while ((ch = tolower (getchar ())) != 'y' && ch != 'n');
      if (ch == 'y')
        Gate_Visible (gptr) = 1;
    }    
*/
       
  if (complex_cir)
    free (complex_cir);
  complex_cir = (context_ptr) malloc (sizeof (context));

  Undo_Complex_Gates (list_of_gates, -1);

  l = 0;
  while (l < 10) {
    l++;
    for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) {

    // June 24
      if (Gate_Type (gptr) == _OUT) {
        Gate_Complex_Type (gptr) = Gate_Type (gptr);
        Add_Complex_Signals (Gate_Outputs (gptr));
        Gate_Next (gptr) = gate_list;
        gate_list = gptr;
        complex_cir->_Gate_Num++;
	if (Cflag) printf ("Complex gate %s\n", Gate_String (gptr));
	continue; 
      }

      // if a gate is Gate_Env, then even if it is not external, the behavior of other Env signals
      // may depend on it, thus we can not hide its outputs via the complex gates
      if (Gate_Comb (gptr) && (Gate_Ext_Out (gptr) != NULL)) { 
        Add_Complex_Signals (Gate_Ext_Out (gptr));
        Gate_Next (gptr) = gate_list;
        gate_list = gptr;
        complex_cir->_Gate_Num++;
        Gate_Complex_Type (gptr) = _SPEC;
        bdd_free (Gate_Complex_Ext_F (gptr));
        Gate_Complex_Ext_F (gptr) = bdd_dup (Gate_Ext_F (gptr));
        if (Cflag) printf ("Complex gate %s ", Gate_String (gptr));
        Gate_Complex_Inputs (gptr) = NULL;
        if (bdd_equal (Gate_Complex_Ext_F (gptr) = Compose_Gates (gptr, &Gate_Complex_Ext_F (gptr)), bdd_zero (Bdd_Man))) {
          if (Cflag) printf ("\n");
          break;
	}
        if (Cflag) printf ("Inputs: "); 
        // NOOOOTTTTIIICCCEEEEd on Feb 26: I'm making the wrong assumption that combinational 
        // gates have only one output!!!
        Gate_Complex_Inputs (gptr) = Create_Input_List (Gate_Complex_Ext_F (gptr), Gate_Outputs (gptr)/*[0]*/); // Feb 26
//      Print_Bdd (Gate_Complex_Ext_F (gptr), Gate_String (gptr)); 
      }

      // April 6th: to take care of combinational cycles which are broken by making internal gates visible!
      else if (Gate_Comb (gptr) && Gate_Visible (gptr)) { // internal visible combinational gate
        Add_Complex_Signals (Gate_Int_Out (gptr));
        Gate_Next (gptr) = gate_list;
        gate_list = gptr;
        complex_cir->_Gate_Num++;
        Gate_Complex_Type (gptr) = _SPEC;
        bdd_free (Gate_Complex_Int_F (gptr));
        Gate_Complex_Int_F (gptr) = bdd_dup (Gate_Int_F (gptr));
        if (Cflag) printf ("Complex gate %s ", Gate_String (gptr));
        Gate_Complex_Inputs (gptr) = NULL;
        if (bdd_equal (Gate_Complex_Int_F (gptr) = Compose_Gates (gptr, &Gate_Complex_Int_F (gptr)), bdd_zero (Bdd_Man))) {
          if (Cflag) printf ("\n");
          break;
        }  
        if (Cflag) printf ("Inputs: "); 
        // NOOOOTTTTIIICCCEEEEd on Feb 26: I'm making the wrong assumption that combinational 
        // gates have only one output!!!
        Gate_Complex_Inputs (gptr) = Create_Input_List (Gate_Complex_Int_F (gptr), Gate_Outputs (gptr)/*[0]*/); // Feb 26
//      Print_Bdd (Gate_Complex_Int_F (gptr), Gate_String (gptr)); 
      }

      // April 8: I am assuming that we always include the outputs and inputs of an SPECification,
      // i.e., the inputs of a _STATE and outputs of _OUT gates into the set of external signals
      // Below, the outputs of both of them are put into the set of visible signals of this context.

      else if (!Gate_Comb (gptr) || Gate_Env (gptr)) { // Gate_Env added on Nov. 5
        // We must make sure that if the _OUT gates are included, the outputs of the _STATE which are the inputs of the
        // _OUT all are visible. This must be true especially if _OUT is not just an input to a subcir, i.e., it is 
        // internal or output.
        Gate_Complex_Type (gptr) = Gate_Type (gptr);
        if (Gate_Type (gptr) == _OUT) // April 8
          Add_Complex_Inputs (Gate_Inputs (gptr));
        Add_Complex_Signals (Gate_Outputs (gptr));
        Gate_Next (gptr) = gate_list;
        gate_list = gptr;
        complex_cir->_Gate_Num++;
        Gate_Complex_Inputs (gptr) = NULL;
        if (Gate_Ext_Out (gptr) != NULL) {
          bdd_free (Gate_Complex_Ext_F (gptr));
          Gate_Complex_Ext_F (gptr) = bdd_dup (Gate_Ext_F (gptr));
          if (Cflag) printf ("Complex gate %s ", Gate_String (gptr));
          if (bdd_equal (Gate_Complex_Ext_F (gptr) = Compose_Gates (gptr, &Gate_Complex_Ext_F (gptr)), bdd_zero (Bdd_Man))) {
            if (Cflag) printf ("\n");
            break;
          }  
          if (Cflag) printf ("Inputs: "); 
          Gate_Complex_Inputs (gptr) = Create_Input_List (Gate_Complex_Ext_F (gptr), Gate_Outputs (gptr) /*[0]*/); // Feb 26
//        Print_Bdd (Gate_Complex_Ext_F (gptr), Gate_String (gptr));
        }
        if (Gate_Int_Out (gptr) != NULL) {
          bdd_free (Gate_Complex_Int_F (gptr));
          Gate_Complex_Int_F (gptr) = bdd_dup (Gate_Int_F (gptr));
          if (Cflag) printf ("Complex gate %s ", Gate_String (gptr));
          if (bdd_equal (Gate_Complex_Int_F (gptr) = Compose_Gates (gptr, &Gate_Complex_Int_F (gptr)), bdd_zero (Bdd_Man))) {
            if (Cflag) printf ("\n");
            break;
          }  
          if (Cflag) printf ("Inputs: "); 
          Gate_Complex_Inputs (gptr) = Merge_Input_Lists (Gate_Complex_Inputs (gptr), // Feb 26
      					  Create_Input_List (Gate_Complex_Int_F (gptr), Gate_Outputs (gptr)/*[0]*/));
 
        }
      }  	      
    } // for
    if (gptr != NULL) { // combinational cycle detected, whatever has been done need to be undone
      Undo_Complex_Gates (list_of_gates, i);
    } else
      break;
  } // while 
  if (l == 10) {
    printf ("Convergence problem in Generate_Complex_Gates_Spec\n");
    exit (-1);
  } 
  
  for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) {
    if (Gate_Visited (gptr)) {
      printf ("%s is wrongly Visited\n", Gate_String (gptr));
      exit (-1);
    }
    if (Gate_Visible (gptr)) {
      if (Cflag) printf ("%s is visible\n", Gate_String (gptr));
    }
  }    
   
  complex_cir->_list_of_gates = Enter_Gate_List ();
  complex_cir->_All_Externals = All_Externals;
/*
  complex_cir->_Internal_Num = complex_cir->_Sig_Num - External_Num; // ??
  complex_cir->_External_Num = External_Num; // ??
  complex_cir->_external_list = external_list; // ??
  // What about signal_array? It is used in Set_Fanouts!!!  
//  printf ("Gate_Num = %d, Sig_Num = %d\n", Gate_Num, Sig_Num);
*/
  Copy_Flat_to_Complex (complex_cir);
  gate_list = NULL;
  temp_cir = cur_cir;  
  cur_cir = complex_cir;
  MODE = COMPLEX;
  Set_Fanouts ();
  if (!tflag && !lflag) 
    NoSort (list_of_gates);
  else  
    TopSort_Internals (list_of_gates);
  if (lflag)  
    Levelize_Internals (DFS_List);
//  printf ("Complex gate Gate_Num = %d, Complex Sig_Num = %d\n", Gate_Num, Sig_Num);  
  cur_cir = temp_cir;
  MODE = FLAT;
  printf ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
}
