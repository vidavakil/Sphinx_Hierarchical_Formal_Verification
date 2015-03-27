typedef union {
	char *sval;
	component_type cval;
	int	ival;
	gate_ptr gptr;
	signal_ptr sptr;
	name_ptr   nptr;
	input_ptr  inptr;
} YYSTYPE;
#define	ID	258
#define	ID1	259
#define	INTEGER	260
#define	UNKNOWN	261
#define	NETWORK	262
#define	END	263
#define	AND	264
#define	FF	265
#define	OR	266
#define	C_ELEMENT	267
#define	NOT	268
#define	BUFFER	269
#define	AOI12	270
#define	AOI22	271
#define	OAI12	272
#define	OAI22	273
#define	TABLE1	274
#define	SPEC	275
#define	HIDDEN	276
#define	ANY	277
#define	NAND	278
#define	NOR	279
#define	ME	280
#define	INIT	281
#define	EXTERNALS	282
#define	BDD_ORDER	283
#define	SUBCIRCUITS	284
#define	NONE	285
#define	FANOUTS	286
#define	GATE_LIST	287
#define	ENTITY	288
#define	INTERNAL_STATUS	289

