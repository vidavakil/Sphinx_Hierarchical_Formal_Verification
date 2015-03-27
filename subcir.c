#include "type.h"
#include "sphinx.h"

int		Cur_Level = 0;  
signal_ptr      *Subcir_In, *Subcir_Out, *Subcir_Internal;
reached_ptr	Reached_top = NULL;

extern int	Level_Num;  
extern int	lflag, tflag, xflag, sflag /*, wflag */;
extern signal_ptr	*External_Signals_of_Level[];
extern gate_ptr	gate_list;
extern bdd_t 	*From, *To, *Reached;
extern bdd_manager	*Bdd_Man;
extern int		MODE;
extern context_ptr	cur_cir, complex_cir, flat_cir;
extern int 	Max_External; 
extern signal_ptr	order[]; 
extern bdd_t 		*R;

int Verify_Subcir (subcir_ptr subptr);
void Make_Internal (gate_ptr gptr);
void Make_Gate_External (gate_ptr gptr);
int Find_Subcir_Externals (void);
void Push_R (bdd_t *R);
void Pop_R (void);
void Push_Subcir (gate_ptr gptr, subcir_ptr subptr, int mode);
void Pop_Subcir (gate_ptr gptr);
void Push_Status (stat_ptr *top, io_type io);
void Pop_Status (stat_ptr *top);
int Add_New_Externals (void);
int Verify_Flat_Hierarchical (context_ptr cptr);
void Set_TRO (gate_ptr *gpptr);
gate_ptr *Find_Siblings (subcir_ptr subptr, gate_ptr *gpptr);
void PopIO (gate_ptr *gpptr);
void Mark_Pushed (gate_ptr *gpptr);
context_ptr New_Context (char *str);
int Create_Support_Changing_Lists (input_ptr iptr, gate_ptr *gate_list, gate_ptr *sible_list);
gate_ptr *Find_Specs (gate_ptr *gate_list, gate_ptr *sib_list);
void *Pop_Specs (gate_ptr *gate_list, gate_ptr *sib_list);
void Free_External_Gates (gate_ptr *gpptr);
void Pop_Input (input_ptr iptr);
gate_ptr *Create_Env_Old (void);
void Create_Cones (void);
gate_ptr *Create_Env (void);
void Remove_Env (void);
void Free_Subcircuit (subcir_ptr subptr);
void Print_Subcir (subcir_ptr subptr);
void Print_Context (context_ptr cptr, int mode);
void Add_Subcir_Component (subcir_ptr subptr, signal_ptr ptr, array_t **comp_array);
void Add_Support_Gates (gate_ptr gptr, subcir_ptr subptr);
void Find_Transitive_Inputs (gate_ptr gptr1, subcir_ptr subptr);
void Find_Transitive_Outputs (gate_ptr gptr, subcir_ptr subptr);
void Identify_Outputs (gate_ptr gptr); 
void Identify_Places (gate_ptr gptr); 
void Assign_Gate_Outputs (void);
gate_ptr *Just_Find_Gates (signal_ptr *sptr); // This function must only be used in connection with subcircuits
gate_ptr *Find_Out_Gates (signal_ptr *sptr); // This function must only be used in connection with subcircuits
gate_ptr *Find_In_Gates (signal_ptr *sptr); // This function must only be used in connection with subcircuits
void ResetIO (gate_ptr *gptrx);
void SetTRO (gate_ptr *gptrx);
void Mirror (gate_ptr *gptrx, int mode); // 0 = SIBLING, 1 = NonSIBLING
void Free_TRFROMs (gate_ptr *gpptr);
void Find_External_Places (void); 

/*******************************************************/
/*******************************************************/
// Push/Pop Functions
/*******************************************************/
void Push_R (bdd_t *R)
{  
  reached_ptr rptr;

  rptr = malloc (sizeof (reached_bdd));
  Reached_Next (rptr) = Reached_top;
  Reached_Bdd (rptr) = R;
  Reached_top = rptr;
  Reached = R;
}

/*******************************************************/
void Pop_R (void)
{
  reached_ptr rptr;
  
  rptr = Reached_top;
  Reached_top = Reached_Next (rptr);
  bdd_free (Reached_Bdd (rptr)); ///???????
  free (rptr);
  if (Reached_top)
    Reached = Reached_Bdd (Reached_top);
}

/*******************************************************/
void Push_Subcir (gate_ptr gptr, subcir_ptr subptr, int mode)
{
  subcir_chain_ptr scptr;
  
  scptr = malloc (sizeof (subcir_chain));
  SubChain_Subcir (scptr) = subptr;
  SubChain_Next (scptr) = Gate_Subcircuit (gptr);
  Gate_Subcircuit (gptr) = scptr;
  if (mode)
    Gate_HLevel (gptr) = Cur_Level + 1;
}

/*******************************************************/
void Pop_Subcir (gate_ptr gptr)
{
  subcir_chain_ptr scptr;
  
  scptr = Gate_Subcircuit (gptr);
  Gate_Subcircuit (gptr) = SubChain_Next (scptr);
  free (scptr);
}

/*******************************************************/
void Push_Status (stat_ptr *top, io_type io)
{
  stat_ptr sptr;
  
  sptr = malloc (sizeof (status));
  Status_Next (sptr) = *top;
  Status_IO (sptr) = io;
  *top = sptr;
}

/*******************************************************/
void Pop_Status (stat_ptr *top)
{
  stat_ptr sptr;
    
  sptr = *top;
  *top = Status_Next (sptr);
  free (sptr);
}

/*******************************************************/
pushed_bdd_ptr Push_Bdd (pushed_bdd_ptr pptr1, bdd_t *R)
{  
  pushed_bdd_ptr pptr;

  pptr = malloc (sizeof (pushed_bdd));
  Pushed_Bdd_Next (pptr) = pptr1;
  Pushed_Bdd_Bdd (pptr) = R;
  return (pptr);
}

/*******************************************************/
pushed_bdd_ptr Pop_Bdd (pushed_bdd_ptr pptr1)
{
  pushed_bdd_ptr pptr;
  
  pptr = Pushed_Bdd_Next (pptr1);
  bdd_free (Pushed_Bdd_Bdd (pptr1)); ///???????
  free (pptr1);
  return (pptr);
}

/*******************************************************/
void Push (rec_ptr *top, bdd_t *bdd, gate_ptr *gpptr, int type)
{
  rec_ptr rptr;
  
  rptr = malloc (sizeof (record));
  Record_Next (rptr) = *top;
  Record_Type (rptr) = type;
  Record_Bdd (rptr) = bdd;
  Record_IO (rptr) = NOTR;
  Record_Support (rptr) = gpptr;
  *top = rptr;
}

/*******************************************************/
bdd_t *Pop (rec_ptr *top)
{
  rec_ptr rptr;
  bdd_t *bdd; // Any pointer type will do!
  
  rptr = *top;
  *top = Record_Next (rptr);
  bdd = Record_Bdd (rptr);
  free (rptr);
  return (bdd);
}

/*******************************************************/
// Subcir related functions
/*******************************************************/
void Print_Subcir (subcir_ptr subptr)
{
    printf ("\nStructural information of subcircuit %s\n", Subcir_String (subptr));
    printf ("Input Signals: ");
    Print_ZArray ((void_ptr *) Subcir_Inputs (subptr), SIGNAL);
    printf ("Output Signals: ");
    Print_ZArray ((void_ptr *) Subcir_Outputs (subptr), SIGNAL);
    printf ("Non-IO Signals: ");
    Print_ZArray ((void_ptr *) Subcir_Internals (subptr), SIGNAL);
    printf ("All Gates: ");
    Print_ZArray ((void_ptr *) Subcir_Gates (subptr), GATE);
    printf ("\n");
}

