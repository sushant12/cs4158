%{
#include <stdio.h>
%}

%%
"void"|"int"|"main"          { printf("%s Reserved Keyword\n", yytext); }
"(" | ")" | "{" | "}" | "," | ";" { printf("%s Special Operator\n", yytext); }
"="                           { printf("%s Arithmetic Operator\n", yytext); }
[0-9]+\.[0-9]+                { printf("%s Floating Number\n", yytext); }
[0-9]+                        { printf("%s Constant\n", yytext); }
[a-zA-Z_][a-zA-Z0-9_]*        { printf("%s Identifier\n", yytext); }
"//".*                        { /* Ignore comments */ }
[ \t\n]+                      { /* Ignore whitespace */ }
.                             { printf("%s Unknown Token\n", yytext); }
%%

int main() {
    yylex();
    return 0;
}
