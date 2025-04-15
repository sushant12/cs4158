#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_SYMBOLS 100
#define MAX_PRODUCTIONS 100
#define MAX_PROD_LEN 50
#define MAX_INPUT_LEN 100
#define MAX_STACK_SIZE 100
#define MAX_LINE_LEN 100
#define EPSILON 'ε'

typedef struct {
    char lhs[2];  // Non-terminal (single character + null terminator)
    char rhs[MAX_PRODUCTIONS][MAX_PROD_LEN];  // Right-hand sides
    int num_rhs;  // Number of right-hand sides
} GrammarRule;

typedef struct {
    GrammarRule rules[MAX_SYMBOLS];
    int num_rules;
    
    char terminals[MAX_SYMBOLS];
    int num_terminals;
    
    char non_terminals[MAX_SYMBOLS];
    int num_non_terminals;
    
    char start_symbol;
    
    // FIRST and FOLLOW sets
    bool first[MAX_SYMBOLS][MAX_SYMBOLS];  // first[NT_idx][symbol_idx]
    bool follow[MAX_SYMBOLS][MAX_SYMBOLS]; // follow[NT_idx][symbol_idx]
    
    // Parsing table
    char parsing_table[MAX_SYMBOLS][MAX_SYMBOLS][MAX_PROD_LEN];
} LL1Parser;

// Function prototypes
void init_parser(LL1Parser *parser);
void parse_grammar(LL1Parser *parser, char lines[][MAX_LINE_LEN], int num_lines);
void compute_first(LL1Parser *parser);
void compute_follow(LL1Parser *parser);
void build_parsing_table(LL1Parser *parser);
bool parse_input(LL1Parser *parser, char *input);
void display_results(LL1Parser *parser);

// Helper functions
int get_terminal_index(LL1Parser *parser, char c);
int get_non_terminal_index(LL1Parser *parser, char c);
bool is_terminal(LL1Parser *parser, char c);
bool is_non_terminal(LL1Parser *parser, char c);

void init_parser(LL1Parser *parser) {
    parser->num_rules = 0;
    parser->num_terminals = 0;
    parser->num_non_terminals = 0;
    
    // Add $ as a terminal
    parser->terminals[parser->num_terminals++] = '$';
    
    // Initialize FIRST and FOLLOW sets to false
    memset(parser->first, 0, sizeof(parser->first));
    memset(parser->follow, 0, sizeof(parser->follow));
    
    // Initialize parsing table to empty strings
    memset(parser->parsing_table, 0, sizeof(parser->parsing_table));
}

void parse_grammar(LL1Parser *parser, char lines[][MAX_LINE_LEN], int num_lines) {
    for (int i = 0; i < num_lines; i++) {
        char *line = lines[i];
        if (strlen(line) == 0) continue;
        
        char *arrow = strstr(line, "->");
        if (!arrow) continue;
        
        // Extract LHS (left-hand side)
        char lhs = line[0];
        if (!parser->start_symbol) {
            parser->start_symbol = lhs;
        }
        
        // Add to non-terminals if not already present
        bool found = false;
        for (int j = 0; j < parser->num_non_terminals; j++) {
            if (parser->non_terminals[j] == lhs) {
                found = true;
                break;
            }
        }
        if (!found) {
            parser->non_terminals[parser->num_non_terminals++] = lhs;
        }
        
        // Find the rule for this LHS or create a new one
        int rule_idx = -1;
        for (int j = 0; j < parser->num_rules; j++) {
            if (parser->rules[j].lhs[0] == lhs) {
                rule_idx = j;
                break;
            }
        }
        if (rule_idx == -1) {
            rule_idx = parser->num_rules;
            parser->num_rules++;
            parser->rules[rule_idx].lhs[0] = lhs;
            parser->rules[rule_idx].lhs[1] = '\0';
            parser->rules[rule_idx].num_rhs = 0;
        }
        
        // Parse RHS (right-hand sides)
        char *rhs = arrow + 2;  // Skip "->"
        char *token = strtok(rhs, "|");
        while (token != NULL) {
            // Remove leading/trailing spaces
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') end--;
            *(end + 1) = '\0';
            
            // Add the production
            strcpy(parser->rules[rule_idx].rhs[parser->rules[rule_idx].num_rhs], token);
            parser->rules[rule_idx].num_rhs++;
            
            // Add terminals
            for (int j = 0; token[j] != '\0'; j++) {
                char c = token[j];
                if (c != EPSILON && !isupper((unsigned char)c)) {
                    // Check if terminal already exists
                    bool found = false;
                    for (int k = 0; k < parser->num_terminals; k++) {
                        if (parser->terminals[k] == c) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        parser->terminals[parser->num_terminals++] = c;
                    }
                }
            }
            
            token = strtok(NULL, "|");
        }
    }
}

