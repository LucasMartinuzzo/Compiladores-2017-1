%option outfile="scanner.c"
%option noyywrap
%option nounput
%option noinput
%option yylineno

%{
#include <stdio.h>
#include "ast.h"
#include "parser.h"
#include "tables.h"

#define process_token(type) return type

extern LitTable *lt;
extern char yytext_ant[10][SYMBOL_MAX_SIZE];
int yylval_int;

extern void ypush(char yytext_push[SYMBOL_MAX_SIZE]);

extern char* ypop();

void imprimir(int linha,char *lexema, char *tipo){
  printf("%d: %s -> %s\n",linha, lexema, tipo);
}

%}
%x matchstr
%x commentblock
%x comment
%%

"else"                  {process_token(ELSE);}
"if"                    {process_token(IF);}
"input"                 {process_token(INPUT);}
"int"                   {process_token(INT);}
"output"                {process_token(OUTPUT);}
"return"                {process_token(RETURN);}
"void"                  {process_token(VOID);}
"while"                 {process_token(WHILE);}
"write"                 {process_token(WRITE);}
"+"                     {process_token(PLUS);}
"-"                     {process_token(MINUS);}
"*"                     {process_token(TIMES);}
"/"                     {process_token(OVER);}
"<"                     {process_token(LT);}
"<="                    {process_token(LE);}
">"                     {process_token(GT);}
">="                    {process_token(GE);}
"=="                    {process_token(EQ);}
"!="                    {process_token(NEQ);}
"="                     {process_token(ASSIGN);}
";"                     {process_token(SEMI);}
","                     {process_token(COMMA);}
"("                     {process_token(LPAREN);}
")"                     {process_token(RPAREN);}
"["                     {process_token(LBRACK);}
"]"                     {process_token(RBRACK);}
"{"                     {process_token(LBRACE);}
"}"                     {process_token(RBRACE);}
[0-9]+                  {yylval_int = atoi(yytext); process_token(NUM);}
[a-zA-Z][a-zA-Z0-9]*    {ypush(yytext); process_token(ID);}
\"                      BEGIN(matchstr);
<matchstr>[^\"]*        {yylval_int = add_literal(lt,yytext); process_token(STRING);}
<matchstr>\"            BEGIN(INITIAL);
"//"                    BEGIN(comment); //Começo de um comentário de 1 linha
<comment>[^\n]*         //Lê todos os caracteres sem executar nenhuma outra ação
<comment>"\n"           BEGIN(INITIAL); //Final do comentário de uma linha
"/*"                    BEGIN(commentblock); //Começo do comentário de bloco
<commentblock>.         //Lê todos os caracteres sem executar nenhuma outra ação
<commentblock>"\n"      //Foi necessário para não imprimir linhas em comentários extensos
<commentblock>"*/"      BEGIN(INITIAL); //Final do comentário de bloco
[ \n\t]*                  //Lê todos os espaços e \n sem executar nenhuma outra ação
.                       {printf("SCANNING ERROR (%d): Unknown symbol %s\n", yylineno, yytext);
                        exit(1);} //Para qualquer lexema diferente dos descritos acima, imprime mensagem de erro e finaliza o programa

%%
