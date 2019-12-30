%option yylineno
%{
#include <stdio.h>
char a;
void imprimir(int linha,char *lexema, char *tipo){
  printf("%d: %s -> %s\n",linha, lexema, tipo);
}
// \{            {while((a = input()) != 125);}
%}

%x commentblock
%x comment
%%

"else"                  {imprimir(yylineno,yytext, "ELSE");}
"if"                    {imprimir(yylineno,yytext, "IF");}
"input"                 {imprimir(yylineno,yytext, "INPUT");}
"int"                   {imprimir(yylineno,yytext, "INT");}
"output"                {imprimir(yylineno,yytext, "OUTPUT");}
"return"                {imprimir(yylineno,yytext, "RETURN");}
"void"                  {imprimir(yylineno,yytext, "VOID");}
"while"                 {imprimir(yylineno,yytext, "WHILE");}
"write"                 {imprimir(yylineno,yytext, "WRITE");}
"+"                     {imprimir(yylineno,yytext, "PLUS");}
"-"                     {imprimir(yylineno,yytext, "MINUS");}
"*"                     {imprimir(yylineno,yytext, "TIMES");}
"/"                     {imprimir(yylineno,yytext, "OVER");}
"<"                     {imprimir(yylineno,yytext, "LT");}
"<="                    {imprimir(yylineno,yytext, "LE");}
">"                     {imprimir(yylineno,yytext, "GT");}
">="                    {imprimir(yylineno,yytext, "GE");}
"=="                    {imprimir(yylineno,yytext, "EQ");}
"!="                    {imprimir(yylineno,yytext, "NEQ");}
"="                     {imprimir(yylineno,yytext, "ASSIGN");}
";"                     {imprimir(yylineno,yytext, "SEMI");}
","                     {imprimir(yylineno,yytext, "COMMA");}
"("                     {imprimir(yylineno,yytext, "LPAREN");}
")"                     {imprimir(yylineno,yytext, "RPAREN");}
"["                     {imprimir(yylineno,yytext, "LBRACK");}
"]"                     {imprimir(yylineno,yytext, "RBRACK");}
"{"                     {imprimir(yylineno,yytext, "LBRACE");}
"}"                     {imprimir(yylineno,yytext, "RBRACE");}
[0-9]+                  {imprimir(yylineno,yytext, "NUM");}
[a-zA-Z][a-zA-Z0-9]*    {imprimir(yylineno,yytext, "ID");}
"\"".*"\""          {imprimir(yylineno,yytext, "STRING");}
"//"                    BEGIN(comment); //Começo de um comentário de 1 linha
<comment>[^\n]*         //Lê todos os caracteres sem executar nenhuma outra ação
<comment>"\n"           BEGIN(INITIAL); //Final do comentário de uma linha
"/*"                    BEGIN(commentblock); //Começo do comentário de bloco
<commentblock>.         //Lê todos os caracteres sem executar nenhuma outra ação
<commentblock>"\n"      //Foi necessário para não imprimir linhas em comentários extensos
<commentblock>"*/"      BEGIN(INITIAL); //Final do comentário de bloco
[ \n]*                  //Lê todos os espaços e \n sem executar nenhuma outra ação
.                       {imprimir(yylineno,yytext, "UNKNOWN");} //Para qualquer lexema diferente dos descritos acima, imprime desconhecido

%%
int main() {
yylex();
return 0; }
