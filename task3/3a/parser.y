%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}

%token NUMBER

%%
program: expression '\n' { printf("Valid Arithmetic Expression\n"); }
       ;

expression: expression '+' term
          | expression '-' term
          | term
          ;

term: term '*' factor
    | term '/' factor
    | factor
    ;

factor: NUMBER
      | '(' expression ')'
      ;
%%

void yyerror(const char *s) {
    printf("Error. Failed to parse.\n");
}

int main() {
    printf("Enter arithmetic expression: ");
    yyparse();
    return 0;
}
