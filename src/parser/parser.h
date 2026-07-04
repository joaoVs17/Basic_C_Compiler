#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "../symbol_stack/symbol_stack.h"
#include "../ast/ast.h"
#include "../node_stack/node_stack.h"
#include "../production/production.h"
 
int is_terminal(GrammarSymbol s);
int is_non_terminal(GrammarSymbol s);

typedef struct sParser {
  Pointer * p;
  Token * t;
  ASTNode *ast;
} Parser;

int init_parser(Parser *parser, Pointer *pointer);

int parse(Parser * p, ParseStack * stack);


int consume(Parser *p, TokenType type);
int consume_by_lex(Parser *p, char *lex);
void next(Parser *p);
int syntax_error(char *);
int matches(GrammarSymbol sym, Token * tk);
GrammarSymbol token_to_symbol(const Token *token);
int apply_production(ParseStack *stack, NodeStack *node, Production production);
int handle_terminal_top(GrammarSymbol top, Token *current_token, Parser *parser);
int handle_non_terminal_top(GrammarSymbol top, Token * current_token, ParseStack * stack, NodeStack * node_stack);
int handle_ast_construct(Production applied_produciton, GrammarSymbol unstacked_symbol, Token * current_token, NodeStack * node_stack);
GrammarSymbol production_non_terminal(Production production);


#endif