#include "type.h"
#include "sphinx.h"

FILE		*OutFile;
char		OutFileName[100];
int		print_sig_num = 0;
st_table 	*state_table, *tran_table;

extern int	Cur_Level;
extern int	oflag, wflag;  
extern bdd_manager	*Bdd_Man;
extern int	MODE;
extern context_ptr	cur_cir, flat_cir; 


int Print_State (char *key, char *value, char *arg); 
int Print_Tran (char *key, char *value, char *arg); 
int Free_State (char *key, char *value, char *arg);
int Free_Tran (char *key, char *value, char *arg);
void To_Hex (char *str);
void Process_Minterm (array_t *cube, gate_ptr gptr, signal_ptr *_Ext_Signals, array_t *s_array);
void Process_Cube (array_t *cube, gate_ptr gptr, int index, signal_ptr *_Ext_Signals, array_t *s_array);
int state_cmp (char *s1, char *s2);
int tran_cmp (tran_ptr tptr1, tran_ptr tptr2);
int state_hash (char *s1, int size);
int tran_hash (tran_ptr tptr, int size);
void Print_CRT1 (gate_ptr *_Ext_Gates);
void Print_CRT2 (gate_ptr *_Ext_Gates);
void Print_CRT3 (gate_ptr *_Ext_Gates);
void Preprocess_Cube (array_t *cube, array_t *s_array, signal_ptr *_Ext_Signals);
void Print_PRG (gate_ptr *_Ext_Gates, signal_ptr *_Ext_Signals, int mode);


/*******************************************************/
// State graph generation functions for PARG
/*******************************************************/
/*******************************************************/
int Print_State (char *key, char *value, char *arg) 
{
  fprintf (OutFile, "N F %s %s\n", key, key);
  return (ST_CONTINUE);
}

/*******************************************************/
int Print_Tran (char *key, char *value, char *arg) 
{
  fprintf (OutFile, "N E T%d %s\n", *((int *)arg), Tran_Transition ((tran_ptr) key));
  fprintf (OutFile, "E P %s 0 T%d 0\n", Tran_Source ((tran_ptr) key), *((int *)arg));
  fprintf (OutFile, "E P T%d 0 %s 0\n", *((int *)arg), Tran_Destination ((tran_ptr) key));
  (*((int *)arg))++;
  return (ST_CONTINUE);
}

/*******************************************************/
int Free_State (char *key, char *value, char *arg) 
{
  if (key) {
    free (key);
    key = NULL;
  }   
  return (ST_CONTINUE);
}

/*******************************************************/
int Free_Tran (char *key, char *value, char *arg) 
{
  if (Tran_Transition ((tran_ptr) key)) {
    free (Tran_Transition ((tran_ptr) key));
    Tran_Transition ((tran_ptr) key) = NULL;
  }  
  return (ST_CONTINUE);
}

/*******************************************************/
void To_Hex (char *str)
{
  char *str1;
  int size1, size2, num, i, k, j, n, l;
  
  size1 = strlen (str);
  if (size1 < 20) 
    n = 5;
  else 
    n = 6;  
  size2 = ((size1-1)/n) + (((size1-1)%n) ? 1 : 0);
  
  str1 = malloc (size2 * n + 1 + 1); // 1 for S, and one for the final NULL
  strcpy (str1, str);
  for (i = size1; i <= size2 * n; i++) // Note that the first character is S, thus we go upto size2
    str1[i] = '0';
  str1[i] = 0;  

  for (i = 1, j = 1; i <= size2; i++) { 
    for ((num = (str1[j]) == '0') ? 0 : 1, k = 1, l = 2, j++; k < n; k++, j++, l *= 2)
      num = ((str1[j] == '0') ? 0 : 1) * l + num;
    if (num < 10)
      str[i] = '0'+num;
    else {
      num -= 10;
      if (num < 26)
        str[i] = 'a'+num;
      else {
        num -= 26;
        if (num < 26)
          str[i] = 'A'+num;
        else {
          num -= 26;
          if (num == 0)
            str[i] = '_';
          else
            str[i] = '-';
        }      
      }        
    }  
  }
  str[i] = 0;
  free (str1);      
}

/*******************************************************/
// Because of the use of external_list and External_Num which 
// is not updated across subcircuits, this function can be 
// used only at the highest level of hierarchy for now!

