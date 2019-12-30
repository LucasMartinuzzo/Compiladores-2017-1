
#ifndef TABLES_H
#define TABLES_H

// Literals Table
// ----------------------------------------------------------------------------

#define LITERAL_MAX_SIZE 128
#define LITERALS_TABLE_MAX_SIZE 100
// Opaque structure.
// For simplicity, the table is implemented as a sequential list.
struct lit_table;
typedef struct lit_table LitTable;
typedef struct node AST;
// Creates an empty literal table.
LitTable* create_lit_table();

// Adds the given string to the table without repetitions.
// String 's' is copied internally.
// Returns the index of the string in the table.
int add_literal(LitTable* lt, char* s);

// Returns a pointer to the string stored at index 'i'.
char* get_literal(LitTable* lt, int i);

// Prints the given table to stdout.
void print_lit_table(LitTable* lt);

// Clears the allocated structure.
void free_lit_table(LitTable* lt);


// Symbols Table
// ----------------------------------------------------------------------------

#define SYMBOL_MAX_SIZE 128
#define SYMBOL_TABLE_MAX_SIZE 100

// Opaque structure.
// For simplicity, the table is implemented as a sequential list.
// This table only stores the variable name and the declaration line.
struct sym_table;
typedef struct sym_table SymTable;

// Creates an empty symbol table.
SymTable* create_sym_table();

// Adds a fresh var to the table.
// No check is made by this function, so make sure to call 'lookup_var' first.
// Returns the index where the variable was inserted.
int add_func(SymTable* st, char* s, int line, int n_parameters, int return_type);
int add_var(SymTable* st, char* s, int line, int func, int array_size);

// Returns the index where the given variable is stored or -1 otherwise.
int lookup_func(SymTable* st, char* s);
int lookup_var(SymTable* st, char* s, int func);

// Returns the variable name stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
char* get_func_name(SymTable* st, int i);
char* get_var_name(SymTable* st, int func, int i);

// Returns the declaration line of the variable stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
int get_func_line(SymTable* st, int i);
int get_var_line(SymTable* st, int func, int i);
int get_var_arr_size(SymTable* st, int func, int i);
// Returns the function arity
int get_arity(SymTable *st, int func);

// Prints the given table to stdout.
void print_sym_table(SymTable* st);

// Clears the allocated structure.
void free_sym_table(SymTable* st);

//Set the arity of the function (i) to n
void change_n_parameters(SymTable* st, int i, int n);

//Set ast pointer to function
void set_ast_func(SymTable* st, int func, AST *ast);
AST * get_ast_func(SymTable* st, int func);


//Outros
int get_func_ref(SymTable* st, int func, int i);
int get_var_ref(SymTable* st, int func, int i);
void set_func_ref(SymTable* st, int func, int i, int ref);
void set_var_ref(SymTable* st, int func, int i, int ref);
#endif // TABLES_H

