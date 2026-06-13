#include "parser.h"

int init_parser(Parser *parser, Pointer *pointer) {
  if (parser == NULL || pointer == NULL)
    return 0;

  parser->p = pointer;
  parser->t = NULL;
  return 1;
}

int parse(Parser *p, ParseStack *stack) {
  // parse_stack_push(stack, SYM_EOF);
  parse_stack_push(stack, SYM_CODE);

  next(p);
  int count = 0;
  while (!parse_stack_is_empty(stack)) {
    // if (count >= 2) return 0;
    GrammarSymbol top;
    Token *current_token = p->t;
    parse_stack_print(stack);
    parse_stack_pop(stack, &top);

    // prt_token(current_token, 1);
    
    if (is_terminal(top)) {
      if (!handle_terminal_top(top, current_token, p)) return 0;
    } else {
      
      GrammarSymbol lookahead = token_to_symbol(current_token);
      Production production = ll1_lookup(top, lookahead);

      if (production == PROD_NONE) {
        syntax_error("Production == PROD_NONE");
        return 0;
      } else {
        if (!apply_production(stack, production))
          return 0;
      }
    }
    count++;
  }

  return 1;
}

int handle_terminal_top(GrammarSymbol top, Token *current_token, Parser *parser) {
  if (matches(top, current_token)) {
    next(parser);
    current_token = parser->t;
    return 1;
  } else {
    syntax_error("CURRENT TOKEN DOESN'T MATCH TOP");
    prt_token(current_token, 1);
    printf("%d symbol", top);
    return 0;
  }
}

static void handle_non_terminal_top(void) {}

// Simple rule, if it's not a termina is a non terminal
int is_non_terminal(GrammarSymbol sym) {
  return sym >= FIRST_NON_TERMINAL &&
         sym <= LAST_NON_TERMINAL;
}

int is_terminal(GrammarSymbol sym) {
  return sym >= FIRST_TERMINAL &&
         sym <= LAST_TERMINAL;
}

void next(Parser *p) { p->t = read_next_token(p->p, 0); }
int consume(Parser *p, TokenType type) {
  if (p->t->type != type) {
    return 0;
  }
  if (p->t->type == TOKEN_EOF)
    return 1;
  next(p);
  return 1;
}
int consume_by_lex(Parser *p, char *lex) {

  if (p == NULL || p->t == NULL || lex == NULL) {
    return 0;
  }

  prt_token(p->t, 1);
  next(p);
  return 1;
}

int syntax_error(char * c) {
  printf("%s\n", c);
  return 0;
}
int matches(GrammarSymbol sym, Token *tk) {
  if (!tk) {
    return 0;
  }

  return sym == token_to_symbol(tk);
}

