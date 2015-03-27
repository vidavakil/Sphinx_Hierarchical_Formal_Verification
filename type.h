// Notice io-type is redefined!!!

#ifndef TYPEH
#define TYPEH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bdd.h"
#include "cmuPortInt.h"
#include "names.h"
#include "struct.h"
#include "array.h"
#include "util.h"
#include "st.h"

#define  UNDEFINED	0

#define  MAX_SUBCIR	40
#define	 MAXSIGNAL	600
#define  MAXGATE	300
#define  MAXLEVEL	10

#define  COMPLEX 	1
#define  FLAT	  	0
#define  HIERARCHICAL 	2


#define  TOTAL    0
#define  PARTIAL  1

#define	 CURRENT  0
#define  NEXT	  1

#define  BLACK	0
#define  WHITE  1
#define  GRAY	2

#define  FORWARD 0
#define  BACKWARD 1

#define  DD_VERBOSE


typedef bdd_t  	*bdd_tp;
typedef void 	*void_ptr;


/*********************************************************************/
typedef struct reached_type {
	struct reached_type 	*next;
	bdd_t 			*bdd;
} reached_bdd; 

#define Reached_Next(x)		((x)->next)	
#define Reached_Bdd(x)		((x)->bdd)

/*********************************************************************/
enum polarity {RISE, FALL};

typedef struct tran_type {
	char 			*source;
	char 			*destination;
	char 			*transition;
} tran; 

#define Tran_Source(x)		((x)->source)
#define Tran_Destination(x) 	((x)->destination)
#define Tran_Transition(x)   	((x)->transition)	        

/*********************************************************************/
typedef struct module_type {
	struct name_type 	*name;
	struct name_type 	*type;
	signal_ptr	 	*bdd_order;
} module;

#define Module_Name(x) 		((x)->name)
#define Module_Type(x) 		((x)->type)
#define Module_BddOrd(x)  	((x)->bdd_order)

/*********************************************************************/
typedef struct pushed_bdd_type {
	struct pushed_bdd_type 	*next;
	bdd_t 			*bdd;
} pushed_bdd; 

#define Pushed_Bdd_Next(x)		((x)->next)	
#define Pushed_Bdd_Bdd(x)		((x)->bdd)

/*********************************************************************/
typedef struct io_signal_type {
//	bdd_t			*TR;
	gate_ptr		*support;
	array_t			*changing_arr;
	struct signal_type 	**changing;
	struct io_signal_type   *next;
} io_signal, *io_ptr;

//#define IO_TR(x)		((x)->TR)
#define IO_Support(x)		((x)->support)
#define IO_ChangingArray(x)	((x)->changing_arr)
#define IO_Changing(x)		((x)->changing)
#define IO_Next(x)		((x)->next)

/*********************************************************************/
typedef struct input_type {
	struct signal_type 	*sptr; // was moved from the end to here on Feb 1st
	char			invert;    /* change to int for optimization */
	bdd_t			*Ext_TR; // June 17
	bdd_t			*dummy;
	bdd_t			*projected;
	struct io_signal_type	*pushed_io; // June 7
	struct pushed_bdd_type	*pbdd; // June 7
} input; 	

#define Input_Signal(x) 	((x).sptr)
#define Input_Invert(x) 	((x).invert)
#define Input_Pushed(x)		((x)->pushed_io)
#define Input_Dummy(x)		((x)->dummy)
#define Input_TR(x)		((x)->pbdd->bdd)
#define Input_Pushed_TR(x)	((x)->pbdd)
#define Input_Ext_TR(x)		((x)->Ext_TR)
#define Input_ChangingArray(x) 	((x)->pushed_io->changing_arr)
#define Input_Changing(x) 	((x)->pushed_io->changing)
#define Input_Support(x)	((x)->pushed_io->support)
#define Input_Next(x)		((x)->pushed_io->next)
#define Input_Projected(x)	((x)->projected)

/*********************************************************************/
typedef enum {
	NOTR, // UNDEFINED
	TRI,
	TRO,
	TRIO
} io_type;

typedef struct rec_type {
	struct rec_type 	*next;
	int 	   		type;
	io_type    		io;
	gate_ptr 		*support; // was signal_ptr!
	bdd_t	   		*bdd;
} record; 

#define Record_Next(x)		((x)->next)
#define Record_Type(x)  	((x)->type)
#define Record_IO(x)		((x)->io)
#define Record_Support(x) 	((x)->support)
#define Record_Bdd(x)		((x)->bdd)

