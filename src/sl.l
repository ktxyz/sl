%option noyywrap
%option yylineno

%{
#include <stdio.h>
#include "sl.tab.h"

extern int yylineno;
%}


DIGIT      [0-9]
LETTER     [a-zA-Z]
IDENT      {LETTER}({LETTER}|{DIGIT})*
NUM        -?{DIGIT}+
STRING     \"([^\"]|\\\")*\"

%%

"and"              return AND;
"or"               return OR;
"not"              return NOT;
"if"               return IF;
"then"             return THEN;
"else"             return ELSE;
"elif"             return ELIF;
"for"              return FOR;
"to"               return TO;
"do"               return DO;
"break"            return BREAK;
"continue"         return CONTINUE;
"print"            return PRINT;
"readint"          return READINT;
"readstr"          return READSTR;
"begin"            return BEGIN_CTX;
"end"              return END_CTX;
"exit"             return EXIT;
"substring"        return SUBSTRING;
"length"           return LENGTH;
"position"         return POSITION;
"concatenate"      return CONCATENATE;
"string"           return STRING_TYPE;
"integer"          return INTEGER_TYPE;

"+"                return PLUS;
"-"                return MINUS;
"*"                return MULTIPLY;
"/"                return DIVIDE;
"%"                return MODULO;
"="                return EQUAL;
"<"                return LESS;
"<="               return LESS_EQUAL;
">"                return GREATER;
">="               return GREATER_EQUAL;
"<>"               return NOT_EQUAL;
"=="               return STRING_EQUAL;
"!="               return STRING_NOT_EQUAL;
":="               return ASSIGN;
"("                return LPAREN;
")"                return RPAREN;
";"                return SEMICOLON;
","                return COMMA;
{IDENT}            return IDENTIFIER;
{NUM}              return NUMBER;
{STRING}           return STRING_LITERAL;
[ \t\n]+           ;

. {
    fprintf(stderr, "Lexical error: unexpected character %c (0x%x) at line %d\n", yytext[0], yytext[0], yylineno);
    exit(1);
}

%%