GrammarSymbol token_to_symbol(const Token *token) {
  switch (token->type) {
  case IDENTIFIER:
    return SYM_IDENTIFIER;

  case NUMBER:
    return SYM_NUMBER;

  case TOKEN_EOF:
    return SYM_EOF;

  case KEYWORD_IF:
    return SYM_IF;

  case KEYWORD_ELSE:
    return SYM_ELSE;

  case KEYWORD_WHILE:
    return SYM_WHILE;

  case KEYWORD_TYPE:

    if (strcmp(token->lex, "int") == 0)
      return SYM_INT;

    if (strcmp(token->lex, "float") == 0)
      return SYM_FLOAT;

    if (strcmp(token->lex, "char") == 0)
      return SYM_CHAR;

    if (strcmp(token->lex, "string") == 0)
      return SYM_STRING;

    break;

  case OPERATOR_ASSIGNMENT:
  
  if (strcmp(token->lex, "=") == 0)
  return SYM_ASSIGN;
  
  break;
  
  case OPERATOR_ARITMETIC:
  if (strcmp(token->lex, "+") == 0)
  return SYM_PLUS;
  
  if (strcmp(token->lex, "-") == 0)
  return SYM_MINUS;
  
  if (strcmp(token->lex, "*") == 0)
  return SYM_MULTIPLY;
  
  if (strcmp(token->lex, "/") == 0)
  return SYM_DIVIDE;
  
  break;
  
  case OPERATOR_LOGIC:
  case OPERATOR:

    if (strcmp(token->lex, "&&") == 0)
      return SYM_AND;

    if (strcmp(token->lex, "||") == 0)
      return SYM_OR;

    if (strcmp(token->lex, "==") == 0)
      return SYM_EQUAL;

    if (strcmp(token->lex, "!=") == 0)
      return SYM_NOT_EQUAL;

    if (strcmp(token->lex, "<") == 0)
      return SYM_LESS;

    if (strcmp(token->lex, "<=") == 0)
      return SYM_LESS_EQUAL;

    if (strcmp(token->lex, ">") == 0)
      return SYM_GREATER;

    if (strcmp(token->lex, ">=") == 0)
      return SYM_GREATER_EQUAL;

    break;

  case SEPARATOR:

    if (strcmp(token->lex, "(") == 0)
      return SYM_LPAREN;

    if (strcmp(token->lex, ")") == 0)
      return SYM_RPAREN;

    if (strcmp(token->lex, "{") == 0)
      return SYM_LBRACE;

    if (strcmp(token->lex, "}") == 0)
      return SYM_RBRACE;

    if (strcmp(token->lex, ";") == 0)
      return SYM_SEMICOLON;

    break;

  case LITERAL:

    if (token->lex[0] == '"')
      return SYM_STRING_LITERAL;

    if (token->lex[0] == '\'')
      return SYM_CHAR_LITERAL;

    break;

  default:
    break;
  }

  return SYM_EPSILON; /* ou SYM_INVALID */
}

