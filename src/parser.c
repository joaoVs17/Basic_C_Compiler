#include "parser.h"
#include "pointer.h"

ASTNode *parse_code(Parser *p) {
  next(p);
  ASTNode *program = create_ast(AST_PROGRAM, NULL);
  ASTNode *block = parse_block(p);

  if (program == NULL || block == NULL) {
    return NULL;
  }
  add_child(program, block);
  if (!consume(p, TOKEN_EOF)) {
    destroy_ast(program);
    return NULL;
  }

  return program;
}

ASTNode *parse_block(Parser *p) {
  Token *block_token = p->current; // { must be taken before consume

  if (!consume_by_lex(p, "{")) {
    return NULL;
  }
  ASTNode *block = create_ast(AST_BLOCK, block_token);

  if (block == NULL || !parse_prog(p, block)) {
    destroy_ast(block);
    return NULL;
  }

  if (!consume_by_lex(p, "}")) {
    destroy_ast(block);
    return NULL;
  }

  return block;
}

int parse_prog(Parser *p, ASTNode *parent) {
  while (!check_by_lex(p, "}")) {
    ASTNode *decl = parse_decl(p);

    if (decl == NULL) {
      // synchronize(p);
      continue;
    }

    if (!add_child(parent, decl)) {
      return 0;
    }
  }

  return 1;
};

ASTNode *parse_decl(Parser *p) {
  if (check(p, KEYWORD_TYPE)) {
    return parse_var_declaration(p);
  } else if (check(p, IDENTIFIER)) {
    return parse_assignment(p);
  } else if (check(p, KEYWORD_IF)) {
    return parse_condition(p);
  } else if (check(p, KEYWORD_WHILE)) {
    return parse_while_loop(p);
  } else {
    if (!p->pp) {
      add_simple_error(p, "INVALID DECLARATION");
    }
  }

  return NULL;
}

ASTNode *parse_var_declaration(Parser *p) {
  Token *type_token = parse_type(p); //
  if (type_token == NULL)
    return NULL;

  Token *identifier_token = p->current;
  if (!consume(p, IDENTIFIER))
    return NULL;

  ASTNode *declaration = create_ast(AST_DECLARATION, type_token);
  ASTNode *identifier = create_ast(AST_IDENTIFIER, identifier_token);

  if (declaration == NULL || identifier == NULL ||
      !add_child(declaration, identifier)) {
    destroy_ast(declaration);
    destroy_ast(identifier);
    return NULL;
  }

  if (check_by_lex(p, "=")) {
    Token *assignment_token = p->current;
    consume_by_lex(p, "=");
    ASTNode *assignment = create_ast(AST_ASSIGNMENT, assignment_token);
    ASTNode *expr = parse_expr(p);

    if (assignment == NULL || expr == NULL || !add_child(assignment, expr) ||
        !add_child(declaration, assignment)) {
      destroy_ast(assignment);
      destroy_ast(expr);
      destroy_ast(declaration);
      return NULL;
    }
  }
  if (!check_by_lex(p, ";")) {
    destroy_ast(declaration);
    return NULL;
  } else {
    consume_by_lex(p, ";");
  }
  return declaration;
};

ASTNode *parse_condition(Parser *p) {
  Token *if_token = p->current;

  if (!consume(p, KEYWORD_IF)) {
    return NULL;
  }
  if (!consume_by_lex(p, "(")) {
    return NULL;
  }

  ASTNode *condition = parse_expr(p);

  if (condition == NULL)
    return NULL;

  if (!consume_by_lex(p, ")")) {
    destroy_ast(condition);
    return NULL;
  }

  ASTNode *then_block = parse_block(p);

  if (then_block == NULL) {
    destroy_ast(condition);
    return NULL;
  }

  ASTNode *node = create_ast(AST_IF, if_token);

  if (node == NULL || !add_child(node, condition) ||
      !add_child(node, then_block)) {
    destroy_ast(node);
    destroy_ast(condition);
    destroy_ast(then_block);
    return NULL;
  }

  if (check(p, KEYWORD_ELSE)) {
    consume(p, KEYWORD_ELSE);
    ASTNode *else_block = parse_block(p);

    if (else_block == NULL || !add_child(node, else_block)) {
      destroy_ast(else_block);
      destroy_ast(node);
      return NULL;
    }
  }

  return node;
};

