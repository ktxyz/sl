%{
#include "ast.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylineno;

int yylex(void);
void yyerror(const char* msg);
ASTNode* root;

#ifdef YYDEBUG
yydebug = 1;
#endif

// TO NIE BYLO PRZYJEMNE PRZEZYCIE
// ZDECYDOWANIE WOLALBYM ZNOWU NAPISAC Z PALCA
// https://github.com/SKE-Projekt/Edlang
// NIE DA SIE ZNALEZC ZADNYCH DOBRYCH TUTORIALI
// KTORE NIE WYMAGALYBY PRZECZYTANIA 200 STRON KSIAZKI
// TYLKO PO TO ZEBY ZNALEZC JAK SIE JEDNAK POWINNO POPRAWNIE
// KOD C++ WYGENEROWAC, RAZ ZNALAZLEM STRONE GDZIE BYLO POKAZANE
// ZE NIE WARTO TYLKO LEPIEJ ZALINKOWAC TO W C DO C++
// ALE MI ZNIKNELA :( WIEC SIE JUZ PODDALEM

const char* var_type_to_str(VarType type) {
    switch(type) {
        case INT_TYPE:  return "integer";
        case STR_TYPE:  return "string";
        case UNDEFINED: return "undefined";
        default:        return "unknown";
    }
}

void check_variable_exists(const char* ident) {
    VarType actual = symtab_lookup(ident);
    if (actual == UNDEFINED) {
        fprintf(stderr, "Error at line %d: Variable '%s' is undeclared\n", 
                yylineno, ident);
        exit(EXIT_FAILURE);
    }
}

void check_variable_type(const char* ident, VarType expected) {
    VarType actual = symtab_lookup(ident);
    if (actual != expected) {
        fprintf(stderr, "Type mismatch at line %d:\n"
                "  Variable:  %s\n"
                "  Expected:  %s\n"
                "  Actual:    %s\n",
                yylineno, ident, 
                var_type_to_str(expected),
                var_type_to_str(actual));
        exit(EXIT_FAILURE);
    }
}
%}

%union {
    int int_val;
    char* str_val;
    char bool_val;
    ASTNode* ast_node;
}

%token T_IF T_THEN T_ELSE T_ELIF T_FOR T_TO T_DO
%token T_PRINT T_READINT T_READSTR T_BEGIN T_END T_SUBSTRING
%token T_LENGTH T_POSITION T_CONCATENATE T_STRING_TYPE T_INTEGER_TYPE
%token T_AND T_OR T_NOT
%token T_ASSIGN T_STR_EQ T_STR_NE T_NE T_LE T_GE T_LT T_GT T_EQ
%token T_ADD T_SUB T_MUL T_DIV T_MOD
%token <int_val> T_NUM
%token <str_val> T_STRING_LIT T_IDENT
%token <bool_val> T_TRUE T_FALSE
%token T_BREAK T_CONTINUE T_EXIT

%type <ast_node> program declarations declaration instr simple_instr
%type <ast_node> num_expr t_num_expr f_num_expr
%type <ast_node> str_expr 
%type <ast_node> bool_expr t_bool_expr f_bool_expr
%type <ast_node> if_stat if_options for_stat assign_stat output_stat
%type <str_val> num_rel str_rel
%type <ast_node> generic_expr

%nonassoc THEN
%nonassoc ELSE
%right T_ASSIGN
%left T_OR
%left T_AND
%left T_EQ T_NE T_LT T_GT T_LE T_GE
%left T_ADD T_SUB
%left T_MUL T_DIV T_MOD
%right T_NOT UMINUS

%%

program:
    declarations instr { root = ast_new_program($1, $2); }
;

declarations:
    { $$ = NULL; }
    | declarations declaration ';' { $$ = ast_add_to_list($1, $2); }
;

declaration:
    T_STRING_TYPE T_IDENT { 
        symtab_add($2, STR_TYPE);
        $$ = ast_new_declaration("string", $2); 
    }
    | T_INTEGER_TYPE T_IDENT { 
        symtab_add($2, INT_TYPE);
        $$ = ast_new_declaration("integer", $2); 
    }
;

instr:
    { $$ = NULL; }
    | instr simple_instr ';' { $$ = ast_add_to_list($1, $2); }
;

simple_instr:
    assign_stat { $$ = $1; }
    | if_stat { $$ = $1; }
    | for_stat { $$ = $1; }
    | T_BEGIN instr T_END { 
        $$ = ast_new_block($2); 
    }
    | output_stat { $$ = $1; }
    | T_BREAK { $$ = ast_new_control_flow(AST_BREAK); }
    | T_CONTINUE { $$ = ast_new_control_flow(AST_CONTINUE); }
    | T_EXIT { $$ = ast_new_control_flow(AST_EXIT); }
;

assign_stat:
    T_IDENT T_ASSIGN num_expr {
        check_variable_type($1, INT_TYPE);
        $$ = ast_new_assignment($1, $3);
    }
    | T_IDENT T_ASSIGN str_expr {
        check_variable_type($1, STR_TYPE);
        $$ = ast_new_assignment($1, $3);
    }
;

if_options:
    { $$ = NULL; }
    | T_ELIF bool_expr T_THEN simple_instr if_options {
        ASTNode* elif = ast_new_elif($2, $4);
        $$ = ast_add_to_list(elif, $5);
    }
    | T_ELSE simple_instr { $$ = ast_new_else($2); }
