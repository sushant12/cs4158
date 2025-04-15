#!/bin/bash

# Generate C code from LEX and YACC files
lex identifier.l
yacc -d identifier.y

# Compile the generated C code
gcc lex.yy.c y.tab.c -o identifier

# Make the program executable
chmod +x identifier

# Run the program
./identifier
