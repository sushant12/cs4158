#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 100
#define MAX_TOKEN_LEN 100

// Arrays to store tokens
char constants[MAX_TOKENS][MAX_TOKEN_LEN];
char keywords[MAX_TOKENS][MAX_TOKEN_LEN];
char identifiers[MAX_TOKENS][MAX_TOKEN_LEN];
char operators[MAX_TOKENS][MAX_TOKEN_LEN];
char symbols[MAX_TOKENS][MAX_TOKEN_LEN];

// Counters for each array
int const_count = 0;
int kw_count = 0;
int id_count = 0;
int op_count = 0;
int sym_count = 0;

// List of keywords in our language
char* keyword_list[] = {"int", "float", "char", "if", "else", "while", "for", "return", "void"};
int keyword_count = 9;

// Function to check if a string is a keyword
int is_keyword(const char* str) {
    for (int i = 0; i < keyword_count; i++) {
        if (strcmp(str, keyword_list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to add token to appropriate array
void add_token(const char* lexeme, const char* type) {
    if (strcmp(type, "Constant") == 0 && const_count < MAX_TOKENS) {
        strcpy(constants[const_count++], lexeme);
    } else if (strcmp(type, "Keyword") == 0 && kw_count < MAX_TOKENS) {
        strcpy(keywords[kw_count++], lexeme);
    } else if (strcmp(type, "Identifier") == 0 && id_count < MAX_TOKENS) {
        strcpy(identifiers[id_count++], lexeme);
    } else if (strcmp(type, "Operator") == 0 && op_count < MAX_TOKENS) {
        strcpy(operators[op_count++], lexeme);
    } else if (strcmp(type, "Symbol") == 0 && sym_count < MAX_TOKENS) {
        strcpy(symbols[sym_count++], lexeme);
    }
}

// Function to analyze input
void lexical_analyze(FILE* fp) {
    int c;
    char buffer[MAX_TOKEN_LEN];
    int i;
    
    while ((c = fgetc(fp)) != EOF) {
        // Skip whitespace
        if (c == ' ' || c == '\t' || c == '\n') {
            continue;
        }
        
        // Skip comments
        if (c == '/') {
            int next_c = fgetc(fp);
            if (next_c == '/') {
                // It's a comment
                while ((c = fgetc(fp)) != '\n' && c != EOF);
                continue;
            } else {
                // It's a division operator
                ungetc(next_c, fp);
                buffer[0] = '/';
                buffer[1] = '\0';
                add_token(buffer, "Operator");
                continue;
            }
        }
        
        // Identifiers and keywords
        if (isalpha(c) || c == '_') {
            i = 0;
            buffer[i++] = c;
            while ((c = fgetc(fp)) != EOF && (isalnum(c) || c == '_')) {
                if (i < MAX_TOKEN_LEN - 1) {
                    buffer[i++] = c;
                }
            }
            ungetc(c, fp);
            buffer[i] = '\0';
            
            if (is_keyword(buffer)) {
                add_token(buffer, "Keyword");
            } else {
                add_token(buffer, "Identifier");
            }
            continue;
        }
        
        // Numbers
        if (isdigit(c)) {
            i = 0;
            buffer[i++] = c;
            while ((c = fgetc(fp)) != EOF && (isdigit(c) || c == '.')) {
                if (i < MAX_TOKEN_LEN - 1) {
                    buffer[i++] = c;
                }
            }
            ungetc(c, fp);
            buffer[i] = '\0';
            
            add_token(buffer, "Constant");
            continue;
        }
        
        // Operators
        if (c == '+' || c == '-' || c == '*' || c == '=' || c == '%' ||
            c == '<' || c == '>' || c == '!') {
            buffer[0] = c;
            buffer[1] = '\0';
            add_token(buffer, "Operator");
            continue;
        }
        
        // Symbols
        if (c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' ||
            c == '[' || c == ']') {
            buffer[0] = c;
            buffer[1] = '\0';
            add_token(buffer, "Symbol");
            continue;
        }
    }
}

// Function to display results
void display_results() {
    printf("Constants: [");
    for (int i = 0; i < const_count; i++) {
        printf("'%s' ", constants[i]);
    }
    printf("]\n");
    
    printf("KeyWord: [");
    for (int i = 0; i < kw_count; i++) {
        printf("'%s' ", keywords[i]);
    }
    printf("]\n");
    
    printf("Identifier: [");
    for (int i = 0; i < id_count; i++) {
        printf("'%s' ", identifiers[i]);
    }
    printf("]\n");
    
    printf("Operators: [");
    for (int i = 0; i < op_count; i++) {
        printf("'%s' ", operators[i]);
    }
    printf("]\n");
    
    printf("Symbols: [");
    for (int i = 0; i < sym_count; i++) {
        printf("'%s' ", symbols[i]);
    }
    printf("]\n");
}

int main(int argc, char* argv[]) {
    FILE* fp;
    
    if (argc == 1) {
        // No file provided, read from stdin
        fp = stdin;
        printf("Enter code (Ctrl+D to end on Unix/Linux, Ctrl+Z on Windows):\n");
    } else if (argc == 2) {
        // File provided
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            printf("Error: Could not open file %s\n", argv[1]);
            return 1;
        }
    } else {
        // Too many arguments
        printf("Usage: %s [input_file]\n", argv[0]);
        return 1;
    }
    
    lexical_analyze(fp);
    
    if (fp != stdin) {
        fclose(fp);
    }
    
    display_results();
    
    return 0;
}
