
#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"
#include "tables.h"
#include <string.h>

extern SymTable *st;
extern LitTable *lt;

// Data stack -----------------------------------------------------------------

#define STACK_SIZE 100
//Stack com o endereço das funcoes chamadas
int fIndexStack[STACK_SIZE];
int sp; // stack pointer

//Stack para as variaveis da funcao atual
typedef struct {
    int stack[STACK_SIZE];
    int sp;
} FuncStack;

FuncStack fStack[STACK_SIZE];

int f_atual(){
    return fIndexStack[sp];
}

void init_f_stack() {
    for (int i = 0; i < STACK_SIZE; i++) {
        fIndexStack[i] = 0;
    }
    sp = -1;
}

void init_stack() {
    for (int i = 0; i < STACK_SIZE; i++) {
        fStack[sp].stack[i] = 0;
    }
    fStack[sp].sp = -1;
}

void f_push(int x){
   // printf("fpush %d %d\n",sp,x);
    fIndexStack[++sp] = x;
    init_stack();
}

int f_pop(){
    return fIndexStack[sp--];
}

int top(){
    return fStack[sp].stack[fStack[sp].sp];
}

void push(int x) {
    fStack[sp].stack[++(fStack[sp].sp)] = x;
   // printf("push %d %d %d\n",sp, (fStack[sp].sp), top());
}

int pop() {
   // printf("pop %d %d %d\n",sp, (fStack[sp].sp), top());
    return fStack[sp].stack[fStack[sp].sp--];
}

void print_stack() {
   // printf("*** STACK(size = %d): ",fStack[sp].sp);
    for (int i = 0; i <= fStack[sp].sp; i++) {
       printf("%d ", fStack[sp].stack[i]);
    }
   // printf("\n");
}

// ----------------------------------------------------------------------------

// Variables memory -----------------------------------------------------------

#define MEM_SIZE 100

//mem[SP DO TOPO DA PILHA DE FUNCOES][INDICE DA VARIAVEL][INDICE P/ VETORES]
int mem[MEM_SIZE][MEM_SIZE][MEM_SIZE];

void store(int addr, int val, int i) {
   // printf("store %d %d %d %d\n",sp,addr,i,val);
    mem[sp][addr][i] = val;
}

int load(int addr,int i) {
   // printf("load %d %d %d %d\n",sp,addr,i,mem[sp][addr][i]);
    return mem[sp][addr][i];
}

//Store para passagem de parametro por referencia
void store_ref(int addr, int val, int i, int sp_a) {
   // printf("store %d %d %d %d\n",sp_a,addr,i,val);
    mem[sp_a][addr][i] = val;
}

//Load para passagem de parametro por referencia
int load_ref(int addr, int i, int sp_a){
   // printf("load_ref %d %d %d %d\n",sp_a,addr,i, mem[sp_a][addr][i]);
    return (mem[sp_a][addr][i]);
}

void init_mem() {
    for(int addf = 0; addf < MEM_SIZE; addf++){
        for (int addr = 0; addr < MEM_SIZE; addr++) {
            for(int i = 0;i<MEM_SIZE;i++)
                mem[addf][addr][i] = 0;
        }
    }
}

// ----------------------------------------------------------------------------

// #define TRACE
#ifdef TRACE
#define trace(msg) printf("TRACE: %s\n", msg)
#else
#define trace(msg)
#endif

void rec_run_ast(AST *ast);

void run_func_list(AST *ast) {
    trace("func_list");
   // printf("func_list\n");
    int size = get_child_count(ast);
    for (int i = 0; i < size; i++) {
        rec_run_ast(get_child(ast, i));
    }
}

void run_func_decl(AST *ast) {
    trace("func_decl");
   // printf("func_decl %s\n",get_func_name(st,f_atual()));
    int size = get_child_count(ast);
    for (int i = 0; i < size; i++) {
        rec_run_ast(get_child(ast, i));
    }
}

