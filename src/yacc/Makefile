LEX=flex
YACC=bison
CC=g++
OBJECT=main

$(OBJECT): lex.yy.o yacc.tab.o
	$(CC) lex.yy.o yacc.tab.o -o $(OBJECT)

lex.yy.o: lex.yy.c yacc.tab.h main.h
	$(CC) -c lex.yy.c

yacc.tab.o: yacc.tab.c main.h
	$(CC) -c yacc.tab.c

yacc.tab.c yacc.tab.h: yacc.y
	$(YACC) -d yacc.y

lex.yy.c: lex.l
	$(LEX) lex.l

clean:
	@rm -f $(OBJECT) *.o