int get_terminal_index(LL1Parser *parser, char c) {
    for (int i = 0; i < parser->num_terminals; i++) {
        if (parser->terminals[i] == c) {
            return i;
        }
    }
    return -1;
}

int get_non_terminal_index(LL1Parser *parser, char c) {
    for (int i = 0; i < parser->num_non_terminals; i++) {
        if (parser->non_terminals[i] == c) {
            return i;
        }
    }
    return -1;
}

bool is_terminal(LL1Parser *parser, char c) {
    return get_terminal_index(parser, c) != -1;
}

bool is_non_terminal(LL1Parser *parser, char c) {
    return get_non_terminal_index(parser, c) != -1;
}

void compute_first(LL1Parser *parser) {
    // Initialize FIRST sets for terminals
    for (int i = 0; i < parser->num_terminals; i++) {
        char t = parser->terminals[i];
        int t_idx = get_terminal_index(parser, t);
        for (int j = 0; j < parser->num_terminals; j++) {
            parser->first[t_idx][j] = (i == j);
        }
    }
    
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (int i = 0; i < parser->num_rules; i++) {
            char lhs = parser->rules[i].lhs[0];
            int lhs_idx = get_non_terminal_index(parser, lhs);
            
            for (int j = 0; j < parser->rules[i].num_rhs; j++) {
                char *rhs = parser->rules[i].rhs[j];
                
                // If RHS is ε, add ε to FIRST(LHS)
                if (rhs[0] == EPSILON || rhs[0] == '\0') {
                    int eps_idx = get_terminal_index(parser, EPSILON);
                    if (!parser->first[lhs_idx][eps_idx]) {
                        parser->first[lhs_idx][eps_idx] = true;
                        changed = true;
                    }
                    continue;
                }
                
                // Process the first symbol
                char first_sym = rhs[0];
                if (is_terminal(parser, first_sym)) {
                    int t_idx = get_terminal_index(parser, first_sym);
                    if (!parser->first[lhs_idx][t_idx]) {
                        parser->first[lhs_idx][t_idx] = true;
                        changed = true;
                    }
                } else if (is_non_terminal(parser, first_sym)) {
                    int nt_idx = get_non_terminal_index(parser, first_sym);
                    // Add all non-ε terminals from FIRST(first_sym) to FIRST(lhs)
                    for (int k = 0; k < parser->num_terminals; k++) {
                        if (parser->terminals[k] != EPSILON && 
                            parser->first[nt_idx][k] && 
                            !parser->first[lhs_idx][k]) {
                            parser->first[lhs_idx][k] = true;
                            changed = true;
                        }
                    }
                    
                    // If ε is in FIRST(first_sym) and this is a single symbol, add ε to FIRST(lhs)
                    int eps_idx = get_terminal_index(parser, EPSILON);
                    if (rhs[1] == '\0' && parser->first[nt_idx][eps_idx] && !parser->first[lhs_idx][eps_idx]) {
                        parser->first[lhs_idx][eps_idx] = true;
                        changed = true;
                    }
                }
            }
        }
    }
}

