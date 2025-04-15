CC = gcc
CFLAGS = -Wall

all: calculator

calculator: y.tab.o lex.yy.o
	$(CC) $(CFLAGS) -o calculator y.tab.o lex.yy.o -lfl

lex.yy.c: calculator.l
	flex calculator.l

y.tab.c y.tab.h: calculator.y
	yacc -d calculator.y

y.tab.o: y.tab.c
	$(CC) $(CFLAGS) -c y.tab.c

lex.yy.o: lex.yy.c y.tab.h
	$(CC) $(CFLAGS) -c lex.yy.c

clean:
	rm -f calculator lex.yy.c y.tab.c y.tab.h *.o
