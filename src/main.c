#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "interpreter.h"

extern FILE* yyin;
extern ASTNode* root;

int yyparse(void);

int main(int argc, char* argv[]) {
    int print_ast = 0;
    const char* filename = NULL;

    if (argc == 3) {
        if (strcmp(argv[1], "--ast") == 0) {
            print_ast = 1;
            filename = argv[2];
        } else {
            fprintf(stderr, "Invalid option: %s\n", argv[1]);
            fprintf(stderr, "Usage: %s [--ast] <input-file>\n", argv[0]);
            return EXIT_FAILURE;
        }
    } else if (argc == 2) {
        filename = argv[1];
    } else {
        fprintf(stderr, "Usage: %s [--ast] <input-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error opening input file");
        return EXIT_FAILURE;
    }

    yyin = input_file;
    int parse_result = yyparse();
    fclose(input_file);

    if (parse_result == 0) {
        if (print_ast) {
            ast_print(root, 0);
        }
        interpret(root);
        ast_free(root);
    } else {
        printf("Parsing failed.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}