#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to find position of an operator in expression
int find_operator(char *expr, char op) {
    char *pos = strchr(expr, op);
    if (pos == NULL)
        return -1;
    return pos - expr;
}

// Function to extract operand (left or right of operator)
void extract_operand(char *expr, int op_pos, int direction, char *operand) {
    int i = 0;
    
    if (direction < 0) { // Left operand
        int pos = op_pos - 1;
        while (pos >= 0 && (isalnum(expr[pos]))) {
            pos--;
        }
        pos++;
        while (pos < op_pos) {
            operand[i++] = expr[pos++];
        }
    } else { // Right operand
        int pos = op_pos + 1;
        while (expr[pos] != '\0' && (isalnum(expr[pos]))) {
            operand[i++] = expr[pos++];
        }
    }
    operand[i] = '\0';
}

// Function to replace substring in expression
void replace_in_expression(char *expr, int start, int end, char *replacement) {
    char temp[100];
    int i = 0, j = 0;
    
    // Copy before the replaced part
    while (i < start) {
        temp[j++] = expr[i++];
    }
    
    // Copy replacement
    i = 0;
    while (replacement[i] != '\0') {
        temp[j++] = replacement[i++];
    }
    
    // Skip the part being replaced
    i = end + 1;
    
    // Copy after the replaced part
    while (expr[i] != '\0') {
        temp[j++] = expr[i++];
    }
    temp[j] = '\0';
    
    strcpy(expr, temp);
}

void generate_intermediate_code(char *expr) {
    char lhs[10], rhs[100];
    char left_operand[20], right_operand[20];
    char temp_expr[100], temp_var_name[2];
    char *temp_vars = "ZYXWVUTSRQ";
    int var_index = 0;
    
    // Split the expression at '='
    char *eq_pos = strchr(expr, '=');
    if (eq_pos == NULL) {
        printf("Invalid expression\n");
        return;
    }
    
    // Extract LHS and RHS
    int eq_index = eq_pos - expr;
    strncpy(lhs, expr, eq_index);
    lhs[eq_index] = '\0';
    strcpy(rhs, eq_pos + 1);
    
    // Handle multiplication and division first
    while (strchr(rhs, '*') != NULL || strchr(rhs, '/') != NULL) {
        int mul_pos = find_operator(rhs, '*');
        int div_pos = find_operator(rhs, '/');
        int op_pos;
        char op;
        
        if (mul_pos != -1 && (div_pos == -1 || mul_pos < div_pos)) {
            op_pos = mul_pos;
            op = '*';
        } else {
            op_pos = div_pos;
            op = '/';
        }
        
        // Extract operands
        extract_operand(rhs, op_pos, -1, left_operand);
        extract_operand(rhs, op_pos, 1, right_operand);
        
        // Get a temporary variable name
        temp_var_name[0] = temp_vars[var_index++];
        temp_var_name[1] = '\0';
        
        // Print intermediate code
        printf("%s=%s%c%s\n", temp_var_name, left_operand, op, right_operand);
        
        // Determine bounds for replacement
        int left_start = op_pos - strlen(left_operand);
        int right_end = op_pos + strlen(right_operand);
        
        // Replace in expression
        char lower_temp[2] = {tolower(temp_var_name[0]), '\0'};
        replace_in_expression(rhs, left_start, right_end, lower_temp);
    }
    
    // Handle addition and subtraction
    while (strchr(rhs, '+') != NULL || strchr(rhs, '-') != NULL) {
        int add_pos = find_operator(rhs, '+');
        int sub_pos = find_operator(rhs, '-');
        int op_pos;
        char op;
        
        if (add_pos != -1 && (sub_pos == -1 || add_pos < sub_pos)) {
            op_pos = add_pos;
            op = '+';
        } else {
            op_pos = sub_pos;
            op = '-';
        }
        
        // Extract operands
        extract_operand(rhs, op_pos, -1, left_operand);
        extract_operand(rhs, op_pos, 1, right_operand);
        
        // Get a temporary variable name
        temp_var_name[0] = temp_vars[var_index++];
        temp_var_name[1] = '\0';
        
        // Print intermediate code
        printf("%s=%s%c%s\n", temp_var_name, left_operand, op, right_operand);
        
        // Determine bounds for replacement
        int left_start = op_pos - strlen(left_operand);
        int right_end = op_pos + strlen(right_operand);
        
        // Replace in expression
        char lower_temp[2] = {tolower(temp_var_name[0]), '\0'};
        replace_in_expression(rhs, left_start, right_end, lower_temp);
    }
    
    // Final assignment
    printf("%s=%c\n", lhs, toupper(rhs[0]));
}

int main() {
    char expr[100];
    printf("Enter the expression: ");
    fgets(expr, 100, stdin);
    
    // Remove newline character
    expr[strcspn(expr, "\n")] = '\0';
    
    printf("Intermediate code:\n");
    generate_intermediate_code(expr);
    
    return 0;
}
