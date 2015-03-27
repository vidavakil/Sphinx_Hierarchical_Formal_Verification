// Note: signal_array of cur_cir includes the set of _PLACE signals only at the 
// first level of hierarchy, and even not for the complex circuits.
// June 11: Assign_Gate_Outputs is called only once at the beginnig, but it has a lot of 
// unnecessary code which better be cleared up!

// June 9::::::::::::: Fix From_to_To in every place!!!!, and bring them out of comment!
// May 5: Very Important: In Find_Internally_Unstable and also other places that I check 
// whether a gate is _OUT, I need to check if it is not an INPUT Env element, cause then
// it won't have the places of the corresponding _STATE gate at all!

// Problem in Error Tracing! I don't have a picture of how to move backward when
// having _STATE/_OUT gates! It seems that whenever an output changes back, the 
// state also needs to change back. Also whenever an input changes back, the _STATE
// again needs to change back! 

// Feb 19: For now, Instead of focusing on making the error trace complete, I'd better
// make sure that the hierarchical framework works correctly, by closely following 
// the messages! Also, NO SIGNAL PROJECTION can be performed for now, since I don't
// have a picture of projecting the signals of a PETRINET yet!

/**************************************************************************************/
/**************************************************************************************/
// To make the parser (parser.c/scanner.c) recursive follow the following:
// 0- the %pure_parser directive in the parser.yacc file will take care of the parser.
// 1- add the following include lines right after the first line of the generated 
// scanner.c file (after #include stdio):
// #include "type.h"
// #include "parser.h"
// 1.5- comment similar lines later in the file
// 2- search the prototype yylex() and change it to yylex(YYSTYPE *)
// 3- search the function yylex(void) and change it to yylex(YYSTYPE *yylval)
// 
// To have a second parser/scanner, compile them and change all yy to say vida
// and call vidaparse to incorporate it!
/**************************************************************************************/
/**************************************************************************************/

// Interestingly: for the internal signals, the complete set of next states must be 
// computed, i.e., one which includes the states which do not have any transitions.
// On the other hand, for external signals, only the actual transitions are needed,
// i.e., the ones in which the signal actually changes!

// Important speed-up: We'd better keep an array of the bdd_vars which need to be 
// quantified, instead of creating it each time, to speed up the computations of TRs!!!

// Given an incomplete TR (one which does not have entries for the cases where nothing
// changes), and a given set of reached states R, the set of next states reachable
//                                __ __
// is R'=Shift Ev (R.TR) + Ev' (R.TR.Ch)
// where Ch indicates the situation where the signals of interest do not change.
// We can also be conservative and let R'=R.TR+R, but this way we will have 
// stuttering states!

// When reading in specifications we should define the status of all the internal signals
// of specs. If the name of a spec appears in the list of external signals, then all
// of its internal signals (places) will become extrnal, and v.v.

// It seems that by the new way that specifications are going to be modeled, which 
// will have one gate per output of specification, there is no need to model gates
// where some outputs are internal and some are external. So, I may be able to remove 
// all the complexity associated with that and simply have a field per gate which
// says whether (all of) its outputs are internal/external



// I need to add primitives for internal state variables, and modify the settling function
// and the choice of external signals correspondingly.????????

// It would be greate if Ext_Gates is correctly evaluated, such that in also includes those
// sibling gates that are external. Also, DFS_List better include internal sibling gates!

// Error traces!

// To be fixed: Complex_Gates, MultiOut gates and their TRIO status

// Note! since TRs are now complete, if we are settling internal signals, we may want 
// their changed values, rather than their old values (if they can stay the same) to
// speed up the settling process. This means that we may need to apply Gate_Ch
// to the gate/Env function first!!!!!

