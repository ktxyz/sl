#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode* ast_new_program(ASTNode* decls, ASTNode* stmts) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_PROGRAM;
    node->data.program.declarations = decls;
    node->data.program.statements = stmts;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_declaration(const char* type, const char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_DECLARATION;
    node->var_type = strdup(type);
    node->var_name = strdup(name);
    node->next = NULL;
    return node;
}

ASTNode* ast_new_assignment(const char* name, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->var_name = strdup(name);
    node->data.binary_op.left = NULL;
    node->data.binary_op.right = value;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_if(ASTNode* cond, ASTNode* then_branch, ASTNode* elif_clauses, ASTNode* else_branch) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IF_STATEMENT;
    node->data.if_stmt.condition = cond;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.elif_clauses = elif_clauses;
    node->data.if_stmt.else_branch = else_branch;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_elif(ASTNode* cond, ASTNode* then_branch) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ELIF_CLAUSE;
    node->data.elif_clause.condition = cond;
    node->data.elif_clause.then_branch = then_branch;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_else(ASTNode* else_branch) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ELSE_CLAUSE;
    node->data.else_clause.else_branch = else_branch;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_for(const char* var, ASTNode* start, ASTNode* end, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FOR_LOOP;
    node->var_name = strdup(var);
    node->data.for_loop.start = start;
    node->data.for_loop.end = end;
    node->data.for_loop.body = body;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_binary_op(char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BINARY_OP;
    node->op = strdup(op);
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_unary_op(char* op, ASTNode* operand) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_UNARY_OP;
    node->op = strdup(op);
    node->data.binary_op.left = operand;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_variable(const char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_VARIABLE;
    node->var_name = strdup(name);
    node->next = NULL;
    return node;
}

ASTNode* ast_new_num_literal(int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NUM_LITERAL;
    node->data.num_value = value;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_str_literal(const char* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_STR_LITERAL;
    node->data.str_value = strdup(value);
    node->next = NULL;
    return node;
}

ASTNode* ast_new_bool_literal(char value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BOOL_LITERAL;
    node->data.bool_value = value;
    node->next = NULL;
    return node;
}

ASTNode* ast_add_to_list(ASTNode* list, ASTNode* node) {
    if (!list) return node;
    ASTNode* current = list;
    while (current->next) current = current->next;
    current->next = node;
    return list;
}


ASTNode* ast_new_function_call(char* name, ASTNode** args, int count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FUNCTION_CALL;
    node->data.function_call.func_name = strdup(name);
    node->data.function_call.args = malloc(count * sizeof(ASTNode*));
    memcpy(node->data.function_call.args, args, count * sizeof(ASTNode*));
    node->data.function_call.arg_count = count;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_block(ASTNode* statements) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BLOCK;
    node->data.block_statements = statements;
    node->next = NULL;
    return node;
}

ASTNode* ast_new_control_flow(ASTNodeType type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->next = NULL;
    return node;
}

void ast_free(ASTNode* node) {
    if (!node) return;
    
    if (node->next) ast_free(node->next);

    switch (node->type) {
        case AST_PROGRAM:
            ast_free(node->data.program.declarations);
            ast_free(node->data.program.statements);
            break;
            
        case AST_DECLARATION:
            free(node->var_type);
            free(node->var_name);
            break;
            
        case AST_ASSIGNMENT:
            free(node->var_name);
            ast_free(node->data.binary_op.right);
            break;
            
        case AST_IF_STATEMENT:
            ast_free(node->data.if_stmt.condition);
            ast_free(node->data.if_stmt.then_branch);
            ast_free(node->data.if_stmt.elif_clauses);
            ast_free(node->data.if_stmt.else_branch);
            break;
            
        case AST_ELIF_CLAUSE:
            ast_free(node->data.elif_clause.condition);
            ast_free(node->data.elif_clause.then_branch);
            break;
            
        case AST_FOR_LOOP:
            free(node->var_name);
            ast_free(node->data.for_loop.start);
            ast_free(node->data.for_loop.end);
            ast_free(node->data.for_loop.body);
            break;
            
        case AST_BINARY_OP:
            free(node->op);
            ast_free(node->data.binary_op.left);
            ast_free(node->data.binary_op.right);
            break;
            
        case AST_UNARY_OP:
            free(node->op);
            ast_free(node->data.binary_op.left);
            break;
            
        case AST_VARIABLE:
            free(node->var_name);
            break;
            
        case AST_STR_LITERAL:
            free(node->data.str_value);
            break;
            
        case AST_NUM_LITERAL:
        case AST_BOOL_LITERAL:
            break;
            
        case AST_FUNCTION_CALL:
            free(node->data.function_call.func_name);
            for (int i = 0; i < node->data.function_call.arg_count; i++) {
                ast_free(node->data.function_call.args[i]);
            }
            free(node->data.function_call.args);
            break;
            
        case AST_BLOCK:
            ast_free(node->data.block_statements);
            break;
            
        case AST_BREAK:
        case AST_CONTINUE:
        case AST_EXIT:
            break;
            
        default:
            fprintf(stderr, "Warning: Unknown AST node type during free (%s)\n", 
                    ast_node_type_to_str(node->type));
            break;
    }
    
    free(node);
}

void ast_print(ASTNode* node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case AST_PROGRAM: {
            printf("Program:\n");
            
            ASTNode* decl = node->data.program.declarations;
            printf("%*sDeclarations:\n", (indent+1)*2, "");
            while (decl) {
                ast_print(decl, indent + 2);
                decl = decl->next;
            }
            
            ASTNode* stmt = node->data.program.statements;
            printf("%*sStatements:\n", (indent+1)*2, "");
            while (stmt) {
                ast_print(stmt, indent + 2);
                stmt = stmt->next;
            }
            break;
        }
            
        case AST_DECLARATION:
            printf("Declaration: %s %s\n", node->var_type, node->var_name);
            break;
            
        case AST_ASSIGNMENT:
            printf("Assignment to %s:\n", node->var_name);
            ast_print(node->data.binary_op.right, indent + 1);
            break;
            
        case AST_IF_STATEMENT: {
            printf("If Statement:\n");
            ast_print(node->data.if_stmt.condition, indent + 1);
            printf("%*sThen:\n", indent*2, "");
            ast_print(node->data.if_stmt.then_branch, indent + 1);

            ASTNode* elif = node->data.if_stmt.elif_clauses;
            while (elif) {
                ast_print(elif, indent);
                elif = elif->next;
            }

            if (node->data.if_stmt.else_branch) {
                printf("%*sElse:\n", indent*2, "");
                ast_print(node->data.if_stmt.else_branch, indent + 1);
            }
            break;
        }
            
        case AST_ELIF_CLAUSE: {
            printf("Elif Clause:\n");
            ast_print(node->data.elif_clause.condition, indent + 1);
            printf("%*sThen:\n", indent*2, "");
            ast_print(node->data.elif_clause.then_branch, indent + 1);
            break;
        }
            
        case AST_FOR_LOOP: {
            printf("For Loop (variable: %s):\n", node->var_name);
            printf("%*sStart:\n", indent*2, "");
            ast_print(node->data.for_loop.start, indent + 1);
            printf("%*sEnd:\n", indent*2, "");
            ast_print(node->data.for_loop.end, indent + 1);
            printf("%*sBody:\n", indent*2, "");
            ast_print(node->data.for_loop.body, indent + 1);
            break;
        }
            
        case AST_BINARY_OP:
            printf("Binary Operation (%s):\n", node->op);
            ast_print(node->data.binary_op.left, indent + 1);
            ast_print(node->data.binary_op.right, indent + 1);
            break;
            
        case AST_UNARY_OP:
            printf("Unary Operation (%s):\n", node->op);
            ast_print(node->data.binary_op.left, indent + 1);
            break;
            
        case AST_VARIABLE:
            printf("Variable: %s\n", node->var_name);
            break;
            
        case AST_NUM_LITERAL:
            printf("Number Literal: %d\n", node->data.num_value);
            break;
            
        case AST_STR_LITERAL:
            printf("String Literal: \"%s\"\n", node->data.str_value);
            break;
            
        case AST_BOOL_LITERAL:
            printf("Boolean Literal: %s\n", node->data.bool_value ? "true" : "false");
            break;
            
        case AST_FUNCTION_CALL:
            printf("Function Call: %s\n", node->data.function_call.func_name);
            for (int i = 0; i < node->data.function_call.arg_count; i++) {
                ast_print(node->data.function_call.args[i], indent + 1);
            }
            break;
            
        case AST_BLOCK: {
            printf("Block:\n");
            ASTNode* stmt = node->data.block_statements;
            while (stmt) {
                ast_print(stmt, indent + 1);
                stmt = stmt->next;
            }
            break;
        }
            
        case AST_BREAK:
            printf("Break Statement\n");
            break;
            
        case AST_CONTINUE:
            printf("Continue Statement\n");
            break;
            
        case AST_EXIT:
            printf("Exit Statement\n");
            break;
            
        default:
            printf("Unknown Node Type: %s\n", ast_node_type_to_str(node->type));
            break;
    }
}

const char* ast_node_type_to_str(ASTNodeType type) {
    switch(type) {
        case AST_PROGRAM:        return "PROGRAM";
        case AST_DECLARATIONS:   return "DECLARATIONS";
        case AST_DECLARATION:    return "DECLARATION";
        case AST_STATEMENT_LIST: return "STATEMENT_LIST";
        case AST_PRINT_STATEMENT:return "PRINT_STATEMENT";
        case AST_ASSIGNMENT:     return "ASSIGNMENT";
        case AST_IF_STATEMENT:   return "IF_STATEMENT";
        case AST_ELIF_CLAUSE:    return "ELIF_CLAUSE";
        case AST_ELSE_CLAUSE:    return "ELSE_CLAUSE";
        case AST_FOR_LOOP:       return "FOR_LOOP";
        case AST_BINARY_OP:      return "BINARY_OP";
        case AST_UNARY_OP:       return "UNARY_OP";
        case AST_VARIABLE:       return "VARIABLE";
        case AST_NUM_LITERAL:    return "NUM_LITERAL";
        case AST_STR_LITERAL:    return "STR_LITERAL";
        case AST_BOOL_LITERAL:   return "BOOL_LITERAL";
        case AST_FUNCTION_CALL:  return "FUNCTION_CALL";
        case AST_STRING_TYPE:    return "STRING_TYPE";
        case AST_INTEGER_TYPE:   return "INTEGER_TYPE";
        case AST_BREAK:          return "BREAK";
        case AST_CONTINUE:       return "CONTINUE";
        case AST_EXIT:           return "EXIT";
        case AST_BLOCK:          return "BLOCK";
        default:                 return "UNKNOWN";
    }
}