/*******************************************************/
void Print_Context (context_ptr cptr, int mode)
{
    printf ("\nAbstractional information of %s subcircuit %s\n", 
    	    (mode == FLAT) ? "non-complex" : "complex", cptr->_Subcir_Path);
    printf ("Internal Gates: ");
    Print_ZArray ((void_ptr *) cptr->_DFS_List, GATE);
    printf ("External Gates: ");
    Print_ZArray ((void_ptr *) cptr->_Ext_Gates, GATE);
    printf ("Sibling Gates: ");
    Print_ZArray ((void_ptr *) cptr->_Sibling_Gates, GATE);
    printf ("Spec Gates: ");
    Print_ZArray ((void_ptr *) cptr->_list_of_specs, GATE);
    printf ("Spec Variables: ");
    Print_Array (cptr->_place_array);
    printf ("External Spec Variables: ");
    Print_Array (cptr->_ext_place_array);
    printf ("\n");  
}
/*******************************************************/
subcir_ptr New_Subcircuit (char *name)
{
  name_ptr nptr;
  subcir_ptr sub_ptr;
	
  nptr = Name_Enter (name, SUBCIR);
  sub_ptr = (subcir_ptr) malloc (sizeof (subcircuit));
  Subcir_Name (sub_ptr) = nptr;
  Subcir_Internals (sub_ptr) = NULL;
  Subcir_Inputs (sub_ptr) = NULL;
  Subcir_Outputs (sub_ptr) = NULL;
  Subcir_InternalsArray (sub_ptr) = NULL;
  Subcir_InputsArray (sub_ptr) = NULL;
  Subcir_OutputsArray (sub_ptr) = NULL;
  Subcir_GatesArray (sub_ptr) = NULL;
  Subcir_SiblingsArray (sub_ptr) = NULL;
  Subcir_Gates (sub_ptr) = NULL;
  Subcir_Siblings (sub_ptr) = NULL;
  Subcir_Next (sub_ptr) = NULL;
  return (sub_ptr);
}

/*******************************************************/
void Add_Subcir_Component (subcir_ptr subptr, signal_ptr ptr, array_t **comp_array)
{
  if (*comp_array == NULL)
    *comp_array = array_alloc (signal_ptr, 1);
  array_insert_last (signal_ptr, (*comp_array), ptr);  // Just any pointer data
}


/*******************************************************/
void Add_Support_Gates (gate_ptr gptr, subcir_ptr subptr)
{
  gate_ptr *gpptr, tempg;
  int i;
  
// Notice: This way, the list Subcir_GatesArray will contain gates not necessarily within
// this subcircuit, some gates may just be related (through env-gates) to the gates of 
// this subcircuit
// Notice: However; list_of_gates should contain only the gates within a subcircuit!

  gpptr = Gate_Support (gptr);
  for (i = 0; (tempg = gpptr[i]) != NULL; i++) {
    if (!Is_Array_Member (tempg, (array_t *) Subcir_SiblingsArray (subptr))) {
      Add_Subcir_Component (subptr, (signal_ptr) tempg, (array_t **) &Subcir_SiblingsArray (subptr));
      
      // May 28: Notice! If tempg is a _STATE gate, then it does not have any Support Set! 
      // Thus it needs to be excluded from this test!
//      if (Gate_Support (tempg) != gpptr) // May 28
      if (Gate_Type (tempg) != _STATE && Gate_Support (tempg) != gpptr) // newly brought in in the if part
        Add_Support_Gates (tempg, subptr);
    }    
  }
}

/*******************************************************/
void Find_Transitive_Inputs (gate_ptr gptr1, subcir_ptr subptr)
{
  int j;
  signal_ptr sptr, sptro;
  gate_ptr pgptr, gptr;
  
  /* This does not handle partitions where a gate can belong to multiple blocks */
  
  if (Gate_Subcir (gptr1) == subptr)
    return;

  if (Gate_Subcir (gptr1) != Subcir) { // Subcir was NULL
    printf ("E is not an appropriate partition! Gate %s belongs to multiple subcircuits %s & %s !\n", 
    		Gate_String (gptr1), Gate_Subcir (gptr1) ? Subcir_String (Gate_Subcir (gptr1)) : "", 
    				     Subcir_Path /*Subcir ? Subcir_String (Subcir) : ""*/);
    exit (-1);
  }
  Push_Subcir (gptr1, subptr, 1); // Only internal gates!
//  printf ("%s ", Gate_String (gptr1)); // June 29

//    printf ("Gate %s assigned to this subcir\n", Gate_String (gptr1));
  if (!Is_Array_Member (gptr1, Subcir_GatesArray (subptr)))  // April 29: This condition will always be satisfied
    Add_Subcir_Component (subptr, (signal_ptr) gptr1, (array_t **) &Subcir_GatesArray (subptr));          

// June 15
  if (Gate_Type (gptr1) == _OUT) { // then, it will not have any Input_Signal!, skip back over the _STATE gate 
    gptr = Gate_Uncle (gptr1);
    if (Gate_State_Ext_Places (gptr) != NULL)
      gptr = NULL;
  }
  else 
    gptr = gptr1;
  if (gptr) {
  for (j = 0; (sptr = Input_Signal (Gate_Inputs(gptr)[j])) != NULL; j++) {
    pgptr = Signal_Parent (sptr);
    if (Signal_Type (sptr) == EXTERNAL || 
        Gate_Env (pgptr) && Gate_TRO_T (pgptr) ) {
      Add_Subcir_Component (subptr, sptr, (array_t **) &Subcir_InputsArray (subptr));

      // new! Do we really need to check for presence?
      if (!Is_Array_Member (pgptr, Subcir_GatesArray (subptr))) {
        Add_Subcir_Component (subptr, (signal_ptr) pgptr, (array_t **) &Subcir_GatesArray (subptr));
//      printf ("gate %s made Input\n", Gate_String (pgptr));  
      }
    } else if (!Gate_Env (pgptr)) {
      Add_Subcir_Component (subptr, sptr, (array_t **) &Subcir_InternalsArray (subptr));
      Find_Transitive_Inputs (pgptr, subptr);  
//      printf ("gate %s made Internal and Find_Transitive_Input called\n", Gate_String (pgptr));  
    } else if (Gate_Env (pgptr) && Gate_TRIO_T (pgptr) ) {
//      printf ("gate %s made Input and Find_Transitive_Input not called\n", Gate_String (pgptr));  
      Add_Subcir_Component (subptr, sptr, (array_t **) &Subcir_InputsArray (subptr));
      // not all outputs of this gate return to this block! 
      // When Find_Transitive_Input is called for this gate, eventually Find_Transitive_Outputs will 
      // also be called, while not all outputs of this gate are in this block
      Find_Transitive_Inputs (pgptr, subptr);        
    }
  } 
  }
  if (gptr == NULL)
    gptr = gptr1; 
  if (Gate_Type (gptr) != _STATE)
    Find_Transitive_Outputs (gptr, subptr);  
  else {
    for (j = 0; (gptr1 = Gate_Niece (gptr)[j]) != NULL; j++)
      Find_Transitive_Inputs (gptr1, subptr);
  }     

}

/*******************************************************/
void Find_Transitive_Outputs (gate_ptr gptr, subcir_ptr subptr)
{
  int j, i;
  signal_ptr sptr;
  gate_ptr cgptr;
  
  /* This does not handle partitions where a gate can belong to multiple blocks */

// must work on the fanout gates, not on the parent of the fanout signals which are the same gate!!!!

  for (j = 0; Gate_Outputs(gptr)[j] != NULL; j++) {
    sptr = Gate_Outputs (gptr)[j];
    if (Signal_Type (sptr) == EXTERNAL || Gate_Env (Signal_Parent (sptr)) && Gate_TRI_T (Signal_Parent (sptr)) ) {
      // April 29: Since Find_Transitive_Outputs is always called by Find_Transitive_Inputs, 
      // the Subcir of this gate has already been set to subptr!
      Add_Subcir_Component (subptr, sptr, &Subcir_OutputsArray (subptr));        
//      printf ("gate %s made output\n", Gate_String (Signal_Parent (sptr)));

    } else if (Gate_Env (Signal_Parent (sptr)) && Gate_TRIO_T (Signal_Parent (sptr)) ) {
      Add_Subcir_Component (subptr, sptr, &Subcir_OutputsArray (subptr)); 
//      printf ("gate %s made output and Find_Transitive_Inputs conditionally called\n", Gate_String (Signal_Parent (sptr)));  
      for (i = 0; Signal_Fanout (sptr) [i] != NULL; i++) // not all outputs of this gate return to this block! April 29: ???????????????
        if (Gate_Subcir (Signal_Fanout (sptr) [i]) != subptr) // April 30: was == Subcir
          Find_Transitive_Inputs (Signal_Fanout (sptr) [i], subptr);  
    } else if (!Gate_Env (Signal_Parent (sptr))) {
//      printf ("gate %s: Transitive Input called\n", Gate_String (Signal_Parent (sptr)));  
      for (i = 0; Signal_Fanout (sptr) [i] != NULL; i++) 
        Find_Transitive_Inputs (Signal_Fanout (sptr) [i], subptr);  
    }  
  }      
}

