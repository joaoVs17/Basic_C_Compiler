#ifndef AST_H
#define AST_H

#include "../lexer/lexer.h"
typedef enum {
  AST_CODE,
  AST_PROGRAM,
  AST_BLOCK,
  AST_DECLARATION,
  AST_VAR_DECLARATION,
  AST_ASSIGNMENT,
  AST_IDENTIFIER,
  AST_NUMBER_LITERAL,
  AST_STRING_LITERAL,
  AST_CHAR_LITERAL,
  AST_BINARY_EXPR,
  AST_TYPE,
  AST_IF,
  AST_WHILE,
  AST_FOR,
  AST_RETURN,
  AST_UNKNOWN
} ASTNodeType;

typedef struct ASTNode {
  ASTNodeType type;
  Token *token;
  struct ASTNode **children;
  int child_count;
  int child_capacity;
} ASTNode;

ASTNode *create_ast(ASTNodeType type, Token *token);
int add_child(ASTNode *parent, ASTNode *child);
void prt_ast(ASTNode *node, int depth);
void destroy_ast(ASTNode *node);
const char *get_node_type_str(ASTNodeType type); 

#endif
