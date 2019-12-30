
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tables.h"
#include "ast.h"
// Literals Table
// ----------------------------------------------------------------------------


struct lit_table {
    char t[LITERALS_TABLE_MAX_SIZE][LITERAL_MAX_SIZE];
    int size;
};

LitTable* create_lit_table() {
    LitTable *lt = malloc(sizeof * lt);
    lt->size = 0;
    return lt;
}

int add_literal(LitTable* lt, char* s) {
    for (int i = 0; i < lt->size; i++) {
        if (strcmp(lt->t[i], s) == 0) {
            return i;
        }
    }
    strcpy(lt->t[lt->size], s);
    int idx_added = lt->size;
    lt->size++;
    return idx_added;
}

char* get_literal(LitTable* lt, int i) {
    return lt->t[i];
}

void print_lit_table(LitTable* lt) {
    printf("Literals table:\n");
    for (int i = 0; i < lt->size; i++) {
        printf("Entry %d -- %s\n", i, get_literal(lt, i));
    }
}

void free_lit_table(LitTable* lt) {
    free(lt);
}

// Symbols Table
// ----------------------------------------------------------------------------

typedef struct {
  char name[SYMBOL_MAX_SIZE];
  int line;
  int func_ref; //Caso seja C_VAR_NODE passado por parametro, conterá a referência do vetor original
  int var_ref;
  int arr_size; //Tamanho 0 para inteiros, n > 0 para arrays com tam. definido, -1 para arrays tamanho indefinido (parâmetros)
} Entry;

typedef struct{
   char func_name[SYMBOL_MAX_SIZE];
   int line;
   Entry var[SYMBOL_TABLE_MAX_SIZE];
   int size;
   int n_parameters;
   int return_type; //0 = void; 1 = int
   AST *ast;
} FuncEntry;

struct sym_table {
    FuncEntry t[SYMBOL_TABLE_MAX_SIZE];
    int size;
};

SymTable* create_sym_table() {
    SymTable *st = malloc(sizeof * st);
    st->size = 0;
    return st;
}

int lookup_func(SymTable* st, char* s) {
    for (int i = 0; i < st->size; i++) {
        if (strcmp(st->t[i].func_name, s) == 0)  {
            return i;
        }
    }
    return -1;
}

int lookup_var(SymTable* st, char* s, int func) {
   for(int i = 0;i < st->t[func].size; i++){
      if(strcmp(st->t[func].var[i].name, s) == 0){
         return i;
      }
   }
   return -1;
}

int add_func(SymTable* st, char* s, int line, int n_parameters, int return_type) {
    strcpy(st->t[st->size].func_name, s);
    st->t[st->size].line = line;
    st->t[st->size].n_parameters = n_parameters;
    st->t[st->size].return_type = return_type;
    int idx_added = st->size;
    st->size++;
    return idx_added;
}

int add_var(SymTable* st, char* s, int line, int func, int array_size) {
    strcpy(st->t[func].var[st->t[func].size].name, s);
    st->t[func].var[st->t[func].size].line = line;
    st->t[func].var[st->t[func].size].arr_size = array_size;
    int idx_added = st->t[func].size;
    st->t[func].var[st->t[func].size].func_ref = -1;
    st->t[func].var[st->t[func].size].var_ref = -1;
    st->t[func].size++;

    return idx_added;
}

int get_func_ref(SymTable* st, int func, int i){
    return st->t[func].var[i].func_ref;
}

int get_var_ref(SymTable* st, int func, int i){
    return st->t[func].var[i].var_ref;
}

void set_func_ref(SymTable* st, int func, int i, int ref){
    st->t[func].var[i].func_ref = ref;
}

void set_var_ref(SymTable* st, int func, int i, int ref){
    st->t[func].var[i].var_ref = ref;
}

void change_n_parameters(SymTable* st, int i, int n){
   st->t[i].n_parameters = n;
}

char* get_func_name(SymTable* st, int i) {
    return st->t[i].func_name;
}

char* get_var_name(SymTable* st, int func, int i) {
    return st->t[func].var[i].name;
}

int get_arity(SymTable *st, int func) {
   return st->t[func].n_parameters;
}

int get_func_line(SymTable* st, int i) {
    return st->t[i].line;
}

int get_var_line(SymTable* st, int func, int i) {
    return st->t[func].var[i].line;
}

int get_var_arr_size(SymTable* st, int func, int i){
    return st->t[func].var[i].arr_size;
}

void print_sym_table(SymTable* st) {
    printf("Symbols table:\n");
    for (int i = 0; i < st->size; i++) {
      printf("Entry %d -- function name: %s, line: %d, arity: %d\n", i, get_func_name(st, i), get_func_line(st, i), get_arity(st, i));
      for(int j = 0; j < st->t[i].size;j++){
         printf("\tEntry %d -- variable name: %s, line: %d, type: %d\n", j, get_var_name(st, i, j), get_var_line(st, i, j),st->t[i].var[j].arr_size);
      }
    }
}

void free_sym_table(SymTable* st) {
    free(st);
}

void set_ast_func(SymTable* st, int func, AST *ast){
    st->t[func].ast = ast;
}

AST * get_ast_func(SymTable* st, int func){
    return st->t[func].ast;
}
