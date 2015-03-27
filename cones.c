gate_ptr	Cone_Head = NULL;

/*******************************************************/
// Cone Related Functions
/*******************************************************/
/*******************************************************/
void Print_Inputs_Recursive (gate_ptr gptr, gate_ptr *inputs)
{
  int i;
  signal_ptr sptr;
  gate_ptr gptr1;
  
  Gate_Visited (gptr) = 1;
  if (Gate_Type (gptr) == _OUT) {
    gptr1 = Gate_Uncle (gptr);
    if (!Gate_Visited (gptr1)) {
      printf ("%s ", Gate_String (gptr1));
      cone++;
      if (!Is_ZArray_Member (gptr1, inputs))
        Print_Inputs_Recursive (gptr1, inputs);
      else
        Gate_Visited (gptr1) = 1;  
    }    
  }
  else for (i = 0; (sptr = Input_Signal (Gate_Inputs (gptr)[i])) != NULL; i++) {
    gptr1 = Signal_Parent (sptr);
    if (!Gate_Visited (gptr1)) {
      printf ("%s ", Gate_String (gptr1));
      cone++;
      if (!Is_ZArray_Member (gptr1, inputs))
        Print_Inputs_Recursive (gptr1, inputs);
      else
        Gate_Visited (gptr1) = 1;          
    }  
  }
//  Gate_Visited (gptr) = 0;
}

/*******************************************************/
void Find_Transitive_Output_Cones (gate_ptr gptr, subcir_ptr subptr)
{
  int j, i;
  signal_ptr sptr;
  gate_ptr cgptr;
  
  /* This does not handle partitions where a gate can belong to multiple blocks */

// must work on the fanout gates, not on the parent of the fanout signals which are the same gate!!!!

  for (j = 0; Gate_Outputs(gptr)[j] != NULL; j++) {
    sptr = Gate_Outputs (gptr)[j];
    if (Signal_Type (sptr) == EXTERNAL || 
        Gate_Env (Signal_Parent (sptr)) && Gate_TRI_T (Signal_Parent (sptr)) ) {
      Add_Subcir_Component (subptr, sptr, &Subcir_OutputsArray (subptr));        
//      printf ("gate %s made output\n", Gate_String (Signal_Parent (sptr)));
    } else if (Gate_Env (Signal_Parent (sptr)) && Gate_TRIO_T (Signal_Parent (sptr)) ) {
      Add_Subcir_Component (subptr, sptr, &Subcir_OutputsArray (subptr)); 
//      printf ("gate %s made output and Find_Transitive_Inputs conditionally called\n", Gate_String (Signal_Parent (sptr)));  

    } 
  }      
}

/*******************************************************/
void Create_Subcir_Cone_Recursive (gate_ptr gptr, subcir_ptr subptr)
{
  int j;
  signal_ptr sptr, sptro;
  gate_ptr pgptr;
  
  /* This does not handle partitions where a gate can belong to multiple blocks */
  
  if (Gate_Subcir (gptr) == subptr)
    return;
  else {
/*
    if (Gate_Subcir (gptr) != Subcir) { // Subcir was NULL
      printf ("E is not an appropriate partition! Gate %s belongs to multiple subcircuits!\n", 
      		Gate_String (gptr) );
      exit (-1);
    }  		
*/
//    Gate_Subcir (gptr) = subptr;  // Only internal gates!
    Push_Subcir (gptr, subptr, 1);
//    printf ("Gate %s assigned to this subcir\n", Gate_String (gptr));
//    if (!Is_Array_Member (gptr, Subcir_GatesArray (subptr))) 
      Add_Subcir_Component (subptr, (signal_ptr) gptr, (array_t **) &Subcir_GatesArray (subptr));          
  }  
  
  for (j = 0; (sptr = Input_Signal (Gate_Inputs(gptr)[j])) != NULL; j++) {
    pgptr = Signal_Parent (sptr);
    if (Signal_Type (sptr) == EXTERNAL || Gate_Env (pgptr) && Gate_TRO_T (pgptr) ) {
      Add_Subcir_Component (subptr, sptr, (array_t **) &Subcir_InputsArray (subptr));

      // new! Do we really need to check for presence?
//      if (!Is_Array_Member (pgptr, Subcir_GatesArray (subptr)))
        if (Gate_Subcir (pgptr) != subptr) {
          Push_Subcir (pgptr, subptr, 0);
          Add_Subcir_Component (subptr, (signal_ptr) pgptr, (array_t **) &Subcir_GatesArray (subptr));
  	}  
//      printf ("gate %s made Input\n", Gate_String (pgptr));  
    } else if (!Gate_Env (pgptr) ) {
      Add_Subcir_Component (subptr, sptr, (array_t **) &Subcir_InternalsArray (subptr));
      Create_Subcir_Cone_Recursive (pgptr, subptr);  
//      printf ("gate %s made Internal and Find_Transitive_Input called\n", Gate_String (pgptr));  
    } else if (Gate_Env (pgptr) && Gate_TRIO_T (pgptr) ) {
//      printf ("gate %s made Input and Find_Transitive_Input not called\n", Gate_String (pgptr)));  
      Add_Subcir_Component (subptr, sptr, (array_t **) &Subcir_InputsArray (subptr));
      // not all outputs of this gate return to this block! 
      // When Find_Transitive_Input is called for this gate, eventually Find_Transitive_Outputs will 
      // also be called, while not all outputs of this gate are in this block
      Create_Subcir_Cone_Recursive (pgptr, subptr);        
    }
  }      
//  Find_Transitive_Output_Cones (gptr, subptr);  

}