ASTNode *parse_while_loop(Parser *p) {
  Token *while_token = p->current;

  if (!consume(p, KEYWORD_WHILE)) {
    return NULL;
  }
  if (!consume_by_lex(p, "(")) {
    return NULL;
  }

  ASTNode *condition = parse_expr(p);

  if (condition == NULL)
    return NULL;

  if (!consume_by_lex(p, ")")) {
    destroy_ast(condition);
    return NULL;
  }

  ASTNode *block = parse_block(p);
  ASTNode *node = create_ast(AST_WHILE, while_token);

  if (block == NULL || node == NULL || !add_child(node, condition) ||
      !add_child(node, block)) {
    destroy_ast(condition);
    destroy_ast(block);
    destroy_ast(node);
    return NULL;
  }

  return node;
};

ASTNode *parse_expr(Parser *p) { return parse_logical_and_expr(p); }

ASTNode *parse_logical_and_expr(Parser *p) {
  ASTNode *left = parse_logical_or_expr(p);

  if (left == NULL)
    return NULL;

  return parse_logical_and_expr_tail(p, left);
}

ASTNode *parse_logical_or_expr(Parser *p) {
  ASTNode *left = parse_equality_expr(p);

  if (left == NULL)
    return NULL;
  return parse_logical_or_expr_tail(p, left);
}

ASTNode *parse_logical_or_expr_tail(Parser *p, ASTNode *left) {
  if (check_by_lex(p, "||")) {
    Token *operator_token = p->current;
    consume_by_lex(p, "||");
    ASTNode *right = parse_equality_expr(p);
    ASTNode *expr = create_ast(AST_BINARY_EXPR, operator_token);
    if (right == NULL || expr == NULL || !add_child(expr, left) ||
        !add_child(expr, right)) {
      return NULL;
    }
    return parse_logical_or_expr_tail(p, expr);
  }
}

ASTNode *parse_logical_and_expr_tail(Parser *p, ASTNode *left) {
  if (check_by_lex(p, "&&")) {
    Token *operator_token = p->current;
    consume_by_lex(p, "&&");
    ASTNode *right = parse_logical_or_expr(p);
    ASTNode *expr = create_ast(AST_BINARY_EXPR, operator_token);
    if (right == NULL || expr == NULL || !add_child(expr, left) ||
        !add_child(expr, right)) {
      return NULL;
    }
    return parse_logical_and_expr_tail(p, expr);
  }
  return left;
}

ASTNode *parse_equality_expr(Parser *p) {
  ASTNode *left = parse_relational_expr(p);

  if (left == NULL)
    return NULL;

  return parse_equality_expr_tail(p, left);
}

ASTNode *parse_equality_expr_tail(Parser *p, ASTNode *left) {

  if (check_by_lex(p, "==") || check_by_lex(p, "!=")) {

    Token *operator_token = p->current;

    consume_by_lex(p, p->current->lex);

    ASTNode *right = parse_relational_expr(p);
    ASTNode *expr = create_ast(AST_BINARY_EXPR, operator_token);

    if (right == NULL || expr == NULL || !add_child(expr, left) ||
        !add_child(expr, right)) {
      return NULL;
    }

    return parse_equality_expr_tail(p, expr);
  }

  return left;
}

ASTNode *parse_relational_expr(Parser *p) {
  ASTNode *left = parse_add_expr(p);

  if (left == NULL)
    return NULL;

  return parse_relational_expr_tail(p, left);
}

ASTNode *parse_relational_expr_tail(Parser *p, ASTNode *left) {

  if (check_by_lex(p, "<") || check_by_lex(p, "<=") || check_by_lex(p, ">") ||
      check_by_lex(p, ">=")) {

    Token *operator_token = p->current;

    consume_by_lex(p, p->current->lex);

    ASTNode *right = parse_add_expr(p);
    ASTNode *expr = create_ast(AST_BINARY_EXPR, operator_token);

    if (right == NULL || expr == NULL || !add_child(expr, left) ||
        !add_child(expr, right)) {
      return NULL;
    }

    return parse_relational_expr_tail(p, expr);
  }

  return left;
}

