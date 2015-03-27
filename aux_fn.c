#include "type.h"
#include "aux_fn.h"

extern  int 	MODE, oflag, pflag;
extern  bdd_t 	*signal_var[];

/*******************************************************/
signal_ptr get_implicit_place(transition_ptr t1, transition_ptr t2)
{
	char *str;
	signal_ptr sptr;
	char *s1, *s2;

	s1 = Name_String(Trans_Name(t1));	
	s2 = Name_String(Trans_Name(t2));	

	str = (char *)malloc(strlen(s1) + strlen(s2) + 1);
	strcpy(str, &s1[strlen (Current_Module_Name)+1]);
	strcat(str, &s2[strlen (Current_Module_Name)]);
	
	sptr = Find_Signal(str, PLACE_NODE);
	free (str);
	return sptr;
}

/*******************************************************/
transition_ptr Enter_Trans(signal_ptr p1, transition_ptr t, signal_ptr p2)
{  
  if (t != TRANS_NULL_PTR){
    if (p1)
      array_insert_last(signal_ptr, Trans_Preset(t), p1);
    if (p2)  
      array_insert_last(signal_ptr, Trans_Postset(t), p2);
  }
  return(t);
}  

/*******************************************************/
transition_ptr New_Trans(name_ptr nptr)
{
  	transition_ptr tptr;
	
  	tptr = (transition_ptr)malloc(sizeof(Trans));
  	Trans_Name(tptr) = nptr;
  	Trans_Preset(tptr) = array_alloc(signal_ptr, 1);  
  	Trans_Postset(tptr) = array_alloc(signal_ptr, 1); 
  	Name_Transition(nptr) = tptr;
	
	array_insert_last(transition_ptr, trans_array, tptr); /* transition array */ 
 	
	return(tptr);
}
 
/*******************************************************/
transition_ptr Find_Trans(char *name, char *idx, char polar)
{
	char *str;
	name_ptr nptr;
	transition_ptr tptr;
	signal_ptr sptr;

	str = (char *)malloc(strlen(name) +  strlen(idx) + 2);
	strcpy(str, name);
	strcat(str, idx);
	
	if (polar == '+') strcat(str, "+");
	else strcat(str, "-");

	nptr = Name_Enter(str, TRANSITION);
	free (str);

   	if (Name_Transition(nptr) == TRANS_NULL_PTR) {
    		tptr = New_Trans(nptr);

		sptr = Find_Signal(name, UNDEFINED);
		Trans_Signal(tptr) = sptr;

		if (polar == '+') Trans_Polar(tptr) = RISE;
		else Trans_Polar(tptr) = FALL;	

		array_insert_last(transition_ptr, Signal_Trans(sptr), tptr);
	}
  	else
    		tptr = Name_Transition(nptr);

	return(tptr);
}  
  
/*******************************************************/
int array_member (array_t *arr, signal_ptr sptr)
{
  int j;
  signal_ptr sptr1;
  
  for (j = 0; j < array_n (arr); j++) {
    sptr1 = array_fetch (signal_ptr, arr, j);
    if (sptr1 == sptr)
      return (1);
  }
  return (0);    
}

/*******************************************************/
array_t *array_union (array_t *arr1, array_t *arr2)
{
  int j;
  signal_ptr sptr;
  
  for (j = 0; j <array_n (arr2); j++) {
    sptr = array_fetch (signal_ptr, arr2, j);  
    if (!array_member (arr1, sptr))
      array_insert_last (signal_ptr, arr1, sptr);
  }
  return (arr1);    
}

/*******************************************************/
array_t *Not_Pre_Post_Intersect (signal_ptr sptr, array_t *place_array, array_t **result)
{
  int j;
  transition_ptr tptr;
  array_t *pre_post, *temp;
  
      for (j=0; j<array_n(Signal_Trans(sptr)); j++){
        tptr = array_fetch(transition_ptr, Signal_Trans(sptr), j);
	pre_post = array_join(Trans_Preset(tptr), Trans_Postset(tptr));
	if (j == 0)
	  *result = pre_post;
	else {
	  *result = array_union (*result, pre_post);
	  array_free (pre_post);
	}
      }
      temp = array_minus (place_array, *result);
      return (temp);	
}