void Print_A_State (bdd_t *bdd, int mode)
{
  int data, i, k, g, j;
  signal_ptr sptr, spptr[MAXSIGNAL];
  char str1[200];
  array_t *cube = NULL;
  bdd_gen *gen;
  gate_ptr gptr, *tempg;

// June 23
  for (i = 0; (sptr = flat_cir->_signal_array[i]) != NULL; i++)
    Signal_Index (sptr) = -1;
  if (mode == PARTIAL) {
    k = 0;
    for (g = 0, tempg = Ext_Gates; g < 2; g++) {
      for (i = 0; (gptr = tempg[i]) != NULL; i++) {
        if (Gate_Ext_Out (gptr) == NULL)
          continue;
        for (j = 0; (sptr = Gate_Ext_Out (gptr)[j]) != NULL; j++) {
          Signal_Index (sptr) = k;
          spptr[k++] = sptr;
        }  
      }
      if (g == 0)
        tempg = Sibling_Gates;
    }
    for (i = 0; i < array_n (Ext_Place_Array); i++) {
      sptr = array_fetch (signal_ptr, Ext_Place_Array, i);
      Signal_Index (sptr) = k;
      spptr[k++] = sptr;
    }  
  }
  else {
    k = 0;
    for (g = 0, tempg = Ext_Gates; g < 3; g++) {
      for (i = 0; (gptr = tempg[i]) != NULL; i++) {
        for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++) {
          Signal_Index (sptr) = k;
          spptr[k++] = sptr;
        }  
      }
      if (g == 0)
        tempg = Sibling_Gates;
      else if (g == 1)
        tempg = DFS_List;  
    }
    for (i = 0; i < array_n (Place_Array); i++) {
      sptr = array_fetch (signal_ptr, Place_Array, i);
      Signal_Index (sptr) = k;
      spptr[k++] = sptr;
    }  
  }
  spptr[k] = NULL;
  print_sig_num = k;

  cube = NULL;
  foreach_bdd_cube (bdd, gen, cube) {
    str1[0] = 'S'; //Feb 27
    for (i = 0; (sptr = spptr[i]) != NULL; i++) {
      data = array_fetch (bdd_literal, cube, Signal_Order (sptr));
      str1[i+1] = (data == 0) ? '0' : (data == 1) ? '1' : '2';
    }
    str1[i+1] = 0;  
    
    To_Hex (str1);
    printf ("State = %s\n", str1);
  }
}

/*******************************************************/
void Process_Minterm (array_t *cube, gate_ptr gptr, signal_ptr *_Ext_Signals, array_t *s_array)
{
  int data, data1, data2, i, j, k, num = 0, reg_num;
  signal_ptr sptr, sptr1, *spptr;
  char str1[200], str2[200], str[200];
  char **s1, **s2;
  char *c1, *c2;
  tran_ptr  tr, *tr1;
  
// Notice: The size of the strings better be chosen more carefuly!

  str1[0] = str2[0] = 'S'; //Feb 27
  for (i = 0; (sptr = _Ext_Signals[i]) != NULL; i++) {
    data = array_fetch (bdd_literal, cube, Signal_Order (sptr));
    str1[i+1] = str2[i+1] = (data == 0) ? '0' : (data == 1) ? '1' : '2';
    if (data == 2) {
      printf ("How come %s for %s? External_Num = %d, sig_num = %d\n", 
      		Signal_String (sptr),
      		Gate_String (gptr),
      		External_Num, i );
      exit (-1);		
    }
  }
    
  str[0] = 0;
  for (j = 0; j < array_n (s_array); j++) {
    sptr = array_fetch (signal_ptr, s_array, j);
    k = Signal_Index (sptr);
    if (k == -1) {
      printf ("Place %s has index -1!\n", Signal_String (sptr));
      exit (-1);
    }  
    data1 = array_fetch (bdd_literal, cube, Signal_Order (sptr));
    data2 = array_fetch (bdd_literal, cube, Signal_Next_Order (sptr));
    if (data1 > 1 || data2 > 1) {
      printf ("How come %s for %s? External_Num = %d, sig_num = %d\n", 
      		Signal_String (sptr),
      		Gate_String (gptr),
      		External_Num, i );
      exit (-1);		
    }    
    if (Signal_IO (sptr) != PLACE_NODE) {
      str2[k+1] = (data2 == 0) ? '0' : (data2 == 1) ? '1' : '2';
      if (data1 != data2) {
        strcat (str, Signal_String (sptr));
        strcat (str, (data2 == 0) ? "-" : "+");
      }  
    } else {
      str1[k+1] = (data1 == 0) ? '0' : (data1 == 1) ? '1' : '2';
      str2[k+1] = (data2 == 0) ? '0' : (data2 == 1) ? '1' : '2';
    }        
  }
  str1[print_sig_num+1] = str2[print_sig_num+1] = 0;  

  if (!str[0])
    return;
  
  To_Hex (str1);
  To_Hex (str2);
  
//  printf ("Found transition %s to %s\n", str1, str2);

  if (st_find (state_table, str1, &s1) == 0) {
    c1 = malloc (strlen (str1) + 1);
    strcpy (c1, str1);
    st_find_or_add (state_table, c1, &s1);
    *s1 = c1;
  }  
  if (st_find (state_table, str2, &s2) == 0) {
    c2 = malloc (strlen (str2) + 1);
    strcpy (c2, str2);
    st_find_or_add (state_table, c2, &s2);
    *s2 = c2;
  }
  tr = (tran_ptr) malloc (sizeof (tran));
  Tran_Source (tr) = *s1;
  Tran_Destination (tr) = *s2;
  if (st_find (tran_table, (char *)tr, NULL) == 0) {
    Tran_Transition (tr) = malloc (strlen (str) + 1);
    strcpy (Tran_Transition (tr), str);
    st_find_or_add (tran_table, (char *) tr, (char ***) &tr1);
    *tr1 = tr;
  } else
    free ((char *) tr);  
      
}

