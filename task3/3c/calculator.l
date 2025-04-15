%{
#include <stdio.h>
#include "y.tab.h"
%}

%%

[0-9]+      { yylval = atoi(yytext); return NUMBER; }
"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"("         { return '('; }
")"         { return ')'; }
[ \t]       { /* ignore whitespace */ }
\n          { return '\n'; }
.           { return yytext[0]; } /* return any other character as is */

%%

int yywrap(void) {
    return 1;
}
