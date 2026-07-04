#ifndef NODE_STACK_H
#define NODE_STACK_H

#include "../ast/ast.h"

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

#endif
