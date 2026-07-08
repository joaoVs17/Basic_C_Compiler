#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../node_stack/node_stack.h"
#include "../parse_stack/parse_stack.h"
#include "../production/production.h"

int is_terminal(GrammarSymbol s);

#define SYMTAB_MAX 256

typedef struct sSymbol {
  char *name; // Isso vai ficar no .data
  char *type;// Para saber sobre a memória usada
  int row, col;
} Symbol;

extern Symbol symtab[SYMTAB_MAX];
extern int symcount;

typedef struct sParser {
  Pointer *p;
  Token *t;
  ASTNode *ast;
} Parser;

int init_parser(Parser *parser, Pointer *pointer);

int parse(Parser *p, ParseStack *stack);

void next(Parser *p);
int syntax_error(char *);
int matches(GrammarSymbol sym, Token *tk);
GrammarSymbol token_to_symbol(const Token *token);
int apply_production(ParseStack *stack, NodeStack *node, Production production, Token *current_token);
int handle_terminal_top(StackItem *current_item, Token *current_token, Parser *parser, NodeStack *node_stack);
int handle_non_terminal_top(StackItem *current_item, Token *current_token, ParseStack *stack, NodeStack *node_stack);
int handle_action(StackItem *item, NodeStack *node_stack);
int build_block(NodeStack *node_stack);
char *token_to_mips_instruction(Token *token);

Symbol *symtab_get(char *name);
int symtab_insert(char *name, char *type, int row, int col);
void symtab_print(void);

#endif