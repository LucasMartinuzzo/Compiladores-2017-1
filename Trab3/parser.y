/* Options to bison */
// File name of generated parser.
%output "parser.c"
// Produces a 'parser.h'
%defines "parser.h"
// Give proper error messages when a syntax error is found.
%define parse.error verbose
// Enable LAC (lookahead correction) to improve syntax error handling.
%define parse.lac full
// Enable the trace option so that debugging is possible.
%define parse.trace

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "tables.h"
#include "ast.h"

int yylex(void);
void yyerror(char const *s);

void check_arity(int func, int n_arg);
int check_func() ;
int check_var(int func);

int new_func(int n_parameters, int return_type);

int new_var(int func, int arr_size);

extern char *yytext;
extern int yylineno;

extern int yylval_int;

LitTable *lt;
SymTable *st;
AST *ast;
char yytext_ant[10][SYMBOL_MAX_SIZE]; //atualizado a cada ID
int yytext_i = 0; //índice para a "pilha" acima, foi necessário para chamada de vetores do tipo x[ID]
int cur_func; //índice da função atual
int func_called; //índice da função usada em alguma chamada de funções
AST *temp_node; //nó temporário, foi necessário para chamada de vetores do tipo x[ID]

//Operação com a pilha
void push(char yytext_push[SYMBOL_MAX_SIZE]){
   strcpy(yytext_ant[yytext_i],yytext_push);
   yytext_i++;
}
//Operação com a pilha
char* pop(){
   yytext_i--;
   return yytext_ant[yytext_i];
}

%}

%define api.value.type {struct node*} //Por algum motivo AST* não funcinou


%token IF ELSE RETURN INT VOID WHILE INPUT OUTPUT WRITE
%token SEMI COMMA LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE
%token ASSIGN

%left NUM
%token ID
%token STRING

%left LT LE GT GE EQ NEQ
// Precedence of operators (* and / have higher precedence).
// All four operators are left associative.
%left PLUS MINUS
%left TIMES OVER

// Start symbol for the grammar.
%start program

%%
program:
   func_decl_list             { ast = $1; }
;

func_decl_list:
   func_decl_list func_decl   { add_child($1, $2); $$ = $1; }
|  func_decl                  { $$ = new_subtree(FUNC_LIST_NODE, 1, $1); }
;

func_decl:
   func_header func_body   { $$ = new_subtree(FUNC_DECL_NODE,2,$1,$2); }
;

func_header:
   ret_type ID {cur_func = new_func(1,get_return_type($1));} LPAREN params RPAREN  { $$ = new_subtree(FUNC_HEADER_NODE,3,$1,new_node(ID_NODE,-1,cur_func),$5);change_n_parameters(st,cur_func,get_child_count($5)); }
;

func_body:
   LBRACE opt_var_decl opt_stmt_list RBRACE  { $$ = new_subtree(FUNC_BODY_NODE,2,$2,$3); }
;

ret_type:
   INT   { $$ = new_subtree(INT_TYPE_NODE,0); }
|  VOID  { $$ = new_subtree(VOID_TYPE_NODE,0); }
;

params:
   VOID                    { $$ = new_subtree(PAR_LIST_NODE, 0); }
|  param_list              { $$ = $1; }
;

param_list:
   param_list COMMA param  { add_child($1, $3); $$ = $1; }
|  param                   { $$ = new_subtree(PAR_LIST_NODE, 1, $1); }
;

param:
   INT ID               {$$ = new_node(S_VAR_NODE,cur_func,new_var(cur_func, 0));}
|  INT ID LBRACK RBRACK {$$ = new_node(C_VAR_NODE,cur_func,new_var(cur_func, -1));}
;

opt_var_decl:
   %empty         { $$ = new_subtree(VAR_LIST_NODE,0); }
|  var_decl_list  { $$ = $1; }
;

var_decl_list:
   var_decl_list var_decl  { add_child($1, $2); $$ = $1; }
|  var_decl                { $$ = new_subtree(VAR_LIST_NODE, 1, $1); }
;

