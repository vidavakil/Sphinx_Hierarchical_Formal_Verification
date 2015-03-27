typedef union {
	char *sval;
	//int ival;
	//gate_ptr gptr;
	signal_ptr sptr;
	transition_ptr tptr;
	name_ptr   nptr;
	//input_ptr  inptr;
} YYSTYPE;
#define	ID	258
#define	INTEGER	259
#define	INPUTS	260
#define	OUTPUTS	261
#define	INTERNALS	262
#define	GRAPH	263
#define	END	264
#define	MARKING	265
#define	NAME	266
#define	INIT_STATE	267
#define	DUMMYS	268


extern YYSTYPE vidalval;