// TOOOOOO Important: A circuit block has some inputs, outputs, and input-output signals
// Those which are outputs will always have TRIs for all subsequent subcircuits
// in the hierarchy. Those which are inputs, will always have TROs for all subsequent 
// subcircuits in the hierarchy. However, those which are input-outputs, depending on a
// given sub-subcircuit may become TRI or TRO!
// So, it seems that we need to push TRI/TRO/TRI_TRO status, in addition to other things!
// If it used to be TRI, we keep pushing TRI. If it used to be TRO, we keep pushing TRO.
// If it is still a TRI_TRO we push TRI_TRO; but if it has changed from TRI_TRO to
// either TRI or TRO, we push the right thing!
// The problem arises when it happens that we assign all signals of an Env to TRO!
// Then, the element becomes an autonomous element and can not be stopped, and will
// surely generate hazards/chokes.
// This way, we only need to decide about the TRI_TRO signals (on the stack), and also 
// those which do not have any stack (first become externals!)

// Note! new! Now, the Sibling_Gates may have both internal and external gates, so we should process
// it twice! once for each set!
// Remember to assign EXTERNAL/INTERNAL states to *signals*


// Not a valid comment now!
// The current implementation is useful only when all the external signals of one level
// remain external for the next level. 
// But such a model is not useful for all circuits, e.g., the FIFO circuit!
// It seems that what we really need is SPECIFICATIONS, with set of input and output signals
// where some output signals can be internal and some external (to be consistent/on the safe 
// side, they better all be either external or internal.)
// Such a SPECIFICATION has the current value of all of its outputs. Each time, we need to
// obtain the current value of all of its inputs, and apply its output function to find 
// its next outputs (similar to what we already have).
// The difference here will be that the topology of the network (i.e., its components)
// will change as we go down the hierarchy, cause new elements are introduced to 
// the network (the SPECIFICATIONS), and the connectivities need to be modified!
//

// THIS IS REALLY IMPORTANT: All circuit elements need to have complete output functions,
// that is for all combinations of inputs/current-states, even those which are in the failure
// states. Otherwise, bad initial states will not be captured!!!!
// The same is true for the hazards of gates. Thus those combinations that are possible only
// after an initial hazard need to be in the hazard set.
// An alternative is to have reduced output functions, yet complete hazard functions,
// and then check the presence of hazards at the very beginning for the initial state.


// The hazard set for ME may not be correct! Check it out sometime!

// Very Very Important: It seems that because of the way signals are handled, we cannot 
// have a gate with two input signals which are the same signal!!!!!!


// NOTICE: The PeakVida variable added into the cuddAPI.c and cuddTable.c files is useful only if there is
// just one BDD manager.

// The changed mechanism is not working!!!!

          // NOTICE!!!!! Very important: I am assuming that it is not the case that more than
          // one outputs of a multioutput gate enter a SPEC with even one of them inverted

/* Check to see whether in the flat case the hazards/Int/Ext outputs of gates and the 
   Out_Change fileds are appropriately set */
/* Notice that each set of Ext/Int ouotputs of a gate need to have corresponding "changed"
   variables
*/   


/* use prof, bdd_size, garbage_collection */

/* Need to differentiate between the outputs of a gate, generate the appropriate output 
   lists and initialize the different TRs.
*/   

/*
   Need to find the fanout of gates automatically. before calling Creat_Subcircuits.
   Need to define modules so that their specifications need not be repeated in the input file,
   but just their connectivity.
   Need to implement hierarchy in the verification.
*/   

   
/*
   NOTICE! CURRENTLY, EITHER BOTH OUTPUTS OF MULTIOUTPUT GATES ARE EXTERNAL,
   OR BOTH OF THEM ARE INTERNAL

   Not a valid comment now!
   ALSO, THE BDDS OF BOTH SIGNALS ARE THE SAME, ALL THE TIME, HOWEVER, WITHOUT DUPLICATION,
   I.E., ALWAYS ONE OF THEM IS UPDATED, AND COPIED INTO THE OTHER ONE (WITHOUT BDD_DUP). 
*/   

/* Not a valid comment now!

   Need to implement ME. For this purpose, we need to do the following:
   When operating the gate, we will consider both outputs and their next 
   states in the TR of one of them, but will point both TR's to the same
   thing, at all times. This way, we are assuming that outputs of an ME
   are synchronous. The ME case in the Operate_Gate function will be different
   returning its result from within the case. However, the Find_Enabled 
   function will be tricky.
*/
  