/*******************************************************/
void Identify_Outputs (gate_ptr gptr) 
{
  int i;
  array_t *int_signals, *ext_signals;
  signal_ptr sptr;
  
  int_signals = array_alloc (signal_ptr, 1);
  ext_signals = array_alloc (signal_ptr, 1);
  for (i = 0; (sptr = Gate_Outputs (gptr)[i]) != NULL; i++) {
    if (Signal_Type (sptr) == EXTERNAL) 
      array_insert_last (signal_ptr, ext_signals, sptr);
    if (Signal_Type (sptr) == INTERNAL) 
      array_insert_last (signal_ptr, int_signals, sptr);
  }

  if (array_n (int_signals) != 0) 
    Gate_Int_Out (gptr) = Array_to_ZArray (int_signals);
  if (array_n (ext_signals) != 0) 
    Gate_Ext_Out (gptr) = Array_to_ZArray (ext_signals);
  array_free (int_signals);
  array_free (ext_signals);
}

/*******************************************************/
void Identify_Places (gate_ptr gptr) 
{
  int i;
  array_t *int_signals, *ext_signals;
  signal_ptr sptr;
  
  int_signals = array_alloc (signal_ptr, 1);
  ext_signals = array_alloc (signal_ptr, 1);
  for (i = 0; (sptr = Gate_State_All_Places (gptr)[i]) != NULL; i++) {
    if (Signal_Type (sptr) == EXTERNAL) 
      array_insert_last (signal_ptr, ext_signals, sptr);
    if (Signal_Type (sptr) == INTERNAL) 
      array_insert_last (signal_ptr, int_signals, sptr);
  }
  if (array_n (int_signals) != 0)
    Gate_State_Int_Places (gptr) = Array_to_ZArray (int_signals);
  else
    Gate_State_Int_Places (gptr) = NULL;

  if (array_n (ext_signals) != 0)
    Gate_State_Ext_Places (gptr) = Array_to_ZArray (ext_signals);
  else
    Gate_State_Ext_Places (gptr) = NULL;

  array_free (int_signals);
  array_free (ext_signals);
}

/*******************************************************/
void Assign_Gate_Outputs (void)
{
  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = list_of_gates [i]) != NULL; i++) 
    Identify_Outputs (gptr); 
  for (i = 0; (gptr = list_of_specs [i]) != NULL; i++) 
    Identify_Places (gptr);  
}

/*******************************************************/
/*******************************************************/
subcir_ptr Create_Subcircuit (gate_ptr gptr)
{
  char str[100];
  subcir_ptr subptr;
  int i;
  gate_ptr gptr1;
  
  if (Gate_HLevel (gptr) == Cur_Level+1)
    return (NULL);
      
  sprintf (str, "%s/%d", Subcir_Path, Subcir_Num);
  printf ("\n>>>> GENERATING SUBCIRCUIT %s FOR VERIFICATION >>>>\n", str);
  printf ("Seed gate taken as %s\n", Gate_String (gptr));
//  printf ("The subcir of the following gates were pushed: ");
  subptr = New_Subcircuit (str);
  Find_Transitive_Inputs (gptr, subptr);
//  printf ("\n");
  Subcir_Inputs (subptr) = (signal_ptr *) Array_to_ZArray_Free (Subcir_InputsArray (subptr));
  Subcir_Outputs (subptr) = (signal_ptr *) Array_to_ZArray_Free (Subcir_OutputsArray (subptr));
  Subcir_Internals (subptr) = (signal_ptr *) Array_to_ZArray_Free (Subcir_InternalsArray (subptr));
  Subcir_Gates (subptr) = (gate_ptr *) Array_to_ZArray_Free (Subcir_GatesArray (subptr));  // this is just internal gates!!
  for (i = 0; (gptr1 = Subcir_Gates (subptr)[i]) != NULL; i++)
    if (Gate_Subcir (gptr1) != subptr)
      Push_Subcir (gptr1, subptr, 1);
  subcir_list [Subcir_Num++] = subptr;
  
  return (subptr);
}


/*******************************************************/
bdd_t *From_to_To (gate_ptr gptr, sig_type Int_Ext, bdd_t *temp0, signal_ptr *spptr1, int ret) /* fixed */
{
  // The input temp0 is no more useable!
  
  bdd_t  *temp1;
  signal_ptr *spptr2;

  if (!bdd_equal (bdd_zero (Bdd_Man), temp0)) {
    if (Int_Ext == EXTERNAL) {
      temp1 = bdd_or (Gate_Ext_TR (gptr), temp0, 1, 1);
      bdd_free (Gate_Ext_TR (gptr));
      Gate_Ext_TR (gptr) = temp1;
      spptr2 = Gate_Ext_Out (gptr);  
    } else {
      temp1 = bdd_or (Gate_Int_TR (gptr), temp0, 1, 1);
      bdd_free (Gate_Int_TR (gptr));
      Gate_Int_TR (gptr) = temp1;  
      spptr2 = Gate_Int_Out (gptr);  
    }  
    temp0 = Next_to_Current (temp0, spptr1);
    temp0 = Next_to_Current (temp0, spptr2);  

    temp1 = bdd_or (To, temp0, 1, 1);
    bdd_free (To);
    To = temp1;
  } // if  
  
/*
  // May 17: To implement Chaining!
  temp1 = bdd_or (From, temp0, 1, 1);
  bdd_free (From);
  From = temp1;
*/
  
  if (!ret)
    bdd_free (temp0);
  return (temp0);  
}

/*******************************************************/
gate_ptr *Just_Find_Gates (signal_ptr *sptr) // This function must only be used in connection with subcircuits
{
  array_t *gpptr;
  gate_ptr gptr;
  int i;

  gpptr = array_alloc (gate_ptr, 1);  
  for (i = 0; sptr[i] != NULL; i++) {
    gptr = Signal_Parent (sptr[i]); 
    array_insert_last (gate_ptr, gpptr, gptr);
  }
  return ((gate_ptr *) Array_to_ZArray_Free (gpptr));   // April 19: added free
}

/*******************************************************/
gate_ptr *Find_Out_Gates (signal_ptr *sptr) // This function must only be used in connection with subcircuits
{
  array_t *gpptr;
  gate_ptr gptr;
  int i;

  gpptr = array_alloc (gate_ptr, 1);  
  for (i = 0; sptr[i] != NULL; i++) {
    gptr = Signal_Parent (sptr[i]);
    //if (Gate_Ext_Out (gptr) != NULL) {
    if (/*Signal_Type (sptr[i]) == EXTERNAL || */Gate_Env (Signal_Parent (sptr[i])) != NULL) {
      Gate_TRI (gptr) = Gate_TRI_TRO (gptr);
      Gate_Set_TRI (gptr);
//      printf ("Gate %s made TRI\n", Gate_String (gptr));   
    } 
    array_insert_last (gate_ptr, gpptr, gptr);
  }
  return ((gate_ptr *) Array_to_ZArray_Free (gpptr));  // free added April 19
}

