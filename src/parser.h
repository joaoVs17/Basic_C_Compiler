#ifndef PARSER_H
#define PARSER_H

#include "pointer.h"

typedef struct sParser {
  Pointer *pointer;
  Token *current;
} Parser;

//Parser

int init_parser(Parser *p, Pointer *pointer);

//LET IT PAAAAAARSE
int parse(Parser *p);
int parse_code(Parser *p);
int parse_prog(Parser *p);
int parse_decl(Parser *p);
int parse_block(Parser *p);
int parse_var_declaration(Parser *p);
int parse_condition(Parser *p);
int parse_while_loop(Parser *p);
int parse_assignment(Parser *p);
int parse_type(Parser *p);

//Expression
int parse_expr(Parser *p);
int parse_expr_tail(Parser *p);

int parse_term(Parser *p);
int parse_term_tail(Parser *p);

int parse_factor(Parser *p);
int parse_value(Parser *p);


//Util
int is_assignment_operator(Token *token);

//Handlde token flow
int consume(Parser *p, TokenType type); //Use after check or check_by_lex and before parse_fns
int consume_by_lex(Parser *p, char* lex);
int check_by_lex(Parser *p, char *lex); // Use after parse_fns
int check(Parser *p, TokenType type); // Use after parse_fns
void next(Parser *p);
Token *peek(Parser *p);

#endif