/* I need to be able to generate error traces! */ 

#include "type.h"
#include "parser.h"
#include "sphinx.h"

int		MODE = FLAT;
int		Level_Num = 0;  // The number of levels of hierarchy   
int		Max_External = 0;
int 		fflag = 0, hflag = 0, cflag = 0, rflag = 0, lflag = 0, tflag = 0, gflag = 0, nflag = 0, Cflag = 0,
		xflag = 0, sflag = 0, oflag = 0, iflag = 0, vflag = 0, pflag = 0, zflag = 0, wflag = 0, kflag = 0;
char		Current_Module_Name[100];
gate_ptr	gate_list = GATE_NULL_PTR, spec_list = GATE_NULL_PTR;
signal_ptr	temp_signal = NULL;
signal_ptr	order[MAXSIGNAL*3]; // there was no *3 before Feb 10
signal_ptr	*External_Signals_of_Level[MAXLEVEL];
bdd_t 		*From, *To, *R, *Reached, *init_state;
bdd_t 		*signal_var[MAXSIGNAL*3];  // was *2 before Feb 10
bdd_manager	*Bdd_Man;
context_ptr	flat_cir, complex_cir = NULL, cur_cir /* , sub_cir, temp_cir */; // April 20: commented

// Hoshik structures
array_t *trans_array;
array_t *place_array, *input_array, *output_array; //, *All_place_array; // June 11

name_ptr Name_Enter (char *Name1, int type);
void Initialize (char *name);
void Init_Bdds (void);
void Clear_Bdds (void);
void Print_Help (void);

extern array_t *array_minus(array_t *array1, array_t *array2);
extern void Name_Init_Table (void);


/*******************************************************/
/*******************************************************/
name_ptr Name_Enter (char *Name1, int type)
{
  name_ptr nptr;
  char Name[100];  
  
  sprintf (Name, "%s", Current_Module_Name);
  if (Current_Module_Name[0])
    strcat (Name, "_");
  else
    Name[0] = 0;  
  strcat (Name, Name1);
  nptr = Name_Find_Str (Name);
  if ( nptr == NAME_NULL_PTR ) {
    nptr = Name_Enter_Str (Name);
    Name_Type (nptr) = type;  
  } else if (Name_Type (nptr) != type) {
    printf ("Bad type %d for name %s with type %d\n", type, Name, Name_Type (nptr));
    exit (-1);
  }  
  return (nptr);
}


/*******************************************************/
// Main functions
/*******************************************************/
void Initialize (char *name)
{
  char *chptr;

// hoshik
  trans_array = array_alloc(transition_ptr, 1);
  place_array = array_alloc(signal_ptr, 1); 
  input_array = array_alloc(signal_ptr, 1); 
  output_array = array_alloc(signal_ptr, 1); 
//

  flat_cir = (context_ptr) malloc (sizeof (context));
  flat_cir->_Subcir = NULL;
  flat_cir->_Sibling_Gates[0] = NULL;
  flat_cir->_Parent_Subcir = NULL;
  strcpy (flat_cir->_Subcir_Path, name);
  *(chptr = strchr (flat_cir->_Subcir_Path, '.')) = 0;
  cur_cir = flat_cir;

  temp_signal = SIG_NULL_PTR;
  Gate_Num = 0;
  Spec_Num = 0;
  gate_list = GATE_NULL_PTR;
  spec_list = GATE_NULL_PTR;
  Internal_Num = 0;
  External_Num = 0;
  internal_list = SIG_NULL_ARRAY;
  external_list = SIG_NULL_ARRAY;
  External_Signals_of_Level[Level_Num = 0] = 0;

}  