void run_par_list(AST *ast) {
    trace("par_list");
   // printf("par_list\n");
    int size = get_child_count(ast);
    for (int i = 0; i < size; i++) {
        AST *child = get_child(ast, i);
        int var_idx = get_data(child);
        if(get_kind(child) == C_VAR_NODE){ //Caso seja um vetor, a referencia dele (sp da funcao atual e o indice da variável)
            set_func_ref(st,f_atual(),var_idx,pop()); // é setada na tabela de simbolos para ser resgatada quando necessaria
            set_var_ref(st,f_atual(),var_idx,pop());
        }
        else{
       // printf("store %s %d\n", get_var_name(st,f_atual(),var_idx),top());
        store(var_idx, pop(),0);
        }
    }
}

void run_func_header(AST *ast) {
    trace("func_header");
   // printf("func_header\n");
    run_par_list(get_child(ast,2));
}

void run_func_body(AST *ast) {
    trace("func_body");
   // printf("func_body\n");
    int size = get_child_count(ast);
    for (int i = 0; i < size; i++) {
        rec_run_ast(get_child(ast, i));
    }
}

void run_var_list(AST *ast) {
    trace("var_list");
   // printf("var_list\n");
    int size = get_child_count(ast);
    for (int i = 0; i < size; i++) {
        AST *child = get_child(ast,i);
        if(get_kind(child) == C_VAR_NODE){ //Caso seja um vetor, o tamanho é buscado para setar
            rec_run_ast(get_child(child,0)); // o espaco na memoria (mem)
            int tam = pop();
            for(int j = 0;j<tam;j++){
                store(get_data(child),0,j);
            }
        }
        else
            store(get_data(child),0,0);
    }
}

void run_stmt_seq(AST *ast) {
    trace("stmt_seq");
   // printf("stmt_seq\n");
    int size = get_child_count(ast);
    for (int i = 0; i < size; i++) {
        rec_run_ast(get_child(ast, i));
    }
}

void run_if(AST *ast) {
    trace("if");
   // printf("if\n");
    rec_run_ast(get_child(ast, 0));
    int test = pop();
    if (test == 1) {
        rec_run_ast(get_child(ast, 1));
    } else if (test == 0 && get_child_count(ast) == 3) {
        rec_run_ast(get_child(ast, 2));
    }
}

void run_while(AST *ast) {
    trace("while");
   // printf("while\n");
    int again = 1;
    while (again) {
        rec_run_ast(get_child(ast, 1)); // Run test.
        rec_run_ast(get_child(ast, 0)); // Run body.
        again = pop();
    }
}

void run_assign(AST *ast) {
    trace("assign");
   // printf("assign\n");
    rec_run_ast(get_child(ast, 1));
    AST *child = get_child(ast, 0);
    int var_idx = get_data(child);
    int index = 0;
    if(get_kind(child) == C_VAR_NODE){
        rec_run_ast(get_child(child,0));
        index = pop();
        if(get_func_ref(st,f_atual(),var_idx) != -1){ //Se o vetor foi passado por referencia, usa-se a referencia nas
            int var_ref = get_var_ref(st,f_atual(),var_idx); //operacoes com o mesmo
            int func_ref = get_func_ref(st,f_atual(),var_idx);
            store_ref(var_ref,pop(),index,func_ref);
        }
        else{
           // printf("store %s %d\n", get_var_name(st,f_atual(),var_idx),top());
            store(var_idx, pop(),index);
        }
    }
    else{
       // printf("store %s %d\n", get_var_name(st,f_atual(),var_idx),top());
        store(var_idx, pop(),index);
    }
}

void run_input(AST *ast) {
    trace("input");
    int x;
    printf("input: ");
    scanf("%d", &x);
    push(x);
}

void run_output(AST *ast) {
    trace("output");
   // printf("output\n");
    rec_run_ast(get_child(ast, 0));
    int x = pop();
    printf("%d", x);
}

