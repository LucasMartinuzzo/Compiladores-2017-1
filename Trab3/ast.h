#ifndef AST_H
#define AST_H

typedef enum {
   FUNC_LIST_NODE,
   FUNC_DECL_NODE,
   FUNC_HEADER_NODE,
   FUNC_BODY_NODE,
   INT_TYPE_NODE,
   VOID_TYPE_NODE,
   PAR_LIST_NODE,
   VAR_LIST_NODE,
   STMT_SEQ_NODE,
   IF_NODE,
   WHILE_NODE,
   ASSIGN_NODE,
   INPUT_NODE,
   OUTPUT_NODE,
   WRITE_NODE,
   RETURN_NODE,
   FCALL_NODE,
   ARG_LIST_NODE,
   PLUS_NODE,
   MINUS_NODE,
   TIMES_NODE,
   OVER_NODE,
   LT_NODE,
   LE_NODE,
   GT_NODE,
   GE_NODE,
   EQ_NODE,
   NEQ_NODE,
   NUM_NODE,
   STRING_NODE,
   ID_NODE,
   S_VAR_NODE,
   C_VAR_NODE
} NodeKind;

struct node; // Opaque structure to ensure encapsulation.

typedef struct node AST;

AST* new_node(NodeKind kind, int func, int data);

void add_child(AST *parent, AST *child);
AST* get_child(AST *parent, int idx);

AST* new_subtree(NodeKind kind, int child_count, ...);

NodeKind get_kind(AST *node);
char* kind2str(NodeKind kind);
int get_return_type(AST *node);
int get_data(AST *node);
int get_child_count(AST *node);

void print_tree(AST *ast);
void print_dot(AST *ast);

void free_tree(AST *ast);

#endif
