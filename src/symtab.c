#include "symtab.h"
#include <stdlib.h>
#include <string.h>

static SymTab symbol_table;

void symtab_init() {
    symbol_table.head = NULL;
}

void symtab_add(const char* name, VarType type) {
    Symbol* new_symbol = malloc(sizeof(Symbol));
    new_symbol->name = strdup(name);
    new_symbol->type = type;
    new_symbol->next = symbol_table.head;
    symbol_table.head = new_symbol;
}

VarType symtab_lookup(const char* name) {
    Symbol* current = symbol_table.head;
    while (current) {
        if (strcmp(current->name, name) == 0)
            return current->type;
        current = current->next;
    }
    return UNDEFINED;
}

void symtab_destroy() {
    Symbol* current = symbol_table.head;
    while (current) {
        Symbol* next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    symbol_table.head = NULL;
}