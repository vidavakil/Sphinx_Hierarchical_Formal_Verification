#include "type.h"
#include "sphinx.h"

extern bdd_manager	*Bdd_Man;
extern signal_ptr	order[]; 
extern bdd_t 		*R;
extern bdd_t 		*signal_var[]; 
extern context_ptr	flat_cir;

void bdd_array_free (array_t *arr); 
void Print_Cube (array_t *cube);


/*******************************************************/
// Utility Bdd functions
/*******************************************************/
/*******************************************************/
void free_ite (bdd_t *var, bdd_t *if_term , bdd_t *else_term)
{
  if (var) bdd_free (var);
  if (if_term) bdd_free (if_term);
  if (else_term) bdd_free (else_term);
}

/*******************************************************/
void bdd_array_free (array_t *arr) 
{
  int i;
  bdd_t *temp;
  
  for (i = 0; i < array_n (arr); i++) {  
    temp = (bdd_t *) array_fetch (void_ptr, arr, i);
    bdd_free (temp);
  }
  array_free (arr);  
}

/*******************************************************/
bdd_t *bdd_substitute1 (bdd_t *bdd, array_t *substitute, array_t *quantify)
{
  bdd_t *s, *q, *temp;
  int i;
  
  for (i = 0; i < array_n (substitute); i++) {
    s = array_fetch (bdd_tp, substitute, i);
    q = array_fetch (bdd_tp, quantify, i);
    temp = bdd_compose (bdd, s, q);
    bdd_free (bdd);
    bdd = temp;
  } 
  return (bdd); 
}

/*******************************************************/
void Print_Cube (array_t *cube)
{
  int i, data;
  
  printf ("Cube= ");
  arrayForEachItem (bdd_literal, cube, i, data) 
//      if (i >= flat_cir->_Sig_Num && data != 2 && i < flat_cir->_Sig_Num * 3) // April 6
      if (Signal_Next2_Order (order[i]) != i && data != 2 && i < flat_cir->_Sig_Num * 3) // April 6
      printf ("%s%c=%d ", Signal_String (order[i]), 
      		(i == Signal_Next_Order (order[i])) ? '`' : '\0',
      		data );
  printf ("\n");      		  
}

/*******************************************************/
void Print_Bdd (bdd_t *R, char *s)
{
  int i = 0;
  bdd_gen *gen;
  array_t *cube = NULL;

  printf ("%s: ", s);
  if (R == bdd_zero (Bdd_Man))
    printf ("Empty");
  else if (R == bdd_one (Bdd_Man))
    printf ("Full");
  else {  
    foreach_bdd_cube (R, gen, cube) {
      Print_Cube (cube);
      i++;     
    }
    printf ("# of cubes = %d\n", i);
    if (!i)
      printf ("The size of BDD = %d\n", bdd_size (R));
  }    
}

/*******************************************************/
bdd_t *Exist_Quantify (bdd_t *bdd, signal_ptr *spptr)
{ // it does free bdd
  int i;
  signal_ptr sptr;
  bdd_t *temp;
  array_t *quantify;
  
  if (spptr == NULL || spptr[0] == NULL)
    return (bdd);
  quantify = array_alloc (bdd_tp, 1);

  for (i = 0; (sptr = spptr[i]) != NULL; i++) 
    array_insert_last (bdd_tp, quantify, bdd_get_var (Bdd_Man, Signal_Order (sptr)));  
  temp = bdd_smooth (bdd, quantify); 
  bdd_free (bdd);
  array_free (quantify);
  return (temp); 
}

/*******************************************************/
bdd_t *Exist_Quantify_Array (bdd_t *bdd, array_t *spptr)
{ // it does free bdd
  int i;
  signal_ptr sptr;
  bdd_t *temp;
  array_t *quantify = array_alloc (bdd_tp, 1);

  for (i = 0; i < array_n (spptr); i++) {
    sptr = (signal_ptr) array_fetch (signal_ptr, spptr, i); 
    array_insert_last (bdd_tp, quantify, bdd_get_var (Bdd_Man, Signal_Order (sptr)));
  }    
  temp = bdd_smooth (bdd, quantify); 
  bdd_free (bdd);
  array_free (quantify);
  return (temp); 
}

/*******************************************************/
bdd_t *Exist_Quantify_Next (bdd_t *bdd, signal_ptr *spptr)
{ // it does free bdd
  int i;
  signal_ptr sptr;
  bdd_t *temp;
  array_t *quantify = array_alloc (bdd_tp, 1);

  if (spptr == NULL || spptr[0] == NULL)
    return (bdd);
  for (i = 0; (sptr = spptr[i]) != NULL; i++) 
    array_insert_last (bdd_tp, quantify, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));  
  temp = bdd_smooth (bdd, quantify); 
  bdd_free (bdd);
  array_free (quantify);
  return (temp); 
}

/*******************************************************/
bdd_t *Next_to_Current (bdd_t *temp0, signal_ptr *spptr)
{ // it does free temp0
  int k;
  signal_ptr sptr;
  bdd_t *temp1, *q, *s;
  array_t *quantify, *substitute;
  
  if (spptr == NULL || spptr[0] == NULL)
    return (temp0);
  quantify = array_alloc (bdd_tp, 1);
  substitute = array_alloc (bdd_tp, 1);
     
  // using substitute instead of compose!
  for (k = 0; (sptr = spptr[k]) != NULL; k++) {
    q = bdd_get_var (Bdd_Man, Signal_Order (sptr));
    s = bdd_get_var (Bdd_Man, Signal_Next_Order(sptr));  
    array_insert_last (bdd_tp, quantify, q);
    array_insert_last (bdd_tp, substitute, s);
  }  
  temp1 = bdd_smooth (temp0, quantify);
  bdd_free (temp0); // Notice
  temp0 = bdd_substitute1 (temp1, substitute, quantify);
//  bdd_free (temp1); // New!!
  array_free (quantify);
  array_free (substitute);
  return (temp0);
}      
      