var_decl:
   INT ID SEMI                   {$$ = new_node(S_VAR_NODE,cur_func,new_var(cur_func, 0));}
|  INT ID LBRACK NUM RBRACK SEMI {$$ = new_node(C_VAR_NODE,cur_func,new_var(cur_func, yylval_int)); add_child($$,new_node(NUM_NODE,-1,yylval_int));}
;

opt_stmt_list:
   %empty      { $$ = new_subtree(STMT_SEQ_NODE,0); }
|  stmt_list   { $$ = $1; }
;

stmt_list: stmt_list stmt  { add_child($1, $2); $$ = $1; }
|  stmt                    { $$ = new_subtree(STMT_SEQ_NODE, 1, $1); }
;

stmt:
   assign_stmt    { $$ = $1; }
|  if_stmt        { $$ = $1; }
|  while_stmt     { $$ = $1; }
|  return_stmt    { $$ = $1; }
| func_call SEMI  { $$ = $1; }
;

assign_stmt:
   lval ASSIGN arith_expr SEMI      { $$ = new_subtree(ASSIGN_NODE, 2, $1, $3); }
;

lval:
   ID                   { $$ = new_node(S_VAR_NODE,cur_func,check_var(cur_func)); }
|  ID LBRACK NUM RBRACK { $$ = new_node(C_VAR_NODE,cur_func,check_var(cur_func)); add_child($$,new_node(NUM_NODE,-1,yylval_int)); }
|  ID LBRACK ID RBRACK  { temp_node = new_node(S_VAR_NODE,cur_func,check_var(cur_func)); $$ = new_node(C_VAR_NODE,cur_func,check_var(cur_func)); add_child($$,temp_node); }
;

if_stmt:
   IF LPAREN bool_expr RPAREN block             { $$ = new_subtree(IF_NODE, 2, $3, $5); }
|  IF LPAREN bool_expr RPAREN block ELSE block  { $$ = new_subtree(IF_NODE, 3, $3, $5, $7);}
;

block:
   LBRACE opt_stmt_list RBRACE   { $$ = $2; }
;

while_stmt:
   WHILE LPAREN bool_expr RPAREN block { $$ = new_subtree(WHILE_NODE, 2, $3, $5); }
;

return_stmt:
   RETURN SEMI                { $$ = NULL; }
|  RETURN arith_expr SEMI     { $$ = new_subtree(RETURN_NODE,1,$2); }
;

func_call:
   output_call       { $$ = $1; }
|  write_call        { $$ = $1; }
|  user_func_call    { $$ = $1; }
;

input_call:
   INPUT LPAREN RPAREN  {$$ = new_subtree(INPUT_NODE,0); }//{ scanf("%d",&input);$$ = new_node(NUM_NODE,-1,input); }
;

output_call:
   OUTPUT LPAREN arith_expr RPAREN  { $$ = new_subtree(OUTPUT_NODE, 1, $3); }
;

write_call:
   WRITE LPAREN STRING RPAREN       { $$ = new_subtree(WRITE_NODE, 1, new_node(STRING_NODE,-1,yylval_int)); }
;

user_func_call:
   ID { func_called = check_func(); } LPAREN opt_arg_list RPAREN { check_arity(func_called,get_child_count($4)); $$ = new_node(FCALL_NODE,-1,func_called);add_child($$,$4); }
;

opt_arg_list:
   %empty   { $$ = new_subtree(ARG_LIST_NODE, 0); }
|  arg_list { $$ = $1; }
;

arg_list:
   arg_list COMMA arith_expr { add_child($1, $3); $$ = $1; }
|  arith_expr  { $$ = new_subtree(ARG_LIST_NODE, 1, $1); }
;

bool_expr:
   arith_expr LT arith_expr   { $$ = new_subtree(LT_NODE, 2, $1, $3); }
