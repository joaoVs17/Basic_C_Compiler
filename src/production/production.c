#include "../production/production.h"

Production ll1_lookup(
    GrammarSymbol non_terminal,
    GrammarSymbol lookahead) {
  switch (non_terminal) {
  case SYM_CODE:
    if (lookahead == SYM_LBRACE)
      return PROD_CODE_BLOCK_EOF;
    break;

  case SYM_BLOCK:
    if (lookahead == SYM_LBRACE)
      return PROD_BLOCK_LBRACE_PROG_RBRACE;
    break;

  case SYM_PROG:
    switch (lookahead) {
    case SYM_INT:
    case SYM_FLOAT:
    case SYM_CHAR:
    case SYM_STRING:
    case SYM_IDENTIFIER:
    case SYM_IF:
    case SYM_WHILE:
      return PROD_PROG_DECL_PROG;

    case SYM_RBRACE:
      return PROD_PROG_EPSILON;

    default:
      break;
    }
    break;

  case SYM_DECL:
    switch (lookahead) {
    case SYM_INT:
    case SYM_FLOAT:
    case SYM_CHAR:
    case SYM_STRING:
      return PROD_DECL_VAR_DECLARATION;

    case SYM_IDENTIFIER:
      return PROD_DECL_ASSIGNMENT;

    case SYM_IF:
      return PROD_DECL_CONDITION;

    case SYM_WHILE:
      return PROD_DECL_WHILE_LOOP;

    default:
      break;
    }
    break;

  case SYM_VAR_DECLARATION:
    switch (lookahead) {
    case SYM_INT:
    case SYM_FLOAT:
    case SYM_CHAR:
    case SYM_STRING:
      return PROD_VAR_DECLARATION_TYPE_IDENTIFIER_VAR_DECLARATION_TAIL;

    default:
      break;
    }
    break;

  case SYM_VAR_DECLARATION_TAIL:
    switch (lookahead) {
    case SYM_SEMICOLON:
      return PROD_VAR_DECLARATION_TAIL_SEMICOLON;

    case SYM_ASSIGN:
      return PROD_VAR_DECLARATION_TAIL_ASSIGN_EXPR_SEMICOLON;

    default:
      break;
    }
    break;

  case SYM_ASSIGNMENT:
    if (lookahead == SYM_IDENTIFIER)
      return PROD_ASSIGNMENT_IDENTIFIER_ASSIGN_EXPR_SEMICOLON;
    break;

  case SYM_CONDITION:
    if (lookahead == SYM_IF)
      return PROD_CONDITION_IF_LPAREN_EXPR_RPAREN_BLOCK_CONDITION_TAIL;
    break;

  case SYM_CONDITION_TAIL:
    switch (lookahead) {
    case SYM_ELSE:
      return PROD_CONDITION_TAIL_ELSE_BLOCK;

    case SYM_INT:
    case SYM_FLOAT:
    case SYM_CHAR:
    case SYM_STRING:
    case SYM_IDENTIFIER:
    case SYM_IF:
    case SYM_WHILE:
    case SYM_RBRACE:
      return PROD_CONDITION_TAIL_EPSILON;

    default:
      break;
    }
    break;

  case SYM_WHILE_LOOP:
    if (lookahead == SYM_WHILE)
      return PROD_WHILE_LOOP_WHILE_LPAREN_EXPR_RPAREN_BLOCK;
    break;

  case SYM_EXPR:
    switch (lookahead) {
    case SYM_IDENTIFIER:
    case SYM_NUMBER:
    case SYM_STRING_LITERAL:
    case SYM_CHAR_LITERAL:
    case SYM_LPAREN:
      return PROD_EXPR_LOGICAL_OR_EXPR;

    default:
      break;
    }
    break;

  case SYM_LOGICAL_OR_EXPR:
    switch (lookahead) {
    case SYM_IDENTIFIER:
    case SYM_NUMBER:
    case SYM_STRING_LITERAL:
    case SYM_CHAR_LITERAL:
    case SYM_LPAREN:
      return PROD_LOGICAL_OR_EXPR_LOGICAL_AND_EXPR_LOGICAL_OR_EXPR_TAIL;

    default:
      break;
    }
    break;

  case SYM_LOGICAL_OR_EXPR_TAIL:
    switch (lookahead) {
    case SYM_OR:
      return PROD_LOGICAL_OR_EXPR_TAIL_OR_LOGICAL_AND_EXPR_LOGICAL_OR_EXPR_TAIL;

    case SYM_RPAREN:
    case SYM_SEMICOLON:
      return PROD_LOGICAL_OR_EXPR_TAIL_EPSILON;

    default:
      break;
    }
    break;

  case SYM_LOGICAL_AND_EXPR:
    switch (lookahead) {
    case SYM_IDENTIFIER:
    case SYM_NUMBER:
    case SYM_STRING_LITERAL:
    case SYM_CHAR_LITERAL:
    case SYM_LPAREN:
      return PROD_LOGICAL_AND_EXPR_EQUALITY_EXPR_LOGICAL_AND_EXPR_TAIL;

    default:
      break;
    }
    break;

  case SYM_LOGICAL_AND_EXPR_TAIL:
    switch (lookahead) {
    case SYM_AND:
      return PROD_LOGICAL_AND_EXPR_TAIL_AND_EQUALITY_EXPR_LOGICAL_AND_EXPR_TAIL;

    case SYM_OR:
    case SYM_RPAREN:
    case SYM_SEMICOLON:
      return PROD_LOGICAL_AND_EXPR_TAIL_EPSILON;

    default:
      break;
    }
    break;

  case SYM_EQUALITY_EXPR:
    switch (lookahead) {
    case SYM_IDENTIFIER:
    case SYM_NUMBER:
    case SYM_STRING_LITERAL:
    case SYM_CHAR_LITERAL:
    case SYM_LPAREN:
      return PROD_EQUALITY_EXPR_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL;

    default:
      break;
    }
    break;

  case SYM_EQUALITY_EXPR_TAIL:
    switch (lookahead) {
    case SYM_EQUAL:
      return PROD_EQUALITY_EXPR_TAIL_EQUAL_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL;

    case SYM_NOT_EQUAL:
      return PROD_EQUALITY_EXPR_TAIL_NOT_EQUAL_RELATIONAL_EXPR_EQUALITY_EXPR_TAIL;

    case SYM_AND:
    case SYM_OR:
    case SYM_RPAREN:
    case SYM_SEMICOLON:
      return PROD_EQUALITY_EXPR_TAIL_EPSILON;

    default:
      break;
    }
    break;

  case SYM_RELATIONAL_EXPR:
    switch (lookahead) {
    case SYM_IDENTIFIER:
    case SYM_NUMBER:
    case SYM_STRING_LITERAL:
    case SYM_CHAR_LITERAL:
    case SYM_LPAREN:
      return PROD_RELATIONAL_EXPR_ADD_EXPR_RELATIONAL_EXPR_TAIL;

    default:
      break;
    }
    break;

  case SYM_RELATIONAL_EXPR_TAIL:
    switch (lookahead) {
    case SYM_LESS:
      return PROD_RELATIONAL_EXPR_TAIL_LESS_ADD_EXPR_RELATIONAL_EXPR_TAIL;

    case SYM_LESS_EQUAL:
      return PROD_RELATIONAL_EXPR_TAIL_LESS_EQUAL_ADD_EXPR_RELATIONAL_EXPR_TAIL;

    case SYM_GREATER:
      return PROD_RELATIONAL_EXPR_TAIL_GREATER_ADD_EXPR_RELATIONAL_EXPR_TAIL;

    case SYM_GREATER_EQUAL:
      return PROD_RELATIONAL_EXPR_TAIL_GREATER_EQUAL_ADD_EXPR_RELATIONAL_EXPR_TAIL;

    case SYM_EQUAL:
    case SYM_NOT_EQUAL:
    case SYM_AND:
    case SYM_OR:
    case SYM_RPAREN:
    case SYM_SEMICOLON:
      return PROD_RELATIONAL_EXPR_TAIL_EPSILON;

    default:
      break;
    }
    break;

  case SYM_ADD_EXPR:
    switch (lookahead) {
    case SYM_IDENTIFIER:
    case SYM_NUMBER:
    case SYM_STRING_LITERAL:
    case SYM_CHAR_LITERAL:
    case SYM_LPAREN:
      return PROD_ADD_EXPR_TERM_ADD_EXPR_TAIL;

    default:
      break;
    }
    break;

  case SYM_ADD_EXPR_TAIL:
    switch (lookahead) {
    case SYM_PLUS:
      return PROD_ADD_EXPR_TAIL_PLUS_TERM_ADD_EXPR_TAIL;

    case SYM_MINUS:
      return PROD_ADD_EXPR_TAIL_MINUS_TERM_ADD_EXPR_TAIL;

    case SYM_LESS:
    case SYM_LESS_EQUAL:
    case SYM_GREATER:
    case SYM_GREATER_EQUAL:
    case SYM_EQUAL:
    case SYM_NOT_EQUAL:
    case SYM_AND:
    case SYM_OR:
    case SYM_RPAREN:
    case SYM_SEMICOLON:
      return PROD_ADD_EXPR_TAIL_EPSILON;

    default:
      break;
    }
    break;

  case SYM_TERM:
    switch (lookahead) {
    case SYM_IDENTIFIER:
    case SYM_NUMBER:
    case SYM_STRING_LITERAL:
    case SYM_CHAR_LITERAL:
    case SYM_LPAREN:
      return PROD_TERM_FACTOR_TERM_TAIL;

    default:
      break;
    }
    break;

  case SYM_TERM_TAIL:
    switch (lookahead) {
    case SYM_MULTIPLY:
      return PROD_TERM_TAIL_MULTIPLY_FACTOR_TERM_TAIL;

    case SYM_DIVIDE:
      return PROD_TERM_TAIL_DIVIDE_FACTOR_TERM_TAIL;

    case SYM_PLUS:
    case SYM_MINUS:
    case SYM_LESS:
    case SYM_LESS_EQUAL:
    case SYM_GREATER:
    case SYM_GREATER_EQUAL:
    case SYM_EQUAL:
    case SYM_NOT_EQUAL:
    case SYM_AND:
    case SYM_OR:
    case SYM_RPAREN:
    case SYM_SEMICOLON:
      return PROD_TERM_TAIL_EPSILON;

    default:
      break;
    }
    break;

  case SYM_FACTOR:
    switch (lookahead) {
    case SYM_IDENTIFIER:
      return PROD_FACTOR_IDENTIFIER;

    case SYM_NUMBER:
      return PROD_FACTOR_NUMBER;

    case SYM_STRING_LITERAL:
      return PROD_FACTOR_STRING_LITERAL;

    case SYM_CHAR_LITERAL:
      return PROD_FACTOR_CHAR_LITERAL;

    case SYM_LPAREN:
      return PROD_FACTOR_LPAREN_EXPR_RPAREN;

    default:
      break;
    }
    break;

  case SYM_TYPE:
    switch (lookahead) {
    case SYM_INT:
      return PROD_TYPE_INT;

    case SYM_FLOAT:
      return PROD_TYPE_FLOAT;

    case SYM_CHAR:
      return PROD_TYPE_CHAR;

    case SYM_STRING:
      return PROD_TYPE_STRING;

    default:
      break;
    }
    break;

  default:
    break;
  }

  return PROD_NONE;
}