#include "ast.h"

static int expand_children_capacity(ASTNode *node) {
  int old_capacity = node->child_capacity;
  int new_capacity = old_capacity == 0 ? 4 : old_capacity * 2;
  ASTNode **new_children =
      (ASTNode **)realloc(node->children, sizeof(ASTNode *) * new_capacity);

  if (new_children == NULL) {
    return 0;
  }
  node->children = new_children;
  node->child_capacity = new_capacity;
  return 1;
}

ASTNode *create_ast(ASTNodeType type, Token *token) {
  ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));

  if (node == NULL) {
    return NULL;
  }

  node->type = type;
  node->token = token;
  node->children = NULL;
  node->child_count = 0;
  node->child_capacity = 0;

  return node;
}

int add_child(ASTNode *parent, ASTNode *child) {
  if (parent == NULL || child == NULL) {
    return 0;
  }

  if (parent->child_count == parent->child_capacity &&
      !expand_children_capacity(parent)) {
    return 0;
  }
  parent->children[parent->child_count] = child;
  parent->child_count += 1;
  return 1;
}

void prt_ast(ASTNode *node, int depth) {
  if (node == NULL) {
    return;
  }
  for (int i = 0; i < depth; i++) {
    printf("  ");
  }
  printf("%s", get_node_type_str(node->type));
  if (node->token != NULL && node->token->lex != NULL) {
    printf(" (\x1b[32m%s\x1b[0m) [Row: %d, Col: %d]", node->token->lex, node->token->row, node->token->col);
  }
  printf("\n");
  for (int i = 0; i < node->child_count; i++) {
    prt_ast(node->children[i], depth + 1);
  }
}

void destroy_ast(ASTNode *node) {
  if (node == NULL) {
    return;
  }

  for (int i = 0; i < node->child_count; i++) {
    destroy_ast(node->children[i]);
  }

  free(node->children);
  free(node);
}

const char *get_node_type_str(ASTNodeType type) {
  switch (type) {
  case AST_CODE: 
    return "CODE";
  case AST_PROGRAM:
    return "PROGRAM";
  case AST_BLOCK:
    return "BLOCK";
  case AST_DECLARATION:
    return "DECLARATION";
  case AST_ASSIGNMENT:
    return "ASSIGNMENT";
  case AST_IDENTIFIER:
    return "IDENTIFIER";
  case AST_NUMBER_LITERAL:
    return "NUMBER_LITERAL";
  case AST_STRING_LITERAL:
    return "STRING_LITERAL";
  case AST_CHAR_LITERAL:
    return "CHAR_LITERAL";
  case AST_BINARY_EXPR:
    return "BINARY_EXPR";
  case AST_IF:
    return "IF";
  case AST_WHILE:
    return "WHILE";
  case AST_FOR:
    return "FOR";
  case AST_RETURN:
    return "RETURN";
  case AST_VAR_DECLARATION:
    return "VAR_DECLARATION";
  case AST_TYPE:
    return "TYPE";
  case AST_UNKNOWN:
    return "UNKNOWN";
  default:
    return "INVALID";
  }
}
