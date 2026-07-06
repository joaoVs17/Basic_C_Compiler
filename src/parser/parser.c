#include "parser.h"

ASTNode *ast;
ASTNode *current_node;

int init_parser(Parser *parser, Pointer *pointer) {
  if (parser == NULL || pointer == NULL)
    return 0;

  parser->p = pointer;
  parser->t = NULL;
  return 1;
}

int parse(Parser *p, ParseStack *stack) {
  // symbol_stack_push(stack, SYM_EOF);
  symbol_stack_push(stack, SYM_CODE);
  p->ast = create_ast(AST_CODE, NULL);
  current_node = p->ast;
  next(p);
  NodeStack * node_stack = (NodeStack * ) malloc(sizeof(NodeStack));
  node_stack_init(node_stack);
  node_stack_push(node_stack, p->ast);
  int count = 0;
  while (!symbol_stack_is_empty(stack)) {
    // if (count >= 2) return 0;
    GrammarSymbol top;
    Token *current_token = p->t;
    symbol_stack_print(stack);
    symbol_stack_pop(stack, &top);

    // prt_token(current_token, 1);

    if (is_terminal(top)) {
      if (!handle_terminal_top(top, current_token, p))
        return 0;
    } else {

      if (!handle_non_terminal_top(top, current_token, stack, node_stack))
        return 0;
      // GrammarSymbol lookahead = token_to_symbol(current_token);
      // Production production = ll1_lookup(top, lookahead);

      // if (production == PROD_NONE) {
      //   syntax_error("Production == PROD_NONE");
      //   return 0;
      // } else {
      //   if (!apply_production(stack, production))
      //     return 0;
      // }
    }
    count++;
  }
  printf("================================================\n");
  printf("AST PRINTED ====================================\n");
  printf("================================================\n\n");

  prt_ast(p->ast, 0);
  printf("\n================================================\n");
  printf("END AST ========================================\n");
  printf("================================================\n");
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

int handle_non_terminal_top(GrammarSymbol top, Token *current_token, ParseStack *stack, NodeStack * node_stack) {
  GrammarSymbol lookahead = token_to_symbol(current_token);
  Production production = ll1_lookup(top, lookahead);

  if (production == PROD_NONE) {
    syntax_error("Production == PROD_NONE");
    return 0;
  } else {
    if (!apply_production(stack, node_stack, production))
      return 0;
    handle_ast_construct(production, top, current_token, node_stack);
  }
  return 1;
}

int handle_ast_construct(Production applied_produciton, GrammarSymbol unstacked_symbol, Token *current_token, NodeStack * node_stack) {
  switch (applied_produciton) {
  case PROD_CODE_BLOCK_EOF:
    ASTNode *node = create_ast(AST_PROGRAM, current_token);
    add_child(current_node, node);
    break;
  case PROD_BLOCK_LBRACE_PROG_RBRACE:
          
  break;

  default:
    break;
  }
  return 1;
}

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

int syntax_error(char *c) {
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

  return SYM_EPSILON; // ou SYM_INVALID 
}

static ASTNode *stack_node(NodeStack *node_stack, ASTNodeType type) {
  ASTNode *node = create_ast(type, NULL);
  node_stack_push(node_stack, node);
  return node;
}

int apply_production(ParseStack *stack, NodeStack * node_stack, Production production) {
  switch (production) {
  case PROD_CODE_BLOCK_EOF:
    return symbol_stack_push_many(stack, 2, SYM_EOF, SYM_BLOCK);
    break;

  case PROD_BLOCK_LBRACE_PROG_RBRACE:
    stack_node(node_stack, AST_BLOCK);
    return symbol_stack_push_many(stack, 3, SYM_RBRACE, SYM_PROG, SYM_LBRACE);
    break;

  case PROD_PROG_DECL_PROG:
    return symbol_stack_push_many(stack, 2, SYM_PROG, SYM_DECL);
    break;

  case PROD_PROG_EPSILON:
    return 1;
    break;

  case PROD_DECL_VAR_DECLARATION:
    return symbol_stack_push_many(stack, 1, SYM_VAR_DECLARATION);
    break;

  case PROD_DECL_ASSIGNMENT:
    return symbol_stack_push_many(stack, 1, SYM_ASSIGNMENT);
    break;

  case PROD_DECL_CONDITION:
    return symbol_stack_push_many(stack, 1, SYM_CONDITION);
    break;

  case PROD_DECL_WHILE_LOOP:
    return symbol_stack_push_many(stack, 1, SYM_WHILE_LOOP);
    break;

  case PROD_VAR_DECLARATION_TYPE_IDENTIFIER_VAR_DECLARATION_TAIL:
    stack_node(node_stack, AST_VAR_DECLARATION);
    return symbol_stack_push_many(stack, 3, SYM_VAR_DECLARATION_TAIL,
                                 SYM_IDENTIFIER, SYM_TYPE);
    break;

  case PROD_VAR_DECLARATION_TAIL_SEMICOLON:
    return symbol_stack_push_many(stack, 1, SYM_SEMICOLON);
    break;

  case PROD_VAR_DECLARATION_TAIL_ASSIGN_EXPR_SEMICOLON:
    return symbol_stack_push_many(stack, 3, SYM_SEMICOLON, SYM_EXPR, SYM_ASSIGN);
    break;

  case PROD_ASSIGNMENT_IDENTIFIER_ASSIGN_EXPR_SEMICOLON:
    stack_node(node_stack, AST_ASSIGNMENT);
    return symbol_stack_push_many(stack, 4, SYM_SEMICOLON, SYM_EXPR, SYM_ASSIGN,
                                 SYM_IDENTIFIER);
    break;

  case PROD_CONDITION_IF_LPAREN_EXPR_RPAREN_BLOCK_CONDITION_TAIL:
    stack_node(node_stack, AST_IF);
    return symbol_stack_push_many(stack, 6, SYM_CONDITION_TAIL, SYM_BLOCK,
                                 SYM_RPAREN, SYM_EXPR, SYM_LPAREN, SYM_IF);
    break;

  case PROD_CONDITION_TAIL_ELSE_BLOCK:
    return symbol_stack_push_many(stack, 2, SYM_BLOCK, SYM_ELSE);
    break;

  case PROD_CONDITION_TAIL_EPSILON:
    return 1;
    break;

  case PROD_WHILE_LOOP_WHILE_LPAREN_EXPR_RPAREN_BLOCK:
    stack_node(node_stack, AST_WHILE);
    return symbol_stack_push_many(stack, 5, SYM_BLOCK, SYM_RPAREN, SYM_EXPR,
                                 SYM_LPAREN, SYM_WHILE);
    break;

  case PROD_EXPR_LOGICAL_OR_EXPR:
    return symbol_stack_push_many(stack, 1, SYM_LOGICAL_OR_EXPR);
    break;

  case PROD_LOGICAL_OR_EXPR_LOGICAL_AND_EXPR_LOGICAL_OR_EXPR_TAIL:
    return symbol_stack_push_many(stack, 2, SYM_LOGICAL_OR_EXPR_TAIL,
                                 SYM_LOGICAL_AND_EXPR);
    break;

  case PROD_LOGICAL_OR_EXPR_TAIL_OR_LOGICAL_AND_EXPR_LOGICAL_OR_EXPR_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_LOGICAL_OR_EXPR_TAIL,
                                 SYM_LOGICAL_AND_EXPR, SYM_OR);
    break;

  case PROD_LOGICAL_OR_EXPR_TAIL_EPSILON:
    return 1;
    break;

  case PROD_LOGICAL_AND_EXPR_EQUALITY_EXPR_LOGICAL_AND_EXPR_TAIL:
    return symbol_stack_push_many(stack, 2, SYM_LOGICAL_AND_EXPR_TAIL,
                                 SYM_EQUALITY_EXPR);
    break;

  case PROD_LOGICAL_AND_EXPR_TAIL_AND_EQUALITY_EXPR_LOGICAL_AND_EXPR_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_LOGICAL_AND_EXPR_TAIL,
                                 SYM_EQUALITY_EXPR, SYM_AND);
    break;

  case PROD_LOGICAL_AND_EXPR_TAIL_EPSILON:
    return 1;
    break;

  case PROD_EQUALITY_EXPR_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
    return symbol_stack_push_many(stack, 2, SYM_EQUALITY_EXPR_TAIL,
                                 SYM_RELATIONAL_EXPR);
    break;

  case PROD_EQUALITY_EXPR_TAIL_EQUAL_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_EQUALITY_EXPR_TAIL,
                                 SYM_RELATIONAL_EXPR, SYM_EQUAL);
    break;

  case PROD_EQUALITY_EXPR_TAIL_NOT_EQUAL_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_EQUALITY_EXPR_TAIL,
                                 SYM_RELATIONAL_EXPR, SYM_NOT_EQUAL);
    break;

  case PROD_EQUALITY_EXPR_TAIL_EPSILON:
    return 1;
    break;

  case PROD_RELATIONAL_EXPR_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return symbol_stack_push_many(stack, 2, SYM_RELATIONAL_EXPR_TAIL,
                                 SYM_ADD_EXPR);
    break;

  case PROD_RELATIONAL_EXPR_TAIL_LESS_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_RELATIONAL_EXPR_TAIL,
                                 SYM_ADD_EXPR, SYM_LESS);
    break;

  case PROD_RELATIONAL_EXPR_TAIL_LESS_EQUAL_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_RELATIONAL_EXPR_TAIL,
                                 SYM_ADD_EXPR, SYM_LESS_EQUAL);
    break;

  case PROD_RELATIONAL_EXPR_TAIL_GREATER_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_RELATIONAL_EXPR_TAIL,
                                 SYM_ADD_EXPR, SYM_GREATER);
    break;

  case PROD_RELATIONAL_EXPR_TAIL_GREATER_EQUAL_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_RELATIONAL_EXPR_TAIL,
                                 SYM_ADD_EXPR, SYM_GREATER_EQUAL);
    break;

  case PROD_RELATIONAL_EXPR_TAIL_EPSILON:
    return 1;
    break;

  case PROD_ADD_EXPR_TERM_ADD_EXPR_TAIL:
    return symbol_stack_push_many(stack, 2, SYM_ADD_EXPR_TAIL, SYM_TERM);
    break;

  case PROD_ADD_EXPR_TAIL_PLUS_TERM_ADD_EXPR_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_ADD_EXPR_TAIL, SYM_TERM,
                                 SYM_PLUS);
    break;

  case PROD_ADD_EXPR_TAIL_MINUS_TERM_ADD_EXPR_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_ADD_EXPR_TAIL, SYM_TERM,
                                 SYM_MINUS);
    break;

  case PROD_ADD_EXPR_TAIL_EPSILON:
    return 1;
    break;

  case PROD_TERM_FACTOR_TERM_TAIL:
    return symbol_stack_push_many(stack, 2, SYM_TERM_TAIL, SYM_FACTOR);
    break;

  case PROD_TERM_TAIL_MULTIPLY_FACTOR_TERM_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_TERM_TAIL, SYM_FACTOR,
                                 SYM_MULTIPLY);
    break;

  case PROD_TERM_TAIL_DIVIDE_FACTOR_TERM_TAIL:
    return symbol_stack_push_many(stack, 3, SYM_TERM_TAIL, SYM_FACTOR,
                                 SYM_DIVIDE);
    break;

  case PROD_TERM_TAIL_EPSILON:
    return 1;
    break;

  case PROD_FACTOR_IDENTIFIER:
    return symbol_stack_push_many(stack, 1, SYM_IDENTIFIER);
    break;

  case PROD_FACTOR_NUMBER:
    return symbol_stack_push_many(stack, 1, SYM_NUMBER);
    break;

  case PROD_FACTOR_STRING_LITERAL:
    return symbol_stack_push_many(stack, 1, SYM_STRING_LITERAL);
    break;

  case PROD_FACTOR_CHAR_LITERAL:
    return symbol_stack_push_many(stack, 1, SYM_CHAR_LITERAL);
    break;

  case PROD_FACTOR_LPAREN_EXPR_RPAREN:
    return symbol_stack_push_many(stack, 3, SYM_RPAREN, SYM_EXPR, SYM_LPAREN);
    break;

  case PROD_TYPE_INT:
    return symbol_stack_push_many(stack, 1, SYM_INT);
    break;

  case PROD_TYPE_FLOAT:
    return symbol_stack_push_many(stack, 1, SYM_FLOAT);
    break;

  case PROD_TYPE_CHAR:
    return symbol_stack_push_many(stack, 1, SYM_CHAR);
    break;

  case PROD_TYPE_STRING:
    return symbol_stack_push_many(stack, 1, SYM_STRING);
    break;

  default:
    return 0;
    break;
  }
}

