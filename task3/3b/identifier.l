%{
#include <stdio.h>
#include "y.tab.h"
%}

%%
[a-zA-Z][a-zA-Z0-9]* { return IDENTIFIER; }
[ \t]               ; /* skip whitespace */
.                    { return yytext[0]; }
\n                   { return 0; }
%%

int yywrap() {
    return 1;
}