void run_write(AST *ast) {
    trace("write");
   // printf("write\n");
    rec_run_ast(get_child(ast, 0));
    int x = pop();
    char *string = get_literal(lt,x);
    for(int i = 0;i<strlen(string);i++){ //Gambiarra por causa do \n
        if(string[i] != '\\') printf("%c",string[i]);
        else{
            i++;
            if(string[i]  == 'n') printf("\n");
            else printf("%c",string[i]);
        }
    }
    //printf("%s", get_literal(lt,x));
}

void run_arg_list(int size, int args_list[size]) {
    trace("arg_list");
   // printf("arg_list\n");
    for (int i = size-1; i >= 0; i--) { //Da pop() na ordem inversa para ser recuperado corretamente
        push(args_list[i]);
    }
}

void run_return(AST *ast){
    trace("return");
    if(get_child_count(ast)){
        rec_run_ast(get_child(ast, 0));
        int resp = pop();
       // printf("return %d\n", resp);
        f_pop(); //Da f_pop() na funcao para o valor de retorno
        push(resp); //ser armazenado na pilha da funcao anterior
    }
}

void run_fcall(AST *ast){
    trace("fcall");
   // printf("fcall\n");
    AST *args = get_child(ast,0);
    int size = 0;
    for(int i = 0; i < get_child_count(args);i++){ //Caso seja um vetor, sao necessarios 2 parametros,
        AST *child = get_child(args,i); //a referencia da funcao atual e da variavel, por isso o size n é igual
        if(get_kind(child) == S_VAR_NODE && get_var_arr_size(st,f_atual(),get_data(child)) != 0)
            size+=1; //ao numero de childs
        size+=1;
    }
    int args_list[size];
    for (int i = 0, j = 0; i < get_child_count(args); i++) {
        AST *child = get_child(args,i);
        if(get_kind(child) == S_VAR_NODE && get_var_arr_size(st,f_atual(),get_data(child)) != 0){
            if(get_func_ref(st,f_atual(),get_data(child)) != -1){ //coloca as referencias necessarias na lista de argumentos
                args_list[j++] = get_func_ref(st,f_atual(),get_data(child));
                args_list[j++] = get_var_ref(st,f_atual(),get_data(child));
            }
            else{
                args_list[j++] = sp;
                args_list[j++] = get_data(child);
            }
           // printf("entrou %d %d\n",args_list[j-2],args_list[j-1]);
        }
        else{
            rec_run_ast(child);
            args_list[j++] = pop();
        }
    }
    AST *func = get_ast_func(st,get_data(ast));
    int index = get_data(get_child(get_child(func,0),1));
    f_push(index);
    run_arg_list(size, args_list); //Envia a lista de argumentos depois de dar f_push,
    rec_run_ast(func); //armazenando-os na pilha da funcao a ser chamada
}

#define bin_op() \
    rec_run_ast(get_child(ast, 0)); \
    rec_run_ast(get_child(ast, 1)); \
    int r = pop(); \
    int l = pop()

void run_plus(AST *ast) {
    trace("plus");
    bin_op();
    push(l + r);
}

void run_minus(AST *ast) {
    trace("minus");
    bin_op();
    push(l - r);
}

void run_times(AST *ast) {
    trace("times");
    bin_op();
    push(l * r);
}

void run_over(AST *ast) {
    trace("over");
    bin_op();
    push((int) l / r);
}

void run_arith_op(AST *ast){
    bin_op();
   // printf("arith\n");
    switch(get_kind(ast)) {
    case PLUS_NODE:
        trace("plus");
        push(l + r);
        break;
    case MINUS_NODE:
        trace("minus");
        push(l - r);
        break;
    case TIMES_NODE:
        trace("times");
        push(l * r);
        break;
    case OVER_NODE:
        trace("over");
        push((int) l / r);
        break;
    default:
        break;
    }
}

void run_lt(AST *ast) {
    trace("lt");
    bin_op();
    push(l < r);
}

void run_eq(AST *ast) {
    trace("eq");
    bin_op();
    push(l == r);
}

