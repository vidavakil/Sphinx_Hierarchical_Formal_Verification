#include "type.h"
#include "sphinx.h"

int  		input_no = 0, input_cur = 0;
int		any_spec = 0;
input_ptr 	temp_input = NULL;

extern int		Level_Num;  
extern signal_ptr	temp_signal;
extern signal_ptr	*External_Signals_of_Level[];
extern array_t *place_array, *input_array, *output_array;
extern int		MODE;
extern context_ptr	cur_cir; 

signal_ptr New_Signal (name_ptr nptr, petri_type io);
void Make_External (signal_ptr spptr[]);
void Enter_External_Signals (signal_ptr sptr);

/*******************************************************/

/*******************************************************/
// Signal functions
/*******************************************************/

/*******************************************************/
// Signal entry functions
/*******************************************************/
/*******************************************************/
// Initialization/Initial-Value functions
/*******************************************************/
void Enter_Initial_State (char *name, int init) 
{
  signal_ptr sptr;
	
  Signal_Init (Enter_Signal (name, UNDEFINED)) = init;  
}

/*******************************************************/
// For Hoshik: to initialize his signals/places
/*******************************************************/
void Init_Signal (signal_ptr sptr, int init) 
{
  Signal_Init (sptr) = init;  
}

/*******************************************************/
signal_ptr New_Signal (name_ptr nptr, petri_type io)
{
  signal_ptr sptr;
  
  sptr = (signal_ptr) malloc (sizeof (signal));
  Signal_Name (sptr) = nptr;
  Signal_IO (sptr) = io;
  Signal_Next (sptr) = NULL;
  Signal_Type (sptr) = UNDEFINED;
  Signal_Parent (sptr) = NULL;
  signal_array[Sig_Num] = sptr;	/* global array */
  signal_array[Sig_Num+1] = NULL; /* NOTICE */
  Signal_Init (sptr) = 0; // Feb 10
  Signal_SpecFanout (sptr) = NULL; // June 7

//  Signal_Order (sptr) = Sig_Num++;  // Nov 2nd

  // new, Hoshik. Hoshik's code creates two lists, that of real signals, and that
  // of places.
  Signal_Trans(sptr) = array_alloc (transition_ptr, 1);
  if (io == PLACE_NODE) {
    any_spec = 1; 
    array_insert_last (signal_ptr, place_array, sptr);
  }  
  if (io == INPUT_NODE)
    array_insert_last(signal_ptr, input_array, sptr);
  if (io == OUTPUT_NODE)
    array_insert_last(signal_ptr, output_array, sptr);
    
  Signal_Next2_Order (sptr) = Sig_Num++;

  Name_Signal (nptr) = sptr;
  Signal_Complex_Fanout (sptr) = Signal_Fanout (sptr) = NULL;

  return (sptr);
}

/*******************************************************/
// For Hoshik: It is the responsibility of the user to create his own
// list of signals
/*******************************************************/
signal_ptr Find_Signal (char *name, petri_type io) 
{
  name_ptr nptr;
  signal_ptr sptr;
  array_t *transitions;
  
  nptr = Name_Enter (name, SIGNAL);
  if (Name_Signal (nptr) == SIG_NULL_PTR) {
    sptr = New_Signal (nptr, io);
  }  
  else {
    sptr = Name_Signal (nptr);
    if (io != UNDEFINED) {
      if (Signal_IO (sptr) == UNDEFINED) { // first time the io stat of this signal is defined
        Signal_Trans(sptr) = array_alloc (transition_ptr, 1);
        if (io == PLACE_NODE) 
          array_insert_last(signal_ptr, place_array, sptr);
        if (io == INPUT_NODE)
          array_insert_last(signal_ptr, input_array, sptr);
        if (io == OUTPUT_NODE)
          array_insert_last(signal_ptr, output_array, sptr);
        Signal_IO (sptr) = io;
      } else if (Signal_IO (sptr) != io && io != UNDEFINED) {
        printf ("Redefining io status of signal %s from %d to %d!!\n", 
        	Signal_String (sptr), Signal_IO (sptr), io);
        exit (-1);
      }  
    }
  }      
// No chaining is performed, as opposed to Enter_Signal
  return (sptr);
}

/*******************************************************/
signal_ptr Enter_Signal (char *name, petri_type io)
{
  name_ptr nptr;
  signal_ptr sptr;
  
  nptr = Name_Enter (name, SIGNAL);
  if (Name_Signal (nptr) == SIG_NULL_PTR)
    sptr = New_Signal (nptr, io);
  else 
    sptr = Name_Signal (nptr);  
  Signal_Next (sptr) = temp_signal;
  temp_signal = sptr;
  return (sptr);
}

/*******************************************************/
signal_ptr Enter_Signal_Chain (void) 
{
  signal_ptr sptr;
  sptr = temp_signal;
  temp_signal = NULL;
  return (sptr);
}