/*********************************************************************/
typedef struct status_type {
	struct 	status_type  	*next;
	io_type	 		io;
} status; 

#define Status_Next(x)		((x)->next)
#define Status_IO(x)		((x)->io)

/*********************************************************************/
typedef struct stabilize_type {
	bdd_t	*from_ius;
	bdd_t	*from_is; /* also used for the verification of subcircuits */
} stable; 

typedef struct transit_type {
	bdd_t	*tr;
	bdd_t	*tr_from; /* also used for the verification of subcircuits */
} transit; 		

typedef enum {
	_BUFFER, _NOT, 
	_AND, _NAND, 
	_OR, _NOR,
	_AOI12, _AOI22,
	_OAI12, _OAI22,
	_TABLE1, 
	_C_ELEMENT, 
	_ME, _FF, 
	_SPEC, 
	_STATE, _OUT
} component_type;

/*********************************************************************/
typedef struct cone_type {
	struct gate_type   *gptr;
	struct cone_type   *next;
} cones, *cones_ptr;

#define Cone_Gate(x)		((x)->gptr)
#define Cone_Next(x)		((x)->next)

/*********************************************************************/
typedef struct subcir_chain_type {
	struct subcircuit_type	*subcir;
	struct subcir_chain_type *next;
} subcir_chain, *subcir_chain_ptr;
	
#define SubChain_Subcir(x)	((x)->subcir)
#define SubChain_Next(x)	((x)->next)

/*********************************************************************/
// June 7
typedef struct spec_places_type {
	array_t			*all_places_array;
	struct signal_type	**all_places;
	struct signal_type	**int_places;
	struct signal_type	**ext_places;
	struct spec_places_type *next;
} spec_places, *spec_place_ptr;

#define Spec_All_Places(x)	((x)->all_places)
#define Spec_Int_Places(x)	((x)->int_places)
#define Spec_Ext_Places(x)	((x)->ext_places)
#define Spec_Next(x)		((x)->next)

/*********************************************************************/
// June 7
typedef struct spec_type {
	input			*input_list; 
//	input			*output_list;
	struct spec_places_type *places; // to be pushed at each level 
} spec, *spec_ptr;

/*********************************************************************/
typedef struct gate_type {
	struct gate_type 	*next;
        struct name_type 	*name;
	component_type		type;
	
	int			out_index;
	
	struct spec_type	*spptr; // June 7: _STATE structures
	
	component_type		complex_type;
	struct gate_type 	*uncle; 
	struct gate_type 	**niece;
	struct subcir_chain_type	*subcircuit;    /* pointer to the subcircuit containing this gate */
//	struct subcircuit_type	*subcircuit;    /* pointer to the subcircuit containing this gate */
	int			comb, changed;
	signal_ptr		*output_list;
	signal_ptr		*hidden_list; // list of all state bits
	input			*input_list; // support_list for specifications
	input			*complex_input_list;
	signal_ptr		*internal_outputs;
	bdd_t			*internal_output_function;
	bdd_t			*complex_internal_output_function; // Dec3: do we need such bdds for state functions?
	bdd_t			*int_changed;
	bdd_t			*all_tr;
	array_t			*all_changing;
	bdd_t			*all_ch;
	union {
		transit	tptr;
		stable	sptr;
	      } 		abstract_internal;
	signal_ptr		*external_outputs; 
	bdd_t			*external_output_function;
	bdd_t			*complex_external_output_function;
	bdd_t			*ext_changed;
	union {
		transit	tptr;
		stable	sptr;
	      } 		abstract_external;
	bdd_t			*total_tr;
	bdd_t			*total_tr_from;
	bdd_t			*function;
	bdd_t			*input_hazard; /* hazard condition for each of the inputs */
	struct rec_type		*tri_tro;
	struct status_type  	*stat;
	bdd_t			*reached;
	bdd_t			*backwardTR, *forwardTR;	      	
	bdd_t			*projected;
	bdd_t			*ptr;	
	bdd_t			*TR;
	bdd_t			*synctr;
        int			level, color;
        int			hlevel;
	int			marked, visited, visible, transitive;
	int			is_a_cone;
	struct cone_type	*list_of_cones;
	struct gate_type	*next_cone;
} gate;

