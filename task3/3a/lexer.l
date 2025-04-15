%{
#include <stdio.h>
#include "y.tab.h"
%}

%%
[0-9]+      { return NUMBER; }
"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"("         { return '('; }
")"         { return ')'; }
[ \t]       { /* ignore whitespace */ }
\n          { return '\n'; }
.           { return yytext[0]; } /* Return any other character as is */
%%

int yywrap() {
    return 1;
}
