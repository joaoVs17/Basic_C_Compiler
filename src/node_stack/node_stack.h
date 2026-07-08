#ifndef NODE_STACK_H
#define NODE_STACK_H



#include "../ast/ast.h"

//Até agora, o máximo são 3 filhos
#define NODE_STACK_REDUCE_MAX 3
#define NODE_STACK_MAX 1024

typedef struct {
  ASTNode *items[NODE_STACK_MAX];
  int count;
} NodeStack;

void node_stack_init(NodeStack *stack);
void node_stack_clear(NodeStack *stack);

int node_stack_push(NodeStack *stack, ASTNode *node);
int node_stack_pop(NodeStack *stack, ASTNode **out_node);
int node_stack_peek(const NodeStack *stack, ASTNode **out_node);

int node_stack_is_empty(const NodeStack *stack);
int node_stack_is_full(const NodeStack *stack);
int node_stack_size(const NodeStack *stack);
void node_stack_print(const NodeStack *stack);

// Cria uma folha (nó sem filhos) a partir de um token e empilha-a.
int push_leaf(NodeStack *stack, ASTNodeType type, Token *token);

// Retira os n nós do topo, cria um pai com eles (na ordem correta) e
// empilha o pai de volta. É a operação de "reduzir" filhos num nó.
int node_stack_reduce(NodeStack *stack, ASTNodeType type, Token *token, int n);

#endif