/*******************************************************/
void Process_Cube (array_t *cube, gate_ptr gptr, int index, signal_ptr *_Ext_Signals, array_t *s_array)
{
  int i, j, k;
  signal_ptr sptr;
  gate_ptr gptr1;
  
  if (index == flat_cir->_Sig_Num * 3) // sum is ready ...... 3 was 2
    Process_Minterm (cube, gptr, _Ext_Signals, s_array);
  else {
    for (i = index; i < flat_cir->_Sig_Num * 3; i++) { // ..... 3 was 2

      if (array_fetch (bdd_literal, cube, i) != 3)
        continue;

      array_insert (bdd_literal, cube, i, (bdd_literal) 0);
//      printf ("%d << (%s) ", i+1, Signal_String (order[i]));
      Process_Cube (cube, gptr, i+1, _Ext_Signals, s_array);  

      array_insert (bdd_literal, cube, i, (bdd_literal) 1);
//      printf ("%d >> (%s) ", i+1, Signal_String (order[i]));
      Process_Cube (cube, gptr, i+1, _Ext_Signals, s_array);

      array_insert (bdd_literal, cube, i, (bdd_literal) 3);
      return;
    }  
    if (i == flat_cir->_Sig_Num * 3)
      Process_Minterm (cube, gptr, _Ext_Signals, s_array);      
  }    
}

/*******************************************************/
int state_cmp (char *s1, char *s2)
{
  if (strcmp (s1, s2) == 0)
    return (0);
  else
    return (1);  
}

/*******************************************************/
int tran_cmp (tran_ptr tptr1, tran_ptr tptr2)
{
  if (Tran_Destination (tptr1) == Tran_Destination (tptr2) && Tran_Source (tptr1) == Tran_Source (tptr2))
    return (0);
  else
    return (1);  
}

/*******************************************************/
int state_hash (char *s1, int size) 
{
  int i, length, retval = 0;
  
  length = strlen (s1);
  for (i = 1; i < length; i++)
    retval += ((s1[i] == '0') ? 0 : 1);
  return (retval%size);  
}

/*******************************************************/
int tran_hash (tran_ptr tptr, int size)
{
  return ((int)(((unsigned long) Tran_Source (tptr) + ((unsigned long) Tran_Destination (tptr) ) >> 2) % size));
}