/*******************************************************/
bdd_t *Next_to_Current_Array (bdd_t *temp0, array_t *spptr)
{ // it does free temp0
  int k;
  signal_ptr sptr;
  bdd_t *temp1, *q, *s;
  array_t *quantify, *substitute;
  
  if (spptr == NULL || array_n (spptr) == 0)
    return (temp0);
  quantify = array_alloc (bdd_tp, 1);
  substitute = array_alloc (bdd_tp, 1);
     
  // using substitute instead of compose!
  for (k = 0; k < array_n (spptr); k++) {
    sptr = array_fetch (signal_ptr, spptr, k);
    q = bdd_get_var (Bdd_Man, Signal_Order (sptr));
    s = bdd_get_var (Bdd_Man, Signal_Next_Order(sptr));  
    array_insert_last (bdd_tp, quantify, q);
    array_insert_last (bdd_tp, substitute, s);
  }  
  temp1 = bdd_smooth (temp0, quantify);
  bdd_free (temp0); // Notice
  temp0 = bdd_substitute1 (temp1, substitute, quantify);
//  bdd_free (temp1); // New!!
  array_free (quantify);
  array_free (substitute);
  return (temp0);
}      

/*******************************************************/
bdd_t *Exist_Quantify_Next_Array (bdd_t *bdd, array_t *spptr)
{
  int i;
  signal_ptr sptr;
  bdd_t *temp;
  array_t *quantify = array_alloc (bdd_tp, 1);

  if (spptr == NULL)
    return (bdd);
  for (i = 0; i < array_n (spptr); i++) {
    sptr = array_fetch (signal_ptr, spptr, i);
    array_insert_last (bdd_tp, quantify, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));  
  }  
  temp = bdd_smooth (bdd, quantify); 
  bdd_free (bdd);
  array_free (quantify);
  return (temp); 
}

/*******************************************************/
bdd_t *Make_Bdd_Complete (bdd_t *bdd, gate_ptr gptr)
{
  int j, start, idx;
  signal_ptr sptr;
  bdd_t *temp0, *temp1, *temp2, *temp3, *Ch, *temp4;
  array_t *quantify, *temp_array;
  gate_ptr gptro;

// Feb 28: Notice: If gptr is an _OUT gate and its TR contains the transitions of 
// some of the places of the _STATE gate, then this routine may not work!

  Ch = Gate_Int_Out (gptr) ? Gate_Int_Ch (gptr) : Gate_Ext_Ch (gptr);
  quantify = array_alloc (bdd_tp, 1);
  for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++) // May 7: How will this work for _STATE gates????????????????
    array_insert_last (bdd_tp, quantify, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));  

  temp0 = bdd_and (bdd, Ch, 0, 0);

/* // June 11
  // Feb 28
  if (Gate_Type (gptr) == _OUT && Gate_Ext_Out (Gate_Uncle (gptr)) != NULL) {
    for (j = 0; (sptr = Gate_Changing (gptr)[j]) != NULL; j++)
      if (Signal_Type (sptr) == EXTERNAL) // May 7: Some of the Gate_Changing signals may be internal
        array_insert_last (bdd_tp, quantify, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
        
    // May 7    
    temp4 = bdd_dup (Gate_Stable_Bdd (gptr));
    if (Gate_Int_Out (Gate_Uncle (gptr)) != NULL) {
      temp4 = Exist_Quantify (temp4, Gate_Int_Out (Gate_Uncle (gptr)));
      temp4 = Exist_Quantify_Next (temp4, Gate_Int_Out (Gate_Uncle (gptr)));
    }  
    temp1 = bdd_and (temp0, temp4, 1, 1);
    bdd_free (temp0);
    bdd_free (temp4);
    temp0 = temp1;

//    printf ("Making complete the TR of gate %s\n", Gate_String (gptr));  
//    Print_Bdd (bdd, "");
//    getchar ();

  }  


  // May 28
  for (start = 1; (gptro = Is_External_Input_to_External_State (gptr, &idx, &start)) != NULL; ) {
    if (Gate_Ext_Out (gptro) == NULL) // start becomes a 0 after the first run of the loop
      continue;
    for (j = 0; (sptr = Gate_Ext_Out (gptro)[j]) != NULL; j++)
      array_insert_last (bdd_tp, quantify, bdd_get_var (Bdd_Man, Signal_Next_Order (sptr)));
  }
*/
  temp1 = bdd_smooth (temp0, (array_t *) quantify);
  temp2 = bdd_smooth (bdd, (array_t *) quantify);
  temp3 = bdd_and (temp1, temp2, 1, 0);
  bdd_free (temp1);
  bdd_free (temp2);
  temp1 = bdd_and (temp3, temp0, 1, 1);
  temp2 = bdd_or (bdd, temp1, 1, 1);
  free_ite (temp1, temp3, temp0); // don't free bdd

  array_free (quantify);

  return (temp2);
}
      