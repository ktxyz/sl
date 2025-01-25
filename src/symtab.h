#ifndef SYMTAB_H
#define SYMTAB_H

typedef enum { INT_TYPE, STR_TYPE, UNDEFINED } VarType;

typedef struct Symbol {
    char* name;
    VarType type;
    struct Symbol* next;
} Symbol;

typedef struct SymTab {
    Symbol* head;
} SymTab;

void symtab_init();
void symtab_add(const char* name, VarType type);
VarType symtab_lookup(const char* name);
void symtab_destroy();

#endif