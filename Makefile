CC       = gcc
CCFLAGS  = -g -O0 -Wall
INC      = -I/usr/local/include -I/usr/local/lib/include
LIBS     = -L/usr/local/lib
PROGRAM  = josh
OBJS     = josh_ast.o josh_cmd.o josh_main.o josh_parse.o uba.o

$(PROGRAM): $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $(PROGRAM) $(LIBS)

josh_ast.o: josh_ast.c josh_ast.h josh_cmd.h
	$(CC) $(CCFLAGS) $(INC) -c josh_ast.c -o josh_ast.o $(LIBS)

josh_cmd.o: josh_cmd.c josh_cmd.h
	$(CC) $(CCFLAGS) $(INC) -c josh_cmd.c -o josh_cmd.o $(LIBS)

josh_main.o: josh_main.c josh_ast.h josh_cmd.h josh_main.h josh_parse.h
	$(CC) $(CCFLAGS) $(INC) -c josh_main.c -o josh_main.o $(LIBS)

josh_parse.o: josh_parse.c josh_ast.h josh_parse.h uba.h
	$(CC) $(CCFLAGS) $(INC) -c josh_parse.c -o josh_parse.o $(LIBS)

uba.o: uba.c uba.h
	$(CC) $(CCFLAGS) $(INC) -c uba.c -o uba.o $(LIBS)

.PHONY:
clean:
	/bin/rm -f $(OBJS)
	/bin/rm -f $(PROGRAM)
	/bin/rm -f core
