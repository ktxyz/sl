#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Environment {
    char* name;
    RuntimeVal value;
    struct Environment* next;
} Environment;

static Environment* runtime_env = NULL;
static bool should_exit = false;
static int loop_depth = 0;

void env_insert(const char* name, RuntimeVal value) {
    Environment* new_env = malloc(sizeof(Environment));
    new_env->name = strdup(name);
    new_env->value = value;
    new_env->next = runtime_env;
    runtime_env = new_env;
}

RuntimeVal env_lookup(const char* name) {
    for (Environment* e = runtime_env; e != NULL; e = e->next) {
        if (strcmp(e->name, name) == 0) {
            RuntimeVal copy = e->value;
            if (copy.type == STR_VAL && copy.str_val) {
                copy.str_val = strdup(copy.str_val);
            }
            return copy;
        }
    }
    fprintf(stderr, "Undefined variable: %s\n", name);
    exit(EXIT_FAILURE);
}

void env_update(const char* name, RuntimeVal value) {
    for (Environment* e = runtime_env; e != NULL; e = e->next) {
        if (strcmp(e->name, name) == 0) {
            runtime_val_free(&e->value);
            e->value = value;
            return;
        }
    }
    env_insert(name, value);
}

void runtime_val_free(RuntimeVal* val) {
    if (val->type == STR_VAL && val->str_val) {
        free(val->str_val);
    }
}

RuntimeVal eval_condition(ASTNode* node) {
    RuntimeVal cond = eval(node);
    if (cond.type != BOOL_VAL) {
        fprintf(stderr, "Runtime error: Condition must be boolean instead of %d\n", cond.type);
        exit(EXIT_FAILURE);
    }
    return cond;
}