/*******************************************************/
void Print_CRT1 (gate_ptr *_Ext_Gates)
{
  int i, j, k;
  gate_ptr gptr, gptr1;
  signal_ptr sptr;
  char *str1, *str2, *str3, str[2];  
  char **s1, **s2, **s3;
  char *c1, *c2, *c3;
  tran_ptr  tr, *tr1;
  int num = 0;

  str[0] = '|';
  str[1] = 0;
  state_table = st_init_table (state_cmp, state_hash);
  tran_table = st_init_table (tran_cmp, tran_hash);
  for (i = 0; (gptr = _Ext_Gates[i]) != NULL; i++) {
    if (Gate_Type (gptr) == _STATE) // Draw only the connection to the _OUT gate!
      continue;
    str1 = Gate_String (gptr);
    if (st_find (state_table, str1, &s1) == 0) {
      c1 = malloc (strlen (str1) + 1);
      strcpy (c1, str1);
      st_find_or_add (state_table, c1, &s1);
      *s1 = c1;
//    printf ("Inserting state %s\n", *s1);
    }
    if (Gate_Type (gptr) == _OUT) {  
      gptr1 = Gate_Uncle (gptr);
      if (/*(Gate_Subcir (gptr1) == Gate_Subcir (gptr)) || */Is_ZArray_Member (gptr1, _Ext_Gates)) {
        str2 = Gate_String (gptr1);  
        if (st_find (state_table, str2, &s2) == 0) {
          c2 = malloc (strlen (str2) + 1);
          strcpy (c2, str2);
          st_find_or_add (state_table, c2, &s2);
          *s2 = c2;
//          printf ("Inserting state %s\n", *s2);
        }
        tr = (tran_ptr) malloc (sizeof (tran));
        Tran_Source (tr) = *s2;
        Tran_Destination (tr) = *s1;
        if (st_find (tran_table, (char *)tr, NULL) == 0) {
          Tran_Transition (tr) = malloc (strlen (str) + 1);
         strcpy (Tran_Transition (tr), str);
          st_find_or_add (tran_table, (char *) tr, (char ***) &tr1);
          *tr1 = tr;
//        printf ("Inserting transition %s from %s to %s \n", str, *s1, *s2);
        } else
          free ((char *) tr);
      }
      str2 = Signal_String (sptr = Gate_Outputs (gptr)[0]);    
      for (k = 0; (gptr1 = Signal_Fanout (sptr)[k]) != NULL; k++) {
        if (/*Gate_Subcir (gptr1) != Gate_Subcir (gptr) && */ !Is_ZArray_Member (gptr1, _Ext_Gates) )
          continue;  
        str3 = Gate_String (gptr1);  
        if (st_find (state_table, str3, &s3) == 0) {
          c3 = malloc (strlen (str3) + 1);
          strcpy (c3, str3);
          st_find_or_add (state_table, c3, &s3);
          *s3 = c3;
//        printf ("Inserting state %s\n", *s2);
	}
        tr = (tran_ptr) malloc (sizeof (tran));
        Tran_Source (tr) = *s1;
        Tran_Destination (tr) = *s3;
        if (st_find (tran_table, (char *)tr, NULL) == 0) {
          Tran_Transition (tr) = malloc (strlen (str2) + 1);
          strcpy (Tran_Transition (tr), str2);
          st_find_or_add (tran_table, (char *) tr, (char ***) &tr1);
          *tr1 = tr;
//        printf ("Inserting transition %s from %s to %s \n", str, *s1, *s2);
        } else
          free ((char *) tr); 
      }      
    }
    else { 
      for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++) {
        str2 = Signal_String (sptr);
        for (k = 0; (gptr1 = Signal_Fanout (sptr)[k]) != NULL; k++) {
          if (/*Gate_Subcir (gptr1) != Gate_Subcir (gptr) && */!Is_ZArray_Member (gptr1, _Ext_Gates) )
            continue;  
          str3 = Gate_String (gptr1);  
          if (st_find (state_table, str3, &s3) == 0) {
            c3 = malloc (strlen (str3) + 1);
            strcpy (c3, str3);
            st_find_or_add (state_table, c3, &s3);
            *s3 = c3;
//          printf ("Inserting state %s\n", *s2);
	  }
          tr = (tran_ptr) malloc (sizeof (tran));
          Tran_Source (tr) = *s1;
          Tran_Destination (tr) = *s3;
          if (st_find (tran_table, (char *)tr, NULL) == 0) {
            Tran_Transition (tr) = malloc (strlen (str2) + 1);
            strcpy (Tran_Transition (tr), str2);
            st_find_or_add (tran_table, (char *) tr, (char ***) &tr1);
            *tr1 = tr;
//          printf ("Inserting transition %s from %s to %s \n", str, *s1, *s2);
          } else
            free ((char *) tr);  
        }    
      }  
    } 
  }     
  st_foreach (state_table, Print_State, NULL);
  st_foreach (tran_table, Print_Tran, (char *) &num);
  fprintf (OutFile, "ZZZZZ");
  printf ("#transitions = %d\n", num);
  printf ("\n");    
  st_foreach (state_table, Free_State, NULL);
  st_foreach (tran_table, Free_Tran, NULL);

  st_free_table (state_table);
  st_free_table (tran_table);   

}