/*******************************************************/
// Input signal entry functions
/*******************************************************/
void Enter_Input_Signal (char sign, char *name)
{
  name_ptr nptr;
  signal_ptr sptr;
  
  if (input_cur == input_no) {
    printf ("Too many inputs!");
    exit (-1);
  }     
  nptr = Name_Enter (name, SIGNAL);
  if (Name_Signal (nptr) == SIG_NULL_PTR)
    sptr = New_Signal (nptr, UNDEFINED);
  else 
    sptr = Name_Signal (nptr);
  Input_Signal (temp_input[input_cur]) = sptr;
  Input_Invert (temp_input[input_cur]) = sign;
  input_cur++;
}

/*******************************************************/
input_ptr Enter_Input_Signal_Chain (void) 
{
  input_ptr iptr;
  iptr = temp_input;
  temp_input = NULL;
  input_cur = 0;
  return (iptr);
}

/*******************************************************/
input_ptr Create_Input_ZArray_N (int no)
{
  input_ptr iptr;
	
  iptr = (input_ptr) malloc (sizeof (input) * (no + 1));
  Input_Signal (iptr[no]) = NULL; /* the last element of the array is NULL */  
  return (iptr);  
}

/*******************************************************/
void Push_Input (input_ptr iptr, int mode) 
{
  io_ptr ioptr;
  
  // mode = 0 means initialization; that is, different fields will be provided later
  // mode = 1 means use the current info to create the pushed info

  ioptr = (io_ptr) malloc (sizeof (io_signal)); // June 7
  IO_Next (ioptr) = Input_Pushed (iptr);
  if (mode == 0) {
    IO_Support (ioptr) = malloc (sizeof (gate_ptr));
    IO_Support (ioptr)[0] = NULL;
  }
  if (mode == 1) {
    // To be Filled ........... Compute new changing, support, and stable info based on tr!
    // In computing the Support set, we may need the corresponding _STATE gate so that its places are discluded
  }
  Input_Pushed (iptr) = ioptr;  
}


/*******************************************************/
// External signal entry functions
/*******************************************************/
void Make_External (signal_ptr spptr[]) 
{
  int i, j;
  
  for (i = 0; spptr[i] != NULL; i++) 
    Signal_Type (spptr[i]) = EXTERNAL;
  External_Num = i;  
  Internal_Num = Sig_Num - i;
  internal_list = (signal_ptr *) malloc (sizeof (signal_ptr) * (Internal_Num + 1));  
  for (i = 0, j = 0; i < Sig_Num; i++)
    if (Signal_Type (signal_array[i]) != EXTERNAL) {
      Signal_Type (signal_array [i]) = INTERNAL;  
      internal_list [j++] = signal_array[i];	/* global variable */
    }  
  internal_list[j] = NULL;  
}

/*******************************************************/
void Enter_Level_Externals (signal_ptr sptr)
{
  External_Signals_of_Level[Level_Num] = Signal_Chain_to_ZArray (sptr);
//  printf ("External signals of level %d : \n", Level_Num);
//  Print_ZArray ((void_ptr *) External_Signals_of_Level[Level_Num], SIGNAL);
  if (Level_Num == 0) {
    external_list = External_Signals_of_Level[0];
    Make_External (external_list);
  }
  Level_Num++;
  External_Signals_of_Level[Level_Num] = 0;  
}

/*******************************************************/
/* April 20
void Enter_External_Signals (signal_ptr sptr) 
{	
  external_list = Signal_Chain_to_ZArray (sptr); // global variable 
  Make_External (external_list);
}
*/
/*******************************************************/
// Signal lists of gates functions
/*******************************************************/
signal_ptr *Signal_Chain_to_ZArray (signal_ptr sptr)
{
  int i, max_signal;
  signal_ptr sptr1;
  signal_ptr *spptr;
	
  for (max_signal = 0, sptr1 = sptr; sptr1 != NULL; 
       max_signal++, sptr1 = Signal_Next (sptr1)) {
/*    printf ("%s ", Signal_String (sptr1));*/
  }  
/*  printf ("\n");  */
  
  spptr = (signal_ptr *) malloc (sizeof (signal_ptr) * (max_signal + 1));
  /* NOTICE!!!! This is to keep the correspondance b/w I/O of multioutput gates */  
  for (i = 0, sptr1 = sptr; i < max_signal; i++, sptr1 = Signal_Next (sptr1)) 
    spptr[max_signal-1-i] = sptr1;    
  spptr[i] = NULL; /* the last element of the array is NULL */  
  return (spptr);  
}


/*******************************************************/
void Check_Signals (void)
{
  int i, j;
  signal_ptr  sptr;
  
  for (i = 0; i < Sig_Num; i++) {
    sptr = signal_array[i];
    printf ("\nSignal %s with parent %s has inputs ", Signal_String (sptr),
    		Gate_String (Signal_Parent (sptr)) );
    for (j = 0 ; Input_Signal(Gate_Inputs (Signal_Parent (signal_array[i]))[j]) != NULL; j++)
      printf ("%s ", Signal_String (Input_Signal(Gate_Inputs (Signal_Parent (signal_array[i]))[j])));    
    printf ("with outputs ");
    for (j = 0 ; Gate_Outputs(Signal_Parent (signal_array[i]))[j] != NULL; j++)
      printf ("%s ", Signal_String (Gate_Outputs (Signal_Parent (signal_array[i]))[j]));
  }    
}