/*******************************************************/
gate_ptr *Find_In_Gates (signal_ptr *sptr) // This function must only be used in connection with subcircuits
{
  array_t *gpptr;
  gate_ptr gptr;
  int i;

  gpptr = array_alloc (gate_ptr, 1);  
  for (i = 0; sptr[i] != NULL; i++) {
    gptr = Signal_Parent (sptr[i]);
    //if (Gate_Ext_Out (gptr) != NULL) {
    if (/*Signal_Type (sptr[i]) == EXTERNAL || */ Gate_Env (Signal_Parent (sptr[i])) != NULL) {
      Gate_TRI (gptr) = Gate_TRI_TRO (gptr);
      if (Gate_TR_NONE (gptr)) { 
        Gate_Set_TRO (gptr);
//        printf ("Gate %s made TRO\n", Gate_String (gptr));
      }  
      else if (Gate_TRI_T (gptr)) {
//        printf ("Gate %s made TRIO\n", Gate_String (gptr));
        Gate_Set_TRIO (gptr);  
      }     
    } 
    array_insert_last (gate_ptr, gpptr, gptr);
  }
  return ((gate_ptr *) Array_to_ZArray_Free (gpptr));  // free added April 19
}

/*******************************************************/
void ResetIO (gate_ptr *gptrx)
{

  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = gptrx[i]) != NULL; i++) {
    if (Gate_Env (gptr) && !Gate_Marked (gptr)) {
      Gate_Marked (gptr) = 1;
      Push_Status (&Gate_Env_Stat (gptr), NOTR);
    }  
  }
}    

/*******************************************************/
void SetTRO (gate_ptr *gptrx)
{

  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = gptrx[i]) != NULL; i++) {
    if (Gate_Env (gptr))
      Gate_Set_TRO (gptr);  
  }
}    

/*******************************************************/
void Mirror (gate_ptr *gptrx, int mode) // 0 = SIBLING, 1 = NonSIBLING
{

  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = gptrx[i]) != NULL; i++) {
    if (Gate_Env (gptr) != NULL && Gate_TR_NONE (gptr)) {
      if (Status_Next (Gate_Env_Stat (gptr)) == NULL)
        Gate_Set_TRO (gptr);
      else if (mode == 0 /*&& Status_IO (Status_Next (Gate_Env_Stat (gptr))) == TRIO*/)
        Gate_Set_TRO (gptr);
      else  
        Status_IO (Gate_Env_Stat (gptr)) = Status_IO (Status_Next (Gate_Env_Stat (gptr)));  
      Gate_TRO (gptr) = Gate_TRI_TRO (gptr);
//      printf ("Gate %s is a %s\n", Gate_String (gptr), Gate_TRO_T (gptr) ? "TRO" : (Gate_TRI_T (gptr) ? "TRI" : "TRIO"));
    } 
//    else if (Gate_Env (gptr) != NULL)
//      printf ("Gate %s is a %s\n", Gate_String (gptr), Gate_TRO_T (gptr) ? "TRO" : (Gate_TRI_T (gptr) ? "TRI" : "TRIO"));
  }         
}

/*******************************************************/
void Free_TRFROMs (gate_ptr *gpptr)
{
  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    if (Gate_Ext_Out (gptr) != NULL) {
      bdd_free (Gate_Ext_TRFROM (gptr));
      Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);
    } 
    if (Gate_Int_Out (gptr) != NULL) {
      bdd_free (Gate_Int_TRFROM (gptr));
      Gate_Int_TRFROM (gptr) = bdd_zero (Bdd_Man);
    }
  }  
}


/*******************************************************/
void Update_Sibling (gate_ptr *gpptr1, gate_ptr *gpptr2)
{
  int i, size, j;
  gate_ptr gptr1, gptr2;
  
//  Print_Gates (gpptr1, "Before update: ");
  for (size = 0; gpptr1[size] != NULL; size++);
  j = size - 1;
  for (i = 0; (gptr1 = gpptr1[i]) != NULL; i++) {
    // check if gptr1 is in gptr2. if so, set index i to NULL and from the end of list gpptr2 
    // pointed by j which is decremented
    // check if it is in gptr2, if so set it to NULL, if not move it to index i.
    // repeat this until index i is NULL or j is NULL
    // 
    if (Is_ZArray_Member (gptr1, gpptr2)) {
      gpptr1[i] = NULL; 
      for (; (gptr2 = gpptr1[j]) != NULL; j--) {
        if (Is_ZArray_Member (gptr2, gpptr2)) 
          gpptr1[j] = NULL;
        else {
          gpptr1[j] = NULL;
          gpptr1[i] = gptr2;
          j--;
          break;
        }
      }
    }
  }           
}

/*******************************************************/
void Find_External_Places (void) 
{
  int i, g, j;
  gate_ptr gptr, *gpptr, gptro;
  signal_ptr sptr;
  spec_index_ptr siptr;
  
  Place_Array = array_alloc (signal_ptr, 1);
  Ext_Place_Array = array_alloc (signal_ptr, 1);
  for (i = 0; (gptr = list_of_specs[i]) != NULL; i++) 
    Gate_Visited (gptr) = 0;
    
  for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) {
    if (Gate_Type (gptr) == _OUT) {
      gptro = Gate_Uncle (gptr);
      if (Gate_Visited (gptro) == 1)
        continue;
      else {  
        Gate_Visited (gptro) = 1;
        Append_ZArray_to_Array (Place_Array, Gate_State_All_Places (gptro));
        Append_ZArray_to_Array (Ext_Place_Array, Gate_State_Ext_Places (gptro));
      } 
    }   
    for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++)
      for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        gptro = Spec_Index_Spec (siptr);
        if (Gate_Visited (gptro) == 1)
          continue;
        else {  
          Gate_Visited (gptro) = 1;
          Append_ZArray_to_Array (Place_Array, Gate_State_All_Places (gptro));
          Append_ZArray_to_Array (Ext_Place_Array, Gate_State_Ext_Places (gptro));
        }  
      }
  }    
}

/*******************************************************/
void Prepare_Hierarchical_Verification (int mode)
{

  if (cur_cir == flat_cir) {
    Assign_Gate_Outputs (); // was above before
    Change_Orders ();
    Set_Signal_Vars ();
    Init_Bdds ();
    Set_Gate_Functions ();
    Set_Gate_Hazards ();
  }  
//  Print_Gates (list_of_gates, "list_of_gates: ");
//  Check_Fanouts (); // New! April 7
  if (!tflag && !lflag) 
    NoSort (list_of_gates);
  else  
    TopSort_Internals (list_of_gates);
  if (lflag)  
    Levelize_Internals (DFS_List);
    
  if (cur_cir == flat_cir)
    All_Externals = Ext_Gates;  

  // Up to here, the set of Ext_Gates only has the external outputs of this subcircuit, and not
  // external gates of other subcircuits!
  // We need to have for each context, the set of external gates which are not in this context
  // but whose effect needs to be considered; i.e., external gates of sibling subcircuits.
  // Then, when initializing, flat-verifying, or verifying, we need to also process this set
  // and as we go lower the hierarchy, this list gets bigger, each time the ecternal gates
  // of the siblings at this level of hierarchy is added to the list of the parent context.
  // The corresponding list of the root circuit is empty!

/*
  if (cur_cir == flat_cir) {
    Print_Circuit_Graph (TOTAL);
//    wflag = 0; 
  }  
*/

  // June 1
  Find_External_Places ();
    
  Print_Context (cur_cir, FLAT);

  if (xflag && mode == HIERARCHICAL) { // April 7: "mode == HIERARCHICAL" was added
    if (sflag)
      Generate_Complex_Gates_Spec ();  
    else  
      Generate_Complex_Gates_NoSpec ();  
    Print_Context (complex_cir, COMPLEX);
  }    
}    

/*******************************************************/
void Make_Internal (gate_ptr gptr)
{
  int i;
  signal_ptr sptr;
  
//    printf ("Gate %s made internal\n", Gate_String (gptr));
  if (Gate_Int_Out (gptr) != NULL)
    return; // the gate is already internal
  else { // the gate used to be external
  
    if (Gate_Type (gptr) != _OUT) { // June 11
      bdd_free (Gate_Int_F (gptr));
      Gate_Int_F (gptr) = Gate_Ext_F (gptr); // internal changed to external
      Gate_Ext_F (gptr) = bdd_zero (Bdd_Man);
    }  
    
    bdd_free (Gate_Int_Ch (gptr));
    Gate_Int_Ch (gptr) = Gate_Ext_Ch (gptr);
    Gate_Ext_Ch (gptr) = bdd_zero (Bdd_Man); 
    Gate_Int_Out (gptr) = Gate_Ext_Out (gptr);
    Gate_Ext_Out (gptr) = NULL;
    for (i = 0; (sptr = Gate_Int_Out (gptr)[i]) != NULL; i++) 
      Signal_Type (sptr) = INTERNAL;
  }
}

