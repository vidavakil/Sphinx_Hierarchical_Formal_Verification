# Note: LIBDIR, INCLUDEDIRS, and objdir must point to the appropriate VIS
# directories. 

CC    = gcc
CFLAG = -g -pg
#CFLAG = 
LEX   = lex
YACC  = bison
LIB	  = -ll -ly -lm -lglu -lcu

LIBDIR = -L/home/hala-00/vivakil/VIS/glu-1.2/myglu/lib

INCLUDEDIRS = -I/home/hala-00/vivakil/VIS/glu-1.2/myglu/include 

objdir = /home/hala-00/vivakil/VIS/glu-1.2/obj

OBJS  = main.o hash.o names.o parser.o scanner.o hparser.o hscanner.o aux_fn.o automata.o bdd_order.o complex.o failure.o flat.o gate.o gate_order.o module.o por.o print_graph.o project.o signal.o subcir.o util_bdd.o z_array.o $(objdir)/cuPort.o $(objdir)/cuddTable.o $(objdir)/var_set.o $(objdir)/cuPortIter.o $(objdir)/cuddAPI.o $(objdir)/cuddAnneal.o $(objdir)/cuddGroup.o  $(objdir)/cuddAddNeg.o $(objdir)/cuddGenCof.o $(objdir)/array.o $(objdir)/cpu_time.o $(objdir)/st.o

PROG  = sphinx

$(PROG): $(OBJS)
	$(CC) $(CFLAG) $(OBJS) $(OBJDIR) $(INCLUDEDIRS) $(LIBDIR) $(LIB)  -o $(PROG)
	
# C-files
main.o: main.c type.h names.h parser.h parser.yac scanner.lex hparser.yacc hscanner.lex aux_fn.c automata.c names.c bdd_order.c complex.c failure.c flat.c gate.c gate_order.c module.c por.c print_graph.c project.c signal.c subcir.c util_bdd.c z_array.c
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c main.c

# Simple hashing function
hash.o: hash.h
	$(CC) $(CFLAG) -c hash.c

# Name table module
names.o: names.c names.h hash.c hash.h type.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c names.c

# Automata module
automata.o: automata.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c automata.c

# Hoshik module module
aux_fn.o: aux_fn.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c aux_fn.c

# bdd_order module
bdd_order.o: bdd_order.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c bdd_order.c

# complex module
complex.o: complex.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c complex.c
	
# failure module
failure.o: failure.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c failure.c

# flat module
flat.o: flat.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c flat.c

# gate module
gate.o: gate.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c gate.c

# gate_order module
gate_order.o: gate_order.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c gate_order.c
	
# module module
module.o: module.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c module.c

# por module
por.o: por.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c por.c

# print_graph module
print_graph.o: print_graph.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c print_graph.c

# project module
project.o: project.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c project.c

# signal module
signal.o: signal.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c signal.c

# subcir module
subcir.o: subcir.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c subcir.c

# util_bdd module
util_bdd.o: util_bdd.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c util_bdd.c
	
# z_array module
z_array.o: z_array.c aux_fn.h type.h names.h hash.c parser.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c z_array.c
		
# Parser modules
parser.o: parser.c type.h names.h  
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c parser.c

parser.c: parser.yac scanner.c type.h  names.h 
	$(YACC) -d  parser.yac -o parser.c

hparser.o: hparser.c type.h names.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c hparser.c

hparser.c: hparser.yacc hscanner.c type.h names.h
	$(YACC) -d hparser.yacc -o hparser.c

# Lex modules
scanner.o: scanner.c parser.h type.h names.h 
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c scanner.c

scanner.c: scanner.lex
	$(LEX) -t scanner.lex > scanner.c

hscanner.o: hscanner.c hparser.h type.h names.h
	$(CC) $(CFLAG) $(INCLUDEDIRS) -c hscanner.c

hscanner.c: hscanner.lex
	$(LEX) -t hscanner.lex > hscanner.c