/*******************************************************/
subcir_ptr Create_Subcir_Cone (gate_ptr gptr)
{
  char str[100];
  int i, j;
  gate_ptr gptr2;
  subcir_ptr subptr;
  
  if (Gate_Subcir (gptr) == Subcir) { // This needs to be changed cause now each gate can belong to multiple subcircuits
    sprintf (str, "%s/%d", Subcir_Path, Subcir_Num);
//  printf ("Seed gate taken as %s\n", Gate_String (gptr));
    subptr = New_Subcircuit (str);
 
    Create_Subcir_Cone_Recursive (gptr, subptr);
 
    Subcir_Inputs (subptr) = (signal_ptr *) Array_to_ZArray_Free (Subcir_InputsArray (subptr));
    Subcir_Outputs (subptr) = (signal_ptr *) Array_to_ZArray_Free (Subcir_OutputsArray (subptr));
    Subcir_Internals (subptr) = (signal_ptr *) Array_to_ZArray_Free (Subcir_InternalsArray (subptr));
    Subcir_Gates (subptr) = (gate_ptr *) Array_to_ZArray_Free (Subcir_GatesArray (subptr));  // this is just internal gates!!
    subcir_list[Subcir_Num++] = subptr;  
    for (j = 0; (gptr2 = list_of_gates [j]) != NULL; j++)
      Gate_Visited (gptr2) = 0;
    return (subptr);  
  }
  else return (NULL);
}

/*******************************************************/
cones_ptr Add_Cone (gate_ptr gptr, gate_ptr cgptr)
{
  cones_ptr cptr1, cptr;
  
//  printf ("Gate %s added to the list of cones that gate %s belongs to\n", Gate_String (cgptr), Gate_String (gptr));
  cptr = Gate_Cones (gptr);
  cptr1 = (cones_ptr) malloc (sizeof (cones));
  Cone_Gate (cptr1) = cgptr;
  Cone_Next (cptr1) = cptr; // NULL
  return (cptr1);  
}

/*******************************************************/
int Is_In_Cones (gate_ptr gptr, cones_ptr cptrl)
{ // Check if cptr can be found in the chain cptrl
  
  
  for (; cptrl != NULL; cptrl = Cone_Next (cptrl))
    if (Cone_Gate (cptrl) == gptr)
      return (1);
  return (0);    
}

/*******************************************************/
void Free_Cones (gate_ptr gptr)
{
  cones_ptr cptr, ctemp;
  
  cptr = Gate_Cones (gptr);
  while (cptr != NULL) {
    ctemp = Cone_Next (cptr);
    free (cptr);
    cptr = ctemp;  
  }
  Gate_Cones (gptr) = NULL;
}