/*******************************************************/
void Make_Gate_External (gate_ptr gptr)
{
  int i;
  signal_ptr sptr;
  
//  printf ("Gate %s made external\n", Gate_String (gptr));
  if (Gate_Ext_Out (gptr) != NULL)
    return; // the gate is already external
  else { // the gate used to be external
  
    if (Gate_Type (gptr) != _OUT) { // June 11
      bdd_free (Gate_Ext_F (gptr));
      Gate_Ext_F (gptr) = Gate_Int_F (gptr); // internal changed to external
      Gate_Int_F (gptr) = bdd_zero (Bdd_Man);
    }
      
    bdd_free (Gate_Ext_Ch (gptr));
    Gate_Ext_Ch (gptr) = Gate_Int_Ch (gptr);
    Gate_Int_Ch (gptr) = bdd_zero (Bdd_Man); 
    Gate_Ext_Out (gptr) = Gate_Int_Out (gptr);
    Gate_Int_Out (gptr) = NULL;
    for (i = 0; (sptr = Gate_Ext_Out (gptr)[i]) != NULL; i++) 
      Signal_Type (sptr) = EXTERNAL;
  }
}

/*******************************************************/
int Find_Subcir_Externals (void)
{

// This version does not force any signal to become external to another signal.
// This version is good as long as we do not have any element with internal state variables.
// When such elements are present, we must make all inputs/state-variables of such 
// elements external as soon as some state variable of that element is chosen as external.

  int i, found = 0, g, j;
  signal_ptr sptr, *spptr;
  gate_ptr gptr, *gpptr;
  char ch;

  printf ("\n**** NEXT LEVEL OF HIERARCHICAL VERIFICATION ****\n");
  printf ("Verify subcircuit %s flat or hierarchical? (f/h) ", Subcir_Path);
  while ((ch = tolower (getchar ())) != 'f' && ch != 'h');
  if (ch == 'f') {
    for (i = 0; (gptr = list_of_specs [i]) != NULL; i++) 
      Identify_Places (gptr);
    return (0);
  }  
  for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) 
    Gate_Visited (gptr) = 0;
  for (i = 0; (gptr = Sibling_Gates[i]) != NULL; i++) 
    Gate_Visited (gptr) = 0;
             
  printf ("\n^^^^ SPECIFYING THE SET OF EXTERNAL SIGNALS OF SUBCIRCUIT %s! ^^^^\nChoose internal or external (i/e) status for each gate.\n", Subcir_Path);
// Jan 18, 99: The two loops combined into one!
  for (g = 0, gpptr = list_of_gates; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Visited (gptr)) {
        printf ("Outputs of gate %s are already set to be %s\n", 
                Gate_String (gptr), Gate_Int_Out (gptr) ? "internal" : "external");
        continue;
      }  
      Gate_Visited (gptr) = 1;  
//      printf ("Do you want outputs of gate %s be internal or external? (i/e)", Gate_String (gptr));
      printf ("%s? (i/e) ", Gate_String (gptr));
      while ((ch = tolower (getchar ())) != 'e' && ch != 'i');
//      fseek (stdin, (long) (sizeof (char) * -1), SEEK_CUR);
      if (ch == 'i') 
        Make_Internal (gptr);
      else
        Make_Gate_External (gptr); 	   	                
    }
    if (g == 0)
      gpptr = Sibling_Gates;
  }      

  for (i = 0; (gptr = list_of_gates[i]) != NULL; i++) 
    Gate_Visited (gptr) = 0;
  for (i = 0; (gptr = Sibling_Gates[i]) != NULL; i++) 
    Gate_Visited (gptr) = 0;
    
  if (list_of_specs[0] != NULL)
    printf ("Specify the set of external variables of subcircuit %s! \nChoose internal or external (i/e) status for each variable.\n", Subcir_Path);
// June 11: Determining the status of places
  for (i = 0; (gptr = list_of_specs [i]) != NULL; i++) {
    for (j = 0; (sptr = Gate_State_All_Places (gptr)[j]) != NULL; j++) {
//      printf ("Do you want state variable %s be internal or external? (i/e)", Signal_String (sptr));
      printf ("%s? (i/e) ", Signal_String (sptr));
      while ((ch = tolower (getchar ())) != 'e' && ch != 'i');
      Signal_Type (sptr) = (ch == 'i') ? INTERNAL : EXTERNAL;
    }
    Identify_Places (gptr);
  } 
  printf ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
  return (1);       
}

/*******************************************************/
int Add_New_Externals (void)
{
  int i, found = 0;
  signal_ptr sptr, *spptr;
  
  if (Cur_Level == Level_Num)
    return (0);
  printf ("Signals made external for subcir %s :\n", Subcir_Path);
  spptr = External_Signals_of_Level[Cur_Level];
  for (i = 0; (sptr = spptr[i]) != NULL; i++) {
    if (Gate_Subcir (Signal_Parent (sptr)) == Subcir) {
      Signal_Type (sptr) = EXTERNAL;  
      printf ("%s ", Signal_String (sptr));
      found = 1;
    }  
  }
  return (found);
}

/*******************************************************/
int Verify_Flat_Hierarchical (context_ptr cptr)
{
  char ch;
  int Break;
  subcir_ptr subptr;

  subptr = cptr->_Subcir;
//  Print_Subcir (subptr); // June 29: added and then commented!
  if (!Find_Subcir_Externals ()) {
//    printf ("Verifying the above subcircuit flat!\n");
    Prepare_Hierarchical_Verification (FLAT); // new
    Break = Flat_Verify ();
    return (Break);
  } else {  
//    printf ("Verifying the above subcircuit hierarchically!\n");
    Prepare_Hierarchical_Verification (HIERARCHICAL);
    if (Find_Abstract () && Verify_Old ()) 
      Break = 1;
    else 
      Break = 0;  
    return (Break);      
  }
}


/*******************************************************/
void Set_TRO (gate_ptr *gpptr)
{
  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    if (Gate_Env (gptr)) {
      Gate_TRO (gptr) = Gate_TRI_TRO (gptr);
      Gate_Set_TRO (gptr); // new oct 15
    }  
  }
}

/*******************************************************/
gate_ptr *Find_Siblings (subcir_ptr subptr, gate_ptr *gpptr)
{
  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    if (Gate_Env (gptr) != NULL) { 
    // add all the gates in the supporting set of this gate to GatesArray recursively
      if (!Is_Array_Member (gptr, Subcir_SiblingsArray (subptr))) {
        Add_Subcir_Component (subptr, (signal_ptr) gptr, (array_t **) &Subcir_SiblingsArray (subptr));
        Add_Support_Gates (gptr, subptr);         // newly brought in in the if part 
      }  
    }  
  }
  return ((gate_ptr *) Array_to_ZArray_Free (Subcir_SiblingsArray (subptr)));  
}

/*******************************************************/
void PopIO (gate_ptr *gpptr)
{
  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) 
    if (Gate_Env (gptr) && !Gate_Marked (gptr)) {
      Gate_Marked (gptr) = 1;
      Pop_Status (&Gate_Env_Stat (gptr));
    }  
}

/*******************************************************/
void Mark_Pushed (gate_ptr *gpptr)
{
  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    Gate_Marked (gptr) = 0;
  }  
}

/*******************************************************/
context_ptr New_Context (char *str)
{
  context_ptr sub_cir;
  
  sub_cir = (context_ptr) malloc (sizeof (context));
  strcpy (sub_cir->_Subcir_Path, str);
  
}