/*******************************************************/
void Print_Help (void)
{
  printf ("hvsi [-fhCgxso] In_File\n");
  printf ("In_File : the name of the input circuit file\n");
  printf ("     internal module files must be present in the current directory\n");
  printf ("-f : use flat verification\n");
  printf ("-h : use hierarchical verification\n");
  printf ("-x : enable the additional abstraction of complex-gates\n");
  printf ("-s : allow complex gates with memory elements at their outputs\n");
  printf ("     otherwise, complex gates will be pure combinational\n");
  printf ("-C : enable complex gate generation messages\n");
  printf ("-o : enable the generation of state transition graphs (viewable by parg)\n");
  printf ("     for safe abstractions or flat behaviors\n");
  printf ("-g : disable garbage collection\n");
//  printf ("-w : enables the generation of circuit graphs (viewable by parg)\n");
//  printf ("     for the subcircuits of the hierarchy\n");
//  printf ("Out_File  : the name of the output file\n");
//  printf ("-c : enables the event driven option\n");
//  printf ("-r : enables the suboptimal BDD reordering for hierarchical verification\n");
//  printf ("-l : enables levelized ordering of simulation\n");
//  printf ("-t : enables topological ordering of simulation\n");
//  printf ("-i : enables printing of the internal state at the end of Find_Abstract\n");
//  printf ("-p : all places of specification are taken to be internal\n");
//  printf ("-ww : generate the circuit graph in parg format, second version\n");
//  printf ("-n : use output conization for generating sub-circuits\n");

}

/*******************************************************/
void Init_Bdds (void)
{
  int i, j;
  gate_ptr gptr;
  input_ptr iptr;
  signal_ptr sptr;
  
  for (i = 0; i < Gate_Num; i++) {
    gptr = list_of_gates[i];
    Gate_Int_TR (gptr) = bdd_zero (Bdd_Man);
    Gate_Int_TRFROM (gptr) = bdd_zero (Bdd_Man);
    Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
    Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);
    Gate_Complex_Int_F (gptr) = bdd_zero (Bdd_Man);
    Gate_Complex_Ext_F (gptr) = bdd_zero (Bdd_Man);
    Gate_Int_F (gptr) = bdd_zero (Bdd_Man);
    Gate_Ext_F (gptr) = bdd_zero (Bdd_Man);  
    Gate_Int_Ch (gptr) = bdd_zero (Bdd_Man);
    Gate_Ext_Ch (gptr) = bdd_zero (Bdd_Man);          
    Gate_Function (gptr) = bdd_zero (Bdd_Man);
  }
  for (i = 0; i < Spec_Num; i++) { // June 11
    gptr = list_of_specs[i];
    Gate_Int_TR (gptr) = bdd_zero (Bdd_Man);
    Gate_Int_TRFROM (gptr) = bdd_zero (Bdd_Man);
    Gate_Ext_TR (gptr) = bdd_zero (Bdd_Man);
    Gate_Ext_TRFROM (gptr) = bdd_zero (Bdd_Man);
    Gate_Int_Ch (gptr) = bdd_zero (Bdd_Man);
    Gate_Ext_Ch (gptr) = bdd_zero (Bdd_Man); 
    for (j = 0; (sptr = Input_Signal (Gate_State_Inputs (gptr)[j])) != NULL; j++) {
      iptr = &(Gate_State_Inputs (gptr)[j]);
      Input_Ext_TR (iptr) = bdd_zero (Bdd_Man);
    }  
  }
}

/*******************************************************/
// Need to set all Bdds to zero_bdd at the beginning, and freeing a bdd always before assigning to it!
void Clear_Bdds (void)
{
  int i;
  gate_ptr gptr;

  for (i = 0; i < Gate_Num; i++) {
    gptr = list_of_gates[i];
    bdd_free (Gate_Hazards (gptr));
    bdd_free (Gate_Int_Ch (gptr));
    bdd_free (Gate_Ext_Ch (gptr));
    bdd_free (Gate_Int_TR (gptr));
    bdd_free (Gate_Int_TRFROM (gptr));
    bdd_free (Gate_Ext_TR (gptr));
    bdd_free (Gate_Ext_TRFROM (gptr));
    bdd_free (Gate_Int_F (gptr));
    bdd_free (Gate_Ext_F (gptr));
    bdd_free (Gate_Complex_Int_F (gptr));
    bdd_free (Gate_Complex_Ext_F (gptr));
    bdd_free (Gate_Function (gptr));        
  }
  for (i = 0; i < Sig_Num * 3; i++)
    bdd_free (signal_var[i]);
}