#define Gate_Next(x)		((x)->next)
#define Gate_Name(x)		((x)->name)
#define Gate_Type(x)		((MODE == COMPLEX) ? (x)->complex_type : (x)->type)
#define Gate_Complex_Type(x) 	((x)->complex_type)
#define Gate_Out_Index(x) 	((x)->out_index)
#define Gate_State(x)		((x)->spptr)
#define Gate_State_Inputs(x)	((x)->spptr->input_list)
//#define Gate_State_Outputs(x)	((x)->spptr->output_list)
#define Gate_State_Places(x)	((x)->spptr->places)
#define Gate_State_All_Places(x)	((x)->spptr->places->all_places)
#define Gate_State_All_Places_Array(x)	((x)->spptr->places->all_places_array)
#define Gate_State_Int_Places(x)	((x)->spptr->places->int_places)
#define Gate_State_Ext_Places(x)	((x)->spptr->places->ext_places)
#define Gate_Uncle(x)		((x)->uncle) 
#define Gate_Niece(x)		((x)->niece)
#define Gate_Subcircuit(x)	((x)->subcircuit)
#define Gate_Subcir(x)		((x)->subcircuit->subcir)
#define Gate_Subcir_Next(x)	((x)->subcircuit->next)
#define Gate_Comb(x)    	((x)->comb)
#define Gate_Changed(x)		((x)->changed)
#define Gate_Outputs(x) 	((x)->output_list)
#define Gate_Hidden(x)		((x)->hidden_list)
#define Gate_Inputs(x)		((MODE == COMPLEX) ? (x)->complex_input_list : (x)->input_list)
#define Gate_Complex_Inputs(x)  ((x)->complex_input_list)
#define Gate_Int_Out(x) 	((x)->internal_outputs)
#define Gate_Int_F(x)		((MODE == COMPLEX) ? (x)->complex_internal_output_function : (x)->internal_output_function)
#define Gate_Complex_Int_F(x)  	((x)->complex_internal_output_function)
#define Gate_Int_Ch(x)		((x)->int_changed)
#define Gate_Int_TR(x)	 	((x)->abstract_internal.tptr.tr)
#define Gate_Int_TRFROM(x) 	((x)->abstract_internal.tptr.tr_from)
#define Gate_Int_IUS(x)	 	((x)->abstract_internal.sptr.from_ius)
#define Gate_Int_IS(x)	 	((x)->abstract_internal.sptr.from_is)
#define Gate_All_TR(x)		((x)->all_tr)
#define Gate_All_Changing(x)	((x)->all_changing)	
#define Gate_All_Ch(x)		((x)->all_ch)
#define Gate_Tot_TR(x)		((x)->total_tr)
#define Gate_Tot_TRFROM(x)	((x)->total_tr_from)
#define Gate_Ext_Out(x) 	((x)->external_outputs)
#define Gate_Ext_F(x)		((MODE == COMPLEX) ? (x)->complex_external_output_function : (x)->external_output_function)
#define Gate_Complex_Ext_F(x) 	((x)->complex_external_output_function)
#define Gate_Ext_Ch(x)		((x)->ext_changed)
#define Gate_Ext_TR(x)	 	((x)->abstract_external.tptr.tr)
#define Gate_Ext_TRFROM(x) 	((x)->abstract_external.tptr.tr_from)
#define Gate_Function(x)	((x)->function)
#define Gate_Hazards(x)		((x)->input_hazard)
#define Gate_Env(x)		((x)->tri_tro)
#define Gate_Env_Stat(x)	((x)->stat)
#define Gate_TRI_T(x)		((x)->stat->io == TRI)
#define Gate_TRO_T(x)		((x)->stat->io == TRO)
#define Gate_TRIO_T(x)		((x)->stat->io == TRIO)
#define Gate_TR_NONE(x)		((x)->stat->io == NOTR)
#define Gate_Set_TRI(x)		(x)->stat->io = TRI
#define Gate_Set_TRO(x)		(x)->stat->io = TRO
#define Gate_Set_TRIO(x)	(x)->stat->io = TRIO
#define Gate_Set_NONE(x)	(x)->stat->io = NOTR
#define Gate_TRI_TRO(x)		((x)->tri_tro->bdd)
#define Gate_TRO(x)		((x)->tri_tro->bdd)
#define Gate_TRI(x)		((x)->tri_tro->bdd)
#define Gate_TRI_TRO_List(x)	((x)->tri_tro)
#define Gate_Support(x)		((x)->tri_tro->support)
#define Gate_Reached(x)   	((x)->reached)
#define Gate_BWDTR(x)		((x)->backwardTR)
#define Gate_FWDTR(x)		((x)->forwardTR)
#define Gate_Projected(x)	((x)->projected)
#define Gate_PTR(x)		((x)->ptr)
#define Gate_TR(x)		((x)->TR)
#define Gate_SyncTR(x)		((x)->synctr)
#define Gate_Level(x)   	((x)->level)
#define Gate_Color(x)		((x)->color)
#define Gate_HLevel(x)		((x)->hlevel)
#define Gate_Marked(x)		((x)->marked)
#define Gate_Visited(x)   	((x)->visited)
#define Gate_Visible(x)   	((x)->visible)
#define Gate_Transitive(x)	((x)->transitive)
#define	Gate_Cones(x)		((x)->list_of_cones)
#define Gate_Is_Cone(x)		((x)->is_a_cone)
#define Gate_Next_Cone(x)	((x)->next_cone)
#define GATE_NULL_PTR   	((gate_ptr)NULL)

