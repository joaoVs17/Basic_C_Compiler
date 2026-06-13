#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "../stack/stack.h"
#include "../ast/ast.h"
#include "../production/production.h"
 
int is_terminal(GrammarSymbol s);
int is_non_terminal(GrammarSymbol s);

typedef struct sParser {
  Pointer * p;
  Token * t;
} Parser;

int init_parser(Parser *parser, Pointer *pointer);

int parse(Parser * p, ParseStack * stack);


int consume(Parser *p, TokenType type);
int consume_by_lex(Parser *p, char *lex);
void next(Parser *p);
int syntax_error(char *);
int matches(GrammarSymbol sym, Token * tk);
GrammarSymbol token_to_symbol(const Token *token);
int apply_production(ParseStack *stack, Production production);
int handle_terminal_top(GrammarSymbol top, Token *current_token, Parser *parser);


#endif