/*******************************************************/
void Print_Array (array_t *arr)
{
  int i;
  
  for (i = 0; i < array_n (arr); i++) {
    printf ("%s ", Name_String (Signal_Name ((signal_ptr) array_fetch (signal_ptr, arr, i))));
  }
  printf ("\n");
}

/*******************************************************/
bdd_t *create_tr_function (transition_ptr tptr, array_t *stable)
{
	int i;
        bdd_t *place_bdd, *next_place_bdd, *sig_bdd, *next_sig_bdd;
	bdd_t *not_ch_var, *post_var, *pre_var, *trans, *t1, *t2, *one_bdd, *zero_bdd;
	bdd_t *bdd_rise, *bdd_fall;
	signal_ptr vi, trans_sig;
	array_t *postset, *preset, *pre_post, *not_pre_post, *pre_not_post, *not_prepost;
	
	trans_sig = Trans_Signal(tptr);
        t1 = bdd_get_var (Bdd_Man, Signal_Order (trans_sig));
        t2 = bdd_get_var (Bdd_Man, Signal_Next_Order (trans_sig));
        bdd_rise = bdd_and (t1, t2, 0, 1);
        bdd_fall = bdd_and (t1, t2, 1, 0);
	preset = Trans_Preset(tptr);
	postset = Trans_Postset(tptr);
	pre_post = array_join(preset, postset);
	
	not_pre_post = array_minus (stable, pre_post);
	
	pre_not_post = array_minus(preset, postset); // used to be a comment, Feb 3
	t1 = bdd_one(Bdd_Man);
  	for (i=0; i<array_n(not_pre_post); i++){
	  vi = array_fetch(signal_ptr, not_pre_post, i);
	  place_bdd = bdd_get_var(Bdd_Man, Signal_Order(vi));
	  next_place_bdd = bdd_get_var(Bdd_Man, Signal_Next_Order(vi));
	  not_ch_var = bdd_xnor(next_place_bdd, place_bdd);
	  t2 = bdd_and(t1, not_ch_var, 1, 1);
	  bdd_free(t1);
	  bdd_free(not_ch_var);
	  t1 = t2;
	}

	for (i=0; i<array_n(preset); i++){
	  vi = array_fetch(signal_ptr, preset, i);
	  if (vi != SIG_NULL_PTR){
	    post_var = bdd_get_var(Bdd_Man, Signal_Next_Order(vi));
	    pre_var = bdd_get_var (Bdd_Man, Signal_Order(vi));
	    t2 = bdd_and(t1, pre_var, 1, 1);
	    bdd_free(t1);
	    if (!array_member (/* pre_post */postset, vi)) {  // Feb 3
	      t1 = bdd_and (t2, post_var, 1, 0);
	      bdd_free (t2);
	    }  
	    else {
	      t1 = bdd_and (t2, post_var, 1, 1);
	      bdd_free (t2);
	    }  
	  }
	}

	for (i=0; i<array_n (postset); i++){
	  vi = array_fetch (signal_ptr, postset, i);
	  if (vi != SIG_NULL_PTR){
	    post_var = bdd_get_var (Bdd_Man, Signal_Next_Order(vi));
	    pre_var = bdd_get_var (Bdd_Man, Signal_Order(vi));
	    t2 = bdd_and (t1, post_var, 1, 1);
	    bdd_free (t1);
	    if (!array_member (/* pre_post */ preset, vi)) { // Feb 3
	    // NOTICE: I assume that the postset must be empty, cuase otherwise 
	    // the petrinet won't be safe
	      t1 = bdd_and (t2, pre_var, 1, 0);
	      bdd_free (t2);
	    }  
	    else 
	      t1 = t2;
	  }
	}
	array_free (preset);
	array_free (postset);
	array_free (pre_post);
	array_free (not_pre_post);
	array_free (pre_not_post);

       if (Trans_Polar (tptr) == RISE)
         t2 = bdd_and (t1, bdd_rise, 1, 1);
       else
         t2 = bdd_and (t1, bdd_fall, 1, 1);
       bdd_free (bdd_rise);
       bdd_free (bdd_fall);
       bdd_free (t1);
       return t2;
}
			