/*******************************************************/
void Conize (gate_ptr gptr, gate_ptr cgptr, cones_ptr cptrl, gate_ptr *inputs)
{
// This function is called for any PO gate pogptr as Conize (pogptr, pogptr, cptr), 
// after performing Gate_Is_Cone (pogptr) = 1; Cone_Gate (Gate_Cones (pogptr)) = pogptr;

  int i;
  signal_ptr sptr;
  gate_ptr   gptr1;
  cones_ptr  cptr1;
  
  if (Gate_Visited (gptr))
    return;
  Gate_Visited (gptr) = 1;
/*
  if (!strcmp (Gate_String (gptr), "c1")) {
    getchar ();
  }  
*/
  if (Gate_Type (gptr) == _OUT) {
    gptr1 = Gate_Uncle (gptr);
    if (Gate_Visited (gptr1))
      return;
    if (/*!Gate_Visited (gptr1) && */!Is_ZArray_Member (gptr1, inputs)) { // what if it is a cone
        Gate_Visited (gptr1) = 1;
        if (Gate_Is_Cone (gptr1)) {
//          Gate_Cones (gptr1) = Add_Cone (gptr1, cgptr);
          return; 
        }
        if (!Gate_Cones (gptr1)) {
          Gate_Cones (gptr1) = Add_Cone (gptr1, cgptr);
          Conize (gptr1, cgptr, cptrl, inputs);
        } else if (Cone_Gate (Gate_Cones (gptr1)) != cgptr && Is_In_Cones (Cone_Gate (Gate_Cones (gptr1)), cptrl)) {
          Free_Cones (gptr1);
          Gate_Cones (gptr1) = Add_Cone (gptr1, cgptr);
          Gate_Visited (gptr1) = 0;
          Conize (gptr1, cgptr, cptrl, inputs);
//        continue;
        } else {  
        // Here, gate gptr1 already belongs to other cones. 
        // We should create a new cone at gptr1, and conize it.
        // Meanwhile, cgptr will be added to the list of cones of gptr1.
          Gate_Is_Cone (gptr1) = 1;
/*
          if (Gate_Cones (gptr1))
            Free_Cones (gptr1);
*/
	  Gate_Cones (gptr1) = Add_Cone (gptr1, cgptr);
          Gate_Cones (gptr1) = Add_Cone (gptr1, gptr1); // the second gptr1 was cgptr!
          Gate_Next_Cone (gptr1) = Cone_Head;
          Cone_Head = gptr1;
          Gate_Visited (gptr1) = 0;
          Conize (gptr1, gptr1, Gate_Cones (gptr1), inputs);
        }  
    }      
  }
  else for (i = 0; (sptr = Input_Signal (Gate_Inputs (gptr)[i])) != NULL; i++) {
    gptr1 = Signal_Parent (sptr);
    if (/* !Gate_Visited (gptr1) && */!Is_ZArray_Member (gptr1, inputs)) { // what if it is a cone
      if (Gate_Is_Cone (gptr1)) {
//        Gate_Cones (gptr1) = Add_Cone (gptr1, cgptr);
        continue; 
      }
      if (!Gate_Cones (gptr1)) {
        Gate_Cones (gptr1) = Add_Cone (gptr1, cgptr);
        Conize (gptr1, cgptr, cptrl, inputs);
      } else if (Cone_Gate (Gate_Cones (gptr1)) != cgptr && Is_In_Cones (Cone_Gate (Gate_Cones (gptr1)), cptrl)) {
        Free_Cones (gptr1);
        Gate_Cones (gptr1) = Add_Cone (gptr1, cgptr);
        Gate_Visited (gptr1) = 0;
        Conize (gptr1, cgptr, cptrl, inputs);
//        continue;
      } else {
      // Here, gate gptr1 already belongs to other cones. 
      // We should create a new cone at gptr1, and conize it.
      // Meanwhile, cgptr will be added to the list of cones of gptr1.
        Gate_Is_Cone (gptr1) = 1;
/*
        if (Gate_Cones (gptr1))
          Free_Cones (gptr1);
*/
	Gate_Cones (gptr1) = Add_Cone (gptr1, cgptr);
        Gate_Cones (gptr1) = Add_Cone (gptr1, gptr1); // the second gptr1 was cgptr!
        Gate_Next_Cone (gptr1) = Cone_Head;
        Cone_Head = gptr1;
        Gate_Visited (gptr1) = 0;
        Conize (gptr1, gptr1, Gate_Cones (gptr1), inputs);
      }  
    }
  }    
//  }
  
}