/*******************************************************/
void Print_CRT2 (gate_ptr *_Ext_Gates)
{
  int i, j, k;
  gate_ptr gptr, gptr1;
  signal_ptr sptr;
  char *str1, *str2, *str3, str[2];  
  char **s1, **s2, **s3;
  char *c1, *c2, *c3;
  tran_ptr  tr, *tr1;
  int num = 0;

  str[0] = '|';
  str[1] = 0;
  state_table = st_init_table (state_cmp, state_hash);
  tran_table = st_init_table (tran_cmp, tran_hash);
  for (i = 0; (gptr = _Ext_Gates[i]) != NULL; i++) {
    if (Gate_Type (gptr) == _STATE) // Draw only the connection to the _OUT gate!
      continue;

    if (Gate_Type (gptr) == _OUT) {
      str1 = Signal_String (sptr = Gate_Outputs (gptr)[0]);
      gptr1 = Gate_Uncle (gptr);
      if (Gate_Subcir (gptr1) == Gate_Subcir (gptr)) {
        str2 = Gate_String (gptr1);  
        if (st_find (state_table, str2, &s2) == 0) {
          c2 = malloc (strlen (str2) + 1);
          strcpy (c2, str2);
          st_find_or_add (state_table, c2, &s2);
          *s2 = c2;
//          printf ("Inserting state %s\n", *s2);
        }
      }    
      for (k = 0; (gptr1 = Signal_Fanout (sptr)[k]) != NULL; k++) {
        if (Gate_Subcir (gptr1) != Gate_Subcir (gptr))
          continue;  
        str3 = Gate_String (gptr1);  
        if (st_find (state_table, str3, &s3) == 0) {
          c3 = malloc (strlen (str3) + 1);
          strcpy (c3, str3);
          st_find_or_add (state_table, c3, &s3);
          *s3 = c3;
//        printf ("Inserting state %s\n", *s2);
	}
        tr = (tran_ptr) malloc (sizeof (tran));
        Tran_Source (tr) = *s2;
        Tran_Destination (tr) = *s3;
        if (st_find (tran_table, (char *)tr, NULL) == 0) {
          Tran_Transition (tr) = malloc (strlen (str1) + 1);
          strcpy (Tran_Transition (tr), str1);
          st_find_or_add (tran_table, (char *) tr, (char ***) &tr1);
          *tr1 = tr;
//        printf ("Inserting transition %s from %s to %s \n", str, *s1, *s2);
        } else
          free ((char *) tr); 
      }      
    }
    else { 
      str1 = Gate_String (gptr);
      if (st_find (state_table, str1, &s1) == 0) {
        c1 = malloc (strlen (str1) + 1);
        strcpy (c1, str1);
        st_find_or_add (state_table, c1, &s1);
        *s1 = c1;
//      printf ("Inserting state %s\n", *s1);
      }
      for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++) {
        str2 = Signal_String (sptr);
        for (k = 0; (gptr1 = Signal_Fanout (sptr)[k]) != NULL; k++) {
          if (Gate_Subcir (gptr1) != Gate_Subcir (gptr))
            continue;  
          str3 = Gate_String (gptr1);  
          if (st_find (state_table, str3, &s3) == 0) {
            c3 = malloc (strlen (str3) + 1);
            strcpy (c3, str3);
            st_find_or_add (state_table, c3, &s3);
            *s3 = c3;
//          printf ("Inserting state %s\n", *s2);
	  }
          tr = (tran_ptr) malloc (sizeof (tran));
          Tran_Source (tr) = *s1;
          Tran_Destination (tr) = *s3;
          if (st_find (tran_table, (char *)tr, NULL) == 0) {
            Tran_Transition (tr) = malloc (strlen (str2) + 1);
            strcpy (Tran_Transition (tr), str2);
            st_find_or_add (tran_table, (char *) tr, (char ***) &tr1);
            *tr1 = tr;
//          printf ("Inserting transition %s from %s to %s \n", str, *s1, *s2);
          } else
            free ((char *) tr);  
        }    
      }  
    } 
  }     
  st_foreach (state_table, Print_State, NULL);
  st_foreach (tran_table, Print_Tran, (char *) &num);
  fprintf (OutFile, "ZZZZZ");
  printf ("# of transitions = %d\n", num);
  printf ("\n");    
  st_foreach (state_table, Free_State, NULL);
  st_foreach (tran_table, Free_Tran, NULL);

  st_free_table (state_table);
  st_free_table (tran_table);   

}

