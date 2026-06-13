#include "stack.h"

#include <stdio.h>

void parse_stack_init(ParseStack *stack) {
  if (!stack) {
    return;
  }

  stack->count = 0;
}

void parse_stack_clear(ParseStack *stack) {
  parse_stack_init(stack);
}

int parse_stack_push(ParseStack *stack, GrammarSymbol symbol) {
  if (!stack || parse_stack_is_full(stack)) {
    return 0;
  }

  stack->items[stack->count] = symbol;
  stack->count++;
  return 1;
}

int parse_stack_pop(ParseStack *stack, GrammarSymbol *out_symbol) {
  if (!stack || !out_symbol || parse_stack_is_empty(stack)) {
    return 0;
  }

  stack->count--;
  *out_symbol = stack->items[stack->count];
  return 1;
}

int parse_stack_peek(const ParseStack *stack, GrammarSymbol *out_symbol) {
  if (!stack || !out_symbol || parse_stack_is_empty(stack)) {
    return 0;
  }

  *out_symbol = stack->items[stack->count - 1];
  return 1;
}

int parse_stack_is_empty(const ParseStack *stack) {
  return !stack || stack->count == 0;
}

int parse_stack_is_full(const ParseStack *stack) {
  return stack && stack->count >= PARSE_STACK_MAX;
}

int parse_stack_size(const ParseStack *stack) {
  if (!stack) {
    return 0;
  }

  return stack->count;
}

void parse_stack_print(const ParseStack *stack) {
  printf("[\n");
  for (int i = 0; i < stack->count; i++) {
    print_symbol(stack->items[i]);
  }
  printf("]\n");
}

//Print fn
char *grammar_symbol_to_string(GrammarSymbol sym) {
  switch (sym) {
  case SYM_CODE: return "SYM_CODE";
  case SYM_BLOCK: return "SYM_BLOCK";
  case SYM_PROG: return "SYM_PROG";
  case SYM_DECL: return "SYM_DECL";

  case SYM_VAR_DECLARATION: return "SYM_VAR_DECLARATION";
  case SYM_VAR_DECLARATION_TAIL: return "SYM_VAR_DECLARATION_TAIL";

  case SYM_ASSIGNMENT: return "SYM_ASSIGNMENT";

  case SYM_CONDITION: return "SYM_CONDITION";
  case SYM_CONDITION_TAIL: return "SYM_CONDITION_TAIL";

  case SYM_WHILE_LOOP: return "SYM_WHILE_LOOP";

  case SYM_EXPR: return "SYM_EXPR";

  case SYM_LOGICAL_OR_EXPR: return "SYM_LOGICAL_OR_EXPR";
  case SYM_LOGICAL_OR_EXPR_TAIL: return "SYM_LOGICAL_OR_EXPR_TAIL";

  case SYM_LOGICAL_AND_EXPR: return "SYM_LOGICAL_AND_EXPR";
  case SYM_LOGICAL_AND_EXPR_TAIL: return "SYM_LOGICAL_AND_EXPR_TAIL";

  case SYM_EQUALITY_EXPR: return "SYM_EQUALITY_EXPR";
  case SYM_EQUALITY_EXPR_TAIL: return "SYM_EQUALITY_EXPR_TAIL";

  case SYM_RELATIONAL_EXPR: return "SYM_RELATIONAL_EXPR";
  case SYM_RELATIONAL_EXPR_TAIL: return "SYM_RELATIONAL_EXPR_TAIL";

  case SYM_ADD_EXPR: return "SYM_ADD_EXPR";
  case SYM_ADD_EXPR_TAIL: return "SYM_ADD_EXPR_TAIL";

  case SYM_TERM: return "SYM_TERM";
  case SYM_TERM_TAIL: return "SYM_TERM_TAIL";

  case SYM_FACTOR: return "SYM_FACTOR";

  case SYM_TYPE: return "SYM_TYPE";

  case SYM_IDENTIFIER: return "SYM_IDENTIFIER";
  case SYM_NUMBER: return "SYM_NUMBER";
  case SYM_STRING_LITERAL: return "SYM_STRING_LITERAL";
  case SYM_CHAR_LITERAL: return "SYM_CHAR_LITERAL";

  case SYM_INT: return "SYM_INT";
  case SYM_FLOAT: return "SYM_FLOAT";
  case SYM_CHAR: return "SYM_CHAR";
  case SYM_STRING: return "SYM_STRING";

  case SYM_IF: return "SYM_IF";
  case SYM_ELSE: return "SYM_ELSE";
  case SYM_WHILE: return "SYM_WHILE";

  case SYM_ASSIGN: return "SYM_ASSIGN";
  case SYM_EQUAL: return "SYM_EQUAL";
  case SYM_NOT_EQUAL: return "SYM_NOT_EQUAL";

  case SYM_LESS: return "SYM_LESS";
  case SYM_LESS_EQUAL: return "SYM_LESS_EQUAL";
  case SYM_GREATER: return "SYM_GREATER";
  case SYM_GREATER_EQUAL: return "SYM_GREATER_EQUAL";

  case SYM_AND: return "SYM_AND";
  case SYM_OR: return "SYM_OR";

  case SYM_PLUS: return "SYM_PLUS";
  case SYM_MINUS: return "SYM_MINUS";
  case SYM_MULTIPLY: return "SYM_MULTIPLY";
  case SYM_DIVIDE: return "SYM_DIVIDE";

  case SYM_LPAREN: return "SYM_LPAREN";
  case SYM_RPAREN: return "SYM_RPAREN";

  case SYM_LBRACE: return "SYM_LBRACE";
  case SYM_RBRACE: return "SYM_RBRACE";

  case SYM_SEMICOLON: return "SYM_SEMICOLON";

  case SYM_EOF: return "SYM_EOF";

  case SYM_EPSILON: return "SYM_EPSILON";

  default: return "UNKNOWN_SYMBOL";
  }
}

void print_symbol(GrammarSymbol sym) {
  printf("%s\n", grammar_symbol_to_string(sym));
}