/*******************************************************/
int Create_Support_Changing_Lists (input_ptr iptr, gate_ptr *gate_list, gate_ptr *sible_list)
{
  var_set_t *vst;
  array_t  *arr, *ch_arr;
  int i, j, k;
  gate_ptr *gpptr, gptr;
  signal_ptr sptr;
  
  for (i = 0, gpptr = gate_list; i < 2; i++) {
    for (j = 0; (gptr = gpptr[j]) != NULL; j++)
      Gate_Visited (gptr) = 0;
    if (i == 0)
      gpptr = sible_list;
  }      

  arr = array_alloc (gate_ptr, 1);
  ch_arr = array_alloc (signal_ptr, 1);
//  Print_Bdd (Input_TR (iptr), "\nInput_TR: ");
  vst = bdd_get_support (Input_TR (iptr));
  for (i = 0, j = 0, k = 0; i < flat_cir->_Sig_Num * 3; i++) {
    sptr = order[i];
    if (Signal_Next2_Order (sptr) == i) // April 6: a VVV variable
      continue;
    if (!var_set_get_elt (vst, i))
      continue;
    if (Signal_IO (sptr) == PLACE_NODE) {
      if (Signal_Next_Order (sptr) == i) {
        array_insert_last (signal_ptr, ch_arr, sptr);    
        k++;
      }  
      else 
        continue; // don't add to the support set!
    }    
     
    else if (Gate_Visited (gptr = Signal_Parent (sptr)) == 0) {
      array_insert_last (gate_ptr, arr, gptr);    
      Gate_Visited (gptr) = 1;
      j++;
    }  
  }  
  Push_Input (iptr, 1); // July 8
  Input_Support (iptr) = (gate_ptr *) malloc (sizeof (gate_ptr) * (j + 1));
  Input_Changing (iptr) = (signal_ptr *) malloc (sizeof (signal_ptr) * (k + 1));
  Input_ChangingArray (iptr) = ch_arr;
  for (i = 0; i < j; i++) {
    Input_Support (iptr)[i] = array_fetch (gate_ptr, arr, i);
//    printf ("%s ", Gate_String (Input_Support (iptr)[i]));  
    Gate_Visited (Input_Support (iptr)[i]) = 0;
  }  
//  printf ("\nThe Input_Changing: ");
  for (i = 0; i < k; i++) {
    Input_Changing (iptr)[i] = array_fetch (signal_ptr, ch_arr, i);    
//    printf ("%s ", Signal_String (Input_Changing (iptr)[i]));
  }
//  printf ("\n");  
  Input_Support (iptr)[j] = NULL;
  Input_Changing (iptr)[k] = NULL;
  array_free (arr);
  var_set_free (vst);
  if (k == 0)
    return (-1);
  else
    return (1);  
}

/*******************************************************/
gate_ptr *Find_Specs (gate_ptr *gate_list, gate_ptr *sib_list) 
{
  int i, g, j, num = 0, flag;
  gate_ptr gptr, *gpptr, gptro;
  signal_ptr sptr;
  spec_index_ptr siptr;
  

  for (i = 0; (gptr = list_of_specs[i]) != NULL; i++) // of the parent
    Gate_Visited (gptr) = 0;
    
  for (g = 0, gpptr = gate_list; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Type (gptr) == _OUT) {
        gptro = Gate_Uncle (gptr);
//        printf ("Input_Support (%s) at Spec %s = ", Gate_String (gptr), Gate_String (gptro));
        flag = Create_Support_Changing_Lists (&(Gate_State_Inputs (gptro) [Gate_Out_Index (gptr)]), gate_list, sib_list);
        if (Gate_Visited (gptro) != 1) {
          Gate_Visited (gptro) = flag;
          if (flag)
            num++;
        }    
      }   
      for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++)
        for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
          gptro = Spec_Index_Spec (siptr);
//          printf ("Input_Support (%s) at Spec %s = ", Signal_String (sptr), Gate_String (gptro));
          flag = Create_Support_Changing_Lists (&(Gate_State_Inputs (gptro) [Spec_Index_Index (siptr)]), gate_list, sib_list);
          if (Gate_Visited (gptro) != 1) {
            Gate_Visited (gptro) = flag;
            if (flag)
              num++;
          }    
        }
    }    
    if (g == 0)
      gpptr = sib_list;
  }
  gpptr = (gate_ptr *) malloc (sizeof (gate_ptr) * (num + 1));
  for (i = 0, j = 0; (gptr = list_of_specs[i]) != NULL; i++) // of the parent
    if (Gate_Visited (gptr) == 1)
      gpptr[j++] = gptr;
  gpptr[j] = NULL;
  
  return (gpptr);    
}

/*******************************************************/
void *Pop_Specs (gate_ptr *gate_list, gate_ptr *sib_list) 
{
  int i, g, j, num = 0, flag;
  gate_ptr gptr, *gpptr, gptro;
  signal_ptr sptr;
  spec_index_ptr siptr;
  

  for (g = 0, gpptr = gate_list; g < 2; g++) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
      if (Gate_Type (gptr) == _OUT) {
        gptro = Gate_Uncle (gptr);
//        printf ("Input_Support (%s) at Spec %s = ", Gate_String (gptr), Gate_String (gptro));
        Pop_Input (&(Gate_State_Inputs (gptro) [Gate_Out_Index (gptr)]));
      }   
      for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++)
        for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
          gptro = Spec_Index_Spec (siptr);
//          printf ("Input_Support (%s) at Spec %s = ", Signal_String (sptr), Gate_String (gptro));
          Pop_Input (&(Gate_State_Inputs (gptro) [Spec_Index_Index (siptr)]));
        }
    }    
    if (g == 0)
      gpptr = sib_list;
  }
}

/*******************************************************/
int Verify_Subcir (subcir_ptr subptr) /* fixed */
{
  int  Break;
  context_ptr temp_cir, sub_cir; // April 20: I made sub_cir a local variable, due to the recursion.
  gate_ptr gptr, *support, *subcir_ext_io;
  int i;

  sub_cir = (context_ptr) malloc (sizeof (context)); 
  strcpy (sub_cir->_Subcir_Path, Subcir_String (subptr));
  sub_cir->_Parent_Subcir = cur_cir;
  sub_cir->_place_array = sub_cir->_ext_place_array = NULL; // March 13
  sub_cir->_Subcir = subptr; // bad code w.r.t. object orientation
  sub_cir->_list_of_gates = Subcir_Gates (subptr);  // now 

  Subcir_Siblings (subptr) = Find_Siblings (subptr, Subcir_Gates (subptr));
  Update_Sibling (Subcir_Siblings (subptr), Subcir_Gates (subptr));
  Copy_ZArray (sub_cir->_Sibling_Gates, Subcir_Siblings (subptr));
  
//  Print_Gates (sub_cir->_Sibling_Gates, "Sibling Gates of child: "); // June 29
  
  // ??????? How are we sure that the IO signals of this partition are external now?
  subcir_ext_io = Duplicate_ZArray (Ext_Gates);

//  Print_Gates (subcir_ext_io, "Ext_IO of parent: "); // June 29
  Update_Sibling (subcir_ext_io, sub_cir->_Sibling_Gates);
//  Print_Gates (subcir_ext_io, "Ext_IO of child: "); // June 29

  Print_Subcir (subptr);
  support = Check_Projection (sub_cir->_Sibling_Gates, subcir_ext_io, subptr);
  free (subcir_ext_io);

  Mark_Pushed (Subcir_Gates (subptr));
  Mark_Pushed (sub_cir->_Sibling_Gates);
  ResetIO (Subcir_Gates (subptr));
  ResetIO (sub_cir->_Sibling_Gates);
  sub_cir->_Out_Gates = Find_Out_Gates (Subcir_Outputs (subptr));
  sub_cir->_In_Gates = Find_In_Gates (Subcir_Inputs (subptr));
//  Print_Input_Cones (sub_cir->_In_Gates, sub_cir->_Out_Gates);
//  Conize_All (sub_cir->_In_Gates, sub_cir->_Out_Gates);
  Mirror (Subcir_Gates (subptr) /*sub_cir->_list_of_gates*/, 1); // was sub_cir->_In_Gates
  Mirror (sub_cir->_Sibling_Gates, 0);

// We should be able to free sub_cir->_In_Gates and sub_cir->_Out_Gates at this point
// April 15:
//  free (sub_cir->_In_Gates);  // Commented on April 28. This info is later used to find the cones of logic
//  free (sub_cir->_Out_Gates);

/*
  // April 29
  for (i = 0; (gptr = sub_cir->_list_of_gates [i]) != NULL; i++) {
    if (Gate_Subcir (gptr) != subptr) {
      printf ("Gate %s has wrong subcircuit!\n", Gate_String (gptr));
      exit (-1);
    }  
  }
*/
  sub_cir->_list_of_specs = Find_Specs (sub_cir->_list_of_gates, sub_cir->_Sibling_Gates); // June 18
/*
  if (sub_cir->_list_of_specs[0] != NULL) // June 29
    Print_Gates (sub_cir->_list_of_specs, "list_of_specs"); 
*/
  temp_cir = cur_cir;
  cur_cir = sub_cir;
  Cur_Level++;
  printf (">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"); // for subcircuit generation
  Break = Verify_Flat_Hierarchical (sub_cir);
  Cur_Level--;
//  free (sub_cir->_list_of_gates); // ????? April 19: commented this line, and moved it to Free_Subcir as free (Subcir_Gates (subptr))
  Pop_Specs (sub_cir->_list_of_gates, sub_cir->_Sibling_Gates); // July 8
  array_free (sub_cir->_place_array);		// March 18
  array_free (sub_cir->_ext_place_array);	// March 18
  free (sub_cir->_In_Gates);  // April 28
  free (sub_cir->_Out_Gates); // April 28
  free (sub_cir->_list_of_specs); // April 28
  free (sub_cir);
  cur_cir = temp_cir;
  Mark_Pushed (Subcir_Gates (subptr));
  Mark_Pushed (sub_cir->_Sibling_Gates);
  PopIO (Subcir_Gates (subptr));
  PopIO (sub_cir->_Sibling_Gates);  


  // April 29
//  printf ("The subcirs of the following gates were poped: "); // June 29
  for (i = 0; (gptr = Subcir_Gates (subptr)[i] /* sub_cir->_list_of_gates [i] */ ) != NULL; i++) {
    Pop_Subcir (gptr);
//    printf ("%s ", Gate_String (gptr));
  }
//  printf ("\n"); // June 29

  if (support) {
    Remove_Projection (support);
    free (support);
  }  
  
  return (Break);
}