void run_comp_op(AST *ast){
    bin_op();
   // printf("comp_op\n");
    switch(get_kind(ast)) {
        case LT_NODE:
            trace("lt");
            push (l < r);
            break;
        case LE_NODE:
            trace("le");
            push (l <= r);
            break;
        case GT_NODE:
            trace("gt");
            push (l > r);
            break;
        case GE_NODE:
            trace("ge");
            push (l >= r);
            break;
        case EQ_NODE:
            trace("eq");
            push (l == r);
            break;
        case NEQ_NODE:
            trace("neq");
            push (l != r);
            break;
        default:
            break;
    }
}

void run_num(AST *ast) {
    trace("num");
   // printf("num\n");
    push(get_data(ast));
}

void run_string(AST *ast) {
    trace("string");
   // printf("string\n");
    push(get_data(ast));
}

void run_id(AST *ast) {
    trace("id");
   // printf("id\n");
    int var_idx = get_data(ast);
    push(load(var_idx,0));
}

void run_s_var(AST *ast) {
    trace("s_var");
    int var_idx = get_data(ast);
   // printf("s_var %d\n",var_idx);
    push(load(var_idx,0));
}

void run_c_var(AST *ast) {
    trace("c_var");
   // printf("c_var\n");
    int var_idx = get_data(ast);
    rec_run_ast(get_child(ast,0));
    int index = pop();
    if(get_func_ref(st,f_atual(),var_idx) != -1){ //A particularidade de vetores ja foi explicada acima
        int var_ref = get_var_ref(st,f_atual(),var_idx);
        int func_ref = get_func_ref(st,f_atual(),var_idx);
        push(load_ref(var_ref,index,func_ref));
    }
    else
        push(load(var_idx,index));
}

void rec_run_ast(AST *ast) {
    switch(get_kind(ast)) {
        case FUNC_LIST_NODE:
            run_func_list(ast);
            break;
        case FUNC_DECL_NODE:
            run_func_decl(ast);
            break;
        case FUNC_HEADER_NODE:
            run_func_header(ast);
            break;
        case FUNC_BODY_NODE:
            run_func_body(ast);
            break;
        case PAR_LIST_NODE:
            run_par_list(ast);
            break;
        case VAR_LIST_NODE:
            run_var_list(ast);
            break;
        case STMT_SEQ_NODE:
            run_stmt_seq(ast);
            break;
        case IF_NODE:
            run_if(ast);
            break;
        case WHILE_NODE:
            run_while(ast);
            break;
        case ASSIGN_NODE:
            run_assign(ast);
            break;
        case INPUT_NODE:
            run_input(ast);
            break;
        case OUTPUT_NODE:
            run_output(ast);
            break;
        case WRITE_NODE:
            run_write(ast);
            break;
        case RETURN_NODE:
            run_return(ast);
            break;
        case FCALL_NODE:
            run_fcall(ast);
            break;
        case PLUS_NODE:
        case MINUS_NODE:
        case TIMES_NODE:
        case OVER_NODE:
            run_arith_op(ast);
            break;
        case LT_NODE:
        case LE_NODE:
        case GT_NODE:
        case GE_NODE:
        case EQ_NODE:
        case NEQ_NODE:
            run_comp_op(ast);
            break;
       case NUM_NODE:
            run_num(ast);
            break;
       case STRING_NODE:
            run_string(ast);
            break;
        case ID_NODE:
            run_id(ast);
            break;
        case S_VAR_NODE:
            run_s_var(ast);
            break;
        case C_VAR_NODE:
            run_c_var(ast);
            break;
        case INT_TYPE_NODE:
        case VOID_TYPE_NODE:
            break;
        default:
            fprintf(stderr, "Invalid kind: %s!\n", kind2str(get_kind(ast)));
            exit(1);
    }
}

void run_ast(AST *ast) {
    init_stack();
    init_mem();
    init_f_stack();
    int index = get_data(get_child(get_child(ast,0),1));
    f_push(index);
   // printf("main index %d\n",index);
    AST *main_body = get_child(ast,1);
    rec_run_ast(main_body);
}
