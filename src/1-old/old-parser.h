#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "pointer.h"
#include <string.h>

typedef struct {
  char *msg;
  Token *fnd;
  TokenType expected_type;
  char * expected_lex;
} ParserError;

typedef struct sParser {
  Pointer *pointer;
  Token *current;
  ParserError *errors;
  int error_count;
  int error_capacity;

  int pp;
} Parser;

// Parser

int init_parser(Parser *p, Pointer *pointer);

// LET IT PAAAAAARSE
int parse(Parser *p);
ASTNode *parse_code(Parser *p);
int parse_prog(Parser *p, ASTNode *parent);
ASTNode *parse_decl(Parser *p);
ASTNode *parse_block(Parser *p);
ASTNode *parse_var_declaration(Parser *p);
ASTNode *parse_condition(Parser *p);
ASTNode *parse_while_loop(Parser *p);
ASTNode *parse_assignment(Parser *p);
Token *parse_type(Parser *p);

// Expression
ASTNode *parse_expr(Parser *p);
ASTNode *parse_add_expr(Parser *p);

ASTNode *parse_add_expr_tail(Parser *p, ASTNode *left);

ASTNode *parse_logical_or_expr(Parser *p);
ASTNode *parse_logical_or_expr_tail(Parser *p, ASTNode *left);

ASTNode *parse_logical_and_expr(Parser *p);
ASTNode *parse_logical_and_expr_tail(Parser *p, ASTNode *left);

ASTNode *parse_equality_expr(Parser *p);
ASTNode *parse_equality_expr_tail(Parser *p, ASTNode *left);

ASTNode *parse_relational_expr(Parser *p);
ASTNode *parse_relational_expr_tail(Parser *p, ASTNode *left);

ASTNode *parse_term(Parser *p);
ASTNode *parse_term_tail(Parser *p, ASTNode *left);

ASTNode *parse_factor(Parser *p);
ASTNode *parse_value(Parser *p);

// Util
int is_assignment_operator(Token *token);

// Handlde token flow
int consume(
    Parser *p,
    TokenType type); // Use after check or check_by_lex and before parse_fns
int consume_by_lex(Parser *p, char *lex);
int check_by_lex(Parser *p, char *lex); // Use after parse_fns
int check(Parser *p, TokenType type);   // Use after parse_fns
void next(Parser *p);
Token *peek(Parser *p);

void add_parser_error(Parser *p, char *message, Token* t, TokenType expected);
void add_parser_error_lex(Parser *p, char *message, Token *t, char * expected_lex);
void synchronize(Parser *p);
void prt_parser_errors(Parser *p);
void add_simple_error(Parser *p, char *message);
ParserError get_last_error(Parser *p);
#endif