/*********************************************************************/
typedef struct spec_index_type {
	gate_ptr gptr;
	int	 index;
	struct spec_index_type *next;
} spec_index, *spec_index_ptr;

#define Spec_Index_Spec(x)	((x)->gptr)
#define Spec_Index_Index(x)	((x)->index)
#define Spec_Index_Next(x)	((x)->next)

/*********************************************************************/
typedef enum {
	INTERNAL = 1,
	EXTERNAL,
	INPUT,
	OUTPUT
} sig_type;

typedef enum {
	INPUT_NODE = 1, OUTPUT_NODE, PLACE_NODE,
	INTERNAL_NODE, DUMMY_NODE
} petri_type;

typedef struct signal_type {
	struct signal_type 	*next;
        struct name_type 	*name;
	sig_type		type; /* external/internal */ /* an external signal can be input
				 	 (output) of one subcircuit and output (input) of another one */
        petri_type		io;
	int			initial;
	int			Order;
	int			next_order;
	int			next2_order;
	int 			proj;
	gate_ptr		parent;
	gate_ptr		*fanout;
	struct spec_index_type	*spec_fanout;
	gate_ptr		*complex_fanout;
	bdd_t			*tr;
	bdd_t			*str;
        array_t         	*transitions; /* array of corresponsing signal transitions */
        int			index;
        int			visited;
} signal;

#define Signal_Next(x)	 	((x)->next)
#define Signal_Name(x)	 	((x)->name)
#define Signal_Type(x)	 	((x)->type)
#define Signal_IO(x)	 	((x)->io)
#define Signal_Init(x)   	((x)->initial)
#define Signal_Order(x)  	((x)->Order)
#define Signal_Next_Order(x)  	((x)->next_order)
#define Signal_Next2_Order(x)  	((x)->next2_order)
#define Signal_Proj(x)		((x)->proj)
#define Signal_Parent(x) 	((x)->parent)
#define Signal_Fanout(x) 	((MODE == COMPLEX) ? (x)->complex_fanout : (x)->fanout)

#define Signal_SpecFanout(x) 	((x)->spec_fanout)
#define Signal_SpecFanoutGate(x) ((x)->spec_fanout->gptr)
#define Signal_SpecFanoutIndex(x)	((x)->spec_fanout->index)

#define Signal_Complex_Fanout(x) ((x)->complex_fanout)
#define Signal_TR(x)		((x)->tr)
#define Signal_STR(x)		((x)->str)
#define Signal_Trans(x)         ((x)->transitions)
#define Signal_Index(x)		((x)->index)
#define Signal_Visited(x)	((x)->visited)
#define SIG_NULL_PTR	 	((signal_ptr)NULL)
#define SIG_NULL_ARRAY   	((signal_ptr *)NULL)

/*********************************************************************/
typedef struct subcircuit_type {
	struct subcircuit_type 	*next;
        struct name_type 	*name;
	gate_ptr		*gates;
	array_t			*gates_array;
	signal_ptr		*internals;
	signal_ptr		*inputs;
	signal_ptr		*outputs;
//	signal_ptr		*places, *ext_places;
	gate_ptr		*siblings;
	array_t			*internals_array;
	array_t			*inputs_array;
	array_t			*outputs_array;
	array_t			*siblings_array;
//	array_t			*places_array;
//	array_t			*ext_places_array;
} subcircuit;

