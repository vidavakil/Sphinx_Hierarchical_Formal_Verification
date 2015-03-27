#include "type.h"
#include "sphinx.h"

extern signal_ptr	temp_signal;
extern char Current_Module_Name[];

module_ptr New_Module (name_ptr nptr);
void Enter_Module_Signals (module_ptr mptr);

/*******************************************************/
// Module related functions
/*******************************************************/
/*******************************************************/
void Associate_Signal (char *formal, char *actual)
{
  char temp;
  signal_ptr sptr;
  name_ptr nptr;
  
  temp = Current_Module_Name[0];
  Current_Module_Name[0] = 0;
  sptr = Enter_Signal (actual, NOTR);  // Notice NONE
  Current_Module_Name[0] = temp;  
  nptr = Name_Enter (formal, SIGNAL);
  Name_Signal (nptr) = sptr; //sptr was temp_signal;
}

/*******************************************************/
module_ptr New_Module (name_ptr nptr)
{
  module_ptr mptr;
  
  mptr = (module_ptr) malloc (sizeof (module));
  Module_Name (mptr) = nptr;
  Module_Type (mptr) = NULL;
  Module_BddOrd (mptr) = NULL;
  Name_Module (nptr) = mptr;

  return (mptr);
}

/*******************************************************/
void Enter_Module (char *name, signal_ptr *spptr, sig_type mode)
{
  int i;
  signal_ptr sptr;
  name_ptr nptr;
  module_ptr mptr;
  
  nptr = Name_Enter (name, MODULE);
  if (Name_Module (nptr) == NULL)
    mptr = New_Module (nptr);
  else 
    mptr = Name_Module (nptr);  
  // Feb 9

  Module_BddOrd (mptr) = spptr;
  for (i = 0; (sptr = spptr[i]) != NULL; i++) 
    Signal_Type (sptr) = mode;  
  
}

/*******************************************************/
void Enter_Module_Signals (module_ptr mptr)
{
  int i;
  signal_ptr sptr;
  
  for (i = 0; (sptr = Module_BddOrd (mptr) [i]) != NULL; i++) {
    Signal_Next (sptr) = temp_signal;
    temp_signal = sptr;
  }  
}

/*******************************************************/
void Enter_Item (char *name)
{
  name_ptr nptr;
  signal_ptr sptr;
  
  nptr = Name_Find_Str (name);
  if (Name_Type (nptr) == SIGNAL) {
    sptr = Name_Signal (nptr);
    Signal_Next (sptr) = temp_signal;
    temp_signal = sptr;  
  } else if (Name_Type (nptr) == MODULE)
    Enter_Module_Signals (Name_Module (nptr));
  else {
    printf ("No module with name = %s\n", Name_String (nptr));
    exit (-1);
  }       
}