GrammarSymbol production_non_terminal(Production production) {
  switch (production) {
  case PROD_CODE_BLOCK_EOF:
    return SYM_CODE;

  case PROD_BLOCK_LBRACE_PROG_RBRACE:
    return SYM_BLOCK;

  case PROD_PROG_DECL_PROG:
  case PROD_PROG_EPSILON:
    return SYM_PROG;

  case PROD_DECL_VAR_DECLARATION:
  case PROD_DECL_ASSIGNMENT:
  case PROD_DECL_CONDITION:
  case PROD_DECL_WHILE_LOOP:
    return SYM_DECL;

  case PROD_VAR_DECLARATION_TYPE_IDENTIFIER_VAR_DECLARATION_TAIL:
    return SYM_VAR_DECLARATION;

  case PROD_VAR_DECLARATION_TAIL_SEMICOLON:
  case PROD_VAR_DECLARATION_TAIL_ASSIGN_EXPR_SEMICOLON:
    return SYM_VAR_DECLARATION_TAIL;

  case PROD_ASSIGNMENT_IDENTIFIER_ASSIGN_EXPR_SEMICOLON:
    return SYM_ASSIGNMENT;

  case PROD_CONDITION_IF_LPAREN_EXPR_RPAREN_BLOCK_CONDITION_TAIL:
    return SYM_CONDITION;

  case PROD_CONDITION_TAIL_ELSE_BLOCK:
  case PROD_CONDITION_TAIL_EPSILON:
    return SYM_CONDITION_TAIL;

  case PROD_WHILE_LOOP_WHILE_LPAREN_EXPR_RPAREN_BLOCK:
    return SYM_WHILE_LOOP;

  case PROD_EXPR_LOGICAL_OR_EXPR:
    return SYM_EXPR;

  case PROD_LOGICAL_OR_EXPR_LOGICAL_AND_EXPR_LOGICAL_OR_EXPR_TAIL:
    return SYM_LOGICAL_OR_EXPR;

  case PROD_LOGICAL_OR_EXPR_TAIL_OR_LOGICAL_AND_EXPR_LOGICAL_OR_EXPR_TAIL:
  case PROD_LOGICAL_OR_EXPR_TAIL_EPSILON:
    return SYM_LOGICAL_OR_EXPR_TAIL;

  case PROD_LOGICAL_AND_EXPR_EQUALITY_EXPR_LOGICAL_AND_EXPR_TAIL:
    return SYM_LOGICAL_AND_EXPR;

  case PROD_LOGICAL_AND_EXPR_TAIL_AND_EQUALITY_EXPR_LOGICAL_AND_EXPR_TAIL:
  case PROD_LOGICAL_AND_EXPR_TAIL_EPSILON:
    return SYM_LOGICAL_AND_EXPR_TAIL;

  case PROD_EQUALITY_EXPR_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
    return SYM_EQUALITY_EXPR;

  case PROD_EQUALITY_EXPR_TAIL_EQUAL_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
  case PROD_EQUALITY_EXPR_TAIL_NOT_EQUAL_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
  case PROD_EQUALITY_EXPR_TAIL_EPSILON:
    return SYM_EQUALITY_EXPR_TAIL;

  case PROD_RELATIONAL_EXPR_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return SYM_RELATIONAL_EXPR;

  case PROD_RELATIONAL_EXPR_TAIL_LESS_ADD_EXPR_RELATIONAL_EXPR_TAIL:
  case PROD_RELATIONAL_EXPR_TAIL_LESS_EQUAL_ADD_EXPR_RELATIONAL_EXPR_TAIL:
  case PROD_RELATIONAL_EXPR_TAIL_GREATER_ADD_EXPR_RELATIONAL_EXPR_TAIL:
  case PROD_RELATIONAL_EXPR_TAIL_GREATER_EQUAL_ADD_EXPR_RELATIONAL_EXPR_TAIL:
  case PROD_RELATIONAL_EXPR_TAIL_EPSILON:
    return SYM_RELATIONAL_EXPR_TAIL;

  case PROD_ADD_EXPR_TERM_ADD_EXPR_TAIL:
    return SYM_ADD_EXPR;

  case PROD_ADD_EXPR_TAIL_PLUS_TERM_ADD_EXPR_TAIL:
  case PROD_ADD_EXPR_TAIL_MINUS_TERM_ADD_EXPR_TAIL:
  case PROD_ADD_EXPR_TAIL_EPSILON:
    return SYM_ADD_EXPR_TAIL;

  case PROD_TERM_FACTOR_TERM_TAIL:
    return SYM_TERM;

  case PROD_TERM_TAIL_MULTIPLY_FACTOR_TERM_TAIL:
  case PROD_TERM_TAIL_DIVIDE_FACTOR_TERM_TAIL:
  case PROD_TERM_TAIL_EPSILON:
    return SYM_TERM_TAIL;

  case PROD_FACTOR_IDENTIFIER:
  case PROD_FACTOR_NUMBER:
  case PROD_FACTOR_STRING_LITERAL:
  case PROD_FACTOR_CHAR_LITERAL:
  case PROD_FACTOR_LPAREN_EXPR_RPAREN:
    return SYM_FACTOR;

  case PROD_TYPE_INT:
  case PROD_TYPE_FLOAT:
  case PROD_TYPE_CHAR:
  case PROD_TYPE_STRING:
    return SYM_TYPE;

  default:
    return SYM_EPSILON;
  }
}