/*******************************************************/
void Conize_All (gate_ptr *inputs, gate_ptr *outputs)
{
  int i, j;
  gate_ptr gptr, gptr2;
  context_ptr temp_cir;
  cones_ptr cptr;
  
  temp_cir = cur_cir;
  cur_cir = flat_cir;
  Update_Sibling (inputs, outputs);
  getchar ();
  Print_Gates (inputs, "Input Gates: ");
  Print_Gates (outputs, "Output Gates: ");
  
  for (j = 0; (gptr = list_of_gates [j]) != NULL; j++) {
    Gate_Is_Cone (gptr) = Gate_Visited (gptr) = 0;
    Free_Cones (gptr);
  }  

  Cone_Head = NULL;  
  for (i = 0; (gptr = outputs[i]) != NULL; i++) {
    if (Gate_Is_Cone (gptr)) {
//      Gate_Cones (gptr) = Add_Cone (gptr, gptr);
      continue;
    }  
    Gate_Is_Cone (gptr) = 1;
/*
    if (Gate_Cones (gptr))
      Free_Cones (gptr);
*/
    Gate_Cones (gptr) = Add_Cone (gptr, gptr);
    Gate_Next_Cone (gptr) = Cone_Head;
    Cone_Head = gptr;
    Conize (gptr, gptr, Gate_Cones (gptr), inputs);
    for (j = 0; (gptr2 = list_of_gates [j]) != NULL; j++)
      Gate_Visited (gptr2) = 0;
  }
  printf ("List of Cones: \n");
  for (gptr = Cone_Head; gptr != NULL; gptr = Gate_Next_Cone (gptr)) {
    printf ("%s with gates : ", Gate_String (gptr));
    for (i = 0; (gptr2 = list_of_gates[i]) != NULL; i++) 
      if (Is_In_Cones (gptr, Gate_Cones (gptr2)))
        printf ("%s ", Gate_String (gptr2));
    printf ("in its cone, and being in the cone of gates: ");
    for (cptr = Gate_Cones (gptr); cptr != NULL; cptr = Cone_Next (cptr))
      printf ("%s ", Gate_String (Cone_Gate (cptr)));
    printf ("\n");    
  }  
  printf ("\n");  
  cur_cir = temp_cir;
}

/*******************************************************/
void Print_Input_Cones (gate_ptr *inputs, gate_ptr *outputs)
{
  int i, j;
  gate_ptr gptr, gptr2;
  context_ptr temp_cir;
  
  temp_cir = cur_cir;
  cur_cir = flat_cir;
  Print_Gates (inputs, "Input Gates: ");
  Print_Gates (outputs, "Output Gates: ");
  
  for (i = 0; (gptr = outputs[i]) != NULL; i++) {
    cone = 0;
    printf ("Gates in the input cone of gate %s: ", Gate_String (gptr));
    Print_Inputs_Recursive (gptr, inputs);
    printf ("   # of gates: %d\n", cone);
    for (j = 0; (gptr2 = list_of_gates [j]) != NULL; j++)
      Gate_Visited (gptr2) = 0;
  }   
  cur_cir = temp_cir;
}

/*******************************************************/
/*******************************************************/
int Verify (void) 
{
// Unlike the older version, this version of Verify calls Verify_Subcir
// per the cone of logic for the output gates of the current sub-circuit
// and not the sub-sub-circuits generated by the set of external signals.
// Thus, the sub-sub-circuits may overlap, but chances of their sizes 
// being more manageable are more.

  int i, verified = 1, j;
  gate_ptr gptr, *gpptr, *support = NULL, gptr2;
  subcir_ptr    temp;
  subcir_ptr	subptr;

//  if (cur_cir == flat_cir) // May 28
    return (Verify_Old ());

  All_Externals = Create_Env (); // needs to push R
  Push_R (R);

//  support = Check_Projection (Ext_Gates, NULL, Subcir);

  Update_Sibling (In_Gates, Out_Gates);
  Subcir_Num = 0;

  for (i = 0; (gptr = Out_Gates[i]) != NULL; i++) {
    cone = 0;
//    printf ("Gates in the input cone of gate %s: ", Gate_String (gptr));
    temp = Gate_Subcir (gptr);
    if (!(subptr = Create_Subcir_Cone (gptr)))
      continue;
//    printf ("   # of gates: %d\n", cone);
    if (!Verify_Subcir (subptr)) {
      printf ("Subcir %s not verified\n", Subcir_String (subptr)); // June 29
      verified = 0;
      break;
    } else {
      printf ("Subcir %s verified\n", Subcir_String (subptr)); // June 29
      Free_Subcircuit (subptr);
    }
  }
  
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