/*******************************************************/
void main (int argc, char **argv)
{
  int inx, Verified;
  long start, end;
  extern FILE *yyin;
  

  if ( argc == 1 ) { Print_Help(); return; }
  while (*++argv && **argv == '-') {
    for (inx = 1;  (*argv)[inx] != '\0';  inx ++)
      switch ((*argv)[inx]) {
        case 'f': fflag = 1; break; // flat
	case 'h': hflag = 1; break; // hierarchical
	case 'C': Cflag = 1; break; // enable complex gate reports
	case 'g': gflag = 1; break; // disable garbage collect
	case 'x': xflag = 1; break; // enable complex gate
	case 's': sflag = 1; break; // allow memory elements at the outputs of complex-gates
	case 'o': oflag = 1; break; // enable state transition graph generation
//	case 'r': rflag = 1; break; // reorder BDD vars, otherwise, that of the input file
//	case 'c': cflag = 1; break; // event driven
//	case 'l': lflag = 1; break; // levelized order
//	case 't': tflag = 1; break; // topological order, if not topological/levelized will be random!
//	case 'i': iflag = 1; break; // enable internal state investigation
//	case 'v': vflag = 1; break; // enable verbose mode
//	case 'p': pflag = 1; break; // all places are internal
//	case 'z': zflag = 1; break; // use a XOR tree during projection
//	case 'n': nflag = 1; break; // use conization for subcircuits !!!!!!!!! Discomment it later
//	case 'w': wflag += 1; break; // Generate the circuit graph
        default:  Print_Help(); return;
      }
  }

  if ( *argv ) {
    yyin = fopen (*argv, "r");
    if ( yyin == NULL ) {
      printf ("Cannot open file %s\n", *argv);
      exit (1);
    } 
  } else {
    printf("Input filename missing\n\n");
    Print_Help();
    return;
  }
  
  Name_Init_Table ();
  Initialize (*argv);

  if (yyparse () != 0) {
    printf("Parsing error\n");
    exit(-1);
  }

  Bdd_Man = bdd_start (Sig_Num * 3);
  if (gflag)
    bdd_set_gc_mode (Bdd_Man, 1);

  Prepare_Hierarchical_Verification (hflag ? HIERARCHICAL : FLAT);
  Print_Circuit_Graph (TOTAL);

//  printf ("Just started\n");
/*  Check_Signals ();*/

  if (fflag) {
    start = util_cpu_time ();
    Verified = Flat_Verify ();
    end = util_cpu_time ();
    if (Verified)      
      printf ("Flat circuit verified failure-free\n");    
    else
      printf ("Flat circuit verified failing\n");  
    printf ("Time ellapsed : %ld\n", end - start);  
//    bdd_print_stats (Bdd_Man, stdout);
  }  
  if (hflag) {
    start = util_cpu_time ();
    if (Find_Abstract () && Verify_Old ()) 
      Verified = 1;
    else 
      Verified = 0;  
    end = util_cpu_time ();
    if (Verified) {
      printf ("Hierarchically verified failure-freedom!\n");  
      printf (":-) :-) :-) :-) :-) :-) :-) :-) :-) :-) :-) :-) :-) :-)\n");
    } 
    else {
      printf ("Hierarchically verified failure!\n");
      printf (":-( :-( :-( :-( :-( :-( :-( :-( :-( :-( :-( :-( :-( :-(\n");
    }  
    printf ("Time ellapsed : %ld\n", end - start);
//    bdd_print_stats (Bdd_Man, stdout);
  }
  Clear_Bdds ();
  bdd_print_stats (Bdd_Man, stdout);
  bdd_end (Bdd_Man);
  printf ("Sig_Num=%d\n", Sig_Num);
  printf ("Maximum External Gates in verifying any subcircuit : %d\n", Max_External);
}
