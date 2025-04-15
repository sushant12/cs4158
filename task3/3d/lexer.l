%{
#include <stdio.h>
#include "y.tab.h"
#include <stdlib.h>
void yyerror(char *);
%}

%%
[0-9]+          { yylval.num = atoi(yytext); return NUMBER; }
main            { return MAIN; }
int             { return INT; }
[a-zA-Z][a-zA-Z0-9]* { yylval.id = strdup(yytext); return IDENTIFIER; }
[ \t\n]         ; /* skip whitespace */
[-+*/=,;(){}]   { return *yytext; }
.               { yyerror("Invalid character"); }
%%

int yywrap(void) {
    return 1;
}