/*******************************************************/
void Free_External_Gates (gate_ptr *gpptr)
{
  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) 
    if (Gate_Ext_Out (gptr) != NULL) {
      bdd_free (Gate_Ext_TRFROM (gptr));
      bdd_free (Gate_Ext_TR (gptr));
      Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);
      Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
    }      
}


/*******************************************************/
void Pop_Places (gate_ptr gptr)
{
  spec_place_ptr splptr;
  
  splptr = Gate_State_Places (gptr);
  free (Spec_All_Places (splptr));
  free (Spec_Ext_Places (splptr));
  free (Spec_Int_Places (splptr));
  Gate_State_Places (gptr) = Spec_Next (splptr);
  free (splptr);  
}

/*******************************************************/
void Pop_Input (input_ptr iptr)
{
  io_ptr ioptr;
  
  ioptr = Input_Pushed (iptr);
  if (IO_Support (ioptr))
    free (IO_Support (ioptr));
  if (IO_ChangingArray (ioptr))
    array_free (IO_ChangingArray (ioptr));
  if (IO_Changing (ioptr))
    free (IO_Changing (ioptr));
  Input_Pushed (iptr) = IO_Next (ioptr);
  free (ioptr);
}

/*******************************************************/
/*******************************************************/
/*******************************************************/
gate_ptr *Create_Env ()
{
  gate_ptr gptr, *gpptr, tempg[MAXGATE], *temp, gptro;
  int i, j, g;
  rec_ptr rptr;
  bdd_t *bdd, *bdd1;
  signal_ptr sptr;
  input_ptr iptr;

  for (i = 0, g = 0, gpptr = Ext_Gates; g < 2; g++) {
    for (j = 0; (gptr = gpptr[j]) != NULL; j++)
      if (Gate_Ext_Out (gptr)) // June 1: Gate_Type
        tempg[i++] = gptr;
    if (g == 0)
      gpptr = Sibling_Gates;
  }        

  tempg[i] = NULL;
  
  if (i > Max_External)
    Max_External = i;
  
  temp = (gate_ptr *) malloc (sizeof (gate_ptr) * (i+1));
  for (j = 0; j <= i; j++)
    temp[j] = tempg[j];

//  printf ("Pushing in Create_Env "); // June 29
  for (i = 0; (gptr = temp[i]) != NULL; i++) {
//    printf ("%s ", Gate_String (gptr)); // June 29
/*
    printf ("Pushing Ext_Gate %s\n", Gate_String (gptr));
    Print_Bdd (Gate_Ext_TR (gptr), "Ext_TR");
*/

//    bdd = bdd_dup (Gate_Ext_TR (gptr)); // June 22
    bdd = Make_Bdd_Complete (Gate_Ext_TR (gptr), gptr);  //June 22
//    Print_Bdd (bdd, "Ext_TR_Completed");
    Push ((rec_ptr *) &(Gate_TRI_TRO_List (gptr)), bdd, temp, 0);
    bdd_free (Gate_Ext_TR (gptr));
    if (Gate_Env (gptr) == NULL) {
      printf ("Fatal Push Error\n");
      exit (-1);
    }  
    Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
    bdd_free (Gate_Ext_TRFROM (gptr));
    Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);
  }
  
  gpptr = Sibling_Gates;
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    if (Gate_Env (gptr) == NULL) {
      printf ("Fatal Error in Sibling Gates\n");
      exit (-1);
    }
    if (Gate_Int_Out (gptr)) {
//      printf ("Repushing internal sibling gate %s\n", Gate_String (gptr)); // June 29
//      Print_Bdd (Gate_TRI_TRO (gptr), "TRO_TRI");
      Push ((rec_ptr *) &(Gate_TRI_TRO_List (gptr)), bdd_dup (Gate_TRI_TRO (gptr)), Gate_Support (gptr), 0);
      
      // March 5: QUESTION: SHOULDN'T THE FOLLOWING BE Int BDDS AND NOT Ext ?????????????
      
      bdd_free (Gate_Ext_TR (gptr));
      Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
      bdd_free (Gate_Ext_TRFROM (gptr));
      Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);      
//      printf ("%s ", Gate_String (gptr)); // June 29
    }  
  }
  
  gpptr = list_of_specs;
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    Push_Places (gptr, (signal_ptr *) Duplicate_ZArray ((gate_ptr *) Gate_State_Ext_Places (gptr)));
    for (j = 0; (sptr = Input_Signal (Gate_State_Inputs (gptr)[j])) != NULL; j++) {
      iptr = &(Gate_State_Inputs (gptr)[j]);       
      Input_Pushed_TR (iptr) = Push_Bdd (Input_Pushed_TR (iptr), Input_Ext_TR (iptr));
//      bdd_free (Input_Ext_TR (gptr)); it is used in the above function
      Input_Ext_TR (iptr) = bdd_zero (Bdd_Man);
//     printf ("%s ", Gate_String (gptr)); // June 29
    }  
  }
    
//  printf ("\n"); // June 29
  
  return (temp);  
}