/*******************************************************/
void Print_CRT3 (gate_ptr *_Ext_Gates)
{
  int i, j, k;
  gate_ptr gptr, gptr1;
  signal_ptr sptr;
  char *str1, *str2, *str3, str[2];  
  char **s1, **s2, **s3;
  char *c1, *c2, *c3;
  tran_ptr  tr, *tr1;
  int num = 0;

  str[0] = '|';
  str[1] = 0;
  state_table = st_init_table (state_cmp, state_hash);
  tran_table = st_init_table (tran_cmp, tran_hash);
  for (i = 0; (gptr = _Ext_Gates[i]) != NULL; i++) {
    if (Gate_Type (gptr) == _STATE) // Draw only the connection to the _OUT gate!
      continue;
    if (Gate_Type (gptr) == _OUT)
      str1 = Signal_String (sptr = Gate_Outputs (gptr)[0]);
    else  
      str1 = Gate_String (gptr);
    if (st_find (state_table, str1, &s1) == 0) {
      c1 = malloc (strlen (str1) + 1);
      strcpy (c1, str1);
      st_find_or_add (state_table, c1, &s1);
      *s1 = c1;
//    printf ("Inserting state %s\n", *s1);
    }
    if (Gate_Type (gptr) == _OUT) {  
      gptr1 = Gate_Uncle (gptr);
      if (Gate_Subcir (gptr1) == Gate_Subcir (gptr)) {
        str2 = Gate_String (gptr1);  
        if (st_find (state_table, str2, &s2) == 0) {
          c2 = malloc (strlen (str2) + 1);
          strcpy (c2, str2);
          st_find_or_add (state_table, c2, &s2);
          *s2 = c2;
//          printf ("Inserting state %s\n", *s2);
        }
        tr = (tran_ptr) malloc (sizeof (tran));
        Tran_Source (tr) = *s2;
        Tran_Destination (tr) = *s1;
        if (st_find (tran_table, (char *)tr, NULL) == 0) {
          Tran_Transition (tr) = malloc (strlen (str) + 1);
         strcpy (Tran_Transition (tr), str);
          st_find_or_add (tran_table, (char *) tr, (char ***) &tr1);
          *tr1 = tr;
//        printf ("Inserting transition %s from %s to %s \n", str, *s1, *s2);
        } else
          free ((char *) tr);
      }    
      for (k = 0; (gptr1 = Signal_Fanout (sptr)[k]) != NULL; k++) {
        if (Gate_Subcir (gptr1) != Gate_Subcir (gptr))
          continue;  
        str3 = Gate_String (gptr1);  
        if (st_find (state_table, str3, &s3) == 0) {
          c3 = malloc (strlen (str3) + 1);
          strcpy (c3, str3);
          st_find_or_add (state_table, c3, &s3);
          *s3 = c3;
//        printf ("Inserting state %s\n", *s2);
	}
        tr = (tran_ptr) malloc (sizeof (tran));
        Tran_Source (tr) = *s1;
        Tran_Destination (tr) = *s3;
        if (st_find (tran_table, (char *)tr, NULL) == 0) {
          Tran_Transition (tr) = malloc (strlen (str) + 1);
          strcpy (Tran_Transition (tr), str);
          st_find_or_add (tran_table, (char *) tr, (char ***) &tr1);
          *tr1 = tr;
//        printf ("Inserting transition %s from %s to %s \n", str, *s1, *s2);
        } else
          free ((char *) tr); 
      }      
    }
    else for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++) {
      str2 = Signal_String (sptr);
      if (st_find (state_table, str2, &s2) == 0) {
        c2 = malloc (strlen (str2) + 1);
        strcpy (c2, str2);
        st_find_or_add (state_table, c2, &s2);
        *s2 = c2;
//      printf ("Inserting state %s\n", *s2);
      }
      tr = (tran_ptr) malloc (sizeof (tran));
      Tran_Source (tr) = *s1;
      Tran_Destination (tr) = *s2;
      if (st_find (tran_table, (char *)tr, NULL) == 0) {
        Tran_Transition (tr) = malloc (strlen (str) + 1);
        strcpy (Tran_Transition (tr), str);
        st_find_or_add (tran_table, (char *) tr, (char ***) &tr1);
        *tr1 = tr;
//        printf ("Inserting transition %s from %s to %s \n", str, *s1, *s2);
      } else
        free ((char *) tr);  
      for (k = 0; (gptr1 = Signal_Fanout (sptr)[k]) != NULL; k++) {
        if (Gate_Subcir (gptr1) != Gate_Subcir (gptr))
          continue;  
        str3 = Gate_String (gptr1);  
        if (st_find (state_table, str3, &s3) == 0) {
          c3 = malloc (strlen (str3) + 1);
          strcpy (c3, str3);
          st_find_or_add (state_table, c3, &s3);
          *s3 = c3;
//        printf ("Inserting state %s\n", *s2);
	}
        tr = (tran_ptr) malloc (sizeof (tran));
        Tran_Source (tr) = *s2;
        Tran_Destination (tr) = *s3;
        if (st_find (tran_table, (char *)tr, NULL) == 0) {
          Tran_Transition (tr) = malloc (strlen (str) + 1);
          strcpy (Tran_Transition (tr), str);
          st_find_or_add (tran_table, (char *) tr, (char ***) &tr1);
          *tr1 = tr;
//        printf ("Inserting transition %s from %s to %s \n", str, *s1, *s2);
        } else
          free ((char *) tr);  
      }    
    }  
  }    
  st_foreach (state_table, Print_State, NULL);
  st_foreach (tran_table, Print_Tran, (char *) &num);
  fprintf (OutFile, "ZZZZZ");
  printf ("# of transitions = %d\n", num);
  printf ("\n");    
  st_foreach (state_table, Free_State, NULL);
  st_foreach (tran_table, Free_Tran, NULL);

  st_free_table (state_table);
  st_free_table (tran_table);   

}

