#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bdd.h"
#include "type.h"
#include "parser.h"
#include "names.h"
#include "array.h"
#include "var_set.h"
#include "util.h"
#include "st.h"
#include "struct.h"

extern signal_ptr order[MAXSIGNAL];
extern array_t *trans_array;
extern array_t *st_var_array;
extern array_t *place_array;
extern array_t *sig_array;

extern bdd_manager *Bdd_Man;
extern char Current_Module_Name[];
extern signal_ptr temp_signal;
 
extern signal_ptr get_implicit_place(transition_ptr t1, transition_ptr t2);
extern transition_ptr Enter_Trans(signal_ptr p1, transition_ptr t, signal_ptr p2);
extern transition_ptr Find_Trans(char *name, char *idx, char polar);
extern transition_ptr New_Trans(name_ptr nptr);
extern signal_ptr New_Signal (name_ptr nptr, io_type io);
extern signal_ptr Find_Signal (char *name, io_type io); 
extern void set_bdd_order(void);
extern void old_set_bdd_order(void);
extern void create_spec_tr(void);
extern bdd_t *create_tr_function(transition_ptr tptr, array_t *stable);
extern array_t *array_minus(array_t *array1, array_t *array2);
extern bdd_t *Get_Init_State(void);
extern void Init_Signal (signal_ptr sptr, int init); 
extern signal_ptr Enter_Signal_List (void); 
extern void Print_Cube (array_t *cube);
extern void Report_Bdd (bdd_t *R, char *s);
extern void bdd_array_free (array_t *arr); 
extern name_ptr Name_Enter (char *Name1, int type);