void compute_follow(LL1Parser *parser) {
    // Add $ to FOLLOW(start_symbol)
    int start_idx = get_non_terminal_index(parser, parser->start_symbol);
    int dollar_idx = get_terminal_index(parser, '$');
    parser->follow[start_idx][dollar_idx] = true;
    
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (int i = 0; i < parser->num_rules; i++) {
            char lhs = parser->rules[i].lhs[0];
            int lhs_idx = get_non_terminal_index(parser, lhs);
            
            for (int j = 0; j < parser->rules[i].num_rhs; j++) {
                char *rhs = parser->rules[i].rhs[j];
                int len = strlen(rhs);
                
                for (int k = 0; k < len; k++) {
                    char current = rhs[k];
                    if (!is_non_terminal(parser, current)) continue;
                    
                    int current_idx = get_non_terminal_index(parser, current);
                    
                    // If it's not the last character
                    if (k < len - 1) {
                        char next = rhs[k+1];
                        
                        if (is_terminal(parser, next)) {
                            int next_idx = get_terminal_index(parser, next);
                            if (!parser->follow[current_idx][next_idx]) {
                                parser->follow[current_idx][next_idx] = true;
                                changed = true;
                            }
                        } else if (is_non_terminal(parser, next)) {
                            int next_idx = get_non_terminal_index(parser, next);
                            
                            // Add FIRST(next) - {ε} to FOLLOW(current)
                            for (int t = 0; t < parser->num_terminals; t++) {
                                char term = parser->terminals[t];
                                if (term != EPSILON && 
                                    parser->first[next_idx][t] && 
                                    !parser->follow[current_idx][t]) {
                                    parser->follow[current_idx][t] = true;
                                    changed = true;
                                }
                            }
                        }
                    }
                    
                    // If it's the last character or all following symbols can derive ε
                    // add FOLLOW(lhs) to FOLLOW(current)
                    if (k == len - 1) {
                        for (int t = 0; t < parser->num_terminals; t++) {
                            if (parser->follow[lhs_idx][t] && !parser->follow[current_idx][t]) {
                                parser->follow[current_idx][t] = true;
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

void build_parsing_table(LL1Parser *parser) {
    // Initialize parsing table
    memset(parser->parsing_table, 0, sizeof(parser->parsing_table));
    
    for (int i = 0; i < parser->num_rules; i++) {
        char lhs = parser->rules[i].lhs[0];
        int lhs_idx = get_non_terminal_index(parser, lhs);
        
        for (int j = 0; j < parser->rules[i].num_rhs; j++) {
            char *rhs = parser->rules[i].rhs[j];
            
            if (rhs[0] == EPSILON || strlen(rhs) == 0) {
                // For ε-productions, use FOLLOW set
                for (int t = 0; t < parser->num_terminals; t++) {
                    if (parser->follow[lhs_idx][t]) {
                        if (parser->parsing_table[lhs_idx][t][0] != '\0') {
                            printf("WARNING: Grammar is not LL(1). Conflict at [%c, %c]\n", lhs, parser->terminals[t]);
                        } else {
                            sprintf(parser->parsing_table[lhs_idx][t], "%c->%s", lhs, rhs[0] == EPSILON ? "ε" : "");
                        }
                    }
                }
            } else {
                char first_sym = rhs[0];
                
                if (is_terminal(parser, first_sym)) {
                    int t_idx = get_terminal_index(parser, first_sym);
                    if (parser->parsing_table[lhs_idx][t_idx][0] != '\0') {
                        printf("WARNING: Grammar is not LL(1). Conflict at [%c, %c]\n", lhs, first_sym);
                    } else {
                        sprintf(parser->parsing_table[lhs_idx][t_idx], "%c->%s", lhs, rhs);
                    }
                } else if (is_non_terminal(parser, first_sym)) {
                    int nt_idx = get_non_terminal_index(parser, first_sym);
                    
                    // Add entry for each terminal in FIRST(first_sym)
                    for (int t = 0; t < parser->num_terminals; t++) {
                        if (parser->terminals[t] != EPSILON && parser->first[nt_idx][t]) {
                            if (parser->parsing_table[lhs_idx][t][0] != '\0') {
                                printf("WARNING: Grammar is not LL(1). Conflict at [%c, %c]\n", lhs, parser->terminals[t]);
                            } else {
                                sprintf(parser->parsing_table[lhs_idx][t], "%c->%s", lhs, rhs);
                            }
                        }
                    }
                }
            }
        }
    }
}

bool parse_input(LL1Parser *parser, char *input) {
    // Add $ to the end of the input
    int input_len = strlen(input);
    input[input_len] = '$';
    input[input_len + 1] = '\0';
    
    // Initialize stack with $ and start symbol
    char stack[MAX_STACK_SIZE];
    int stack_top = -1;
    stack[++stack_top] = '$';
    stack[++stack_top] = parser->start_symbol;
    
    int input_pos = 0;
    
    printf("Parsing sequence and actions\n");
    printf("STACK\tINPUT\tACTION\n");
    printf("------------------------------------------------------------------------------------\n");
    
    while (stack_top >= 0) {
        // Display stack
        printf("[");
        for (int i = 0; i <= stack_top; i++) {
            printf("%c", stack[i]);
            if (i < stack_top) printf(" ");
        }
        printf("]\t");
        
        // Display remaining input
        printf("%s\t", &input[input_pos]);
        
        char top = stack[stack_top];
        char current = input[input_pos];
        
        if (top == '$' && current == '$') {
            printf("\nParsed successfully.\n");
            return true;
        }
        
        if (is_terminal(parser, top)) {
            if (top == current) {
                stack_top--;
                input_pos++;
                printf("Reduced: %c\n", top);
            } else {
                printf("Error: Expected %c, found %c\n", top, current);
                return false;
            }
        } else {  // top is a non-terminal
            int nt_idx = get_non_terminal_index(parser, top);
            int t_idx = get_terminal_index(parser, current);
            
            if (parser->parsing_table[nt_idx][t_idx][0] != '\0') {
                stack_top--;  // Pop the non-terminal
                
                // Get the right-hand side of the production
                char *arrow = strchr(parser->parsing_table[nt_idx][t_idx], '>');
                char *rhs = arrow + 1;
                
                // Push the RHS in reverse order
                if (strcmp(rhs, "ε") != 0) {
                    int rhs_len = strlen(rhs);
                    for (int i = rhs_len - 1; i >= 0; i--) {
                        stack[++stack_top] = rhs[i];
                    }
                }
                
                printf("Shift: %s\n", parser->parsing_table[nt_idx][t_idx]);
            } else {
                printf("Error: No rule for %c with %c\n", top, current);
                return false;
            }
        }
    }
    
    return false;
}

void display_results(LL1Parser *parser) {
    // Display FIRST sets
    for (int i = 0; i < parser->num_non_terminals; i++) {
        char nt = parser->non_terminals[i];
        printf("FIRST( %c ): { ", nt);
        bool first = true;
        for (int j = 0; j < parser->num_terminals; j++) {
            if (parser->first[i][j]) {
                if (!first) printf(", ");
                first = false;
                printf("%c", parser->terminals[j]);
            }
        }
        printf(" }\n");
    }
    
    // Display FOLLOW sets
    for (int i = 0; i < parser->num_non_terminals; i++) {
        char nt = parser->non_terminals[i];
        printf("FOLLOW( %c ): { ", nt);
        bool first = true;
        for (int j = 0; j < parser->num_terminals; j++) {
            if (parser->follow[i][j]) {
                if (!first) printf(", ");
                first = false;
                printf("%c", parser->terminals[j]);
            }
        }
        printf(" }\n");
    }
    
    // Display parsing table
    printf("Predictive Parsing Table:\n");
    printf("    ");
    for (int j = 0; j < parser->num_terminals; j++) {
        printf("%c ", parser->terminals[j]);
    }
    printf("\n");
    printf("----------------------------------------------------\n");
    
    for (int i = 0; i < parser->num_non_terminals; i++) {
        printf("%c | ", parser->non_terminals[i]);
        for (int j = 0; j < parser->num_terminals; j++) {
            if (parser->parsing_table[i][j][0] != '\0') {
                printf("%s ", parser->parsing_table[i][j]);
            } else {
                printf("_ ");
            }
        }
        printf("\n");
    }
}

int main() {
    LL1Parser parser;
    init_parser(&parser);
    
    char grammar_lines[MAX_PRODUCTIONS][MAX_LINE_LEN];
    int num_lines = 0;
    
    printf("Enter grammar rules (format: S->aA, end with an empty line):\n");
    while (1) {
        if (fgets(grammar_lines[num_lines], MAX_LINE_LEN, stdin) == NULL) {
            break;
        }
        
        // Remove newline character
        size_t len = strlen(grammar_lines[num_lines]);
        if (len > 0 && grammar_lines[num_lines][len-1] == '\n') {
            grammar_lines[num_lines][len-1] = '\0';
        }
        
        if (strlen(grammar_lines[num_lines]) == 0) {
            break;
        }
        
        num_lines++;
    }
    
    parse_grammar(&parser, grammar_lines, num_lines);
    compute_first(&parser);
    compute_follow(&parser);
    build_parsing_table(&parser);
    display_results(&parser);
    
    char input[MAX_INPUT_LEN];
    printf("Enter string for parsing: ");
    if (fgets(input, MAX_INPUT_LEN, stdin) == NULL) {
        return 1;
    }
    
    // Remove newline character
    size_t len = strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        input[len-1] = '\0';
    }
    
    parse_input(&parser, input);
    
    return 0;
}