/*******************************************************/
/*
bdd_t *Set_Stable_Bdd (array_t *arr)
{
  int i;
  signal_ptr 	sptr;
  bdd_t		*var, *next_var, *temp1, *temp2, *temp0;
  
  temp0 = bdd_one (Bdd_Man);
  if (array_n (arr) != 0)
    for (i = 0; i < array_n (arr); i++) {
      sptr = array_fetch (signal_ptr, arr, i);
      next_var = bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)); 
      var = bdd_get_var (Bdd_Man, Signal_Order (sptr));
      temp1 = bdd_xnor (var, next_var);
      temp2 = bdd_and (temp0, temp1, 1, 1);
      bdd_free (temp1);
      bdd_free (temp0);
      temp0 = temp2;
    }
  return (temp0);
}
*/
/*******************************************************/
// Note!!! THings are messed up! Move the different fields to
// the gates from signals!!! For a _STATE element, what we do is that
// we find the Stable, TR, and STR bdds for each input, and keep
// them in the input data structure of that element along with the
// corresponding input signal! However, the TR bdd is different from that 
// of _Out elements. See your notes.

void Hoshik_Signal_State (signal_ptr si, input_ptr iptr, gate_ptr gptr)
{
  int i, j;
  bdd_t *tf_bdd, *tr_bdd, *temp;	
  transition_ptr tj;
  array_t *changing_signals;
  signal_ptr sptr;

 // June 21
/*  Input_Stable (iptr) = */ Not_Pre_Post_Intersect (si, Gate_State_All_Places_Array (gptr), &changing_signals);
/* // June 21
//  if (oflag && !pflag)
    Input_Stable_Bdd (iptr) = Set_Stable_Bdd (Input_Stable (iptr));
*/
  Input_ChangingArray (iptr) = changing_signals;
  Input_Changing (iptr) = (signal_ptr *) Array_to_ZArray (changing_signals);
          
  tr_bdd = bdd_zero(Bdd_Man);
  for (j=0; j<array_n(Signal_Trans(si)); j++){
    tj = array_fetch(transition_ptr, Signal_Trans(si), j);

    tf_bdd = create_tr_function(tj, Input_ChangingArray (iptr)); /*disjunctive TR*/

    temp =  bdd_or(tr_bdd, tf_bdd, 1, 1);
    bdd_free(tr_bdd);
    bdd_free(tf_bdd);
    tr_bdd = temp;
  }  	  
  // Or should it be Gate_TR (Signal_Parent (si)) ??????
  Input_TR(iptr) = tr_bdd; // cause Next_to_Current substitutes its argument!
}

/*******************************************************/
void Hoshik_State (gate_ptr gptr)
{
  int i;
  input_ptr iptr;
  signal_ptr sptr;
  
  for (i = 0; (sptr = Input_Signal (Gate_State_Inputs (gptr)[i])) != NULL; i++) {
    iptr = &(Gate_State_Inputs (gptr)[i]);
    Hoshik_Signal_State (sptr, iptr, gptr); 
  }  
}

/*******************************************************/
array_t *array_minus(array_t *array1, array_t *array2)
{
  int i, j;
  signal_ptr a1, a2;
  var_set_t *var_set;
  array_t *new_array;
  var_set = var_set_new(array_n(array1));
  new_array = array_alloc(signal_ptr, 0);

  for (i=0; i<array_n(array1); i++){
    a1 = array_fetch(signal_ptr, array1, i);
    for (j=0; j<array_n(array2); j++){
      a2 = array_fetch(signal_ptr, array2, j);
      if (a1 == a2) var_set_set_elt(var_set, i);
    }    
  }
  for (i=0; i<array_n(array1); i++){
    if (var_set_get_elt(var_set, i) == 0){
      a1 = array_fetch(signal_ptr, array1, i);
      array_insert_last(signal_ptr, new_array, a1);
    }
  }
  return new_array;
}

/*******************************************************/
void Enter_Mark (signal_ptr sptr)
{
  Signal_Init (sptr) = 1;
}