/*******************************************************/
void Preprocess_Cube (array_t *cube, array_t *s_array, signal_ptr *_Ext_Signals)
{
  int i;
  signal_ptr sptr;
  
  for (i = 0; (sptr = _Ext_Signals[i]) != NULL; i++) {
    if (array_fetch (bdd_literal, cube, Signal_Order(sptr)) == 2)
      array_insert (bdd_literal, cube, Signal_Order(sptr), (bdd_literal) 3);
  }
  for (i = 0; i < array_n (s_array); i++) {
    sptr = array_fetch (signal_ptr, s_array, i);
    if (array_fetch (bdd_literal, cube, Signal_Order(sptr)) == 2)
      array_insert (bdd_literal, cube, Signal_Order(sptr), (bdd_literal) 3);
    if (array_fetch (bdd_literal, cube, Signal_Next_Order (sptr)) == 2)
      array_insert (bdd_literal, cube, Signal_Next_Order (sptr), (bdd_literal) 3);
  }    
}

/*******************************************************/
void Print_PRG (gate_ptr *_Ext_Gates, signal_ptr *_Ext_Signals, int mode)
{
  int i, j = 0, k;
  gate_ptr gptr, gptro;
  bdd_t *bdd, *temp;
  bdd_gen *gen;
  array_t *cube = NULL;
  int num = 0;
  signal_ptr sptr, *spptr, sptr1;
  spec_index_ptr siptr;
  input_ptr iptr;
  array_t *s_array;  

/*
  if (Cur_Level)
    return;
*/
  state_table = st_init_table (state_cmp, state_hash);
  tran_table = st_init_table (tran_cmp, tran_hash);
  for (i = 0; (gptr = _Ext_Gates[i]) != NULL; i++) {
    if (Gate_Ext_Out (gptr)) {
      bdd = bdd_dup (Gate_Ext_TR (gptr));
      spptr = Gate_Ext_Out (gptr);
    } else {
      bdd = bdd_dup (Gate_Int_TR (gptr)); // May 20: DDDAAAANNNNGGGEEERRRR!!!!
      spptr = Gate_Int_Out (gptr);
    }
    s_array = array_alloc (signal_ptr, 1);
    for (j = 0; (sptr = spptr[j]) != NULL; j++) {
      array_insert_last (signal_ptr, s_array, sptr); // June 15
      for (siptr = Signal_SpecFanout (sptr); siptr != NULL; siptr = Spec_Index_Next (siptr)) {
        iptr = &(Gate_State_Inputs (gptro = Spec_Index_Spec (siptr))[Spec_Index_Index (siptr)]);
        if (mode == PARTIAL && (Gate_State_Ext_Places (gptro) == NULL || Gate_State_Ext_Places (gptro)[0] == NULL))
          continue;
        if (array_n (Input_ChangingArray (iptr)) == 0)
          continue;  
        temp = bdd_and (bdd, Input_Ext_TR (iptr), 1, 1);
        bdd_free (bdd);
        bdd = temp;
        if (mode == TOTAL)
          array_append (s_array, Input_ChangingArray (iptr)); // contains all signals that may change!
        else {
          for (k = 0; k < array_n (Input_ChangingArray (iptr)); k++) {
            sptr1 = array_fetch (signal_ptr, Input_ChangingArray (iptr), k);
            if (Signal_Type (sptr1) == EXTERNAL)
              array_insert_last (signal_ptr, s_array, sptr1);
          }      
        }  
      }
    }                
    if (bdd_equal (bdd_zero (Bdd_Man), bdd))
      continue;
    cube = NULL;
    foreach_bdd_cube (bdd, gen, cube) {
      Preprocess_Cube (cube, s_array, _Ext_Signals);
      Process_Cube (cube, gptr, 0, _Ext_Signals, s_array); // ..... flat_cir->_Sig_Num was 0
      j++;     
    }
    array_free (s_array);
    bdd_free (bdd);
  }  

  st_foreach (state_table, Print_State, NULL);
  st_foreach (tran_table, Print_Tran, (char *) &num);
  fprintf (OutFile, "ZZZZZ");
  printf ("# of transitions = %d\n", num);
  printf ("\n");    

/* // June 29: redundant code
  for (i = 0; (sptr = _Ext_Signals [i]) != NULL; i++)
    printf ("%s ", Signal_String (sptr));
  printf ("\n\n"); 
*/
  st_foreach (state_table, Free_State, NULL);
  st_foreach (tran_table, Free_Tran, NULL);
  st_free_table (state_table);
  st_free_table (tran_table);   

}