/*******************************************************/
gate_ptr *Create_Env_Old ()
{
  gate_ptr gptr, *gpptr, tempg[MAXGATE], *temp, gptro;
  int i, j;
  rec_ptr rptr;
  bdd_t *bdd, *bdd1;

  gpptr = Ext_Gates;
  for (i = 0, j = 0; (gptr = gpptr[j]) != NULL; j++) 
    if (Gate_Type (gptr) != _STATE) // June 1
      tempg[i++] = gptr;
  gpptr = Sibling_Gates;
  for (j = 0; (gptr = gpptr[j]) != NULL; j++)
    if (Gate_Ext_Out (gptr) && Gate_Type (gptr) != _STATE) // June 1: Gate_Type
      tempg[i++] = gptr;
  tempg[i] = NULL;
  
  if (i > Max_External)
    Max_External = i;
  
  temp = (gate_ptr *) malloc (sizeof (gate_ptr) * (i+1));
  for (j = 0; j <= i; j++)
    temp[j] = tempg[j];

  printf ("PUSHing ");
  for (i = 0; (gptr = temp[i]) != NULL; i++) {
/*
    printf ("Pushing Ext_Gate %s\n", Gate_String (gptr));
    Print_Bdd (Gate_Ext_TR (gptr), "Ext_TR");
*/
    // May 10: External place transitions of _STATES are moved to the TRs of the fanout _OUT gates!
    if (Gate_Type (gptr) == _STATE) // May 28
      continue;
/*  Commented on May 24
    if (Gate_Type (gptr) == _OUT && Gate_Ext_Out (Gate_Uncle (gptr)) != NULL) {
      gptro = Gate_Uncle (gptr);
      bdd = bdd_and (Gate_Ext_TR (gptro), Gate_Ext_Ch (gptr), 1, 0);
      bdd1 = bdd_and (Gate_Ext_TR (gptr), Gate_Stable_Bdd (gptr), 1, 1);
      bdd1 = Exist_Quantify (bdd1, Gate_Int_Out (gptro));
      bdd1 = Exist_Quantify_Next (bdd1, Gate_Int_Out (gptro));
      bdd_free (Gate_Ext_TR (gptr));
      Gate_Ext_TR (gptr) = bdd_or (bdd, bdd1, 1, 1);
      bdd_free (bdd);
      bdd_free (bdd1);
    }
*/    
    bdd = Make_Bdd_Complete (Gate_Ext_TR (gptr), gptr);  //Oct 31!!!!!
    
//    bdd = bdd_dup (Gate_Ext_TR (gptr));
     
//    Print_Bdd (bdd, "Ext_TR_Completed");
    Push ((rec_ptr *) &(Gate_TRI_TRO_List (gptr)), bdd, temp, 0);
    bdd_free (Gate_Ext_TR (gptr));
    if (Gate_Env (gptr) == NULL) {
      printf ("Fatal Push Error\n");
      exit (-1);
    }  
    Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
    bdd_free (Gate_Ext_TRFROM (gptr));
    Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);
//    printf ("%s ", Gate_String (gptr));
  }
  
  gpptr = Sibling_Gates;
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    if (Gate_Env (gptr) == NULL) {
      printf ("Fatal Error in Sibling Gates\n");
      exit (-1);
    }
    if (Gate_Type (gptr) == _STATE) // May 28
      continue;   
    if (Gate_Int_Out (gptr)) {
      printf ("Repushing internal sibling gate %s\n", Gate_String (gptr));
//      Print_Bdd (Gate_TRI_TRO (gptr), "TRO_TRI");
      Push ((rec_ptr *) &(Gate_TRI_TRO_List (gptr)), bdd_dup (Gate_TRI_TRO (gptr)), Gate_Support (gptr), 0);
      
      // March 5: QUESTION: SHOULDN'T THE FOLLOWING BE Int BDDS AND NOT Ext ?????????????
      
      bdd_free (Gate_Ext_TR (gptr));
      Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
      bdd_free (Gate_Ext_TRFROM (gptr));
      Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);      
//      printf ("%s ", Gate_String (gptr));
    }  
  }
//  printf ("\n");
  
  return (temp);  
}

/*******************************************************/
void Remove_Env (void)
{
  gate_ptr gptr, *gpptr;
  int i, j;
  bdd_t *bdd;
  signal_ptr sptr;
  input_ptr iptr;
  
  gpptr = Ext_Gates;
//  printf ("Poping in Remove_Env :");
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    bdd_free (bdd = Pop ((rec_ptr *) &(Gate_TRI_TRO_List (gptr))));
//    printf ("%s ", Gate_String (gptr));
  }
  gpptr = Sibling_Gates;
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    bdd_free (bdd = Pop ((rec_ptr *) &(Gate_TRI_TRO_List (gptr))));
//    printf ("%s ", Gate_String (gptr));
  }

  gpptr = list_of_specs;
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    Pop_Places (gptr);
    for (j = 0; (sptr = Input_Signal (Gate_State_Inputs (gptr)[j])) != NULL; j++) {
      iptr = &(Gate_State_Inputs (gptr)[j]);
      Input_Pushed_TR (iptr) = Pop_Bdd (Input_Pushed_TR (iptr));
//      printf ("%s ", Gate_String (gptr));
    }  
  }
//  printf ("\n");
}

/*******************************************************/
void Free_Subcircuit (subcir_ptr subptr)
{
  free (Subcir_Inputs (subptr));
  Subcir_Inputs (subptr) = NULL;
  free (Subcir_Outputs (subptr));
  Subcir_Outputs (subptr) = NULL;
  free (Subcir_Internals (subptr));  
  Subcir_Internals (subptr) = NULL;
  free (Subcir_Gates (subptr));
  Subcir_Gates (subptr) = NULL;
  free (Subcir_Siblings (subptr));
  Subcir_Siblings (subptr) = NULL;
}

/*******************************************************/
void Create_Cones (void)
{
  char str[100];
  int i;
  gate_ptr gptr;
  subcir_ptr subptr;
  
  for (i = 0, Subcir_Num = 0; (gptr = list_of_gates[i]) != NULL; i++) {
    if (Gate_Subcir (gptr) == Subcir) {
      sprintf (str, "%s/%d", Subcir_Path, Subcir_Num);
//      printf ("Seed gate taken as %s\n", Gate_String (gptr));
      subcir_list [Subcir_Num] = New_Subcircuit (str);
      Find_Transitive_Inputs (gptr, subcir_list[Subcir_Num++]);
    }
  }
  for (i = 0; i < Subcir_Num; i++) {
    subptr = subcir_list[i];
    Subcir_Inputs (subptr) = (signal_ptr *) Array_to_ZArray_Free (Subcir_InputsArray (subptr));
    Subcir_Outputs (subptr) = (signal_ptr *) Array_to_ZArray_Free (Subcir_OutputsArray (subptr));
    Subcir_Internals (subptr) = (signal_ptr *) Array_to_ZArray_Free (Subcir_InternalsArray (subptr));
    Subcir_Gates (subptr) = (gate_ptr *) Array_to_ZArray_Free (Subcir_GatesArray (subptr));  // this is just internal gates!!
  }  
}

/*******************************************************/
int Verify_Old (void) 
{
// This version verifies the subcircuits which are created by the 
// partitioning set of external signals.

  int i, verified = 1, j;
  gate_ptr gptr, *gpptr, *support = NULL;
  subcir_ptr subptr[MAX_SUBCIR];
  char ch;

  Project_Places (); // May 24

  All_Externals = Create_Env (); // needs to push R
  Push_R (R);

//  support = Check_Projection (Ext_Gates, NULL, Subcir);

  printf ("\n&&&& VERYFYING THE SUBCIRCUITS OF SUBCIRCUIT %s &&&&\n", Subcir_Path);
  Subcir_Num = 0;
  for (i = 0, j = 0; (gptr = list_of_gates[i]) != NULL; i++) 
    if (subptr[j] = Create_Subcircuit (gptr)) {
/*
      printf ("Do you like to verify the above subcir?(y/n)\n");
      while ((ch = tolower (getchar ())) != 'n' && ch != 'y');
      if (ch == 'n') {
        Free_Subcircuit (subptr[j]);
        printf ("Skipping the verification of subcir %s\n", Subcir_String (subptr[j]));
        continue;
      }   
*/    
      
      if (!Verify_Subcir (subptr[j])) {
//        printf ("Subcir %s not verified\n", Subcir_String (subptr[j]));   
        verified = 0;
        break;
      } else {
//        printf ("Subcir %s verified\n", Subcir_String (subptr[j]));
        Free_Subcircuit (subptr[j]);
      }
      j++;
    } 

  printf ("&&&& VERYFIED THE SUBCIRCUITS OF SUBCIRCUIT %s &&&&\n\n", Subcir_Path);

  for (i = 0; (gptr = list_of_gates[i]) != NULL; i++)
    Gate_HLevel (gptr) = Cur_Level;       

/**/
  if (support) {
    Remove_Projection (support);
    free (support);
  }  
/**/

  Remove_Env (/*All_Externals*/); // needs to pop R
  Pop_R ();
  free (All_Externals);
  return (verified);          
}

