#include "parser.h"

int init_parser(Parser *parser, Pointer *pointer) {
  if (parser == NULL || pointer == NULL)
    return 0;

  parser->p = pointer;
  parser->t = NULL;
  parser->ast = create_ast(AST_CODE, NULL);
  return 1;
}

int parse(Parser *p, ParseStack *stack) {

  // declaracções de fora do loop
  StackItem *current_item = (StackItem *)malloc(sizeof(StackItem));
  NodeStack *node_stack = (NodeStack *)malloc(sizeof(NodeStack));
  Token *current_token;

  // Iniciar a nodestack
  node_stack_init(node_stack);

  // Push na node stack. Só a raiz

  // Primeiro push. só pra iniciar a pilha com alguma coisa
  parse_stack_push_symbol(stack, SYM_EOF);
  parse_stack_push_symbol(stack, SYM_CODE);

  // Pedir o primeiro token para o lexer
  next(p);

  while (!parse_stack_is_empty(stack)) {
    // parse_stack_print(stack);
    // printf("\n\n");
    current_token = p->t;
    parse_stack_pop(stack, current_item);

    if (current_item->type == ITEM_SYMBOL) {
      if (is_terminal(current_item->symbol)) {
        if (!handle_terminal_top(current_item, current_token, p, node_stack))
          return 0;
      } else {
        if (!handle_non_terminal_top(current_item, current_token, stack, node_stack))
          return 0;
      }
    } else if (current_item->type == ITEM_ACTION) {
      if (!handle_action(current_item, node_stack))
        return 0;
    }
  }
  ASTNode *root;
  if (node_stack_pop(node_stack, &root))
    add_child(p->ast, root);
  printf("================================================\n");
  printf("AST PRINTED ====================================\n");
  printf("================================================\n\n");
  prt_ast(p->ast, 0);
  printf("\n================================================\n");
  printf("END AST ========================================\n");
  printf("================================================\n");
  return 1;
}

int handle_action(StackItem *item, NodeStack *node_stack) {
  // para cada ação tem um número de filhos esperado.
  // Ex: um if else significa experar um
  switch (item->action) {
  case ACT_BUILD_BINARY:
    return node_stack_reduce(node_stack, AST_BINARY_EXPR, item->token, 2);
  case ACT_BUILD_ASSIGN:
    return node_stack_reduce(node_stack, AST_ASSIGNMENT, item->token, 2);
  case ACT_BUILD_VARDECL:
    return node_stack_reduce(node_stack, AST_VAR_DECLARATION, item->token, 2);
  case ACT_BUILD_VARDECL_INIT:
    return node_stack_reduce(node_stack, AST_VAR_DECLARATION, item->token, 3);
  case ACT_BUILD_IF:
    return node_stack_reduce(node_stack, AST_IF, item->token, 2);
  case ACT_BUILD_IF_ELSE:
    return node_stack_reduce(node_stack, AST_IF, item->token, 3);
  case ACT_BUILD_WHILE:
    return node_stack_reduce(node_stack, AST_WHILE, item->token, 2);
  case ACT_OPEN_BLOCK:
    return node_stack_push(node_stack, NULL);
  case ACT_BUILD_BLOCK:
    return build_block(node_stack);
  default:
    return 0;
  }
}

int build_block(NodeStack *node_stack) {
  ASTNode *block = create_ast(AST_BLOCK, NULL);
  ASTNode *tmp[128]; // O block só vai poder ter até 128 filhos. Ver se compensa implementar uma lista com crescimento
  int n = 0;
  ASTNode *top;
  while (node_stack_pop(node_stack, &top) && top != NULL) {
    // Quando top for NULL chegou no ACT_OPEN_BLOCK. Terminou de escrever o bloco
    tmp[n] = top;
    n++;
  }
  for (int i = n - 1; i >= 0; i--) {
    // Os itens desempilhados estão de trás pra frente.
    // o primeiro desempilhado é na prática o último que deveria ser escrito
    add_child(block, tmp[i]);
  }
  return node_stack_push(node_stack, block);
}

