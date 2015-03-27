#include "type.h"
#include "sphinx.h"

void Add_List (gate_ptr *gpptr1, gate_ptr *gpptr2);

/*******************************************************/
/*******************************************************/
// ZArray Functions
/*******************************************************/
/*******************************************************/
void Print_ZArray (void_ptr *ptr, int type)
{
  int i;
  
  if (ptr == NULL)
    return;
    
  for (i = 0; ptr[i] != NULL; i++)
    printf ("%s ", (type == SIGNAL) ? 
    		   Signal_String ((signal_ptr) ptr[i]) : 
    		   Gate_String ((gate_ptr) ptr[i]) );
  printf ("\n");    		   
}

/*******************************************************/
void Copy_ZArray (gate_ptr *gptr1, gate_ptr *gptr2)
{
  int i;
  gate_ptr gptr;
  
  for (i = 0; (gptr = gptr2[i]) != NULL; i++) 
    gptr1[i] = gptr2[i];
  gptr1[i] = NULL;  
}

/*******************************************************/
gate_ptr *Duplicate_ZArray (gate_ptr *gptr2)
{
  int i;
  gate_ptr gptr, *gpptr;
  
  if (gptr2 == NULL)
    return (gptr2);
  for (i = 0; (gptr = gptr2[i]) != NULL; i++);
  gpptr = (gate_ptr *) malloc (sizeof (gate_ptr) * (i + 1));
  for (i = 0; (gptr = gptr2[i]) != NULL; i++)
    gpptr[i] = gptr2[i];
  gpptr[i] = NULL;  
  return (gpptr);
}

/*******************************************************/
void *Array_to_ZArray (array_t *arr)
{
// Generates a ZArray of elements from an Array

  void_ptr *sptr, temp;
  int i, j, k;
  
  if (arr == NULL) {
    sptr = (void_ptr *) malloc (sizeof (signal_ptr)); /* just any ptr will work! */
    sptr [0] = NULL;
    return (sptr);
  }  
    
  sptr = (void_ptr *) malloc (sizeof (arr->obj_size) * (array_n (arr) + 1));
  for (i = 0, j = 0; i < array_n (arr); i++) {
    temp = array_fetch (void_ptr, arr, i);    // Note: signal_ptr used instead of (void *)
    for (k = 0; k < j; k++)
      if (sptr [k] == temp)
        break;
    if (k == j)
      sptr [j++] = temp;
  }        
  sptr[j] = NULL;  
  return (sptr);
    
}

/*******************************************************/
void Append_ZArray_to_Array (array_t *arr, signal_ptr *spptr)
{
  int i;
  
  if (spptr == NULL)
    return;
    
  for (i = 0; spptr[i] != NULL; i++)
    array_insert_last (signal_ptr, arr, spptr[i]);
}

/*******************************************************/
void *Array_to_ZArray_Free (array_t *arr) 
{
  void *pptr;
  
  pptr = Array_to_ZArray (arr);
  array_free (arr);
  return (pptr);
}

/*******************************************************/
int Is_Array_Member (gate_ptr gptr, array_t *arr)
{
  int i;
  gate_ptr tempg;
  
  if (arr == NULL)
    return (0);
  for (i = 0; i < array_n (arr); i++) {  
    tempg = (gate_ptr) array_fetch (gate_ptr, arr, i);
    if (tempg == gptr)
      return (1);
  }  
  return (0);
}

/*******************************************************/
int Is_ZArray_Member (gate_ptr gptr, gate_ptr *gpptr)
{
  int i;
  gate_ptr gptr1;
  
  for (i = 0; (gptr1 = gpptr[i]) != NULL; i++) {
    if (gptr1 == gptr)
      return (1);
  }
  return (0);   
}

/*******************************************************/
void Add_List (gate_ptr *gpptr1, gate_ptr *gpptr2) 
{
  int i, j, k, size;
  gate_ptr gptr, gtemp;
  
  for (i = 0; (gptr = gpptr1[i]) != NULL; i++);
  size = i;
  for (j = 0; (gptr = gpptr2[j]) != NULL; j++) {
    for (k = 0; (gtemp = gpptr1[k]) != NULL; k++)
      if (gtemp == gptr)
        break;
    if (k == size)    
      gpptr1[i++] = gptr;
  }
  gpptr1[i] = NULL;    
}
