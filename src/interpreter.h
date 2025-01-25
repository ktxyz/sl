#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "symtab.h"

typedef enum {
    INT_VAL,
    STR_VAL,
    BOOL_VAL,
    CTRL_FLOW
} RuntimeValType;

typedef enum {
    CF_BREAK,
    CF_CONTINUE,
    CF_EXIT
} ControlFlow;

typedef struct RuntimeVal {
    RuntimeValType type;
    union {
        int int_val;
        char* str_val;
        char bool_val;
        ControlFlow cf;
    };
} RuntimeVal;

RuntimeVal eval(ASTNode* node);
void interpret(ASTNode* node);
void runtime_val_free(RuntimeVal* val);

#endif