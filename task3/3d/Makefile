CC=gcc
CFLAGS=-I.

compiler: y.tab.c lex.yy.c
	$(CC) -o compiler y.tab.c lex.yy.c -ll

y.tab.c: parser.y
	yacc -d parser.y

lex.yy.c: lexer.l
	lex lexer.l

clean:
	rm -f compiler lex.yy.c y.tab.c y.tab.h