RuntimeVal eval_binary_op(ASTNode* node) {
    RuntimeVal left = eval(node->data.binary_op.left);
    RuntimeVal right = eval(node->data.binary_op.right);
    RuntimeVal result = {0};

    if (strcmp(node->op, "+") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            result.type = INT_VAL;
            result.int_val = left.int_val + right.int_val;
        } else if (left.type == STR_VAL && right.type == STR_VAL) {
            result.type = STR_VAL;
            result.str_val = malloc(strlen(left.str_val) + strlen(right.str_val) + 1);
            strcpy(result.str_val, left.str_val);
            strcat(result.str_val, right.str_val);
        }
    }
    else if (strcmp(node->op, "-") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            result.type = INT_VAL;
            result.int_val = left.int_val - right.int_val;
        } else {
            fprintf(stderr, "Runtime error: - operator requires integers\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "*") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            result.type = INT_VAL;
            result.int_val = left.int_val * right.int_val;
        } else {
            fprintf(stderr, "Runtime error: * operator requires integers\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "/") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            if (right.int_val == 0) {
                fprintf(stderr, "Runtime error: Division by zero\n");
                exit(EXIT_FAILURE);
            }
            result.type = INT_VAL;
            result.int_val = left.int_val / right.int_val;
        } else {
            fprintf(stderr, "Runtime error: / operator requires integers\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "%") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            if (right.int_val == 0) {
                fprintf(stderr, "Runtime error: Modulo by zero\n");
                exit(EXIT_FAILURE);
            }
            result.type = INT_VAL;
            result.int_val = left.int_val % right.int_val;
        } else {
            fprintf(stderr, "Runtime error: %% operator requires integers\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "<") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = left.int_val < right.int_val;
        } else {
            fprintf(stderr, "Runtime error: < operator requires integers\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "<=") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = left.int_val <= right.int_val;
        } else {
            fprintf(stderr, "Runtime error: <= operator requires integers\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, ">") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = left.int_val > right.int_val;
        } else {
            fprintf(stderr, "Runtime error: > operator requires integers\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, ">=") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = left.int_val >= right.int_val;
        } else {
            fprintf(stderr, "Runtime error: >= operator requires integers\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "=") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = left.int_val == right.int_val;
        } else {
            fprintf(stderr, "Runtime error: = operator requires integers\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "<>") == 0) {
        if (left.type == INT_VAL && right.type == INT_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = left.int_val != right.int_val;
        } else {
            fprintf(stderr, "Runtime error: <> operator requires integers\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "==") == 0) {
        if (left.type == STR_VAL && right.type == STR_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = strcmp(left.str_val, right.str_val) == 0;
        } else {
            fprintf(stderr, "Runtime error: == operator requires strings\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "!=") == 0) {
        if (left.type == STR_VAL && right.type == STR_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = strcmp(left.str_val, right.str_val) != 0;
        } else {
            fprintf(stderr, "Runtime error: != operator requires strings\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "and") == 0) {
        if (left.type == BOOL_VAL && right.type == BOOL_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = left.bool_val && right.bool_val;
        } else {
            fprintf(stderr, "Runtime error: and operator requires booleans\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "or") == 0) {
        if (left.type == BOOL_VAL && right.type == BOOL_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = left.bool_val || right.bool_val;
        } else {
            fprintf(stderr, "Runtime error: or operator requires booleans\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Runtime error: Unknown operator %s\n", node->op);
        exit(EXIT_FAILURE);
    }

    return result;
}

RuntimeVal eval_unary_op(ASTNode* node) {
    RuntimeVal operand = eval(node->data.binary_op.left);
    RuntimeVal result = {0};

    if (strcmp(node->op, "-") == 0) {
        if (operand.type == INT_VAL) {
            result.type = INT_VAL;
            result.int_val = -operand.int_val;
        } else {
            fprintf(stderr, "Runtime error: - operator expects integer\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(node->op, "not") == 0) {
        if (operand.type == BOOL_VAL) {
            result.type = BOOL_VAL;
            result.bool_val = !operand.bool_val;
        } else {
            fprintf(stderr, "Runtime error: not operator expects boolean\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        fprintf(stderr, "Runtime error: Unknown unary operator %s\n", node->op);
        exit(EXIT_FAILURE);
    }
    
    return result;
}

RuntimeVal eval(ASTNode* node) {
    if (!node || should_exit) return (RuntimeVal){0};

    RuntimeVal result = {0};

    switch (node->type) {
        case AST_PROGRAM:
            for (ASTNode* decl = node->data.program.declarations; decl; decl = decl->next) {
                eval(decl);
            }
            for (ASTNode* stmt = node->data.program.statements; stmt; stmt = stmt->next) {
                RuntimeVal tmp = eval(stmt);
                if (tmp.type == CTRL_FLOW && tmp.cf == CF_EXIT) {
                    should_exit = true;
                    break;
                }
            }
            break;

        case AST_DECLARATIONS:
            for (ASTNode* d = node; d; d = d->next) {
                eval(d);
            }
            break;

        case AST_STATEMENT_LIST:
            for (ASTNode* stmt = node; stmt; stmt = stmt->next) {
                RuntimeVal tmp = eval(stmt);
                if (tmp.type == CTRL_FLOW) {
                    result = tmp;
                    break;
                }
            }
            break;

        case AST_BOOL_LITERAL:
            result.type = BOOL_VAL;
            result.bool_val = node->data.bool_value;
            break;

        case AST_BLOCK:
            for (ASTNode* stmt = node->data.block_statements; stmt; stmt = stmt->next) {
                result = eval(stmt);
                if (result.type == CTRL_FLOW) break;
            }
            break;

        case AST_NUM_LITERAL:
            result.type = INT_VAL;
            result.int_val = node->data.num_value;
            break;

        case AST_STR_LITERAL:
            result.type = STR_VAL;
            result.str_val = strdup(node->data.str_value);
            break;

        case AST_VARIABLE:
            return env_lookup(node->var_name);

        case AST_DECLARATION: {
            RuntimeVal init_val = {0};
            if (strcmp(node->var_type, "int") == 0) {
                init_val.type = INT_VAL;
                init_val.int_val = 0;
            } else if (strcmp(node->var_type, "string") == 0) {
                init_val.type = STR_VAL;
                init_val.str_val = strdup("");
            } else if (strcmp(node->var_type, "bool") == 0) {
                init_val.type = BOOL_VAL;
                init_val.bool_val = false;
            }
            env_insert(node->var_name, init_val);
            break;
        }

        case AST_ASSIGNMENT: {
            RuntimeVal rhs = eval(node->data.binary_op.right);
            env_update(node->var_name, rhs);
            result = rhs;
            break;
        }

        case AST_BINARY_OP:
            return eval_binary_op(node);

        case AST_UNARY_OP:
            return eval_unary_op(node);

        case AST_IF_STATEMENT: {
            RuntimeVal cond = eval_condition(node->data.if_stmt.condition);
            bool matched = false;

            if (cond.bool_val) {
                result = eval(node->data.if_stmt.then_branch);
                matched = true;
            } else {
                ASTNode* current_elif = node->data.if_stmt.elif_clauses;
                while (current_elif && !matched) {
                    RuntimeVal elif_cond = eval_condition(current_elif->data.elif_clause.condition);
                    if (elif_cond.bool_val) {
                        result = eval(current_elif->data.elif_clause.then_branch);
                        matched = true;
                    }
                    current_elif = current_elif->next;
                }
            }

            if (!matched && node->data.if_stmt.else_branch) {
                result = eval(node->data.if_stmt.else_branch);
            }
            break;
        }

        case AST_FOR_LOOP: {
            RuntimeVal start = eval(node->data.for_loop.start);
            RuntimeVal end = eval(node->data.for_loop.end);
            
            if (start.type != INT_VAL || end.type != INT_VAL) {
                fprintf(stderr, "For loop bounds must be integers\n");
                exit(EXIT_FAILURE);
            }

            bool has_original = false;
            RuntimeVal original;
            for (Environment* e = runtime_env; e != NULL; e = e->next) {
                if (strcmp(e->name, node->var_name) == 0) {
                    original = e->value;
                    has_original = true;
                    break;
                }
            }
            loop_depth++;
            for (int i = start.int_val; i <= end.int_val; i++) {
                if (should_exit) break;
                
                RuntimeVal current = {.type = INT_VAL, .int_val = i};
                env_update(node->var_name, current);
                
                RuntimeVal body_result = eval(node->data.for_loop.body);
                if (body_result.type == CTRL_FLOW) {
                    if (body_result.cf == CF_BREAK) break;
                    if (body_result.cf == CF_EXIT) should_exit = true;
                }
            }
            loop_depth--;
            if (has_original) {
                env_update(node->var_name, original);
            }
            break;
        }

        case AST_BREAK:
            result.type = CTRL_FLOW;
            result.cf = CF_BREAK;
            if (loop_depth == 0) {
                fprintf(stderr, "Runtime error: %s outside loop\n", 
                        node->type == AST_BREAK ? "break" : "continue");
                exit(EXIT_FAILURE);
            }
            break;

        case AST_CONTINUE:
            result.type = CTRL_FLOW;
            result.cf = CF_CONTINUE;
            if (loop_depth == 0) {
                fprintf(stderr, "Runtime error: %s outside loop\n", 
                        node->type == AST_BREAK ? "break" : "continue");
                exit(EXIT_FAILURE);
            }
            break;

        case AST_EXIT:
            result.type = CTRL_FLOW;
            result.cf = CF_EXIT;
            should_exit = true;
            break;

        case AST_FUNCTION_CALL:
            if (strcmp(node->data.function_call.func_name, "print") == 0) {
                for (int i = 0; i < node->data.function_call.arg_count; i++) {
                    RuntimeVal arg = eval(node->data.function_call.args[i]);
                    switch (arg.type) {
                        case INT_VAL: printf("%d", arg.int_val); break;
                        case STR_VAL: printf("%s", arg.str_val); break;
                        case BOOL_VAL: printf(arg.bool_val ? "true" : "false"); break;
                        default: break;
                    }
                }
                printf("\n");
            }
            else if (strcmp(node->data.function_call.func_name, "concatenate") == 0) {
                if (node->data.function_call.arg_count != 2) {
                    fprintf(stderr, "concatenate() requires exactly 2 arguments\n");
                    exit(EXIT_FAILURE);
                }
                
                RuntimeVal arg1 = eval(node->data.function_call.args[0]);
                RuntimeVal arg2 = eval(node->data.function_call.args[1]);
                
                if (arg1.type != STR_VAL || arg2.type != STR_VAL) {
                    fprintf(stderr, "concatenate() requires two strings\n");
                    exit(EXIT_FAILURE);
                }
                
                result.type = STR_VAL;
                result.str_val = malloc(strlen(arg1.str_val) + strlen(arg2.str_val) + 1);
                strcpy(result.str_val, arg1.str_val);
                strcat(result.str_val, arg2.str_val);
            } else if (strcmp(node->data.function_call.func_name, "position") == 0) {
                result.type = INT_VAL;

                RuntimeVal arg1 = eval(node->data.function_call.args[0]);
                RuntimeVal arg2 = eval(node->data.function_call.args[1]);
                
                if (arg1.type != STR_VAL || arg2.type != STR_VAL) {
                    fprintf(stderr, "concatenate() requires two strings\n");
                    exit(EXIT_FAILURE);
                }

                if (arg2.str_val == NULL || strlen(arg2.str_val) == 0) {
                    result.int_val = 0;
                } else {
                    char* found = strstr(arg1.str_val, arg2.str_val);
                    if (found == NULL)
                        result.int_val = 0;
                    else
                        result.int_val = (found - arg1.str_val) + 1;
                }
            } else if (strcmp(node->data.function_call.func_name, "length") == 0) {
                result.type = INT_VAL;
                
                RuntimeVal arg = eval(node->data.function_call.args[0]);

                if (arg.type != STR_VAL) {
                    fprintf(stderr, "length() requires string\n");
                    exit(EXIT_FAILURE);
                }

                result.int_val = strlen(arg.str_val);
            } else if (strcmp(node->data.function_call.func_name, "substring") == 0) {
                result.type = STR_VAL;
                
                RuntimeVal arg1 = eval(node->data.function_call.args[0]);
                RuntimeVal arg2 = eval(node->data.function_call.args[1]);
                RuntimeVal arg3 = eval(node->data.function_call.args[2]);

                if (arg1.type != STR_VAL || arg2.type != INT_VAL || arg3.type != INT_VAL) {
                    fprintf(stderr, "substring() requires string, int pos, and int length\n");
                    exit(EXIT_FAILURE);
                }

                int l = strlen(arg1.str_val);
                if (arg2.int_val < 1 || arg2.int_val > l) {
                    result.str_val = malloc(1);
                    result.str_val[0] = '\0';
                } else {
                    int start = arg2.int_val - 1;
                    int requested_length = arg3.int_val;
                    
                    if (requested_length <= 0) {
                        result.str_val = malloc(1);
                        result.str_val[0] = '\0';
                    } else {
                        int remaining = strlen(arg1.str_val) - start;
                        int actual_length = requested_length < remaining ? requested_length : remaining;
                        
                        result.str_val = malloc(actual_length + 1);
                        if (!result.str_val) {
                            fprintf(stderr, "Memory allocation failed\n");
                            exit(EXIT_FAILURE);
                        }
                        strncpy(result.str_val, arg1.str_val + start, actual_length);
                        result.str_val[actual_length] = '\0';
                    }
                }
            } else if (strcmp(node->data.function_call.func_name, "readint") == 0) {
                int val;
                scanf("%d", &val);
                result.type = INT_VAL;
                result.int_val = val;
            } else if (strcmp(node->data.function_call.func_name, "readstr") == 0) {
                char buffer[1024];
                scanf(" %1023s", buffer);
                result.type = STR_VAL;
                result.str_val = strdup(buffer);
            } else {
                fprintf(stderr, "Unsupported function call: %s\n", node->data.function_call.func_name);
                break;
            }
            break;

        default:
            fprintf(stderr, "Unsupported node type: %s\n", ast_node_type_to_str(node->type));
            break;
    }

    return result;
}

void interpret(ASTNode* node) {
    symtab_init();
    runtime_env = NULL;
    should_exit = false;
    
    eval(node);

    while (runtime_env) {
        Environment* next = runtime_env->next;
        runtime_val_free(&runtime_env->value);
        free(runtime_env->name);
        free(runtime_env);
        runtime_env = next;
    }
    
    symtab_destroy();
}