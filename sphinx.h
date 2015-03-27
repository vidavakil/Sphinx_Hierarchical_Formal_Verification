#ifndef SPHINXH
#define SPHINXH

extern void Generate_Complex_Gates_NoSpec (void);
extern void Generate_Complex_Gates_Spec (void);

extern int Check_Failure (gate_ptr gptr1, sig_type Int_Ext);
extern int Check_Fixedpoint (void);
extern int Flat_Verify (void);
extern void Associate_Signal (char *formal, char *actual);
extern void Enter_Module (char *name, signal_ptr *spptr, sig_type mode);
extern void Enter_Item (char *name);
extern void Levelize_Internals (gate_ptr *gpptr);
extern void NoSort (gate_ptr *gpptr);
extern void TopSort_Internals (gate_ptr *gpptr); 
extern void New_Specification (char *spec_name);
extern void Set_Gate_Hazards (void); 
extern bdd_t *Set_Gate_Function (gate_ptr gptr);
extern void Set_Gate_Functions (void);
extern void Print_ZArray (void_ptr *ptr, int type);
extern void Copy_ZArray (gate_ptr *gptr1, gate_ptr *gptr2);
extern gate_ptr *Duplicate_ZArray (gate_ptr *gptr2);
extern void *Array_to_ZArray (array_t *arr);
extern void Append_ZArray_to_Array (array_t *arr, signal_ptr *spptr);
extern void *Array_to_ZArray_Free (array_t *arr);
extern int Is_Array_Member (gate_ptr gptr, array_t *arr);
extern int Is_ZArray_Member (gate_ptr gptr, gate_ptr *gpptr);
extern void Change_Orders (void);
extern void BDD_Order (signal_ptr sptr);
extern void Set_Signal_Vars (void);
extern void Initial_State (void);
extern void Test_TR_Functions (void);
extern void Find_Enabled (gate_ptr gptr, int mode, sig_type Int_Ext/**/, signal_ptr schanged /**/);
extern int  Find_Abstract (void);

extern void free_ite (bdd_t *var, bdd_t *if_term , bdd_t *else_term);
extern bdd_t *bdd_substitute1 (bdd_t *bdd, array_t *substitute, array_t *quantify);
extern void Print_Bdd (bdd_t *R, char *s);
extern bdd_t *Exist_Quantify (bdd_t *bdd, signal_ptr *spptr);
extern bdd_t *Exist_Quantify_Array (bdd_t *bdd, array_t *spptr);
extern bdd_t *Exist_Quantify_Next (bdd_t *bdd, signal_ptr *spptr);
extern bdd_t *Next_to_Current (bdd_t *temp0, signal_ptr *spptr);
extern bdd_t *Next_to_Current_Array (bdd_t *temp0, array_t *spptr);
extern bdd_t *Exist_Quantify_Next_Array (bdd_t *bdd, array_t *spptr);
extern bdd_t *Make_Bdd_Complete (bdd_t *bdd, gate_ptr gptr);

extern void Print_A_State (bdd_t *bdd, int mode);
extern void Print_Graph (int mode);
extern void Print_Circuit_Graph (int mode);

extern void Pop_Places (gate_ptr gptr);
extern int Verify_Old (void);
extern subcir_ptr New_Subcircuit (char *name);
extern subcir_ptr Create_Subcircuit (gate_ptr gptr);
extern bdd_t *From_to_To (gate_ptr gptr, sig_type Int_Ext, bdd_t *temp0, signal_ptr *spptr1, int ret);
extern void Update_Sibling (gate_ptr *gpptr1, gate_ptr *gpptr2);
extern void Prepare_Hierarchical_Verification (int mode);

extern gate_ptr *Gate_Chain_to_ZArray (gate_ptr gptr, int num);
extern void Print_Gates (gate_ptr *gpptr, char *str);
extern void Push_Places (gate_ptr gptr, signal_ptr *spptr1);

extern void End_Gate_Enter (void);
extern gate_ptr Enter_Gate (name_ptr nptr, component_type gtype, 
		     input_ptr in_ptr, signal_ptr out_ptr, signal_ptr hid_ptr, int mode);
		     
extern pushed_bdd_ptr Push_Bdd (pushed_bdd_ptr pptr1, bdd_t *R);
extern pushed_bdd_ptr Pop_Bdd (pushed_bdd_ptr pptr1);
extern void Push (rec_ptr *top, bdd_t *bdd, gate_ptr *gpptr, int type);
extern bdd_t *Pop (rec_ptr *top);

extern gate_ptr *Check_Projection (gate_ptr *externals, gate_ptr *must_vis, subcir_ptr sub_cir);
extern void Remove_Projection (gate_ptr *support);
extern void Project_Places (void);

extern void Set_Fanouts (void);
extern void Check_Fanouts (void);

extern void Enter_Initial_State (char *name, int init);
extern void Init_Signal (signal_ptr sptr, int init); 
extern signal_ptr Find_Signal (char *name, petri_type io);
extern signal_ptr Enter_Signal (char *name, petri_type io);
extern signal_ptr Enter_Signal_Chain (void);
extern void Enter_Input_Signal (char sign, char *name);
extern input_ptr Enter_Input_Signal_Chain (void);
extern void Push_Input (input_ptr iptr, int mode);
extern void Enter_Level_Externals (signal_ptr sptr);
extern input_ptr Create_Input_ZArray_N (int no);
extern void Check_Signals (void);
extern signal_ptr *Signal_Chain_to_ZArray (signal_ptr sptr);

extern name_ptr Name_Enter (char *Name, int type);
extern name_ptr Name_Find_Enter_Str (char *Name);

#endif