int handle_terminal_top(StackItem *current_item, Token *current_token, Parser *parser, NodeStack *node_stack) {
  if (matches(current_item->symbol, current_token)) {
    switch (current_item->symbol) {
    case SYM_IDENTIFIER:
      push_leaf(node_stack, AST_IDENTIFIER, current_token);
      break;
    case SYM_NUMBER:
      push_leaf(node_stack, AST_NUMBER_LITERAL, current_token);
      break;
    case SYM_STRING_LITERAL:
      push_leaf(node_stack, AST_STRING_LITERAL, current_token);
      break;
    case SYM_CHAR_LITERAL:
      push_leaf(node_stack, AST_CHAR_LITERAL, current_token);
      break;
    case SYM_INT:
    case SYM_FLOAT:
    case SYM_CHAR:
    case SYM_STRING:
      push_leaf(node_stack, AST_TYPE, current_token);
      break;
    default:
      break;
    }
    next(parser);
    current_token = parser->t;
    // print_symbol(current_item->symbol);
    return 1;
  } else {
    syntax_error("CURRENT TOKEN DOESN'T MATCH TOP");
    prt_token(current_token, 1);
    printf("%d symbol", current_item->symbol);
    return 0;
  }
}

int handle_non_terminal_top(StackItem *current_item, Token *current_token, ParseStack *stack, NodeStack *node_stack) {
  GrammarSymbol lookahead = token_to_symbol(current_token);
  Production production = ll1_lookup(current_item->symbol, lookahead);

  if (production == PROD_NONE) {
    syntax_error("Production == PROD_NONE");
    return 0;
  } else {
    if (!apply_production(stack, node_stack, production, current_token))
      return 0;
    // print_symbol(current_item->symbol);
  }
  return 1;
}

int is_terminal(GrammarSymbol sym) {
  return sym >= FIRST_TERMINAL &&
         sym <= LAST_TERMINAL;
}

void next(Parser *p) { p->t = read_next_token(p->p, 0); }

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

  return SYM_EPSILON;
}

