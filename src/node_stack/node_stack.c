#include "node_stack.h"

#include <stdio.h>

void node_stack_init(NodeStack *stack) {
  if (!stack) {
    return;
  }

  stack->count = 0;
}

void node_stack_clear(NodeStack *stack) {
  node_stack_init(stack);
}

int node_stack_push(NodeStack *stack, ASTNode *node) {
  if (!stack || node_stack_is_full(stack)) {
    return 0;
  }

  stack->items[stack->count] = node;
  stack->count++;
  return 1;
}

int node_stack_pop(NodeStack *stack, ASTNode **out_node) {
  if (!stack || !out_node || node_stack_is_empty(stack)) {
    return 0;
  }

  stack->count--;
  *out_node = stack->items[stack->count];
  return 1;
}

int node_stack_peek(const NodeStack *stack, ASTNode **out_node) {
  if (!stack || !out_node || node_stack_is_empty(stack)) {
    return 0;
  }

  *out_node = stack->items[stack->count - 1];
  return 1;
}

int node_stack_is_empty(const NodeStack *stack) {
  return !stack || stack->count == 0;
}

int node_stack_is_full(const NodeStack *stack) {
  return stack && stack->count >= NODE_STACK_MAX;
}

int node_stack_size(const NodeStack *stack) {
  if (!stack) {
    return 0;
  }

  return stack->count;
}

void node_stack_print(const NodeStack *stack) {
  printf("[\n");
  for (int i = 0; i < stack->count; i++) {
    printf("  %s\n", get_node_type_str(stack->items[i]->type));
  }
  printf("]\n");
}

int push_leaf(NodeStack *stack, ASTNodeType type, Token *token) {
  ASTNode *node = create_ast(type, token);
  if (node == NULL) {
    return 0;
  }
  return node_stack_push(stack, node);
}

int node_stack_reduce(NodeStack *stack, ASTNodeType type, Token *token, int n) {
  if (!stack || n < 0 || n > NODE_STACK_REDUCE_MAX) {
    return 0;
  }

  // desempilha n ASTNodes. 
  // O máximo de ASTNodes que podem ser desempilhados é NODE_STACK_REDUCE_MAX
  // desempilha em ordem inversa já que na pilha o último a entrar é o primeiro a sair
  ASTNode *children[NODE_STACK_REDUCE_MAX];
  for (int i = n - 1; i >= 0; i--) {
    if (!node_stack_pop(stack, &children[i])) {
      return 0;
    }
  }

  ASTNode *parent = create_ast(type, token);
  if (parent == NULL) {
    return 0;
  }
  
  //Coloca os ASTNodes desempilhados no pai
  for (int i = 0; i < n; i++) {
    add_child(parent, children[i]);
  }

  return node_stack_push(stack, parent);
}
