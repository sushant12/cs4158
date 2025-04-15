%{
#include <stdio.h>
#include <stdlib.h>
void yyerror(const char *s);
int yylex(void);
%}

%token NUMBER

%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%%

input:   /* empty string */
       | input line
;

line:     '\n'
        | expr '\n'  { printf("Result: %d\n", $1); }
        | error '\n' { printf("Error. Failed to parse.\n"); yyerrok; }
;

expr:     NUMBER               { $$ = $1; }
        | expr '+' expr        { $$ = $1 + $3; }
        | expr '-' expr        { $$ = $1 - $3; }
        | expr '*' expr        { $$ = $1 * $3; }
        | expr '/' expr        { 
            if ($3 == 0) {
                yyerror("Division by zero");
                $$ = 0;
            } else {
                $$ = $1 / $3; 
            }
          }
        | '-' expr %prec UMINUS { $$ = -$2; }
        | '(' expr ')'          { $$ = $2; }
;

%%

void yyerror(const char *s) {
    // We'll handle specific errors in the grammar
}

int main(void) {
    while (1) {
        printf("Enter arithmetic expression: ");
        if (yyparse()) {
            // Error is handled in the grammar
        }
    }
    return 0;
}