int apply_production(ParseStack *stack, NodeStack *node_stack, Production production, Token *current_token) {
  switch (production) {
  case PROD_CODE_BLOCK_EOF:
    return parse_stack_push_many(stack, 2, SYMBOL(SYM_EOF), SYMBOL(SYM_BLOCK));

  case PROD_BLOCK_LBRACE_PROG_RBRACE:
    return parse_stack_push_many(stack, 5, ACTION(ACT_BUILD_BLOCK, NULL), SYMBOL(SYM_RBRACE), SYMBOL(SYM_PROG), ACTION(ACT_OPEN_BLOCK, NULL), SYMBOL(SYM_LBRACE));

  case PROD_PROG_DECL_PROG:
    return parse_stack_push_many(stack, 2, SYMBOL(SYM_PROG), SYMBOL(SYM_DECL));

  case PROD_PROG_EPSILON:
    return 1;

  case PROD_DECL_VAR_DECLARATION:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_VAR_DECLARATION));

  case PROD_DECL_ASSIGNMENT:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_ASSIGNMENT));

  case PROD_DECL_CONDITION:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_CONDITION));

  case PROD_DECL_WHILE_LOOP:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_WHILE_LOOP));

  case PROD_VAR_DECLARATION_TYPE_IDENTIFIER_VAR_DECLARATION_TAIL:
    return parse_stack_push_many(stack, 3, SYMBOL(SYM_VAR_DECLARATION_TAIL), SYMBOL(SYM_IDENTIFIER), SYMBOL(SYM_TYPE));

  case PROD_VAR_DECLARATION_TAIL_SEMICOLON:
    return parse_stack_push_many(stack, 2, ACTION(ACT_BUILD_VARDECL, NULL), SYMBOL(SYM_SEMICOLON));

  case PROD_VAR_DECLARATION_TAIL_ASSIGN_EXPR_SEMICOLON:
    return parse_stack_push_many(stack, 4, ACTION(ACT_BUILD_VARDECL_INIT, NULL), SYMBOL(SYM_SEMICOLON), SYMBOL(SYM_EXPR), SYMBOL(SYM_ASSIGN));

  case PROD_ASSIGNMENT_IDENTIFIER_ASSIGN_EXPR_SEMICOLON:
    return parse_stack_push_many(stack, 5, ACTION(ACT_BUILD_ASSIGN, NULL), SYMBOL(SYM_SEMICOLON), SYMBOL(SYM_EXPR), SYMBOL(SYM_ASSIGN), SYMBOL(SYM_IDENTIFIER));

  case PROD_CONDITION_IF_LPAREN_EXPR_RPAREN_BLOCK_CONDITION_TAIL:
    return parse_stack_push_many(stack, 6, SYMBOL(SYM_CONDITION_TAIL), SYMBOL(SYM_BLOCK), SYMBOL(SYM_RPAREN), SYMBOL(SYM_EXPR), SYMBOL(SYM_LPAREN), SYMBOL(SYM_IF));

  case PROD_CONDITION_TAIL_ELSE_BLOCK:
    return parse_stack_push_many(stack, 3, ACTION(ACT_BUILD_IF_ELSE, NULL), SYMBOL(SYM_BLOCK), SYMBOL(SYM_ELSE));

  case PROD_CONDITION_TAIL_EPSILON:
    return parse_stack_push_action(stack, ACT_BUILD_IF, NULL);

  case PROD_WHILE_LOOP_WHILE_LPAREN_EXPR_RPAREN_BLOCK:
    return parse_stack_push_many(stack, 6, ACTION(ACT_BUILD_WHILE, NULL), SYMBOL(SYM_BLOCK), SYMBOL(SYM_RPAREN), SYMBOL(SYM_EXPR), SYMBOL(SYM_LPAREN), SYMBOL(SYM_WHILE));

  case PROD_EXPR_LOGICAL_OR_EXPR:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_LOGICAL_OR_EXPR));

  case PROD_LOGICAL_OR_EXPR_LOGICAL_AND_EXPR_LOGICAL_OR_EXPR_TAIL:
    return parse_stack_push_many(stack, 2, SYMBOL(SYM_LOGICAL_OR_EXPR_TAIL), SYMBOL(SYM_LOGICAL_AND_EXPR));

  case PROD_LOGICAL_OR_EXPR_TAIL_OR_LOGICAL_AND_EXPR_LOGICAL_OR_EXPR_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_LOGICAL_OR_EXPR_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_LOGICAL_AND_EXPR), SYMBOL(SYM_OR));

  case PROD_LOGICAL_OR_EXPR_TAIL_EPSILON:
    return 1;

  case PROD_LOGICAL_AND_EXPR_EQUALITY_EXPR_LOGICAL_AND_EXPR_TAIL:
    return parse_stack_push_many(stack, 2, SYMBOL(SYM_LOGICAL_AND_EXPR_TAIL), SYMBOL(SYM_EQUALITY_EXPR));

  case PROD_LOGICAL_AND_EXPR_TAIL_AND_EQUALITY_EXPR_LOGICAL_AND_EXPR_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_LOGICAL_AND_EXPR_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_EQUALITY_EXPR), SYMBOL(SYM_AND));

  case PROD_LOGICAL_AND_EXPR_TAIL_EPSILON:
    return 1;

  case PROD_EQUALITY_EXPR_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
    return parse_stack_push_many(stack, 2, SYMBOL(SYM_EQUALITY_EXPR_TAIL), SYMBOL(SYM_RELATIONAL_EXPR));

  case PROD_EQUALITY_EXPR_TAIL_EQUAL_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_EQUALITY_EXPR_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_RELATIONAL_EXPR), SYMBOL(SYM_EQUAL));

  case PROD_EQUALITY_EXPR_TAIL_NOT_EQUAL_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_EQUALITY_EXPR_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_RELATIONAL_EXPR), SYMBOL(SYM_NOT_EQUAL));

  case PROD_EQUALITY_EXPR_TAIL_EPSILON:
    return 1;

  case PROD_RELATIONAL_EXPR_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return parse_stack_push_many(stack, 2, SYMBOL(SYM_RELATIONAL_EXPR_TAIL), SYMBOL(SYM_ADD_EXPR));

  case PROD_RELATIONAL_EXPR_TAIL_LESS_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_RELATIONAL_EXPR_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_ADD_EXPR), SYMBOL(SYM_LESS));

  case PROD_RELATIONAL_EXPR_TAIL_LESS_EQUAL_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_RELATIONAL_EXPR_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_ADD_EXPR), SYMBOL(SYM_LESS_EQUAL));

  case PROD_RELATIONAL_EXPR_TAIL_GREATER_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_RELATIONAL_EXPR_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_ADD_EXPR), SYMBOL(SYM_GREATER));

  case PROD_RELATIONAL_EXPR_TAIL_GREATER_EQUAL_ADD_EXPR_RELATIONAL_EXPR_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_RELATIONAL_EXPR_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_ADD_EXPR), SYMBOL(SYM_GREATER_EQUAL));

  case PROD_RELATIONAL_EXPR_TAIL_EPSILON:
    return 1;

  case PROD_ADD_EXPR_TERM_ADD_EXPR_TAIL:
    return parse_stack_push_many(stack, 2, SYMBOL(SYM_ADD_EXPR_TAIL), SYMBOL(SYM_TERM));

  case PROD_ADD_EXPR_TAIL_PLUS_TERM_ADD_EXPR_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_ADD_EXPR_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_TERM), SYMBOL(SYM_PLUS));

  case PROD_ADD_EXPR_TAIL_MINUS_TERM_ADD_EXPR_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_ADD_EXPR_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_TERM), SYMBOL(SYM_MINUS));

  case PROD_ADD_EXPR_TAIL_EPSILON:
    return 1;

  case PROD_TERM_FACTOR_TERM_TAIL:
    return parse_stack_push_many(stack, 2, SYMBOL(SYM_TERM_TAIL), SYMBOL(SYM_FACTOR));

  case PROD_TERM_TAIL_MULTIPLY_FACTOR_TERM_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_TERM_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_FACTOR), SYMBOL(SYM_MULTIPLY));

  case PROD_TERM_TAIL_DIVIDE_FACTOR_TERM_TAIL:
    return parse_stack_push_many(stack, 4, SYMBOL(SYM_TERM_TAIL), ACTION(ACT_BUILD_BINARY, current_token), SYMBOL(SYM_FACTOR), SYMBOL(SYM_DIVIDE));

  case PROD_TERM_TAIL_EPSILON:
    return 1;

  case PROD_FACTOR_IDENTIFIER:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_IDENTIFIER));

  case PROD_FACTOR_NUMBER:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_NUMBER));

  case PROD_FACTOR_STRING_LITERAL:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_STRING_LITERAL));

  case PROD_FACTOR_CHAR_LITERAL:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_CHAR_LITERAL));

  case PROD_FACTOR_LPAREN_EXPR_RPAREN:
    // Parênteses só agrupam: EXPR deixa 1 nó, '(' e ')' são silenciosos.
    return parse_stack_push_many(stack, 3, SYMBOL(SYM_RPAREN), SYMBOL(SYM_EXPR), SYMBOL(SYM_LPAREN));

  case PROD_TYPE_INT:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_INT));

  case PROD_TYPE_FLOAT:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_FLOAT));

  case PROD_TYPE_CHAR:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_CHAR));

  case PROD_TYPE_STRING:
    return parse_stack_push_many(stack, 1, SYMBOL(SYM_STRING));

  default:
    return 0;
  }
}

char *token_to_mips_instruction(Token *token) {
  switch (token_to_symbol(token)) {
  // aritméticos
  case SYM_PLUS:
    return "add";
  case SYM_MINUS:
    return "sub";
  case SYM_MULTIPLY:
    return "mul";
  case SYM_DIVIDE:
    return "div";
  // relacionais (pseudo-instruções do MARS/SPIM)
  case SYM_LESS:
    return "slt";
  case SYM_LESS_EQUAL:
    return "sle";
  case SYM_GREATER:
    return "sgt";
  case SYM_GREATER_EQUAL:
    return "sge";
  // igualdade
  case SYM_EQUAL:
    return "seq";
  case SYM_NOT_EQUAL:
    return "sne";
  // lógicos
  case SYM_AND:
    return "and";
  case SYM_OR:
    return "or";
  default:
    return NULL;
  }
}
