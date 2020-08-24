CC=gcc
OBJ=test
YACC=bison
LEX=flex
RM=@rm -f

$(OBJ): yacc.tab.o lex.yy.o main.o
	$(CC) -o $(OBJ) yacc.tab.o lex.yy.o main.o
	$(RM) yacc.tab.o lex.yy.o main.o

yacc.tab.o: yacc.tab.c main.h
	$(CC) -c yacc.tab.c

lex.yy.o: lex.yy.c main.h
	$(CC) -c lex.yy.c

yacc.tab.c: yacc.y main.h
	$(YACC) -d yacc.y

lex.yy.c: lex.l main.h
	$(LEX) lex.l

clean:
	$(RM) $(OBJ) *.o
