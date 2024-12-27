%{
#include <stdio.h>

extern int yylex();
extern int yyparse();
extern FILE *yyin;
int yyerror(const char *msg);

int valid_grammar = 1;
%}

%define parse.error verbose


%token AND OR NOT IF THEN ELSE ELIF FOR TO DO BREAK CONTINUE PRINT READINT READSTR BEGIN_CTX END_CTX EXIT SUBSTRING LENGTH POSITION CONCATENATE STRING_TYPE INTEGER_TYPE
%token PLUS MINUS MULTIPLY DIVIDE MODULO EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL NOT_EQUAL STRING_EQUAL STRING_NOT_EQUAL ASSIGN LPAREN RPAREN SEMICOLON COMMA
%token IDENTIFIER NUMBER STRING_LITERAL

%%

program : declarations instr ;

declarations : declarations declaration SEMICOLON 
             | ;

declaration:
    STRING_TYPE IDENTIFIER
    | INTEGER_TYPE IDENTIFIER
    ;

instr:

    | instr simple_instr SEMICOLON
    ;

simple_instr:
    assign_stat
    | if_stat
    | for_stat
    | BEGIN_CTX instr END_CTX
    | output_stat
    | BREAK
    | CONTINUE
    | EXIT
    ;

assign_stat:
    IDENTIFIER ASSIGN num_expr
    | IDENTIFIER ASSIGN str_expr
    ;

if_stat:
    IF bool_expr THEN simple_instr if_options
    ;

if_options:
    /* pusty */
    | ELIF bool_expr THEN simple_instr if_options
    | ELSE simple_instr
    ;

for_stat:
    FOR IDENTIFIER ASSIGN num_expr TO num_expr DO simple_instr
    ;

output_stat:
    PRINT LPAREN num_expr RPAREN
    | PRINT LPAREN str_expr RPAREN
    ;

num_expr:
    num_expr PLUS t_num_expr
    | num_expr MINUS t_num_expr
    | t_num_expr
    ;

t_num_expr:
    t_num_expr MULTIPLY f_num_expr
    | t_num_expr DIVIDE f_num_expr
    | t_num_expr MODULO f_num_expr
    | f_num_expr
    ;

f_num_expr:
    NUMBER
    | IDENTIFIER
    | READINT
    | MINUS num_expr
    | LPAREN num_expr RPAREN
    | LENGTH LPAREN str_expr RPAREN
    | POSITION LPAREN str_expr COMMA str_expr RPAREN
    ;

str_expr:
    STRING_LITERAL
    | IDENTIFIER
    | READSTR
    | CONCATENATE LPAREN str_expr COMMA str_expr RPAREN
    | SUBSTRING LPAREN str_expr COMMA num_expr COMMA num_expr RPAREN
    ;

bool_expr:
    bool_expr OR t_bool_expr
    | t_bool_expr
    ;

t_bool_expr:
    t_bool_expr AND f_bool_expr
    | f_bool_expr
    ;

f_bool_expr:
    "true"
    | "false"
    | LPAREN bool_expr RPAREN
    | NOT bool_expr
    | num_expr num_rel num_expr
    | str_expr str_rel str_expr
    ;

num_rel:
    EQUAL
    | LESS
    | LESS_EQUAL
    | GREATER
    | GREATER_EQUAL
    | NOT_EQUAL
    ;

str_rel:
    STRING_EQUAL
    | STRING_NOT_EQUAL
    ;

%%

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s program.sl\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 2) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Error opening file");
            return EXIT_FAILURE;
        }
    }

#if YYDEBUG == 1
    yydebug = 1;
#endif
    rewind(yyin);
    yyparse();
    fclose(yyin);

    if (valid_grammar) {
        printf("Execution finished - program valid\n");
    }   return EXIT_SUCCESS;
    
    printf("Execution failed - program invalid\n");
    return 1;
}

void print_error(int line_number, const char *error_message, const char* line_buffer) {
    fprintf(stderr, "Parsing failed at line %d: %s\n\t--> %s\n", line_number, error_message, line_buffer);
}

int yyerror(const char *msg) {
	extern int yylineno;

    char buffer[1024]; 
    long original_position = ftell(yyin); 
    int valid_position = 1;

    rewind(yyin);
    for (int i = 0; i < yylineno; ++i) {
        if (!fgets(buffer, sizeof(buffer), yyin)) {
            valid_position = 0;
            break;
        }
    }

    if (valid_position) {
        print_error(yylineno, msg, buffer);
    } else {
        print_error(yylineno, msg, "Error: Could not read line");
    }

	return valid_grammar = 0;
}


