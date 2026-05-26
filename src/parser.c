#include "parser.h"
#include "pointer.h"

int parse_code(Parser *p) {
  next(p);
  if (!parse_block(p)) {
    return 0;
  }
  return consume(p, TOKEN_EOF);
}

int parse_block(Parser *p) {

  if (!consume_by_lex(p, "{")) {
    return 0;
  }
  if (!parse_prog(p))
    return 0;
  int s = consume_by_lex(p, "}"); 
  return s;
}

int parse_prog(Parser *p) {
  while (!check_by_lex(p, "}")) {
    if (!parse_decl(p))
      return 0;
  }

  return 1;
};

int parse_decl(Parser *p) {
  if (check(p, KEYWORD_TYPE)) {
    if (!parse_var_declaration(p))
      return 0;
  } else if (check(p, IDENTIFIER)) {
    if (!parse_assignment(p))
      return 0;
  } else if (check(p, KEYWORD_IF)) {
    if (!parse_condition(p))
      return 0;
  } else if (check(p, KEYWORD_WHILE)) {
    if (!parse_while_loop(p))
      return 0;
  } else {
    return 0;
  }
  return 1;
}

int parse_var_declaration(Parser *p) {
  if (!parse_type(p))
    return 0;
  if (!consume(p, IDENTIFIER))
    return 0;
  if (check_by_lex(p, "=")) {
    consume_by_lex(p, "=");
    if (!parse_expr(p))
      return 0;
  }
  consume_by_lex(p, ";");
  return 1;
};

int parse_condition(Parser *p) {

  if (!consume(p, KEYWORD_IF)) {
    return 0;
  }
  if (!consume_by_lex(p, "(")) {
    return 0;
  }
  parse_expr(p);
  if (!consume_by_lex(p, ")")) {
    return 0;
  }
  parse_block(p);
  if (consume(p, KEYWORD_ELSE)) {
    parse_block(p);
  }
  return 1;
};

int parse_while_loop(Parser *p) {

  if (!consume(p, KEYWORD_WHILE)) {
    return 0;
  }
  if (!consume_by_lex(p, "(")) {
    return 0;
  }
  parse_expr(p);
  if (!consume_by_lex(p, ")"))
    return 0;
  if (!parse_block(p)) return 0;
  return 1;
};

int parse_expr(Parser *p) {
  if (!parse_term(p))
    return 0;
  if (!parse_expr_tail(p))
    return 0;
  return 1;
}

int parse_expr_tail(Parser *p) {
  if (check_by_lex(p, "+") || check_by_lex(p, "-")) {
    consume_by_lex(p, p->current->lex);
    if (!parse_term(p))
      return 0;
    return parse_expr_tail(p);
  }

  return 1;
}

int parse_term(Parser *p) {
  if (!parse_factor(p))
    return 0;
  if (!parse_term_tail(p))
    return 0;
  return 1;
}

int parse_term_tail(Parser *p) {
  if (check_by_lex(p, "*") || check_by_lex(p, "/")) {
    consume_by_lex(p, p->current->lex);
    if (!parse_factor(p))
      return 0;
    return parse_term_tail(p);
  }
  return 1;
}

int parse_factor(Parser *p) {
  if (check(p, NUMBER)) {
    consume(p, NUMBER);
    return 1;
  } else if (check(p, IDENTIFIER)) {
    consume(p, IDENTIFIER);
    return 1;
  } else if (check_by_lex(p, "(")) {
    consume_by_lex(p, "(");
    if (!parse_expr(p))
      return 0;
    return consume_by_lex(p, ")");
  }
  return 0;
}

int parse_type(Parser *p) { return consume(p, KEYWORD_TYPE); }

int is_assignment_operator(Token *token) {
  if (token == NULL || token->type != OPERATOR_ASSIGNMENT) {
    return 0;
  }

  return strcmp(token->lex, "=") == 0 || strcmp(token->lex, "+=") == 0 ||
         strcmp(token->lex, "-=") == 0 || strcmp(token->lex, "*=") == 0 ||
         strcmp(token->lex, "/=") == 0 || strcmp(token->lex, "%=") == 0;
}

int parse_assignment(Parser *p) {
  if (!consume(p, IDENTIFIER)) {
    return 0;
  }

  if (!check(p, OPERATOR_ASSIGNMENT)) {
    return 0;
  }

  consume(p, OPERATOR_ASSIGNMENT);

  if (!parse_expr(p))
    return 0;

  return consume_by_lex(p, ";");
};

// int expression();

void next(Parser *p) {
  p->current = read_next_token(p->pointer, 0);
  // prt_token(p->current, 1);
}
int consume(Parser *p, TokenType type) {
  if (p->current->type != type) {
    return 0;
  }
  prt_token(p->current, 1);
  if (p->current->type == TOKEN_EOF) return 1;
  next(p);
  return 1;
}
int consume_by_lex(Parser *p, char *lex) {

  if (p == NULL || p->current == NULL || lex == NULL) {
    return 0;
  }

  if (strcmp(p->current->lex, lex) != 0) {
    return 0;
  }
  prt_token(p->current, 1);
  next(p);
  return 1;
}
int check_by_lex(Parser *p, char *lex) {

  if (p == NULL || p->current == NULL || lex == NULL) {
    return 0;
  }

  return strcmp(p->current->lex, lex) == 0;
}

int check(Parser *p, TokenType type) {
  if (p == NULL || p->current == NULL) {
    return 0;
  }
  return p->current->type == type;
}

int init_parser(Parser *p, Pointer *pointer) {
  p->current = pointer->token;
  p->pointer = pointer;
  return 1;
}

int parse_value(Parser *p) {
  if (check(p, IDENTIFIER))
    return consume(p, IDENTIFIER);

  if (check(p, NUMBER))
    return consume(p, NUMBER);

  if (check(p, LITERAL))
    return consume(p, LITERAL);

  return 0;
}