int apply_production(ParseStack *stack, Production production) {
  switch (production) {
  case PROD_CODE_BLOCK_EOF:
    return parse_stack_push(stack, SYM_EOF) &&
           parse_stack_push(stack, SYM_BLOCK);

  case PROD_BLOCK_LBRACE_PROG_RBRACE:
    return parse_stack_push(stack, SYM_RBRACE) &&
           parse_stack_push(stack, SYM_PROG) &&
           parse_stack_push(stack, SYM_LBRACE);

  case PROD_PROG_DECL_PROG:
    return parse_stack_push(stack, SYM_PROG) &&
           parse_stack_push(stack, SYM_DECL);

  case PROD_PROG_EPSILON:
    return 1;

  case PROD_DECL_VAR_DECLARATION:
    return parse_stack_push(stack, SYM_VAR_DECLARATION);

  case PROD_DECL_ASSIGNMENT:
    return parse_stack_push(stack, SYM_ASSIGNMENT);

  case PROD_DECL_CONDITION:
    return parse_stack_push(stack, SYM_CONDITION);

  case PROD_DECL_WHILE_LOOP:
    return parse_stack_push(stack, SYM_WHILE_LOOP);

  case PROD_VAR_DECLARATION_TYPE_IDENTIFIER_VAR_DECLARATION_TAIL:
    return parse_stack_push(stack, SYM_VAR_DECLARATION_TAIL) &&
           parse_stack_push(stack, SYM_IDENTIFIER) &&
           parse_stack_push(stack, SYM_TYPE);

  case PROD_VAR_DECLARATION_TAIL_SEMICOLON:
    return parse_stack_push(stack, SYM_SEMICOLON);

  case PROD_VAR_DECLARATION_TAIL_ASSIGN_EXPR_SEMICOLON:
    return parse_stack_push(stack, SYM_SEMICOLON) &&
           parse_stack_push(stack, SYM_EXPR) &&
           parse_stack_push(stack, SYM_ASSIGN);

  case PROD_ASSIGNMENT_IDENTIFIER_ASSIGN_EXPR_SEMICOLON:
    return parse_stack_push(stack, SYM_SEMICOLON) &&
           parse_stack_push(stack, SYM_EXPR) &&
           parse_stack_push(stack, SYM_ASSIGN) &&
           parse_stack_push(stack, SYM_IDENTIFIER);

  case PROD_CONDITION_IF_LPAREN_EXPR_RPAREN_BLOCK_CONDITION_TAIL:
    return parse_stack_push(stack, SYM_CONDITION_TAIL) &&
           parse_stack_push(stack, SYM_BLOCK) &&
           parse_stack_push(stack, SYM_RPAREN) &&
           parse_stack_push(stack, SYM_EXPR) &&
           parse_stack_push(stack, SYM_LPAREN) &&
           parse_stack_push(stack, SYM_IF);

  case PROD_CONDITION_TAIL_ELSE_BLOCK:
    return parse_stack_push(stack, SYM_BLOCK) &&
           parse_stack_push(stack, SYM_ELSE);

  case PROD_CONDITION_TAIL_EPSILON:
    return 1;

  case PROD_WHILE_LOOP_WHILE_LPAREN_EXPR_RPAREN_BLOCK:
    return parse_stack_push(stack, SYM_BLOCK) &&
           parse_stack_push(stack, SYM_RPAREN) &&
           parse_stack_push(stack, SYM_EXPR) &&
           parse_stack_push(stack, SYM_LPAREN) &&
           parse_stack_push(stack, SYM_WHILE);

  case PROD_EXPR_LOGICAL_OR_EXPR:
    return parse_stack_push(stack, SYM_LOGICAL_OR_EXPR);

  case PROD_LOGICAL_OR_EXPR_LOGICAL_AND_EXPR_LOGICAL_OR_EXPR_TAIL:
    return parse_stack_push(stack, SYM_LOGICAL_OR_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_LOGICAL_AND_EXPR);

  case PROD_LOGICAL_OR_EXPR_TAIL_OR_LOGICAL_AND_EXPR_LOGICAL_OR_EXPR_TAIL:
    return parse_stack_push(stack, SYM_LOGICAL_OR_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_LOGICAL_AND_EXPR) &&
           parse_stack_push(stack, SYM_OR);

  case PROD_LOGICAL_OR_EXPR_TAIL_EPSILON:
    return 1;

  case PROD_LOGICAL_AND_EXPR_EQUALITY_EXPR_LOGICAL_AND_EXPR_TAIL:
    return parse_stack_push(stack, SYM_LOGICAL_AND_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_EQUALITY_EXPR);

  case PROD_LOGICAL_AND_EXPR_TAIL_AND_EQUALITY_EXPR_LOGICAL_AND_EXPR_TAIL:
    return parse_stack_push(stack, SYM_LOGICAL_AND_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_EQUALITY_EXPR) &&
           parse_stack_push(stack, SYM_AND);

  case PROD_LOGICAL_AND_EXPR_TAIL_EPSILON:
    return 1;

  case PROD_EQUALITY_EXPR_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
    return parse_stack_push(stack, SYM_EQUALITY_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_RELATIONAL_EXPR);

  case PROD_EQUALITY_EXPR_TAIL_EQUAL_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
    return parse_stack_push(stack, SYM_EQUALITY_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_RELATIONAL_EXPR) &&
           parse_stack_push(stack, SYM_EQUAL);

  case PROD_EQUALITY_EXPR_TAIL_NOT_EQUAL_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
    return parse_stack_push(stack, SYM_EQUALITY_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_RELATIONAL_EXPR) &&
           parse_stack_push(stack, SYM_NOT_EQUAL);

  case PROD_EQUALITY_EXPR_TAIL_EPSILON:
    return 1;

  case PROD_RELATIONAL_EXPR_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return parse_stack_push(stack, SYM_RELATIONAL_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_ADD_EXPR);

  case PROD_RELATIONAL_EXPR_TAIL_LESS_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return parse_stack_push(stack, SYM_RELATIONAL_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_ADD_EXPR) &&
           parse_stack_push(stack, SYM_LESS);

  case PROD_RELATIONAL_EXPR_TAIL_LESS_EQUAL_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return parse_stack_push(stack, SYM_RELATIONAL_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_ADD_EXPR) &&
           parse_stack_push(stack, SYM_LESS_EQUAL);

  case PROD_RELATIONAL_EXPR_TAIL_GREATER_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return parse_stack_push(stack, SYM_RELATIONAL_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_ADD_EXPR) &&
           parse_stack_push(stack, SYM_GREATER);

  case PROD_RELATIONAL_EXPR_TAIL_GREATER_EQUAL_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return parse_stack_push(stack, SYM_RELATIONAL_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_ADD_EXPR) &&
           parse_stack_push(stack, SYM_GREATER_EQUAL);

  case PROD_RELATIONAL_EXPR_TAIL_EPSILON:
    return 1;

  case PROD_ADD_EXPR_TERM_ADD_EXPR_TAIL:
    return parse_stack_push(stack, SYM_ADD_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_TERM);

  case PROD_ADD_EXPR_TAIL_PLUS_TERM_ADD_EXPR_TAIL:
    return parse_stack_push(stack, SYM_ADD_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_TERM) &&
           parse_stack_push(stack, SYM_PLUS);

  case PROD_ADD_EXPR_TAIL_MINUS_TERM_ADD_EXPR_TAIL:
    return parse_stack_push(stack, SYM_ADD_EXPR_TAIL) &&
           parse_stack_push(stack, SYM_TERM) &&
           parse_stack_push(stack, SYM_MINUS);

  case PROD_ADD_EXPR_TAIL_EPSILON:
    return 1;

  case PROD_TERM_FACTOR_TERM_TAIL:
    return parse_stack_push(stack, SYM_TERM_TAIL) &&
           parse_stack_push(stack, SYM_FACTOR);

  case PROD_TERM_TAIL_MULTIPLY_FACTOR_TERM_TAIL:
    return parse_stack_push(stack, SYM_TERM_TAIL) &&
           parse_stack_push(stack, SYM_FACTOR) &&
           parse_stack_push(stack, SYM_MULTIPLY);

  case PROD_TERM_TAIL_DIVIDE_FACTOR_TERM_TAIL:
    return parse_stack_push(stack, SYM_TERM_TAIL) &&
           parse_stack_push(stack, SYM_FACTOR) &&
           parse_stack_push(stack, SYM_DIVIDE);

  case PROD_TERM_TAIL_EPSILON:
    return 1;

  case PROD_FACTOR_IDENTIFIER:
    return parse_stack_push(stack, SYM_IDENTIFIER);

  case PROD_FACTOR_NUMBER:
    return parse_stack_push(stack, SYM_NUMBER);

  case PROD_FACTOR_STRING_LITERAL:
    return parse_stack_push(stack, SYM_STRING_LITERAL);

  case PROD_FACTOR_CHAR_LITERAL:
    return parse_stack_push(stack, SYM_CHAR_LITERAL);

  case PROD_FACTOR_LPAREN_EXPR_RPAREN:
    return parse_stack_push(stack, SYM_RPAREN) &&
           parse_stack_push(stack, SYM_EXPR) &&
           parse_stack_push(stack, SYM_LPAREN);

  case PROD_TYPE_INT:
    return parse_stack_push(stack, SYM_INT);

  case PROD_TYPE_FLOAT:
    return parse_stack_push(stack, SYM_FLOAT);

  case PROD_TYPE_CHAR:
    return parse_stack_push(stack, SYM_CHAR);

  case PROD_TYPE_STRING:
    return parse_stack_push(stack, SYM_STRING);

  default:
    return 0;
  }
}

