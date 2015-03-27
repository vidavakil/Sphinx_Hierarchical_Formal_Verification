#include "type.h"
#include "sphinx.h"

int 		Levelized_Num = 0;
gate_ptr	Levelized[MAXGATE];
extern 		int MODE;
extern 		context_ptr	cur_cir; 
extern 		bdd_t		*signal_var[]; 

void DFS_Visit (gate_ptr gptr); 
int No_Internal_Fanin (gate_ptr gptr); 

/*******************************************************/
// Ordering functions for gate evaluations
/*******************************************************/
/*******************************************************/
void Levelize_Internals (gate_ptr *gpptr)
{
  int i, j, level, flag, max;
  gate_ptr gptr;
  signal_ptr sptr;
  
  return; // June 10: Due to the changes in the modeling of _STATE and _OUT gates, 
          // this function will not work properly any more. Fix it later if you need it!


  for (i = 0; (gptr = gpptr[i]) != NULL; i++) 
    Gate_Level (gptr) = -1;
  Levelized_Num = 0;
//  printf ("DFS_Count = %d\n", DFS_Count);  
  while (DFS_Count != Levelized_Num) {
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) 
      if (Gate_Level (gptr) == -1) {
        flag = 0;
        max = 0;
        for (j = 0; (sptr = Input_Signal (Gate_Inputs (gptr)[j])) != NULL; j++) {
          if (Signal_Type (sptr) == EXTERNAL)
            continue;
          if ((level = Gate_Level (Signal_Parent (sptr))) == -1) {
            flag = 1;
            break;
          } else if (level > max)
            max = level;
        }
        if (flag)
          break;
        else {
          Gate_Level (gptr) = max + 1;
          Levelized_Num++;
//          printf ("%s levelized to %d\n", Gate_String (gptr), Gate_Level (gptr));          
//	  printf ("Levelized_Num = %d\n", Levelized_Num);  
        }  
      }
    if (DFS_Count != Levelized_Num) 
      for (i = 0; (gptr = gpptr[i]) != NULL; i++) 
        if (Gate_Level (gptr) == -1) {
          max = 0;
          for (j = 0; (sptr = Input_Signal (Gate_Inputs (gptr)[j])) != NULL; j++) {
            if (Signal_Type (sptr) == EXTERNAL || Gate_Level (Signal_Parent (sptr)) == -1)
              continue;
            else if (Gate_Level (Signal_Parent (sptr)) > max)
              max = level;
          }
          Gate_Level (gptr) = max + 1;
          Levelized_Num++;
//          printf ("%s levelized to %d\n", Gate_String (gptr), Gate_Level (gptr));
//	  printf ("Levelized_Num = %d\n", Levelized_Num);  
          break; // break just one cycle each time!
        }
  } // while
  Levelized_Num = 0;
  j = -1;
  while (Levelized_Num != DFS_Count) {
    j++;
    for (i = 0; (gptr = gpptr[i]) != NULL; i++) 
      if (Gate_Level(gptr) == j) 
        Levelized[Levelized_Num++] = gptr;
  }
  for (i = 0; i < DFS_Count; i++) 
    DFS_List[i] = Levelized[i]; 
  printf ("Done levelizing!\n");
}

/*******************************************************/
void DFS_Visit (gate_ptr gptr) 
{
  int i, j;
  gate_ptr fanout;
  signal_ptr sptr;

  Gate_Color (gptr) = GRAY;
  for (i = 0; (sptr = Gate_Outputs (gptr)[i]) != NULL; i++) {
    if (Signal_Type (sptr) == EXTERNAL)
      continue;
    for (j = 0; (fanout = Signal_Fanout (sptr)[j]) != NULL; j++) {
      if (Gate_Type (fanout) == _STATE) // June 10
        continue;
      if ((Gate_Int_Out (fanout) != NULL) && Gate_Color (fanout) == WHITE) 
        DFS_Visit (fanout);
    // Signal_SpecFanout????
    }
  }  
  Gate_Color (gptr) = BLACK;
  DFS_List [DFS_Count++] = gptr;
  DFS_List [DFS_Count] = NULL;
}

/*******************************************************/
int No_Internal_Fanin (gate_ptr gptr) 
{
  int i;
  signal_ptr sptr;
  gate_ptr pgptr;
  
  if (Gate_Type (gptr) == _OUT) // June 10: Just to simplify matters
    return (0);
    
  else
    for (i = 0; (sptr = Input_Signal (Gate_Inputs (gptr)[i])) != NULL; i++) 
      if (Signal_Type (sptr) == INTERNAL)
        return (0);

  return (1);    
}

/*******************************************************/
void NoSort (gate_ptr *gpptr)
{
  int i;
  gate_ptr gptr;

  DFS_Count = 0;
  Ext_Gates_Count = 0;
  Ext_Gates[0] = DFS_List[0] = NULL;
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    if (Gate_Type (gptr) == _STATE) // June 10: _STATE gates are not set into any of the set of EXT/DFS lists!
      continue; 
    if (Gate_Int_Out (gptr) != NULL /*|| Gate_Type (gptr) == _STATE*/) { // June 1: Gate_Type
      DFS_List [DFS_Count++] = gptr;
      DFS_List [DFS_Count] = NULL;
    }  
    if (Gate_Ext_Out (gptr) != NULL /*&& Gate_Type (gptr) != _STATE*/) { // June 1: Gate_Type
      Ext_Gates[Ext_Gates_Count++] = gptr;
      Ext_Gates[Ext_Gates_Count] = NULL;
    }
  }              
}

/*******************************************************/
void TopSort_Internals (gate_ptr *gpptr) 
{
  int i;
  gate_ptr gptr;
  
  Ext_Gates[0] = NULL;
  Ext_Gates_Count = 0;
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) {
    Gate_Color (gptr) = WHITE;
//    if (Gate_Ext_Out (gptr) != NULL) { // June 10
    if (Gate_Type (gptr) != _STATE && Gate_Ext_Out (gptr) != NULL) { // June 10: _STATE gates are not put in any set
      Ext_Gates[Ext_Gates_Count++] = gptr;
      Ext_Gates[Ext_Gates_Count] = NULL;
    }        
  }
  DFS_List[0] = NULL;
  DFS_Count = 0;
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) { // June 1: Gate_Type
    if (Gate_Type (gptr) != _STATE) // No_Internal_Fanin does not work for _OUT gates, June 10
      if (Gate_Int_Out (gptr) != NULL && No_Internal_Fanin (gptr) && Gate_Color (gptr) == WHITE)
        DFS_Visit (gptr); 
  }    
  for (i = 0; (gptr = gpptr[i]) != NULL; i++) { // June 1: Gate_Type
    if (Gate_Type (gptr) != _STATE) 
      if (Gate_Int_Out (gptr) != NULL && Gate_Color (gptr) == WHITE)
        DFS_Visit (gptr);    
  }
  printf ("Gate_Num = %d, Ext_Gates_Count = %d, DFS_Count = %d\n", 
          Ext_Gates_Count + DFS_Count, Ext_Gates_Count, DFS_Count);    
}