ASTNode *parse_add_expr(Parser *p) {
  ASTNode *left = parse_term(p);

  if (left == NULL)
    return NULL;

  return parse_add_expr_tail(p, left);
}

ASTNode *parse_add_expr_tail(Parser *p, ASTNode *left) {
  if (check_by_lex(p, "+") || check_by_lex(p, "-")) {
    Token *operator_token = p->current;
    consume_by_lex(p, p->current->lex);
    ASTNode *right = parse_term(p);
    ASTNode *expr = create_ast(AST_BINARY_EXPR, operator_token);

    if (right == NULL || expr == NULL || !add_child(expr, left) ||
        !add_child(expr, right)) {
      return NULL;
    }

    return parse_add_expr_tail(p, expr);
  }

  return left;
}

ASTNode *parse_term(Parser *p) {
  ASTNode *left = parse_factor(p);

  if (left == NULL)
    return NULL;

  return parse_term_tail(p, left);
}

ASTNode *parse_term_tail(Parser *p, ASTNode *left) {
  if (check_by_lex(p, "*") || check_by_lex(p, "/")) {
    Token *operator_token = p->current;
    consume_by_lex(p, p->current->lex);
    ASTNode *right = parse_factor(p);
    ASTNode *expr = create_ast(AST_BINARY_EXPR, operator_token);

    if (right == NULL || expr == NULL || !add_child(expr, left) ||
        !add_child(expr, right)) {
      destroy_ast(right);
      destroy_ast(expr);
      destroy_ast(left);
      return NULL;
    }

    return parse_term_tail(p, expr);
  }
  return left;
}

ASTNode *parse_factor(Parser *p) {
  if (check(p, NUMBER)) {
    Token *token = p->current;
    consume(p, NUMBER);
    return create_ast(AST_NUMBER_LITERAL, token);
  } else if (check(p, IDENTIFIER)) {
    Token *token = p->current;
    consume(p, IDENTIFIER);
    return create_ast(AST_IDENTIFIER, token);
  } else if (check(p, LITERAL)) {
    Token *token = p->current;
    consume(p, LITERAL);
    return create_ast(AST_STRING_LITERAL, token);
  } else if (check_by_lex(p, "(")) {
    consume_by_lex(p, "(");
    ASTNode *expr = parse_expr(p);

    if (expr == NULL || !consume_by_lex(p, ")")) {
      destroy_ast(expr);
      return NULL;
    }

    return expr;
  }
  return NULL;
}

Token *parse_type(Parser *p) {
  Token *token = p->current;

  if (!consume(p, KEYWORD_TYPE))
    return NULL;

  return token;
}

int is_assignment_operator(Token *token) {
  if (token == NULL || token->type != OPERATOR_ASSIGNMENT) {
    return 0;
  }

  return strcmp(token->lex, "=") == 0 || strcmp(token->lex, "+=") == 0 ||
         strcmp(token->lex, "-=") == 0 || strcmp(token->lex, "*=") == 0 ||
         strcmp(token->lex, "/=") == 0 || strcmp(token->lex, "%=") == 0;
}

ASTNode *parse_assignment(Parser *p) {
  Token *identifier_token = p->current;

  if (!consume(p, IDENTIFIER)) {
    return NULL;
  }

  if (!check(p, OPERATOR_ASSIGNMENT)) {
    return NULL;
  }

  Token *assignment_token = p->current;
  consume(p, OPERATOR_ASSIGNMENT);

  ASTNode *expr = parse_expr(p);

  if (expr == NULL)
    return NULL;

  if (!consume_by_lex(p, ";")) {
    destroy_ast(expr);
    return NULL;
  }

  ASTNode *assignment = create_ast(AST_ASSIGNMENT, assignment_token);
  ASTNode *identifier = create_ast(AST_IDENTIFIER, identifier_token);

  if (assignment == NULL || identifier == NULL ||
      !add_child(assignment, identifier) || !add_child(assignment, expr)) {
    destroy_ast(assignment);
    destroy_ast(identifier);
    destroy_ast(expr);
    return NULL;
  }

  return assignment;
};

