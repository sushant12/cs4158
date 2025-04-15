#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 1000
#define MAX_CONSTANTS 100
#define MAX_PARTS 10
#define MAX_VAR_LENGTH 100

// Structure to store a constant and its value
typedef struct {
    char name[MAX_VAR_LENGTH];
    char value[MAX_VAR_LENGTH];
} Constant;

// Check if a string is a number
bool is_digit(const char* str) {
    if (str == NULL || *str == '\0') {
        return false;
    }
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

// Find a constant by name, return index or -1 if not found
int find_constant(const Constant* constants, int count, const char* name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(constants[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Perform constant propagation
char** constant_propagation(char** input_lines, int line_count, int* output_count) {
    Constant constants[MAX_CONSTANTS];
    int constant_count = 0;
    
    char** output_lines = malloc(line_count * sizeof(char*));
    *output_count = 0;
    
    // Process each line
    for (int i = 0; i < line_count; i++) {
        char line_copy[MAX_LINE_LENGTH];
        strcpy(line_copy, input_lines[i]);
        
        // Split the line into parts
        char* parts[MAX_PARTS];
        int part_count = 0;
        
        char* token = strtok(line_copy, " \t\n");
        while (token != NULL && part_count < MAX_PARTS) {
            parts[part_count++] = token;
            token = strtok(NULL, " \t\n");
        }
        
        if (part_count > 0) {
            // Process based on operation
            if (strcmp(parts[0], "=") == 0 && part_count >= 4) {
                // Assignment: "= value - var"
                if (is_digit(parts[1])) {
                    char var[MAX_VAR_LENGTH];
                    strcpy(var, parts[3]); // The variable being assigned
                    
                    int idx = find_constant(constants, constant_count, var);
                    if (idx == -1) {
                        // Add new constant
                        strcpy(constants[constant_count].name, var);
                        strcpy(constants[constant_count].value, parts[1]);
                        constant_count++;
                    } else {
                        // Update existing constant
                        strcpy(constants[idx].value, parts[1]);
                    }
                }
            } else {
                // Other operations: +, -, *, /
                char output_line[MAX_LINE_LENGTH] = "";
                strcat(output_line, parts[0]); // Add operation
                
                // Add operands, replacing constants if known
                for (int j = 1; j < part_count; j++) {
                    strcat(output_line, " ");
                    int idx = find_constant(constants, constant_count, parts[j]);
                    if (idx != -1) {
                        strcat(output_line, constants[idx].value);
                    } else {
                        strcat(output_line, parts[j]);
                    }
                }
                
                // Add to output
                output_lines[*output_count] = strdup(output_line);
                (*output_count)++;
            }
        }
    }
    
    return output_lines;
}

int main() {
    // Read input
    char* input_lines[100]; // Assuming max 100 lines
    int line_count = 0;
    
    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, sizeof(buffer), stdin)) {
        // Remove newline character if present
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (strlen(buffer) == 0) { // End on empty line
            break;
        }
        
        input_lines[line_count] = strdup(buffer);
        line_count++;
    }
    
    // Perform constant propagation
    int output_count;
    char** output_lines = constant_propagation(input_lines, line_count, &output_count);
    
    // Print results
    for (int i = 0; i < output_count; i++) {
        printf("%s\n", output_lines[i]);
    }
    
    // Clean up
    for (int i = 0; i < line_count; i++) {
        free(input_lines[i]);
    }
    for (int i = 0; i < output_count; i++) {
        free(output_lines[i]);
    }
    free(output_lines);
    
    return 0;
}