;

if_stat:
    T_IF bool_expr T_THEN simple_instr if_options {
        ASTNode* current = $5;
        ASTNode* prev_elif = NULL;
        ASTNode* elif_clauses = NULL;
        ASTNode* else_branch = NULL;

        while (current) {
            if (current->type == AST_ELIF_CLAUSE) {
                ASTNode* next = current->next;
                current->next = NULL;
                
                if (!elif_clauses) {
                    elif_clauses = current;
                    prev_elif = current;
                } else {
                    prev_elif->next = current;
                    prev_elif = current;
                }
                
                current = next;
            } else if (current->type == AST_ELSE_CLAUSE) {
                else_branch = current->data.else_clause.else_branch;
                break;
            } else {
                current = current->next;
            }
        }
        $$ = ast_new_if($2, $4, elif_clauses, else_branch);
    }
;

for_stat:
    T_FOR T_IDENT T_ASSIGN num_expr T_TO num_expr T_DO simple_instr {
        check_variable_type($2, INT_TYPE);
        $$ = ast_new_for($2, $4, $6, $8);
    }
;

generic_expr:
    T_IDENT { 
        check_variable_exists($1);
        $$ = ast_new_variable($1); 
    }
    | T_NUM { $$ = ast_new_num_literal($1); }
    | T_STRING_LIT { $$ = ast_new_str_literal($1); }
    | num_expr
    | str_expr

output_stat:
    T_PRINT '(' generic_expr ')' { 
        ASTNode* args[] = {$3};
        $$ = ast_new_function_call("print", args, 1); 
    }
;

num_expr:
    num_expr T_ADD t_num_expr { $$ = ast_new_binary_op("+", $1, $3); }
    | num_expr T_SUB t_num_expr { $$ = ast_new_binary_op("-", $1, $3); }
    | t_num_expr { $$ = $1; }
;

t_num_expr:
    t_num_expr T_MUL f_num_expr { $$ = ast_new_binary_op("*", $1, $3); }
    | t_num_expr T_DIV f_num_expr { $$ = ast_new_binary_op("/", $1, $3); }
    | t_num_expr T_MOD f_num_expr { $$ = ast_new_binary_op("%", $1, $3); }
    | f_num_expr { $$ = $1; }
;

f_num_expr:
    T_NUM { $$ = ast_new_num_literal($1); }
    | T_IDENT { 
        check_variable_type($1, INT_TYPE);
        $$ = ast_new_variable($1); 
    }
    | T_READINT { $$ = ast_new_function_call("readint", NULL, 0); }
    | T_SUB f_num_expr %prec UMINUS { $$ = ast_new_unary_op("-", $2); }
    | '(' num_expr ')' { $$ = $2; }
    | T_LENGTH '(' str_expr ')' { $$ = ast_new_function_call("length", &$3, 1); }
    | T_POSITION '(' str_expr ',' str_expr ')' {
        ASTNode* args[] = {$3, $5};
        $$ = ast_new_function_call("position", args, 2);
    }
;

str_expr:
    T_STRING_LIT { $$ = ast_new_str_literal($1); }
    | T_IDENT { 
        check_variable_type($1, STR_TYPE);
        $$ = ast_new_variable($1); 
    }
    | T_READSTR { $$ = ast_new_function_call("readstr", NULL, 0); }
    | T_CONCATENATE '(' str_expr ',' str_expr ')' {
        ASTNode* args[] = {$3, $5};
        $$ = ast_new_function_call("concatenate", args, 2);
    }
    | T_SUBSTRING '(' str_expr ',' num_expr ',' num_expr ')' {
        ASTNode* args[] = {$3, $5, $7};
        $$ = ast_new_function_call("substring", args, 3);
    }
;

bool_expr:
    bool_expr T_OR t_bool_expr { $$ = ast_new_binary_op("or", $1, $3); }
    | t_bool_expr { $$ = $1; }
;

t_bool_expr:
    t_bool_expr T_AND f_bool_expr { $$ = ast_new_binary_op("and", $1, $3); }
    | f_bool_expr { $$ = $1; }
;

f_bool_expr:
    T_TRUE { $$ = ast_new_bool_literal(1); }
    | T_FALSE { $$ = ast_new_bool_literal(0); }
    | '(' bool_expr ')' { $$ = $2; }
    | T_NOT f_bool_expr { $$ = ast_new_unary_op("not", $2); }
    | num_expr num_rel num_expr { $$ = ast_new_binary_op($2, $1, $3); }
    | str_expr str_rel str_expr { $$ = ast_new_binary_op($2, $1, $3); }
;

num_rel:
    T_EQ { $$ = "="; }
    | T_LT { $$ = "<"; }
    | T_LE { $$ = "<="; }
    | T_GT { $$ = ">"; }
    | T_GE { $$ = ">="; }
    | T_NE { $$ = "<>"; }
;

str_rel:
    T_STR_EQ { $$ = "=="; }
    | T_STR_NE { $$ = "!="; }
;

%%

void yyerror(const char* msg) {
    fprintf(stderr, "Parser error at line %d: %s\n", yylineno, msg);
    exit(EXIT_FAILURE);
}