// int expression();

void next(Parser *p) {
  p->current = read_next_token(p->pointer, 0);
  // prt_token(p->current, 1);
}
int consume(Parser *p, TokenType type) {
  if (p->current->type != type) {
    if (!p->pp) {
      add_simple_error(p, "UNEXPECTED TOKEN");
    }
    return 0;
  }
  prt_token(p->current, 1);
  if (p->current->type == TOKEN_EOF)
    return 1;
  next(p);
  return 1;
}
int consume_by_lex(Parser *p, char *lex) {

  if (p == NULL || p->current == NULL || lex == NULL) {

    return 0;
  }

  if (strcmp(p->current->lex, lex) != 0) {
    if (!p->pp) {
      add_simple_error(p, "UNEXPECTED TOKEN");
    }
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

  // Errors
  p->error_count = 0;
  p->error_capacity = 16;

  p->errors = malloc(sizeof(ParserError) * p->error_capacity);

  p->pp = 0;
  return 1;
}

ASTNode *parse_value(Parser *p) { return parse_factor(p); }

void add_parser_error(Parser *p, char *message, Token *t, TokenType expected) {
  if (p == NULL || p->current == NULL || message == NULL)
    return;

  ParserError *err = &p->errors[p->error_count];

  err->msg = malloc(strlen(message) + 1);

  if (err->msg != NULL) {
    strcpy(err->msg, message);
  }

  err->expected_type = expected;

  err->fnd = p->current;
  p->pp = 1;

  p->error_count += 1;
}

void add_parser_error_lex(Parser *p, char *message, Token *t,
                          char *expected_lex) {
  if (p == NULL || p->current == NULL || message == NULL)
    return;

  ParserError *err = &p->errors[p->error_count];

  err->msg = malloc(strlen(message) + 1);

  if (err->msg != NULL) {
    strcpy(err->msg, message);
  }

  err->expected_lex = malloc(strlen(expected_lex) + 1);
  if (err->expected_lex != NULL) {
    strcpy(err->expected_lex, expected_lex);
  }

  err->fnd = p->current;
  p->pp = 1;
  p->error_count += 1;
}

void synchronize(Parser *p) {
  if (p == NULL)
    return;
  p->pp = 0;
  while (!check(p, TOKEN_EOF)) {
    if (get_last_error(p).fnd->type == KEYWORD_ELSE) {
      if (!check_by_lex(p, "}")) {
        next(p);
        return;
      }
    }
    if (check_by_lex(p, ";")) {
      next(p);
      return;
    }
    if (check_by_lex(p, "}")) {
      next(p);
      return;
    }
    if (check(p, KEYWORD_IF) || check(p, KEYWORD_WHILE) ||
        check(p, KEYWORD_TYPE)) {
      printf("RECOVERED row: %d col: %d\n ", p->current->row, p->current->col);
      return;
    }
    next(p);
  }
}

void prt_parser_errors(Parser *p) {
  printf("PARSER ERRORS? HMMM \n");
  for (int i = 0; i < p->error_count; i++) {

    ParserError *err = &p->errors[i];

    printf("[Row: %d, Col: %d] Error: %s. Found '%s'\n", err->fnd->row,
           err->fnd->col, err->msg, err->fnd->lex);
  }
}

ParserError get_last_error(Parser *p) { return p->errors[p->error_count - 1]; }

void add_simple_error(Parser *p, char *message) {
  if (p == NULL || p->current == NULL || message == NULL)
    return;

  // printf("ERRR %s\n", p->current->lex);

  ParserError *err = &p->errors[p->error_count];

  err->msg = malloc(strlen(message) + 1);

  if (err->msg != NULL) {
    strcpy(err->msg, message);
  }

  err->fnd = p->current;
  p->pp = 1;
  p->error_count += 1;
}