#define Subcir_Name(x)	   	 ((x)->name)
#define Subcir_Gates(x)	    	 ((x)->gates)
#define Subcir_Internals(x) 	 ((x)->internals)
#define Subcir_Inputs(x)    	 ((x)->inputs)
//#define Subcir_Places(x)	 ((x)->places)
//#define Subcir_Ext_Places(x)	 ((x)->ext_places)
#define Subcir_Siblings(x)	 ((x)->siblings)
#define Subcir_Outputs(x)   	 ((x)->outputs)
#define Subcir_InternalsArray(x) ((x)->internals_array)
#define Subcir_InputsArray(x)    ((x)->inputs_array)
#define Subcir_OutputsArray(x)   ((x)->outputs_array)
#define Subcir_GatesArray(x)     ((x)->gates_array)
#define Subcir_SiblingsArray(x)  ((x)->siblings_array)
//#define Subcir_PlacesArray(x)    ((x)->places_array)
//#define Subcir_ExtPlacesArray(x) ((x)->ext_places_array)
#define Subcir_Next(x)      	 ((x)->next)
#define SUB_NULL_PTR	    	 ((subcir_ptr)NULL)
	
/*********************************************************************/
typedef struct context_type {
	char			_Subcir_Path[30];
	subcir_ptr		_Subcir, _subcir_list[MAX_SUBCIR];
	struct context_type	*_Parent_Subcir;
	int 			_Sig_Num, _Gate_Num, _Spec_Num, _Subcir_Num;
	int			_Internal_Num, _External_Num;
	int			_DFS_Count, _Ext_Gates_Count;
	signal_ptr 		_signal_array[MAXSIGNAL];
	signal_ptr		*_internal_list , *_external_list;
	array_t			*_place_array, *_ext_place_array;
	int			_projected_ext_places;
	gate_ptr   		*_list_of_gates, *_All_Externals, *_list_of_specs;
	gate_ptr		_DFS_List[MAXGATE], _Ext_Gates[MAXGATE];
	gate_ptr		_Sibling_Gates[MAXGATE];
	gate_ptr		*_In_Gates, *_Out_Gates;
} context;	
	
#define 	Subcir_Path	cur_cir->_Subcir_Path
#define 	Subcir		cur_cir->_Subcir
#define		Parent_Subcir	cur_cir->_Parent_Subcir
#define 	subcir_list	cur_cir->_subcir_list
#define 	Sig_Num		cur_cir->_Sig_Num
#define		Gate_Num	cur_cir->_Gate_Num
#define		Spec_Num	cur_cir->_Spec_Num
#define		Subcir_Num	cur_cir->_Subcir_Num
#define		Internal_Num	cur_cir->_Internal_Num
#define		External_Num	cur_cir->_External_Num
#define		DFS_Count	cur_cir->_DFS_Count
#define		Ext_Gates_Count	cur_cir->_Ext_Gates_Count
#define		signal_array	cur_cir->_signal_array
#define		internal_list	cur_cir->_internal_list
#define		external_list	cur_cir->_external_list
#define 	Place_Array	cur_cir->_place_array
#define		Ext_Place_Array cur_cir->_ext_place_array
#define		list_of_gates	cur_cir->_list_of_gates
#define		list_of_specs	cur_cir->_list_of_specs
#define		All_Externals	cur_cir->_All_Externals
#define		DFS_List	cur_cir->_DFS_List
#define		Ext_Gates	cur_cir->_Ext_Gates
#define		Sibling_Gates	cur_cir->_Sibling_Gates
#define		In_Gates	cur_cir->_In_Gates
#define		Out_Gates	cur_cir->_Out_Gates
#define		Ext_Places_Projected cur_cir->_projected_ext_places

/*********************************************************************/
typedef struct transition_type
{
  struct name_type 		*name; 		/*Need to change the name structure*/
  array_t 			*preset; 	/*array of places*/
  array_t 			*postset; 	/*array of places*/
  signal_ptr 			sig_var; 	/*corresponding signal_ptr*/
  int 				polarity;
} Trans;

#define Trans_Name(x)       	((x) -> name)
#define Trans_Polar(x)     	((x) -> polarity)
#define Trans_Signal(x)    	((x) -> sig_var)
#define Trans_Preset(x)    	((x) -> preset)
#define Trans_Postset(x)    	((x) -> postset)
#define TRANS_NULL_PTR      	((transition_ptr)NULL)

#define  bdd_get_var(man,ord)	(signal_var[ord])
#define  Signal_String(x)       (Name_String(Signal_Name(x)))
#define  Gate_String(x)		(Name_String(Gate_Name(x)))
#define  Subcir_String(x)	(Name_String(Subcir_Name(x)))
#define  Module_String(x)	(Name_String(Module_Name(x)))
	
#include "parser.h"

#endif
	      		
	
	