/*******************************************************/
void Print_Graph (int mode)
{
  int i, j, k, kk, g;
  gate_ptr gpptr[MAXGATE], *tempg, gptr;
  signal_ptr spptr[MAXSIGNAL], sptr;
  char file_name[30], ch;
  
  if (!oflag)
    return;
  if (mode == TOTAL)  
    printf ("\n~~~~ PRINTING THE FLAT STATE TRANSITION GRAPH OF SUBCIRCUIT %s! ~~~~\n", Subcir_Path);
  else
    printf ("\n~~~~ PRINTING THE SAFE ABSTRACTION OF THE BEHAVIOR OF SUBCIRCUIT %s! ~~~~\n", Subcir_Path);    
  printf ("Do you want to print the state graph?(y/n) ");
  while ((ch = tolower (getchar ())) != 'y' && ch != 'n');
  if (ch == 'n')
    return;
  strcpy (file_name, Subcir_Path);
  for (i = 0; file_name[i] != NULL; i++)
    if (file_name[i] == '/')
      file_name[i] = '_';
  strcat (file_name, ".o");    
  OutFile = fopen (file_name, "w");
  for (i = 0; (sptr = flat_cir->_signal_array[i]) != NULL; i++)
    Signal_Index (sptr) = -1;
  printf ("The signals of the state diagram: ");  
  if (mode == PARTIAL) {
    k = 0;
    kk = 0;
    for (g = 0, tempg = Ext_Gates; g < 2; g++) {
      for (i = 0; (gptr = tempg[i]) != NULL; i++) {
        if (Gate_Ext_Out (gptr) == NULL)
          continue;
        gpptr[kk++] = gptr;
        for (j = 0; (sptr = Gate_Ext_Out (gptr)[j]) != NULL; j++) {
          Signal_Index (sptr) = k;
          printf ("%s ", Signal_String (sptr));
          spptr[k++] = sptr;
        }  
      }
      if (g == 0)
        tempg = Sibling_Gates;
    }
    for (i = 0; i < array_n (Ext_Place_Array); i++) {
      sptr = array_fetch (signal_ptr, Ext_Place_Array, i);
      Signal_Index (sptr) = k;
      printf ("%s ", Signal_String (sptr));
      spptr[k++] = sptr;
    }  
  }
  else {
    k = 0;
    kk = 0;
    for (g = 0, tempg = Ext_Gates; g < 3; g++) {
      for (i = 0; (gptr = tempg[i]) != NULL; i++) {
        gpptr[kk++] = gptr;
        for (j = 0; (sptr = Gate_Outputs (gptr)[j]) != NULL; j++) {
          Signal_Index (sptr) = k;
          printf ("%s ", Signal_String (sptr));
          spptr[k++] = sptr;
        }  
      }
      if (g == 0)
        tempg = Sibling_Gates;
      else if (g == 1)
        tempg = DFS_List;  
    }
    for (i = 0; i < array_n (Place_Array); i++) {
      sptr = array_fetch (signal_ptr, Place_Array, i);
      Signal_Index (sptr) = k;
      printf ("%s ", Signal_String (sptr));
      spptr[k++] = sptr;
    }  
  }
  gpptr[kk] = NULL;
  spptr[k] = NULL;
  print_sig_num = k;
  printf ("\nPrinting the state transition graph in file %s, #signals = %d, #gates = %d, ", file_name, k, kk);
  Print_PRG (gpptr, spptr, mode); 
  printf ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  fclose (OutFile);
}


/*******************************************************/
void Print_Circuit_Graph (int mode)
{
  int i, j, kk, g;
  gate_ptr gpptr[MAXGATE], *tempg, gptr;
  signal_ptr sptr;
  char file_name[30];
  
  if (!wflag)
    return;
  if (cur_cir != flat_cir) {  
    strcpy (file_name, Subcir_Path);
    for (i = 0; file_name[i] != NULL; i++)
      if (file_name[i] == '/')
        file_name[i] = '_';
  }
  else 
    strcpy (file_name, "flat");      
  strcat (file_name, ".g");    
  OutFile = fopen (file_name, "w");
  printf ("Gates: ");
  if (mode == PARTIAL) {
    kk = 0;
    for (g = 0, tempg = Ext_Gates; g < 2; g++) {
      for (i = 0; (gptr = tempg[i]) != NULL; i++) {
        if (Gate_Ext_Out (gptr) == NULL)
          continue;
        printf ("%s ", Gate_String (gptr));
        gpptr[kk++] = gptr;
      }
      if (g == 0)
        tempg = Sibling_Gates;
    }
  }
  else {
    kk = 0;
    for (g = 0, tempg = Ext_Gates; g < 3; g++) {
      for (i = 0; (gptr = tempg[i]) != NULL; i++) {
        printf ("%s ", Gate_String (gptr));
        gpptr[kk++] = gptr;
      }
      if (g == 0)
        tempg = Sibling_Gates;
      else if (g == 1)
        tempg = DFS_List;  
    }
  }
  gpptr[kk] = NULL;
  printf ("\nPrinting graph in file %s, #gates = %d\n", file_name, kk);
  if (wflag == 1)
    Print_CRT1 (gpptr);
  else if (wflag == 2)  
    Print_CRT2 (gpptr);              
  else 
    Print_CRT3 (gpptr);  
  fclose (OutFile);
}

