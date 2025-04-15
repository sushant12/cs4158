%{
#include <stdio.h>
#include <stdlib.h>
void yyerror(char *s);
int yylex();
%}

%token IDENTIFIER

%%
program:
    identifier { printf("Valid Identifier\n"); }
    ;

identifier:
    IDENTIFIER
    ;
%%

void yyerror(char *s) {
    printf("Error. Failed to parse\n");
}

int main() {
    printf("Enter identifier: ");
    yyparse();
    return 0;
}
