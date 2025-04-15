%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(char *);
int yylex(void);

struct quad {
    int pos;
    char op;
    char arg1[10];
    char arg2[10];
    char result[10];
};

struct quad quads[100];
int quad_index = 0;
int temp_var = 0;

char* new_temp() {
    static char temp[10];
    sprintf(temp, "t%d", temp_var++);
    return strdup(temp);
}

void emit(char op, char* arg1, char* arg2, char* result) {
    quads[quad_index].pos = quad_index;
    quads[quad_index].op = op;
    strcpy(quads[quad_index].arg1, arg1);
    strcpy(quads[quad_index].arg2, arg2);
    strcpy(quads[quad_index].result, result);
    quad_index++;
}
%}

%union {
    int num;
    char* id;
    struct {
        char* code;
        char* addr;
    } expr;
}

%token <num> NUMBER
%token <id> IDENTIFIER
%token MAIN INT
%type <expr> expr

%%

program:
    MAIN '(' ')' '{' declarations statements '}'
    ;

declarations:
    INT id_list ';'
    ;

id_list:
    IDENTIFIER
    | id_list ',' IDENTIFIER
    ;

statements:
    statement
    | statements statement
    ;

statement:
    IDENTIFIER '=' expr ';'  { emit('=', $3.addr, "", $1); }
    ;

expr:
    IDENTIFIER  { $$.addr = $1; }
    | expr '+' expr  { 
        $$.addr = new_temp();
        emit('+', $1.addr, $3.addr, $$.addr);
    }
    | expr '*' expr  {
        $$.addr = new_temp();
        emit('*', $1.addr, $3.addr, $$.addr);
    }
    | expr '/' expr  {
        $$.addr = new_temp();
        emit('/', $1.addr, $3.addr, $$.addr);
    }
    | '(' expr ')'  { $$ = $2; }
    ;

%%

void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

void print_quads() {
    printf("-----------------------------------\n");
    printf("Pos Operator Arg1 Arg2 Result\n");
    printf("-----------------------------------\n");
    for(int i = 0; i < quad_index; i++) {
        printf("%d %c %s %s %s\n", 
            quads[i].pos,
            quads[i].op,
            quads[i].arg1,
            quads[i].arg2,
            quads[i].result);
    }
    printf("-----------------------------------\n");
}

int main(void) {
    yyparse();
    print_quads();
    return 0;
}
