calculator: lex.yy.c y.tab.c
	gcc -o calculator lex.yy.c y.tab.c -lfl

y.tab.c: parser.y
	yacc -d parser.y

lex.yy.c: lexer.l
	lex lexer.l

clean:
	rm -f calculator y.tab.c y.tab.h lex.yy.c
