%{
#include "ast.h"
#include "sl.tab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char* strip_quotes(const char* text) {
    int len = strlen(text);
    if (len >= 2 && text[0] == '"' && text[len-1] == '"') {
        char* result = malloc(len-1);
        strncpy(result, text+1, len-2);
        result[len-2] = '\0';
        return result;
    }
    char* result = malloc(len+1);
    strcpy(result, text);
    return result;
}

%}

%option noyywrap
%option yylineno

WHITESPACE  [ \t\r]
DIGIT       [0-9]
LETTER      [a-zA-Z_]
IDENT       {LETTER}({LETTER}|{DIGIT})*

%%

{WHITESPACE}+   { }
\n              { }
"//"[^\n]*      { }

"if"            { return T_IF; }
"then"          { return T_THEN; }
"else"          { return T_ELSE; }
"elif"          { return T_ELIF; }
"for"           { return T_FOR; }
"to"            { return T_TO; }
"do"            { return T_DO; }
"break"         { return T_BREAK; }
"continue"      { return T_CONTINUE; }
"print"         { return T_PRINT; }
"readint"       { return T_READINT; }
"readstr"       { return T_READSTR; }
"begin"         { return T_BEGIN; }
"end"           { return T_END; }
"exit"          { return T_EXIT; }
"substring"     { return T_SUBSTRING; }
"length"        { return T_LENGTH; }
"position"      { return T_POSITION; }
"concatenate"   { return T_CONCATENATE; }
"string"        { return T_STRING_TYPE; }
"integer"       { return T_INTEGER_TYPE; }
"and"           { return T_AND; }
"or"            { return T_OR; }
"not"           { return T_NOT; }
"true"          { yylval.bool_val = 1; return T_TRUE; }
"false"         { yylval.bool_val = 0; return T_FALSE; }

":="            { return T_ASSIGN; }
"=="            { return T_STR_EQ; }
"!="            { return T_STR_NE; }
"<>"            { return T_NE; }
"<="            { return T_LE; }
">="            { return T_GE; }
"<"             { return T_LT; }
">"             { return T_GT; }
"="             { return T_EQ; }
"+"             { return T_ADD; }
"-"             { return T_SUB; }
"*"             { return T_MUL; }
"/"             { return T_DIV; }
"%"             { return T_MOD; }

";"             { return ';'; }
","             { return ','; }
"("             { return '('; }
")"             { return ')'; }

{DIGIT}+        {
    yylval.int_val = atoi(yytext);
    return T_NUM;
}

\"(\\.|[^\\"])*\" {
    yylval.str_val = strip_quotes(yytext);
    return T_STRING_LIT;
}

{IDENT}         {
    yylval.str_val = strdup(yytext);
    return T_IDENT;
}

.               {
    fprintf(stderr, "Lexer error at line %d: Invalid character '%c'\n", 
            yylineno, yytext[0]);
    exit(EXIT_FAILURE);
}

%%
