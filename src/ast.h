#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAM,
    AST_DECLARATIONS,
    AST_DECLARATION,
    AST_STATEMENT_LIST,
    AST_PRINT_STATEMENT,
    AST_ASSIGNMENT,
    AST_IF_STATEMENT,
    AST_ELIF_CLAUSE,
    AST_ELSE_CLAUSE,
    AST_FOR_LOOP,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_VARIABLE,
    AST_NUM_LITERAL,
    AST_STR_LITERAL,
    AST_BOOL_LITERAL,
    AST_FUNCTION_CALL,
    AST_STRING_TYPE,
    AST_INTEGER_TYPE,
    AST_BREAK,
    AST_CONTINUE,
    AST_EXIT,
    AST_BLOCK
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    char* var_type;
    char* var_name;
    char* op;
    union {
        struct {
            struct ASTNode* declarations;
            struct ASTNode* statements;
        } program;
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* elif_clauses;
            struct ASTNode* else_branch;
        } if_stmt;
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
        } elif_clause;
        struct {
            struct ASTNode* else_branch;
        } else_clause;
        struct {
            char* loop_var;
            struct ASTNode* start;
            struct ASTNode* end;
            struct ASTNode* body;
        } for_loop;
        struct {
            struct ASTNode* left;
            struct ASTNode* right;
        } binary_op;
        int num_value;
        char* str_value;
        char bool_value;
        struct {
            char* func_name;
            struct ASTNode** args;
            int arg_count;
        } function_call;
        struct ASTNode* block_statements;
    } data;
    struct ASTNode* next;
} ASTNode;

ASTNode* ast_new_program(ASTNode* decls, ASTNode* stmts);
ASTNode* ast_new_declaration(const char* type, const char* name);
ASTNode* ast_new_assignment(const char* name, ASTNode* value);
ASTNode* ast_new_if(ASTNode* cond, ASTNode* then_branch, ASTNode* elif_clauses, ASTNode* else_branch);
ASTNode* ast_new_elif(ASTNode* cond, ASTNode* then_branch);
ASTNode* ast_new_else(ASTNode* else_branch) ;
ASTNode* ast_new_for(const char* var, ASTNode* start, ASTNode* end, ASTNode* body);
ASTNode* ast_new_binary_op(char* op, ASTNode* left, ASTNode* right);
ASTNode* ast_new_unary_op(char* op, ASTNode* operand);
ASTNode* ast_new_variable(const char* name);
ASTNode* ast_new_num_literal(int value);
ASTNode* ast_new_str_literal(const char* value);
ASTNode* ast_new_bool_literal(char value);
ASTNode* ast_new_function_call(char* name, ASTNode** args, int count);
ASTNode* ast_new_block(ASTNode* statements);
ASTNode* ast_add_to_list(ASTNode* list, ASTNode* node);
ASTNode* ast_new_control_flow(ASTNodeType type);

void ast_free(ASTNode* node);
void ast_print(ASTNode* node, int indent);
const char* ast_node_type_to_str(ASTNodeType type);

#endif