|   arith_expr LE arith_expr   { $$ = new_subtree(GE_NODE, 2, $1, $3); }
|   arith_expr GT arith_expr   { $$ = new_subtree(GT_NODE, 2, $1, $3); }
|   arith_expr GE arith_expr   { $$ = new_subtree(GE_NODE, 2, $1, $3); }
|   arith_expr EQ arith_expr   { $$ = new_subtree(EQ_NODE, 2, $1, $3); }
|   arith_expr NEQ arith_expr  { $$ = new_subtree(NEQ_NODE, 2, $1, $3); }
;

/*bool_op:
   LT
|  LE
|  GT
|  GE
|  EQ
|  NEQ
;*/

arith_expr:
   arith_expr PLUS arith_expr    { $$ = new_subtree(PLUS_NODE, 2, $1, $3); }
|   arith_expr MINUS arith_expr   { $$ = new_subtree(MINUS_NODE, 2, $1, $3); }
|  arith_expr TIMES arith_expr   { $$ = new_subtree(TIMES_NODE, 2, $1, $3); }
|  arith_expr OVER arith_expr    { $$ = new_subtree(OVER_NODE, 2, $1, $3); }
|  LPAREN arith_expr RPAREN      { $$ = $2; }
|  lval                          { $$ = $1; }
|  input_call                    { $$ = $1; }
|  user_func_call                { $$ = $1; }
|  NUM                           { $$ = new_node(NUM_NODE,-1,yylval_int);}
;

%%

//Verifica a aridade da função chamada
void check_arity(int func,int n_arg){
   if(get_arity(st,func) != n_arg){
      printf("SEMANTIC ERROR (%d): function '%s' was called with %d arguments but declared with %d parameters.\n",
             yylineno, get_func_name(st,func),n_arg,get_arity(st,func));
      exit(1);
   }
}

//Verifica se a função de nome dado por ID existe e retorna o seu índice
int check_func() {
    char *text = pop();
    int idx = lookup_func(st, text);
    if (idx == -1) {
        printf("SEMANTIC ERROR (%d): function '%s' was not declared.\n",
                yylineno, text);
        exit(1);
    }
    return idx;
}

//Verifica se a variável de nome dado por ID existe no escopo da função func e retorna o seu índice
int check_var(int func) {
    char *text = pop();
    int idx = lookup_var(st, text, func);
    if (idx == -1) {
        printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n",
                yylineno, text);
        exit(1);
    }
    return idx;
}

//Caso a função com nome lido no scanner não exista, é criada uma representação da mesma na tabela de funções e seu índice é retornado
int new_func(int n_parameters, int return_type) {
    char *text = pop();
    int idx = lookup_func(st, text);
    if (idx != -1) {
        printf("SEMANTIC ERROR (%d): function '%s' already declared at line %d.\n",
                yylineno, text, get_func_line(st, idx));
        exit(1);
    }
    return add_func(st, text, yylineno, n_parameters, return_type);
}
//Caso a variável com nome lido no scanner não exista no escopo da função atual, é criada uma representação da mesma na tabela de variáveis da função e seu índice é retornado
int new_var(int func, int arr_size) {
    char *text = pop();
    int idx = lookup_var(st, text, func);
    if (idx != -1) {
        printf("SEMANTIC ERROR (%d): variable '%s' already declared at line %d.\n",
                yylineno, text, get_var_line(st, func, idx));
        exit(1);
    }
    return add_var(st, text, yylineno, func, arr_size);
}

// Error handling.
void yyerror (char const *s) {
    printf("PARSE ERROR (%d): %s\n", yylineno, s);
}

// Main.
int main() {
    yydebug = 0; // Toggle this variable to enter debug mode.
    int dot = 0; // A mensagem de PARSE SUCCESSFUL gerava erro ao transformar o .dot em .pdf
    // Initialization of tables before parsing.
    lt = create_lit_table();
    st = create_sym_table();

    int ret = yyparse();
    if(!dot){
       /*printf("\n\n");
       print_lit_table(lt); printf("\n\n");
       print_sym_table(st); printf("\n\n");*/
       if (ret == 0) {
           printf("PARSE SUCCESSFUL!\n");
       }
    }
    else
      print_dot(ast);
    free_tree(ast);
    free_lit_table(lt);
    free_sym_table(st);


    return 0;
}
